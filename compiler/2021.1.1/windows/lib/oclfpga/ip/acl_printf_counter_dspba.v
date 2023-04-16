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

module acl_printf_counter
(
    enable, clock, resetn,
    i_start,
    i_init,
    i_limit,
    i_increment,
    i_counter_reset,
    o_size,
    o_resultvalid,
    o_result,
    o_full,
    o_stall
);

/*************
* Parameters *
*************/

// default parameters if host does not specify them
parameter INIT=0;            // counter reset value
parameter LIMIT=65536;       // counter limit (this value should not be higher
                             // than the buffer size allocated by the host)
parameter ASYNC_RESET=1;        // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
parameter SYNCHRONIZE_RESET=0;  // set to '1' to pass the incoming reset signal through a synchronizer before use

/********
* Ports *
********/

// Standard global signals
input wire  clock;
input wire  resetn;
input wire  enable;
input wire  i_start;
input wire  [63:0] i_init;
input wire  [31:0] i_limit;
input wire  i_counter_reset; // reset signal from host

// operand and result
input wire  [31:0] i_increment; 

output logic [31:0] o_size; 
output logic o_resultvalid;
output logic [63:0] o_result;

// raised high for one cycle to interrupt the host when the buffer is full
output logic o_full;
// stall signal, kept high until reset is received
output logic o_stall;

// register to store next buffer address
reg [31:0] counter;

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


assign o_size = counter;

always@(posedge clock or negedge aclrn)
begin

   if ( ~aclrn ) begin
     o_resultvalid <= 1'b0;
     counter <= 32'b0;
     o_result <= 64'b0;
     o_full <=  1'b0;
     o_stall <= 1'b0;
   end else begin
   
     if( i_counter_reset ) begin
       // reset the signals
       // there is definitely a request waiting, we will not receive the request again
       // so, send the response
       o_resultvalid <= 1'b1;
       counter <= i_increment;
       o_result <= i_init;
       o_stall <= 1'b0;
     end
     else if( o_full ) begin
       // stall pipeline signal was raised in previous cycle
       // clear it so we dont stall again
       o_full <= 1'b0;
     end
     // stall until the internal full signal is cleared
     else if( ~o_stall ) begin
     
       if  (enable) begin
       
         // raise the full&stall signal only when there is a new request
         if( (counter != 32'b0) && (counter + i_increment > i_limit) ) begin
           o_full <= 1'b1;
           o_stall <= 1'b1;
           o_resultvalid <= 1'b0;
         end
         else begin
           o_result <= i_init + counter;
           o_resultvalid <= 1'b1;
           counter <= counter + i_increment;
         end
       end
       else begin
         o_resultvalid <= 1'b0;
       end
     
     end
     
     if (~sclrn[0] || i_start) begin
       o_resultvalid <= 1'b0;
       counter <= 32'b0;
       o_result <= 64'b0;
       o_full <=  1'b0;
       o_stall <= 1'b0;
     end
     
   end
end

endmodule

`default_nettype wire