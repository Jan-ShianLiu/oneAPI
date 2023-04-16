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


//  Desynchronization IP for Stall Latency
//  
//  DESCRIPTION
//  ===========
//  This block implements a one-to-many adapter between FIFO-like interfaces using the stall latency protocol. The comments assume the reader is
//  familiar with the stall latency protocol, which is described in detail at //depot/docs/hld/compiler/Stall_Latency_FD.docx.
//  
//  PURPOSE OF DESYNC
//  =================
//  The purpose of the one-to-many adapter is read one item from the upstream block (make forward progress) only when all downstream blocks have
//  space for it. Stall latency allows for pipeline stages from the almost full of each downstream block to the read of the upstream block, and
//  we just add extra capacity to the downstream blocks to compensate for this latency. Beware that more pipelining means more dead space in the
//  downstream FIFOs (capacity added just to catch transactions already in flight when backpressure starts to assert, this is capacity that would
//  not be needed otherwise).
//
//  OVERVIEW OF INTERFACES
//  ======================
//  Multiple downstream interfaces:
//    - Write side FIFO interfaces
//    - Advertise almost full to desync
//    - Desync will OR the almost full signals, and present one backpressure signal to upstream
//  
//  Single upstream interface:
//    - Read side FIFO interface
//    - Consumes backpressure from desync, indicates to desync whether or not data was read
//    - If data was read, desync sends a write to all downstream interfaces
//  
//  Beware the FIFO interfaces use the stall latency protocol, meaning that "valid" actually means forced write and "stall" means almost full.
//  
//  HIERARCHY
//  =========
//  This file contains 2 modules:
//  
//  acl_desync_predicate_nonblocking:
//    - This module adds pipeline register stages from i_stall to o_stall, and from i_valid to o_valid
//    - Separate pipeline registers bring almost full from each downstream block into a centralized place (pipelining for routability), once they reach
//      the centralized place they are merged, then the result is pipelined to the stall of the upstream block
//    - o_valid from upstream indicates whether to do a transaction (read from upstream, write to all downstream), each destination that this signal will
//       go to has its own copy of pipeline registers (logically equivalent but separated because they can physically route to different areas)
//  
//  acl_desync:
//    - This wraps acl_desync_predicate_nonblocking and disables predication and nonblocking mode (these two features are not needed for desyncing data paths)
//    - The data path is also not connected, one must handle that externally
//  
//  The moost common features are exposed by acl_desync. The additional features in acl_desync_predicate_nonblocking are intended for channels.
//
//  PIPELINE STAGES: PHYSICAL VS LOGICAL
//  ====================================
//  The ***_LATENCY parameters describe the number of pipeline stages assuming no FIFO retiming, e.g. upstream FIFOs have STALL_IN_EARLINESS = 0 and the downstream
//  FIFO has VALID_IN_EARLINESS = 0. In other words, the FIFO-like interfaces surrounding the sync block all have no internal latency. However this is likely not
//  a realistic assumption in practice.
//  
//  As an example, suppose we set STALL_LATENCY = 5. This means there is 5 clocks cycles of latency from the time the merged almost full is computed until the 
//  upstream FIFO indicates whether it performed a read. If the upstream FIFO had STALL_IN_EARLINESS = 0, then we would physically have 5 pipeline registers 
//  before the i_stall port of the upstream FIFO. Instead, if the upstream FIFO had STALL_IN_EARLINESS = 1 then we would only have 4 pipeline registers. In this
//  example the upstream FIFO has stolen one pipeline register for its own internal retiming. The hyper-retimer in Quartus could also do something similar, where
//  we add some registers intended for pipelining for routability, but Quartus decides to retime some registers into the read logic of the upstream FIFO.
//  
//  The ***_LATENCY parameters specify the amount of LOGICAL pipeline register stages (latency specification), so it is advisable to set these slightly higher so
//  that after retiming the remaining PHYSICAL pipeline registers stages are sufficient for pipelining for routability.
//
//  Example of recommended settings:
//    - STALL_LATENCY = 5
//    - DATA_LATENCY = 2
//    - FULL_LATENCY = 2
//    - STALL_IN_EARLINESS = 3 -> this means 5 - 3 = 2 physical registers for stall to upstream
//    - VALID_IN_EARLINESS = 1 -> this means 2 - 1 = 1 physical registers for valid to downstream

`timescale 1ns/1ps
`default_nettype none

module acl_desync #(
    //fifo config
    parameter int NUM_IFACES,               // >= 1, how many downstream interfaces
    
    //reset config
    parameter bit ASYNC_RESET = 0,          // how do we use reset: 1 means registers are reset asynchronously, 0 means registers are reset synchronously
    parameter bit SYNCHRONIZE_RESET = 1,    // based on how reset gets to us, what do we need to do: 1 means synchronize reset before consumption (if reset arrives asynchronously), 0 means passthrough (managed externally)
    parameter bit RESET_EVERYTHING = 0,     // intended for partial reconfig debug, set to 1 to reset every register (normally async reset excludes data path and sync reset additionally excludes some control signals)
    
    //stall latency config -- the ***_LATENCY parameters below add pipeline stages to the control signals (data is managed externally from acl_desync)
    parameter int STALL_LATENCY,            // clock cycles of latency from centralized merged almost_full of all downstream fifos to stall_in of upstream fifo
    parameter int DATA_LATENCY,             // clock cycles of latency from data_out of upstream fifo to data_in of downstream fifos
    parameter int FULL_LATENCY,             // clock cycles of latency from almost_full of each downstream fifo to centralized merged almost_full
    parameter int STALL_IN_EARLINESS = 0,   // do not set above STALL_LATENCY
    parameter int VALID_IN_EARLINESS = 0    // do not set above DATA_LATENCY
)
(
    input  wire                         clock,
    input  wire                         resetn,
    
    //upstream - single fifo read side interface
    input  wire                         i_valid,        //forced write from upstream
    input  wire                         i_empty,        //indicates if upstream has data to send. Must be synchronized with i_valid. Currently used only for profiling.
    output logic                        o_stall,        //backpressure to upstream
    
    //downstream - multiple fifo write side interfaces
    output logic       [NUM_IFACES-1:0] o_valid,        //forced write to downstream
    input  wire        [NUM_IFACES-1:0] i_stall,        //backpressure from downstream
    
    //profiler ports
    output logic       [NUM_IFACES-1:0] o_profiler_downstream_stall // Index [i] asserts when downstream interface i is the only one stalling when upstream has non-predicated valid data available. In other words, this profiles when each downstream interface is purely responsible for preventing forward progress.

    //note that data needs to be handled externally to acl_desync
);

    acl_desync_predicate_nonblocking #(
        //fifo config
        .NUM_IFACES         (NUM_IFACES),
        .DATA_WIDTH         (0),
        
        //reset config
        .ASYNC_RESET        (ASYNC_RESET),      
        .SYNCHRONIZE_RESET  (SYNCHRONIZE_RESET),
        .RESET_EVERYTHING   (RESET_EVERYTHING), 
        
        //stall latency config
        .STALL_LATENCY      (STALL_LATENCY),
        .DATA_LATENCY       ({NUM_IFACES{DATA_LATENCY}}),
        .FULL_LATENCY       ({NUM_IFACES{FULL_LATENCY}}),
        .STALL_IN_EARLINESS (STALL_IN_EARLINESS),
        .VALID_IN_EARLINESS ({NUM_IFACES{VALID_IN_EARLINESS}}),
        
        //special config
        .NON_BLOCKING       (0)
    )
    acl_desync_predicate_nonblocking_inst
    (
        .clock                      (clock),
        .resetn                     (resetn),
        
        //upstream - single fifo read size interface
        .i_data                     (),         //not driven -- to be managed externally from acl_desync
        .i_valid                    (i_valid),
        .i_predicate                (1'b0),
        .i_empty                    (i_empty),
        .o_stall                    (o_stall),
        
        //downstream - multiple fifo write side interfaces
        .o_data                     (),         //ignored -- to be managed externally from acl_desync
        .o_valid                    (o_valid),
        .i_stall                    (i_stall),

        //profiler ports
        .o_profiler_downstream_stall (o_profiler_downstream_stall)
    );
    
endmodule





module acl_desync_predicate_nonblocking #(
    //fifo config
    parameter int NUM_IFACES,               // >= 1, how many downstream interfaces
    parameter int DATA_WIDTH,               // data width of each downstream block, if a downstream blocks has a narrower width then quartus will prune away the unused bits
    
    //reset config
    parameter bit ASYNC_RESET = 0,          // how do we use reset: 1 means registers are reset asynchronously, 0 means registers are reset synchronously
    parameter bit SYNCHRONIZE_RESET = 1,    // based on how reset gets to us, what do we need to do: 1 means synchronize reset before consumption (if reset arrives asynchronously), 0 means passthrough (managed externally)
    parameter bit RESET_EVERYTHING = 0,     // intended for partial reconfig debug, set to 1 to reset every register (normally async reset excludes data path and sync reset additionally excludes some control signals)
    
    //stall latency config -- the ***_LATENCY parameters below add pipeline stages to the control signals and data signals
    //**IMPORTANT**: the latency for EACH interface can be specified, if you want to set FULL_LATENCY to 2 for all interfaces, then set the parameter value to {NUM_IFACES{32'd2}}
    //the parameter slice into 32 bit values per interface, e.g. interface 0 uses the value in bits 31:0, interface 1 uses the value in bits 63:32, interface 2 uses bits 95:64, etc.
    parameter bit [32*         1-1:0] STALL_LATENCY = 0,        // clock cycles of latency from centralized merged almost_full of all downstream fifos to stall_in of upstream fifo
    parameter bit [32*NUM_IFACES-1:0] DATA_LATENCY = 0,         // clock cycles of latency from data_out of upstream fifo to data_in of downstream fifo N
    parameter bit [32*NUM_IFACES-1:0] FULL_LATENCY = 0,         // clock cycles of latency from almost_full of downstream fifo N to centralized merged almost_full
    parameter bit [32*         1-1:0] STALL_IN_EARLINESS = 0,   // value cannot exceed STALL_LATENCY
    parameter bit [32*NUM_IFACES-1:0] VALID_IN_EARLINESS = 0,   // each values cannot exceed the corresponding DATA_LATENCY, e.g. for interface 0 VALID_IN_EARLINESS[31:0] cannot be larger than DATA_LATENCY[31:0]
    
    //special config
    parameter bit NON_BLOCKING = 0          // for st_write, set to 1 to try to write immediately even if channel is full
)
(
    input  wire                                 clock,
    input  wire                                 resetn,
    
    //upstream - single fifo read size interface
    input  wire     [NUM_IFACES*DATA_WIDTH-1:0] i_data,         //i_data[ DATA_WIDTH*(i+1)-1 :  DATA_WIDTH*i ] eventually drives downstream interface i, upper bits of this region can be x if interface i has narrower data path
    input  wire                                 i_valid,        //forced write from upstream
    input  wire                                 i_predicate,    //transaction should be ignored for all except o_valid[0]
    input  wire                                 i_empty,        //indicates if upstream has data to send. Must be synchronized with i_valid. Currently used only for profiling.
    output logic                                o_stall,        //backpressure to upstream
    
    //downstream - multiple fifo write side interfaces
    output logic    [NUM_IFACES*DATA_WIDTH-1:0] o_data,         //downstream interface i consumes from o_data[ DATA_WIDTH*(i+1)-1 :  DATA_WIDTH*i ], upper bits of this region can be x if interface i has narrower data path
    output logic               [NUM_IFACES-1:0] o_valid,        //forced write to downstream
    input  wire                [NUM_IFACES-1:0] i_stall,        //backpressure from downstream

    //profiler ports
    output logic               [NUM_IFACES-1:0] o_profiler_downstream_stall // Index [i] asserts when downstream interface i is the only one stalling when upstream has non-predicated valid data available. In other words, this profiles when each downstream interface is purely responsible for preventing forward progress.
);
    
    
    genvar f, g;
    
    //reset
    logic aclrn, sclrn, sclrn_reset_everything;
    acl_reset_handler
    #(
        .ASYNC_RESET       (ASYNC_RESET),
        .USE_SYNCHRONIZER  (SYNCHRONIZE_RESET),
        .SYNCHRONIZE_ACLRN (SYNCHRONIZE_RESET),
        .PIPE_DEPTH        (2),
        .NUM_COPIES        (1)
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
    
    
    
    //stall latency signals
    logic  [NUM_IFACES-1:0] central_almost_full;
    logic                   central_almost_full_any_asserted;
    
    //polarity of the pipeline stages has been chosen so that initial values of 0 will match stall/valid reset behavior of s10
    //this makes it safe for older families when not using partial reconfig, will not get spurious transactions as the power-on reset releases
    //when using partial reconfig one must use reset for control logic, reset must be held for long enough that the control signal pipelines can flush
    
    //the parameter checks are done in Quartus pro and Modelsim, it is disabled in Quartus standard because it results in a syntax error (parser is based on an older systemverilog standard)
    //the workaround is to use synthesis translate to hide this from Quartus standard, ALTERA_RESERVED_QHD is only defined in Quartus pro, and Modelsim ignores the synthesis comment
    
    /////////////////////////////////////////////////////////////////////////////////
    // STAGE 1: collect pipelined versions of almost_full from all downstream fifos
    //
    
    generate
    for (f=0; f<NUM_IFACES; f=f+1) begin : gen_full
        localparam int AFULL_PIPELINE_STAGES = FULL_LATENCY[32*f +: 32];
        `ifdef ALTERA_RESERVED_QHD
        `else
        //synthesis translate_off
        `endif
        if (AFULL_PIPELINE_STAGES < 0) begin
            $fatal(1, "acl_desync_predicate_nonblocking: invalid AFULL_PIPELINE_STAGES %d, interface number %d\n", AFULL_PIPELINE_STAGES, f);
        end
        `ifdef ALTERA_RESERVED_QHD
        `else
        //synthesis translate_on
        `endif
        logic [AFULL_PIPELINE_STAGES:0] pipe_has_capacity /* synthesis dont_merge */;
        
        assign pipe_has_capacity[0] = ~i_stall[f];
        if (AFULL_PIPELINE_STAGES > 0) begin
            always_ff @(posedge clock or negedge aclrn) begin
                if (~aclrn) begin
                    pipe_has_capacity[AFULL_PIPELINE_STAGES:1] <= '0;
                end
                else begin
                    pipe_has_capacity[AFULL_PIPELINE_STAGES:1] <= pipe_has_capacity[AFULL_PIPELINE_STAGES-1:0];
                    if (~sclrn_reset_everything) pipe_has_capacity[AFULL_PIPELINE_STAGES:1] <= '0;
                end
            end
        end
        assign central_almost_full[f] = ~pipe_has_capacity[AFULL_PIPELINE_STAGES];
    end
    endgenerate
    
    
    
    ////////////////////////////////////
    //                                //
    //  STALL LATENCY IMPLEMENTATION  //
    //                                //
    ////////////////////////////////////
    
    assign central_almost_full_any_asserted = (NON_BLOCKING) ? central_almost_full[0] : (|central_almost_full);
    
    
    
    /////////////////////////////////////////////////////////////////////////
    // STAGE 2: if any almost_full is asserted, send stall to upstream fifo
    //
    localparam int STALL_PIPELINE_STAGES = STALL_LATENCY[31:0] - STALL_IN_EARLINESS[31:0];
    `ifdef ALTERA_RESERVED_QHD
    `else
    //synthesis translate_off
    `endif
    if (STALL_PIPELINE_STAGES < 0) begin
        $fatal(1, "acl_desync_predicate_nonblocking: invalid STALL_PIPELINE_STAGES %d\n", STALL_PIPELINE_STAGES);
    end
    `ifdef ALTERA_RESERVED_QHD
    `else
    //synthesis translate_on
    `endif
    logic [STALL_PIPELINE_STAGES:0] pipe_not_stall /* synthesis dont_merge */;
    
    assign pipe_not_stall[0] = ~central_almost_full_any_asserted;
    generate
    if (STALL_PIPELINE_STAGES > 0) begin
        always_ff @(posedge clock or negedge aclrn) begin
            if (~aclrn) begin
                pipe_not_stall[STALL_PIPELINE_STAGES:1] <= '0;
            end
            else begin
                pipe_not_stall[STALL_PIPELINE_STAGES:1] <= pipe_not_stall[STALL_PIPELINE_STAGES-1:0];
                if (~sclrn) pipe_not_stall[STALL_PIPELINE_STAGES] <= '0;    //assert backpressure to upstream while in reset, don't wait for pipeline to flush
                if (~sclrn_reset_everything) pipe_not_stall[STALL_PIPELINE_STAGES:1] <= '0;
            end
        end
    end
    endgenerate
    assign o_stall = ~pipe_not_stall[STALL_PIPELINE_STAGES];
    
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STAGE 3: given stall, upstream fifo will decide whether it wants to FORCE data to downstream (no backpressure)
    //
    generate
    for (f=0; f<NUM_IFACES; f=f+1) begin : gen_valid
        localparam int VALID_PIPELINE_STAGES = DATA_LATENCY[32*f+:32] - VALID_IN_EARLINESS[32*f+:32];
        `ifdef ALTERA_RESERVED_QHD
        `else
        //synthesis translate_off
        `endif
        if (VALID_PIPELINE_STAGES < 0) begin
            $fatal(1, "acl_desync_predicate_nonblocking: invalid VALID_PIPELINE_STAGES %d, interface number %d\n", VALID_PIPELINE_STAGES, f);
        end
        `ifdef ALTERA_RESERVED_QHD
        `else
        //synthesis translate_on
        `endif
        logic [VALID_PIPELINE_STAGES:0] pipe_valid /* synthesis dont_merge */;
        
        assign pipe_valid[0] = (f==0) ? i_valid : (i_valid & ~i_predicate);     //valid must still be seen by downstream data path regardless of predication, but must not be seen by sidepaths if predicated
        if (VALID_PIPELINE_STAGES > 0) begin
            always_ff @(posedge clock or negedge aclrn) begin
                if (~aclrn) begin
                    pipe_valid[VALID_PIPELINE_STAGES:1] <= '0;
                end
                else begin
                    pipe_valid[VALID_PIPELINE_STAGES:1] <= pipe_valid[VALID_PIPELINE_STAGES-1:0];
                    if (~sclrn) pipe_valid[VALID_PIPELINE_STAGES] <= '0;        //do not write to downstream while in reset, don't wait for pipeline to flush
                    if (~sclrn_reset_everything) pipe_valid[VALID_PIPELINE_STAGES:1] <= '0;
                end
            end
        end
        assign o_valid[f] = pipe_valid[VALID_PIPELINE_STAGES];
    end
    endgenerate
    
    //valid and data have the same latency for a given downstream block, but may have different number of pipeline stages due to VALID_IN_EARLINESS
    generate
    for (f=0; f<NUM_IFACES; f=f+1) begin : gen_data
        localparam int DATA_PIPELINE_STAGES = DATA_LATENCY[32*f+:32];
        `ifdef ALTERA_RESERVED_QHD
        `else
        //synthesis translate_off
        `endif
        if (DATA_PIPELINE_STAGES < 0) begin
            $fatal(1, "acl_desync_predicate_nonblocking: invalid DATA_PIPELINE_STAGES %d, interface number %d\n", DATA_PIPELINE_STAGES, f);
        end
        `ifdef ALTERA_RESERVED_QHD
        `else
        //synthesis translate_on
        `endif
        if (DATA_WIDTH > 0) begin
            logic [DATA_PIPELINE_STAGES:0][DATA_WIDTH-1:0] pipe_data /* synthesis dont_merge */;
            
            assign pipe_data[0] = i_data[DATA_WIDTH*f +: DATA_WIDTH];
            for (g=1; g<=DATA_PIPELINE_STAGES; g=g+1) begin : gen_pipe_data
                always_ff @(posedge clock or negedge aclrn) begin
                    if (~aclrn) begin
                        pipe_data[g] <= (RESET_EVERYTHING) ? '0 : 'x;
                    end
                    else begin
                        pipe_data[g] <= pipe_data[g-1];
                        if (~sclrn_reset_everything) pipe_data[g] <= '0;
                    end
                end
            end
            assign o_data[DATA_WIDTH*f +: DATA_WIDTH] = pipe_data[DATA_PIPELINE_STAGES];
        end
    end
    endgenerate
    
        
    // Profiler ports
    localparam          UPSTREAM_EMPTY_ROUNDTRIP_LATENCY = STALL_LATENCY;   // This is the latency from when i_stall toggles to when: i_valid toggles and i_empty is updated.
    logic               [UPSTREAM_EMPTY_ROUNDTRIP_LATENCY:0][NUM_IFACES-1:0] profiler_downstream_stall_pipe;
    generate
        /*  Detect when downstream port [f] is preventing forward progress. In other words, check when it's the only port stalling AND we would have received valid data from upstream had we requested it.
            To determine this latter fact, look at the state of i_empty UPSTREAM_EMPTY_ROUNDTRIP_LATENCY cycles from now. The idea is: had we unstalled upstream now, in UPSTREAM_EMPTY_ROUNDTRIP_LATENCY cycles we'd get data if it was available. 
            So the i_empty signal UPSTREAM_EMPTY_ROUNDTRIP_LATENCY cycles from now tells us if upstream had data to send.
        */
        for (f=0; f<NUM_IFACES; f=f+1) begin : GEN_RANDOM_BLOCK_NAME_R5
            always_comb begin
                profiler_downstream_stall_pipe[0][f] = (central_almost_full == (1'b1 << f)); // Check if downstream port [f] is the only one stalling right now.
            end
        end
        // Delay by UPSTREAM_EMPTY_ROUNDTRIP_LATENCY to match the latency on i_empty becoming updated.
        for (g=1; g<=UPSTREAM_EMPTY_ROUNDTRIP_LATENCY; g++) begin : GEN_RANDOM_BLOCK_NAME_R6
            always_ff @(posedge clock or negedge aclrn) begin
                if (!aclrn) begin
                    profiler_downstream_stall_pipe[g] <= '0;
                end else begin
                    profiler_downstream_stall_pipe[g] <= profiler_downstream_stall_pipe[g-1];
                    if (!sclrn_reset_everything) begin
                        profiler_downstream_stall_pipe[g] <= '0;
                    end
                end
            end
        end
        // Check if port [f] was the only one stalling and we missed out on receiving real (non-predicated) valid data from upstream.
        for (f=0; f<NUM_IFACES; f=f+1) begin : gen_profiler
            always_ff @(posedge clock or negedge aclrn) begin
                if (!aclrn) begin
                    o_profiler_downstream_stall[f] <= 1'b0;
                end else begin
                    o_profiler_downstream_stall[f] <= profiler_downstream_stall_pipe[UPSTREAM_EMPTY_ROUNDTRIP_LATENCY][f] && !i_empty && !i_predicate;   
                    if (!sclrn) begin
                        o_profiler_downstream_stall[f] <= 1'b0;
                    end
                end
            end
        end        
    endgenerate    
    
endmodule


`default_nettype wire
