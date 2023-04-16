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


// Terminate local memory bank router ports that aren't connected to memory.
// Aside from tying return signals to zero, also provides a flag to indicate
// whether an unexpected access was attempted on this bank.  This flag should
// be checked someday by an exception monitor.

// There is currently no visibility into this module - intended for simulation
// until new performance monitor complete.

`default_nettype none

module acl_ic_local_mem_router_terminator #(    
    parameter integer DATA_W = 256,
    parameter ASYNC_RESET = 1,                              // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
    parameter SYNCHRONIZE_RESET = 0                         // set to '1' to pass the incoming reset signal through a synchronizer before use
)
(
    input wire clock,
    input wire resetn,

    // To each bank
    input wire b_arb_request,
    input wire b_arb_read,
    input wire b_arb_write,

    output logic b_arb_stall,
    output logic b_wrp_ack,
    output logic b_rrp_datavalid,
    output logic [DATA_W-1:0] b_rrp_data,

    output logic b_invalid_access
);

reg saw_unexpected_access;
reg first_unexpected_was_read;

   localparam                    NUM_RESET_COPIES = 1;
   localparam                    RESET_PIPE_DEPTH = 1;
   logic                         aclrn;
   logic [NUM_RESET_COPIES-1:0]  sclrn;
   logic                         resetn_synchronized;
   acl_reset_handler #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
      .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES)
   ) acl_reset_handler_inst (
      .clk                    (clock),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn),
      .o_sclrn                (sclrn),
      .o_resetn_synchronized  (resetn_synchronized)
   );


assign b_arb_stall = 1'b0;
assign b_wrp_ack = 1'b0;
assign b_rrp_datavalid = 1'b0;
assign b_rrp_data = '0;

assign b_invalid_access = saw_unexpected_access;

always@(posedge clock or negedge aclrn)
   begin
      if (~aclrn)
      begin
         saw_unexpected_access <= 1'b0;
         first_unexpected_was_read <= 1'b0;
      end
      else
      begin
      if (b_arb_request && ~saw_unexpected_access)
         begin
            saw_unexpected_access <= 1'b1;
            first_unexpected_was_read <= b_arb_read;

            if (~sclrn[0]) begin
               saw_unexpected_access <= 1'b0;
               first_unexpected_was_read <= 1'b0;
            end
            
            // Simulation-only failure message
            // synthesis_off
            $fatal(0,"Local memory router: accessed bank that isn't connected.  Hardware will hang.");
            // synthesis_on
         end
      end
   end

endmodule

`default_nettype wire