`timescale 1ps / 1ps

// -------------------------------------------------------------------
// Dual clock FIFO. N bits wide. 5 addr lines.
//
// Originally generated by one of Gregg's toys. Modified for our
// needs. Share And Enjoy.
// -------------------------------------------------------------------

module alt_st_mlab_dcfifo #(
    parameter DATA_WIDTH = 128,
    parameter PREVENT_UNDERFLOW = 0,
    parameter READY_THRESHOLD = 14,
    parameter SIM_EMULATE = 1'b0
) 
(
    input din_clk,
    input din_sclr,
    input din_wreq,
    input [DATA_WIDTH-1:0] din,
    output reg din_ready,

    input dout_clk,
    input dout_sclr,
    input dout_rreq,
    output reg dout_valid,
    output [DATA_WIDTH-1:0] dout,
    output dout_qual_rreq
);

///////////////////////////////
// resets

reg din_sclr_r = 1'b0 /* synthesis dont_merge */;
always @(posedge din_clk) begin
    din_sclr_r <= din_sclr;
end

reg dout_sclr_r = 1'b0 /* synthesis dont_merge */;
always @(posedge dout_clk) begin
    dout_sclr_r <= dout_sclr;
end

///////////////////////////////
// underflow prevention
wire qual_rreq;
reg allow_read;

assign qual_rreq = (PREVENT_UNDERFLOW) ? dout_rreq & allow_read :
                                        dout_rreq;

///////////////////////////////
// write pointer

wire [4:0] din_wptr;
alt_hiconnect_cnt5ic wr_ctr (
    .clk(din_clk),
    .inc(din_wreq),
    .sclr(din_sclr_r),
    .dout(din_wptr)
);
defparam wr_ctr .SIM_EMULATE = SIM_EMULATE;

///////////////////////////////
// read pointer

wire [4:0] dout_rptr;
alt_hiconnect_cnt5ic rd_ctr (
    .clk(dout_clk),
    .inc(qual_rreq),
    .sclr(dout_sclr_r),
    .dout(dout_rptr)
);
defparam rd_ctr .SIM_EMULATE = SIM_EMULATE;

///////////////////////////////
// storage

localparam MEM_WIDTH = (DATA_WIDTH % 20 == 0) ? DATA_WIDTH :
                                               (DATA_WIDTH / 20 + 1) * 20;
localparam RAM_BLOCKS = MEM_WIDTH / 20;

genvar i;

wire [MEM_WIDTH-1:0] sized_din;
wire [MEM_WIDTH-1:0] sized_dout;
wire [MEM_WIDTH-1:0] dout_w;
wire [4:0] waddr;
wire [4:0] raddr;

assign sized_din = din;
assign dout_w = sized_dout;

assign waddr = din_wptr;
assign raddr = dout_rptr;

generate for (i = 0; i < RAM_BLOCKS; i=i+1) begin : mem

    reg [4:0] waddr_m = 5'b0 /* synthesis dont_merge */;
    always @(posedge din_clk) waddr_m <= waddr;

    reg [19:0] wdata_m = 20'b0 /* synthesis dont_merge noprune */;
    always @(posedge din_clk) wdata_m <= sized_din[(i+1)*20-1:i*20];

    reg [4:0] raddr_m = 5'b0 /* synthesis dont_merge */;
    always @(posedge dout_clk) raddr_m <= raddr;

    alt_hiconnect_mlab m (
        .wclk(din_clk),
        .wena(1'b1),
        .waddr_reg(waddr_m),
        .wdata_reg(wdata_m),
        .raddr(raddr_m),
        .rdata(sized_dout[(i+1)*20-1:i*20])
    );
    defparam m .WIDTH = 20;
    defparam m .ADDR_WIDTH = 5;
    defparam m .SIM_EMULATE = SIM_EMULATE;

end
endgenerate

///////////////////////////////////
// pointer cross domain exchange

wire [4:0] dout_xwptr;
wire [4:0] din_xrptr;

alt_hiconnect_gpx5 gx0 (
    .din_clk(din_clk),
    .din(din_wptr),
    .dout_clk(dout_clk),
    .dout(dout_xwptr)
);
defparam gx0 .SIM_EMULATE = SIM_EMULATE;

alt_hiconnect_gpx5 gx1 (
    .din_clk(dout_clk),
    .din(dout_rptr),
    .dout_clk(din_clk),
    .dout(din_xrptr)
);
defparam gx1 .SIM_EMULATE = SIM_EMULATE;

///////////////////////////////////
// compute used words

reg [4:0] rusedw = 5'b0;
reg [4:0] wusedw = 5'b0;

always @(posedge dout_clk) begin
    rusedw <= dout_xwptr - dout_rptr;
end

always @(posedge din_clk) begin
    wusedw <= din_wptr - din_xrptr;
end

///////////////////////////////
// compute almost full signal

always @(posedge din_clk) begin
    din_ready <= (wusedw < READY_THRESHOLD);
end

///////////////////////////////
// throttle reads after dropping below a threshold

reg empty_delayed;
wire almost_empty;
reg fast_read_mode;
reg [1:0] slow_mode_read_ctr;
wire slow_mode_allow_read;

always @(posedge dout_clk) begin
    empty_delayed <= (rusedw == 0);
end

assign almost_empty = (rusedw[4:2] == 3'h0); // rusedw < 4

always @(posedge dout_clk) begin
    if (dout_sclr_r) begin
        fast_read_mode <= 1'b0;
    end else begin
        if (almost_empty)
            fast_read_mode <= 1'b0;
        else if (~almost_empty)
            fast_read_mode <= 1'b1;
    end
end

always @(posedge dout_clk) begin
    if (dout_sclr_r) begin
        slow_mode_read_ctr <= 2'h3;
    end else begin
        if (~fast_read_mode)
            slow_mode_read_ctr <= slow_mode_read_ctr - 2'h1;
        if (fast_read_mode)
            slow_mode_read_ctr <= 2'h3;
    end
end

assign slow_mode_allow_read = (slow_mode_read_ctr == 2'h0) && ~empty_delayed;

always @(posedge dout_clk) begin
    allow_read <= (fast_read_mode | slow_mode_allow_read);
end

///////////////////////////////
// compute valid signal

reg dout_rreq_r;

always @(posedge dout_clk) begin
    dout_rreq_r <= qual_rreq;
    dout_valid <= dout_rreq_r;
end
assign dout_qual_rreq = qual_rreq;    //expose this to the outside world for occupancy tracking

///////////////////////////////
// output registers

reg [MEM_WIDTH-1:0] dout_r = {MEM_WIDTH{1'b0}};

generate for (i = 0; i < RAM_BLOCKS; i=i+1) begin : oreg

    reg rdena = 1'b0 /* synthesis dont_merge */;
    always @(posedge dout_clk) begin
        rdena <= qual_rreq;
        if (rdena) dout_r[(i+1)*20-1:i*20] <= dout_w[(i+1)*20-1:i*20];
    end

end endgenerate

assign dout = dout_r[DATA_WIDTH-1:0];

endmodule

