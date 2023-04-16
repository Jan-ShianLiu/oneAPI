//// (c) 1992-2020 Intel Corporation.                            
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


`default_nettype none

module acl_embedded_workgroup_issuer_fifo #(
  parameter unsigned MAX_SIMULTANEOUS_WORKGROUPS = 2,    // >0
  parameter unsigned MAX_WORKGROUP_SIZE = 2147483648,    // >0

  parameter unsigned WG_SIZE_BITS = $clog2({1'b0, MAX_WORKGROUP_SIZE} + 1),
  parameter unsigned LLID_BITS = (MAX_WORKGROUP_SIZE > 1 ? $clog2(MAX_WORKGROUP_SIZE) : 1),
  parameter unsigned WG_ID_BITS = (MAX_SIMULTANEOUS_WORKGROUPS > 1 ? $clog2(MAX_SIMULTANEOUS_WORKGROUPS) : 1),
  parameter GLOBAL_ID_WIDTH = 32,
  parameter bit ASYNC_RESET = 1,              // how do the registers CONSUME reset: 1 means registers are reset asynchronously, 0 means registers are reset synchronously
  parameter bit SYNCHRONIZE_RESET = 0         // before consumption, do we SYNCHRONIZE the reset: 1 means use a synchronizer (assume reset arrived asynchronously), 0 means passthrough (assume reset was already synchronized)
)
(
  input wire clock, 
  input wire resetn, 

  // Handshake for item entry into function.
  input wire valid_in, 
  output logic stall_out, 
  
  // Handshake with entry basic block
  output logic valid_entry, 
  input wire stall_entry,

  // Observe threads exiting the function .
  // This is asserted when items are ready to be retired from the workgroup.
  input wire valid_exit, 
  // This is asserted when downstream is not ready to retire an item from
  // the workgroup.
  input wire stall_exit, 
  
  // Need workgroup_size to know when one workgroup ends
  // and another begins.
  input wire [WG_SIZE_BITS - 1:0] workgroup_size, 

  // Linearized local id. In range of [0, workgroup_size - 1].
  output logic [LLID_BITS - 1:0] linear_local_id_out,

  // Hardware work-group id. In range of [0, MAX_SIMULTANEOUS_WORKGROUPS - 1].
  output logic [WG_ID_BITS - 1:0] hw_wg_id_out,

  // The hardware work-group id of the work-group that is exiting.
  input wire [WG_ID_BITS - 1:0] done_hw_wg_id_in,

  // Pass though global_id, local_id and group_id.
  input wire [GLOBAL_ID_WIDTH-1:0] global_id_in[2:0],
  input wire [31:0] local_id_in[2:0],
  input wire [31:0] group_id_in[2:0],
  output logic [GLOBAL_ID_WIDTH-1:0] global_id_out[2:0],
  output logic [31:0] local_id_out[2:0],
  output logic [31:0] group_id_out[2:0]
);
  
  //reset
  logic aclrn, sclrn, resetn_synchronized;
  acl_reset_handler
  #(
    .ASYNC_RESET            (ASYNC_RESET),
    .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
    .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
    .PULSE_EXTENSION        (0),
    .PIPE_DEPTH             (1),
    .NUM_COPIES             (1)
  )
  acl_reset_handler_inst
  (
    .clk                    (clock),
    .i_resetn               (resetn),
    .o_aclrn                (aclrn),
    .o_resetn_synchronized  (resetn_synchronized),
    .o_sclrn                (sclrn)
  );
  
  // Entry: 1 cycle latency
  // Exit: 1 cycle latency
  acl_work_group_limiter #(
    .WG_LIMIT(MAX_SIMULTANEOUS_WORKGROUPS),
    .KERNEL_WG_LIMIT(MAX_SIMULTANEOUS_WORKGROUPS),
    .MAX_WG_SIZE(MAX_WORKGROUP_SIZE),
    .WG_FIFO_ORDER(1),
    .IMPL("kernel"),   // this parameter is very important to get the right implementation
    .ASYNC_RESET(ASYNC_RESET),
    .SYNCHRONIZE_RESET(0)
  )
  limiter(
    .clock(clock),
    .resetn(resetn_synchronized),
    .wg_size(workgroup_size),

    .entry_valid_in(valid_in),
    .entry_k_wgid(),
    .entry_stall_out(stall_out),
    .entry_valid_out(valid_entry),
    .entry_l_wgid(hw_wg_id_out),
    .entry_stall_in(stall_entry),

    .exit_valid_in(valid_exit & ~stall_exit),
    .exit_stall_out(),
    .exit_valid_out(),
    .exit_stall_in(1'b0)
  );

  // Pass through ids (global, local, group).
  // Match the latency of the work-group limiter, which is one cycle.
  always @(posedge clock)
    if( ~stall_entry ) 
    begin
      global_id_out <= global_id_in;
      local_id_out <= local_id_in;
      group_id_out <= group_id_in;
    end

  // local id 3 generator
  always @(posedge clock or negedge aclrn) begin
    if( ~aclrn ) begin
      linear_local_id_out <= '0;
    end
    else begin
      if( valid_entry & ~stall_entry ) begin
        if( linear_local_id_out == workgroup_size - 'd1 )
          linear_local_id_out <= '0;
        else
          linear_local_id_out <= linear_local_id_out + 'd1;
      end
      if( ~sclrn ) begin
        linear_local_id_out <= '0;
      end
    end
  end

endmodule

`default_nettype wire

// vim:set filetype=verilog_systemverilog:
