//__DESIGN_FILE_COPYRIGHT__
`default_nettype none
`include "acl_parameter_assert.svh"

//this module is a part of acl_hbm_interconnect, it is downstream of the pipelined arb, and it drives the axi interface for hbm
//incoming data to this module is captured in a fifo, this is needed to provide an almost full signal to backpressure pipelined arb
//avalon data read from the fifo is converted to axi, and then some pipelining is added between here and the HBM IP
//note that the fifo as described above is actually implemented as 3 separate fifos since there are 3 separate control flows in the axi interface - write addr, write data, and read addr
//there is no ordering between reads and writes, e.g. one should use writeack to ensure a write commits before sending a read request
//everything in this module is on the hbm clock

module acl_hbm_avalon_to_axi #(
    //signal widths
    parameter int unsigned ADDRESS_WIDTH,                   //address width for kernel and hbm
    parameter int unsigned BYTEENABLE_WIDTH,                //sets the word size for data path
    
    //pipeline latency
    parameter int unsigned ARB_ROUND_TRIP_LATENCY,          //this sets ALMOST_FULL_CUTOFF for the fifo, add up all the pipe stages (excluding response) in acl_hbm_pipelined_arb
    parameter int unsigned PIPE_STAGES_AXI_CMD_TO_HBM,      //number of pipeline stages for commands from axi command to hbm
    parameter int unsigned PIPE_STAGES_HBM_TO_AXI_CMD,      //number of pipeline stages for backpressure from hbm to axi command
    parameter int unsigned PIPE_STAGES_HBM_RESPONSE,        //number of pipeline stages for response path (read data, read data valid, write ack)
    parameter int unsigned FIFO_DEPTH,                      //depth of fifo
    parameter string       FIFO_RAM_BLOCK_TYPE,
    
    //derived parameters
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH,
    localparam int unsigned ADDRESS_BITS_PER_WORD = $clog2(BYTEENABLE_WIDTH),               //how many lower bits of the byte address are stuck at 0 to ensure it is word aligned
    localparam int unsigned WORD_ADDRESS_WIDTH = ADDRESS_WIDTH - ADDRESS_BITS_PER_WORD      //convert byte address width to word address width
) (
    //common
    input  wire                         clock,
    input  wire                         resetn,
    
    //avalon slave -- this has already gone through arbitration
    input  wire                         avs_write,
    input  wire                         avs_read,
    input  wire                         avs_is_pcie,
    input  wire  [ADDRESS_WIDTH-1:0]    avs_address,
    input  wire  [DATA_WIDTH-1:0]       avs_writedata,
    input  wire  [BYTEENABLE_WIDTH-1:0] avs_byteenable,
    output logic                        avs_waitrequest,
    output logic                        avs_readdatavalid_to_pcie,
    output logic                        avs_readdatavalid_to_kernel,
    output logic [DATA_WIDTH-1:0]       avs_readdata,
    output logic                        avs_writeack_to_pcie,       //not part or avalon group
    output logic                        avs_writeack_to_kernel,     //not part or avalon group
    
    //axi master
    //write command channel
    output logic [8:0]                  axm_awid,       //axi id - mostly driven by LSBs of word address, MSB indicates whether for kernel or pcie
    output logic [ADDRESS_WIDTH-1:0]    axm_awaddr,     //byte address for writes
    output logic [7:0]                  axm_awlen,      //burst length, tied to 0
    output logic [2:0]                  axm_awsize,     //burst size, fixed to 32 bytes, encoding = 3'h5
    output logic [1:0]                  axm_awburst,    //burst type, currently no hbm support, signaltap shows this should be 2'h1
    output logic [2:0]                  axm_awprot,     //protection support, tied to 0
    output logic                        axm_awuser,     //user pre-charge, we don't use this, tie to 0
    output logic [3:0]                  axm_awqos,      //quality of service, legal values are 4'hf or 4'h0, we tie to 0
    output logic                        axm_awvalid,    //valid
    input  wire                         axm_awready,    //ready
    
    //write data channel
    output logic [DATA_WIDTH-1:0]       axm_wdata,      //write data
    output logic [BYTEENABLE_WIDTH-1:0] axm_wstrb,      //byte enable
    output logic                        axm_wlast,      //indicates last word in a burst, tie to 1 since we only use burst length 1
    output logic                        axm_wvalid,     //write data valid
    input  wire                         axm_wready,     //ready
    
    //write response channel
    input  wire  [8:0]                  axm_bid,        //axi id for write response
    input  wire  [1:0]                  axm_bresp,      //indicates status of write transaction - 2'h0 means okay -- IGNORED
    input  wire                         axm_bvalid,     //valid
    output logic                        axm_bready,     //ready - tie to 1 since we never backpressure write ack
    
    //read command channel -- same signal set as write command channel, see comments there
    output logic [8:0]                  axm_arid,
    output logic [ADDRESS_WIDTH-1:0]    axm_araddr,
    output logic [7:0]                  axm_arlen,
    output logic [2:0]                  axm_arsize,
    output logic [1:0]                  axm_arburst,
    output logic [2:0]                  axm_arprot,
    output logic                        axm_aruser,
    output logic [3:0]                  axm_arqos,
    output logic                        axm_arvalid,
    input  wire                         axm_arready,
    
    //read response channel
    input  wire  [8:0]                  axm_rid,        //axi id for read response
    input  wire  [DATA_WIDTH-1:0]       axm_rdata,      //read data
    input  wire  [1:0]                  axm_rresp,      //indicates status of read transaction - 2'h0 means okay -- IGNORED
    input  wire                         axm_rlast,      //indicates last word in a burst - IGNORED, we only use burst length 1
    input  wire                         axm_rvalid,     //valid
    output logic                        axm_rready      //ready - tie to 1 since we never backpressure read ack
);
    
    
    //////////////////////////////////////
    //                                  //
    //  Sanity check on the parameters  //
    //                                  //
    //////////////////////////////////////
    
    generate
    `ACL_PARAMETER_ASSERT(ADDRESS_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH == 2**ADDRESS_BITS_PER_WORD)
    endgenerate
    
    
    
    /////////////////////////////////////////
    //                                     //
    //  Tied off signals on hbm interface  //
    //                                     //
    /////////////////////////////////////////
    
    //write command channel
    assign axm_awlen = 8'h0;
    assign axm_awsize = 3'h5;
    assign axm_awburst = 2'h1;
    assign axm_awprot = 3'h0;
    assign axm_awuser = 1'h0;
    assign axm_awqos = 4'h0;
    
    //write data channel
    assign axm_wlast = 1'h1;
    
    //write response channel
    assign axm_bready = 1'h1;
    
    //read command channel
    assign axm_arlen = 8'h0;
    assign axm_arsize = 3'h5;
    assign axm_arburst = 2'h1;
    assign axm_arprot = 3'h0;
    assign axm_aruser = 1'h0;
    assign axm_arqos = 4'h0;
    
    //read response channel
    assign axm_rready = 1'h1;
    
    
    
    ///////////////////////////
    //                       //
    //  Signal declarations  //
    //                       //
    ///////////////////////////
    
    //convert avalon to axi -- only the non-tied-off signals
    logic                          hbm_awready;
    logic                          hbm_wready;
    logic                          hbm_arready;
    logic                          hbm_arvalid;
    logic                          hbm_awvalid;
    logic                          hbm_wvalid;
    logic      [ADDRESS_WIDTH-1:0] hbm_araddr;
    logic      [ADDRESS_WIDTH-1:0] hbm_awaddr;
    logic         [DATA_WIDTH-1:0] hbm_wdata;
    logic   [BYTEENABLE_WIDTH-1:0] hbm_wstrb;
    logic                    [8:0] hbm_awid;
    logic                    [8:0] hbm_arid;
    
    //pipelined version of response axi signals
    logic                          hbm_rvalid;      //readdatavalid
    logic                    [8:0] hbm_rid;         //read response axi id
    logic         [DATA_WIDTH-1:0] hbm_rdata;       //readdata
    logic                          hbm_bvalid;      //writeack
    logic                    [8:0] hbm_bid;         //write response axi id
    
    //word addresses
    logic [WORD_ADDRESS_WIDTH-1:0] avs_word_address;
    logic [WORD_ADDRESS_WIDTH-1:0] hbm_araddr_word;
    logic [WORD_ADDRESS_WIDTH-1:0] hbm_awaddr_word;
    
    //signals for each of the fifos
    logic wr_data_fifo_almost_full, wr_data_fifo_valid;
    logic wr_addr_fifo_almost_full, wr_addr_fifo_valid;
    logic rd_addr_fifo_almost_full, rd_addr_fifo_valid;
    logic wr_addr_fifo_is_pcie;
    logic rd_addr_fifo_is_pcie;
    
    
    
    /////////////////////////////////////////////////////
    //                                                 //
    //  FIFOs immediately following the pipelined arb  //
    //                                                 //
    /////////////////////////////////////////////////////
    
    // fifo is needed to expose almost_full to pipelined arbitration upstream
    // the axi interface exposes a ready latency of 2, but it is not enough for the pipelined arb
    
    // pipelined arb does not examine whether transactions are read or write, so backpressure if any of the fifos are almost full
    assign avs_waitrequest = wr_data_fifo_almost_full | wr_addr_fifo_almost_full | rd_addr_fifo_almost_full;
    
    // only store the word address in the fifos
    // normally 
    assign avs_word_address = avs_address >> ADDRESS_BITS_PER_WORD;
    
    //FUTURE OPTIMIZATION: share the reset logic between the fifos
    
    hld_fifo #(
        .WIDTH              (DATA_WIDTH + BYTEENABLE_WIDTH),
        .DEPTH              (FIFO_DEPTH),
        .NEVER_OVERFLOWS    (1),
        .ALMOST_FULL_CUTOFF (ARB_ROUND_TRIP_LATENCY),
        .ASYNC_RESET        (0),
        .SYNCHRONIZE_RESET  (1),
        .STYLE              ("ms"),
        .RAM_BLOCK_TYPE     (FIFO_RAM_BLOCK_TYPE)
    )
    wr_data_fifo
    (
        .clock              (clock),
        .resetn             (resetn),
        .i_valid            (avs_write),
        .i_data             ({avs_writedata, avs_byteenable}),
        .o_stall            (),
        .o_almost_full      (wr_data_fifo_almost_full),
        .o_valid            (wr_data_fifo_valid),
        .o_data             ({hbm_wdata, hbm_wstrb}),
        .i_stall            (~hbm_wready),
        .o_almost_empty     (),
        .o_empty            (),
        .ecc_err_status     ()
    );
    
    
    hld_fifo #(
        .WIDTH              (1 + WORD_ADDRESS_WIDTH),
        .DEPTH              (FIFO_DEPTH),
        .NEVER_OVERFLOWS    (1),
        .ALMOST_FULL_CUTOFF (ARB_ROUND_TRIP_LATENCY),
        .ASYNC_RESET        (0),
        .SYNCHRONIZE_RESET  (1),
        .STYLE              ("ms"),
        .RAM_BLOCK_TYPE     (FIFO_RAM_BLOCK_TYPE)
    )
    wr_addr_fifo
    (
        .clock              (clock),
        .resetn             (resetn),
        .i_valid            (avs_write),
        .i_data             ({avs_is_pcie, avs_word_address}),
        .o_stall            (),
        .o_almost_full      (wr_addr_fifo_almost_full),
        .o_valid            (wr_addr_fifo_valid),
        .o_data             ({wr_addr_fifo_is_pcie, hbm_awaddr_word}),
        .i_stall            (~hbm_awready),
        .o_almost_empty     (),
        .o_empty            (),
        .ecc_err_status     ()
    );
    
    
    hld_fifo #(
        .WIDTH              (1 + WORD_ADDRESS_WIDTH),
        .DEPTH              (FIFO_DEPTH),
        .NEVER_OVERFLOWS    (1),
        .ALMOST_FULL_CUTOFF (ARB_ROUND_TRIP_LATENCY),
        .ASYNC_RESET        (0),
        .SYNCHRONIZE_RESET  (1),
        .STYLE              ("ms"),
        .RAM_BLOCK_TYPE     (FIFO_RAM_BLOCK_TYPE)
    )
    rd_addr_fifo
    (
        .clock              (clock),
        .resetn             (resetn),
        .i_valid            (avs_read),
        .i_data             ({avs_is_pcie, avs_word_address}),
        .o_stall            (),
        .o_almost_full      (rd_addr_fifo_almost_full),
        .o_valid            (rd_addr_fifo_valid),
        .o_data             ({rd_addr_fifo_is_pcie, hbm_araddr_word}),
        .i_stall            (~hbm_arready),
        .o_almost_empty     (),
        .o_empty            (),
        .ecc_err_status     ()
    );
    
    
    
    /////////////////////////////
    //                         //
    //  Convert avalon to axi  //
    //                         //
    /////////////////////////////
    
    //write data axi channel
    assign hbm_wvalid  = wr_data_fifo_valid & hbm_wready;           //forced transaction
    //hbm_wdata and hbm_wstrb are driven directly from the fifo output
    
    //write address axi channel
    assign hbm_awvalid = wr_addr_fifo_valid & hbm_awready;          //forced transaction
    assign hbm_awaddr = hbm_awaddr_word << ADDRESS_BITS_PER_WORD;   //convert word address to byte address
    assign hbm_awid = {wr_addr_fifo_is_pcie, hbm_awaddr_word[7:0]}; //axi id -- tie to least significant bits of word address, except for MSB which is used to determine if kernel or pcie
    
    //read address axi channel
    assign hbm_arvalid = rd_addr_fifo_valid & hbm_arready;          //forced transaction
    assign hbm_araddr = hbm_araddr_word << ADDRESS_BITS_PER_WORD;   //convert word address to byte address
    assign hbm_arid = {rd_addr_fifo_is_pcie, hbm_araddr_word[7:0]}; //axi id -- tie to least significant bits of word address, except for MSB which is used to determine if kernel or pcie
    
    
    
    ////////////////////////////////////////////////////////
    //                                                    //
    //  Pipelining of hbm signals between core/periphery  //
    //                                                    //
    ////////////////////////////////////////////////////////
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (3),
        .STAGES (PIPE_STAGES_HBM_TO_AXI_CMD)
    )
    pipeline_backpressure_to_hbm
    (
        .clock  (clock),
        .D      ({axm_awready, axm_wready, axm_arready}),
        .Q      ({hbm_awready, hbm_wready, hbm_arready})
    );
    
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (3 + 2*9 + 2*ADDRESS_WIDTH + DATA_WIDTH + BYTEENABLE_WIDTH),    //3 valids, 2 axi ids, 2 addr, writedata, byteenable
        .STAGES (PIPE_STAGES_AXI_CMD_TO_HBM)
    )
    pipeline_hbm_command
    (
        .clock  (clock),
        .D      ({hbm_arvalid, hbm_awvalid, hbm_wvalid, hbm_awid, hbm_arid, hbm_araddr, hbm_awaddr, hbm_wdata, hbm_wstrb}),
        .Q      ({axm_arvalid, axm_awvalid, axm_wvalid, axm_awid, axm_arid, axm_araddr, axm_awaddr, axm_wdata, axm_wstrb})
    );
    
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (2 + 2*9 + DATA_WIDTH),     //2 valids, 2 axi ids, readdata
        .STAGES (PIPE_STAGES_HBM_RESPONSE)
    )
    pipeline_hbm_response
    (
        .clock  (clock),
        .D      ({axm_rvalid, axm_bvalid, axm_rid, axm_bid, axm_rdata}),
        .Q      ({hbm_rvalid, hbm_bvalid, hbm_rid, hbm_bid, hbm_rdata})
    );
    
    
    
    /////////////////////////////////////////////////////
    //                                                 //
    //  Decode whether response is for kernel or pcie  //
    //                                                 //
    /////////////////////////////////////////////////////
    
    //use the MSB of axi id to determine if kernel (0) or pcie (1)
    
    //readdatavalid
    assign avs_readdatavalid_to_pcie   = hbm_rvalid &  hbm_rid[8];
    assign avs_readdatavalid_to_kernel = hbm_rvalid & ~hbm_rid[8];
    
    //readdata is shared between pcie and kernel
    assign avs_readdata = hbm_rdata;
    
    //writeack
    assign avs_writeack_to_pcie   = hbm_bvalid &  hbm_bid[8];
    assign avs_writeack_to_kernel = hbm_bvalid & ~hbm_bid[8];
    
    
endmodule

`default_nettype wire
