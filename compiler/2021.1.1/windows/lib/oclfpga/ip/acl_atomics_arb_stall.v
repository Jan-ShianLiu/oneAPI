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

module acl_atomics_arb_stall
#(
    // Configuration
    parameter integer STALL_CYCLES = 6,
    parameter ASYNC_RESET=1,                              // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
    parameter SYNCHRONIZE_RESET=0                         // set to '1' to pass the incoming reset signal through a synchronizer before use
)
(
    input wire clock,
    input wire resetn,
    acl_arb_intf in_intf,
    acl_arb_intf out_intf
);

/******************
* Local Variables *
******************/

reg shift_register [0:STALL_CYCLES-1];
wire atomic;
wire stall;
integer t;

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


assign out_intf.req.request = ( in_intf.req.request & ~stall ); // mask request
assign out_intf.req.read = ( in_intf.req.read & ~stall ); // mask read
assign out_intf.req.write = ( in_intf.req.write & ~stall ); // mask write
assign out_intf.req.enable = in_intf.req.enable;
assign out_intf.req.writedata = in_intf.req.writedata;
assign out_intf.req.burstcount = in_intf.req.burstcount;
assign out_intf.req.address = in_intf.req.address;
assign out_intf.req.byteenable = in_intf.req.byteenable;
assign out_intf.req.id = in_intf.req.id;
assign in_intf.stall = ( out_intf.stall | stall );

/*****************
* Detect Atomic *
******************/

assign atomic = ( out_intf.req.request == 1'b1 &&
                  out_intf.req.read == 1'b1 &&
                  out_intf.req.writedata[0:0] == 1'b1 ) ? 1'b1 : 1'b0;

always@(posedge clock or negedge aclrn)
begin
  if (~aclrn) begin
    shift_register[0] <= 1'b0;
  end
  else begin
    shift_register[0] <= atomic;
    if (~sclrn[0]) shift_register[0] <= 1'b0;
  end
end

/*****************
* Shift Register *
******************/

always@(posedge clock or negedge aclrn)
begin
  for (t=1; t< STALL_CYCLES; t=t+1)
  begin 
   if (~aclrn) begin
     shift_register[t] <= 1'b0;
   end
   else begin
     shift_register[t] <= shift_register[t-1];
     if (~sclrn[0]) shift_register[t] <= 1'b0;
   end
  end
end

/***************
* Detect Stall *
***************/

assign stall = ( shift_register[STALL_CYCLES-1] == 1'b1 );

endmodule

`default_nettype wire