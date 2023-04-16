// This module serves the same purpose as timer.v in the same directory but was optimized
// to achieve better fmax on the kernel clock domain for Stratix 10 devices.
// From the OpenCL host runtime the kernel clock frequency can be estimated by clearing the
// counters, waiting for a known amount of time and then reading back the counter values.
//
// The original counter was replaced by the acl_multistage_accumulator.
// It is instantiated as part of the kernel clock generator IP which for S10 is located within
// the board variant directory and named acl_kernel_clk_s10_reconfig_hw.tcl  
//
// The slave_write clears both counters while at the same time slave_writedata determines
// which counter output will be muxed onto the output slave_readdata.
// A slave_read will result in the selected counter value.

module acl_timer_s10 #(
  parameter WIDTH = 64,		// bit width of timer and AVMM data interface 
  parameter USE_2XCLK = 0,	// in case the 2x clock is present in the design the output of both 1x and 2x clock counter will be available
  parameter S_WIDTH_A = 2	// word address width of the AVMM interface
)
(
  input clk,			// kernel clock input
  input clk2x,			// 2x kernel clock input
  input resetn,			// reset input from kernel clock generator toplevel (currently asynchronous)

  // Slave port AVMM interface
  input [S_WIDTH_A-1:0] slave_address,  // Word address
  input [WIDTH-1:0] slave_writedata,
  input slave_read,
  input slave_write,
  input [WIDTH/8-1:0] slave_byteenable,
  output slave_waitrequest,
  output [WIDTH-1:0] slave_readdata,
  output slave_readdatavalid
);

  wire [WIDTH-1:0] counter;
  wire [WIDTH-1:0] counter2x;
  reg  clock_sel;
  wire resetn_synchronized;
  wire sclrn;

  acl_reset_handler #(
    .ASYNC_RESET	(0),
    .USE_SYNCHRONIZER	(1),
    .SYNCHRONIZE_ACLRN	(1),
    .PIPE_DEPTH		(3),
    .NUM_COPIES		(1)
  ) reset_handler_inst (  
    .clk(clk),
    .i_resetn(resetn),
    .o_aclrn(),
    .o_resetn_synchronized(resetn_synchronized),
    .o_sclrn(sclrn)
  );

  // select either the 1x or 2x clock counter
  // based on slave_writedata 
  always@(posedge clk) begin
    if (!sclrn) begin
      clock_sel <= 1'b0;
    end
    else if (slave_write) begin
      if (|slave_writedata) begin
        clock_sel <= 1'b1;
      end
      else begin
        clock_sel <= 1'b0;
      end
    end
  end

  // counter for 1x kernel clock
  acl_multistage_accumulator #(
    .ACCUMULATOR_WIDTH	(WIDTH),
    .INCREMENT_WIDTH	(1),
    .SECTION_SIZE	(16),
    .ASYNC_RESET	(0),
    .SYNCHRONIZE_RESET	(0)
  ) counter1x_inst (
    .clock(clk),
    .resetn(resetn_synchronized),
    .clear(slave_write),
    .result(counter),
    .increment(1'b1),
    .go(1'b1)
  );

  // counter for 2x kernel clock
  acl_multistage_accumulator #(
    .ACCUMULATOR_WIDTH	(WIDTH),
    .INCREMENT_WIDTH	(1),
    .SECTION_SIZE	(16),
    .ASYNC_RESET	(0),
    .SYNCHRONIZE_RESET	(0)
  ) counter2x_inst (
    .clock(clk2x),
    .resetn(resetn_synchronized),
    .clear(slave_write),
    .result(counter2x),
    .increment(1'b1),
    .go(1'b1)
  );

  assign slave_waitrequest = 1'b0;
  assign slave_readdata = (USE_2XCLK && clock_sel) ? counter2x : counter;
  assign slave_readdatavalid = slave_read;

endmodule
