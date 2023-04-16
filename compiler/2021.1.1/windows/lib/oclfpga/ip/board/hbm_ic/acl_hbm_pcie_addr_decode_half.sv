//__DESIGN_FILE_COPYRIGHT__
`default_nettype none
`include "acl_parameter_assert.svh"

//this module is a part of acl_hbm_pcie_addr_decode, which is part of acl_hbm_interconnect
//after bottom/top address decoding and some pipeline stages, each of bottom and top will connect to this module (typically there are two instances of this module)
//this module contains the following:
// - fifo to receive transactions, fifo is needed to produce an almost full back to the bottom/top decode, there are many pipeline stages between the fifo and bottom/top decode (acl_hbm_pcie_addr_decode_terminal)
// - decode for each hbm interface in this half (acl_hbm_pcie_addr_decode_terminal)
// - shift register to distribute commands and collect responses (acl_hbm_pcie_addr_decode_shiftreg)
// - clock crossing fifos (one command dcfifo per 2 hbm interfaces, one shared response dcfifo which lives inside acl_hbm_pcie_addr_decode_terminal)
//
//note that the command shiftreg operates on the pcie clock domain whereas the response shiftreg operates on the hbm clock domain
//it is preferable to run the shiftreg on the slower clock domain (like the command does), but in order to share the response dcfifo, the collection of response data has to be on the faster hbm clock
//
//also note that the burstcount is missing, bursts have already been split before the bottom/top address decode

module acl_hbm_pcie_addr_decode_half #(
    parameter int unsigned NUM_HBM,                 //total number of hbm interfaces
    parameter int unsigned NUM_HBM_BOTTOM,          //the first NUM_HBM_BOTTOM HBM interfaces will get mapped to the bottom instance
    parameter bit          IS_TOP,                  //0 for bottom, 1 for top
    parameter int unsigned ADDRESS_WIDTH,           //address width for kernel and hbm, pcie is wider
    parameter int unsigned BYTEENABLE_WIDTH,
    parameter bit          USE_WRITE_ACK,
    
    //after decode bottom/top and the pipeline stages that follow, transactions land in a fifo (one for each bottom/top half), configure this fifo
    parameter int unsigned PCIE_FIFO_DEPTH,
    parameter bit          PCIE_FIFO_USE_MLAB,
    parameter int unsigned PCIE_FIFO_ALMOST_FULL,   //must be at least as large as the roundtrip latency to the bottom/top address decode
    
    //clock crossing fifo settings
    parameter int unsigned CMD_DCFIFO_DEPTH,
    parameter int unsigned RSP_DCFIFO_DEPTH,
    parameter int unsigned CMD_DCFIFO_USE_MLAB,     //bit i indicates whether command dcfifo index i should use mlab (1) or m20k (0), note that indexing starts at NUM_HBM_START_INDEX
    parameter int unsigned RSP_DCFIFO_USE_MLAB,
    
    //derived parameters
    localparam int unsigned NUM_HBM_THIS_HALF = (IS_TOP) ? (NUM_HBM - NUM_HBM_BOTTOM) : NUM_HBM_BOTTOM, //how many hbm interfaces in this half
    localparam int unsigned NUM_HBM_START_INDEX = (IS_TOP) ? NUM_HBM_BOTTOM : 0,                        //index of the first hbm interface in this half
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH,
    localparam int unsigned PCIE_ADDRESS_WIDTH = ADDRESS_WIDTH + $clog2(NUM_HBM),           //address width for pcie is wider since the upper $clog2(NUM_HBM) bits indicate which HBM to target
    localparam int unsigned ADDRESS_BITS_PER_WORD = $clog2(BYTEENABLE_WIDTH),               //how many lower bits of the byte address are stuck at 0 to ensure it is word aligned
    localparam int unsigned WORD_ADDRESS_WIDTH = ADDRESS_WIDTH - ADDRESS_BITS_PER_WORD      //convert byte address width to word address width
) (
    input  wire                             p_clock,
    input  wire                             p_resetn,
    input  wire                             h_clock,
    input  wire                             h_resetn,
    
    //pcie after burst split and decoding for bottom/top
    input  wire                             p_valid,        //forced transaction
    input  wire                             p_is_write,     //this is considered data path, not control
    input  wire  [PCIE_ADDRESS_WIDTH-1:0]   p_address,
    input  wire  [DATA_WIDTH-1:0]           p_writedata,
    input  wire  [BYTEENABLE_WIDTH-1:0]     p_byteenable,
    output logic                            p_waitrequest,
    output logic                            p_readdatavalid,
    output logic [DATA_WIDTH-1:0]           p_readdata,
    output logic                            p_writeack,
    
    //leaves of address decode -- ccb to hbm
    output logic                            h_write         [NUM_HBM_THIS_HALF-1:0],
    output logic                            h_read          [NUM_HBM_THIS_HALF-1:0],
    output logic [ADDRESS_WIDTH-1:0]        h_address       [NUM_HBM_THIS_HALF-1:0],
    output logic [DATA_WIDTH-1:0]           h_writedata     [NUM_HBM_THIS_HALF-1:0],
    output logic [BYTEENABLE_WIDTH-1:0]     h_byteenable    [NUM_HBM_THIS_HALF-1:0],
    input  wire                             h_waitrequest   [NUM_HBM_THIS_HALF-1:0],
    input  wire                             h_readdatavalid [NUM_HBM_THIS_HALF-1:0],
    input  wire  [DATA_WIDTH-1:0]           h_readdata      [NUM_HBM_THIS_HALF-1:0],
    input  wire                             h_writeack      [NUM_HBM_THIS_HALF-1:0],
    output logic                            h_empty         [NUM_HBM_THIS_HALF-1:0]     //for arb bewteen pcie and kernel
);
    
    
    //////////////////////////////////////
    //                                  //
    //  Sanity check on the parameters  //
    //                                  //
    //////////////////////////////////////
    
    generate
    `ACL_PARAMETER_ASSERT(NUM_HBM_THIS_HALF >= 1)
    `ACL_PARAMETER_ASSERT(WORD_ADDRESS_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH == 2**ADDRESS_BITS_PER_WORD)
    `ACL_PARAMETER_ASSERT(PCIE_FIFO_DEPTH > PCIE_FIFO_ALMOST_FULL)
    `ACL_PARAMETER_ASSERT(CMD_DCFIFO_DEPTH >= 32)
    `ACL_PARAMETER_ASSERT(RSP_DCFIFO_DEPTH >= 32)
    endgenerate
    
    
    
    ///////////////////////////
    //                       //
    //  Signal declarations  //
    //                       //
    ///////////////////////////
    
    genvar g;
    
    //slave side of ccb
    logic                         p_ccb_valid              [NUM_HBM_THIS_HALF-1:0];
    logic                         p_ccb_is_write           [NUM_HBM_THIS_HALF-1:0];
    logic     [ADDRESS_WIDTH-1:0] p_ccb_address            [NUM_HBM_THIS_HALF-1:0];
    logic        [DATA_WIDTH-1:0] p_ccb_writedata          [NUM_HBM_THIS_HALF-1:0];
    logic  [BYTEENABLE_WIDTH-1:0] p_ccb_byteenable         [NUM_HBM_THIS_HALF-1:0];
    logic                         p_ccb_waitrequest        [NUM_HBM_THIS_HALF-1:0];
    
    //command shiftreg on pcie clock
    logic [NUM_HBM_THIS_HALF-1:0] p_shiftreg_valid_one_hot [NUM_HBM_THIS_HALF:0];
    logic                         p_shiftreg_is_write      [NUM_HBM_THIS_HALF:0];
    logic     [ADDRESS_WIDTH-1:0] p_shiftreg_address       [NUM_HBM_THIS_HALF:0];
    logic        [DATA_WIDTH-1:0] p_shiftreg_writedata     [NUM_HBM_THIS_HALF:0];
    logic  [BYTEENABLE_WIDTH-1:0] p_shiftreg_byteenable    [NUM_HBM_THIS_HALF:0];
    logic                         p_shiftreg_waitrequest   [NUM_HBM_THIS_HALF:0];
    
    //response shiftreg on hbm clock
    logic                         h_shiftreg_readdatavalid [NUM_HBM_THIS_HALF:0];
    logic [DATA_WIDTH-1:0]        h_shiftreg_readdata      [NUM_HBM_THIS_HALF:0];
    logic                         h_shiftreg_writeack      [NUM_HBM_THIS_HALF:0];
    
    
    
    //decode valid for each hbm interface
    acl_hbm_pcie_addr_decode_terminal #(
        .NUM_HBM                    (NUM_HBM),
        .NUM_HBM_BOTTOM             (NUM_HBM_BOTTOM),
        .IS_TOP                     (IS_TOP),
        .ADDRESS_WIDTH              (ADDRESS_WIDTH),
        .BYTEENABLE_WIDTH           (BYTEENABLE_WIDTH),
        .USE_WRITE_ACK              (USE_WRITE_ACK),
        .PCIE_FIFO_DEPTH            (PCIE_FIFO_DEPTH),
        .PCIE_FIFO_USE_MLAB         (PCIE_FIFO_USE_MLAB),
        .PCIE_FIFO_ALMOST_FULL      (PCIE_FIFO_ALMOST_FULL),
        .RSP_DCFIFO_DEPTH           (RSP_DCFIFO_DEPTH),
        .RSP_DCFIFO_USE_MLAB        (RSP_DCFIFO_USE_MLAB)
    )
    terminal_inst
    (
        .p_clock                    (p_clock),
        .p_resetn                   (p_resetn),
        .h_clock                    (h_clock),
        .h_resetn                   (h_resetn),
        
        //pcie after burst split and decoding for bottom/top
        .p_pcie_valid               (p_valid),           //forced transaction
        .p_pcie_is_write            (p_is_write),            //this is considered data path, not control
        .p_pcie_address             (p_address),
        .p_pcie_writedata           (p_writedata),
        .p_pcie_byteenable          (p_byteenable),
        .p_pcie_waitrequest         (p_waitrequest),
        .p_pcie_readdatavalid       (p_readdatavalid),
        .p_pcie_readdata            (p_readdata),
        .p_pcie_writeack            (p_writeack),
        
        //pcie after decode - entrance to shiftreg
        .p_shiftreg_valid_one_hot   (p_shiftreg_valid_one_hot[0]),
        .p_shiftreg_is_write        (p_shiftreg_is_write[0]),
        .p_shiftreg_address         (p_shiftreg_address[0]),
        .p_shiftreg_writedata       (p_shiftreg_writedata[0]),
        .p_shiftreg_byteenable      (p_shiftreg_byteenable[0]),
        .p_shiftreg_waitrequest     (p_shiftreg_waitrequest[0]),
        .h_shiftreg_readdatavalid   (h_shiftreg_readdatavalid[0]),
        .h_shiftreg_readdata        (h_shiftreg_readdata[0]),
        .h_shiftreg_writeack        (h_shiftreg_writeack[0])
    );
    
    
    
    //tie off reverse direction ends
    assign p_shiftreg_waitrequest[NUM_HBM_THIS_HALF] = 1'b0;
    assign h_shiftreg_readdatavalid[NUM_HBM_THIS_HALF] = 1'b0;
    assign h_shiftreg_writeack[NUM_HBM_THIS_HALF] = 1'b0;
    
    //this only reason this exists as a module instead of just coding directly here is because of logic lock, it was easier to logic lock by module
    generate
    for (g=0; g<NUM_HBM_THIS_HALF; g++) begin : GEN_SHIFTREG
        acl_hbm_pcie_addr_decode_shiftreg #(
            .NUM_HBM_THIS_HALF          (NUM_HBM_THIS_HALF),
            .ADDRESS_WIDTH              (ADDRESS_WIDTH),
            .BYTEENABLE_WIDTH           (BYTEENABLE_WIDTH),
            .PIPELINE_CMD_FORWARD       ((g & 1) ? 0 : 1),  //command shiftreg: 1 pipeline stage every 2 HBM interfaces, this runs on the slower pcie clock
            .PIPELINE_CMD_BACKPRESSURE  ((g & 1) ? 0 : 1),  //likewise for command backpressure
            .PIPELINE_RSP               (1)                 //response shiftreg: 1 pipeline stage every HBM interface, this runs on the faster hbm clock
        )
        shiftreg_inst
        (
            .p_clock                    (p_clock),
            .p_resetn                   (p_resetn),
            .h_clock                    (h_clock),
            .h_resetn                   (h_resetn),
        
            //command forward in
            .p_left_valid_one_hot       (p_shiftreg_valid_one_hot[g]),
            .p_left_is_write            (p_shiftreg_is_write[g]),
            .p_left_address             (p_shiftreg_address[g]),
            .p_left_writedata           (p_shiftreg_writedata[g]),
            .p_left_byteenable          (p_shiftreg_byteenable[g]),
            
            //command forward out
            .p_right_valid_one_hot      (p_shiftreg_valid_one_hot[g+1]),
            .p_right_is_write           (p_shiftreg_is_write[g+1]),
            .p_right_address            (p_shiftreg_address[g+1]),
            .p_right_writedata          (p_shiftreg_writedata[g+1]),
            .p_right_byteenable         (p_shiftreg_byteenable[g+1]),
            
            //command backpressure in
            .p_right_waitrequest        (p_shiftreg_waitrequest[g+1]),
            .p_ccb_waitrequest          (p_ccb_waitrequest[g]),
            
            //command backpressure out
            .p_left_waitrequest         (p_shiftreg_waitrequest[g]),
            
            //response in
            .h_right_readdatavalid      (h_shiftreg_readdatavalid[g+1]),
            .h_ccb_readdatavalid        (h_readdatavalid[g]),
            .h_right_readdata           (h_shiftreg_readdata[g+1]),
            .h_ccb_readdata             (h_readdata[g]),
            .h_right_writeack           (h_shiftreg_writeack[g+1]),
            .h_ccb_writeack             (h_writeack[g]),
            
            //response out
            .h_left_readdatavalid       (h_shiftreg_readdatavalid[g]),
            .h_left_readdata            (h_shiftreg_readdata[g]),
            .h_left_writeack            (h_shiftreg_writeack[g])
        );
        
        assign p_ccb_valid[g]      = p_shiftreg_valid_one_hot[g+1][g];
        assign p_ccb_is_write[g]   = p_shiftreg_is_write[g+1];
        assign p_ccb_address[g]    = p_shiftreg_address[g+1];
        assign p_ccb_writedata[g]  = p_shiftreg_writedata[g+1];
        assign p_ccb_byteenable[g] = p_shiftreg_byteenable[g+1];
    end
    endgenerate
    
    
    
    //clock cross the command interface from pcie to hbm clock
    //every two hbm interfaces share the same shiftreg stage, so share the cmd dcfifo as well
    generate
    for (g=0; g<NUM_HBM_THIS_HALF; g=g+2) begin : GEN_CCB
        localparam int unsigned CLOCKS_FROM_DECODE = (g+2) / 2; //index 0 and 1 are 1 clock away, index 2 and 3 are 2 clocks away, index 4 and 5 are 3 clocks away ...
        
        logic p_wr_req;     //write request to cmd dcfifo
        logic p_is_upper;   //one bit of data path to indicate whether transaction is for first or second hbm interface shared by this dcfifo
        if (g+1 < NUM_HBM_THIS_HALF) begin  //the next interface exists
            assign p_wr_req = p_ccb_valid[g] | p_ccb_valid[g+1];
            assign p_is_upper = p_ccb_valid[g+1];
        end
        else begin  //odd number of interfaces and this is the last one by itself
            assign p_wr_req = p_ccb_valid[g];
            assign p_is_upper = 1'b0;
        end
        
        logic h_is_write, h_is_upper, h_rd_ack, h_empty_raw;
        logic [WORD_ADDRESS_WIDTH-1:0] p_word_address;
        logic [WORD_ADDRESS_WIDTH-1:0] h_word_address;
        
        assign p_word_address = p_ccb_address[g] >> ADDRESS_BITS_PER_WORD;
        
        acl_dcfifo #(
            .WIDTH                  (WORD_ADDRESS_WIDTH + BYTEENABLE_WIDTH + DATA_WIDTH + 2),
            .DEPTH                  (CMD_DCFIFO_DEPTH),
            .ALMOST_FULL_CUTOFF     (2*CLOCKS_FROM_DECODE), //latency to and from
            .NEVER_OVERFLOWS        (1),
            .RAM_BLOCK_TYPE         ((CMD_DCFIFO_USE_MLAB[g+NUM_HBM_START_INDEX]) ? "MLAB" : "M20K")
        )
        cmd_dcfifo
        (
            .async_resetn           (p_resetn & h_resetn),
            .wr_clock               (p_clock),
            .wr_req                 (p_wr_req),
            .wr_data                ({p_word_address, p_ccb_byteenable[g], p_ccb_writedata[g], p_ccb_is_write[g], p_is_upper}),
            .wr_full                (),
            .wr_almost_full         (p_ccb_waitrequest[g]),
            .wr_read_update_for_ccb (),
            .rd_clock               (h_clock),
            .rd_ack                 (h_rd_ack),
            .rd_empty               (h_empty_raw),
            .rd_almost_empty        (),
            .rd_data                ({h_word_address, h_byteenable[g], h_writedata[g], h_is_write, h_is_upper})
        );
        
        //convert back to byte address
        assign h_address[g] = h_word_address << ADDRESS_BITS_PER_WORD;
        
        if (g+1 < NUM_HBM_THIS_HALF) begin  //the next interface exists
            assign p_ccb_waitrequest[g+1] = 1'b0;   //any backpressure is enough
            
            //this is not safe in general, it works because:
            //- outstanding transactions can only target one hbm at a time, so we won't be switching between the hbm interfaces on a per-clock-cycle basis
            //- arbitration defaults to letting the kernel data through, only when pcie is not empty will the arb eventually deassert waitrequest
            assign h_rd_ack = ~h_waitrequest[g] | ~h_waitrequest[g+1];
            
            assign h_empty[g]    = h_empty_raw |  h_is_upper;
            assign h_empty[g+1]  = h_empty_raw | ~h_is_upper;
            
            //downstream of here is the arb between pcie and kernel, that uses stall latency (forced transaction) style of handshaking
            assign h_read[g]    = ~h_empty[g]   & ~h_waitrequest[g]   & ~h_is_write;
            assign h_write[g]   = ~h_empty[g]   & ~h_waitrequest[g]   &  h_is_write;
            assign h_read[g+1]  = ~h_empty[g+1] & ~h_waitrequest[g+1] & ~h_is_write;
            assign h_write[g+1] = ~h_empty[g+1] & ~h_waitrequest[g+1] &  h_is_write;
            
            //replicate the rest of the data path signals
            assign h_address[g+1]    = h_address[g];
            assign h_byteenable[g+1] = h_byteenable[g];
            assign h_writedata[g+1]  = h_writedata[g];
        end
        else begin  //odd number of interfaces and this is the last one by itself
            assign h_rd_ack   = ~h_waitrequest[g];
            assign h_empty[g] =  h_empty_raw;
            assign h_read[g]  = ~h_empty[g] & ~h_waitrequest[g] & ~h_is_write;
            assign h_write[g] = ~h_empty[g] & ~h_waitrequest[g] & h_is_write;
        end
    end
    endgenerate
    
endmodule

`default_nettype wire
