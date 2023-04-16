// // (c) 1992-2020 Intel Corporation.                            
// Intel, the Intel logo, Intel, MegaCore, NIOS II, Quartus and TalkBack words    
// and logos are trademarks of Intel Corporation or its subsidiaries in the U.S.  
// and/or other countries. Other marks and brands may be claimed as the property  
// of others. See Trademarks on intel.com for full list of Intel trademarks or    
// the Trademarks & Brands Names Database (if Intel) or See www.Intel.com/legal (if Altera) 
// Your use of Intel Corporation's design tools, logic functions and other        
// software and tools, and its AMPP partner logic functions, and any output       
// files any of the foregoing (including device programming or simulation         
// files), and any associated documentation or information are expressly subject  
// to the terms and conditions of the Altera Program License Subscription         
// Agreement, Intel MegaCore Function License Agreement, or other applicable      
// license agreement, including, without limitation, that your use is for the     
// sole purpose of programming logic devices manufactured by Intel and sold by    
// Intel or its authorized distributors.  Please refer to the applicable          
// agreement for further details.                                                 


// Work-group limiter. This module has two interface points: the entry point
// and the exit point. The purpose of the module is to ensure that there are
// no more than WG_LIMIT work-groups in the pipeline between the entry and
// exit points. The limiter also remaps the kernel-level work-group id into
// a local work-group id; this is needed because in general the kernel-level
// work-group id space is larger than the local work-group id space. It is 
// assumed that any work-item that passes through the entry point will pass 
// through the exit point at some point.
//
// The ordering of the work-groups affects the implementation. In particular,
// if the work-group order is the same through the entry point and the exit
// point, the implementation is simple. This is referred to as work-group FIFO
// (first-in-first-out) order. It remains a TODO to support work-group
// non-FIFO order (through the exit point).
//
// The work-group order does NOT matter if WG_LIMIT >= KERNEL_WG_LIMIT.
// In this configuration, the real limiter is at the kernel-level and this
// work-group limiter does not do anything useful. It does match the latency
// specifiction though so that the latency and capacity of the core is the
// same regardless of the configuration.
//
// Latency/capacity:
//  Through entry: 1 cycle
//  Through exit: 1 cycle
`default_nettype none

module acl_work_group_limiter_dspba(
  clock,
  resetn,

  wg_size,

  // Limiter entry
  entry_valid_in,
  entry_k_wgid,
  entry_stall_out,
  entry_valid_out,
  entry_l_wgid,
  entry_stall_in,

  // Limiter exit
  exit_valid_in,
  exit_stall_out,
  exit_valid_out,
  exit_stall_in

);
  parameter WG_LIMIT = 1;
  parameter KERNEL_WG_LIMIT = 1;
  parameter MAX_WG_SIZE = 1;
  parameter WG_FIFO_ORDER = 1;
  parameter bit ASYNC_RESET = 1;          // how do the registers CONSUME reset: 1 means registers are reset asynchronously, 0 means registers are reset synchronously
  parameter bit SYNCHRONIZE_RESET = 0;    // before consumption, do we SYNCHRONIZE the reset: 1 means use a synchronizer (assume reset arrived asynchronously), 0 means passthrough (assume reset was already synchronized)
  
  function integer my_max;
    input integer a;
    input integer b;
    begin
      my_max = (a > b) ? a : b;
    end
  endfunction  
  
  localparam MAX_WG_SIZE_WIDTH = $clog2({1'b0, MAX_WG_SIZE} + 1);  
  localparam KWG_BIT_LIMIT = my_max($clog2(KERNEL_WG_LIMIT),1);
  localparam WG_BIT_LIMIT = my_max($clog2(WG_LIMIT),1);

  input wire clock;
  input wire resetn;
  input wire [31:0] wg_size;
  
  // Limiter entry
  input wire entry_valid_in;
  input wire [31:0] entry_k_wgid; // not used if WG_FIFO_ORDER==1
  output logic entry_stall_out;
  output logic entry_valid_out;
  output logic [31:0] entry_l_wgid;
  input wire entry_stall_in;

  // Limiter exit
  input wire exit_valid_in;
  output logic exit_stall_out;
  output logic exit_valid_out;
  input wire exit_stall_in;
  
  logic [WG_BIT_LIMIT-1:0] wgid;
  logic entry_reg_valid_in;
  logic entry_reg_stall_in;  
  acl_work_group_limiter limiter(
    .clock(clock),
    .resetn(resetn),
    .wg_size(wg_size[MAX_WG_SIZE_WIDTH-1:0]),

    // Limiter entry
    .entry_valid_in(entry_valid_in),
    .entry_k_wgid(entry_k_wgid[KWG_BIT_LIMIT-1:0]),
    .entry_stall_out(entry_stall_out),
    .entry_valid_out(entry_valid_out),
    .entry_l_wgid(wgid),
    .entry_stall_in(entry_stall_in),

    // Limiter exit
    .exit_valid_in(exit_valid_in),
    .exit_stall_out(exit_stall_out),
    .exit_valid_out(exit_valid_out),
    .exit_stall_in(exit_stall_in));
  defparam limiter.WG_LIMIT = WG_LIMIT;
  defparam limiter.KERNEL_WG_LIMIT = KERNEL_WG_LIMIT;
  defparam limiter.MAX_WG_SIZE = MAX_WG_SIZE;
  defparam limiter.WG_FIFO_ORDER = WG_FIFO_ORDER;
  defparam limiter.IMPL = "local";
  defparam limiter.ASYNC_RESET = ASYNC_RESET;
  defparam limiter.SYNCHRONIZE_RESET = SYNCHRONIZE_RESET;
  
  assign entry_l_wgid[31:WG_BIT_LIMIT] = '0;
  assign entry_l_wgid[WG_BIT_LIMIT-1:0] = wgid;

endmodule

`default_nettype wire
