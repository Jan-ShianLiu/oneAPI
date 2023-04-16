/////////////////////////////////////////////////////////////////
// MODULE kernel_system
/////////////////////////////////////////////////////////////////
module kernel_system
(
   input logic clock_reset_clk,
   input logic clock_reset2x_clk,
   input logic clock_reset_reset_reset_n,
   input logic cc_snoop_clk_clk,
   // AVM kernel_mem0
   output logic kernel_mem0_enable,
   output logic kernel_mem0_read,
   output logic kernel_mem0_write,
   output logic [30:0] kernel_mem0_address,
   output logic [511:0] kernel_mem0_writedata,
   output logic [63:0] kernel_mem0_byteenable,
   input logic kernel_mem0_waitrequest,
   input logic [511:0] kernel_mem0_readdata,
   input logic kernel_mem0_readdatavalid,
   output logic [15:0] kernel_mem0_burstcount,
   input logic kernel_mem0_writeack,
   input logic kernel_cra_debugaccess,
   input logic kernel_cra_burstcount,
   // AVS kernel_cra
   input logic kernel_cra_enable,
   input logic kernel_cra_read,
   input logic kernel_cra_write,
   input logic [29:0] kernel_cra_address,
   input logic [63:0] kernel_cra_writedata,
   input logic [7:0] kernel_cra_byteenable,
   output logic kernel_cra_waitrequest,
   output logic [63:0] kernel_cra_readdata,
   output logic kernel_cra_readdatavalid,
   output logic kernel_irq_irq,
   input logic [30:0] cc_snoop_data,
   input logic cc_snoop_valid,
   output logic cc_snoop_ready,
   output logic [63:0] device_exception_bus
);
endmodule

