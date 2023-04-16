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



//===----------------------------------------------------------------------===//
//
// Shift-Register implementation of a barrier
//
//===----------------------------------------------------------------------===//

// This is the same barrier as before, but it produces a stall out rather than
// setting it to zero.

`default_nettype none

module acl_barrier_simple_with_stallout (
        // The first parameters are exactly the same as the normal barrier just
        // for plugin compatibility
	clock,
	resetn,
	valid_entry,
	data_entry,
	stall_entry,
	valid_exit,
	data_exit,
	stall_exit,
  workgroup_size
);

parameter DATA_WIDTH=1024;
parameter DEPTH=256;        // Assumed power of 2
parameter WORKGROUP_SIZE_BITS = 10;
parameter ASYNC_RESET=1;        // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
parameter SYNCHRONIZE_RESET=0;  // set to '1' to pass the incoming reset signal through a synchronizer before use


localparam OCOUNT_BITS=WORKGROUP_SIZE_BITS+1;

input  wire clock;
input  wire resetn;
input  wire valid_entry;
input  wire [DATA_WIDTH-1:0] data_entry;
output wire stall_entry;
output wire valid_exit;
output wire [DATA_WIDTH-1:0] data_exit;
input wire  stall_exit;
input wire [WORKGROUP_SIZE_BITS-1:0] workgroup_size;
 
   reg [WORKGROUP_SIZE_BITS-1:0] reg_workgroup_size;
   reg [WORKGROUP_SIZE_BITS-1:0] reg_workgroup_size_minus_1;

   reg [WORKGROUP_SIZE_BITS-1:0] i_count;
   reg [OCOUNT_BITS:0] o_count;
 
    (* altera_attribute = "-name auto_shift_register_recognition OFF" *) reg [1:0] reg_valid_entry;

   localparam                    NUM_RESET_COPIES = 1;
   localparam                    RESET_PIPE_DEPTH = 3;
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
    
   assign valid_exit = o_count[OCOUNT_BITS];
   assign stall_entry = (o_count[OCOUNT_BITS:OCOUNT_BITS-1] == 2'b10) & reg_valid_entry[1];

   wire output_accepted;
   assign output_accepted = valid_exit & !stall_exit;
   
   wire incr_valids;
   assign incr_valids = reg_valid_entry[1] & !stall_entry & (i_count == (reg_workgroup_size_minus_1));

   always @(posedge clock or negedge aclrn)
   begin
     if (~aclrn)
     begin
       i_count <= {WORKGROUP_SIZE_BITS{1'b0}};
       o_count <= {OCOUNT_BITS{1'b0}};
       reg_workgroup_size <= 'x;
       reg_workgroup_size_minus_1 <= 'x;
       reg_valid_entry <= 2'b0;
     end
     else 
     begin
       if (incr_valids) i_count <= {WORKGROUP_SIZE_BITS{1'b0}}; 
       else if (reg_valid_entry[1] & !stall_entry) i_count <= i_count+1;

       o_count <= o_count - (incr_valids ? reg_workgroup_size : 0) + output_accepted;       

       reg_workgroup_size <= workgroup_size; 
       reg_workgroup_size_minus_1 <= reg_workgroup_size-1;
       
       if (~stall_entry)
         reg_valid_entry <= {reg_valid_entry[0], valid_entry};
         
       if (~sclrn[0]) begin
         i_count <= {WORKGROUP_SIZE_BITS{1'b0}};
         o_count <= {OCOUNT_BITS{1'b0}};
         reg_valid_entry <= 2'b0;
       end
     end
   end

endmodule

`default_nettype wire