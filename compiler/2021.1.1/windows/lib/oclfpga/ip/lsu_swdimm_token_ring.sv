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


// This module is used for sw-dimm-partiton. 
// It has two features: 
// [1] data reordering is disabled 
// [2] each load LSU has a counter to make sure bank switch is allowed to happen only when all the previous requests have returned.
`default_nettype none
module lsu_swdimm_token_ring (
  clk,
  resetn,
  // from LSU
  i_rd_byteenable,
  i_rd_address, 
  i_rd_request,          
  i_rd_burstcount,  
  i_wr_byteenable,
  i_wr_address, 
  i_wr_request,          
  i_wr_burstcount,
  i_wr_writedata,  
  // from MEM
  i_avm_waitrequest,
  i_avm_write_ack,
  i_avm_readdata, 
  i_avm_return_valid,        
  // to MEM  
  o_id,              
  o_avm_byteenable,          
  o_avm_address,              
  o_avm_read,        
  o_avm_write,             
  o_avm_burstcount,
  o_avm_writedata,
  // to kernel
  o_rd_waitrequest,
  o_wr_waitrequest,
  o_avm_readdata,
  o_avm_readdatavalid,
  o_avm_writeack,
  ecc_err_status
);


parameter AWIDTH = 32;            
parameter MWIDTH_BYTES = 64;     
parameter BURST_CNT_W = 5;          
parameter NUM_RD_PORT = 2;
parameter NUM_WR_PORT = 2;
parameter NUM_DIMM = 1;
parameter DISABLE_ROOT_FIFO = 0;
parameter MAX_MEM_DELAY = 64;
parameter ENABLE_READ_FAST = 0;
parameter HYPER_PIPELINE = 0;             // 1 = optimized, highly pipelined mode, at the expense of area.
parameter ROOT_FIFO_AW = 8;               // Token root FIFO address width; FIFO depth = 2**ROOT_FIFO_AW
parameter RD_ROOT_FIFO_AW = 7; 
parameter ENABLE_DATA_REORDER = 0;
parameter ENABLE_DUAL_RING = 0;   
parameter ENABLE_MULTIPLE_WR_RING = 0;
parameter ENABLE_LAST_WAIT = 0;
parameter NUM_AVM_OUTPUT_PIPE_STAGES = 1;  // Minimum value 1. Length of pipeline stages between root FIFOs and CCB. This can be increased for performance (note that the slave-side
                                            // waitrequest allowance must be increased by the same amount as well). Only used when HYPER_PIPELINE=1
parameter PIPELINE_RD_RETURN = 0;            // 1 = Route the read-return data to the LSUs in a pipelined vine (may help with performance). 0 = fan-out to all LSUs.
parameter ENABLE_BSP_WAITREQUEST_ALLOWANCE = 0; // Enables waitrequest-allowance on the AVMM interface. This param is passed down to lsu_token_ring so see comments in that module for more details.
parameter WRITE_ACK_FIFO_DEPTH = 1024;  
parameter ENABLE_BSP_AVMM_WRITE_ACK = 0; 
parameter AVM_WRITE_DATA_LATENESS = 0;          // fmax and area optimization - run the write data path this many clocks later than stall/valid
parameter AVM_READ_DATA_LATENESS = 0;           // fmax and area optimization - o_avm_readdata is late by this many clocks compared to o_avm_readdatavalid
parameter WIDE_DATA_SLICING = 0;                // for large MWIDTH_BYTES, a nonzero value indicate how wide to width-slice hld_fifo, also mux select signals are replicated based on width needed
parameter ROOT_FIFO_STALL_IN_EARLINESS = 0;     // How much stall-in earliness should be expected for W/R root FIFOs when HYPER_PIPELINE=1. 
parameter ROOT_WFIFO_VALID_IN_EARLINESS = 0;   // How much valid-in earliness should be expected for write root FIFO when HYPER_PIPELINE=1
parameter ALLOW_HIGH_SPEED_FIFO_USAGE = 1;      // choice of hld_fifo style, 0 = mid speed fifo, 1 = high speed fifo
parameter enable_ecc = "FALSE";    // Enable error correction coding
parameter MAX_REQUESTS_PER_LSU = 4;

parameter int NUM_MEM_SYSTEMS = 1;             
parameter [NUM_MEM_SYSTEMS-1:0][31:0] NUM_BANKS_PER_MEM_SYSTEM = {(NUM_MEM_SYSTEMS){32'd1}}; // index position [0] is in the right-most position.
parameter [NUM_MEM_SYSTEMS-1:0][31:0] NUM_BANKS_W_PER_MEM_SYSTEM = {(NUM_MEM_SYSTEMS){32'd1}}; // Bit-width of each NUM_BANKS
parameter [NUM_MEM_SYSTEMS-1:0][31:0] BANK_BIT_LSB_PER_MEM_SYSTEM = {(NUM_MEM_SYSTEMS){32'd30}};
parameter [NUM_MEM_SYSTEMS-1:0][31:0] ENABLE_BANK_INTERLEAVING = {(NUM_MEM_SYSTEMS){32'd1}};   // Interconnect will permute the AVMM addresses to stripe accesses across available banks. This can be controlled on each mem system.
parameter int LARGEST_NUM_BANKS = 1;
parameter [NUM_MEM_SYSTEMS-1:0][LARGEST_NUM_BANKS-1:0][31:0] ROOT_PORT_MAP  = {'0}; //'
parameter int ROOT_ARB_BALANCED_RW = 0;
localparam LARGEST_NUM_BANKS_W = $clog2(LARGEST_NUM_BANKS);
localparam DIMM_W = $clog2(NUM_DIMM);
localparam O_AVM_ADDRESS_W = (NUM_MEM_SYSTEMS > 1)? AWIDTH : AWIDTH-DIMM_W;
localparam NUM_MEM_SYSTEMS_W = (NUM_MEM_SYSTEMS==1)? 1 : $clog2(NUM_MEM_SYSTEMS);
localparam MWORD_PAD = $clog2(MWIDTH_BYTES);

localparam MWIDTH=8*MWIDTH_BYTES;
localparam NUM_ID = NUM_RD_PORT+NUM_WR_PORT;
localparam NUM_DIMM_W = $clog2(NUM_DIMM);
localparam RD_ID_WIDTH = (NUM_RD_PORT==1)?  1 : $clog2(NUM_RD_PORT);
localparam WR_ID_WIDTH = (NUM_WR_PORT==1)?  1 : $clog2(NUM_WR_PORT);
localparam ID_WIDTH = (RD_ID_WIDTH > WR_ID_WIDTH)? RD_ID_WIDTH : WR_ID_WIDTH;
localparam CNT_WIDTH = 8;
// avoid modelsim compile error
localparam P_NUM_RD_PORT   = (NUM_RD_PORT > 0)?   NUM_RD_PORT   : 1;   
localparam P_NUM_WR_PORT   = (NUM_WR_PORT > 0)?   NUM_WR_PORT   : 1;   
   

input wire clk;
input wire resetn;   // reset is synchronous (if HYPER_PIPELINE==1), asynchronous otherwise
// from LSU
input wire [MWIDTH_BYTES-1:0] i_rd_byteenable [P_NUM_RD_PORT];
input wire [AWIDTH-1:0] i_rd_address [P_NUM_RD_PORT];  // MWORD addresses, LSBs are truncated by lsu_ic_top
input wire i_rd_request [P_NUM_RD_PORT];          
input wire [BURST_CNT_W-1:0] i_rd_burstcount [P_NUM_RD_PORT];  
input wire [MWIDTH_BYTES-1:0] i_wr_byteenable [P_NUM_WR_PORT]; 
input wire [AWIDTH-1:0] i_wr_address [P_NUM_WR_PORT];
input wire i_wr_request [P_NUM_WR_PORT];         
input wire [BURST_CNT_W-1:0] i_wr_burstcount [P_NUM_WR_PORT];
input wire [MWIDTH-1:0] i_wr_writedata [P_NUM_WR_PORT];
// from MEM
input wire i_avm_waitrequest [NUM_DIMM];
input wire i_avm_write_ack [NUM_DIMM];    
input wire [MWIDTH-1:0] i_avm_readdata [NUM_DIMM];            
input wire i_avm_return_valid [NUM_DIMM];                   
// to MEM                
output logic [ID_WIDTH-1:0] o_id [NUM_DIMM];
output logic [MWIDTH_BYTES-1:0] o_avm_byteenable [NUM_DIMM];            
output logic [O_AVM_ADDRESS_W-1:0] o_avm_address [NUM_DIMM];                
output logic o_avm_read [NUM_DIMM];                   
output logic o_avm_write [NUM_DIMM];                          
output logic [BURST_CNT_W-1:0] o_avm_burstcount [NUM_DIMM];  
output logic [MWIDTH-1:0] o_avm_writedata [NUM_DIMM];  
// to kernel
output logic o_rd_waitrequest [P_NUM_RD_PORT]; 
output logic o_wr_waitrequest [P_NUM_WR_PORT]; 
output logic [MWIDTH-1:0] o_avm_readdata [P_NUM_RD_PORT];     
output logic o_avm_readdatavalid [P_NUM_RD_PORT]; 
output logic o_avm_writeack [P_NUM_WR_PORT];
output logic  [1:0] ecc_err_status;  // ecc status signals

integer i, j;
wire  i_read  [P_NUM_RD_PORT];  
wire  to_lsu_rd_waitrequest [P_NUM_RD_PORT];
wire  avm_read [NUM_DIMM]; 

// dimm switch guard
wire [0:NUM_RD_PORT-1] rd_accept;
wire [0:NUM_RD_PORT-1] switch_wait;
reg  [P_NUM_RD_PORT-1:0][CNT_WIDTH-1:0] pending_rd ;      
reg  [0:NUM_RD_PORT-1] pending_af; 
reg  [NUM_DIMM_W-1:0] pending_dimm_num [P_NUM_RD_PORT];           
reg  [0:NUM_RD_PORT-1]  wait_rd;

// read back data
wire id_af [NUM_DIMM]; 
wire ring_i_stall [NUM_DIMM]; 
reg  [MWIDTH-1:0] rd_data [NUM_DIMM];                   
      

wire avm_writeack [NUM_DIMM][P_NUM_WR_PORT];
wire [0:NUM_DIMM-1] v_avm_writeack [P_NUM_WR_PORT];
reg [0:NUM_DIMM-1] R_valid;

localparam                    ASYNC_RESET = HYPER_PIPELINE? 0 : 1; // Use synchronous resets in hyper-pipeline mode.
localparam                    NUM_RESET_COPIES = 1;
localparam                    RESET_PIPE_DEPTH = 5;
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


// The following block of code examines the current address from each read LSU and determines which
// root port the request is targeting. We're looking for changes to the target on back-to-back requests
// to decide if we need to block the request.
logic [LARGEST_NUM_BANKS_W-1:0] bank_mask [NUM_MEM_SYSTEMS];
logic [NUM_DIMM_W-1:0] rd_address_current_root_port [P_NUM_RD_PORT]; // The DIMM # indicated by the current read addres (i_rd_address)

genvar g, z, z0;
generate

  // Helper mask to isolate the relevant address bits, per mem system
  for (g=0; g<NUM_MEM_SYSTEMS;g=g+1) begin : GEN_RANDOM_HACK_NAME_R1911_0
    // Put zeroes in the bit positions that are not used for bank bits (not all mem systems use the same number of bank bits)
    if (LARGEST_NUM_BANKS > 1) begin // But only if there are bank bits
      assign bank_mask[g] = {LARGEST_NUM_BANKS_W{1'b1}} >> (LARGEST_NUM_BANKS_W - NUM_BANKS_W_PER_MEM_SYSTEM[g]);
    end
  end

  // Determine the current target mem system
  for(g=0; g<NUM_RD_PORT; g=g+1) begin : GEN_RANDOM_HACK_NAME_R191_1
    logic [NUM_MEM_SYSTEMS_W-1:0]   current_mem_system;
    logic [LARGEST_NUM_BANKS_W-1:0] current_bank_within_mem_system;

    if (NUM_DIMM == 1) begin
      assign rd_address_current_root_port[g] = 0;
    end else begin // NUM_DIMM > 1
      if (NUM_MEM_SYSTEMS == 1) begin
        assign current_mem_system = 0;
      end else begin
        assign current_mem_system = i_rd_address[g][AWIDTH-1 -: NUM_MEM_SYSTEMS_W];
      end
      // Now that we know the target mem system, grab the bank bits
      if (LARGEST_NUM_BANKS > 1) begin
        // Not all bank bits may be used in the current mem system, so zero out the unused bits with bank_mask.
        // MWORD_PAD is subtracted because i_rd_address (and i_wr_address) are MWORD addresses but BANK_BIT_LSB_PER_MEM_SYSTEM specifies bit locations assuming byte-addresses.
        assign current_bank_within_mem_system = bank_mask[current_mem_system] & (i_rd_address[g][BANK_BIT_LSB_PER_MEM_SYSTEM[current_mem_system]-MWORD_PAD+LARGEST_NUM_BANKS_W-1 -: LARGEST_NUM_BANKS_W]);
      end else begin
        assign current_bank_within_mem_system = 0;
      end
      // Now that we have both the mem system and bank, look up the corresponding root port being targeted
      assign rd_address_current_root_port[g] = ROOT_PORT_MAP[current_mem_system][current_bank_within_mem_system];
    end 
  end
endgenerate

always @(posedge clk) begin
  for(i=0; i<NUM_DIMM; i=i+1) rd_data[i] <= i_avm_readdata[i];
  for(i=0; i<NUM_RD_PORT; i=i+1) begin  : GEN_RANDOM_BLOCK_NAME_R191
    if(rd_accept[i]) pending_dimm_num[i] <= rd_address_current_root_port[i];
  end
end   

/*
  pending_af asserts if there are more than 128 pending read words (read words, not read requests).
  wait_rd is asserted as long as there are pending reads.
  Therefore switch_wait asserts (ie. the LSU is backpressured) if there are some pending reads and the LSU is trying to switch banks.
  Also, when switch_wait asserts, we don't let the ring see the LSU's read-request.
  switch_wait is like a gatekeeper between the LSU and ring. It blocks the LSU and ring from "seeing" each other if a bank switch is being attempted.
  In this way, a bank switch is not allowed to happen. Since re-ordering is disabled, a bank switch could result in data being returned to the load-LSU
  out of order.
  As the comment below says, it's not clear why pending_af is used.
*/
generate    
  for(z=0; z<NUM_RD_PORT; z=z+1) begin : GEN_MERGE_RD        
    assign i_read[z] = i_rd_request[z] & !switch_wait[z];        
    assign o_rd_waitrequest[z] = to_lsu_rd_waitrequest[z] | switch_wait[z];           
    assign rd_accept[z] = i_rd_request[z] & !o_rd_waitrequest[z];    
    assign switch_wait[z] = wait_rd[z] & pending_dimm_num[z] !== rd_address_current_root_port[z] | pending_af[z];// may not need pending_af    
  end
endgenerate

lsu_token_ring #(
  .AWIDTH(AWIDTH),
  .MWIDTH_BYTES(MWIDTH_BYTES),
  .BURST_CNT_W(BURST_CNT_W),
  .NUM_RD_PORT(NUM_RD_PORT),
  .NUM_WR_PORT(NUM_WR_PORT),
  .NUM_DIMM(NUM_DIMM),
  .MAX_MEM_DELAY(MAX_MEM_DELAY),     
  .ENABLE_LAST_WAIT(ENABLE_LAST_WAIT),    
  .ENABLE_DUAL_RING(ENABLE_DUAL_RING),
  .ENABLE_MULTIPLE_WR_RING(ENABLE_MULTIPLE_WR_RING),
  .ROOT_FIFO_AW(ROOT_FIFO_AW),
  .RD_ROOT_FIFO_AW(RD_ROOT_FIFO_AW),
  .ENABLE_DATA_REORDER(ENABLE_DATA_REORDER),  // Should be disabled by the parent
  .ENABLE_READ_FAST(ENABLE_READ_FAST),
  .DISABLE_ROOT_FIFO(DISABLE_ROOT_FIFO),
  .PIPELINE_RD_RETURN(PIPELINE_RD_RETURN),
  .HYPER_PIPELINE(HYPER_PIPELINE),
  .ENABLE_BSP_WAITREQUEST_ALLOWANCE(ENABLE_BSP_WAITREQUEST_ALLOWANCE),
  .ENABLE_BSP_AVMM_WRITE_ACK(ENABLE_BSP_AVMM_WRITE_ACK),
  .WRITE_ACK_FIFO_DEPTH(WRITE_ACK_FIFO_DEPTH),   
  .AVM_WRITE_DATA_LATENESS(AVM_WRITE_DATA_LATENESS),
  .AVM_READ_DATA_LATENESS(AVM_READ_DATA_LATENESS),
  .WIDE_DATA_SLICING(WIDE_DATA_SLICING),
  .ROOT_FIFO_STALL_IN_EARLINESS(ROOT_FIFO_STALL_IN_EARLINESS),
  .ROOT_WFIFO_VALID_IN_EARLINESS(ROOT_WFIFO_VALID_IN_EARLINESS),
  .ALLOW_HIGH_SPEED_FIFO_USAGE(ALLOW_HIGH_SPEED_FIFO_USAGE),
  .NUM_AVM_OUTPUT_PIPE_STAGES(NUM_AVM_OUTPUT_PIPE_STAGES),
  .MAX_REQUESTS_PER_LSU(MAX_REQUESTS_PER_LSU),
  .enable_ecc(enable_ecc),
  .NUM_MEM_SYSTEMS             (NUM_MEM_SYSTEMS),
  .NUM_BANKS_PER_MEM_SYSTEM    (NUM_BANKS_PER_MEM_SYSTEM),
  .NUM_BANKS_W_PER_MEM_SYSTEM  (NUM_BANKS_W_PER_MEM_SYSTEM),
  .BANK_BIT_LSB_PER_MEM_SYSTEM (BANK_BIT_LSB_PER_MEM_SYSTEM),
  .ENABLE_BANK_INTERLEAVING    (ENABLE_BANK_INTERLEAVING),
  .LARGEST_NUM_BANKS           (LARGEST_NUM_BANKS),
  .ROOT_PORT_MAP               (ROOT_PORT_MAP),
  .ROOT_ARB_BALANCED_RW        (ROOT_ARB_BALANCED_RW)
 ) lsu_token_ring (
  .clk              (clk),
  .resetn           (resetn_synchronized),
  .i_rd_byteenable  (i_rd_byteenable),
  .i_rd_address     (i_rd_address),
  .i_rd_request     (i_read),
  .i_rd_burstcount  (i_rd_burstcount),
  .i_wr_byteenable  (i_wr_byteenable),
  .i_wr_address     (i_wr_address),
  .i_wr_request     (i_wr_request),
  .i_wr_burstcount  (i_wr_burstcount),
  .i_wr_writedata   (i_wr_writedata),
  .i_avm_waitrequest(i_avm_waitrequest),
  .i_avm_write_ack  (i_avm_write_ack),
  .i_avm_readdata   (i_avm_readdata),
  .i_avm_return_valid(i_avm_return_valid),
  .o_avm_byteenable (o_avm_byteenable),
  .o_avm_address    (o_avm_address),
  .o_avm_read       (o_avm_read),
  .o_avm_write      (o_avm_write),
  .o_avm_burstcount (o_avm_burstcount),
  .o_avm_writedata  (o_avm_writedata),
  .o_avm_readdata   (o_avm_readdata),
  .o_avm_readdatavalid(o_avm_readdatavalid),
  .o_id             (o_id),
  .o_avm_writeack   (o_avm_writeack),
  .o_rd_waitrequest (to_lsu_rd_waitrequest),
  .o_wr_waitrequest (o_wr_waitrequest),
  .ecc_err_status(ecc_err_status)
);


// ----------------------------
// Generate read pending count   
// ----------------------------   
always @(posedge clk or negedge aclrn) begin
  if(!aclrn) begin
    pending_rd <= '0;
    wait_rd <= '0;
    pending_af <= '0;
  end
  else begin          
    for(i=0; i<NUM_RD_PORT; i=i+1) begin : GEN_RANDOM_HACK_NAME_R191
      pending_af[i] <= pending_rd[i][CNT_WIDTH-1];
      case({rd_accept[i], o_avm_readdatavalid[i]})         
        2'b01: begin pending_rd[i] <= pending_rd[i] - 1'b1;                       wait_rd[i] <= pending_rd[i] > 1; end   
        2'b10: begin pending_rd[i] <= pending_rd[i] + i_rd_burstcount[i];         wait_rd[i] <= 1'b1; end
        2'b11: begin pending_rd[i] <= pending_rd[i] + i_rd_burstcount[i] - 1'b1;  wait_rd[i] <= 1'b1; end
      endcase           
    end       
    if(!sclrn[0]) begin
      pending_rd <= '0;
      wait_rd <= '0;
      pending_af <= '0;
    end    
  end  
end

endmodule
`default_nettype wire