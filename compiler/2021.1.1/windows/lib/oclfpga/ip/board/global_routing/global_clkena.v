
module global_clockena
(
   input s,
   input clk,
   output g
);

				twentynm_clkena #(
				  .clock_type("GLOBAL CLOCK")
				) clkena (
				  .inclk (s),
				  .outclk(g)
                                 ); 

endmodule
