
module periph_clkena
(
   input s,
   input clk,
   output g
);

				twentynm_clkena #(
				  .clock_type("PERIPHERY CLOCK")
				) clkena (
				  .inclk (s),
				  .outclk(g)
                                 ); 

endmodule
