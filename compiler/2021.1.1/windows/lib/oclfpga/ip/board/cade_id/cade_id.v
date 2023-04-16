module cade_id
#( 
  parameter WIDTH=32
)
(
   input clk,
   input resetn,

   // Slave port
   input slave_address,  // Word address
   input [WIDTH-1:0] slave_writedata,
   input slave_read,
   input slave_write,
   input [WIDTH/8-1:0] slave_byteenable,
   output [WIDTH-1:0] slave_readdata,
   output reg slave_readdatavalid,
   output slave_waitrequest,

   output reg [31:0]      probe
);

reg [31:0] cade_id;

always@(posedge clk)
  if (slave_write) begin
    cade_id <= {slave_byteenable[3] ? slave_writedata[31:24] : cade_id[31:24],
			slave_byteenable[2] ? slave_writedata[23:16] : cade_id[23:16],
			slave_byteenable[1] ? slave_writedata[15:8] : cade_id[15:8],
			slave_byteenable[0] ? slave_writedata[7:0] : cade_id[7:0]};
  end

//pipeline the register to system probe
always@(posedge clk) probe[31:0] <= cade_id[31:0];

// rddata valid
always@(posedge clk) slave_readdatavalid <= slave_read;

assign slave_waitrequest = 1'b0; 
assign slave_readdata[31:0] = cade_id[31:0];
  
endmodule

