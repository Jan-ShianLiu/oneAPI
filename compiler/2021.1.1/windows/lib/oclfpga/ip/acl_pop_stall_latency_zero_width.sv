//// (c) 1992-2020 Intel Corporation.                            
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


`timescale 1ns/1ps
`default_nettype none

module acl_pop_stall_latency_zero_width #(
    //supported parameters from acl_pop
    parameter int DATA_WIDTH = 32,              //THIS PARAMETER IS IGNORED
    
    //unsupported parameters from acl_pop
    //parameter string STYLE = "COALESCE",      //only "COALESCE" is supported, "REGULAR" is not implemented
    //parameter COALESCE_DISTANCE = 1,          //this will now be handled with initial occupancy in the backedge fifo
    //parameter INF_LOOP = 0,
    
    //choice of hld_fifo style
    parameter bit ALLOW_HIGH_SPEED_FIFO_USAGE = 1,  //0 = mid speed fifo, 1 = high speed fifo
    
    //predication
    parameter bit NO_PREDICATION = 0,           //set to 1 if it is known that predicate_in = 0, unfortunately quartus doesn't propagate the constant 0 through the fifo so we will manually sweep it away
    
    //stall latency config - upstream
    parameter int UP_FIFO_DEPTH = 32,           //how deep is the fifo between upstream and sync block
    parameter int UP_ALMOST_FULL_CUTOFF = 0,    //for stall_out
    
    //stall latency config - downstream
    parameter int DOWN_FIFO_DEPTH = 32,         //how deep is the fifo between sync block and downstream
    parameter int DOWN_ALMOST_EMPTY_CUTOFF = 0, //for almost_empty_out
    parameter int DOWN_STALL_IN_EARLINESS = 0,  //for stall_in
    
    //reset configuration
    parameter bit ASYNC_RESET = 0,              // how do we use reset: 1 means registers are reset asynchronously, 0 means registers are reset synchronously
    parameter bit SYNCHRONIZE_RESET = 1,        // based on how reset gets to us, what do we need to do: 1 means synchronize reset before consumption (if reset arrives asynchronously), 0 means passthrough (managed externally)
    parameter bit RESET_EVERYTHING = 0,         // intended for partial reconfig debug, set to 1 to reset every register (normally async reset excludes data path and sync reset additionally excludes some control signals)
    //parameter bit RESET_EXTERNALLY_HELD = 1,  // we assume reset is held sufficiently long to flush the stall/empty/valid pipelines
    parameter enable_ecc = "FALSE"              // Enable error correction coding
) (
    input  wire                     clock,
    input  wire                     resetn,
    
    //input from upstream - fifo write side interface
    input  wire                     valid_in,                   //"forced write"
    input  wire    [DATA_WIDTH-1:0] data_in,
    output logic                    stall_out,                  //"almost full", threshold determined by UP_ALMOST_FULL_CUTOFF
    
    //input from upstream - control signals, on the same clock cycle as valid_in
    input  wire                     predicate_in,               //0 = normal operation, 1 = bypass this transaction (valid still gets sent to downstream, but no action otherwise)
    input  wire                     dir_in,                     //0 = consume from backedge fifo, 1 = consume from upstream input
    
    //input from backedge - in a naive implementation, this would have been a fifo write side interface
    //similar to channel fifo merging, we have optimized away the fifo between the backedge input the sync block, so we actually have a fifo read side interface
    input  wire                     feedback_empty_in,
    input  wire                     feedback_almost_empty_in,   //threshold determined by BACK_ALMOST_EMPTY_CUTOFF localparam, must exactly match value in acl_push_stall_latency
    input  wire    [DATA_WIDTH-1:0] feedback_data_in,
    output logic                    feedback_stall_out,         //earliness is set by STALL_IN_EARLINESS__BE localparam, must exactly match value in acl_push_stall_latency
    
    //output to downstream - fifo read side interface
    output logic                    valid_out,                  //indicates that there is a (non backpressureable) read happening from downstream fifo
    output logic                    empty_out,                  //advertise that downstream fifo cannot be read because it is empty
    output logic                    almost_empty_out,           //threshold determined by DOWN_ALMOST_EMPTY_CUTOFF
    output logic   [DATA_WIDTH-1:0] data_out,
    input  wire                     stall_in,                    //downstream provides backpressure early if DOWN_STALL_IN_EARLINESS >= 1
    output logic              [1:0] ecc_err_status               // ecc status signals
);

    
    acl_pop_stall_latency #(
        .DATA_WIDTH (0),
        .ALLOW_HIGH_SPEED_FIFO_USAGE (ALLOW_HIGH_SPEED_FIFO_USAGE),
        .NO_PREDICATION (NO_PREDICATION),
        .UP_FIFO_DEPTH (UP_FIFO_DEPTH),
        .UP_ALMOST_FULL_CUTOFF (UP_ALMOST_FULL_CUTOFF),
        .DOWN_FIFO_DEPTH (DOWN_FIFO_DEPTH),
        .DOWN_ALMOST_EMPTY_CUTOFF (DOWN_ALMOST_EMPTY_CUTOFF),
        .DOWN_STALL_IN_EARLINESS (DOWN_STALL_IN_EARLINESS),
        .ASYNC_RESET (ASYNC_RESET),
        .SYNCHRONIZE_RESET (SYNCHRONIZE_RESET),
        .RESET_EVERYTHING (RESET_EVERYTHING),
        .enable_ecc (enable_ecc)
    )
    acl_pop_stall_latency_inst
    (
        .clock (clock),
        .resetn (resetn),
        .valid_in (valid_in),
        .data_in (),            //unused
        .stall_out (stall_out),
        .predicate_in (predicate_in),
        .dir_in (dir_in),
        .feedback_empty_in (feedback_empty_in),
        .feedback_almost_empty_in (feedback_almost_empty_in),
        .feedback_data_in (),   //unused
        .feedback_stall_out (feedback_stall_out),
        .valid_out (valid_out),
        .empty_out (empty_out),
        .almost_empty_out (almost_empty_out),
        .data_out (),           //unused
        .stall_in (stall_in),
        .ecc_err_status (ecc_err_status)
    );
    
endmodule

`default_nettype wire
