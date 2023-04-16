//__DESIGN_FILE_COPYRIGHT__
`default_nettype none
`include "acl_parameter_assert.svh"

//this module is a part of acl_hbm_interconnect, it has 2 upstream interfaces (1 pcie, 1 kernel) which are already on the hbm clock domain
//downstream of this module is avalon to axi conversion, there is a fifo inside that module to provide pipelined arb with an almost full
//there is pipelining EVERYWHERE in this arbiter, it is assumed all interfaces are using stall latency (force transaction) handshaking
//the arbitration itself is very simple because:
// - arbitration happens per word, it is assumed bursts have already been split by the time they get here, so arb doesn't need to switch on burst boundaries for example
// - arbitration steers the almost full from downstream to only one of the upstreams, valid can arrive from that upstream N clocks later, valid is guaranteed to not collide with the other upstream
// - arbitration statically favors pcie, we snoop on the pcie cmd dcfifo empty signal to determine how to steer the almost full
//everything in this module is on the hbm clock

module acl_hbm_pipelined_arb #(
    //signal widths
    parameter int unsigned ADDRESS_WIDTH,               //address width for kernel and hbm
    parameter int unsigned BYTEENABLE_WIDTH,            //sets the word size for data path
    
    //pipeline latency
    parameter int unsigned PIPE_STAGES_ARB_TO_CCB,      //number of pipeline stages from the arb to each command dcfifo read ack inside the ccb
    parameter int unsigned PIPE_STAGES_CCB_TO_ARB,      //number of pipeline stages from read data of each command dcfifo inside the ccb to arb
    parameter int unsigned PIPE_STAGES_ARB_TO_FIFO,     //number of pipeline stages after the arb but before the fifo that is needed to produce an almost full for arb
    parameter int unsigned PIPE_STAGES_FIFO_TO_ARB,     //number of pipeline stages for almost full from fifo back to the arb
    parameter int unsigned PIPE_STAGES_ARB_RESPONSE,    //number of pipeline stages for response -- this is in addition to the pipe stages in the axi converter, those are logic locked whereas pipe stages in here are not
    
    //derived parameters
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH
) (
    //common
    input  wire                         clock,
    input  wire                         resetn,
    
    //avalon slave -- from pcie
    input  wire                         pcie_write,
    input  wire                         pcie_read,
    input  wire  [ADDRESS_WIDTH-1:0]    pcie_address,           //byte address
    input  wire  [DATA_WIDTH-1:0]       pcie_writedata,
    input  wire  [BYTEENABLE_WIDTH-1:0] pcie_byteenable,
    output logic                        pcie_waitrequest,
    output logic                        pcie_readdatavalid,
    output logic [DATA_WIDTH-1:0]       pcie_readdata,
    output logic                        pcie_writeack,          //not part of avalon group
    input  wire                         pcie_empty,             //not part of avalon group -- used to control arbitration
    
    //avalon slave -- from kernel
    input  wire                         kernel_write,
    input  wire                         kernel_read,
    input  wire  [ADDRESS_WIDTH-1:0]    kernel_address,         //byte address
    input  wire  [DATA_WIDTH-1:0]       kernel_writedata,
    input  wire  [BYTEENABLE_WIDTH-1:0] kernel_byteenable,
    output logic                        kernel_waitrequest,
    output logic                        kernel_readdatavalid,
    output logic [DATA_WIDTH-1:0]       kernel_readdata,
    output logic                        kernel_writeack,        //not part of avalon group
    
    //avalon master -- to hbm
    output logic                        hbm_write,
    output logic                        hbm_read,
    output logic                        hbm_is_pcie,            //indicate whether a transaction is for pcie (1) or kernel (0)
    output logic [ADDRESS_WIDTH-1:0]    hbm_address,
    output logic [DATA_WIDTH-1:0]       hbm_writedata,
    output logic [BYTEENABLE_WIDTH-1:0] hbm_byteenable,
    input  wire                         hbm_waitrequest,
    input  wire                         hbm_readdatavalid_to_pcie,
    input  wire                         hbm_readdatavalid_to_kernel,
    input  wire  [DATA_WIDTH-1:0]       hbm_readdata,           //shared between pcie and kernel, only one (or none) of the readdatavalid signals can be high at any point in time
    input  wire                         hbm_writeack_to_pcie,   //not part of avalon group
    input  wire                         hbm_writeack_to_kernel  //not part of avalon group
);

    //////////////////////////////////////
    //                                  //
    //  Sanity check on the parameters  //
    //                                  //
    //////////////////////////////////////
    
    generate
    `ACL_PARAMETER_ASSERT(ADDRESS_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH >= 1)
    endgenerate
    
    
    
    ///////////////////////////
    //                       //
    //  Signal declarations  //
    //                       //
    ///////////////////////////
    
    //pipelined version of the command portion of pcie_*** input signals
    logic                        p_write;
    logic                        p_read;
    logic [ADDRESS_WIDTH-1:0]    p_address;
    logic [DATA_WIDTH-1:0]       p_writedata;
    logic [BYTEENABLE_WIDTH-1:0] p_byteenable;
    logic                        p_waitrequest;
    logic                        p_empty;
    
    //pipelined version of the command portion of kernel_*** input signals
    logic                        k_write;
    logic                        k_read;
    logic [ADDRESS_WIDTH-1:0]    k_address;
    logic [DATA_WIDTH-1:0]       k_writedata;
    logic [BYTEENABLE_WIDTH-1:0] k_byteenable;
    logic                        k_waitrequest;
    
    //arbitration control between pcie and kernel
    logic                        select_pcie;
    
    //arbitration data path -- there is latency from the time the arbitration decision is made until the data arrives from that upstream source
    logic                        arb_write;
    logic                        arb_read;
    logic [ADDRESS_WIDTH-1:0]    arb_address;
    logic [DATA_WIDTH-1:0]       arb_writedata;
    logic [BYTEENABLE_WIDTH-1:0] arb_byteenable;
    logic                        arb_waitrequest;
    logic                        arb_is_pcie;
    
    
    
    ///////////////////////////////////////////////////////////////
    //                                                           //
    //  Pipelining to/from arbitration for both pcie and kernel  //
    //                                                           //
    ///////////////////////////////////////////////////////////////
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (2),
        .STAGES (PIPE_STAGES_ARB_TO_CCB)
    )
    pipeline_backpressure_to_pcie_kernel
    (
        .clock  (clock),
        .D      ({   p_waitrequest,      k_waitrequest}),
        .Q      ({pcie_waitrequest, kernel_waitrequest})
    );
    
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (3 + ADDRESS_WIDTH + DATA_WIDTH + BYTEENABLE_WIDTH),
        .STAGES (PIPE_STAGES_CCB_TO_ARB)
    )
    pipeline_pcie_command
    (
        .clock  (clock),
        .D      ({pcie_empty, pcie_write, pcie_read, pcie_address, pcie_writedata, pcie_byteenable}),
        .Q      ({   p_empty,    p_write,    p_read,    p_address,    p_writedata,    p_byteenable})
    );
    
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (2 + ADDRESS_WIDTH + DATA_WIDTH + BYTEENABLE_WIDTH),
        .STAGES (PIPE_STAGES_CCB_TO_ARB)
    )
    pipeline_kernel_command
    (
        .clock  (clock),
        .D      ({kernel_write, kernel_read, kernel_address, kernel_writedata, kernel_byteenable}),
        .Q      ({     k_write,      k_read,      k_address,      k_writedata,      k_byteenable})
    );
    
    
    
    /////////////////////////////////////////
    //                                     //
    //  Arbitrate between pcie and kernel  //
    //                                     //
    /////////////////////////////////////////
    
    //IMPORTANT: it is assumed that by the point, every transaction is contained within one word
    //this means write bursts must have already been split, but read requests can still be bursts
    
    //arbitration simply steers the almost full from the fifo below to one of the upstreams, pcie or kernel, and the other upstream is backpressured
    //statically favor pcie over kernel, but beware arbitration operates on a stale version of empty from pcie (if pcie not empty, then select pcie)
    //latencies are the same for all upstream interfaces, so when an upstream interface sends data, by construction it is guaranteed to not collide with any other upstream interface
    
    
    //choose whether to direct ready to pcie or kernel, the other one will be backpressured
    //FUTURE OPTIMIZATION: we can look at empty / almost_empty from both pcie and kernel to be smarter about who to select, can also use timeouts
    assign select_pcie  = ~p_empty;
    assign p_waitrequest =  (select_pcie) ? arb_waitrequest : 1'b1;
    assign k_waitrequest = (~select_pcie) ? arb_waitrequest : 1'b1;
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (1),
        .STAGES (PIPE_STAGES_ARB_TO_CCB + PIPE_STAGES_CCB_TO_ARB)
    )
    pipeline_arb_is_pcie
    (
        .clock  (clock),
        .D      (select_pcie),
        .Q      (arb_is_pcie)
    );
    
    //MASTER_STALL_LATENCY has been enabled for both upstream CCBs, so the read_*** and write_*** signals are actually forced transactions
    assign arb_write      = p_write | k_write;  //at any time, only one upstream source is not backpressured, and the roundtrip latencies are matched, so by construction these will not collide
    assign arb_read       = p_read  | k_read;
    assign arb_address    = (arb_is_pcie) ? p_address    : k_address;
    assign arb_writedata  = (arb_is_pcie) ? p_writedata  : k_writedata;
    assign arb_byteenable = (arb_is_pcie) ? p_byteenable : k_byteenable;
    
    
    
    ///////////////////////////////////////////////////////////////
    //                                                           //
    //  Pipelining between arbitration and hbm avalon interface  //
    //                                                           //
    ///////////////////////////////////////////////////////////////
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (3 + ADDRESS_WIDTH + DATA_WIDTH + BYTEENABLE_WIDTH),
        .STAGES (PIPE_STAGES_ARB_TO_FIFO)
    )
    pipeline_arb_command
    (
        .clock  (clock),
        .D      ({arb_is_pcie, arb_write, arb_read, arb_address, arb_writedata, arb_byteenable}),
        .Q      ({hbm_is_pcie, hbm_write, hbm_read, hbm_address, hbm_writedata, hbm_byteenable})
    );
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (1),
        .STAGES (PIPE_STAGES_FIFO_TO_ARB)
    )
    pipeline_backpressure_to_arb
    (
        .clock  (clock),
        .D      (hbm_waitrequest),
        .Q      (arb_waitrequest)
    );
    
    
    
    /////////////////////////
    //                     //
    //  Response pipeline  //
    //                     //
    /////////////////////////
    
    acl_shift_register_no_reset_dont_merge
    #(
        .WIDTH  (4 + DATA_WIDTH),
        .STAGES (PIPE_STAGES_ARB_RESPONSE)
    )
    pipeline_arb_response
    (
        .clock  (clock),
        .D      ({hbm_writeack_to_kernel, hbm_writeack_to_pcie, hbm_readdatavalid_to_kernel, hbm_readdatavalid_to_pcie,  hbm_readdata}),
        .Q      ({       kernel_writeack,        pcie_writeack,        kernel_readdatavalid,        pcie_readdatavalid, pcie_readdata})
    );
    
    assign kernel_readdata = pcie_readdata; //shared, only one (or none) of the readdatavalid signals can be high at any point in time
    
    
endmodule

`default_nettype wire
