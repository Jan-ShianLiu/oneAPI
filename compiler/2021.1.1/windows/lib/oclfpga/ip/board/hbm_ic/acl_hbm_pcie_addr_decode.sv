//__DESIGN_FILE_COPYRIGHT__
`default_nettype none
`include "acl_parameter_assert.svh"
`include "acl_width_clip.svh"

//this module is a part of acl_hbm_interconnect, it serves as the top level module for pcie address decoding
//this module itself does the following:
// - burst split, both reads and writes, best to do this in one place rather than after address decoding
// - decode whether a transaction is destined for the bottom or top hbm, no decoding is done within each half
// - pipeline stages between here and each instance of acl_hbm_pcie_addr_decode_half

module acl_hbm_pcie_addr_decode #(
    parameter int unsigned NUM_HBM,                 //total number of hbm interfaces
    parameter int unsigned NUM_HBM_BOTTOM,          //the first NUM_HBM_BOTTOM HBM interfaces will get mapped to the bottom instance, can infer NUM_HBM_TOP as NUM_HBM - NUM_HBM_BOTTOM
    parameter int unsigned ADDRESS_WIDTH,           //address width for kernel and hbm, pcie is wider
    parameter int unsigned BURSTCOUNT_WIDTH,
    parameter int unsigned BYTEENABLE_WIDTH,
    parameter bit          USE_WRITE_ACK,
    
    //pipeline stages between bottom/top decode and fifos
    parameter int unsigned PIPE_STAGES_PCIE_TO_BOTTOM_FIFO,
    parameter int unsigned PIPE_STAGES_BOTTOM_FIFO_TO_PCIE,
    parameter int unsigned PIPE_STAGES_PCIE_TO_TOP_FIFO,
    parameter int unsigned PIPE_STAGES_TOP_FIFO_TO_PCIE,
    
    //bottom/top fifo settings
    parameter int unsigned PCIE_BOTTOM_FIFO_DEPTH,
    parameter int unsigned PCIE_TOP_FIFO_DEPTH,
    parameter bit          PCIE_BOTTOM_FIFO_USE_MLAB,
    parameter bit          PCIE_TOP_FIFO_USE_MLAB,
    
    //clock crossing fifo settings
    parameter int unsigned CMD_DCFIFO_DEPTH,
    parameter int unsigned RSP_DCFIFO_DEPTH,
    parameter int unsigned CMD_DCFIFO_USE_MLAB,     //bit i indicates whether command dcfifo index i should use mlab (1) or m20k (0)
    parameter int unsigned RSP_DCFIFO_USE_MLAB,
    
    //derived parameters
    localparam int unsigned NUM_HBM_TOP = NUM_HBM - NUM_HBM_BOTTOM,
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH,
    localparam int unsigned PCIE_ADDRESS_WIDTH = ADDRESS_WIDTH + $clog2(NUM_HBM)    //address width for pcie is wider since the upper $clog2(NUM_HBM) bits indicate which HBM to target
) (
    input  wire                           p_clock,
    input  wire                           p_resetn,
    input  wire                           h_b_clock,
    input  wire                           h_b_resetn,
    input  wire                           h_t_clock,
    input  wire                           h_t_resetn,
    
    //pcie interface to board.qsys
    input  wire                           p_write,
    input  wire                           p_read,
    input  wire  [PCIE_ADDRESS_WIDTH-1:0] p_address,
    input  wire    [BURSTCOUNT_WIDTH-1:0] p_burstcount,
    input  wire          [DATA_WIDTH-1:0] p_writedata,
    input  wire    [BYTEENABLE_WIDTH-1:0] p_byteenable,
    output logic                          p_waitrequest,
    output logic                          p_readdatavalid,
    output logic         [DATA_WIDTH-1:0] p_readdata,
    output logic                          p_writeack,
    
    //after address decode and clock crosser -- connects to arb between kernel/pcie
    output logic                          h_write         [NUM_HBM-1:0],
    output logic                          h_read          [NUM_HBM-1:0],
    output logic      [ADDRESS_WIDTH-1:0] h_address       [NUM_HBM-1:0],
    output logic         [DATA_WIDTH-1:0] h_writedata     [NUM_HBM-1:0],
    output logic   [BYTEENABLE_WIDTH-1:0] h_byteenable    [NUM_HBM-1:0],
    input  wire                           h_waitrequest   [NUM_HBM-1:0],
    input  wire                           h_readdatavalid [NUM_HBM-1:0],
    input  wire          [DATA_WIDTH-1:0] h_readdata      [NUM_HBM-1:0],
    input  wire                           h_writeack      [NUM_HBM-1:0],
    output logic                          h_empty         [NUM_HBM-1:0]     //for arb bewteen pcie and kernel
);
    
    //////////////////////////////////////
    //                                  //
    //  Sanity check on the parameters  //
    //                                  //
    //////////////////////////////////////
    
    generate
    `ACL_PARAMETER_ASSERT(NUM_HBM >= 1)
    `ACL_PARAMETER_ASSERT(NUM_HBM_BOTTOM <= NUM_HBM)
    `ACL_PARAMETER_ASSERT(ADDRESS_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BURSTCOUNT_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH == 2**$clog2(BYTEENABLE_WIDTH))
    endgenerate
    
    
    
    ///////////////////////////
    //                       //
    //  Signal declarations  //
    //                       //
    ///////////////////////////
    
    genvar g;
    
    //reset
    logic                          p_sclrn;
    
    //after burst splitter
    logic                          p_split_write;
    logic                          p_split_read;
    logic [PCIE_ADDRESS_WIDTH-1:0] p_split_address;
    logic         [DATA_WIDTH-1:0] p_split_writedata;
    logic   [BYTEENABLE_WIDTH-1:0] p_split_byteenable;
    logic                          p_split_waitrequest;
    
    //decode commands into bottom and top
    logic                          p_split_address_is_top;
    logic                          p_bottom_valid, p_top_valid;
    logic                          p_bottom_waitrequest, p_top_waitrequest;
    
    //collect responses from bottom and top
    logic                          p_bottom_readdatavalid, p_top_readdatavalid;
    logic                          p_bottom_writeack, p_top_writeack;
    logic         [DATA_WIDTH-1:0] p_bottom_readdata, p_top_readdata;
    
    //bottom hbm interface
    logic                          h_bottom_write         [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic                          h_bottom_read          [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic      [ADDRESS_WIDTH-1:0] h_bottom_address       [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic         [DATA_WIDTH-1:0] h_bottom_writedata     [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic   [BYTEENABLE_WIDTH-1:0] h_bottom_byteenable    [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic                          h_bottom_waitrequest   [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic                          h_bottom_readdatavalid [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic         [DATA_WIDTH-1:0] h_bottom_readdata      [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic                          h_bottom_writeack      [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    logic                          h_bottom_empty         [`ACL_WIDTH_CLIP(NUM_HBM_BOTTOM)];
    
    //top hbm interface
    logic                          h_top_write            [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic                          h_top_read             [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic      [ADDRESS_WIDTH-1:0] h_top_address          [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic         [DATA_WIDTH-1:0] h_top_writedata        [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic   [BYTEENABLE_WIDTH-1:0] h_top_byteenable       [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic                          h_top_waitrequest      [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic                          h_top_readdatavalid    [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic         [DATA_WIDTH-1:0] h_top_readdata         [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic                          h_top_writeack         [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    logic                          h_top_empty            [`ACL_WIDTH_CLIP(NUM_HBM_TOP)];
    
    
    
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
    
    
    
    //////////////////////
    //                  //
    //  Burst splitter  //
    //                  //
    //////////////////////
    
    //split bursts, both read and write, better to do it here once instead of at each interface after address decode
    //make no assumption about the burst alignment, we need the full address adder width, okay for fmax since on the slower pcie clock
    
    acl_burst_splitter #(
        .ADDRESS_WIDTH              (PCIE_ADDRESS_WIDTH),
        .BURSTCOUNT_WIDTH           (BURSTCOUNT_WIDTH),
        .BYTEENABLE_WIDTH           (BYTEENABLE_WIDTH),
        .BURST_BOUNDARY             (0),                //from pcie we are not guaranteed anything about bursts staying within some boundary
        .ASYNC_RESET                (0),
        .SYNCHRONIZE_RESET          (1),
        .BACKPRESSURE_DURING_RESET  (0)
    )
    acl_burst_splitter_inst
    (
        .clock                      (p_clock),
        .resetn                     (p_resetn),
        
        .up_waitrequest             (p_waitrequest),
        .up_read                    (p_read),
        .up_write                   (p_write),
        .up_address                 (p_address),
        .up_writedata               (p_writedata),
        .up_byteenable              (p_byteenable),
        .up_burstcount              (p_burstcount),
        
        .down_waitrequest           (p_split_waitrequest),
        .down_read                  (p_split_read),
        .down_write                 (p_split_write),
        .down_address               (p_split_address),
        .down_writedata             (p_split_writedata),
        .down_byteenable            (p_split_byteenable),
        .down_burstcount            ()      //will be a constant 1
    );
    
    
    
    ///////////////////////////////////////////
    //                                       //
    //  Decode commands into bottom and top  //
    //                                       //
    ///////////////////////////////////////////
    
    //backpressure pcie if either bottom or top fifo is almost full
    assign p_split_waitrequest = p_bottom_waitrequest | p_top_waitrequest;
    
    //decode valid = forced transaction for bottom and top
    assign p_split_address_is_top = (p_split_address >> ADDRESS_WIDTH) >= NUM_HBM_BOTTOM;
    assign p_bottom_valid = (p_split_write | p_split_read) & ~p_split_waitrequest & ~p_split_address_is_top;
    assign p_top_valid    = (p_split_write | p_split_read) & ~p_split_waitrequest &  p_split_address_is_top;
    
    
    
    /////////////////////////////////////////////
    //                                         //
    //  Collect responses from bottom and top  //
    //                                         //
    /////////////////////////////////////////////
    
    //no collision handling
    
    //response path to pcie
    always_ff @(posedge p_clock) begin
        p_readdatavalid <= p_bottom_readdatavalid | p_top_readdatavalid;
        p_readdata <= (p_bottom_readdatavalid) ? p_bottom_readdata : p_top_readdata;
        if (~p_sclrn) p_readdatavalid <= 1'b0;
    end
    
    //writeack to pcie
    generate
    if (USE_WRITE_ACK) begin
        always_ff @(posedge p_clock) begin
            p_writeack <= p_bottom_writeack | p_top_writeack;
            if (~p_sclrn) p_writeack <= 1'b0;
        end
    end
    else begin
        assign p_writeack = 1'b0;
    end
    endgenerate
    
    
    
    //re-map the 2d array into two 2d arrays
    generate
    for (g=0; g<NUM_HBM; g++) begin : CONNECT_HBM
        assign h_write[g]      = (g<NUM_HBM_BOTTOM) ? h_bottom_write[g]      : h_top_write[g-NUM_HBM_BOTTOM];
        assign h_read[g]       = (g<NUM_HBM_BOTTOM) ? h_bottom_read[g]       : h_top_read[g-NUM_HBM_BOTTOM];
        assign h_address[g]    = (g<NUM_HBM_BOTTOM) ? h_bottom_address[g]    : h_top_address[g-NUM_HBM_BOTTOM];
        assign h_writedata[g]  = (g<NUM_HBM_BOTTOM) ? h_bottom_writedata[g]  : h_top_writedata[g-NUM_HBM_BOTTOM];
        assign h_byteenable[g] = (g<NUM_HBM_BOTTOM) ? h_bottom_byteenable[g] : h_top_byteenable[g-NUM_HBM_BOTTOM];
        assign h_empty[g]      = (g<NUM_HBM_BOTTOM) ? h_bottom_empty[g]      : h_top_empty[g-NUM_HBM_BOTTOM];
        if (g < NUM_HBM_BOTTOM) begin
            assign h_bottom_waitrequest[g]   = h_waitrequest[g];
            assign h_bottom_readdatavalid[g] = h_readdatavalid[g];
            assign h_bottom_readdata[g]      = h_readdata[g];
            assign h_bottom_writeack[g]      = h_writeack[g];
        end
        else begin
            assign h_top_waitrequest[g-NUM_HBM_BOTTOM]   = h_waitrequest[g];
            assign h_top_readdatavalid[g-NUM_HBM_BOTTOM] = h_readdatavalid[g];
            assign h_top_readdata[g-NUM_HBM_BOTTOM]      = h_readdata[g];
            assign h_top_writeack[g-NUM_HBM_BOTTOM]      = h_writeack[g];
        end
    end
    endgenerate
    
    
    generate
    if (NUM_HBM_BOTTOM >= 1) begin : GEN_BOTTOM
        logic                          p_pipe_bottom_valid;
        logic                          p_pipe_bottom_is_write;
        logic [PCIE_ADDRESS_WIDTH-1:0] p_pipe_bottom_address;
        logic [DATA_WIDTH-1:0]         p_pipe_bottom_writedata;
        logic [BYTEENABLE_WIDTH-1:0]   p_pipe_bottom_byteenable;
        logic                          p_pipe_bottom_waitrequest;
        logic                          p_pipe_bottom_readdatavalid;
        logic [DATA_WIDTH-1:0]         p_pipe_bottom_readdata;
        logic                          p_pipe_bottom_writeack;
        
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (2 + PCIE_ADDRESS_WIDTH + DATA_WIDTH + BYTEENABLE_WIDTH),
            .STAGES (PIPE_STAGES_PCIE_TO_BOTTOM_FIFO)
        )
        pipeline_pcie_to_bottom_fifo
        (
            .clock  (p_clock),
            .D      ({     p_bottom_valid,         p_split_write,        p_split_address,       p_split_writedata,       p_split_byteenable}),
            .Q      ({p_pipe_bottom_valid, p_pipe_bottom_is_write, p_pipe_bottom_address, p_pipe_bottom_writedata, p_pipe_bottom_byteenable})
        );
        
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (1),
            .STAGES (PIPE_STAGES_BOTTOM_FIFO_TO_PCIE)
        )
        pipeline_bottom_fifo_to_pcie_backpressure
        (
            .clock  (p_clock),
            .D      (p_pipe_bottom_waitrequest),
            .Q      (p_bottom_waitrequest)
        );
        
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (2 + DATA_WIDTH),
            .STAGES (PIPE_STAGES_BOTTOM_FIFO_TO_PCIE)   //TODO make this another parameter
        )
        pipeline_bottom_fifo_to_pcie_response
        (
            .clock  (p_clock),
            .D      ({p_pipe_bottom_writeack, p_pipe_bottom_readdatavalid, p_pipe_bottom_readdata}),
            .Q      ({     p_bottom_writeack,      p_bottom_readdatavalid,      p_bottom_readdata})
        );
        
        acl_hbm_pcie_addr_decode_half #(
            .NUM_HBM                    (NUM_HBM),
            .NUM_HBM_BOTTOM             (NUM_HBM_BOTTOM),
            .IS_TOP                     (0),
            .ADDRESS_WIDTH              (ADDRESS_WIDTH),
            .BYTEENABLE_WIDTH           (BYTEENABLE_WIDTH),
            .USE_WRITE_ACK              (USE_WRITE_ACK),
            .PCIE_FIFO_DEPTH            (PCIE_BOTTOM_FIFO_DEPTH),
            .PCIE_FIFO_USE_MLAB         (PCIE_BOTTOM_FIFO_USE_MLAB),
            .PCIE_FIFO_ALMOST_FULL      (PIPE_STAGES_PCIE_TO_BOTTOM_FIFO + PIPE_STAGES_BOTTOM_FIFO_TO_PCIE),
            .CMD_DCFIFO_DEPTH           (CMD_DCFIFO_DEPTH),
            .RSP_DCFIFO_DEPTH           (RSP_DCFIFO_DEPTH),
            .CMD_DCFIFO_USE_MLAB        (CMD_DCFIFO_USE_MLAB),
            .RSP_DCFIFO_USE_MLAB        (RSP_DCFIFO_USE_MLAB)
        )
        bottom_inst
        (
            .p_clock                    (p_clock),
            .p_resetn                   (p_resetn),
            .h_clock                    (h_b_clock),
            .h_resetn                   (h_b_resetn),
            
            //pcie after burst split and decoding for bottom/top
            .p_valid                    (p_pipe_bottom_valid),           //forced transaction
            .p_is_write                 (p_pipe_bottom_is_write),            //this is considered data path, not control
            .p_address                  (p_pipe_bottom_address),
            .p_writedata                (p_pipe_bottom_writedata),
            .p_byteenable               (p_pipe_bottom_byteenable),
            .p_waitrequest              (p_pipe_bottom_waitrequest),
            .p_readdatavalid            (p_pipe_bottom_readdatavalid),
            .p_readdata                 (p_pipe_bottom_readdata),
            .p_writeack                 (p_pipe_bottom_writeack),
            
            //after distribution to each hbm by shiftreg and clock crossed into hbm clock
            .h_write                    (h_bottom_write),
            .h_read                     (h_bottom_read),
            .h_address                  (h_bottom_address),
            .h_writedata                (h_bottom_writedata),
            .h_byteenable               (h_bottom_byteenable),
            .h_waitrequest              (h_bottom_waitrequest),
            .h_readdatavalid            (h_bottom_readdatavalid),
            .h_readdata                 (h_bottom_readdata),
            .h_writeack                 (h_bottom_writeack),
            .h_empty                    (h_bottom_empty)
        );
    end
    else begin : NO_BOTTOM
        assign p_bottom_waitrequest = 1'b0;
        assign p_bottom_readdatavalid = 1'b0;
        assign p_bottom_writeack = 1'b0;
    end
    endgenerate
    
    
    
    generate
    if (NUM_HBM_TOP >= 1) begin : GEN_TOP
        logic                          p_pipe_top_valid;
        logic                          p_pipe_top_is_write;
        logic [PCIE_ADDRESS_WIDTH-1:0] p_pipe_top_address;
        logic [DATA_WIDTH-1:0]         p_pipe_top_writedata;
        logic [BYTEENABLE_WIDTH-1:0]   p_pipe_top_byteenable;
        logic                          p_pipe_top_waitrequest;
        logic                          p_pipe_top_readdatavalid;
        logic [DATA_WIDTH-1:0]         p_pipe_top_readdata;
        logic                          p_pipe_top_writeack;
        
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (2 + PCIE_ADDRESS_WIDTH + DATA_WIDTH + BYTEENABLE_WIDTH),
            .STAGES (PIPE_STAGES_PCIE_TO_TOP_FIFO)
        )
        pipeline_pcie_to_top_fifo
        (
            .clock  (p_clock),
            .D      ({     p_top_valid,         p_split_write,        p_split_address,       p_split_writedata,       p_split_byteenable}),
            .Q      ({p_pipe_top_valid, p_pipe_top_is_write, p_pipe_top_address, p_pipe_top_writedata, p_pipe_top_byteenable})
        );
        
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (1),
            .STAGES (PIPE_STAGES_TOP_FIFO_TO_PCIE)
        )
        pipeline_top_fifo_to_pcie_backpressure
        (
            .clock  (p_clock),
            .D      (p_pipe_top_waitrequest),
            .Q      (p_top_waitrequest)
        );
        
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (2 + DATA_WIDTH),
            .STAGES (PIPE_STAGES_TOP_FIFO_TO_PCIE)   //TODO make this another parameter
        )
        pipeline_top_fifo_to_pcie_response
        (
            .clock  (p_clock),
            .D      ({p_pipe_top_writeack, p_pipe_top_readdatavalid, p_pipe_top_readdata}),
            .Q      ({     p_top_writeack,      p_top_readdatavalid,      p_top_readdata})
        );
        
        acl_hbm_pcie_addr_decode_half #(
            .NUM_HBM                    (NUM_HBM),
            .NUM_HBM_BOTTOM             (NUM_HBM_BOTTOM),
            .IS_TOP                     (1),
            .ADDRESS_WIDTH              (ADDRESS_WIDTH),
            .BYTEENABLE_WIDTH           (BYTEENABLE_WIDTH),
            .USE_WRITE_ACK              (USE_WRITE_ACK),
            .PCIE_FIFO_DEPTH            (PCIE_TOP_FIFO_DEPTH),
            .PCIE_FIFO_USE_MLAB         (PCIE_TOP_FIFO_USE_MLAB),
            .PCIE_FIFO_ALMOST_FULL      (PIPE_STAGES_PCIE_TO_TOP_FIFO + PIPE_STAGES_TOP_FIFO_TO_PCIE),
            .CMD_DCFIFO_DEPTH           (CMD_DCFIFO_DEPTH),
            .RSP_DCFIFO_DEPTH           (RSP_DCFIFO_DEPTH),
            .CMD_DCFIFO_USE_MLAB        (CMD_DCFIFO_USE_MLAB),
            .RSP_DCFIFO_USE_MLAB        (RSP_DCFIFO_USE_MLAB)
        )
        top_inst
        (
            .p_clock                    (p_clock),
            .p_resetn                   (p_resetn),
            .h_clock                    (h_t_clock),
            .h_resetn                   (h_t_resetn),
            
            //pcie after burst split and decoding for bottom/top
            .p_valid                    (p_pipe_top_valid),          //forced transaction
            .p_is_write                 (p_pipe_top_is_write),        //this is considered data path, not control
            .p_address                  (p_pipe_top_address),
            .p_writedata                (p_pipe_top_writedata),
            .p_byteenable               (p_pipe_top_byteenable),
            .p_waitrequest              (p_pipe_top_waitrequest),
            .p_readdatavalid            (p_pipe_top_readdatavalid),
            .p_readdata                 (p_pipe_top_readdata),
            .p_writeack                 (p_pipe_top_writeack),
            
            //after distribution to each hbm by shiftreg and clock crossed into hbm clock
            .h_write                    (h_top_write),
            .h_read                     (h_top_read),
            .h_address                  (h_top_address),
            .h_writedata                (h_top_writedata),
            .h_byteenable               (h_top_byteenable),
            .h_waitrequest              (h_top_waitrequest),
            .h_readdatavalid            (h_top_readdatavalid),
            .h_readdata                 (h_top_readdata),
            .h_writeack                 (h_top_writeack),
            .h_empty                    (h_top_empty)
        );  
    end
    else begin : NO_TOP
        assign p_top_waitrequest = 1'b0;
        assign p_top_readdatavalid = 1'b0;
        assign p_top_writeack = 1'b0;
    end
    endgenerate
    
endmodule

`default_nettype wire
