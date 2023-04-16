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


`default_nettype none

module acl_atomics
(
   clock, resetn,

   // arbitration port
   mem_arb_enable, //not used
   mem_arb_read,
   mem_arb_write,
   mem_arb_burstcount,
   mem_arb_address,
   mem_arb_writedata,
   mem_arb_byteenable,
   mem_arb_waitrequest,
   mem_arb_readdata,
   mem_arb_readdatavalid,
   mem_arb_writeack,

   //  Avalon port
   mem_avm_enable, //not used
   mem_avm_read,
   mem_avm_write,
   mem_avm_burstcount,
   mem_avm_address,
   mem_avm_writedata,
   mem_avm_byteenable,
   mem_avm_waitrequest,
   mem_avm_readdata,
   mem_avm_readdatavalid,
   mem_avm_writeack
);

/*************
* Parameters *
*************/

// default parameters if host does not specify them
parameter ADDR_WIDTH=27;         // Address width
parameter DATA_WIDTH=256;        // Data width
parameter BURST_WIDTH=6;
parameter BYTEEN_WIDTH=32;
parameter OPERATION_WIDTH=32; // atomic operations are ALL 32-bit
parameter ATOMIC_OP_WIDTH=3;

parameter LATENCY=4096; // assuming we get a response back in this many cycles

parameter ASYNC_RESET=1;        // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
parameter SYNCHRONIZE_RESET=0;  // set to '1' to pass the incoming reset signal through a synchronizer before use

//FIXME -- not consumed yet Case:539698
parameter USED_ATOMIC_OPERATIONS=8'b11111111; // atomics operations used in kernel

/******************
* Local Variables *
******************/

localparam LATENCY_BITS=$clog2(LATENCY);
localparam DATA_WIDTH_BITS=$clog2(DATA_WIDTH/8);    //number of bits needed to indicate which byte we want within a 512-bit memory word

// these should match ACLIntrinsics::ID enum in ACLIntrinsics.h
localparam a_ADD=0;
localparam a_XCHG=1;
localparam a_CMPXCHG=2;
localparam a_MIN=3;
localparam a_MAX=4;
localparam a_AND=5;
localparam a_OR=6;
localparam a_XOR=7;
//no support for subtract, increment, decrement -> these can be transformed into add

// atomic operation states
localparam a_IDLE=0;
localparam a_STARTED=1;
localparam a_READCOMPLETE=2;
localparam a_WRITEBACK=3;

/********
* Ports *
********/

// Standard global signals
input logic clock;
input logic resetn;

// Arbitration port
input logic mem_arb_enable;
input logic mem_arb_read;
input logic mem_arb_write;
input logic [BURST_WIDTH-1:0] mem_arb_burstcount;
input logic [ADDR_WIDTH-1:0] mem_arb_address;
input logic [DATA_WIDTH-1:0] mem_arb_writedata;
input logic [BYTEEN_WIDTH-1:0] mem_arb_byteenable;
output logic mem_arb_waitrequest;
output logic [DATA_WIDTH-1:0] mem_arb_readdata;
output logic mem_arb_readdatavalid;
output logic mem_arb_writeack;

// Avalon port
output logic mem_avm_enable;
output logic mem_avm_read;
output logic mem_avm_write;
output logic [BURST_WIDTH-1:0] mem_avm_burstcount;
output logic [ADDR_WIDTH-1:0] mem_avm_address;
output logic [DATA_WIDTH-1:0] mem_avm_writedata;
output logic [BYTEEN_WIDTH-1:0] mem_avm_byteenable;
input logic mem_avm_waitrequest;
input logic [DATA_WIDTH-1:0] mem_avm_readdata;
input logic mem_avm_readdatavalid;
input logic mem_avm_writeack;

/**********
* Signals *
**********/

wire atomic;
reg [OPERATION_WIDTH-1:0] a_operand0;
reg [OPERATION_WIDTH-1:0] a_operand1;
reg [DATA_WIDTH_BITS-1:0] a_segment_address;
wire [DATA_WIDTH_BITS-3:0] a_segment_address_32_aligned;
reg [ATOMIC_OP_WIDTH-1:0] a_atomic_op;
reg [BURST_WIDTH-1:0] a_burstcount;

reg [7:0] counter;

wire atomic_read_ready; // high when memory is ready to receive to a read request 
wire atomic_write_ready; // high when memory is ready to receive to a write request

reg [OPERATION_WIDTH-1:0] a_masked_readdata; 
wire [OPERATION_WIDTH-1:0] atomic_add_out;
wire [OPERATION_WIDTH-1:0] atomic_xchg_out;
wire [OPERATION_WIDTH-1:0] atomic_cmpxchg_out;
wire [OPERATION_WIDTH-1:0] atomic_min_out;
wire [OPERATION_WIDTH-1:0] atomic_max_out;
wire [OPERATION_WIDTH-1:0] atomic_and_out;
wire [OPERATION_WIDTH-1:0] atomic_or_out;
wire [OPERATION_WIDTH-1:0] atomic_xor_out;
wire [OPERATION_WIDTH-1:0] atomic_out;

// count responses/requests so we can determine when the atomic load is returned
reg [LATENCY_BITS-1:0] count_requests;
reg [LATENCY_BITS-1:0] count_responses;
reg [LATENCY_BITS-1:0] count_atomic;
wire waiting_atomic_read_response;
wire atomic_read_response;

// keep track of atomic operation state
reg [1:0] atomic_state;
wire atomic_idle;
wire atomic_started;
wire atomic_read_complete;
wire atomic_writeback;

// current atomic operation we are servicing
reg [DATA_WIDTH-1:0] a_writedata;
reg [BYTEEN_WIDTH-1:0] a_byteenable;
reg [ADDR_WIDTH-1:0] a_address;


logic aclrn;
logic sclrn;
acl_reset_handler #(
    .ASYNC_RESET            (ASYNC_RESET),
    .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
    .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
    .PIPE_DEPTH             (1),
    .NUM_COPIES             (1)
) acl_reset_handler_inst (
    .clk                    (clock),
    .i_resetn               (resetn),
    .o_aclrn                (aclrn),
    .o_sclrn                (sclrn),
    .o_resetn_synchronized  ()
);


/************************************
* Read Atomic Inputs, Write Output *
************************************/

always@(posedge clock or negedge aclrn) begin
    if ( !aclrn ) begin
        a_writedata <= {DATA_WIDTH{1'b0}};
        a_byteenable <= {BYTEEN_WIDTH{1'b0}};
        a_address <= {ADDR_WIDTH{1'b0}}; 
        a_operand0 <= {OPERATION_WIDTH{1'b0}};
        a_operand1 <= {OPERATION_WIDTH{1'b0}};
        a_segment_address <= {DATA_WIDTH_BITS{1'b0}};
        a_atomic_op <= {ATOMIC_OP_WIDTH{1'b0}};
        a_burstcount <= {BURST_WIDTH{1'b0}};
    end
    else begin
        if( atomic_read_ready ) begin
            //the bit mapping needs to match avm_writedata from the lsu_atomic_pipelined module in lsu_atomic.v which is shown below, assume WIDTH=32
            //for reference, it should also match acl_atomics_nostall.v
            a_operand0 <= mem_arb_writedata[1 +: OPERATION_WIDTH];                         //32:1
            a_operand1 <= mem_arb_writedata[OPERATION_WIDTH+1 +: OPERATION_WIDTH];         //64:2
            a_atomic_op <= mem_arb_writedata[2*OPERATION_WIDTH+1 +: ATOMIC_OP_WIDTH];      //hitorically this was 70:65, but ATOMIC_OP_WIDTH changed from 6 to now 3, so the bit range is now 67:65
            a_segment_address <= mem_arb_writedata[2*OPERATION_WIDTH+ATOMIC_OP_WIDTH+1 +: DATA_WIDTH_BITS]; //historically this was 75:71 and it was an address for a 32-bit word...
                                                                                                //...now we get a byte address on bits 73:68, but we can ignore 69:68 since atomics are 32-bit aligned
            a_byteenable <= mem_arb_byteenable;
            a_address <= mem_arb_address;
            a_burstcount <= mem_arb_burstcount;
        end
        else if( atomic_read_complete ) begin
            a_writedata <= ( atomic_out << (32*a_segment_address_32_aligned) );
        end
        else if( atomic_write_ready ) begin
            a_writedata <= {DATA_WIDTH{1'b0}};
        end
        if ( !sclrn ) begin
            a_writedata <= {DATA_WIDTH{1'b0}};
            a_byteenable <= {BYTEEN_WIDTH{1'b0}};
            a_address <= {ADDR_WIDTH{1'b0}}; 
            a_operand0 <= {OPERATION_WIDTH{1'b0}};
            a_operand1 <= {OPERATION_WIDTH{1'b0}};
            a_segment_address <= {DATA_WIDTH_BITS{1'b0}};
            a_atomic_op <= {ATOMIC_OP_WIDTH{1'b0}};
            a_burstcount <= {BURST_WIDTH{1'b0}};
        end
    end
end
assign a_segment_address_32_aligned = a_segment_address[DATA_WIDTH_BITS-1:2];   //indicates which 32-bit word inside the 512-bit memory word we are interested in, multiply by 32 to use as an index in a 512-bit vector

/*******************************
* Extract inputs from writedata *
********************************/

assign atomic = mem_arb_read & mem_arb_writedata[0:0];

/*********************************
* Arbitration/Avalon connections *
*********************************/

// mask the response
assign mem_arb_readdatavalid = mem_avm_readdatavalid;
assign mem_arb_writeack =  mem_avm_writeack;
assign mem_arb_readdata = mem_avm_readdata;

// dont send a new read/write if servicing an atomic
assign mem_avm_read = ( mem_arb_read & atomic_idle );
assign mem_avm_write = ( mem_arb_write & atomic_idle ) | atomic_writeback;
assign mem_avm_burstcount = atomic_writeback ? a_burstcount : mem_arb_burstcount;
assign mem_avm_address = atomic_writeback ? a_address : mem_arb_address;
assign mem_avm_writedata = atomic_writeback ? a_writedata : mem_arb_writedata;
assign mem_avm_byteenable = atomic_writeback ? a_byteenable : mem_arb_byteenable;
assign mem_avm_enable = mem_arb_enable;

// stall IC 1. stalled by memory
//          2. when servicing an atomic
assign mem_arb_waitrequest = mem_avm_waitrequest | atomic_started | atomic_read_complete | atomic_writeback;

/****************************************
* Keep track of atomic operation states *
****************************************/

always@(posedge clock or negedge aclrn) begin
    if ( !aclrn ) begin
        atomic_state <= a_IDLE;
    end
    else begin
        if( atomic_idle & atomic_read_ready ) atomic_state <= a_STARTED;
        else if( atomic_started & atomic_read_response ) atomic_state <= a_READCOMPLETE;
        else if( atomic_read_complete ) atomic_state <= a_WRITEBACK;
        else if( atomic_writeback & atomic_write_ready ) atomic_state <= a_IDLE;
        if ( !sclrn ) begin
            atomic_state <= a_IDLE;
        end
    end
end

assign atomic_idle = ( atomic_state == a_IDLE );
assign atomic_started = ( atomic_state == a_STARTED );
assign atomic_read_complete = ( atomic_state == a_READCOMPLETE );
assign atomic_writeback = ( atomic_state == a_WRITEBACK );

assign atomic_read_ready = ( atomic & atomic_idle & ~mem_avm_waitrequest );
assign atomic_write_ready = ( atomic_writeback & ~mem_avm_waitrequest );

/****************************
* ALU for atomic operations *
****************************/

// read readdata from memory
always@(posedge clock or negedge aclrn) begin
    if ( !aclrn ) begin
        a_masked_readdata <= {OPERATION_WIDTH{1'bx}};
    end
    else begin
        a_masked_readdata <= mem_avm_readdata[(32*a_segment_address_32_aligned) +: OPERATION_WIDTH];
        //sclrn not needed
    end
end

assign atomic_add_out = a_masked_readdata + a_operand0;
assign atomic_xchg_out = a_operand0;
assign atomic_cmpxchg_out = ( a_masked_readdata == a_operand0 ) ? a_operand1 : a_masked_readdata;
assign atomic_min_out = ( a_masked_readdata < a_operand0 ) ? a_masked_readdata : a_operand0;
assign atomic_max_out = ( a_masked_readdata > a_operand0 ) ? a_masked_readdata : a_operand0;
assign atomic_and_out = ( a_masked_readdata & a_operand0 );
assign atomic_or_out = ( a_masked_readdata | a_operand0 );
assign atomic_xor_out = ( a_masked_readdata ^ a_operand0 );

// select the output based on atomic operation
assign atomic_out = 
      ( a_atomic_op == a_ADD )     ? atomic_add_out :
      ( a_atomic_op == a_XCHG )    ? atomic_xchg_out :
      ( a_atomic_op == a_CMPXCHG ) ? atomic_cmpxchg_out :
      ( a_atomic_op == a_MIN )     ? atomic_min_out :
      ( a_atomic_op == a_MAX )     ? atomic_max_out :
      ( a_atomic_op == a_AND )     ? atomic_and_out :
      ( a_atomic_op == a_OR )      ? atomic_or_out  :
      ( a_atomic_op == a_XOR )     ? atomic_xor_out : {OPERATION_WIDTH{1'bx}};

/***********************
* Find atomic response *
***********************/

always@(posedge clock or negedge aclrn) begin
    // atomic response, clear counters
    if ( !aclrn ) begin
        count_requests <= { LATENCY_BITS{1'b0} };
        count_responses <= { LATENCY_BITS{1'b0} };
        count_atomic <= {1'b0};
    end
    else begin

        // new read request
        if( mem_avm_read & ~mem_avm_waitrequest ) begin
        count_requests <= count_requests + mem_arb_burstcount;
        end

        // new read response
        if( mem_avm_readdatavalid ) begin
        count_responses <= count_responses + 1;
        end

        // new read atomic request
        if( atomic_read_ready ) begin
        count_atomic <= count_requests;
        end

        if ( !sclrn ) begin
            count_requests <= { LATENCY_BITS{1'b0} };
            count_responses <= { LATENCY_BITS{1'b0} };
            count_atomic <= {1'b0};
        end
    end
end

// we already sent a request for an atomic
// the next response will be for atomic
assign waiting_atomic_read_response = atomic_started & ( count_atomic == count_responses );

assign atomic_read_response = ( waiting_atomic_read_response & mem_avm_readdatavalid );

endmodule

`default_nettype wire
