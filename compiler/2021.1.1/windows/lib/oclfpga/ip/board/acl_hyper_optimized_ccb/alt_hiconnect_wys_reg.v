
`timescale 1 ps / 1 ps

// baeckler - 03-06-2014

////////////////////////////////////////////
// DESCRIPTION
// 
// This is just a register with explicitly specified secondary signals. Used by some of the other examples to
// get a specific register signal mapping.
// 
// 

module alt_hiconnect_wys_reg #(
	parameter WIDTH = 16,
	parameter SIM_EMULATE = 1'b0
)(
	input clk,
	input arst,
	input [WIDTH-1:0] d,
	input ena,
	input sclr,		
	input sload,
	input [WIDTH-1:0] sdata,
	output [WIDTH-1:0] q
);

genvar i;
generate
if (SIM_EMULATE)
begin
	//////////////////////
	// soft register
	//////////////////////
	
	reg [WIDTH-1:0] q_r = {WIDTH{1'b0}};
	always @(posedge clk or posedge arst) begin
		if (arst) q_r <= 0;
		else begin
			if (ena) begin
				if (sclr) q_r <= 0;
				else if (sload) q_r <= sdata;
				else q_r <= d;
			end
		end
	end		
	assign q = q_r;
end
else begin
	//////////////////////
	// WYSIWYG register
	//////////////////////

	for (i=0; i<WIDTH; i=i+1) begin : lp
	
		dffeas r (
			.clk(clk),
			.ena(ena),
			.d (d[i]),
			.sload (sload),
			.asdata (sdata[i]),
			.sclr (sclr),
			.aload(1'b0),
			.clrn(!arst),
			// These are simulation-only chipwide
			// reset signals.  Both active low.				
			// synthesis translate_off
			.devpor(1'b1),
			.devclrn(1'b1),
			.prn(1'b1),
			// synthesis translate on
			.q (q[i])
		);
	end

end
endgenerate
endmodule

