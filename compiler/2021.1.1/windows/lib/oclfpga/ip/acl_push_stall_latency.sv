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


//  This module implements the "push" of push/pop, a desync that distributes upstream data to downstream and the backedge fifo
//  Only the "COALESCE" style from acl_pop is implemented (no such style exists in acl_push, but it is emulated by hardcoding dir=1), which means initial occupancy is required in the backedge fifo
//  All interfaces use the stall latency protocol
//  Push is similar to iowr in that it uses a desync block

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


module acl_push_stall_latency #(
    //supported parameters from acl_push
    parameter int DATA_WIDTH = 32,
    parameter int FIFO_DEPTH = 1,               //original capacity of the backedge fifo, we will increase it to add dead space for almost full and almost empty
    parameter string STYLE = "COALESCE",        //"COALESCE" gives the behavior of dir=1 with STYLE="REGULAR" in the old ip, "TOKEN" makes the backedge fifo output a cyclic sequence of numbers from FIFO_DEPTH-1 decreasing to 0
    
    //backedge fifo config
    parameter int INITIAL_OCCUPANCY = 0,        //number of words of garbage data in the backedge fifo when it comes out of reset
    
    //choice of hld_fifo style
    parameter bit ALLOW_HIGH_SPEED_FIFO_USAGE = 1,  //0 = mid speed fifo, 1 = high speed fifo
    
    //predication
    parameter bit NO_PREDICATION = 0,           //set to 1 if it is known that predicate_in = 0, unfortunately quartus doesn't propagate the constant 0 through the fifo so we will manually sweep it away
    
    //unsupported parameters from acl_push
    //parameter int MIN_FIFO_LATENCY = 0,       //maximum write to read latency of the backedge fifo, should really be called MAX_FIFO_LATENCY, min depth of 16 needed for stall latency so forcing use of high speed fifo
    //parameter STALLFREE = 0,
    //parameter ENABLED = 0,
    //parameter INF_LOOP = 0,                   //this will now be handled with initial occupancy in the backedge fifo
    //parameter INF_LOOP_INITIALIZATION = 0,
    //parameter RAM_FIFO_DEPTH_INC = 0,
    
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
    parameter bit RESET_EVERYTHING = 0,          // intended for partial reconfig debug, set to 1 to reset every register (normally async reset excludes data path and sync reset additionally excludes some control signals)
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
    //input wire                      dir_in,                   //this signal has been deprecated, we follow the behavior of the legacy acl_push when dir is hardcoded to 1
    
    //output to backedge - fifo read side interface
    //similar to channel fifo merging, the backedge fifo is the only remaining fifo, it plugs in directly to the sync block internals in acl_pop_stall_latency
    output logic                    feedback_empty_out,
    output logic                    feedback_almost_empty_out,  //threshold determined by BACK_ALMOST_EMPTY_CUTOFF localparam, must exactly match value in acl_pop_stall_latency
    output logic   [DATA_WIDTH-1:0] feedback_data_out,
    input  wire                     feedback_stall_in,          //earliness is set by BACK_STALL_IN_EARLINESS localparam, must exactly match value in acl_pop_stall_latency
    
    //output to downstream - fifo read side interface
    output logic                    valid_out,                  //indicates that there is a (non backpressureable) read happening from downstream fifo
    output logic                    empty_out,                  //advertise that downstream fifo cannot be read because it is empty
    output logic                    almost_empty_out,           //threshold determined by DOWN_ALMOST_EMPTY_CUTOFF
    output logic   [DATA_WIDTH-1:0] data_out,
    input  wire                     stall_in,                   //downstream provides backpressure early if DOWN_STALL_IN_EARLINESS >= 1
    output logic              [1:0] ecc_err_status              // ecc status signals
);

    //stall latency parameters for sync -- localparams for now, these can later be exposed as parameters once the compiler has a reasonable method of setting them
    localparam int DOWN_FULL_LATENCY = 2;       //almost full, downstream to central
    localparam int BACK_FULL_LATENCY = 2;       //almost full, backedge to central
    localparam int UP_STALL_LATENCY = 5;        //stall, central to upstream
    localparam int UP_STALL_IN_EARLINESS = 3;   //stall in earliness of upstream fifo, cannot exceed UP_STALL_LATENCY
    localparam int DOWN_DATA_LATENCY = 2;       //data latency, upstream to downstream
    localparam int BACK_DATA_LATENCY = 2;       //data latency, upstream to backedge
    
    //backedge fifo configuration
    localparam int BACK_STALL_IN_EARLINESS = 3; //stall in earliness of backedge fifo -- must exactly match the value in acl_pop_stall_latency
    localparam int BACK_EMPTY_LATENCY = 2;      //for use witn sync inside acl_pop_stall_latency -- must exactly match the value there
    localparam int BACK_STALL_LATENCY = 5;      //for use witn sync inside acl_pop_stall_latency -- must exactly match the value there
    localparam int BACK_ALMOST_EMPTY_CUTOFF = BACK_EMPTY_LATENCY + BACK_STALL_LATENCY;
    localparam int BACK_ALMOST_FULL_CUTOFF = BACK_FULL_LATENCY + UP_STALL_LATENCY + BACK_DATA_LATENCY;
    localparam int BACK_FIFO_DEPTH = FIFO_DEPTH + BACK_ALMOST_EMPTY_CUTOFF + BACK_ALMOST_FULL_CUTOFF;
    
    //sanity check on fifo sizes
    localparam int UP_MIN_FIFO_DEPTH = UP_ALMOST_FULL_CUTOFF + 1;
    localparam int UP_REAL_FIFO_DEPTH = (UP_FIFO_DEPTH < UP_MIN_FIFO_DEPTH) ? UP_MIN_FIFO_DEPTH : UP_FIFO_DEPTH;
    localparam int DOWN_ALMOST_FULL_CUTOFF = DOWN_FULL_LATENCY + UP_STALL_LATENCY + DOWN_DATA_LATENCY;
    localparam int DOWN_MIN_FIFO_DEPTH = DOWN_ALMOST_FULL_CUTOFF + 1;
    localparam int DOWN_REAL_FIFO_DEPTH = (DOWN_FIFO_DEPTH < DOWN_MIN_FIFO_DEPTH) ? DOWN_MIN_FIFO_DEPTH : DOWN_FIFO_DEPTH;
    
    
    //signals before and after the desync block
    logic before_stall, before_valid, before_predicate;
    logic [DATA_WIDTH-1:0] before_data;
    logic [2*DATA_WIDTH-1:0] before_data_wide, after_data_wide;
    logic [DATA_WIDTH-1:0] after_data_downstream, after_data_backedge;
    logic [1:0] after_valid, after_stall;
    logic [1:0] ecc_err_status_0, ecc_err_status_1, ecc_err_status_2;
    
    //upstream fifo: from upstream to desync
    localparam UPSTREAM_FIFO_WIDTH = (NO_PREDICATION) ? DATA_WIDTH : (DATA_WIDTH + 1);  //+1 for predicate
    logic [UPSTREAM_FIFO_WIDTH-1:0] upstream_fifo_i_data, upstream_fifo_o_data;
    assign upstream_fifo_i_data = (NO_PREDICATION) ? data_in : {data_in, predicate_in};
    assign before_data = (NO_PREDICATION) ? upstream_fifo_o_data[DATA_WIDTH-1:0] : upstream_fifo_o_data[DATA_WIDTH:1];    //bit 0 is used for predication only if present
    assign before_predicate = (NO_PREDICATION) ? 1'b0 : upstream_fifo_o_data[0];
    hld_fifo
    #(
        //fifo config
        .WIDTH                          (UPSTREAM_FIFO_WIDTH),
        .DEPTH                          (UP_REAL_FIFO_DEPTH),
        .ALMOST_EMPTY_CUTOFF            (0),    //not needed by desync
        .ALMOST_FULL_CUTOFF             (UP_ALMOST_FULL_CUTOFF),
        
        //reset config
        .ASYNC_RESET                    (ASYNC_RESET),
        .SYNCHRONIZE_RESET              (SYNCHRONIZE_RESET),
        .RESET_EVERYTHING               (RESET_EVERYTHING),
        .RESET_EXTERNALLY_HELD          (1),
        
        //special config
        .REGISTERED_DATA_OUT_COUNT      ((NO_PREDICATION) ? 0 : 1),    //register just the predicate, not the data
        .NEVER_OVERFLOWS                (1),
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
        .i_valid                        (valid_in),
        .i_data                         (upstream_fifo_i_data),
        .o_stall                        (stall_out),
        
        //downstream - to desync
        .o_valid                        (before_valid),
        .o_data                         (upstream_fifo_o_data),
        .i_stall                        (before_stall),

        .ecc_err_status                 (ecc_err_status_0)
    );
    
    
    //stall latency implementation of desync
    assign before_data_wide = {before_data, before_data};
    acl_desync_predicate_nonblocking
    #(
        //fifo config
        .NUM_IFACES                     (2),
        .DATA_WIDTH                     (DATA_WIDTH),
        
        //reset config  
        .ASYNC_RESET                    (ASYNC_RESET),
        .SYNCHRONIZE_RESET              (SYNCHRONIZE_RESET),
        .RESET_EVERYTHING               (RESET_EVERYTHING),
        
        //stall latency config
        .STALL_LATENCY                  (UP_STALL_LATENCY),
        .DATA_LATENCY                   ({BACK_DATA_LATENCY, DOWN_DATA_LATENCY}),
        .FULL_LATENCY                   ({BACK_FULL_LATENCY, DOWN_FULL_LATENCY}),
        .STALL_IN_EARLINESS             (UP_STALL_IN_EARLINESS),
        .VALID_IN_EARLINESS             ({32'h0, 32'h0}),
        
        //special config    
        .NON_BLOCKING                   (0)
    )
    desync_inst
    (
        .clock                          (clock),
        .resetn                         (resetn),
        
        //from src - single fifo read interface
        .i_data                         (before_data_wide),
        .i_predicate                    (before_predicate),
        .i_valid                        (before_valid),
        .o_stall                        (before_stall),
        
        //to dst - multiple fifo write interfaces
        .o_valid                        (after_valid),
        .o_data                         (after_data_wide),
        .i_stall                        (after_stall)
    );
    
    
    //downstream fifo: from desync to downstream
    assign after_data_downstream = after_data_wide[DATA_WIDTH-1:0];
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
        .i_valid                        (after_valid[0]),
        .i_data                         (after_data_downstream),
        .o_stall                        (after_stall[0]),
        
        //downstream - to downstream
        .o_valid                        (valid_out),
        .o_data                         (data_out),
        .i_stall                        (stall_in),
        .o_empty                        (empty_out),
        .o_almost_empty                 (almost_empty_out),

        .ecc_err_status                 (ecc_err_status_1)
    );
    
    
    //backedge fifo: from desync to backedge
    generate
    
    if (STYLE == "TOKEN") begin
        logic aclrn, sclrn, sclrn_reset_everything;
        logic backedge_fifo_wrreq_prev;
        logic backedge_fifo_read_m2, backedge_fifo_read_m1, backedge_fifo_read;
        logic backedge_fifo_valid_m3, backedge_fifo_valid_m2, backedge_fifo_valid_m1, backedge_fifo_valid;
        logic backedge_fifo_many_valid_m3, backedge_fifo_many_valid_m2, backedge_fifo_many_valid_m1, backedge_fifo_many_valid;
        
        //reset
        acl_reset_handler
        #(
            .ASYNC_RESET            (ASYNC_RESET),
            .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
            .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
            .PIPE_DEPTH             (2),
            .NUM_COPIES             (1)
        )
        acl_reset_handler_inst
        (
            .clk                    (clock),
            .i_resetn               (resetn),
            .o_aclrn                (aclrn),
            .o_resetn_synchronized  (),
            .o_sclrn                (sclrn)
        );
        assign sclrn_reset_everything = (RESET_EVERYTHING) ? sclrn : 1'b1;
        
        //delay by 1 clock the write request that would have gone to backedge_fifo
        always_ff @(posedge clock or negedge aclrn) begin
            if (~aclrn) begin
                backedge_fifo_wrreq_prev <= 1'b0;
            end
            else begin
                backedge_fifo_wrreq_prev <= after_valid[1];
                if (~sclrn && RESET_EVERYTHING) backedge_fifo_wrreq_prev <= 1'b0;
            end
        end
        
        //normally read used words is computed by delaying the write request by 4 clocks (1 less than write to read latency of high speed fifo)
        //we only delayed the write by 1 clock, so we can compute valid_out 3 clocks early (since we also have stall_in 3 clocks early)
        acl_tessellated_incr_decr_threshold #(
            .CAPACITY                     (BACK_FIFO_DEPTH),
            .THRESHOLD                    (1),
            .INITIAL_OCCUPANCY            (INITIAL_OCCUPANCY),
            .WRITE_AND_READ_DURING_FULL   (0),
            .THRESHOLD_REACHED_AT_RESET   (0),
            .RESET_RELEASE_DELAY          (0),
            .ASYNC_RESET                  (ASYNC_RESET),
            .SYNCHRONIZE_RESET            (SYNCHRONIZE_RESET),
            .RESET_EVERYTHING             (RESET_EVERYTHING),
            .RESET_EXTERNALLY_HELD        (1)
        )
        backedge_fifo_early_valid
        (
            .clock                        (clock),
            .resetn                       (resetn),
            .incr_no_overflow             (backedge_fifo_wrreq_prev),
            .incr_raw                     (backedge_fifo_wrreq_prev),
            .decr_no_underflow            (~feedback_stall_in & backedge_fifo_valid_m3),
            .decr_raw                     (~feedback_stall_in),
            .threshold_reached            (backedge_fifo_valid_m3)
        );
        
        //likewise almost empty can also be computed 3 clocks early, it uses the same incr and decr as empty but a different threshold
        acl_tessellated_incr_decr_threshold #(
            .CAPACITY                     (BACK_FIFO_DEPTH),
            .THRESHOLD                    (BACK_ALMOST_EMPTY_CUTOFF + 1),   //convert cutoff into threshold
            .INITIAL_OCCUPANCY            (INITIAL_OCCUPANCY),
            .WRITE_AND_READ_DURING_FULL   (0),
            .THRESHOLD_REACHED_AT_RESET   (0),
            .RESET_RELEASE_DELAY          (0),
            .ASYNC_RESET                  (ASYNC_RESET),
            .SYNCHRONIZE_RESET            (SYNCHRONIZE_RESET),
            .RESET_EVERYTHING             (RESET_EVERYTHING),
            .RESET_EXTERNALLY_HELD        (1)
        )
        backedge_fifo_almost_empty
        (
            .clock                        (clock),
            .resetn                       (resetn),
            .incr_no_overflow             (backedge_fifo_wrreq_prev),
            .incr_raw                     (backedge_fifo_wrreq_prev),
            .decr_no_underflow            (~feedback_stall_in & backedge_fifo_valid_m3),
            .decr_raw                     (~feedback_stall_in),
            .threshold_reached            (backedge_fifo_many_valid_m3)
        );
        
        //keep the early signals live to restore the original timing as exported by the fifo
        //m3 means at time minus 3 (3 clocks early), m2 means 2 clocks early, etc.
        always_ff @(posedge clock or negedge aclrn) begin
            if (~aclrn) begin
                backedge_fifo_read_m2 <= 1'b0;
                backedge_fifo_read_m1 <= 1'b0;
                backedge_fifo_read <= 1'b0;
                backedge_fifo_valid_m2 <= 1'b0;
                backedge_fifo_valid_m1 <= 1'b0;
                backedge_fifo_valid <= 1'b0;
                backedge_fifo_many_valid_m2 <= 1'b0;
                backedge_fifo_many_valid_m1 <= 1'b0;
                backedge_fifo_many_valid <= 1'b0;
            end
            else begin
                backedge_fifo_read_m2 <= backedge_fifo_valid_m3 & ~feedback_stall_in;
                backedge_fifo_read_m1 <= backedge_fifo_read_m2;
                backedge_fifo_read <= backedge_fifo_read_m1;
                backedge_fifo_valid_m2 <= backedge_fifo_valid_m3;
                backedge_fifo_valid_m1 <= backedge_fifo_valid_m2;
                backedge_fifo_valid <= backedge_fifo_valid_m1;
                backedge_fifo_many_valid_m2 <= backedge_fifo_many_valid_m3;
                backedge_fifo_many_valid_m1 <= backedge_fifo_many_valid_m2;
                backedge_fifo_many_valid <= backedge_fifo_many_valid_m1;
                if (~sclrn) begin
                    backedge_fifo_read <= 1'b0;
                    backedge_fifo_valid <= 1'b0;
                    backedge_fifo_many_valid <= 1'b0;
                end
                if (~sclrn && RESET_EVERYTHING) begin
                    backedge_fifo_read_m2 <= 1'b0;
                    backedge_fifo_read_m1 <= 1'b0;
                    backedge_fifo_valid_m2 <= 1'b0;
                    backedge_fifo_valid_m1 <= 1'b0;
                    backedge_fifo_many_valid_m2 <= 1'b0;
                    backedge_fifo_many_valid_m1 <= 1'b0;
                end
            end
        end
        assign feedback_empty_out = ~backedge_fifo_valid;
        assign feedback_almost_empty_out = ~backedge_fifo_many_valid;
        
        //unlike empty, almost full cannot be computed early unless we peak at what drives after_valid[1]
        //there is no big advantage to computing this early anyways
        acl_tessellated_incr_decr_threshold #(
            .CAPACITY                     (BACK_FIFO_DEPTH),
            .THRESHOLD                    (BACK_FIFO_DEPTH - BACK_ALMOST_FULL_CUTOFF),  //convert almost full cutoff into threshold
            .INITIAL_OCCUPANCY            (INITIAL_OCCUPANCY),
            .WRITE_AND_READ_DURING_FULL   (0),
            .THRESHOLD_REACHED_AT_RESET   (1),  //stall should be asserted during reset
            .RESET_RELEASE_DELAY          (0),
            .ASYNC_RESET                  (ASYNC_RESET),
            .SYNCHRONIZE_RESET            (SYNCHRONIZE_RESET),
            .RESET_EVERYTHING             (RESET_EVERYTHING),
            .RESET_EXTERNALLY_HELD        (1)
        )
        backedge_fifo_almost_full
        (
            .clock                        (clock),
            .resetn                       (resetn),
            .incr_no_overflow             (after_valid[1]),
            .incr_raw                     (after_valid[1]),
            .decr_no_underflow            (backedge_fifo_read),
            .decr_raw                     (backedge_fifo_read),
            .threshold_reached            (after_stall[1])
        );
        
        
        //data path
        //TODO: should be okay for POC but this won't have high fmax for large FIFO_DEPTH due to the wide compare against 0 and wide decrement
        always_ff @(posedge clock or negedge aclrn) begin
            if (~aclrn) begin
                feedback_data_out <= FIFO_DEPTH-1;
            end
            else begin
                if (backedge_fifo_read) begin
                    if (feedback_data_out == '0) feedback_data_out <= FIFO_DEPTH-1;
                    else feedback_data_out <= feedback_data_out - 1'b1;
                end
                if (~sclrn) feedback_data_out <= FIFO_DEPTH-1;
            end
        end
        assign ecc_err_status_2 = 2'h0;
    end
    else begin
        assign after_data_backedge = after_data_wide[2*DATA_WIDTH-1:DATA_WIDTH];
        hld_fifo
        #(
            //fifo config
            .WIDTH                          (DATA_WIDTH),
            .DEPTH                          (BACK_FIFO_DEPTH),
            .ALMOST_EMPTY_CUTOFF            (BACK_ALMOST_EMPTY_CUTOFF),
            .ALMOST_FULL_CUTOFF             (BACK_ALMOST_FULL_CUTOFF),
            
            //reset config
            .ASYNC_RESET                    (ASYNC_RESET),
            .SYNCHRONIZE_RESET              (SYNCHRONIZE_RESET),
            .RESET_EVERYTHING               (RESET_EVERYTHING),
            .RESET_EXTERNALLY_HELD          (1),
            
            //special config
            .REGISTERED_DATA_OUT_COUNT      (0),
            .NEVER_OVERFLOWS                (1),
            .STALL_IN_EARLINESS             (BACK_STALL_IN_EARLINESS),
            .VALID_IN_EARLINESS             (0),
            .HOLD_DATA_OUT_WHEN_EMPTY       (0),
            .INITIAL_OCCUPANCY              (INITIAL_OCCUPANCY),
            
            //interface selection
            .USE_STALL_LATENCY_UPSTREAM     (1),
            .USE_STALL_LATENCY_DOWNSTREAM   (1),
            
            //fifo selection
            .STYLE                          (ALLOW_HIGH_SPEED_FIFO_USAGE ? "hs" : "ms"),

            .enable_ecc                     (enable_ecc)
        )
        backedge_fifo
        (
            .clock                          (clock),
            .resetn                         (resetn),
            
            //upstream - from sync
            .i_valid                        (after_valid[1]),
            .i_data                         (after_data_backedge),
            .o_stall                        (after_stall[1]),
            
            //downstream - to channel
            .o_valid                        (), //not used since feedback interface is a stall latency fifo read interface
            .o_data                         (feedback_data_out),
            .i_stall                        (feedback_stall_in),
            .o_empty                        (feedback_empty_out),
            .o_almost_empty                 (feedback_almost_empty_out),

            .ecc_err_status                 (ecc_err_status_2)
        );
    end
    endgenerate
    
    assign ecc_err_status = ecc_err_status_0 | ecc_err_status_1 | ecc_err_status_2;
    
endmodule
