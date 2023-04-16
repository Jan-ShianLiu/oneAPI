//__DESIGN_FILE_COPYRIGHT__
`default_nettype none
`include "acl_parameter_assert.svh"

//this module is part of acl_hbm_pcie_addr_decode_half, which is part of acl_hbm_interconnect
//valid has already been decoded for each hbm interface
//instantiate this module N times to distribute commands and collect responses for N HBM interfaces
//each instance can choose whetheer to register or not
//the primary reason this is its own module was for logic lock, easier to logic lock per module
//
//command shiftreg:
// - goes from left to right, except for backpressure which goes from right to left
// - runs on the pcie clock domain
//
//response shiftreg:
// - goes from right to left, no backpressure
// - by construction collisions inserting into the shiftreg cannot happen since outstanding transactions can only target 1 HBM interface at a time
// - runs on the hbm clock domain

module acl_hbm_pcie_addr_decode_shiftreg #(
    //signal widths
    parameter int unsigned NUM_HBM_THIS_HALF,       //how many hbm interfaces in this half
    parameter int unsigned ADDRESS_WIDTH,
    parameter int unsigned BYTEENABLE_WIDTH,
    
    //choose whether or not to register each stage
    parameter bit          PIPELINE_CMD_FORWARD,
    parameter bit          PIPELINE_CMD_BACKPRESSURE,
    parameter bit          PIPELINE_RSP,
    
    //derived parameters
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH
) (
    input  wire                          p_clock,
    input  wire                          p_resetn,
    input  wire                          h_clock,
    input  wire                          h_resetn,
    
    //command forward in
    input  wire  [NUM_HBM_THIS_HALF-1:0] p_left_valid_one_hot,
    input  wire                          p_left_is_write,
    input  wire  [ADDRESS_WIDTH-1:0]     p_left_address,
    input  wire  [DATA_WIDTH-1:0]        p_left_writedata,
    input  wire  [BYTEENABLE_WIDTH-1:0]  p_left_byteenable,
    
    //command forward out
    output logic [NUM_HBM_THIS_HALF-1:0] p_right_valid_one_hot,
    output logic                         p_right_is_write,
    output logic [ADDRESS_WIDTH-1:0]     p_right_address,
    output logic [DATA_WIDTH-1:0]        p_right_writedata,
    output logic [BYTEENABLE_WIDTH-1:0]  p_right_byteenable,
    
    //command backpressure in
    input  wire                          p_right_waitrequest,
    input  wire                          p_ccb_waitrequest,
    
    //command backpressure out
    output logic                         p_left_waitrequest,
    
    //response in
    input  wire                          h_right_readdatavalid,
    input  wire                          h_ccb_readdatavalid,
    input  wire  [DATA_WIDTH-1:0]        h_right_readdata,
    input  wire  [DATA_WIDTH-1:0]        h_ccb_readdata,
    input  wire                          h_right_writeack,
    input  wire                          h_ccb_writeack,
    
    //response out
    output logic                         h_left_readdatavalid,
    output logic [DATA_WIDTH-1:0]        h_left_readdata,
    output logic                         h_left_writeack
);

    //parameters were already sanity checked in acl_hbm_pcie_addr_decode_half
    
    //pipeline the command to each hbm and pipeline the backpressure from each hbm
    generate
    if (PIPELINE_CMD_FORWARD) begin
        always_ff @(posedge p_clock) begin
            p_right_valid_one_hot <= p_left_valid_one_hot;
            p_right_is_write      <= p_left_is_write;
            p_right_address       <= p_left_address;
            p_right_writedata     <= p_left_writedata;
            p_right_byteenable    <= p_left_byteenable;
        end
    end
    else begin
        always_comb begin
            p_right_valid_one_hot = p_left_valid_one_hot;
            p_right_is_write      = p_left_is_write;
            p_right_address       = p_left_address;
            p_right_writedata     = p_left_writedata;
            p_right_byteenable    = p_left_byteenable;
        end
    end
    endgenerate
    
    generate
    if (PIPELINE_CMD_BACKPRESSURE) begin
        always_ff @(posedge p_clock) begin
            p_left_waitrequest <= p_right_waitrequest | p_ccb_waitrequest;
        end
    end
    else begin
        always_comb begin
            p_left_waitrequest = p_right_waitrequest | p_ccb_waitrequest;
        end
    end
    endgenerate
    
    
    generate
    if (PIPELINE_RSP) begin
        always_ff @(posedge h_clock) begin
            h_left_readdatavalid <= h_right_readdatavalid | h_ccb_readdatavalid;
            h_left_readdata      <= (h_ccb_readdatavalid) ? h_ccb_readdata : h_right_readdata;
            h_left_writeack      <= h_right_writeack | h_ccb_writeack;
        end
    end
    else begin
        always_comb begin
            h_left_readdatavalid = h_right_readdatavalid | h_ccb_readdatavalid;
            h_left_readdata      = (h_ccb_readdatavalid) ? h_ccb_readdata : h_right_readdata;
            h_left_writeack      = h_right_writeack | h_ccb_writeack;
        end
    end
    endgenerate
    

endmodule

`default_nettype wire
