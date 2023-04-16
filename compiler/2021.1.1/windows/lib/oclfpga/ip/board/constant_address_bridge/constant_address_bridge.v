// --------------------------------------
// Avalon-MM constant address bridge
// --------------------------------------
`default_nettype none
module constant_address_bridge
#(
  parameter DATA_WIDTH          = 32,  		// data width of the AVMM interface
  parameter ADDRESS_WIDTH       = 28,       	// address width of the AVMM interface
  parameter BURSTCOUNT_WIDTH    = 4,		// burstcount width of the AVMM interface
  parameter CONSTANT_ADDRESS    = 'h0,      	// constant address to write to
  parameter CONSTANT_BURSTCOUNT = 4'b0001 	// constant burstcount to set
)
(
  input   wire                          clk,      // clock input
  input   wire                          reset,    // reset input

  // Avalon-MM slave interface (sink for previous stage in data path) 
  output  logic                         s0_waitrequest,
  input   wire [BURSTCOUNT_WIDTH-1:0]   s0_burstcount,
  input   wire [DATA_WIDTH-1:0]         s0_writedata,
  input   wire [ADDRESS_WIDTH-1:0]      s0_address, 
  input   wire                          s0_write, 
  input   wire [(DATA_WIDTH/8)-1:0]   	s0_byteenable, 

  // Avalon-MM master interface (source for next stage in data path)
  input   wire                          m0_waitrequest,
  output  logic [BURSTCOUNT_WIDTH-1:0]  m0_burstcount,
  output  logic [DATA_WIDTH-1:0]        m0_writedata,
  output  logic [ADDRESS_WIDTH-1:0]     m0_address, 
  output  logic                         m0_write, 
  output  logic [(DATA_WIDTH/8)-1:0] 	m0_byteenable
);

  // feed signals through unregistered
  assign s0_waitrequest = m0_waitrequest;
  assign m0_burstcount  = CONSTANT_BURSTCOUNT;
  assign m0_writedata   = s0_writedata;
  assign m0_address     = CONSTANT_ADDRESS;
  assign m0_write       = s0_write;
  assign m0_byteenable  = s0_byteenable; 

endmodule
`default_nettype wire
