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


// This block is used for load ring.
// The datapath for load ring is much narrower compared with store ring
// In most cases, it does not need a group of 2-to-1 MUX to pipeline the selected request.
// Instead, a N-to-1 one clock cycle delay MUX is enough.
// It is optional, enabled by setting lsu_ic_top.ENABLE_READ_FAST
`default_nettype none
module lsu_n_fast (
  clk,
  resetn,
  i_token,
  i_ext_id,
  i_ext_address,
  i_ext_read,        
  i_ext_burstcount,
  o_ext_waitrequest,  
  i_avm_waitrequest, 
  i_avm_byteenable,
  i_avm_address, 
  i_avm_read,        
  i_avm_write,        
  i_avm_burstcount,
  o_avm_waitrequest, 
  o_avm_byteenable,
  o_avm_address, 
  o_avm_read,       
  o_avm_write,    
  o_avm_request,
  o_avm_burstcount,
  o_id,
  o_token,
  // write
  i_avm_writedata,
  o_avm_writedata
);

parameter AWIDTH=32;            // Address width (32-bits for Avalon)
parameter MWIDTH_BYTES=64;      // Width of the global memory bus (bytes)
parameter BURST_CNT_W=6;                             
parameter NUM_PORT = 10; 
parameter NUM_DIMM = 2;
parameter START_ID = 0;
parameter READ = 1;   
parameter ENABLE_DATA_REORDER = 0;
parameter OPEN_RING = 0;  
parameter START_ACTIVE = 0;
parameter ID_WIDTH = (NUM_PORT+START_ID == 1 )? 1 : $clog2(NUM_PORT+START_ID);
parameter ENABLE_LAST_WAIT = 0;
parameter HYPER_PIPELINE = 0;                      // 1 = optimized, highly pipelined mode, only sclrs (no aclrs) at the expense of area.

// MAX_REQUESTS_PER_LSU is a parameter for controlling the max # of read requests the interconnect will accept from an LSU. In a multi-bank BSP where bank-interleaving is used, in order to get good memory throughput, it's important that
// the traffic from all LSUs is spread somewhat equally. This is because the interconnect has various throttling mechanisms that limit how many requests a given LSU can send and how many requests can be sent to a given bank.
// An even distribution of traffic across LSUs and banks helps ensure that no single throttling mechanism will prematurely trigger long before some other throttle mechanism, which can lower throughput. By setting MAX_REQUESTS_PER_LSU to a somewhat low 
// number (say 4), the interconnect
// will service the LSUs more fairly. A large value allows one LSU to gain unfair access to the interconnect, which can lead to poorer performance in bank-interleaving. MAX_REQUESTS_PER_LSU is only used for reads today but it could be
// applied for writes in the future. In a multi-bank system the ENABLE_MULTIPLE_WR_RING parameter on lsu_ic_top must be used to saturate store bandwidth. The multi-ring feature inherently enforces a request limit per LSU due to switching 
// rings on each interleaving chunk (the chunk size is specified in board_spec.xml as "interleaved_bytes" and specifies the chunk size of data striped to each bank during interleaving). This is why MAX_REQUESTS_PER_LSU is not used for writes today.
parameter MAX_REQUESTS_PER_LSU = 4; // Set to desired value. 

localparam MWIDTH=8*MWIDTH_BYTES;
localparam DIMM_W = $clog2(NUM_DIMM);
localparam REQ_WIDTH_READ  = AWIDTH + BURST_CNT_W + ID_WIDTH;          
localparam REQ_WIDTH_WRITE = AWIDTH + BURST_CNT_W + MWIDTH + MWIDTH_BYTES + ID_WIDTH;
localparam REQ_WIDTH = READ? REQ_WIDTH_READ : REQ_WIDTH_WRITE;


input wire                          clk;  
// resetn must be driven from the output of the parent's reset pipeline to ensure the parent comes out of reset first.
input wire                          resetn;   // reset is synchronous if HYPER_PIPELINE==1, asynchronous otherwise.
// from another open ring part
input wire     [ID_WIDTH-1:0]       i_ext_id;
input wire     [AWIDTH-1:0]         i_ext_address;
input wire                          i_ext_read;        
input wire     [BURST_CNT_W-1:0]    i_ext_burstcount;
// from LSUs
input wire                          i_token;
input wire     [MWIDTH_BYTES-1:0]   i_avm_byteenable      [NUM_PORT];     
input wire     [AWIDTH-1:0]         i_avm_address         [NUM_PORT];     
input wire                          i_avm_read            [NUM_PORT];     
input wire                          i_avm_write           [NUM_PORT];   
input wire     [BURST_CNT_W-1:0]    i_avm_burstcount      [NUM_PORT];  
// to LSUs
output logic                        o_avm_waitrequest     [NUM_PORT];   
output logic                        o_token;
// from memory
input  wire                         i_avm_waitrequest [NUM_DIMM];     
// to another open ring
output logic                        o_ext_waitrequest [NUM_DIMM]; // delayed i_avm_waitrequest
// to memory,
output wire    [MWIDTH_BYTES-1:0]   o_avm_byteenable;           
output wire    [AWIDTH-1:0]         o_avm_address;              
output wire                         o_avm_read;
output wire                         o_avm_write;               
output wire                         o_avm_request;  
output wire    [BURST_CNT_W-1:0]    o_avm_burstcount;
output wire    [ID_WIDTH-1:0]       o_id;
// write  
input wire     [MWIDTH-1:0]         i_avm_writedata [NUM_PORT];
output wire   [MWIDTH-1:0]         o_avm_writedata;

localparam MAX_REQUEST_W = $clog2(MAX_REQUESTS_PER_LSU);

integer i;
genvar z, z0;
wire i_avm_request  [NUM_PORT];   
wire [ID_WIDTH-1:0] i_id [NUM_PORT];      
wire [REQ_WIDTH-1:0] i_req [NUM_PORT];  
wire [REQ_WIDTH-1:0] ext_req;     
reg  active;
reg  [ID_WIDTH-1:0]  ptr;    
wire [REQ_WIDTH-1:0] request_dout;
wire [0:NUM_DIMM-1] backpressure_stall [NUM_PORT];
wire backpressure [NUM_PORT]; 
reg  avm_read [NUM_PORT];
reg  avm_write [NUM_PORT];
reg  avm_request [NUM_PORT];
wire avm_read_wire;
wire avm_write_wire;
wire avm_request_wire;                
reg  avm_read_p;        
reg  avm_write_p;       
reg  avm_request_p;     
reg  [BURST_CNT_W-1:0] wr_cnt; 
reg  wr_burst_done;
wire pass_token;
logic     [MAX_REQUEST_W:0]    request_counter;

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

assign avm_read_wire    = active & !o_avm_waitrequest[ptr] & i_avm_read[ptr];       
assign avm_write_wire   = active & !o_avm_waitrequest[ptr] & i_avm_write[ptr];      
assign avm_request_wire = active & !o_avm_waitrequest[ptr] & i_avm_request[ptr];   
assign ext_req = {i_ext_address, i_ext_burstcount, {MWIDTH{1'b0}}, {MWIDTH_BYTES{1'b1}}, i_ext_id};   

always @(posedge clk or negedge aclrn) begin 
  if(!aclrn) begin 
    avm_read_p    <= 1'b0;
    avm_write_p   <= 1'b0;
    avm_request_p <= 1'b0;         
     for(i=0; i<NUM_DIMM; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R135
      o_ext_waitrequest[i] <= 1'b0;
    end
    for(i=0; i<NUM_PORT; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R136
      avm_read[i] <= 0;
      avm_write[i] <= 0;
      avm_request[i] <= 0;
    end    
  end
  else begin  
    avm_read_p    <= avm_read_wire;      
    avm_write_p   <= avm_write_wire;    
    avm_request_p <= avm_request_wire;  
    for(i=0; i<NUM_DIMM; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R137
      o_ext_waitrequest[i] <= i_avm_waitrequest[i];
    end
    
    for(i=0; i<NUM_PORT; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R138
      if(active & i==ptr) begin
        avm_read[i]    <=  avm_read_wire;  
        avm_write[i]   <=  avm_write_wire;  
        avm_request[i] <=  avm_request_wire;
      end
      else if(i>0) begin
        avm_read[i]  <= avm_read[i-1];
        avm_write[i] <= avm_write[i-1];
        avm_request[i] <= avm_request[i-1];
      end
      else begin
        avm_read[0]  <= i_ext_read;
        avm_write[0] <= 1'b0;
        avm_request[0] <= i_ext_read;
      end
    end
    // synchronous reset (these assignments override the assignments above)
    if(!sclrn[0]) begin 
      avm_read_p    <= 1'b0;
      avm_write_p   <= 1'b0;
      avm_request_p <= 1'b0;         
       for(i=0; i<NUM_DIMM; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R139
        o_ext_waitrequest[i] <= 1'b0;
      end
      for(i=0; i<NUM_PORT; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R140
        avm_read[i] <= 0;
        avm_write[i] <= 0;
        avm_request[i] <= 0;
      end    
    end    
  end
end

always @(posedge clk or negedge aclrn) begin 
  if (!aclrn) begin
     request_counter <= MAX_REQUESTS_PER_LSU-2;
  end else begin
    
    // request counter is used for READs (loads) only
    if (!pass_token & avm_read_wire) begin // decrement on cycles where we're not passing the token and we're accepting a request from the LSU.
      request_counter <= request_counter - 1;
    end else if (pass_token) begin
      request_counter <= MAX_REQUESTS_PER_LSU-2; // We do not allow loads to pause. They must dump their requests on back to back cycles. If the read ring itself is backpressured, we pass the token anyways. So it's safe to just reset the counter here.
    end    

    if (!sclrn) begin
      request_counter <= MAX_REQUESTS_PER_LSU-2;
    end
  end
end

generate
if(ENABLE_LAST_WAIT) begin : GEN_ENABLE_LAST_WAIT  
  always @(posedge clk or negedge aclrn) begin 
    if(!aclrn) begin
      ptr <= 0;
      active <= START_ACTIVE;
      o_avm_waitrequest[0] <= 1'b1; // Stall the LSU while in reset
      for(i=1; i<NUM_PORT; i=i+1) o_avm_waitrequest[i] <= 1'b1;      
      o_token <= 1'b0;
    end
    else begin
      o_token <= 1'b0;    
      if(i_token) active <= 1'b1;        
      if(pass_token) begin           
        if(ptr == NUM_PORT-1) begin if (!o_avm_waitrequest[NUM_PORT-1] | !active & !i_token) ptr <= '0;end
        else if(active) ptr <= ptr + 1'b1;
        if(ptr == NUM_PORT-1 & !o_avm_waitrequest[NUM_PORT-1]) begin
          o_token <= active;
          active <= !OPEN_RING; 
        end
      end 
      
      for(i=0; i<NUM_PORT; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R141
        if(i==ptr) o_avm_waitrequest[i] <= !active & !i_token | backpressure[i] | ((ptr < NUM_PORT - 1)? !i_avm_request[i] : !i_avm_request[i] & !o_avm_waitrequest[i]);     
        else o_avm_waitrequest[i] <= 1'b1;
      end
      // synchronous reset (these assignments override the assignments above)
      if(!sclrn[0]) begin
        ptr <= 0;
        active <= START_ACTIVE;
        o_avm_waitrequest[0] <= 1'b1; // Stall the LSU while in reset
        for(i=1; i<NUM_PORT; i=i+1) o_avm_waitrequest[i] <= 1'b1;      
        o_token <= 1'b0;
      end      
    end 
  end  
end
else begin : GEN_DISABLE_LAST_WAIT
  always @(posedge clk or negedge aclrn) begin 
    if(!aclrn) begin
      ptr <= 0;
      active <= START_ACTIVE;
      o_avm_waitrequest[0] <= 1'b1; // Stall the LSU while in reset
      for(i=1; i<NUM_PORT; i=i+1) o_avm_waitrequest[i] <= 1'b1;
      o_token <= 1'b0;
    end
    else begin
      o_token <= 1'b0;    
      if(i_token) active <= 1'b1;       
      if(pass_token) begin
        if(ptr == NUM_PORT - 1) begin
          o_token <= active | i_token;
          active <= !OPEN_RING; 
          ptr <= '0;
        end
        else if(active) ptr <= ptr + 1'b1;
      end         
      
      for(i=0; i<NUM_PORT; i=i+1) begin : GEN_RANDOM_BLOCK_NAME_R142
        if(i==ptr) o_avm_waitrequest[i] <= !active & !i_token | backpressure[i] | pass_token;     
        else o_avm_waitrequest[i] <= 1'b1;
      end
      // synchronous reset (these assignments override the assignments above)
      if(!sclrn[0]) begin
        ptr <= 0;
        active <= START_ACTIVE;
        o_avm_waitrequest[0] <= 1'b1; // Stall the LSU while in reset
        for(i=1; i<NUM_PORT; i=i+1) o_avm_waitrequest[i] <= 1'b1;
        o_token <= 1'b0;
      end      
    end 
  end
end

// Create per-bank stall when using SWDIMM. Pass the token when stalled.
// If not using SWDIMM (i.e. using bank-interleaving), creating a single stall signal.

localparam SWDIMM_ENABLED = NUM_DIMM > 1 & !ENABLE_DATA_REORDER;
for(z=0; z<NUM_PORT; z=z+1) begin : GEN_STALL
  if(SWDIMM_ENABLED) begin : GEN_MULTIPLE_DIMM
    for(z0=0; z0<NUM_DIMM; z0=z0+1) begin : GEN_
      assign backpressure_stall[z][z0] = i_avm_waitrequest[z0] & i_avm_address[z][AWIDTH-1:AWIDTH-DIMM_W] == z0;   
    end
  end
  else begin : GEN_SINGLE_DIMM
    for(z0=0; z0<NUM_DIMM; z0=z0+1) begin : GEN_
      assign backpressure_stall[z][z0] = i_avm_waitrequest[z0];
    end
  end
  assign backpressure[z] = |backpressure_stall[z];
  assign i_avm_request[z] = i_avm_read[z] | i_avm_write[z];
end
if(READ) begin : ENABLE_READ    
  // Pass the token if the current LSU has no requests to issue or the LSU has reached its MAX_REQUEST allotment. If using SWDIMM, also pass the token when stalled to allow another LSU to access a potentially other free bank.
  // In SWDIMM we get a per-bank stall. In bank-interleaving, we only get a stall on bit-0, which is an OR of the stalls from all banks (this is OK to do because interleaving distributes even traffic to all banks).
  assign pass_token = !i_avm_request[ptr] | (SWDIMM_ENABLED & backpressure[ptr]) | (request_counter[MAX_REQUEST_W] & avm_read_wire);
  for(z=0; z<NUM_PORT; z=z+1) begin : GEN_I_DATA  
    assign i_id[z] = START_ID + z;
    assign i_req[z] = {i_avm_address[z], i_avm_burstcount[z], i_id[z]};   
    assign {o_avm_address, o_avm_burstcount, o_id} = request_dout;
    assign o_avm_byteenable = {MWIDTH_BYTES{1'b1}};    
  end
end
else begin : ENABLE_WRITE 
  assign pass_token = active & (!i_avm_request[ptr] | backpressure[ptr] & wr_burst_done);
  for(z=0; z<NUM_PORT; z=z+1) begin : GEN_I_DATA         
    assign i_id[z] = START_ID + z;      
    assign i_req[z] = {i_avm_address[z], i_avm_burstcount[z], i_avm_writedata[z], i_avm_byteenable[z], i_id[z]};                     
    assign {o_avm_address, o_avm_burstcount, o_avm_writedata, o_avm_byteenable, o_id} = request_dout;        
  end
  always @(posedge clk or negedge aclrn) begin 
    if(!aclrn) begin 
      wr_cnt <= 1;
      wr_burst_done <= 1'b1;
    end    
    else begin
      if(avm_write_wire) begin
        wr_cnt <= (wr_cnt == i_avm_burstcount[ptr])? 1 : wr_cnt + 1'b1;     
        if(!o_avm_waitrequest[ptr] & wr_cnt == i_avm_burstcount[ptr] & backpressure[ptr])  wr_burst_done <= 1'b1;  // make sure that token is not passed to the next LSU until the burst write is fully processed for the current one       
        else wr_burst_done <= 1'b0;                   
      end
      else if(!backpressure[ptr] &  i_token) begin
        wr_burst_done  <= 1'b0;
      end
      // synchronous reset (these assignments override the assignments above)      
      if(!sclrn[0]) begin 
        wr_cnt <= 1;
        wr_burst_done <= 1'b1;
      end        
    end
  end // end always
end

reg  [REQ_WIDTH-1:0] R_req;
assign request_dout = R_req;
assign o_avm_read = avm_read_p;
assign o_avm_write = avm_write_p;
assign o_avm_request = avm_request_p;
always @(posedge clk) begin
  R_req <= i_req[ptr];  // N-to-1 MUX
end

endgenerate

endmodule
`default_nettype wire