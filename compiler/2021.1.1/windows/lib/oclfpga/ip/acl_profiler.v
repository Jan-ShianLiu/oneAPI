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


//
// This module records profiling information. It is connected to the desired
// pipeline ports that are needed to be profiled. 
// cntl_in signal determines when a profiling register is updated.
// incr_in signal determines the increment value for each counter.
// NUM_COUNTERS of profiling registers are instantiated. When the profile_shift
// signal is high, profiling registers are shifted out DAISY_WIDTH bits (64-bits) at a time.
//
// TEST_MODE causes the counters to scan out fixed IDs corresponding to their location in the
// scan chain.  Used to test profile data readback through the CRA.
//
`default_nettype none
module acl_profiler
(
  clock,
  resetn,
  enable,

  profile_shift,
  incr_cntl,
  incr_val,
  load_buffer,
  daisy_out,
  daisy_in
);

parameter COUNTER_WIDTH=64;
parameter INCREMENT_WIDTH=32;
parameter NUM_COUNTERS=4;
parameter TOTAL_INCREMENT_WIDTH=INCREMENT_WIDTH * NUM_COUNTERS;
parameter DAISY_WIDTH=64;
parameter TEST_MODE=0;
parameter ENABLE_TESSELLATION=0;
parameter TESSELLATION_SECTION_SIZE=16; // Must be <= INCREMENT_WIDTH
parameter ASYNC_RESET=1;        // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
parameter SYNCHRONIZE_RESET=0;  // set to '1' to pass the incoming reset signal through a synchronizer before use
// Note that this level of hierarchy does NOT include a reset handler, the input reset signal and parameters are simply passed through to sub-modules.
// The acl_profile_counter modules are likely to be widely distributed throughout the chip, so this is a case where hierarchy is NOT a good proxy for locality and we are better
// off using the global reset signal to distribute reset to each counter, and synchronize locally.

// Set ENABLE_DOUBLE_BUFFER to '1' to add a buffer for each counter. When
// enabled, the 'load_buffer' signal loads the contents of the counter into the
// buffer, enabling us to take a 'snapshot' of the counter values.  This also
// resets the counters to zero.  This feature is used for temporal profiling,
// where we sample the profile counters several times over a full run of the
// design, rather than simply accumulating data and reading it all back when the
// kernel finishes.
parameter ENABLE_DOUBLE_BUFFER=0;

// These parameters affect the mode of the profiler counters, stopping them from adding as usual 
// CURRENT_VALUE_COUNTERS mode will tell the profiler counter to stop adding values, and instead just expose
//  the current increment value (no matter what the control value is)
parameter CURRENT_VALUE_COUNTERS=0;
// MAXIMUM_VALUE_COUNTERS mode will tell the profiler counter to stop adding values, and instead record the 
//  maximum incremement value this counter has seen (ignoring control)
parameter MAXIMUM_VALUE_COUNTERS=0;

input wire clock;
input wire resetn;
input wire enable;

input wire profile_shift;
input wire [NUM_COUNTERS-1:0] incr_cntl;
input wire [TOTAL_INCREMENT_WIDTH-1:0] incr_val;
input wire load_buffer; // Trigger buffering of the counter into 'buffer'.
output logic [DAISY_WIDTH-1:0] daisy_out;
input wire [DAISY_WIDTH-1:0] daisy_in;

// if there are NUM_COUNTER counters, there are NUM_COUNTER-1 connections between them
wire [NUM_COUNTERS-2:0][DAISY_WIDTH-1:0] shift_wire;
wire [31:0] data_out [0:NUM_COUNTERS-1];// for debugging, always 32-bit for ease of modelsim

generate
genvar n;
  for(n=0; n<NUM_COUNTERS; n++) begin : counter_n
    localparam CURRENT_VALUE_MODE_VAL = ((CURRENT_VALUE_COUNTERS & (1'b1 << n)) != 0);
    localparam MAXIMUM_VALUE_MODE_VAL = ((MAXIMUM_VALUE_COUNTERS & (1'b1 << n)) != 0);
    localparam ENABLE_BUFFER = ((ENABLE_DOUBLE_BUFFER == 1) && (CURRENT_VALUE_MODE_VAL == 0) && (MAXIMUM_VALUE_MODE_VAL == 0));
    if(NUM_COUNTERS == 1) begin 
      acl_profile_counter #(
         .COUNTER_WIDTH( COUNTER_WIDTH ),
         .INCREMENT_WIDTH( INCREMENT_WIDTH ),
         .DAISY_WIDTH( DAISY_WIDTH ),
         .TEST_MODE( TEST_MODE ),
         .TEST_VALUE( n+1 ),
         .CURRENT_VALUE_MODE( CURRENT_VALUE_MODE_VAL ),
         .MAXIMUM_VALUE_MODE( MAXIMUM_VALUE_MODE_VAL ),
         .ENABLE_TESSELLATION(ENABLE_TESSELLATION),
         .ENABLE_DOUBLE_BUFFER(ENABLE_BUFFER),
         .TESSELLATION_SECTION_SIZE(TESSELLATION_SECTION_SIZE),
         .ASYNC_RESET(ASYNC_RESET),
         .SYNCHRONIZE_RESET(SYNCHRONIZE_RESET)
      ) counter (
         .clock( clock ),
         .resetn( resetn ),
         .enable( enable ),
         .shift( profile_shift ),
         .incr_cntl( incr_cntl[n] ),
         .shift_in( daisy_in ),
         .incr_val( incr_val[ ((n+1)*INCREMENT_WIDTH-1) : (n*INCREMENT_WIDTH) ] ),
         .load_buffer( load_buffer ),
         .data_out( data_out[ n ] ),
         .shift_out( daisy_out )
      );
    end else if(n == 0) begin
      acl_profile_counter #(
         .COUNTER_WIDTH( COUNTER_WIDTH ),
         .INCREMENT_WIDTH( INCREMENT_WIDTH ),
         .DAISY_WIDTH( DAISY_WIDTH ),
         .TEST_MODE( TEST_MODE ),
         .TEST_VALUE( n+1 ),
         .CURRENT_VALUE_MODE( CURRENT_VALUE_MODE_VAL ),
         .MAXIMUM_VALUE_MODE( MAXIMUM_VALUE_MODE_VAL ),
         .ENABLE_TESSELLATION(ENABLE_TESSELLATION),
         .ENABLE_DOUBLE_BUFFER(ENABLE_BUFFER),
         .TESSELLATION_SECTION_SIZE(TESSELLATION_SECTION_SIZE),
         .ASYNC_RESET(ASYNC_RESET),
         .SYNCHRONIZE_RESET(SYNCHRONIZE_RESET)
      ) counter (
         .clock( clock ),
         .resetn( resetn ),
         .enable( enable ),
         .shift( profile_shift ),
         .incr_cntl( incr_cntl[n] ),
         .shift_in( shift_wire[n] ),
         .incr_val( incr_val[ ((n+1)*INCREMENT_WIDTH-1) : (n*INCREMENT_WIDTH) ] ),
         .load_buffer( load_buffer ),
         .data_out( data_out[ n ] ),
         .shift_out( daisy_out )
      );
    end else if(n == NUM_COUNTERS-1) begin
      acl_profile_counter #(
         .COUNTER_WIDTH( COUNTER_WIDTH ),
         .INCREMENT_WIDTH( INCREMENT_WIDTH ),
         .DAISY_WIDTH( DAISY_WIDTH ),
         .TEST_MODE( TEST_MODE ),
         .TEST_VALUE( n+1 ),
         .CURRENT_VALUE_MODE( CURRENT_VALUE_MODE_VAL ),
         .MAXIMUM_VALUE_MODE( MAXIMUM_VALUE_MODE_VAL ),
         .ENABLE_TESSELLATION(ENABLE_TESSELLATION),
         .ENABLE_DOUBLE_BUFFER(ENABLE_BUFFER),
         .TESSELLATION_SECTION_SIZE(TESSELLATION_SECTION_SIZE),
         .ASYNC_RESET(ASYNC_RESET),
         .SYNCHRONIZE_RESET(SYNCHRONIZE_RESET)
      ) counter (
         .clock( clock ),
         .resetn( resetn ),
         .enable( enable ),
         .shift( profile_shift ),
         .incr_cntl( incr_cntl[n] ),
         .shift_in( daisy_in ),
         .incr_val( incr_val[ ((n+1)*INCREMENT_WIDTH-1) : (n*INCREMENT_WIDTH) ] ),
         .load_buffer( load_buffer ),
         .data_out( data_out[ n ] ),
         .shift_out( shift_wire[n-1] )
      );
    end else begin
      acl_profile_counter #(
         .COUNTER_WIDTH( COUNTER_WIDTH ),
         .INCREMENT_WIDTH( INCREMENT_WIDTH ),
         .DAISY_WIDTH( DAISY_WIDTH ),
         .TEST_MODE( TEST_MODE ),
         .TEST_VALUE( n+1 ),
         .CURRENT_VALUE_MODE( CURRENT_VALUE_MODE_VAL ),
         .MAXIMUM_VALUE_MODE( MAXIMUM_VALUE_MODE_VAL ),
         .ENABLE_TESSELLATION(ENABLE_TESSELLATION),
         .ENABLE_DOUBLE_BUFFER(ENABLE_BUFFER),
         .TESSELLATION_SECTION_SIZE(TESSELLATION_SECTION_SIZE),
         .ASYNC_RESET(ASYNC_RESET),
         .SYNCHRONIZE_RESET(SYNCHRONIZE_RESET)
      ) counter (
         .clock( clock ),
         .resetn( resetn ),
         .enable( enable ),
         .shift( profile_shift ),
         .incr_cntl( incr_cntl[n] ),
         .shift_in( shift_wire[n] ),
         .incr_val( incr_val[ ((n+1)*INCREMENT_WIDTH-1) : (n*INCREMENT_WIDTH) ] ),
         .load_buffer( load_buffer ),
         .data_out( data_out[ n ] ),
         .shift_out( shift_wire[n-1] )
      );
    end
  end
endgenerate

endmodule

module acl_profile_counter
(
  clock,
  resetn,
  enable,

  shift,
  incr_cntl,
  shift_in,
  incr_val,
  load_buffer,
  data_out,
  shift_out
);

parameter COUNTER_WIDTH=64;
parameter INCREMENT_WIDTH=32;
parameter DAISY_WIDTH=64;
parameter TEST_MODE=0;
parameter TEST_VALUE=0;
parameter CURRENT_VALUE_MODE=0;
parameter MAXIMUM_VALUE_MODE=0;
parameter ASYNC_RESET=1;        // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
parameter SYNCHRONIZE_RESET=0;  // set to '1' to pass the incoming reset signal through a synchronizer before use
parameter ENABLE_TESSELLATION=0;
parameter ENABLE_DOUBLE_BUFFER=0;
parameter TESSELLATION_SECTION_SIZE=8; // Must be <= INCREMENT_WIDTH

input wire clock;
input wire resetn;
input wire enable;

input wire shift;
input wire incr_cntl;
input wire [DAISY_WIDTH-1:0] shift_in;
input wire [INCREMENT_WIDTH-1:0] incr_val;
input wire load_buffer;

output logic [31:0] data_out;// for debugging, always 32-bit for ease of modelsim
output logic [DAISY_WIDTH-1:0] shift_out;

logic tess_clear;
reg [COUNTER_WIDTH-1:0] counter, tess_counter;
logic [COUNTER_WIDTH-1:0] buffer; // Used to store the value of 'counter' when the 'load_buffer' signal is asserted.

   localparam                    NUM_RESET_COPIES = 1;
   localparam                    RESET_PIPE_DEPTH = 3;
   logic                         aclrn;
   logic [NUM_RESET_COPIES-1:0]  sclrn;
   logic                         resetn_synchronized;
   acl_reset_handler #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (1), // Always synchronize the reset since resetn_synchronized is used to drive logic.
      .SYNCHRONIZE_ACLRN      (1), // Always synchronize the reset since resetn_synchronized is used to drive logic.
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES)
   ) acl_reset_handler_inst (
      .clk                    (clock),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn),
      .o_sclrn                (sclrn),
      .o_resetn_synchronized  (resetn_synchronized)
   );

always@(posedge clock)
begin
  if (ENABLE_DOUBLE_BUFFER==1) begin
    if (ENABLE_TESSELLATION) begin
      if (load_buffer | !resetn_synchronized) begin
        tess_clear <= '1;
      end else begin
        tess_clear <= '0;
      end
    end
  end else begin
    if (ENABLE_TESSELLATION) begin
      if (shift | !resetn_synchronized) begin
        tess_clear <= '1;
      end else begin
        tess_clear <= '0;
      end
    end
  end
end

generate
  if (ENABLE_TESSELLATION && CURRENT_VALUE_MODE == 0 && MAXIMUM_VALUE_MODE == 0) begin
    /*
      This is a pipelined (AKA tessellated) accumulator so it has latency > 1, meaning the result is not ready immediately. But it can be incremented on every cycle.
      The latency is (ACCUMULATOR_WIDTH/SECTION_SIZE)+1 cycles.
    */
    acl_multistage_accumulator #(
      .ACCUMULATOR_WIDTH (COUNTER_WIDTH),
      .INCREMENT_WIDTH (INCREMENT_WIDTH),
      .SECTION_SIZE (TESSELLATION_SECTION_SIZE),
      .ASYNC_RESET (ASYNC_RESET),
      .SYNCHRONIZE_RESET (0)  // If reset signal should be synchronized it will happen in acl_profile_counter's reset handler (this module)
    )
    tessellated_counter
    (
      .clock        (clock),
      .resetn       (!tess_clear),            // This resets everything but the counter
      .clear        (tess_clear),             // This resets the counter itself
      .increment    (incr_val),               // Increment value
      .go           (enable && incr_cntl),    // Trigger an increment
      .result       (tess_counter)            // Output
    );
  end else begin
    assign tess_counter = '0;
  end
endgenerate

generate
  if (ENABLE_DOUBLE_BUFFER==1) begin : GEN_ENABLE_DOUBLE_BUFFER
    always@(posedge clock or negedge aclrn)
    begin
      if( ~aclrn )
      begin
        if(TEST_MODE==0) begin
          counter <= { COUNTER_WIDTH{1'b0} };
          buffer <= { COUNTER_WIDTH{1'b0} };
        end else begin
          counter <= TEST_VALUE;
          buffer <= TEST_VALUE;
        end
      end else begin
        if (shift) begin// shift by DAISY_WIDTH bits
          buffer <= {buffer, shift_in};
        end else if (TEST_MODE == 0) begin // If TEST_MODE == 1, buffer is assigned TEST_VALUE on reset and never changes.
          if (ENABLE_TESSELLATION && enable) begin
            counter <= tess_counter;
          end else if(enable && incr_cntl) begin // Use a normal, non-tessellated counter
            counter <= counter + incr_val;
          end
        end

        if (load_buffer) begin
          buffer <= counter;
          if (ENABLE_TESSELLATION && enable) begin
            counter <= tess_counter;
          end else if(enable && incr_cntl) begin // Use a normal, non-tessellated counter
            counter <= incr_val;
          end else begin
            counter <= { COUNTER_WIDTH{1'b0} };
          end
        end

        if (~sclrn[0]) begin
          if(TEST_MODE==0) begin
            counter <= { COUNTER_WIDTH{1'b0} };
            buffer <= { COUNTER_WIDTH{1'b0} };
          end else begin
            counter <= TEST_VALUE;
            buffer <= TEST_VALUE;
          end
        end

      end
    end
  end else if (CURRENT_VALUE_MODE == 0 && MAXIMUM_VALUE_MODE == 0) begin : GEN_DISABLE_DOUBLE_BUFFER
    always@(posedge clock or negedge aclrn)
    begin
      if( ~aclrn )
      begin
        if(TEST_MODE==0)
          counter <= { COUNTER_WIDTH{1'b0} };
        else
          counter <= TEST_VALUE;
      end else begin
        if (shift) begin// shift by DAISY_WIDTH bits
          counter <= {counter, shift_in};
        end else if (TEST_MODE == 0) begin // If TEST_MODE == 1, counter is assigned TEST_VALUE on reset and never changes.
          if (ENABLE_TESSELLATION && enable) begin
            counter <= tess_counter;
          end else if(enable && incr_cntl) begin // Use a normal, non-tessellated counter
            counter <= counter + incr_val;
          end
        end

        if (~sclrn[0]) begin
          if(TEST_MODE==0)
            counter <= { COUNTER_WIDTH{1'b0} };
          else
            counter <= TEST_VALUE;
        end

      end
    end
    assign buffer = '0;
  end else if (CURRENT_VALUE_MODE == 1) begin
    always@(posedge clock or negedge aclrn)
    begin
      if( ~aclrn )
      begin
        counter <= { COUNTER_WIDTH{1'b0} };
      end else begin
        if (shift) begin// shift by DAISY_WIDTH bits
          counter <= {counter, shift_in};
        end else if (enable) begin
          // this counter is not summing - it just holds the most recent incr_val (no need for tesselation or double buffering)
          counter <= { COUNTER_WIDTH{1'b0} } | incr_val;
        end

        if (~sclrn[0]) begin
          counter <= { COUNTER_WIDTH{1'b0} };
        end

      end
    end
  end else if (MAXIMUM_VALUE_MODE == 1) begin
    always@(posedge clock or negedge aclrn)
    begin
      if( ~aclrn )
      begin
        counter <= { COUNTER_WIDTH{1'b0} };
      end else begin
        if (shift) begin// shift by DAISY_WIDTH bits
          counter <= {counter, shift_in};
        end else if (enable) begin
          if (incr_val > counter) begin
            // this counter is not summing - it holds the largest increment value seen so far (no need for tesselation or double buffering)
            counter <= { COUNTER_WIDTH{1'b0} } | incr_val;
          end
        end

        if (~sclrn[0]) begin
          counter <= { COUNTER_WIDTH{1'b0} };
        end

      end
    end
  end
endgenerate

generate
if (ENABLE_DOUBLE_BUFFER==1) begin
  assign data_out = buffer;
  assign shift_out = {buffer, shift_in} >> COUNTER_WIDTH;
end else begin
  assign data_out = counter;
  assign shift_out = {counter, shift_in} >> COUNTER_WIDTH;
end
endgenerate

endmodule
`default_nettype wire
