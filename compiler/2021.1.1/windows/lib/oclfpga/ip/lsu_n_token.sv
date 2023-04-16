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


// A group of the basic unit lsu_ic_token
`default_nettype none
module lsu_n_token (
  clk,
  resetn,
  i_token,
  i_id,
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
  o_avm_burstcount,
  o_id,
  o_token,
  o_active,
  // write
  i_avm_writedata,
  o_avm_writedata
);

parameter AWIDTH=32;            // Address width (32-bits for Avalon)
parameter MWIDTH_BYTES=64;      // Width of the global memory bus (bytes)
parameter BURST_CNT_W=6;                             
parameter NUM_PORT = 10; 
parameter START_ID = 0;
parameter READ = 1;   
parameter ENABLE_FAST = 0;
parameter OPEN_RING = 0;  
parameter START_ACTIVE = 0;   
parameter NUM_DIMM = 1;    
parameter ENABLE_DATA_REORDER = 0;
parameter ROOT_FIFO_AW = 8;  
parameter ROOT_FIFO_THRESH = (NUM_PORT+START_ID)*2;
parameter ENABLE_LAST_WAIT = 0;                               
parameter ID_WIDTH = (NUM_PORT+START_ID == 1 )? 1 : $clog2(NUM_PORT+START_ID);
parameter SINGLE_STALL = 0;
parameter HYPER_PIPELINE = 0;                      // 1 = optimized, highly pipelined mode, only sclrs (no aclrs) at the expense of area.
parameter AVM_WRITE_DATA_LATENESS = 0;             // fmax and area optimization - run the write data path this many clocks later than stall/valid
parameter WIDE_DATA_SLICING = 0;                   // for large MWIDTH_BYTES, mux select signals are replicated for each data width section of WIDE_DATA_SLICING bits if nonzero
parameter MAX_REQUESTS_PER_LSU = 4;                    // Max # of requests before passing the token. Used only for reads. See lsu_ic_token/lsu_n_fast for additional comments.

localparam MWIDTH=8*MWIDTH_BYTES;
localparam REQ_WIDTH_READ  = AWIDTH + BURST_CNT_W + ID_WIDTH;          
localparam REQ_WIDTH_WRITE = AWIDTH + BURST_CNT_W + MWIDTH + MWIDTH_BYTES + ID_WIDTH;
localparam REQ_WIDTH = READ? REQ_WIDTH_READ : REQ_WIDTH_WRITE;
localparam ROOT_FIFO_DEPTH = 2 ** ROOT_FIFO_AW;
localparam NUM_DIMM_INT = SINGLE_STALL? 1 : NUM_DIMM;


input wire                       clk;  
// reset is consumed synchronously if HYPER_PIPELINE == 1, asynchronously otherwise.
// If HYPER_PIPELINE==1 it must be ensured that this module is held in reset longer than its parent. This can be done simply by
// feeding resetn with the output of the parent's reset pipeline.
input wire                       resetn;   
// from another open ring part
input wire    [ID_WIDTH-1:0]     i_id;
input wire                       i_token;
input wire    [AWIDTH-1:0]       i_ext_address;
input wire                       i_ext_read;        
input wire    [BURST_CNT_W-1:0]  i_ext_burstcount; 
output logic                     o_ext_waitrequest [NUM_DIMM];     
// from LSUs
input wire    [MWIDTH_BYTES-1:0] i_avm_byteenable  [NUM_PORT];     
input wire    [AWIDTH-1:0]       i_avm_address     [NUM_PORT];     
input wire                       i_avm_read        [NUM_PORT];     
input wire                       i_avm_write       [NUM_PORT];     
input wire    [BURST_CNT_W-1:0]  i_avm_burstcount  [NUM_PORT];  
// to LSUs
output logic                     o_avm_waitrequest [NUM_PORT];   
output logic                     o_token;
output logic                     o_active          [NUM_PORT];   
// from memory
input  wire                      i_avm_waitrequest [NUM_DIMM];       
// to memory 
output logic  [MWIDTH_BYTES-1:0] o_avm_byteenable;           
output logic  [AWIDTH-1:0]       o_avm_address;              
output logic                     o_avm_read;  
output logic                     o_avm_write;                
output logic  [BURST_CNT_W-1:0]  o_avm_burstcount;
output logic  [ID_WIDTH-1:0]     o_id;
// write
input wire    [MWIDTH-1:0]       i_avm_writedata [NUM_PORT];
output logic  [MWIDTH-1:0]       o_avm_writedata;

genvar z, z0;  
wire [ID_WIDTH-1:0]     ci_id             [NUM_PORT][2];            
wire [MWIDTH_BYTES-1:0] ci_byteenable     [NUM_PORT][2];      
wire [AWIDTH-1:0]       ci_address        [NUM_PORT][2];      
wire                    ci_read           [NUM_PORT][2];   
wire                    ci_write          [NUM_PORT][2];   
wire [BURST_CNT_W-1:0]  ci_burstcount     [NUM_PORT][2];       
wire                    ci_token          [NUM_PORT];
wire                    ci_waitrequest    [NUM_PORT][NUM_DIMM_INT]; 
wire                    co_waitrequest_0  [NUM_PORT][NUM_DIMM_INT]; 
wire                    co_waitrequest_1  [NUM_PORT]; 
wire [MWIDTH-1:0]       co_readdata       [NUM_PORT];
wire                    co_return_valid   [NUM_PORT][2];
wire [MWIDTH_BYTES-1:0] co_byteenable     [NUM_PORT];
wire [AWIDTH-1:0]       co_address        [NUM_PORT];
wire                    co_read           [NUM_PORT];
wire                    co_write          [NUM_PORT];
wire [BURST_CNT_W-1:0]  co_burstcount     [NUM_PORT];
wire [ID_WIDTH-1:0]     co_id             [NUM_PORT];
wire                    co_token          [NUM_PORT];
wire [ID_WIDTH-1:0]     co_index          [NUM_PORT];
wire [MWIDTH-1:0]       ci_writedata      [NUM_PORT][2];    
wire [MWIDTH-1:0]       co_writedata      [NUM_PORT];        

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

//delay the write signal so that the late data from the LSUs can catch up
//safe to just delay without considering backpressure because write root fifo has NEVER_OVERFLOWS=1 so this is really a forced write into the fifo, e.g. the backpressure has already been integrated into the write
//we have to do this here instead of just setting VALID_IN_EARLINESS=4 on the write root fifo because the burst count is consumed to generate a "last word in a burst" signal which is written into the write root fifo
logic early_avm_write;
logic [AVM_WRITE_DATA_LATENESS:0] delayed_avm_write; //index N is delayed by N clocks, so N=0 is original timing
genvar g;
generate
always_comb begin
  delayed_avm_write[0] = early_avm_write;
end
for (g=1; g<=AVM_WRITE_DATA_LATENESS; g++) begin : GEN_AVM_WRITE_DATA_LATENESS
  always_ff @(posedge clk) begin
    delayed_avm_write[g] <= delayed_avm_write[g-1];
  end
end
endgenerate
assign o_avm_write = delayed_avm_write[AVM_WRITE_DATA_LATENESS];  

generate  
    if(ENABLE_FAST) begin : GEN_ENABLE_FAST        
      wire i_avm_wait_int [NUM_DIMM_INT]; 
      if(NUM_DIMM_INT > 1) begin
        assign i_avm_wait_int = i_avm_waitrequest;          
      end
      else  begin
        assign i_avm_wait_int[0] = i_avm_waitrequest[0];        
      end
      lsu_n_fast #(
         .AWIDTH(AWIDTH),       
         .MWIDTH_BYTES(MWIDTH_BYTES),
         .BURST_CNT_W(BURST_CNT_W),
         .NUM_PORT(NUM_PORT),
         .START_ID(START_ID),      
         .ID_WIDTH(ID_WIDTH),
         .OPEN_RING(OPEN_RING),
         .NUM_DIMM(NUM_DIMM_INT),
         .ENABLE_DATA_REORDER(ENABLE_DATA_REORDER),
         .START_ACTIVE(START_ACTIVE|!OPEN_RING),
         .ENABLE_LAST_WAIT(ENABLE_LAST_WAIT),               
         .READ(READ),
         .HYPER_PIPELINE(HYPER_PIPELINE),
         .MAX_REQUESTS_PER_LSU(MAX_REQUESTS_PER_LSU)
       ) lsu_n_fast (
        .clk              (clk),
        .resetn           (resetn_synchronized),
        .i_ext_read       (1'b0), 
        .i_avm_write      (i_avm_write),
        .i_token          (i_token),
        .i_avm_address    (i_avm_address),
        .i_avm_read       (i_avm_read),
        .i_avm_burstcount (i_avm_burstcount),
        .o_avm_waitrequest(o_avm_waitrequest),
        .i_avm_waitrequest(i_avm_wait_int),
        .o_avm_address    (o_avm_address),
        .o_avm_read       (o_avm_read),
        .o_avm_burstcount (o_avm_burstcount),
        .o_token          (o_token),
        .o_id             (o_id)
      );           
    end
    else begin : GEN_DISABLE_FAST 
      assign o_avm_byteenable   = co_byteenable  [NUM_PORT-1];
      assign o_avm_address      = co_address     [NUM_PORT-1];
      assign o_avm_read         = co_read        [NUM_PORT-1];
      assign early_avm_write    = co_write       [NUM_PORT-1];
      assign o_avm_burstcount   = co_burstcount  [NUM_PORT-1];  
      assign o_id               = co_id          [NUM_PORT-1];
      assign o_token            = co_token       [NUM_PORT-1];
      assign o_avm_writedata    = co_writedata   [NUM_PORT-1];    
      if(NUM_DIMM_INT > 1) begin
        assign ci_waitrequest[NUM_PORT-1] = i_avm_waitrequest;  
        assign o_ext_waitrequest = co_waitrequest_0[0]; 
      end
      else  begin
        assign ci_waitrequest[NUM_PORT-1][0] = i_avm_waitrequest[0];      
        assign o_ext_waitrequest[0] = co_waitrequest_0[0][0];    
      end                  
                
      for(z=0; z<NUM_PORT; z=z+1) begin : GEN_IC
        if(z==0) begin   
          assign ci_token[z] = OPEN_RING? i_token : co_token [NUM_PORT - 1]; 
          assign ci_read       [z][0] = OPEN_RING? i_ext_read : 1'b0;                
          assign ci_write      [z][0] = 1'b0;
          assign ci_id         [z][0] = i_id;
          assign ci_byteenable [z][0] = '1;      
          assign ci_address    [z][0] = i_ext_address;      
          assign ci_burstcount [z][0] = i_ext_burstcount;    
          assign ci_read       [z][1] = i_avm_read [0];        
          assign ci_write      [z][1] = i_avm_write[0];            
          assign ci_id         [z][1] = START_ID;
          assign ci_byteenable [z][1] = i_avm_byteenable [0];      
          assign ci_address    [z][1] = i_avm_address    [0];      
          assign ci_burstcount [z][1] = i_avm_burstcount [0]; 
          assign ci_writedata  [z][1] = i_avm_writedata  [0];        
        end
        else begin
          assign ci_token[z] = co_token [z-1]; 
          assign ci_read       [z][0] = co_read          [z-1];   
          assign ci_write      [z][0] = co_write         [z-1]; 
          assign ci_id         [z][0] = co_id            [z-1];
          assign ci_byteenable [z][0] = co_byteenable    [z-1];
          assign ci_address    [z][0] = co_address       [z-1];        
          assign ci_burstcount [z][0] = co_burstcount    [z-1];   
          assign ci_writedata  [z][0] = co_writedata     [z-1];            
          assign ci_id         [z][1] = START_ID+z;
          assign ci_byteenable [z][1] = i_avm_byteenable [z];      
          assign ci_address    [z][1] = i_avm_address    [z];      
          assign ci_read       [z][1] = i_avm_read       [z];      
          assign ci_write      [z][1] = i_avm_write      [z];      
          assign ci_burstcount [z][1] = i_avm_burstcount [z];      
          assign ci_writedata  [z][1] = i_avm_writedata  [z];            
        end           
        assign o_avm_waitrequest[z] = co_waitrequest_1[z];   
        if(z < NUM_PORT - 1)  assign ci_waitrequest[z] = co_waitrequest_0[z+1];           
              
        lsu_ic_token #(
          .AWIDTH(AWIDTH),
          .START_ACTIVE(z==0 & (START_ACTIVE|!OPEN_RING)),
          .ENABLE_DATA_REORDER(ENABLE_DATA_REORDER),
          .MWIDTH_BYTES(MWIDTH_BYTES),
          .READ(READ),   
          .ENABLE_LAST_WAIT(ENABLE_LAST_WAIT & (z == NUM_PORT - 1)),
          .ID_WIDTH(ID_WIDTH),
          .NUM_DIMM(NUM_DIMM_INT),
          .BURST_CNT_W(BURST_CNT_W),
          .HYPER_PIPELINE(HYPER_PIPELINE),
          .AVM_WRITE_DATA_LATENESS(AVM_WRITE_DATA_LATENESS),
          .WIDE_DATA_SLICING(WIDE_DATA_SLICING),
          .MAX_REQUESTS_PER_LSU(MAX_REQUESTS_PER_LSU)
        ) ic (
          .clk(clk),                      
          .resetn(resetn_synchronized),
          .i_token(ci_token[z]),
          .i_id(ci_id[z]),                    
          .i_avm_waitrequest(ci_waitrequest[z]),          
          .i_avm_byteenable(ci_byteenable[z]),         
          .i_avm_address(ci_address[z]),            
          .i_avm_read(ci_read[z]),            
          .i_avm_write(ci_write[z]),             
          .i_avm_burstcount(ci_burstcount[z]),      
          .i_avm_writedata(ci_writedata[z]),   
          .o_avm_writedata(co_writedata[z]),    
          .o_avm_waitrequest_0(co_waitrequest_0[z]),        
          .o_avm_waitrequest_1(co_waitrequest_1[z]),        
          .o_avm_byteenable(co_byteenable[z]),         
          .o_avm_address(co_address[z]),            
          .o_avm_read(co_read[z]),          
          .o_avm_write(co_write[z]),            
          .o_avm_burstcount(co_burstcount[z]),    
          .o_token(co_token[z]),    
          .o_index(co_index[z]),
          .o_active(o_active[z]),
          .o_id(co_id[z])
        );        
      end // end GEN_MULTIPLE_PORT   
      `ifdef SIM_ONLY
          reg burst_done;
          reg [BURST_CNT_W-1:0] cnt;
          reg [AWIDTH-1:0]  R_address;
          reg error;         
          if(READ == 0) begin : GEN_WRITE_DEBUG
            always @(posedge clk or negedge aclrn) begin
              if(!aclrn) begin
                cnt <= 1;
                burst_done <= 1'b0;  
                error <= 1'b0; 
              end
              else begin                
                if(o_avm_write) begin
                  R_address <= o_avm_address;
                  cnt <= (cnt == o_avm_burstcount)? 1 : cnt + 1;
                  burst_done <= cnt == o_avm_burstcount;
                  error <= (R_address != o_avm_address) & !burst_done;
                  if(error) begin
                    $display("write ring error");
                    $stop;
                  end
                end
                // synchronous reset (these assignments override the assignments above)
                if(!sclrn[0]) begin
                  cnt <= 1;
                  burst_done <= 1'b0;  
                  error <= 1'b0; 
                end                
              end      
            end      
          end
        `endif   
    end
endgenerate

endmodule
`default_nettype wire