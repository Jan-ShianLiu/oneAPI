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


// This is a basic block in token ring. It has two sets of input signals: one set is from a LSU; and the other is from the previous lsu_ic_token it connects to.
// The output is registered. It selects the input based on if it owns token. 
// Token is passed to the next lsu_ic_token block if it owns th token and the LSU it serves has no active request.  

`default_nettype none
module lsu_ic_token (
  clk,
  resetn,
  i_token,
  i_id,  
  i_avm_waitrequest, // backpressure from root FIFO
  i_avm_byteenable,
  i_avm_address, 
  i_avm_read,  
  i_avm_write,
  i_avm_burstcount,
  o_avm_waitrequest_0, 
  o_avm_waitrequest_1,
  o_avm_byteenable,
  o_avm_address, 
  o_avm_read,
  o_avm_write,
  o_avm_burstcount,
  o_id,
  o_index,
  o_token,
  o_active,
  // write
  i_avm_writedata,
  o_avm_writedata
);      

parameter AWIDTH=32;           
parameter NUM_DIMM = 1;         
parameter MWIDTH_BYTES=64;     
parameter BURST_CNT_W=6;   
parameter READ = 1; 
parameter START_ACTIVE = 0;
parameter ID_WIDTH = 1;
parameter FIFO_DEPTH = 64;  
parameter ENABLE_LAST_WAIT = 0;
parameter ENABLE_DATA_REORDER = 0;
parameter HYPER_PIPELINE = 0;
parameter AVM_WRITE_DATA_LATENESS = 0;             // fmax and area optimization - run the write data path this many clocks later than stall/valid
parameter WIDE_DATA_SLICING = 0;                   // for large MWIDTH_BYTES, mux select signals are replicated for each data width section of WIDE_DATA_SLICING bits if nonzero

// MAX_REQUESTS_PER_LSU is a parameter for controlling the max # of read requests the interconnect will accept from an LSU. In a multi-bank BSP where bank-interleaving is used, in order to get good memory throughput, it's important that
// the traffic from all LSUs is spread somewhat equally. This is because the interconnect has various throttling mechanisms that limit how many requests a given LSU can send and how many requests can be sent to a given bank.
// An even distribution of traffic across LSUs and banks helps ensure that no single throttling mechanism will prematurely trigger long before some other throttle mechanism, which can lower throughput. By setting MAX_REQUESTS_PER_LSU to a somewhat low 
// number (say 4), the interconnect
// will service the LSUs more fairly. A large value allows one LSU to gain unfair access to the interconnect, which can lead to poorer performance in bank-interleaving. MAX_REQUESTS_PER_LSU is only used for reads today but it could be
// applied for writes in the future. In a multi-bank system the ENABLE_MULTIPLE_WR_RING parameter on lsu_ic_top must be used to saturate store bandwidth. The multi-ring feature inherently enforces a request limit per LSU due to switching 
// rings on each interleaving chunk (the chunk size is specified in board_spec.xml as "interleaved_bytes" and specifies the chunk size of data striped to each bank during interleaving). This is why MAX_REQUESTS_PER_LSU is not used for writes today.
parameter MAX_REQUESTS_PER_LSU = 4; // Set to desired value. Used to initialize a down-counter. 

localparam FIFO_AW = $clog2(FIFO_DEPTH);
localparam MWIDTH=8*MWIDTH_BYTES;
localparam REQUEST_FIFO_AW = 8;
localparam REQUEST_FIFO_DEPTH = 2**REQUEST_FIFO_AW;
localparam REQ_WIDTH_READ  = AWIDTH + BURST_CNT_W + ID_WIDTH;          
localparam REQ_WIDTH_WRITE = AWIDTH + BURST_CNT_W + MWIDTH + MWIDTH_BYTES + ID_WIDTH;
localparam REQ_WIDTH_WIDER = (REQ_WIDTH_WRITE >  REQ_WIDTH_READ)?  REQ_WIDTH_WRITE : REQ_WIDTH_READ;
localparam REQ_WIDTH = (READ? REQ_WIDTH_READ : REQ_WIDTH_WRITE);
localparam DIMM_W = (NUM_DIMM > 1)? $clog2(NUM_DIMM) : 1; // Modelsim complains about reversed bit range in i_avm_address if DIMM_W == 0. This is a workaround.

input wire                        clk;  
// resetn must be driven from the output of the parent's reset pipeline to ensure the parent comes out of reset first.
input wire                        resetn;   // reset is synchronous if (HYPER_PIPELINE=1), asynchronous otherwise
input wire                        i_token;
input wire    [ID_WIDTH-1:0]      i_id [2];
input wire                        i_avm_waitrequest     [NUM_DIMM];  // waitrequest from the previous node in the ring   
// Index [0] is the data from the previous ring node. Index [1] is the data from this node's LSU. Index [1] is forwarded to the output when this node is holding the token, otherwise Index [0] is forwarded.
input wire    [MWIDTH_BYTES-1:0]  i_avm_byteenable      [2];     
input wire    [AWIDTH-1:0]        i_avm_address         [2];     
input wire                        i_avm_read            [2];     
input wire                        i_avm_write           [2];    
input wire    [BURST_CNT_W-1:0]   i_avm_burstcount      [2]; 
output logic                      o_avm_waitrequest_0   [NUM_DIMM];   // waitrequest to the next node in the ring  
output logic                      o_avm_waitrequest_1;               // waitrequest to the LSU
output logic  [MWIDTH_BYTES-1:0]  o_avm_byteenable;           
output logic  [AWIDTH-1:0]        o_avm_address;   
output logic                      o_avm_read;               
output logic                      o_avm_write;          
output logic  [BURST_CNT_W-1:0]   o_avm_burstcount;           
input  wire   [MWIDTH-1:0]        i_avm_writedata[2];
output logic  [MWIDTH-1:0]        o_avm_writedata;
output logic  [ID_WIDTH-1:0]      o_id;
output logic                      o_token;
output logic                      o_active;
output logic  [ID_WIDTH-1:0]      o_index;

localparam MUX_DATA_SLICING_MULTIPLIER = 8; //WIDE_DATA_SLICING is intended to specify how wide each section of hld_fifo should be, e.g. 512, multiplexer data path needs to be cut smaller, e.g. 64
localparam ACTIVE_COPIES = (WIDE_DATA_SLICING==0) ? 1 : (REQ_WIDTH*MUX_DATA_SLICING_MULTIPLIER+WIDE_DATA_SLICING-1) / WIDE_DATA_SLICING;

localparam MAX_REQUEST_W = $clog2(MAX_REQUESTS_PER_LSU);

integer i;
genvar z;
reg       [ACTIVE_COPIES-1:0]    active /* synthesis dont_merge */;         //all copies are only used if MUX_SELECT_DELAY==0, otherwise if MUX_SELECT_DELAY>=1 then only active[0] is consumed and the others will be pruned away
logic     [ACTIVE_COPIES-1:0]    active_correct_lateness /* synthesis dont_merge */;    //a delayed version of active, by MUX_SELECT_DELAY clocks
wire      [REQ_WIDTH-1:0]        i_req [2];              
reg       [REQ_WIDTH-1:0]        request_dout;
logic     [0:NUM_DIMM-1]         backpressure;  
logic                            backpressure_stall;  
wire                             pass_token;
logic     [MAX_REQUEST_W:0]      request_counter; // This is a down-counter so it's sized an extra 1 bit wide to allow the counter to go negative.

localparam  ASYNC_RESET = HYPER_PIPELINE? 0 : 1; // Use synchronous resets in hyper-pipeline mode.
localparam  NUM_RESET_COPIES = 1;
/* It's important that this module is held in reset longer than the rest of the interconnect ("Rest of the interconnect" is defined as everything other than lsu_n_fast and lsu_ic_token. 
    This should be ensured by driving resetn from the output of the parent's reset pipeline.
    During reset, the LSUs are waitrequested (stalled) so staying in reset longer ensures that when we come out of reset
    and begin accepting requests from the LSU that the rest of the interconnect is ready to accept these requests.
*/
localparam  RESET_PIPE_DEPTH = 10;  
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


assign o_active = active[0];  

localparam MUX_SELECT_DELAY = (READ) ? 0 : AVM_WRITE_DATA_LATENESS;

genvar g;
generate
if (MUX_SELECT_DELAY == 0) begin : GEN_ACTIVE_COPIES_ON_TIME
  assign active_correct_lateness = active;
end
else begin : GEN_ACTIVE_COPIES_LATE             //MUX_SELECT_DELAY >= 1
  logic [MUX_SELECT_DELAY-1:0] active_delayed;  //index N is late by N+1 clocks
  for (g=0; g<MUX_SELECT_DELAY; g++) begin : GEN_ACTIVE_DELAYED
    always_ff @(posedge clk) begin    //no reset
      active_delayed[g] <= (g==0) ? active[0] : active_delayed[g-1];
    end
  end
  always_ff @(posedge clk) begin    //no reset
    active_correct_lateness <= (MUX_SELECT_DELAY==1) ? {ACTIVE_COPIES{active[0]}} : {ACTIVE_COPIES{active_delayed[MUX_SELECT_DELAY-2]}};
  end
end
//now that we have constructed active_correct_lateness (the mux select), width-slice the data path
for (g=0; g<ACTIVE_COPIES; g++) begin : GEN_REQUEST_DOUT
  localparam START_BIT = g * REQ_WIDTH / ACTIVE_COPIES;
  localparam END_BIT = (g+1) * REQ_WIDTH / ACTIVE_COPIES;
  localparam WIDTH = END_BIT - START_BIT;
  always @(posedge clk) begin
    request_dout[START_BIT +: WIDTH] <= (active_correct_lateness[g]) ? i_req[1][START_BIT +: WIDTH] : i_req[0][START_BIT +: WIDTH];
  end
end
endgenerate

always @(posedge clk or negedge aclrn) begin 
  if(!aclrn) begin
    for(i=0; i<NUM_DIMM; i=i+1) o_avm_waitrequest_0[i] <= 1'b0;
    o_avm_waitrequest_1 <= 1'b1; // Stall the LSU while the interconnect is in reset   
    o_avm_read <= 1'b0;
    o_avm_write <= 1'b0;
    for(i=0; i<ACTIVE_COPIES; i=i+1) active[i] <= START_ACTIVE;
    o_token <= 1'b0;
    o_index <= '0;
    request_counter <= MAX_REQUESTS_PER_LSU-2;
  end    
  else begin       
    o_token <= 1'b0;      
    if(ENABLE_LAST_WAIT) begin
      if(i_token) begin
        for(i=0; i<ACTIVE_COPIES; i=i+1) active[i] <= 1'b1; 
        o_index <= i_id[1];
      end   
      else if(active[0]) begin
        o_token <= pass_token;
        for(i=0; i<ACTIVE_COPIES; i=i+1) active[i] <= !pass_token;
        if(pass_token) o_index <= '0;
      end
    end
    else begin
      if(pass_token) begin
        for(i=0; i<ACTIVE_COPIES; i=i+1) active[i] <= 1'b0;
        o_token <= active[0] | i_token;  // Pass the token along.
        o_index <= '0;
      end
      else if(i_token) begin // We want to grab the token (ie. the LSU has data to send) so grab it when it arrives. active[0]==1 means we have the token and the LSU is granted access to the interconnect.
        for(i=0; i<ACTIVE_COPIES; i=i+1) active[i] <= 1'b1;
        o_index <= i_id[1];
      end
    end
    o_avm_waitrequest_0  <= i_avm_waitrequest; // pass the backpressure to the next node
    // Waitrequest backpressure to the LSU if we don't have the token, or if we're stalled, or if we're about to pass the token
    o_avm_waitrequest_1 <= !active[0] & (!i_token | ENABLE_LAST_WAIT == 0) | backpressure_stall | pass_token & active[0]; 
    o_avm_read  <= active[0] & !o_avm_waitrequest_1 & i_avm_read[1] | i_avm_read[0];  
    o_avm_write <= active[0] & !o_avm_waitrequest_1 & i_avm_write[1] | i_avm_write[0];  
    // request counter is used for READs (loads) only
    if (!pass_token & ((i_avm_read[1] | i_avm_write[1]) & !o_avm_waitrequest_1)) begin // decrement on cycles where we're not passing the token and we're accepting a request from the LSU.
      request_counter <= request_counter - 1;
    end else if (pass_token) begin
      request_counter <= MAX_REQUESTS_PER_LSU-2; // Reset value for the down-counter.
    end

    // synchronous reset (these assignments override the assignments above if srst is asserted)
    if(!sclrn[0]) begin
      for(i=0; i<NUM_DIMM; i=i+1) o_avm_waitrequest_0[i] <= 1'b0;
      o_avm_waitrequest_1 <= 1'b1;  // Stall the LSU while the interconnect is in reset          
      o_avm_read <= 1'b0;
      o_avm_write <= 1'b0;
      for(i=0; i<ACTIVE_COPIES; i=i+1) active[i] <= START_ACTIVE;
      o_token <= 1'b0;
      o_index <= '0;
      request_counter <= MAX_REQUESTS_PER_LSU-2; // Reset value for the down-counter.
    end  

  end
end

/*  Create a backpressure signal, per bank, then OR all the signals together to create a single stall to the LSU.
    In general, we stall the LSU if ANY bank is stalling.
    It might seem intuitive to instead only stall the LSU if the bank it is accessing stalls (ie. who cares if some other bank stalls). The problem here
    is that o_avm_waitrequest_1 is registered, so there is 1 cycle of latency from receiving the stall from the bank to applying the stall to the LSU.
    If the LSU is toggling between accesses to multiple banks, when it attempts to access a bank that is stalling, it will take 1 cycle to propagate that stall,
    during which time we will mistakenly accept a request to that bank (and possibly overflow the root FIFO). To implement this properly, we would need lookahead
    on the LSU's addresses to compensate for the latency in applying stall.
    To simplify things, we stall the LSU if any bank is stalling. So even if the LSU is toggling between access to multiple banks, the requests will only be accepted into
    the interconnect if all of the banks can accept a request -- this guarantees the root FIFOs won't overflow.

    However, there is one case in which we can safely create a per-bank stall signal based on the current address: a read ring with data-reordering disabled (ie. SW-DIMM mode).
    In this mode, by definition of "SW-DIMM", the LSU only ever accesses 1 bank, so there is no toggling between banks. For reads, if the current bank is stalling we want to allow the token to circulate so that
    another Load-LSU who wants to read from another unstalled bank can do so.
*/
assign backpressure_stall = |backpressure;
localparam SWDIMM_ENABLED = NUM_DIMM > 1 && !ENABLE_DATA_REORDER && READ;
generate
  if(SWDIMM_ENABLED) begin : GEN_MULTIPLE_STALL
    for(z=0; z<NUM_DIMM; z=z+1) begin : GEN_
      assign backpressure[z] = i_avm_waitrequest[z] & i_avm_address[1][AWIDTH-1:AWIDTH-DIMM_W] == z;
    end
  end
  else begin : GEN_SINGLE_STALL
    for(z=0; z<NUM_DIMM; z=z+1) begin : GEN_
      assign backpressure[z] = i_avm_waitrequest[z];
    end
  end
  if(READ) begin : ENABLE_READ    
    // Pass the token if the LSU has no requests to issue or the LSU has reached its MAX_REQUEST allotment. If using SWDIMM, also pass the token when stalled to allow another LSU to access a potentially other free bank.
    // In SWDIMM we get a per-bank stall. In bank-interleaving, we only get a stall on bit-0, which is an OR of the stalls from all banks (this is OK to do because interleaving distributes even traffic to all banks).
    assign pass_token = !i_avm_read[1] | (SWDIMM_ENABLED & backpressure_stall) | (request_counter[MAX_REQUEST_W] & !o_avm_waitrequest_1); // Pass the token when MAX_REQUESTS_PER_LSU have been sent
    assign i_req[0] = {i_avm_address[0], i_avm_burstcount[0], i_id[0]};
    assign i_req[1] = {i_avm_address[1], i_avm_burstcount[1], i_id[1]};  
    assign {o_avm_address, o_avm_burstcount, o_id} = request_dout;
    assign o_avm_byteenable = {MWIDTH_BYTES{1'b1}};    
  end
  else begin : ENABLE_WRITE        
    assign pass_token = !i_avm_write[1];
    assign i_req[0] = {i_avm_address[0], i_avm_burstcount[0], i_avm_writedata[0], i_avm_byteenable[0], i_id[0]} ;
    assign i_req[1] = {i_avm_address[1], i_avm_burstcount[1], i_avm_writedata[1], i_avm_byteenable[1], i_id[1]} ;                                                 
    assign {o_avm_address, o_avm_burstcount, o_avm_writedata, o_avm_byteenable, o_id} = request_dout;    
  end
endgenerate
endmodule
`default_nettype wire
