//the fake PLL generates a 1X clock and 2X clock for OpenCL simulator
//- the reconfiguration of PLL frequency through a file written by aoc during compiling for simulator
//- the aoc flag is -sim-clk-freq=<value> (in MHz between 10 and 1000)

module aoc_sim_fake_pll #(
  parameter real REF_CLK_RATE = 125.0,
  parameter NUM_CLOCK_WAIT_TO_LOCK = 2
) (
  output logic       outclk_0,  // 1x clock
  output logic       outclk_1,  // 2x clock
  output logic       locked,
  input  wire        refclk,
  input  wire        reset
);

  timeunit 1ps;
  timeprecision 1ps;

  // file written by aoc compile
  // aoc uses -sim-clk-freq=<value> (in MHz) to generate a statement shown below, units in ps
  // Example: localparam real clock2x_half_period_ps = 250;
`include "set_sim_freq.svh";

  // The 2x clock
  initial begin
    outclk_1 = 1'b0;
    repeat(3) @(posedge refclk);
    outclk_1 = 1'b1;
    forever begin
      #(clock2x_half_period_ps) outclk_1 = ~outclk_1;
    end
  end

  // The 1x clock
  initial begin
    outclk_0 = 1'b0;
    forever @(posedge outclk_1) outclk_0 = ~outclk_0;
  end

  // The lock signal
  // 0 when reset is low, 1 after NUM_CLOCK_WAIT_TO_CLOCK of 1x_clock cycles. 1x clock is used
  // because there's no ambiguity of which clock edge. It'll always be positive edge on both
  // 1x clock and 2x clock
  always @(reset) begin
    if (reset)
      locked = 1'b0;
    else begin
      repeat(NUM_CLOCK_WAIT_TO_LOCK) @(posedge outclk_0);
      locked = 1'b1;
    end
  end

endmodule
