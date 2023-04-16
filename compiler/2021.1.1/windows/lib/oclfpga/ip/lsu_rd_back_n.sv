// (c) 1992-2020 Intel Corporation.                            
// Intel, the Intel logo, Intel, MegaCore, NIOS II, Quartus and TalkBack words    
// and logos are trademarks of Intel Corporation or its subsidiaries in the U.S.  
// and/or other countries. Other marks and brands may be claimed as the property  
// of others. See Trademarks on intel.com for full list of Intel trademarks or    
// the Trademarks & Brands Names Database (if Intel) or See www.Intel.com/legal (if Altera) 
// Your use of Intel Corporation's design tools, logic functions and other        
// software and tools, and its AMPP partner logic functions, and any output       
// files any of the foregoing (including device programming or simulation         
// files), and any associated documentation or information are expressly subject  
// to the terms and conditions of the Altera Program License Subscription         
// Agreement, Intel MegaCore Function License Agreement, or other applicable      
// license agreement, including, without limitation, that your use is for the     
// sole purpose of programming logic devices manufactured by Intel and sold by    
// Intel or its authorized distributors.  Please refer to the applicable          
// agreement for further details.                                                 


/* 
  lsu_rd_back_n is a "router" + "reordering" module whose function is to return responses, in
  command-order, to multiple masters, in a multiple-slave system. It's typically used in memory 
  systems (masters being LSUs, slaves being memory banks, read responses being read data, and write
  responses being write acks).

  Note: this module was originally written for read responses so you may encounter language and naming specific to
  reads, but it can be used for write responses or responses of any kind.

  What is "routing"?
  In this context, a slave is assumed to generate responses in the same order as it received commands.
  But a given slave may be targeted by multiple masters. Therefore the slave's responses need to be
  "routed" to the master that generated the corresponding command.

  What is "reordering"?
  Typically, slaves in a multi-slave system may generate responses that are not synchronized to
  other slaves. A simple example is if M0 (Master-0) issues a command to S0 then S1 (sequentially in time),
  it's possible that S1 generates its response before S0, despite S1 receiving its command after
  S0. But M0 must receive its responses in command-order, therefore the received responses must be "reordered"
  into the command-order.

  Given the above functional description, this module's interface is comprised of:
    1. Command input
    2. Response input
    3. Response output

  #1 is used to track the commands as they're issued by the multiple masters.
  #2 is used to receive the responses from the multiple-slaves
  #3 is used to route the responses received on #2 to the masters indicated on #1, in the correct order.

  On #1, this module supports the concept of serial vs parallel command inputs.
  
  "Serial" commands means that the commands, from all masters, are serialized onto one command bus.
  In lsu_token_ring this is called "single" ring. The read command ring operates this way and the write
  command ring can optionally operate this way.
  
  "Parallel" commands simply means that no serialization point exists. Multiple command busses exist and 
  multiple masters can be issuing commands in parallel. In lsu_token_ring, this is called "multiple write ring"
  and is possible for write commands.

  When parallel commands are used, the number of input command busses, the number of masters, and the number of 
  L1 reordering units (described below) must all be equal. This is simply because, by definition of "parallel"
  in this context, masters can all generate commands on the same cycle, and we need to track all those commands,
  therefore we require one input command bus per master. This lets us track, on a per-master basis, the slave-order
  of the commands. But we also need to track, on a per-slave basis, the master-order of the commands. Therefore
  we need an additional command input, per-slave. By tracking these two things (slave-order of the commands
  issued by a given master and master-order of commands received by a given slave) we have all the information 
  needed to perform reordering.
  
  When serial commands are used, there is only one input command bus and the number of L1 reordering units
  is parameterizable. Therefore less area is needed when using serial commands. All of the information needed
  for reordering is available on this one, serialized, input command bus.
  Note, the theoretical throughput
  of the response output (#3) will meet or exceed the aggregrate throughput of all slaves only if the number
  of L1 reordering units is >= # slaves. For example, in a 4-bank memory system, if it's desired to deliver
  4 bank's worth of read bandwidth to the kernel, we need at least 4 L1 reordering units. If we use less than 4, the
  system will still be functional and it will consume less area, but the total read bandwidth delivered to the kernel
  will be lower. So there is an area vs. throughput tradeoff that's possible here. Typically it doesn't make sense
  to have more L1 units than # banks.

  Typically, in lsu_token_ring, serial commands are used for reads and parallel commands are used for writes.
*/

`default_nettype none
module lsu_rd_back_n (
  clk,
  resetn,
  // Commands from masters (LSUs)
  i_to_avm_port_num,
  i_to_avm_burstcount,
  i_to_avm_id,
  i_to_avm_valid,  

  // Commands received at each slave, used when ENABLE_PARALLEL_COMMANDS = 1
  i_slave_avm_burstcount,
  i_slave_master_id,
  i_slave_valid,    

  // Mapping of masters to L1 reordering units
  i_reorder_id_per_load,

  // Responses from slaves
  i_data,
  i_data_valid,

  // Response output to masters
  o_data,
  o_data_valid,

  // Backpressure
  o_id_af,

  // Asserts when a word is read out of each return data FIFO. Used for throttling at the root.
  o_rd_bank,
  // ecc status signals
  ecc_err_status
);

parameter NUM_DIMM = 1;
parameter NUM_RD_PORT = 1;
parameter ID_WIDTH = $clog2(NUM_RD_PORT);  
parameter BURST_CNT_W = 1;
parameter MWIDTH = 512;
parameter MAX_MEM_DELAY = 64; 
parameter DATA_FIFO_DEPTH = 64; 
parameter PIPELINE = 0;
parameter NUM_REORDER = 2;
parameter HYPER_PIPELINE = 0;  // 1 = optimized, highly pipelined, at the expense of area
/*  Specifies the amount of additional latency that exists on the o_id_af signal before it actually backpressures the read-ring. 
    There might be pipelining on this backpressure, so this latency needs to be accounted for when setting the almost-full threshold 
    of avm_read_req_fifo.
*/
parameter AVM_READ_DATA_LATENESS = 0;  // fmax and area optimization - o_data is late by this many clocks compared to o_data_valid
parameter WIDE_DATA_SLICING = 0;       // for large MWIDTH_BYTES, a nonzero value indicate how wide to width-slice hld_fifo, also mux select signals are replicated based on width needed
parameter ALLOW_HIGH_SPEED_FIFO_USAGE = 1;  // choice of hld_fifo style, 0 = mid speed fifo, 1 = high speed fifo
parameter ID_AF_EXTERNAL_LATENCY = 0;
parameter ENABLE_PARALLEL_COMMANDS = 0; // 1 = enables multiple command inputs. 0 = single command input. See comments at top of this file for more information.
parameter enable_ecc = "FALSE";    // Enable error correction coding

localparam NUM_DIMM_W = $clog2(NUM_DIMM);       
localparam MAX_BURST = 2**(BURST_CNT_W-1);
localparam P_ID_WIDTH = (ID_WIDTH == 0)? 1 : ID_WIDTH;
localparam P_NUM_DIMM_W = (NUM_DIMM_W == 0)? 1 :NUM_DIMM_W;
localparam P_NUM_REORDER_WIDTH = (NUM_REORDER == 0)? 1 : $clog2(NUM_REORDER);

// Constants to define the number of inputs and L1 units used when we have parallel vs. serial commands.
localparam NUM_MASTER_COMMAND_INPUTS = ENABLE_PARALLEL_COMMANDS? NUM_RD_PORT : 1; 
localparam NUM_L1_REORDER_UNITS = ENABLE_PARALLEL_COMMANDS? NUM_MASTER_COMMAND_INPUTS : NUM_REORDER;
localparam GENERATE_SINGLE_REORDER = (NUM_REORDER == 1) && !ENABLE_PARALLEL_COMMANDS;

input wire clk;
input wire resetn;  // reset is synchronous if HYPER_PIPELINE == 1, asynchronous otherwise

// 1. Command inputs
// From masters
input wire [P_NUM_DIMM_W-1:0] i_to_avm_port_num   [NUM_MASTER_COMMAND_INPUTS];  // Bank number
input wire [BURST_CNT_W-1:0]  i_to_avm_burstcount [NUM_MASTER_COMMAND_INPUTS];
input wire [P_ID_WIDTH-1:0]   i_to_avm_id         [NUM_MASTER_COMMAND_INPUTS];  // LSU ID, used when NUM_MASTER_COMMAND_INPUTS == 1. Otherwise the index simply indicates the master ID.
input wire                    i_to_avm_valid      [NUM_MASTER_COMMAND_INPUTS];

// From slaves (not used if ENABLE_PARALLEL_COMMANDS = 0). Serialized stream of commands received at the slave inputs. Allows us to track the master-order of commands received by each slave.
input wire [BURST_CNT_W-1:0]  i_slave_avm_burstcount [NUM_DIMM];  
input wire [P_ID_WIDTH-1:0]   i_slave_master_id      [NUM_DIMM];  // LSU ID
input wire                    i_slave_valid          [NUM_DIMM];

// The assignment of each LSU to a specific L1 reorder unit. This assignment comes from outside. It comes in as a signal to make it easy to use for control logic but it's typically a static parameterization 
// (i.e. this input signal is usually a constant at compile time). Typically the compiler should assign high bandwidth LSUs to separate reorder units so they can receive their data in parallel.
// Used only if ENABLE_PARALLEL_COMMANDS = 0.
input wire [NUM_RD_PORT-1:0][P_NUM_REORDER_WIDTH-1:0] i_reorder_id_per_load; 

// 2. Response inputs
input wire [MWIDTH-1:0] i_data [NUM_DIMM];
input wire i_data_valid [NUM_DIMM];

// 3. Response Outputs
output logic [MWIDTH-1:0] o_data [NUM_RD_PORT] /* synthesis dont_merge */;   // Important when PIPELINE=0 (ie. data is replicated/fanned out to the LSUs)
output logic o_data_valid  [NUM_RD_PORT];

// Data FIFO occupancy signals, used for read-request throttling
output logic [NUM_L1_REORDER_UNITS-1:0][NUM_DIMM-1:0] o_rd_bank;
output logic o_id_af;

output logic  [1:0] ecc_err_status;  // ecc status signals

localparam  ASYNC_RESET = HYPER_PIPELINE? 0 : 1; // Use synchronous resets in hyper-pipeline mode.
localparam  NUM_RESET_COPIES = 1;
localparam  RESET_PIPE_DEPTH = 5;  
logic                         aclrn;
logic [NUM_RESET_COPIES-1:0]  sclrn;
logic                         resetn_synchronized;

acl_reset_handler
#(
    .ASYNC_RESET            (ASYNC_RESET),
    .USE_SYNCHRONIZER       (0),  // Reset is synchronized at a higher level.
    .SYNCHRONIZE_ACLRN      (0),
    .PIPE_DEPTH             (RESET_PIPE_DEPTH),
    .NUM_COPIES             (NUM_RESET_COPIES) 
)
acl_reset_handler_inst
(
    .clk                    (clk),
    .i_resetn               (resetn),
    .o_aclrn                (aclrn),
    .o_resetn_synchronized  (resetn_synchronized),  
    .o_sclrn                (sclrn)
);

genvar z, z0;
generate
  if(GENERATE_SINGLE_REORDER) begin : GEN_SINGLE_REORDER
    wire [NUM_DIMM-1:0] rd_bank;

    for(z=0; z<NUM_DIMM; z=z+1) begin : GEN_RD
      //o_rd_bank [NUM_REORDER][NUM_DIMM];
      assign o_rd_bank[0][z] = rd_bank[z];
    end
    lsu_rd_back #(
      .NUM_DIMM (NUM_DIMM),
      .NUM_RD_PORT (NUM_RD_PORT),
      .BURST_CNT_W (BURST_CNT_W),
      .ID_WIDTH(ID_WIDTH),
      .MWIDTH (MWIDTH),    
      .DATA_FIFO_DEPTH(DATA_FIFO_DEPTH),
      .MAX_MEM_DELAY (MAX_MEM_DELAY),
      .PIPELINE (PIPELINE),
      .HYPER_PIPELINE(HYPER_PIPELINE),
      .AVM_READ_DATA_LATENESS(AVM_READ_DATA_LATENESS),
      .WIDE_DATA_SLICING(WIDE_DATA_SLICING),
      .ALLOW_HIGH_SPEED_FIFO_USAGE(ALLOW_HIGH_SPEED_FIFO_USAGE),
      .ID_AF_EXTERNAL_LATENCY(ID_AF_EXTERNAL_LATENCY), //This block does not add any additional latency to o_id_af
      .enable_ecc(enable_ecc)
    ) lsu_rd_back (
      .clk                (clk),
      .resetn             (resetn_synchronized),
      .i_to_avm_port_num  (i_to_avm_port_num[0]),
      .i_to_avm_burstcount(i_to_avm_burstcount[0]),
      .i_to_avm_id        (i_to_avm_id[0]),
      .i_to_avm_valid     (i_to_avm_valid[0]),   
      .i_data             (i_data),
      .i_data_valid       (i_data_valid),
      .o_data             (o_data),
      .o_data_valid       (o_data_valid),
      .o_id_af            (o_id_af),
      .o_rd_bank          (rd_bank) ,
      .ecc_err_status     (ecc_err_status)
    );       
  end
  else begin : GEN_N_REORDER

    /* With NUM_L1_REORDER_UNITS reordering units (lsu_rd_back) the LSUs are split roughly evenly among them. The responses have to be routed to the unit 
       that's servicing the LSU to which that data is destined. To accomplish this we use 2 levels of lsu_rd_back instances.
       The lsu_rd_back module can function as both a router and optional re-reording unit.
       
       The L2-lsu-rd-back functions as a pure router (no-ordering) and routes the response to the appropriate downstream L1-lsu-rd-back. The 
       L1-lsu-rd-back then reorders the response from the NUM_DIMM banks and feeds it to the appropriate LSU.

       L2 = "level-2", meaning the higher-level lsu_rd_back. There are NUM_DIMM modules each receiving responses from exactly one bank and routing that data downstream to L1.
       L1 = "level-1". There are NUM_REORDER modules, each receiving responses from all banks, reordering, and deliver to its corresponding LSUs.

       Another way of saying it: the L2 units track the master-order of commands received at each bank (slave), and the L1 units track the slave-order of commands
       generated by each master.
    */

    logic [NUM_DIMM-1:0]              L2_backpressure;
    logic [NUM_L1_REORDER_UNITS-1:0]  L1_backpressure; 
    logic [MWIDTH-1:0]                group_o_data                  [NUM_L1_REORDER_UNITS][NUM_RD_PORT];
    logic                             group_o_data_en               [NUM_L1_REORDER_UNITS][NUM_RD_PORT];    
    integer i, j;

    logic [MWIDTH-1:0]                l2_lsu_rd_back_o_data         [NUM_DIMM][NUM_L1_REORDER_UNITS];
    logic                             l2_lsu_rd_back_o_data_valid   [NUM_DIMM][NUM_L1_REORDER_UNITS];  
              
    logic [MWIDTH-1:0]                l1_lsu_rd_back_i_data         [NUM_L1_REORDER_UNITS][NUM_DIMM];
    logic                             l1_lsu_rd_back_i_data_valid   [NUM_L1_REORDER_UNITS][NUM_DIMM];

    logic [1:0] ecc_err_status_0;
    logic [NUM_L1_REORDER_UNITS-1:0] ecc_err_status_0_for_0;
    logic [NUM_L1_REORDER_UNITS-1:0] ecc_err_status_0_for_1;
    assign ecc_err_status_0[0] = |ecc_err_status_0_for_0;
    assign ecc_err_status_0[1] = |ecc_err_status_0_for_1;

    for(z=0; z<NUM_L1_REORDER_UNITS; z=z+1) begin : GEN_DATA_REORDER_L1     
      // The L2 output data is arranged by [bank][group] whereas the L1-lsu-rd-back needs it arranged by [group][bank], so here
      // we do a simple re-arrangement.
      for(z0=0; z0<NUM_DIMM; z0=z0+1) begin : GEN_L2_OUTPUT_REARRANGE
        assign l1_lsu_rd_back_i_data[z][z0]       = l2_lsu_rd_back_o_data[z0][z];
        assign l1_lsu_rd_back_i_data_valid[z][z0] = l2_lsu_rd_back_o_data_valid[z0][z];
      end      
      
      logic [P_NUM_DIMM_W-1:0]  L1_i_to_avm_port_num_reg;
      logic [BURST_CNT_W-1:0]   L1_i_to_avm_burstcount_reg;
      logic [P_ID_WIDTH-1:0]    L1_i_to_avm_id_reg;
      logic                     L1_i_to_avm_valid_reg;

      /*
        If ENABLE_PARALLEL_COMMANDS == 1, then simply connect each master command input to its respective L1 instance.
        If ENABLE_PARALLEL_COMMANDS == 0, then use i_reorder_id_per_load to map the single (serialized) master command input to one
          of the L1 instances.
      */
      if (ENABLE_PARALLEL_COMMANDS) begin
        always @(posedge clk) begin // Parallel inputs, connect each master directly to an L1 unit
          L1_i_to_avm_port_num_reg    <= i_to_avm_port_num[z];
          L1_i_to_avm_burstcount_reg  <= i_to_avm_burstcount[z];
          L1_i_to_avm_id_reg          <= i_to_avm_id[z];
          L1_i_to_avm_valid_reg       <= i_to_avm_valid[z];
        end
      end else begin // Serialized inputs, connect each master to the L1 unit specified by the i_reorder_id_per_load mapping.
        always @(posedge clk) begin 
          L1_i_to_avm_port_num_reg    <= i_to_avm_port_num[0];
          L1_i_to_avm_burstcount_reg  <= i_to_avm_burstcount[0];
          L1_i_to_avm_id_reg          <= i_to_avm_id[0];
          // Check if the LSU that generated this command should be assigned to this (z'th) L1 unit.
          L1_i_to_avm_valid_reg       <= (i_reorder_id_per_load[i_to_avm_id[0]] == z) & i_to_avm_valid[0]; 
        end
      end

      lsu_rd_back #(
        .NUM_DIMM (NUM_DIMM),
        .NUM_RD_PORT (NUM_RD_PORT), // Each L1 instance may not service all NUM_RD_PORT masters, but use the full number here for convenience. It doesn't consume much (or any) extra area to do so.
        .BURST_CNT_W (BURST_CNT_W),
        .ID_WIDTH(ID_WIDTH),
        .MWIDTH (MWIDTH),
        .DATA_FIFO_DEPTH(DATA_FIFO_DEPTH),
        .MAX_MEM_DELAY (MAX_MEM_DELAY),
        .PIPELINE (PIPELINE),
        .ID_AF_EXTERNAL_LATENCY(ID_AF_EXTERNAL_LATENCY), //This block does not add any additional latency to o_id_af        
        .HYPER_PIPELINE(HYPER_PIPELINE),
        .AVM_READ_DATA_LATENESS(AVM_READ_DATA_LATENESS),
        .WIDE_DATA_SLICING(WIDE_DATA_SLICING),
        .ALLOW_HIGH_SPEED_FIFO_USAGE(ALLOW_HIGH_SPEED_FIFO_USAGE),
        .enable_ecc(enable_ecc)
      ) L1_lsu_rd_back (
        .clk                (clk),
        .resetn             (resetn_synchronized),
        // Commands from the masters
        .i_to_avm_port_num  (L1_i_to_avm_port_num_reg),  // Bank number
        .i_to_avm_burstcount(L1_i_to_avm_burstcount_reg),
        .i_to_avm_id        (L1_i_to_avm_id_reg),        // LSU ID
        .i_to_avm_valid     (L1_i_to_avm_valid_reg),
        // Responses from the slaves
        .i_data             (l1_lsu_rd_back_i_data[z]), 
        .i_data_valid       (l1_lsu_rd_back_i_data_valid[z]),
        // Reordered responses to the masters
        .o_data             (group_o_data[z]),
        .o_data_valid       (group_o_data_en[z]),

        .o_id_af            (L1_backpressure[z]),
        .o_rd_bank          (o_rd_bank[z]) ,
        .ecc_err_status ({ecc_err_status_0_for_1[z], ecc_err_status_0_for_0[z]})
      );       
    end // end GEN_N_REORDER_L1

    // Assign the output data. The group_o_data grabs the data from all the reorder units. i_reorder_id_per_load is used to determine which reorder unit has the data for the given LSU.
    for (z=0; z < NUM_RD_PORT; z++) begin : GEN_DATA_OUTPUT
      if (ENABLE_PARALLEL_COMMANDS) begin
        // With parallel commands, there is one L1 unit per master, so just assign the outputs directly
        assign o_data[z]        = group_o_data[z][z];
        assign o_data_valid[z]  = group_o_data_en[z][z];
      end else begin
        // With serialized commands, the masters are mapped to L1 units using i_reorder_id_per_load.
        assign o_data[z]        = group_o_data      [i_reorder_id_per_load[z]][z];
        assign o_data_valid[z]  = group_o_data_en   [i_reorder_id_per_load[z]][z];
      end
    end

    /*
      Allow either L1 or L2 to backpressure the read-ring.
      1. The backpressure will tend to come from L1 if a given group of masters is generating lots of read requests.
      2. The backpressure will tend to come from L2 if a given bank is targeted with lots of read requests.
      
      It may seem aggressive to simply OR all of these backpressure sources together to create one signal to
      backpressure the entire read-ring. And it may appear possible that global mem bandwidth can be under-utilized (ie.
      if we backpressure the entire ring just because one group or one bank backpressures, what if other groups
      or other banks are available to transact -- won't they be sitting idle, thereby reducing bandwidth utilization?)

      Conceptually, in order to maintain full global mem bandwidth utilization with only reads, the # of outstanding reads needs
      to cover the round trip latency to global memory. This is the latency from a request being accepted from the read-LSU
      into the ring, to the corresponding readdata arriving at the LSU. This latency of course depends on the latency
      through the ring (which depends on the # of LSUs) and the actual round-trip to the EMIF. But to a pessimistic first-order, 
      the former is around 20 kernel clock cycles and the EMIF round trip (for DDR) could be around 200 kernel clock cycles 
      (@ 400 MHz, with refreshes etc). So we could say we need to have outstanding requests for ~220 words per bank, in order
      to maintain full bandwidth utilization. (This is not meant to be a cycle-accurate explanation, but just a high-level
      approximation of why it's okay to OR all the backpressure signals.)

      The avm_read_request FIFO inside lsu_rd_back that ultimately generates backpressure is
      nominally sized to be 512 (MAX_MEM_DELAY). So it will fill with ~512 *requests* before backpressuring.

      In Case #1, this means that such a group will have ~512 requests outstanding before backpressuring. If the system
      is using bank-interleaving (as opposed to SW-DIMM), this means these 512 requests should be roughly evenly spread across
      all the banks. For example, in a 4-bank system this would be 128 requests per bank, and if each is a full burst of 16, that would be
      128 x 16 = 2K outstanding words requested, *per bank*. In fact, in order to *not* have enough outstanding requests for full bandwidth
      utilization, the system would need about 512 * 16 / 220 = ~37 banks of memory.

      If the system is instead using SW-DIMM, we simply will not be in this code path since reordering is disabled. In this mode,
      we generate separate backpressure signals per bank so that if one bank stalls other banks can still be accessed by other LSUs.

      Case #2, should generally be an unlikely scenario since, again, with bank-interleaving the requests should spread
      evenly across banks, and in SW-DIMM we are not in this code path.
    */
    assign o_id_af = |{L1_backpressure, L2_backpressure};

    logic [1:0] ecc_err_status_1;
    logic [NUM_DIMM-1:0] ecc_err_status_1_for_0;
    logic [NUM_DIMM-1:0] ecc_err_status_1_for_1;
    assign ecc_err_status_1[0] = |ecc_err_status_1_for_0;
    assign ecc_err_status_1[1] = |ecc_err_status_1_for_1;
    for(z=0; z<NUM_DIMM; z=z+1) begin : GEN_L2_ROUTER
 
      // One per bank. Set num_dimm=1, which turns lsu_rd_back into just a router (ie. no re-ordering function).
      // The # of outputs is NUM_L1_REORDER_UNITS, meaning that a word from the given bank can be routed to any of the NUM_L1_REORDER_UNITS downstream blocks
      
      logic [MWIDTH-1:0]  l2_lsu_rd_back_i_data [1];
      logic               l2_lsu_rd_back_i_data_valid [1];

      assign l2_lsu_rd_back_i_data[0]       = i_data[z];
      assign l2_lsu_rd_back_i_data_valid[0] = i_data_valid[z];

      logic [BURST_CNT_W-1:0]   L2_i_to_avm_burstcount_reg;
      logic [P_ID_WIDTH-1:0]    L2_i_to_avm_id_reg;
      logic                     L2_i_to_avm_valid_reg;
      
      /*
        Recall that the L2 units track the master-order of the commands received by each slave.
        If ENABLE_PARALLEL_COMMANDS = 1, then use the i_slave* inputs. Each input represents a serialized stream of commands (per slave)
        If ENABLE_PARALLEL_COMMANDS = 0, then use the i_to_avm* inputs since all commands across the system are serialized here.

        Update lsu_token_ring WRITE_COMMAND_LATENCY_THROUGH_WRITE_ACK_REORDERER if the number of input stages changes here.
      */
      if (ENABLE_PARALLEL_COMMANDS) begin
        always @(posedge clk) begin 
          L2_i_to_avm_burstcount_reg  <= i_slave_avm_burstcount[z];
          L2_i_to_avm_id_reg          <= i_slave_master_id[z];
          L2_i_to_avm_valid_reg       <= i_slave_valid[z];
        end
      end else begin
        always @(posedge clk) begin 
          L2_i_to_avm_burstcount_reg  <= i_to_avm_burstcount[0];
          // Indicates the L1 router to which we must route the response for the current command. This is indicated by the map.
          L2_i_to_avm_id_reg          <= i_reorder_id_per_load[i_to_avm_id[0]];
          // Check if the current command is targeting slave-z
          L2_i_to_avm_valid_reg       <= i_to_avm_valid[0] & (i_to_avm_port_num[0] == z);
        end
      end

      lsu_rd_back #(
        .NUM_DIMM (1),
        .NUM_RD_PORT (NUM_L1_REORDER_UNITS),
        .BURST_CNT_W (BURST_CNT_W),
        .MWIDTH (MWIDTH),
         /* This block does not add any additional latency to o_id_af. However, lsu_rd_back uses the NUM_RD_PORT parameter to determine
            when to assert backpressure, since it must account for the round-trip latency of the stall signal through the ring. It calculates
            the length of the ring using NUM_RD_PORT, and doubles this number to calculate the round-trip latency. But it also adds 
            ID_AF_EXTERNAL_LATENCY into the calculation to account for any pipelining the parent includes on the backpressure signal (before
            and after the ring).
            But in this case, we set NUM_RD_PORT = NUM_L1_REORDER_UNITS which may not reflect the true # of LSUs. So we compensate by 
            adjusting ID_AF_EXTERNAL LATENCY to account for the difference.
         */
        .ID_AF_EXTERNAL_LATENCY(ID_AF_EXTERNAL_LATENCY + 2*(NUM_RD_PORT-NUM_L1_REORDER_UNITS)),
        .MAX_MEM_DELAY (MAX_MEM_DELAY),
        .PIPELINE (PIPELINE),
        .HYPER_PIPELINE(HYPER_PIPELINE),
        .AVM_READ_DATA_LATENESS(0), //o_data_valid from this is consumed by another lsu_rd_back instance, don't double the delay of o_data compared to o_data_valid
        .WIDE_DATA_SLICING(WIDE_DATA_SLICING),
        .ALLOW_HIGH_SPEED_FIFO_USAGE(ALLOW_HIGH_SPEED_FIFO_USAGE),
        .enable_ecc(enable_ecc)
      ) L2_lsu_rd_back (
        .clk                (clk),
        .resetn             (resetn_synchronized),
        .i_to_avm_port_num  (1'b0),                   // This is the bank# of request. Set to 0 since NUM_DIMM==1
        
        .i_to_avm_burstcount(L2_i_to_avm_burstcount_reg),
        .i_to_avm_id        (L2_i_to_avm_id_reg),
        .i_to_avm_valid     (L2_i_to_avm_valid_reg),

        .i_data             (l2_lsu_rd_back_i_data),
        .i_data_valid       (l2_lsu_rd_back_i_data_valid),            // Take in the data from bank-z

        .o_data             (l2_lsu_rd_back_o_data[z]),               // Fan out the data to the NUM_L1_REORDER_UNITS downstream blocks
        .o_data_valid       (l2_lsu_rd_back_o_data_valid[z]),         // Assert valid only to the correct downstream block
        .o_id_af            (L2_backpressure[z]),
        .o_rd_bank          (),                                       // Only used when doing reordering.
        .ecc_err_status     ({ecc_err_status_1_for_1[z], ecc_err_status_1_for_0[z]})
      );  

    end   
    assign ecc_err_status = ecc_err_status_0 | ecc_err_status_1;
  end
endgenerate


endmodule
`default_nettype wire
