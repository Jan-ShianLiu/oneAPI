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


// The HLD compiler uses acl_channel_fifo to connect a write and a read from the
// same Opencl channel together. The user may specify the minimum depth of the
// channel, if using almost full as backpressure the compiler must inflate the
// depth to catch transactions already in flight, and the IP quantizes the depth
// to fully utilize the memory storage of MLAB or M20K.
//
// Looking at iowr->channel->iord as one system, in some cases it is beneficial
// to merge the capacity of the channel fifo into either iord or iowr. In this
// case, acl_channel_fifo acts as wires.

`default_nettype none

module acl_channel_fifo #(
    //core spec
    parameter int FIFO_DEPTH = 16,                      //minimum depth of the fifo, note the IP will quantize depth to fully utilize the memory of MLAB or M20K
    parameter int ALMOST_FULL_VALUE = 0,                //threshold at which to assert almost full, used to predicate writes into non-blocking iowr if channel won't have space
    parameter int DATA_W = 64,                          //width of the data path

    //reset
    parameter bit ASYNC_RESET = 1,                      //should registers consume reset asynchronously (ASYNC_RESET=1) or synchronously (ASYNC_RESET=0)
    parameter bit SYNCHRONIZE_RESET = 0,                //before consumption by registers, should the reset be synchronized to the clock

    //consolidation of channel capacity
    parameter bit BYPASS_CHANNEL = 0,                   //if it is beneficial to merge the capacity of the channel fifo into either iord or iowr, then acl_channel_fifo should act as wires
    parameter int INTER_KERNEL_PIPELINING = 0,          //how much pipelining to add before the single fifo that holds all of the capacity for iowr, channel and iord

    //others
    parameter bit ALLOW_HIGH_SPEED_FIFO_USAGE = 0,      //choice of hld_fifo style, 0 = mid speed fifo, 1 = high speed fifo
    parameter bit ACL_PROFILE = 0,                      //enable the profiler
    parameter     enable_ecc = "FALSE",                 //enable error correction codes for RAM, legal values are "FALSE" or "TRUE"
    parameter     INTENDED_DEVICE_FAMILY = "Arria 10"   //which FPGA family to use
) (
    input  wire                 clock,
    input  wire                 resetn,

    //write side of fifo -- beware the naming, think of "avst_in" as the interface name, "in" in the name does not mean input
    //all interfaces are stall/valid unless the fifo is put into bypass mode (but in that case iord and iowr talk to each other directly, so channel doesn't care if handshake is stall latency or not)
    input  wire                 avst_in_valid,
    input  wire    [DATA_W-1:0] avst_in_data,
    output logic                avst_in_ready,
    output logic                almost_full,
    output logic                avst_in_channel_stall,  //to iowr, iord tells iowr when the channel interface would have been stalling upstream, intended for profiler

    //read side of fifo
    output logic                avst_out_valid,
    output logic   [DATA_W-1:0] avst_out_data,
    input  wire                 avst_out_ready,
    input  wire                 avst_out_almost_full,   //provided by iord, goes out through almost_full, used to predicate writes into non-blocking iowr if channel won't have space (where channel is implemented in iord)
    input  wire                 avst_out_channel_stall, //from iord, iord tells iowr when the channel interface would have been stalling upstream, intended for profiler

    //others
    output logic          [1:0] ecc_err_status,         //error correction code status
    output logic         [31:0] profile_fifosize,       //occupancy of the channel fifo
    input  wire          [31:0] avst_in_fifosize,       //the fifosize from iowr, in case it took up the channel capacity
    input  wire          [31:0] avst_out_fifosize       //the fifosize from iord, in case it took up the channel capacity
);

    //convert from ready to stall semantics
    logic avst_in_stall, avst_out_stall;
    assign avst_in_ready = ~avst_in_stall;
    assign avst_out_stall = ~avst_out_ready;

    assign avst_in_channel_stall = avst_out_channel_stall;



    //reset
    logic aclrn, sclrn, resetn_synchronized;
    acl_reset_handler
    #(
        .ASYNC_RESET            (ASYNC_RESET),
        .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
        .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
        .PIPE_DEPTH             (1),
        .NUM_COPIES             (1)
    )
    acl_reset_handler_inst
    (
        .clk                    (clock),
        .i_resetn               (resetn),
        .o_aclrn                (aclrn),
        .o_resetn_synchronized  (resetn_synchronized),
        .o_sclrn                (sclrn)
    );


    //profiler is interested in the occupancy of the channel fifo
    generate
    if (ACL_PROFILE) begin : GEN_PROFILER
        if (!BYPASS_CHANNEL) begin : GEN_FIFOSIZE
            logic incr, decr;
            always_ff @(posedge clock or negedge aclrn) begin
                if (~aclrn) begin
                    incr <= 1'b0;
                    decr <= 1'b0;
                    profile_fifosize <= '0;
                end
                else begin
                    incr <= avst_in_valid & avst_in_ready;      //write into fifo
                    decr <= avst_out_valid & avst_out_ready;    //read from fifo
                    profile_fifosize <= profile_fifosize + incr - decr; //how many words are inside the fifo
                    if (~sclrn) begin
                        incr <= 1'b0;
                        decr <= 1'b0;
                        profile_fifosize <= '0;
                    end
                end
            end
        end
        else begin : OR_FIFOSIZE
            assign profile_fifosize = avst_in_fifosize | avst_out_fifosize;
        end
    end
    else begin : NO_PROFILER
        assign profile_fifosize = '0;
    end
    endgenerate


    generate
    if (BYPASS_CHANNEL) begin : GEN_BYPASS
        assign avst_out_valid = avst_in_valid;
        assign avst_out_data  = avst_in_data;
        assign avst_in_stall  = avst_out_stall;
        assign almost_full    = avst_out_almost_full;
    end
    else if (FIFO_DEPTH == 0) begin : GEN_STAGING_REG
        acl_staging_reg
        #(
            .WIDTH              (DATA_W),
            .ASYNC_RESET        (ASYNC_RESET),
            .SYNCHRONIZE_RESET  (SYNCHRONIZE_RESET)
        )
        acl_staging_reg_inst
        (
            .clk      (clock),
            .reset    (~resetn),
            .i_data   (avst_in_data),
            .i_valid  (avst_in_valid),
            .o_stall  (avst_in_stall),
            .o_data   (avst_out_data),
            .o_valid  (avst_out_valid),
            .i_stall  (avst_out_stall)
        );
        assign almost_full = avst_in_stall;
        assign ecc_err_status = 2'h0;
    end
    else begin : GEN_HLD_FIFO
        //to save area, using several shallow back-to-back FIFOs instead of one deep FIFO, note this transform only occurs if the depth exceeds 2048
        //this is beneficial for several reasons:
        //- hld_fifo requires a power of 2 memory backing due to the LFSR address, this is inefficient if the FIFO depth is slightly larger than a power of 2
        //- for a giant FIFO requiring hundreds of M20K, several back-to-back FIFOs is easier for Quartus to physically place and route
        //- for A10 and older device families which support very tall and narrow M20K geometries, avoid making the M20K so tall that we lose access to all 20k bits per memory
        //- for S10 and newer device families, avoid tall RAMs to eliminate the read data mux
        //however, there are some disadvantages:
        //- higher latency through all the back-to-back FIFOs, however channels are supposed to be latency insensitive anyways
        //- replicated logic for address control of each FIFO, however this is probably cheaper than the read data mux on a wide data path (S10 and newer)
        localparam bit DEVICE_FAMILY_A10_OR_OLDER = (INTENDED_DEVICE_FAMILY == "Cyclone 10 GX") || (INTENDED_DEVICE_FAMILY == "Arria 10");  //from hld_ram
        localparam int MAX_FIFO_DEPTH     = (DEVICE_FAMILY_A10_OR_OLDER) ? 4096 : 2048; //limit to 4k to maximize usable memory (at 8k can only get 16k bits per ram), on s10 and newer max physical depth is 2048
        localparam int NUM_FIFOS_NEEDED   = (FIFO_DEPTH + MAX_FIFO_DEPTH - 1) / MAX_FIFO_DEPTH;
        localparam int DEPTH_PER_FIFO     = (FIFO_DEPTH <= 32) ? 32 : (FIFO_DEPTH <= 512) ? 512 : (FIFO_DEPTH <= 1024) ? 1024 : (FIFO_DEPTH <= 2048 || MAX_FIFO_DEPTH == 2048) ? 2048 : 4096; //use full depth of mlab or m20k
        localparam int ALMOST_FULL_CUTOFF = FIFO_DEPTH - ALMOST_FULL_VALUE;

        genvar             g;
        logic              fifo_valid          [NUM_FIFOS_NEEDED:0];
        logic [DATA_W-1:0] fifo_data           [NUM_FIFOS_NEEDED:0];
        logic              fifo_stall          [NUM_FIFOS_NEEDED:0];
        logic        [1:0] fifo_ecc_err_status [NUM_FIFOS_NEEDED:0];

        always_comb begin
            //avst_in interface connects to write side of first fifo
            fifo_valid[0]  = avst_in_valid;
            fifo_data[0]   = avst_in_data;
            avst_in_stall  = fifo_stall[0];

            //read side of last fifo connects to avst_out interface
            avst_out_valid = fifo_valid[NUM_FIFOS_NEEDED];
            avst_out_data  = fifo_data[NUM_FIFOS_NEEDED];
            fifo_stall[NUM_FIFOS_NEEDED] = avst_out_stall;

            //error correction codes
            ecc_err_status = 2'h0;
            for (int i=0; i<NUM_FIFOS_NEEDED; i++) ecc_err_status |= fifo_ecc_err_status[i];
        end
        for (g=0; g<NUM_FIFOS_NEEDED; g++) begin : GEN_FIFO

            //reset -- every fifo and its related logic gets its own reset
            //the whole point of depth slicing is to avoid floorplan problems, don't let reset be a problem

            if (g==0) begin
                acl_tessellated_incr_decr_threshold #(
                    .CAPACITY                   (DEPTH_PER_FIFO),
                    .THRESHOLD                  (DEPTH_PER_FIFO - ALMOST_FULL_CUTOFF),
                    .THRESHOLD_REACHED_AT_RESET (1),
                    .ASYNC_RESET                (ASYNC_RESET),
                    .SYNCHRONIZE_RESET          (0)
                )
                almost_full_inst
                (
                    .clock                      (clock),
                    .resetn                     (resetn_synchronized),
                    .incr_no_overflow           (fifo_valid[g] & ~fifo_stall[g]),
                    .incr_raw                   (fifo_valid[g] & ~fifo_stall[g]),
                    .decr_no_underflow          (fifo_valid[g+1] & ~fifo_stall[g+1]),
                    .decr_raw                   (fifo_valid[g+1] & ~fifo_stall[g+1]),
                    .threshold_reached          (almost_full)
                );
            end

            genvar gg;
            logic              delayed_forced_write;
            logic [DATA_W-1:0] delayed_data;
            logic              pipe_forced_write [INTER_KERNEL_PIPELINING:0];
            logic [DATA_W-1:0] pipe_data         [INTER_KERNEL_PIPELINING:0];

            always_comb begin
                pipe_forced_write[0] = fifo_valid[g] & ~fifo_stall[g];
                pipe_data[0] = fifo_data[g];
            end
            for (gg=1; gg<=INTER_KERNEL_PIPELINING; gg++) begin :GEN_RANDOM_BLOCK_NAME_R0
                always_ff @(posedge clock or negedge aclrn) begin
                    if (~aclrn) pipe_forced_write[gg] <= 1'b0;
                    else begin
                        pipe_forced_write[gg] <= pipe_forced_write[gg-1];
                        if (~sclrn) pipe_forced_write[gg] <= 1'b0;
                    end
                end
                always_ff @(posedge clock) begin
                    pipe_data[gg] <= pipe_data[gg-1];
                end
            end
            assign delayed_forced_write = pipe_forced_write[INTER_KERNEL_PIPELINING];
            assign delayed_data = pipe_data[INTER_KERNEL_PIPELINING];

            //the inter kernel pipeline stages before hld_fifo as well as hld_fifo itself are treated as one "logical fifo"
            //to generate backpressure for the logical fifo, we must reserve space as soon as data enters the inter kernel pipelining (before it reaches the fifo)
            //as an analogy this is similar to a stall free cluster, space is reserved when an item enters the cluster, not when it arrives at the exit fifo
            acl_tessellated_incr_decr_threshold #(
                .CAPACITY                   (DEPTH_PER_FIFO),
                .THRESHOLD                  (DEPTH_PER_FIFO),
                .THRESHOLD_REACHED_AT_RESET (1),
                .ASYNC_RESET                (ASYNC_RESET),
                .SYNCHRONIZE_RESET          (0)
            )
            full_inst
            (
                .clock                      (clock),
                .resetn                     (resetn_synchronized),
                .incr_no_overflow           (fifo_valid[g] & ~fifo_stall[g]),
                .incr_raw                   (fifo_valid[g] & ~fifo_stall[g]),
                .decr_no_underflow          (fifo_valid[g+1] & ~fifo_stall[g+1]),
                .decr_raw                   (fifo_valid[g+1] & ~fifo_stall[g+1]),
                .threshold_reached          (fifo_stall[g])
            );

            hld_fifo
            #(
                .WIDTH                          (DATA_W),
                .REGISTERED_DATA_OUT_COUNT      (DATA_W),
                .DEPTH                          (DEPTH_PER_FIFO),
                .ASYNC_RESET                    (ASYNC_RESET),
                .SYNCHRONIZE_RESET              (0),
                .USE_STALL_LATENCY_UPSTREAM     (1),
                .NEVER_OVERFLOWS                (1),
                .STYLE                          (ALLOW_HIGH_SPEED_FIFO_USAGE ? "hs" : "ms"),
                .enable_ecc                     (enable_ecc)
            )
            hld_fifo_inst
            (
                .clock                          (clock),
                .resetn                         (resetn_synchronized),
                .i_valid                        (delayed_forced_write),
                .i_data                         (delayed_data),
                .o_valid                        (fifo_valid[g+1]),
                .o_data                         (fifo_data[g+1]),
                .i_stall                        (fifo_stall[g+1]),
                .ecc_err_status                 (fifo_ecc_err_status[g])
            );
        end
    end
    endgenerate

endmodule

`default_nettype wire

