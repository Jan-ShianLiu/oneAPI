module uniphy_status_20nm
#( 
  parameter WIDTH=32,
  parameter NUM_UNIPHYS=1
)
(
   input clk,
   input resetn,

   // Slave port
   input slave_read,
   output [WIDTH-1:0] slave_readdata,

   // hw.tcl won't let me index into a bit vector :(
   input  mem0_local_cal_success,
   input  mem0_local_cal_fail,

   input  mem1_local_cal_success,
   input  mem1_local_cal_fail,

   input  mem2_local_cal_success,
   input  mem2_local_cal_fail,

   input  mem3_local_cal_success,
   input  mem3_local_cal_fail,

   input  mem4_local_cal_success,
   input  mem4_local_cal_fail,

   input  mem5_local_cal_success,
   input  mem5_local_cal_fail,

   input  mem6_local_cal_success,
   input  mem6_local_cal_fail,

   input  mem7_local_cal_success,
   input  mem7_local_cal_fail,

   output  export_local_cal_success,
   output  export_local_cal_fail
);

reg [WIDTH-1:0] aggregate_uniphy_status;
wire  local_cal_success;
wire  local_cal_fail;
wire [7:0] mask;

assign mask = (NUM_UNIPHYS < 1) ? 0 : ~(8'hff << NUM_UNIPHYS);

assign local_cal_success = &( ~mask | {mem7_local_cal_success,
                                       mem6_local_cal_success,
                                       mem5_local_cal_success,
                                       mem4_local_cal_success,
                                       mem3_local_cal_success,
                                       mem2_local_cal_success,
                                       mem1_local_cal_success,
                                       mem0_local_cal_success});

assign local_cal_fail    = mem0_local_cal_fail |
                           mem1_local_cal_fail |
                           mem2_local_cal_fail |
                           mem3_local_cal_fail |
                           mem4_local_cal_fail |
                           mem5_local_cal_fail |
                           mem6_local_cal_fail |
                           mem7_local_cal_fail;

// Desire status==0 to imply success - may cause false positives, but the
// alternative is headaches for non-uniphy memories.
// Status MSB-LSB: 16 zeros, !calsuccess, calfail
always@(posedge clk or negedge resetn)
  if (!resetn)
    aggregate_uniphy_status <= {WIDTH{1'b0}};
  else
    aggregate_uniphy_status <= { 16'b0000000000000000,
                                  {~local_cal_success,local_cal_fail}
                                };

assign slave_readdata = aggregate_uniphy_status;
assign export_local_cal_success = local_cal_success;
assign export_local_cal_fail = local_cal_fail;

endmodule

