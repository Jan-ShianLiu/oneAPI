// Module is simulation model. Quartus synthesize RTL in
// global_routing.v
module global_routing
(
   input s,
   input clk,
   output g
);

  assign g = s;

endmodule
