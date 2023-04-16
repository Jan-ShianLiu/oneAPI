//__DESIGN_FILE_COPYRIGHT__
`default_nettype none
`include "acl_parameter_assert.svh"

//  ACL HBM INTERCONNECT
//
//  This is the top level module for the custom RTL interconnect for the HBM BSP. For each HBM interface, it performs arbitration between one common PCIe interface (from board.qsys)
//  and each kernel interface. There are clock crossing FIFOs so that HBM, PCIe, kernel attach to this interconnect on their own native clock domains. Each kernel interface can only
//  access one HBM interface, the kernel interface from acl_hbm_interconnnect connects to the root of the Opencl interconnect (multiple LSUs from multiple kernels have already been
//  arbitrated into one access point). PCIe can access any HBM interface, so there is an address decode to determine which HBM a command should go to. For PCIe, it is assumed that
//  there will only be outstanding transactions to one HBM at any time. This avoids the need to handle read data returning from multiple HBMs at the same time. This is enforced by
//  the Opencl run time and by the random traffic generator in the testbench.
//
//  This interconnect has no support for width-stitching, e.g. making multiple physical HBM interfaces look like one logic interface with a wider data path. That is future work and
//  this implementation is extensible to supporting this.
//
//  High level structure:
//
//                        +----------------+   +-------------------+   +-----------------+   +--------------------------------------------------------+
//                PCIE ---| burst splitter |---| bottom/top decode |---| pipeline stages |---| top half (same structure as below, not drawing it out) |
//                        +----------------+   +-------------------+   +-----------------+   +--------------------------------------------------------+
//                                                      |
//                                              +-----------------+
//                                              | pipeline stages |
//                                              +-----------------+
//                                                      |
//                                             +--------------------+   +----------+                                                                                    +----------+
//                                             | pcie bottom decode |---| shiftreg |--------------------------------------- ...  ---------------------------------------| shiftreg |
//                                             +--------------------+   +----------+                                                                                    +----------+
//                                                                           |                                                                                               |
//                                                                    +-------------+                                                                                 +-------------+
//                                                                    | clock cross |                                                                                 | clock cross |
//       KERNEL 0                                                     +-------------+                    KERNEL N                                                     +-------------+
//          |                                                                |                              |                                                                |
//  +-----------------+   +-------------+   +----------------+   +-----------------------+          +-----------------+   +-------------+   +----------------+   +-----------------------+
//  | pipeline stages |---| clock cross |---| burst splitter |---| pipelined arbitration |          | pipeline stages |---| clock cross |---| burst splitter |---| pipelined arbitration |
//  +-----------------+   +-------------+   +----------------+   +-----------------------+          +-----------------+   +-------------+   +----------------+   +-----------------------+
//                                                                           |                                                                                               |
//                                                                   +---------------+                                                                               +---------------+
//                                                                   | avalon to axi |                                                                               | avalon to axi |
//                                                                   +---------------+                                                                               +---------------+
//                                                                           |                                                                                               |
//                                                                         HBM 0                                                                                           HBM N
//
//  Naming conventions:
//  - all signals are prefixed with k_, p_, or h_, which indicates that it is on the kernel, pcie, or hbm clock domain
//  - after the prefix, b_ or t_ indicates bottom or top
//  - after the prefix, the name indicates which interface it is, e.g. h_pcie_ is the pcie interface on the hbm clock domain
//
//  Other conventions:
//  - all address signals are byte address, not word address, unless explicitly indicated otherwise
//  - interfaces missing the burstcount signal means that burstcount is implicitly 1, e.g. the bursts have already been split
//
//  Required files:
//  - acl_hbm_interconnect.sv
//  - acl_hbm_pcie_addr_decode.sv
//  - acl_hbm_pcie_addr_decode_half.sv
//  - acl_hbm_pcie_addr_decode_shiftreg.sv
//  - acl_hbm_pcie_addr_decode_terminal.sv
//  - acl_hbm_pipelined_arb.sv
//  - acl_hbm_avalon_to_axi.sv
//  - acl_shift_register_no_reset_dont_merge.sv
//  - acl_clock_crossing_bridge.sv
//  - acl_dcfifo.sv
//  - acl_burst_splitter.sv
//  - acl_reset_handler.sv
//  - hld_fifo.sv
//  - acl_mid_speed_fifo.sv
//  - acl_lfsr.sv
//  - acl_tessellated_incr_decr_threshold.sv
//  - acl_parameter_assert.svh
//  - acl_width_clip.svh


module acl_hbm_interconnect #(
    parameter int unsigned NUM_HBM = 8,              //number of HBMs used = number of global memories = number of kernel interfaces
    parameter int unsigned NUM_HBM_BOTTOM = 4,       //the first NUM_HBM_BOTTOM HBM interfaces will get mapped to the bottom instance, can infer NUM_HBM_TOP as NUM_HBM - NUM_HBM_BOTTOM
    parameter int unsigned BURSTCOUNT_WIDTH = 5,     //common across all interfaces
    parameter int unsigned BYTEENABLE_WIDTH = 32,    //common access all interfaces, must be a power of 2, determines the word size
    parameter int unsigned ADDRESS_WIDTH = 28,       //address width for kernel and hbm, pcie is wider
    parameter int unsigned KERNEL_STALL_LATENCY = 4, //latency from k_kernel_waitrequest to k_kernel_write/k_kernel_read, not necessarily the same as the amount of full side deadspace in cmd dcfifo of kernel ccb
    parameter bit          PCIE_USE_WRITE_ACK = 0,   //set to 1 if you want to consume p_pcie_writeack, currently only exercised by testbench
    
    //derived parameters
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH,
    localparam int unsigned PCIE_ADDRESS_WIDTH = ADDRESS_WIDTH + $clog2(NUM_HBM),           //address width for pcie is wider since the upper $clog2(NUM_HBM) bits indicate which HBM to target
    localparam int unsigned ADDRESS_BITS_PER_WORD = $clog2(BYTEENABLE_WIDTH),               //how many lower bits of the byte address are stuck at 0 to ensure it is word aligned
    localparam int unsigned BURST_BOUNDARY = ADDRESS_BITS_PER_WORD + BURSTCOUNT_WIDTH - 1   //burst splitter doesn't need address adder to be full width, kernel LSUs ensure bursts don't cross boundary
) (
    //clocks and resets
    input  wire                             p_clock,
    input  wire                             p_resetn,
    input  wire                             k_clock,
    input  wire                             k_resetn,
    input  wire                             h_b_clock,      //hbm has a common clock and reset across all channels for one instance
    input  wire                             h_b_resetn,     //bottom and top instances have separate clocks and reset
    input  wire                             h_t_clock,      //use bottom clock/reset for interfaces 0 through NUM_HBM_BOTTOM-1 inclusive
    input  wire                             h_t_resetn,     //use top clock/reset for interfaces NUM_HBM_BOTTOM through NUM_HBM-1 inclusive
    
    //pcie avalon slave
    input  wire                             p_pcie_write,
    input  wire                             p_pcie_read,
    input  wire  [PCIE_ADDRESS_WIDTH-1:0]   p_pcie_address,
    input  wire  [BURSTCOUNT_WIDTH-1:0]     p_pcie_burstcount,
    input  wire  [DATA_WIDTH-1:0]           p_pcie_writedata,
    input  wire  [BYTEENABLE_WIDTH-1:0]     p_pcie_byteenable,
    output logic                            p_pcie_waitrequest,
    output logic                            p_pcie_readdatavalid,
    output logic [DATA_WIDTH-1:0]           p_pcie_readdata,
    output logic                            p_pcie_writeack,
    
    //kernel avalon slaves
    input  wire                             k_kernel_write          [NUM_HBM-1:0],
    input  wire                             k_kernel_read           [NUM_HBM-1:0],
    input  wire  [ADDRESS_WIDTH-1:0]        k_kernel_address        [NUM_HBM-1:0],
    input  wire  [BURSTCOUNT_WIDTH-1:0]     k_kernel_burstcount     [NUM_HBM-1:0],
    input  wire  [DATA_WIDTH-1:0]           k_kernel_writedata      [NUM_HBM-1:0],
    input  wire  [BYTEENABLE_WIDTH-1:0]     k_kernel_byteenable     [NUM_HBM-1:0],
    output logic                            k_kernel_waitrequest    [NUM_HBM-1:0],
    output logic                            k_kernel_readdatavalid  [NUM_HBM-1:0],
    output logic [DATA_WIDTH-1:0]           k_kernel_readdata       [NUM_HBM-1:0],
    output logic                            k_kernel_writeack       [NUM_HBM-1:0],
    
    //SIM ONLY interface indicates which order the reads and writes got written into their respective fifos (just after pipelined arb)
    //synthesis translate_off
    output logic                            SIM_ONLY_valid          [NUM_HBM-1:0],
    output logic                            SIM_ONLY_is_write       [NUM_HBM-1:0],
    //synthesis translate_on
    
    //hbm AXI masters
    output logic [8:0]                      h_hbm_awid              [NUM_HBM-1:0],
    output logic [ADDRESS_WIDTH-1:0]        h_hbm_awaddr            [NUM_HBM-1:0],
    output logic [7:0]                      h_hbm_awlen             [NUM_HBM-1:0],
    output logic [2:0]                      h_hbm_awsize            [NUM_HBM-1:0],
    output logic [1:0]                      h_hbm_awburst           [NUM_HBM-1:0],
    output logic [2:0]                      h_hbm_awprot            [NUM_HBM-1:0],
    output logic                            h_hbm_awuser            [NUM_HBM-1:0],
    output logic [3:0]                      h_hbm_awqos             [NUM_HBM-1:0],
    output logic                            h_hbm_awvalid           [NUM_HBM-1:0],
    input  wire                             h_hbm_awready           [NUM_HBM-1:0],
    output logic [DATA_WIDTH-1:0]           h_hbm_wdata             [NUM_HBM-1:0],
    output logic [BYTEENABLE_WIDTH-1:0]     h_hbm_wstrb             [NUM_HBM-1:0],
    output logic                            h_hbm_wlast             [NUM_HBM-1:0],
    output logic                            h_hbm_wvalid            [NUM_HBM-1:0],
    input  wire                             h_hbm_wready            [NUM_HBM-1:0],
    input  wire  [8:0]                      h_hbm_bid               [NUM_HBM-1:0],
    input  wire  [1:0]                      h_hbm_bresp             [NUM_HBM-1:0],
    input  wire                             h_hbm_bvalid            [NUM_HBM-1:0],
    output logic                            h_hbm_bready            [NUM_HBM-1:0],
    output logic [8:0]                      h_hbm_arid              [NUM_HBM-1:0],
    output logic [ADDRESS_WIDTH-1:0]        h_hbm_araddr            [NUM_HBM-1:0],
    output logic [7:0]                      h_hbm_arlen             [NUM_HBM-1:0],
    output logic [2:0]                      h_hbm_arsize            [NUM_HBM-1:0],
    output logic [1:0]                      h_hbm_arburst           [NUM_HBM-1:0],
    output logic [2:0]                      h_hbm_arprot            [NUM_HBM-1:0],
    output logic                            h_hbm_aruser            [NUM_HBM-1:0],
    output logic [3:0]                      h_hbm_arqos             [NUM_HBM-1:0],
    output logic                            h_hbm_arvalid           [NUM_HBM-1:0],
    input  wire                             h_hbm_arready           [NUM_HBM-1:0],
    input  wire  [8:0]                      h_hbm_rid               [NUM_HBM-1:0],
    input  wire  [DATA_WIDTH-1:0]           h_hbm_rdata             [NUM_HBM-1:0],
    input  wire  [1:0]                      h_hbm_rresp             [NUM_HBM-1:0],
    input  wire                             h_hbm_rlast             [NUM_HBM-1:0],
    input  wire                             h_hbm_rvalid            [NUM_HBM-1:0],
    output logic                            h_hbm_rready            [NUM_HBM-1:0]
);

    //////////////////////////////////////
    //                                  //
    //  Sanity check on the parameters  //
    //                                  //
    //////////////////////////////////////
    
    generate
    `ACL_PARAMETER_ASSERT(NUM_HBM >= 1)
    `ACL_PARAMETER_ASSERT(NUM_HBM_BOTTOM <= NUM_HBM)
    `ACL_PARAMETER_ASSERT(BURSTCOUNT_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH == 2**ADDRESS_BITS_PER_WORD)
    `ACL_PARAMETER_ASSERT(ADDRESS_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(ADDRESS_WIDTH >= ADDRESS_BITS_PER_WORD)
    endgenerate
    
    
    
    /////////////////////////////////
    //                             //
    //  Various tuning parameters  //
    //                             //
    /////////////////////////////////
    
    //acl_hbm_pcie_addr_decode
    localparam int unsigned PIPE_STAGES_PCIE_TO_BOTTOM_FIFO = 3;
    localparam int unsigned PIPE_STAGES_BOTTOM_FIFO_TO_PCIE = 3;
    localparam int unsigned PIPE_STAGES_PCIE_TO_TOP_FIFO = 8;
    localparam int unsigned PIPE_STAGES_TOP_FIFO_TO_PCIE = 8;
    
    //between kernel and ccb
    localparam int unsigned PIPE_STAGES_KERNEL_TO_CCB = 1;
    localparam int unsigned PIPE_STAGES_CCB_TO_KERNEL = 1;
    
    //acl_hbm_pipelined_arb
    localparam int unsigned PIPE_STAGES_ARB_TO_CCB = 2;
    localparam int unsigned PIPE_STAGES_CCB_TO_ARB = 1;
    localparam int unsigned PIPE_STAGES_ARB_TO_FIFO = 1;
    localparam int unsigned PIPE_STAGES_FIFO_TO_ARB = 1;
    localparam int unsigned PIPE_STAGES_ARB_RESPONSE = 1;
    
    //acl_hbm_avalon_to_axi
    localparam int unsigned PIPE_STAGES_AXI_CMD_TO_HBM = 0;
    localparam int unsigned PIPE_STAGES_HBM_TO_AXI_CMD = 0;
    localparam int unsigned PIPE_STAGES_HBM_RESPONSE = 1;
    
    //fifo settings
    //static region is more limited by ALMs than M20K, convert what would normally be MLAB into M20K
    //for ***_USE_MLAB, bit i indicates whether command dcfifo index i should use mlab (1) or m20k (0)
    //bits 7:0 are bottom left
    //bits 15:8 are bottom right
    //bits 23:16 are top left
    //bits 31:24 are top right
    localparam int unsigned PCIE_BOTTOM_FIFO_DEPTH = 512;
    localparam bit          PCIE_BOTTOM_FIFO_USE_MLAB = 0;
    localparam int unsigned PCIE_TOP_FIFO_DEPTH = 512;
    localparam bit          PCIE_TOP_FIFO_USE_MLAB = 0;
    localparam int unsigned PCIE_CCB_CMD_DCFIFO_DEPTH = 32;
    localparam int unsigned PCIE_CCB_RSP_DCFIFO_DEPTH = 512;
    localparam int unsigned PCIE_CCB_CMD_DCFIFO_USE_MLAB = 32'hff000000;    //top right is limited in m20k more so than other quadrants, leave the pcie cmd dcfifo in mlab
    localparam int unsigned PCIE_CCB_RSP_DCFIFO_USE_MLAB = 32'h00000000;
    localparam int unsigned KERNEL_CCB_CMD_DCFIFO_DEPTH = 32;
    localparam int unsigned KERNEL_CCB_RSP_DCFIFO_DEPTH = 512;
    localparam int unsigned KERNEL_CCB_CMD_DCFIFO_USE_MLAB = 32'h00000000;
    localparam int unsigned KERNEL_CCB_RSP_DCFIFO_USE_MLAB = 32'h00000000;
    localparam int unsigned AVALON_TO_AXI_FIFO_DEPTH = 32;
    localparam int unsigned AVALON_TO_AXI_FIFO_USE_MLAB = 32'hffffffff;
    
    
    
    //signal declarations
    genvar g;
    
    //pcie after it has been address decoded and crossed clock domains into hbm clock
    logic                           h_pcie_write                    [NUM_HBM-1:0];
    logic                           h_pcie_read                     [NUM_HBM-1:0];
    logic [ADDRESS_WIDTH-1:0]       h_pcie_address                  [NUM_HBM-1:0];
    logic [DATA_WIDTH-1:0]          h_pcie_writedata                [NUM_HBM-1:0];
    logic [BYTEENABLE_WIDTH-1:0]    h_pcie_byteenable               [NUM_HBM-1:0];
    logic                           h_pcie_waitrequest              [NUM_HBM-1:0];
    logic                           h_pcie_readdatavalid            [NUM_HBM-1:0];
    logic [DATA_WIDTH-1:0]          h_pcie_readdata                 [NUM_HBM-1:0];
    logic                           h_pcie_writeack                 [NUM_HBM-1:0];
    logic                           h_pcie_empty                    [NUM_HBM-1:0];
    
    //kernel after it has crossed clock domains into hbm clock
    logic                           h_kernel_write                  [NUM_HBM-1:0];
    logic                           h_kernel_read                   [NUM_HBM-1:0];
    logic [ADDRESS_WIDTH-1:0]       h_kernel_address                [NUM_HBM-1:0];
    logic [DATA_WIDTH-1:0]          h_kernel_writedata              [NUM_HBM-1:0];
    logic [BYTEENABLE_WIDTH-1:0]    h_kernel_byteenable             [NUM_HBM-1:0];
    logic                           h_kernel_waitrequest            [NUM_HBM-1:0];
    logic                           h_kernel_readdatavalid          [NUM_HBM-1:0];
    logic [DATA_WIDTH-1:0]          h_kernel_readdata               [NUM_HBM-1:0];
    logic                           h_kernel_writeack               [NUM_HBM-1:0];
    
    //hbm avalon -- after arbitration but before conversion to axi
    logic                           h_hbm_write                     [NUM_HBM-1:0];
    logic                           h_hbm_read                      [NUM_HBM-1:0];
    logic                           h_hbm_is_pcie                   [NUM_HBM-1:0];
    logic [ADDRESS_WIDTH-1:0]       h_hbm_address                   [NUM_HBM-1:0];
    logic [DATA_WIDTH-1:0]          h_hbm_writedata                 [NUM_HBM-1:0];
    logic [BYTEENABLE_WIDTH-1:0]    h_hbm_byteenable                [NUM_HBM-1:0];
    logic                           h_hbm_waitrequest               [NUM_HBM-1:0];
    logic                           h_hbm_readdatavalid_to_pcie     [NUM_HBM-1:0];
    logic                           h_hbm_readdatavalid_to_kernel   [NUM_HBM-1:0];
    logic [DATA_WIDTH-1:0]          h_hbm_readdata                  [NUM_HBM-1:0];
    logic                           h_hbm_writeack_to_pcie          [NUM_HBM-1:0];
    logic                           h_hbm_writeack_to_kernel        [NUM_HBM-1:0];
    
    
    
    //decode pcie for each hbm
    //all signals widths are common between pcie and hbm except for the address, pcie address is wider by $clog2(NUM_HBM) bits
    acl_hbm_pcie_addr_decode #(
        .NUM_HBM                            (NUM_HBM),
        .NUM_HBM_BOTTOM                     (NUM_HBM_BOTTOM),
        .ADDRESS_WIDTH                      (ADDRESS_WIDTH),
        .BURSTCOUNT_WIDTH                   (BURSTCOUNT_WIDTH),
        .BYTEENABLE_WIDTH                   (BYTEENABLE_WIDTH),
        .USE_WRITE_ACK                      (PCIE_USE_WRITE_ACK),
        .PIPE_STAGES_PCIE_TO_BOTTOM_FIFO    (PIPE_STAGES_PCIE_TO_BOTTOM_FIFO),
        .PIPE_STAGES_BOTTOM_FIFO_TO_PCIE    (PIPE_STAGES_BOTTOM_FIFO_TO_PCIE),
        .PIPE_STAGES_PCIE_TO_TOP_FIFO       (PIPE_STAGES_PCIE_TO_TOP_FIFO),
        .PIPE_STAGES_TOP_FIFO_TO_PCIE       (PIPE_STAGES_TOP_FIFO_TO_PCIE),
        .PCIE_BOTTOM_FIFO_DEPTH             (PCIE_BOTTOM_FIFO_DEPTH),
        .PCIE_BOTTOM_FIFO_USE_MLAB          (PCIE_BOTTOM_FIFO_USE_MLAB),
        .PCIE_TOP_FIFO_DEPTH                (PCIE_TOP_FIFO_DEPTH),
        .PCIE_TOP_FIFO_USE_MLAB             (PCIE_TOP_FIFO_USE_MLAB),
        .CMD_DCFIFO_DEPTH                   (PCIE_CCB_CMD_DCFIFO_DEPTH),
        .RSP_DCFIFO_DEPTH                   (PCIE_CCB_RSP_DCFIFO_DEPTH),
        .CMD_DCFIFO_USE_MLAB                (PCIE_CCB_CMD_DCFIFO_USE_MLAB),
        .RSP_DCFIFO_USE_MLAB                (PCIE_CCB_RSP_DCFIFO_USE_MLAB)
    )
    acl_hbm_pcie_addr_decode_inst
    (
        .p_clock                            (p_clock),
        .p_resetn                           (p_resetn),
        .h_b_clock                          (h_b_clock),
        .h_b_resetn                         (h_b_resetn),
        .h_t_clock                          (h_t_clock),
        .h_t_resetn                         (h_t_resetn),
        
        .p_address                          (p_pcie_address),
        .p_burstcount                       (p_pcie_burstcount),
        .p_byteenable                       (p_pcie_byteenable),
        .p_read                             (p_pcie_read),
        .p_readdata                         (p_pcie_readdata),
        .p_readdatavalid                    (p_pcie_readdatavalid),
        .p_waitrequest                      (p_pcie_waitrequest),
        .p_write                            (p_pcie_write),
        .p_writeack                         (p_pcie_writeack),
        .p_writedata                        (p_pcie_writedata),
        
        .h_address                          (h_pcie_address),
        .h_byteenable                       (h_pcie_byteenable),
        .h_read                             (h_pcie_read),
        .h_readdata                         (h_pcie_readdata),
        .h_readdatavalid                    (h_pcie_readdatavalid),
        .h_waitrequest                      (h_pcie_waitrequest),
        .h_write                            (h_pcie_write),
        .h_writeack                         (h_pcie_writeack),
        .h_writedata                        (h_pcie_writedata),
        .h_empty                            (h_pcie_empty)
    );
    
    
    
    
    //kernel interface - go from kernel clock to hbm clock, burst splitter has been moved before the pipelined arb
    generate
    for (g=0; g<NUM_HBM; g++) begin : GEN_KERNEL_TO_HBM_CCB
        //on kernel clock, after pipelining but before ccb
        logic                           k_kernel_ccb_read;
        logic                           k_kernel_ccb_write;
        logic [ADDRESS_WIDTH-1:0]       k_kernel_ccb_address;
        logic [BURSTCOUNT_WIDTH-1:0]    k_kernel_ccb_burstcount;
        logic [DATA_WIDTH-1:0]          k_kernel_ccb_writedata;
        logic [BYTEENABLE_WIDTH-1:0]    k_kernel_ccb_byteenable;
        logic                           k_kernel_ccb_waitrequest;
        logic                           k_kernel_ccb_readdatavalid;
        logic [DATA_WIDTH-1:0]          k_kernel_ccb_readdata;
        logic                           k_kernel_ccb_writeack;
        
        //determine which hbm clock and reset to use
        logic                           h_resetn;
        logic                           h_clock;
        
        //on hbm clock, after ccb but before burst splitter
        logic                           h_kernel_ccb_write;
        logic                           h_kernel_ccb_read;
        logic [ADDRESS_WIDTH-1:0]       h_kernel_ccb_address;
        logic [BURSTCOUNT_WIDTH-1:0]    h_kernel_ccb_burstcount;
        logic [DATA_WIDTH-1:0]          h_kernel_ccb_writedata;
        logic [BYTEENABLE_WIDTH-1:0]    h_kernel_ccb_byteenable;
        logic                           h_kernel_ccb_waitrequest;
        logic                           h_kernel_ccb_readdatavalid;
        logic [DATA_WIDTH-1:0]          h_kernel_ccb_readdata;
        logic                           h_kernel_ccb_writeack;
        
        //after burst splitter, convert stall valid to stall latency
        logic                           h_kernel_read_raw;
        logic                           h_kernel_write_raw;
        
        
        //pipelining between kernel and ccb
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (2 + ADDRESS_WIDTH + BURSTCOUNT_WIDTH + BYTEENABLE_WIDTH + DATA_WIDTH),
            .STAGES (PIPE_STAGES_KERNEL_TO_CCB)
        )
        pipeline_kernel_to_ccb
        (
            .clock  (k_clock),
            .D      ({k_kernel_read[g] , k_kernel_write[g] , k_kernel_address[g] , k_kernel_burstcount[g] , k_kernel_byteenable[g] , k_kernel_writedata[g] }),
            .Q      ({k_kernel_ccb_read, k_kernel_ccb_write, k_kernel_ccb_address, k_kernel_ccb_burstcount, k_kernel_ccb_byteenable, k_kernel_ccb_writedata})
        );
        
        acl_shift_register_no_reset_dont_merge
        #(
            .WIDTH  (3 + DATA_WIDTH),
            .STAGES (PIPE_STAGES_CCB_TO_KERNEL)
        )
        pipeline_ccb_to_kernel
        (
            .clock  (k_clock),
            .D      ({k_kernel_ccb_waitrequest, k_kernel_ccb_readdatavalid, k_kernel_ccb_writeack, k_kernel_ccb_readdata}),
            .Q      ({k_kernel_waitrequest[g] , k_kernel_readdatavalid[g] , k_kernel_writeack[g] , k_kernel_readdata[g] })
        );
        
        
        //the ccb itself
        assign h_resetn = (g < NUM_HBM_BOTTOM) ? h_b_resetn : h_t_resetn;
        assign h_clock  = (g < NUM_HBM_BOTTOM) ? h_b_clock  : h_t_clock;
        acl_clock_crossing_bridge #(
            .ADDRESS_WIDTH              (ADDRESS_WIDTH),
            .BYTEENABLE_WIDTH           (BYTEENABLE_WIDTH),
            .BURSTCOUNT_WIDTH           (BURSTCOUNT_WIDTH),
            .CMD_DCFIFO_MIN_DEPTH       (KERNEL_CCB_CMD_DCFIFO_DEPTH),
            .RSP_DCFIFO_MIN_DEPTH       (KERNEL_CCB_RSP_DCFIFO_DEPTH),
            .CMD_DCFIFO_RAM_TYPE        ((KERNEL_CCB_CMD_DCFIFO_USE_MLAB[g]) ? "MLAB" : "M20K"),
            .RSP_DCFIFO_RAM_TYPE        ((KERNEL_CCB_RSP_DCFIFO_USE_MLAB[g]) ? "MLAB" : "M20K"),
            .SLAVE_STALL_LATENCY        (KERNEL_STALL_LATENCY + PIPE_STAGES_KERNEL_TO_CCB + PIPE_STAGES_CCB_TO_KERNEL),
            .MASTER_STALL_LATENCY       (0),    //downstream connects to the burst splitter, not the pipelined arb
            .USE_WRITE_ACK              (1)
        )
        kernel_to_hbm_ccb
        (
            .async_resetn               (k_resetn & h_resetn),
            
            //slave interface on kernel clock
            .s_clock                    (k_clock),
            .s_read                     (k_kernel_ccb_read),
            .s_write                    (k_kernel_ccb_write),
            .s_address                  (k_kernel_ccb_address),
            .s_burstcount               (k_kernel_ccb_burstcount),
            .s_byteenable               (k_kernel_ccb_byteenable),
            .s_writedata                (k_kernel_ccb_writedata),
            .s_waitrequest              (k_kernel_ccb_waitrequest),
            .s_readdatavalid            (k_kernel_ccb_readdatavalid),
            .s_readdata                 (k_kernel_ccb_readdata),
            .s_writeack                 (k_kernel_ccb_writeack),
            
            //unused -- intended for use with wide ccb
            .s_readack_empty            (),
            .s_readack_almost_empty     (),
            .s_readack_stall            (),
            .s_writeack_empty           (),
            .s_writeack_almost_empty    (),
            .s_writeack_stall           (),
            
            //master interface on hbm clock
            .m_clock                    (h_clock),
            .m_read                     (h_kernel_ccb_read),
            .m_write                    (h_kernel_ccb_write),
            .m_address                  (h_kernel_ccb_address),
            .m_burstcount               (h_kernel_ccb_burstcount),
            .m_byteenable               (h_kernel_ccb_byteenable),
            .m_writedata                (h_kernel_ccb_writedata),
            .m_waitrequest              (h_kernel_ccb_waitrequest),
            .m_readdatavalid            (h_kernel_ccb_readdatavalid),
            .m_readdata                 (h_kernel_ccb_readdata),
            .m_writeack                 (h_kernel_ccb_writeack),
            .m_empty                    ()  //unused
        );
        
        
        //burst splitter
        acl_burst_splitter #(
            .ADDRESS_WIDTH              (ADDRESS_WIDTH),
            .BURSTCOUNT_WIDTH           (BURSTCOUNT_WIDTH),
            .BYTEENABLE_WIDTH           (BYTEENABLE_WIDTH),
            .SPLIT_WRITE_BURSTS         (1),
            .SPLIT_READ_BURSTS          (1),
            .BURST_BOUNDARY             (BURST_BOUNDARY),
            .USE_STALL_LATENCY          (0),
            .ASYNC_RESET                (0),
            .SYNCHRONIZE_RESET          (1),
            .BACKPRESSURE_DURING_RESET  (0)
        )
        acl_burst_splitter_inst
        (
            .clock                      (h_clock),
            .resetn                     (h_resetn),
            
            .up_waitrequest             (h_kernel_ccb_waitrequest),
            .up_read                    (h_kernel_ccb_read),
            .up_write                   (h_kernel_ccb_write),
            .up_address                 (h_kernel_ccb_address),
            .up_writedata               (h_kernel_ccb_writedata),
            .up_byteenable              (h_kernel_ccb_byteenable),
            .up_burstcount              (h_kernel_ccb_burstcount),
            
            .down_waitrequest           (h_kernel_waitrequest[g]),
            .down_read                  (h_kernel_read_raw),
            .down_write                 (h_kernel_write_raw),
            .down_address               (h_kernel_address[g]),
            .down_writedata             (h_kernel_writedata[g]),
            .down_byteenable            (h_kernel_byteenable[g]),
            .down_burstcount            ()      //implicitly 1
        );
        
        //downstream of here is the pipelined arb between pcie and kernel, it uses "forced transaction" semantics
        assign h_kernel_read[g]  = h_kernel_read_raw  & ~h_kernel_waitrequest[g];
        assign h_kernel_write[g] = h_kernel_write_raw & ~h_kernel_waitrequest[g];
        
        //response path
        assign h_kernel_ccb_readdatavalid = h_kernel_readdatavalid[g];
        assign h_kernel_ccb_readdata      = h_kernel_readdata[g];
        assign h_kernel_ccb_writeack      = h_kernel_writeack[g];
    end
    endgenerate
    
    
    
    generate
    for (g=0; g<NUM_HBM; g++) begin : GEN_HBM_PIPELINED_ARB
        acl_hbm_pipelined_arb
        #(
            .ADDRESS_WIDTH                  (ADDRESS_WIDTH),
            .BYTEENABLE_WIDTH               (BYTEENABLE_WIDTH),
            .PIPE_STAGES_ARB_TO_CCB         (PIPE_STAGES_ARB_TO_CCB),
            .PIPE_STAGES_CCB_TO_ARB         (PIPE_STAGES_CCB_TO_ARB),
            .PIPE_STAGES_ARB_TO_FIFO        (PIPE_STAGES_ARB_TO_FIFO),
            .PIPE_STAGES_FIFO_TO_ARB        (PIPE_STAGES_FIFO_TO_ARB),
            .PIPE_STAGES_ARB_RESPONSE       (PIPE_STAGES_ARB_RESPONSE)
        )
        acl_hbm_pipelined_arb_inst
        (
            //common
            .clock                          ((g < NUM_HBM_BOTTOM) ? h_b_clock  : h_t_clock),
            .resetn                         ((g < NUM_HBM_BOTTOM) ? h_b_resetn : h_t_resetn),

            //pcie avalon slave
            .pcie_write                     (h_pcie_write[g]),
            .pcie_read                      (h_pcie_read[g]),
            .pcie_address                   (h_pcie_address[g]),
            .pcie_writedata                 (h_pcie_writedata[g]),
            .pcie_byteenable                (h_pcie_byteenable[g]),
            .pcie_waitrequest               (h_pcie_waitrequest[g]),
            .pcie_readdatavalid             (h_pcie_readdatavalid[g]),
            .pcie_readdata                  (h_pcie_readdata[g]),
            .pcie_writeack                  (h_pcie_writeack[g]),
            .pcie_empty                     (h_pcie_empty[g]),

            //kernel avalon slave
            .kernel_write                   (h_kernel_write[g]),
            .kernel_read                    (h_kernel_read[g]),
            .kernel_address                 (h_kernel_address[g]),
            .kernel_writedata               (h_kernel_writedata[g]),
            .kernel_byteenable              (h_kernel_byteenable[g]),
            .kernel_waitrequest             (h_kernel_waitrequest[g]),
            .kernel_readdatavalid           (h_kernel_readdatavalid[g]),
            .kernel_readdata                (h_kernel_readdata[g]),
            .kernel_writeack                (h_kernel_writeack[g]),
            
            //hbm avalon master
            .hbm_write                      (h_hbm_write[g]),
            .hbm_read                       (h_hbm_read[g]),
            .hbm_is_pcie                    (h_hbm_is_pcie[g]),
            .hbm_address                    (h_hbm_address[g]),
            .hbm_writedata                  (h_hbm_writedata[g]),
            .hbm_byteenable                 (h_hbm_byteenable[g]),
            .hbm_waitrequest                (h_hbm_waitrequest[g]),
            .hbm_readdatavalid_to_pcie      (h_hbm_readdatavalid_to_pcie[g]),
            .hbm_readdatavalid_to_kernel    (h_hbm_readdatavalid_to_kernel[g]),
            .hbm_readdata                   (h_hbm_readdata[g]),
            .hbm_writeack_to_pcie           (h_hbm_writeack_to_pcie[g]),
            .hbm_writeack_to_kernel         (h_hbm_writeack_to_kernel[g])
        );
    end
    endgenerate
    
    generate
    for (g=0; g<NUM_HBM; g++) begin : GEN_HBM_AVALON_TO_AXI
        //testbench requires that read requests and write requests stay in order, track the order in which reads and writes exit from pipelined arb
        //without this, tb_memory_slave_axi could see a valid in all 3 fifos and would have no idea whether it should process the read or write first
        //there is no backpressure on this interface because each SIM_ONLY_valid writes into a systemverilog queue
        //synthesis translate_off
        assign SIM_ONLY_valid[g] = h_hbm_write[g] | h_hbm_read[g];
        assign SIM_ONLY_is_write[g] = h_hbm_write[g];
        //synthesis translate_on
        
        acl_hbm_avalon_to_axi #(
            .ADDRESS_WIDTH                  (ADDRESS_WIDTH),
            .BYTEENABLE_WIDTH               (BYTEENABLE_WIDTH),
            .ARB_ROUND_TRIP_LATENCY         (PIPE_STAGES_ARB_TO_CCB + PIPE_STAGES_CCB_TO_ARB + PIPE_STAGES_ARB_TO_FIFO + PIPE_STAGES_FIFO_TO_ARB),
            .PIPE_STAGES_AXI_CMD_TO_HBM     (PIPE_STAGES_AXI_CMD_TO_HBM),
            .PIPE_STAGES_HBM_TO_AXI_CMD     (PIPE_STAGES_HBM_TO_AXI_CMD),
            .PIPE_STAGES_HBM_RESPONSE       (PIPE_STAGES_HBM_RESPONSE),
            .FIFO_DEPTH                     (AVALON_TO_AXI_FIFO_DEPTH),
            .FIFO_RAM_BLOCK_TYPE            ((AVALON_TO_AXI_FIFO_USE_MLAB[g]) ? "MLAB" : "M20K")
        )
        acl_hbm_avalon_to_axi_inst
        (   
            //common
            .clock                          ((g < NUM_HBM_BOTTOM) ? h_b_clock  : h_t_clock),
            .resetn                         ((g < NUM_HBM_BOTTOM) ? h_b_resetn : h_t_resetn),
            
            //hbm avalon
            .avs_write                      (h_hbm_write[g]),
            .avs_read                       (h_hbm_read[g]),
            .avs_is_pcie                    (h_hbm_is_pcie[g]),
            .avs_address                    (h_hbm_address[g]),
            .avs_writedata                  (h_hbm_writedata[g]),
            .avs_byteenable                 (h_hbm_byteenable[g]),
            .avs_waitrequest                (h_hbm_waitrequest[g]),
            .avs_readdatavalid_to_pcie      (h_hbm_readdatavalid_to_pcie[g]),
            .avs_readdatavalid_to_kernel    (h_hbm_readdatavalid_to_kernel[g]),
            .avs_readdata                   (h_hbm_readdata[g]),
            .avs_writeack_to_pcie           (h_hbm_writeack_to_pcie[g]),
            .avs_writeack_to_kernel         (h_hbm_writeack_to_kernel[g]),
            
            //hbm axi
            .axm_awid                       (h_hbm_awid[g]),
            .axm_awaddr                     (h_hbm_awaddr[g]),
            .axm_awlen                      (h_hbm_awlen[g]),
            .axm_awsize                     (h_hbm_awsize[g]),
            .axm_awburst                    (h_hbm_awburst[g]),
            .axm_awprot                     (h_hbm_awprot[g]),
            .axm_awuser                     (h_hbm_awuser[g]),
            .axm_awqos                      (h_hbm_awqos[g]),
            .axm_awvalid                    (h_hbm_awvalid[g]),
            .axm_awready                    (h_hbm_awready[g]),
            .axm_wdata                      (h_hbm_wdata[g]),
            .axm_wstrb                      (h_hbm_wstrb[g]),
            .axm_wlast                      (h_hbm_wlast[g]),
            .axm_wvalid                     (h_hbm_wvalid[g]),
            .axm_wready                     (h_hbm_wready[g]),
            .axm_bid                        (h_hbm_bid[g]),
            .axm_bresp                      (h_hbm_bresp[g]),
            .axm_bvalid                     (h_hbm_bvalid[g]),
            .axm_bready                     (h_hbm_bready[g]),
            .axm_arid                       (h_hbm_arid[g]),
            .axm_araddr                     (h_hbm_araddr[g]),
            .axm_arlen                      (h_hbm_arlen[g]),
            .axm_arsize                     (h_hbm_arsize[g]),
            .axm_arburst                    (h_hbm_arburst[g]),
            .axm_arprot                     (h_hbm_arprot[g]),
            .axm_aruser                     (h_hbm_aruser[g]),
            .axm_arqos                      (h_hbm_arqos[g]),
            .axm_arvalid                    (h_hbm_arvalid[g]),
            .axm_arready                    (h_hbm_arready[g]),
            .axm_rid                        (h_hbm_rid[g]),
            .axm_rdata                      (h_hbm_rdata[g]),
            .axm_rresp                      (h_hbm_rresp[g]),
            .axm_rlast                      (h_hbm_rlast[g]),
            .axm_rvalid                     (h_hbm_rvalid[g]),
            .axm_rready                     (h_hbm_rready[g])
        );
    end
    endgenerate
    
    
endmodule

`default_nettype wire
