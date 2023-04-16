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


//  This module implements the "pop" of push/pop, a 2:1 mux that selects upstream input or backedge loop input, and sends this to downstream
//  Only the "COALESCE" style from acl_pop is implemented, which means initial occupancy is required in the backedge fifo
//  All interfaces use the stall latency protocol
//  Pop is similar to iord in that it uses a sync block, beware the latency through sync is not deterministic because there is slow read mode and fast read mode

//  Associated files:
//  - Stall latency: acl_push_stall_latency.sv acl_pop_stall_latency.sv acl_sync.sv acl_desync.sv
//  - FIFO         : hld_fifo.sv acl_zero_latency_fifo.sv acl_low_latency_fifo.sv acl_high_speed_fifo.sv acl_tessellated_incr_decr_threshold.sv acl_tessellated_incr_lookahead.sv acl_lfsr.sv
//  - Reset control: acl_reset_handler.sv acl_fanout_pipeline.sv acl_std_synchronizer_nocut.v

//              +---- POP IP ----+
//              |                |
//              |  UpstreamFifo  |
//                      |        |
//          +-------- Sync       |
//          |           |        |
//          |   | DownstreamFifo |
//          |   |       |        |
//          |   +-----  |  ------+
//          |           |
//          |   +-----  |  ------+
//          |   |       |        |
//          |   |   DataPath     |
//          |   |       |        |
//          |   +-----  |  ------+
//          |           |
//          |   +-----  |  ----- PUSH IP -------+
//          |   |       |                       |
//          |   |  UpstreamFifo                 |
//          |   |       |                       |
//          |   |    Desync ------------+       |
//          |   |       |               |       |
//          |   | DownstreamFifo   BackedgeFifo |
//          |   |                       |       |
//          |   +---------------------  |  -----+
//          |                           |
//          +---------------------------+
//    
//  Total roundtrip latency in steady state (not considering slow read mode of Sync) = 19 + DataPath latency
//        
//  Delay element      | Parameter that governs this delay        | Default value
//  -------------------+------------------------------------------+--------------
//  POP.Sync           | acl_pop_stall_latency.BACK_DATA_LATENCY  | 2
//  POP.DownstreamFifo | write to read latency of high speed fifo | 5
//  DataPath           | N/A                                      | N/A
//  PUSH.UpstreamFifo  | write to read latency of high speed fifo | 5
//  PUSH.Desync        | acl_push_stall_latency.BACK_DATA_LATENCY | 2
//  PUSH.BackedgeFifo  | write to read latency of high speed fifo | 5

`timescale 1ns/1ps
`default_nettype none

module acl_pop_stall_latency #(
    //supported parameters from acl_pop
    parameter int DATA_WIDTH = 32,
    
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

    //stall latency parameters for sync -- localparams for now, these can later be exposed as parameters once the compiler has a reasonable method of setting them
    localparam int DOWN_FULL_LATENCY = 2;       //almost full, downstream to central
    localparam int UP_EMPTY_LATENCY = 2;        //empty/almost empty, upstream to central
    localparam int BACK_EMPTY_LATENCY = 2;      //empty/almost empty, backedge to central
    localparam int UP_STALL_LATENCY = 5;        //stall, central to upstream
    localparam int BACK_STALL_LATENCY = 5;      //stall, central to backedge
    localparam int UP_STALL_IN_EARLINESS = 3;   //stall in earliness of upstream fifo, cannot exceed UP_STALL_LATENCY
    localparam int BACK_STALL_IN_EARLINESS = 3; //stall in earliness of backedge fifo, cannot exceed BACK_STALL_LATENCY -- must exactly match the value in acl_push_stall_latency
    localparam int UP_DATA_LATENCY = 2;         //data latency, upstream to downstream
    localparam int BACK_DATA_LATENCY = 2;       //data latency, backedge to downstream
    //IMPORANT: UP_STALL_LATENCY + UP_DATA_LATENCY must be equal to BACK_STALL_LATENCY + BACK_DATA_LATENCY so that data arrives at downstream on the same clock cycle
    
    //the values UP_EMPTY_LATENCY, BACK_EMPTY_LATENCY, UP_STALL_LATENCY, and BACK_STALL_LATENCY can be set independently
    //the waiting period used by the fast read/slow read state machine inside sync must use the larger of the two round-trip latencies
    localparam int UP_EMPTY_PLUS_STALL_LATENCY = UP_EMPTY_LATENCY + UP_STALL_LATENCY;
    localparam int BACK_EMPTY_PLUS_STALL_LATENCY = BACK_EMPTY_LATENCY + BACK_STALL_LATENCY;
    localparam int EMPTY_PLUS_STALL_LATENCY = (UP_EMPTY_PLUS_STALL_LATENCY > BACK_EMPTY_PLUS_STALL_LATENCY) ? UP_EMPTY_PLUS_STALL_LATENCY : BACK_EMPTY_PLUS_STALL_LATENCY;
    
    //sanity check on fifo sizes
    localparam int UP_MIN_FIFO_DEPTH = UP_ALMOST_FULL_CUTOFF + 1;
    localparam int UP_REAL_FIFO_DEPTH = (UP_FIFO_DEPTH < UP_MIN_FIFO_DEPTH) ? UP_MIN_FIFO_DEPTH : UP_FIFO_DEPTH;
    localparam int DOWN_ALMOST_FULL_CUTOFF = DOWN_FULL_LATENCY + UP_STALL_LATENCY + UP_DATA_LATENCY;
    localparam int DOWN_MIN_FIFO_DEPTH = DOWN_ALMOST_FULL_CUTOFF + 1;
    localparam int DOWN_REAL_FIFO_DEPTH = (DOWN_FIFO_DEPTH < DOWN_MIN_FIFO_DEPTH) ? DOWN_MIN_FIFO_DEPTH : DOWN_FIFO_DEPTH;
    
    
    //signals before and after the sync block
    logic [DATA_WIDTH:0] before_data_up, before_data_back;
    logic [1:0] before_empty, before_stall, before_almost_empty;
    logic before_predicate_data, before_predicate_stall;
    logic [2*DATA_WIDTH+1:0] before_data, after_data;
    logic [DATA_WIDTH-1:0] after_data_up, after_data_back, after_data_mux;
    logic after_data_dir;
    logic after_valid, after_stall;
    logic [1:0] ecc_err_status_0, ecc_err_status_1, ecc_err_status_2;
    
    //upstream fifo: from upstream to sync
    hld_fifo
    #(
        //fifo config
        .WIDTH                          (DATA_WIDTH + 1),   //data_in and dir_in
        .DEPTH                          (UP_REAL_FIFO_DEPTH),
        .ALMOST_EMPTY_CUTOFF            (UP_EMPTY_PLUS_STALL_LATENCY),
        .ALMOST_FULL_CUTOFF             (UP_ALMOST_FULL_CUTOFF),
        
        //reset config
        .ASYNC_RESET                    (ASYNC_RESET),
        .SYNCHRONIZE_RESET              (SYNCHRONIZE_RESET),
        .RESET_EVERYTHING               (RESET_EVERYTHING),
        .RESET_EXTERNALLY_HELD          (1),
        
        //special config
        .REGISTERED_DATA_OUT_COUNT      (0),
        .NEVER_OVERFLOWS                (1),                //upstream is a stall latency interface, trusting that it integrates early backpressure into valid_in
        .STALL_IN_EARLINESS             (UP_STALL_IN_EARLINESS),
        .VALID_IN_EARLINESS             (0),
        .HOLD_DATA_OUT_WHEN_EMPTY       (0),
        .INITIAL_OCCUPANCY              (0),
        
        //interface selection
        .USE_STALL_LATENCY_UPSTREAM     (1),
        .USE_STALL_LATENCY_DOWNSTREAM   (1),
        
        //fifo selection
        .STYLE                          (ALLOW_HIGH_SPEED_FIFO_USAGE ? "hs" : "ms"),

        .enable_ecc                     (enable_ecc)
    )
    upstream_fifo
    (
        .clock                          (clock),
        .resetn                         (resetn),
        
        //upstream - from upstream
        .i_valid                        (valid_in),     //forced write
        .i_data                         ({dir_in, data_in}),
        .o_stall                        (stall_out),
        
        //downstream - to sync
        .o_data                         (before_data_up),
        .i_stall                        (before_stall[0]),
        .o_empty                        (before_empty[0]),
        .o_almost_empty                 (before_almost_empty[0]),

        .ecc_err_status                 (ecc_err_status_0)
    );
    
    //predicate_fifo and upstream_fifo are written to at the same time, but read at different times (predicate fifo is always read earlier or at the same time as upstream_fifo)
    generate
    if (NO_PREDICATION) begin
        assign before_predicate_data = '0;
        assign ecc_err_status_1 = '0;
    end
    else begin
        hld_fifo
        #(
            //fifo config
            .WIDTH                          (1),
            .DEPTH                          (UP_FIFO_DEPTH),
            .ALMOST_EMPTY_CUTOFF            (0),
            .ALMOST_FULL_CUTOFF             (0),
            
            //reset config
            .ASYNC_RESET                    (ASYNC_RESET),
            .SYNCHRONIZE_RESET              (SYNCHRONIZE_RESET),
            .RESET_EVERYTHING               (RESET_EVERYTHING),
            .RESET_EXTERNALLY_HELD          (1),
            
            //special config
            .REGISTERED_DATA_OUT_COUNT      (1),        //register the predicate, it acts as a mask for the can_fast_read and can_slow_read
            .NEVER_OVERFLOWS                (1),        //okay since forced write is guarded
            .STALL_IN_EARLINESS             (0),
            .VALID_IN_EARLINESS             (0),
            .HOLD_DATA_OUT_WHEN_EMPTY       (0),
            .INITIAL_OCCUPANCY              (0),
            
            //interface selection
            .USE_STALL_LATENCY_UPSTREAM     (1),        //set to 1 so that i_valid is forced write
            .USE_STALL_LATENCY_DOWNSTREAM   (1),
            
            //fifo selection
            .STYLE                          (ALLOW_HIGH_SPEED_FIFO_USAGE ? "hs" : "ms"),

            .enable_ecc                     (enable_ecc)
        )
        predicate_fifo
        (
            .clock                          (clock),
            .resetn                         (resetn),
            
            //upstream - from backedge
            .i_valid                        (valid_in),     //forced write
            .i_data                         (predicate_in),
            
            //downstream - to sync
            .o_data                         (before_predicate_data),
            .i_stall                        (before_predicate_stall),

            .ecc_err_status                 (ecc_err_status_1)
        );
    end
    endgenerate
    
    
    //no need for backedge fifo, we already have a fifo read interface
    assign before_data_back = {1'b0, feedback_data_in}; //dir_in is carried in the msb of before_data_up, no corresponding control signal to carry in backedge but widths need to match
    assign before_almost_empty[1] = feedback_almost_empty_in;
    assign before_empty[1] = feedback_empty_in;
    assign feedback_stall_out = before_stall[1];
    
    
    //stall latency implementation of sync
    assign before_data = {before_data_back, before_data_up};
    acl_sync_predicate_nonblocking
    #(
        //fifo config
        .NUM_IFACES                     (2),
        .DATA_WIDTH                     (DATA_WIDTH + 1),   //per interface
        
        //reset config
        .ASYNC_RESET                    (ASYNC_RESET),
        .SYNCHRONIZE_RESET              (SYNCHRONIZE_RESET),
        .RESET_EVERYTHING               (RESET_EVERYTHING),
        
        //stall latency fast read/slow read state machine config
        .EMPTY_PLUS_STALL_LATENCY       (EMPTY_PLUS_STALL_LATENCY),
        
        //stall latency pipelining config
        .EMPTY_LATENCY                  ({BACK_EMPTY_LATENCY, UP_EMPTY_LATENCY}),
        .STALL_LATENCY                  ({BACK_STALL_LATENCY, UP_STALL_LATENCY}),
        .DATA_LATENCY                   ({BACK_DATA_LATENCY, UP_DATA_LATENCY}),
        .FULL_LATENCY                   (DOWN_FULL_LATENCY),
        .STALL_IN_EARLINESS             ({BACK_STALL_IN_EARLINESS, UP_STALL_IN_EARLINESS}),
        .VALID_IN_EARLINESS             (0),
        
        //special config
        .NON_BLOCKING                   (0)
    )
    sync_inst
    (
        .clock                          (clock),
        .resetn                         (resetn),
        
        //upstream multiple fifo read interfaces
        .i_data                         (before_data),
        .i_almost_empty                 (before_almost_empty),
        .i_empty                        (before_empty),
        .o_stall                        (before_stall),
        
        //predicate fifo read interface
        .i_predicate_data               (before_predicate_data),
        .o_predicate_stall              (before_predicate_stall),
        
        //downstream single fifo write interface
        .o_valid                        (after_valid),
        .o_data                         (after_data),
        .i_stall                        (after_stall)
    );
    
    
    generate
    if (DATA_WIDTH == 0) begin
        assign after_data_mux = 'x;
    end
    else begin
        assign after_data_up = after_data[DATA_WIDTH-1:0];
        assign after_data_dir = after_data[DATA_WIDTH];
        assign after_data_back = after_data[2*DATA_WIDTH:DATA_WIDTH+1];
        assign after_data_mux = (after_data_dir) ? after_data_up : after_data_back;
    end
    endgenerate
    
    //downstream fifo: from sync to downstream
    hld_fifo
    #(
        //fifo config
        .WIDTH                          (DATA_WIDTH),
        .DEPTH                          (DOWN_REAL_FIFO_DEPTH),
        .ALMOST_EMPTY_CUTOFF            (DOWN_ALMOST_EMPTY_CUTOFF),
        .ALMOST_FULL_CUTOFF             (DOWN_ALMOST_FULL_CUTOFF),
        
        //reset config
        .ASYNC_RESET                    (ASYNC_RESET),
        .SYNCHRONIZE_RESET              (SYNCHRONIZE_RESET),
        .RESET_EVERYTHING               (RESET_EVERYTHING),
        .RESET_EXTERNALLY_HELD          (1),
        
        //special config
        .REGISTERED_DATA_OUT_COUNT      (0),
        .NEVER_OVERFLOWS                (1),
        .STALL_IN_EARLINESS             (DOWN_STALL_IN_EARLINESS),
        .VALID_IN_EARLINESS             (0),
        .HOLD_DATA_OUT_WHEN_EMPTY       (0),
        .INITIAL_OCCUPANCY              (0),
        
        //interface selection
        .USE_STALL_LATENCY_UPSTREAM     (1),
        .USE_STALL_LATENCY_DOWNSTREAM   (1),
        
        //fifo selection
        .STYLE                          (ALLOW_HIGH_SPEED_FIFO_USAGE ? "hs" : "ms"),

        .enable_ecc                     (enable_ecc)
    )
    downstream_fifo
    (
        .clock                          (clock),
        .resetn                         (resetn),
        
        //upstream - from sync
        .i_valid                        (after_valid),
        .i_data                         (after_data_mux),
        .o_stall                        (after_stall),
        
        //downstream - to downstream
        .o_valid                        (valid_out),
        .o_data                         (data_out),
        .i_stall                        (stall_in),
        .o_empty                        (empty_out),
        .o_almost_empty                 (almost_empty_out),

        .ecc_err_status                 (ecc_err_status_2)
    );
    
    assign ecc_err_status = ecc_err_status_0 | ecc_err_status_1 | ecc_err_status_2;
endmodule

`default_nettype wire
