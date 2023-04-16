//__DESIGN_FILE_COPYRIGHT__
`default_nettype none
`include "acl_parameter_assert.svh"

//this module is part of acl_hbm_pcie_addr_decode_half, which is part of acl_hbm_interconnect
//this module deals with the start and end of the shift register
//after the bottom/top decode and the pipeline stages, transactions land in a fifo, that fifo lives inside this module
//data read from the fifo is then address decoded, this acts as the start of the command shiftreg
//the end of the response shiftreg comes into this module, this goes into the shared response dcfifo which lives inside this module
//finally, this module also manages the throttle logic to ensure the response dcfifo never overflows by limiting the number of oustanding transactions

module acl_hbm_pcie_addr_decode_terminal #(
    parameter int unsigned NUM_HBM,                 //total number of hbm interfaces
    parameter int unsigned NUM_HBM_BOTTOM,          //the first NUM_HBM_BOTTOM HBM interfaces will get mapped to the bottom instance
    parameter bit          IS_TOP,                  //0 for bottom, 1 for top
    parameter int unsigned ADDRESS_WIDTH,           //address width for kernel and hbm, pcie is wider
    parameter int unsigned BYTEENABLE_WIDTH,
    parameter bit          USE_WRITE_ACK,
    
    //after decode bottom/top and the pipeline stages that follow, transactions land in a fifo (one for each bottom/top half), configure this fifo
    parameter int unsigned PCIE_FIFO_DEPTH,
    parameter bit          PCIE_FIFO_USE_MLAB,
    parameter int unsigned PCIE_FIFO_ALMOST_FULL,
    
    //clock crossing fifo settings
    parameter int unsigned RSP_DCFIFO_DEPTH,
    parameter int unsigned RSP_DCFIFO_USE_MLAB,     //bit i indicates whether command dcfifo index i should use mlab (1) or m20k (0), note that indexing starts at NUM_HBM_START_INDEX
    
    //derived parameters
    localparam int unsigned NUM_HBM_THIS_HALF = (IS_TOP) ? (NUM_HBM - NUM_HBM_BOTTOM) : NUM_HBM_BOTTOM, //how many hbm interfaces in this half
    localparam int unsigned NUM_HBM_START_INDEX = (IS_TOP) ? NUM_HBM_BOTTOM : 0,                        //index of the first hbm interface in this half
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH,
    localparam int unsigned PCIE_ADDRESS_WIDTH = ADDRESS_WIDTH + $clog2(NUM_HBM),                       //address width for pcie is wider since the upper $clog2(NUM_HBM) bits indicate which HBM to target
    localparam int unsigned ADDRESS_BITS_PER_WORD = $clog2(BYTEENABLE_WIDTH),                           //how many lower bits of the byte address are stuck at 0 to ensure it is word aligned
    localparam int unsigned WORD_ADDRESS_WIDTH = ADDRESS_WIDTH - ADDRESS_BITS_PER_WORD,                 //convert byte address width to word address width
    localparam int unsigned PCIE_WORD_ADDRESS_WIDTH = PCIE_ADDRESS_WIDTH - ADDRESS_BITS_PER_WORD,
    localparam int unsigned RSP_COUNT_WIDTH = $clog2(RSP_DCFIFO_DEPTH) + 1                              //add 1 to width of counter for throttle logic so that we can use the MSB as the check for threshold reached
) (
    input  wire                           p_clock,
    input  wire                           p_resetn,
    input  wire                           h_clock,
    input  wire                           h_resetn,
    
    //pcie after burst split and decoding for bottom/top
    input  wire                           p_pcie_valid,        //forced transaction
    input  wire                           p_pcie_is_write,     //this is considered data path, not control
    input  wire  [PCIE_ADDRESS_WIDTH-1:0] p_pcie_address,
    input  wire  [DATA_WIDTH-1:0]         p_pcie_writedata,
    input  wire  [BYTEENABLE_WIDTH-1:0]   p_pcie_byteenable,
    output logic                          p_pcie_waitrequest,
    output logic                          p_pcie_readdatavalid,
    output logic [DATA_WIDTH-1:0]         p_pcie_readdata,
    output logic                          p_pcie_writeack,
    
    //pcie after decode - entrance to shiftreg
    output logic [NUM_HBM_THIS_HALF-1:0]  p_shiftreg_valid_one_hot,   //one bit per hbm interface
    output logic                          p_shiftreg_is_write,
    output logic [ADDRESS_WIDTH-1:0]      p_shiftreg_address,
    output logic [DATA_WIDTH-1:0]         p_shiftreg_writedata,
    output logic [BYTEENABLE_WIDTH-1:0]   p_shiftreg_byteenable,
    input  wire                           p_shiftreg_waitrequest,
    //beware the response path is on the hbm clock domain
    input  wire                           h_shiftreg_readdatavalid,
    input  wire  [DATA_WIDTH-1:0]         h_shiftreg_readdata,
    input  wire                           h_shiftreg_writeack
);
    
    //parameters were already sanity checked in acl_hbm_pcie_addr_decode_half
    
    ///////////////////////////
    //                       //
    //  Signal declarations  //
    //                       //
    ///////////////////////////
    
    //reset
    logic                               p_sclrn;
    
    //command interface, fifo and address decode
    logic [PCIE_WORD_ADDRESS_WIDTH-1:0] p_pcie_word_address;
    logic [PCIE_WORD_ADDRESS_WIDTH-1:0] p_shiftreg_word_address;
    logic                               p_fifo_valid;
    logic                               p_fifo_empty;
    
    //response interface, read side of response dcfifos
    logic                               p_readack_empty, p_writeack_empty;
    
    //throttle logic, track outstanding transactions to ensure response dcfifo never overflows, can't backpressure responses
    logic                               p_fifo_throttle;
    logic                               p_outstanding_reads_incr, p_outstanding_reads_decr;
    logic                               p_outstanding_writes_incr, p_outstanding_writes_decr;
    logic                         [1:0] p_outstanding_reads_update, p_outstanding_writes_update;
    logic         [RSP_COUNT_WIDTH-1:0] p_outstanding_reads, p_outstanding_writes;
    
    
    
    /////////////
    //         //
    //  Reset  //
    //         //
    /////////////
    
    acl_reset_handler
    #(
        .ASYNC_RESET            (0),
        .USE_SYNCHRONIZER       (1),
        .SYNCHRONIZE_ACLRN      (1),
        .PIPE_DEPTH             (2),
        .NUM_COPIES             (1)
    )
    acl_reset_handler_inst
    (
        .clk                    (p_clock),
        .i_resetn               (p_resetn),
        .o_aclrn                (),
        .o_resetn_synchronized  (),
        .o_sclrn                (p_sclrn)
    );
    
    
    
    /////////////////////////
    //                     //
    //  Command interface  //
    //                     //
    /////////////////////////
    
    // after the bottom/top decode and the pipeline stages, the transactions land in this fifo, fifo needed to produce almost full since pipeline stages add handshake latency
    // after the fifo, decode the upper bits of pcie address
    
    assign p_pcie_word_address = p_pcie_address[PCIE_ADDRESS_WIDTH-1:ADDRESS_BITS_PER_WORD];    //convert to word address - few bits to store in fifo
    
    hld_fifo #(
        .WIDTH              (1 + PCIE_WORD_ADDRESS_WIDTH + DATA_WIDTH + BYTEENABLE_WIDTH),
        .DEPTH              (PCIE_FIFO_DEPTH),
        .NEVER_OVERFLOWS    (1),
        .ALMOST_FULL_CUTOFF (PCIE_FIFO_ALMOST_FULL),
        .ASYNC_RESET        (0),
        .SYNCHRONIZE_RESET  (1),
        .STYLE              ("ms"),
        .RAM_BLOCK_TYPE     ((PCIE_FIFO_USE_MLAB) ? "MLAB" : "M20K")
    )
    hld_fifo_inst
    (
        .clock              (p_clock),
        .resetn             (p_resetn),
        .i_valid            (p_pcie_valid),
        .i_data             ({p_pcie_is_write, p_pcie_word_address, p_pcie_writedata, p_pcie_byteenable}),
        .o_stall            (),
        .o_almost_full      (p_pcie_waitrequest),
        .o_valid            (),
        .o_data             ({p_shiftreg_is_write, p_shiftreg_word_address, p_shiftreg_writedata, p_shiftreg_byteenable}),
        .i_stall            (p_shiftreg_waitrequest | p_fifo_throttle),
        .o_almost_empty     (),
        .o_empty            (p_fifo_empty),
        .ecc_err_status     ()
    );
    assign p_shiftreg_address = p_shiftreg_word_address << ADDRESS_BITS_PER_WORD;       //convert word address to byte address
    assign p_fifo_valid = ~p_fifo_empty & ~p_shiftreg_waitrequest & ~p_fifo_throttle;    //forced transaction
    
    //decode the upper bits of the address
    generate
    if (NUM_HBM_THIS_HALF == 1) begin : TRIVIAL_ADDRESS_DECODE
        assign p_shiftreg_valid_one_hot[0] = p_fifo_valid;
    end
    else begin : GEN_ADDRESS_DECODE
        always_comb begin
            for (int i=0; i<NUM_HBM_THIS_HALF; i++) begin
                p_shiftreg_valid_one_hot[i] = p_fifo_valid & (p_shiftreg_word_address[PCIE_WORD_ADDRESS_WIDTH-1:WORD_ADDRESS_WIDTH] == (i+NUM_HBM_START_INDEX));
            end
        end
    end
    endgenerate
    
    
    
    //////////////////////////
    //                      //
    //  Response interface  //
    //                      //
    //////////////////////////
    
    //clock cross the collected responses into pcie clock domain
    acl_dcfifo #(
        .WIDTH                  (DATA_WIDTH),
        .DEPTH                  (RSP_DCFIFO_DEPTH),
        .NEVER_OVERFLOWS        (1),
        .RAM_BLOCK_TYPE         ((RSP_DCFIFO_USE_MLAB[NUM_HBM_START_INDEX]) ? "MLAB" : "M20K")
    )
    rsp_dcfifo_readack
    (
        .async_resetn           (p_resetn & h_resetn),
        .wr_clock               (h_clock),
        .wr_req                 (h_shiftreg_readdatavalid),
        .wr_data                (h_shiftreg_readdata),
        .wr_full                (),
        .wr_almost_full         (),
        .wr_read_update_for_ccb (),
        .rd_clock               (p_clock),
        .rd_ack                 (1'b1),
        .rd_empty               (p_readack_empty),
        .rd_almost_empty        (),
        .rd_data                (p_pcie_readdata)
    );
    assign p_pcie_readdatavalid = ~p_readack_empty;
    
    generate
    if (USE_WRITE_ACK) begin : GEN_WRITE_ACK_FIFO
        acl_dcfifo #(
            .WIDTH                  (0),
            .DEPTH                  (RSP_DCFIFO_DEPTH),
            .NEVER_OVERFLOWS        (1)
        )
        rsp_dcfifo_writeack
        (
            .async_resetn           (p_resetn & h_resetn),
            .wr_clock               (h_clock),
            .wr_req                 (h_shiftreg_writeack),
            .wr_data                (),
            .wr_full                (),
            .wr_almost_full         (),
            .wr_read_update_for_ccb (),
            .rd_clock               (p_clock),
            .rd_ack                 (1'b1),
            .rd_empty               (p_writeack_empty),
            .rd_almost_empty        (),
            .rd_data                ()
        );
        assign p_pcie_writeack = ~p_writeack_empty;
    end
    else begin : NO_WRITE_ACK_FIFO
        assign p_pcie_writeack = 1'b0;
    end
    endgenerate
    
    
    
    ////////////////
    //            //
    //  Throttle  //
    //            //
    ////////////////
    
    //responses cannot be backpressured, so to ensure the response dcfifo never overflows, limit the number of outstanding transactions
    //something is considered an outstanding transaction once it leaves the above fifo, reclaim space once the readdatavalid exits from the shared response dcfifo
    //burstcount is known to be 1, so it's like the throttle logic from acl_clock_crossing_bridge but with MAX_BURST_SIZE = 1, identical pipelining to that
    
    //comments from acl_clock_crossing_bridge:
    
    // How lateness in the throttle logic affects the threshold:
    // - p_fifo_throttle is 3 clocks late (p_fifo_throttle is registerd, so one clock later than p_outstanding_reads)
    // - due to lateness, we need to reserve 3*MAX_BURST_SIZE of full-side dead space in the response fifo
    // - we reserve an additional MAX_BURST_SIZE because by the time the MSB of p_outstanding_reads asserts, we may have gone over by MAX_BURST_SIZE-1
    //   to illustrate this, think in base 10, the MSB asserts at a value of 100, MAX_BURST_SIZE is 10, and the counter is increasing like this: 79, 89, 99, 109
    // - total reserved space = 4*MAX_BURST_SIZE
    
    // Implementation:
    // - naive implementation would be to have a counter start at 0 and check if value is RSP_DCFIFO_DEPTH-4*MAX_BURST_SIZE or higher
    // - our implementation starts the counter at 4*MAX_BURST_SIZE and checks if the value is RSP_DCFIFO_DEPTH or higher
    // - since RSP_DCFIFO_DEPTH is a power of 2, just check if the MSB is 1
    
    always_ff @(posedge p_clock) begin
        p_outstanding_reads_incr <= p_fifo_valid & ~p_shiftreg_is_write;                        //on-time throttle -- transaction just went out this clock cycle, throttle as of the next clock cycle
        p_outstanding_reads_decr <= p_pcie_readdatavalid;
        p_outstanding_reads_update <= p_outstanding_reads_incr - p_outstanding_reads_decr;      //1 clock late
        p_outstanding_reads <= p_outstanding_reads + { {(RSP_COUNT_WIDTH-1){p_outstanding_reads_update[1]}}, p_outstanding_reads_update[0]};    //2 clocks late
        if (~p_sclrn) p_outstanding_reads <= 4;                                                 //offset so that the MSB detects almost full
    end
    
    //same as above but track outstanding writes intead of reads
    generate
    if (USE_WRITE_ACK) begin : GEN_WRITE_ACK_THROTTLE
        always_ff @(posedge p_clock) begin
            p_outstanding_writes_incr <= p_fifo_valid & p_shiftreg_is_write;
            p_outstanding_writes_decr <= p_pcie_writeack;
            p_outstanding_writes_update <= p_outstanding_writes_incr - p_outstanding_writes_decr;
            p_outstanding_writes <= p_outstanding_writes + { {(RSP_COUNT_WIDTH-1){p_outstanding_writes_update[1]}}, p_outstanding_writes_update[0]};
            if (~p_sclrn) p_outstanding_writes <= 4;
        end
    end
    else begin : NO_WRITE_ACK_THROTTLE
        assign p_outstanding_writes = '0;
    end
    endgenerate
    
    always_ff @(posedge p_clock) begin
        //too much oustanding stuff for some response dcfifo and just finished committing the current transaction
        if ((p_outstanding_reads[RSP_COUNT_WIDTH-1] | p_outstanding_writes[RSP_COUNT_WIDTH-1]) & ~p_shiftreg_waitrequest) p_fifo_throttle <= 1'b1;
        
        //if both response dcfifos will have space, allow transaction to be visible
        if (~p_outstanding_reads[RSP_COUNT_WIDTH-1] & ~p_outstanding_writes[RSP_COUNT_WIDTH-1]) p_fifo_throttle <= 1'b0;
        
        if (~p_sclrn) p_fifo_throttle <= 1'b0;
    end
    
    
endmodule

`default_nettype wire
