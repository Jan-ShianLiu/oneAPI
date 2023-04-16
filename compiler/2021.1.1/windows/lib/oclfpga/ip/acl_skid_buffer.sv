// (c) 1992-2020 Intel Corporation.                            
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


`timescale 1ns / 1ns
`default_nettype none
/* This module implements a skid-buffer, which is effectively a 1-deep FIFO made of registers.
   The read-req signal is fed combinationally upstream.
   Incoming data, along with the empty signal, are registered.
*/
module acl_skid_buffer #(
   parameter DATA_WIDTH = 32,
   parameter ASYNC_RESET = 1,        // 1 = Registers are reset asynchronously. 0 = Registers are reset synchronously -- the reset signal is pipelined before consumption. In both cases, some registesr are not reset at all.
   parameter SYNCHRONIZE_RESET = 0   // 1 = resetn is synchronized before consumption. The consumption itself is either asynchronous or synchronous, as specified by ASYNC_RESET.
) (
   input wire                    clk,
   input wire                    resetn,

   output logic                  rreq_to_fifo,
   input wire                    empty_fr_fifo,
   input wire [DATA_WIDTH-1:0]   data_fr_fifo,

   input wire                    rreq,
   output logic                  rdempty,
   output logic [DATA_WIDTH-1:0] out_data
);

   localparam                    NUM_RESET_COPIES = 1;
   localparam                    RESET_PIPE_DEPTH = 3;
   logic                         aclrn;
   logic [NUM_RESET_COPIES-1:0]  sclrn;
   logic                         resetn_synchronized;

   acl_reset_handler
   #(
       .ASYNC_RESET            (ASYNC_RESET),
       .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
       .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
       .PIPE_DEPTH             (RESET_PIPE_DEPTH),
       .NUM_COPIES             (NUM_RESET_COPIES) 
   )
   acl_reset_handler_inst
   (
       .clk                    (clk),
       .i_resetn               (resetn),
       .o_aclrn                (aclrn),
       .o_resetn_synchronized  (resetn_synchronized),  
       .o_sclrn                (sclrn)
   );

   logic                   full1;
   logic [DATA_WIDTH-1:0]  data1;

   assign rdempty = ~full1;
   assign out_data  = data1;    
   
  // rreq_to_fifo will be a pass through of the rreq signal as it is not registered
  // Also assert read to the previous stage if not full (ie. there is no data stored in data1)
  assign rreq_to_fifo = (~full1) | rreq;

   // NON_RESETABLE FLOPS
   // Data path registers
   always @(posedge clk) begin
      if (rreq_to_fifo) begin
         data1 <= data_fr_fifo;
      end
   end

   // SYNC_RESET FLOPS
   // Ready/Valid control logic
   always @(posedge clk or negedge aclrn) begin
      if (!aclrn) begin
         full1 <= 1'b0;
      end
      else begin
         if (rreq_to_fifo) begin
            full1 <= ~empty_fr_fifo;  //full1 indicates if we have valid data
         end
         if (!sclrn[0]) begin
            full1 <= 1'b0;
         end
      end
   end
endmodule
`default_nettype wire