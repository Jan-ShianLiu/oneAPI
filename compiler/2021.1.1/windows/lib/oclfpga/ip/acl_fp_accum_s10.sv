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
                                                
    
// This design implements a single-precision FP accumulator, with varying options in terms of the
// initiation interval it supports. For II=1, the FP accumulator mode of the DSP block is used.
// For larger II, the accumulator is implemented as an adder/subtractor (with a potential for use of the multiplier as well)
// and a feedback FIFO to mimic the pop+add+push implementation that would otherwise be required. The DSP block implementing the 
// addition is configured such that its latency is min(4,II).
// This implementation allows us to make a late design decision about picking the implementation of the accumulator, without 
// having to rerun earlier stages of the flow again (push-pop generation, scheduling, etc.).

// Assumes resetn has been synchronized by the parent.
module acl_fp_accum_s10(dataa, datab, pop_feedback_n, clear_accumulator, keep_going, predicate, clock, result, valid_in, valid_out, stall_in, stall_out, resetn, ecc_err_status);
parameter FEEDBACK_CAPACITY=1;
parameter USING_MULTIPLIER=0;
parameter IS_STALL_FREE=0;
parameter SUBTRACTOR=0;
parameter enable_ecc = "FALSE";    // Enable error correction coding

input [31:0] dataa;
input [31:0] datab;
input clock, keep_going, predicate;
/* Used when ii>1, which results in an implementation that resembles a push-pop implementation of an accumulator. 
  pop_feedback_n=0 indicates that the feedback FIFO should be popped when possible. 
  pop_feedback_n=1 will result in data accumulating in the feedback FIFO. Typically pop_feedback_n is fed by the 'forked' signal from the loop orchestration logic.
*/
input pop_feedback_n; 
/*
  clear_accumulator=1 indicates that the accumulation value (the running sum) should be reset to zero.
*/
input clear_accumulator;
input stall_in, valid_in, resetn;
output stall_out, valid_out;
output [31:0] result;
output [1:0] ecc_err_status;  // ecc status signals

wire [31:0] negation_mask = (SUBTRACTOR==1) ? 32'h80000000 : 32'd0;

localparam ACCUM_LATENCY = ((FEEDBACK_CAPACITY >= 2) && (FEEDBACK_CAPACITY <= 4)) ? FEEDBACK_CAPACITY : 4;

  localparam                    NUM_RESET_COPIES = 1;
  localparam                    RESET_PIPE_DEPTH = 3;
  logic [NUM_RESET_COPIES-1:0]  sclrn;
  logic                         resetn_synchronized;

  // This module is only used on S10, so reset is assumed to be synchronized by the parent
  acl_reset_handler
  #(
      .ASYNC_RESET            (0),
      .USE_SYNCHRONIZER       (0),
      .SYNCHRONIZE_ACLRN      (0),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES) 
  )
  acl_reset_handler_inst
  (
      .clk                    (clock),
      .i_resetn               (resetn),
      .o_aclrn                (),
      .o_resetn_synchronized  (resetn_synchronized),  
      .o_sclrn                (sclrn)
  );

generate
if (FEEDBACK_CAPACITY==1)
begin
    // This is the low area implementation of the multiply-accumulate block, using a single DSP block.
    // In this implementation, keep_going is not needed. This is because II=1 and we only have one accumulator value, which
    // gets cleared when accumulate is set to 0 the next time the loop is started.
    reg [3:0] valid_reg;
    wire [31:0] result_temp;
    wire stall_accum;
    wire enable = (~valid_out | ~stall_accum);
    always @(posedge clock)
    begin
      if (enable) begin
          valid_reg <= {valid_reg[3:0], valid_in};
      end
      if(~sclrn) begin
          valid_reg <= 4'd0;            
      end
    end    

    /*
      Clear the accumulator if clear_accumulator==1 or if we're not supposed to pop from the feedback (strictly speaking we don't need to check this for ii=1 but this signal exists in the compiler)
    */
    wire clear_mac_accumulator = (valid_in && (clear_accumulator || pop_feedback_n));

    // FP MAC wysiwyg
    // In this mode, the DSP block perfoms the operation resulta = (accumulator ? resulta : 32'd0) + ay*az.
    fourteennm_fp_mac mac_fp_wys (
        // inputs
        .accumulate(~clear_mac_accumulator), 
        .ax(),
        .ay((valid_in & (~predicate)) ? (dataa ^ negation_mask) : 32'd0),
        .az((valid_in & (~predicate)) ? datab : 32'd0),
		    .clk({2'b00, clock}),
        .ena({2'b00, enable}),
        .clr(2'b00),
        // outputs
		    .resulta(result_temp)
    );
    defparam mac_fp_wys.operation_mode = "SP_MULT_ACC";
    defparam mac_fp_wys.use_chainin = "false";
    defparam mac_fp_wys.adder_subtract = "false";
    defparam mac_fp_wys.clear_type = "none";
    defparam mac_fp_wys.ax_clock = "none";
    defparam mac_fp_wys.ay_clock = "0";
    defparam mac_fp_wys.az_clock = "0";
    defparam mac_fp_wys.accumulate_clock = "0";
    defparam mac_fp_wys.ax_chainin_pl_clock = "none";
    defparam mac_fp_wys.accum_pipeline_clock = "0";
    defparam mac_fp_wys.mult_pipeline_clock = "0";
    defparam mac_fp_wys.ax_chainin_2nd_pl_clock = "none";
    defparam mac_fp_wys.accum_2nd_pipeline_clock = "none";
    defparam mac_fp_wys.mult_2nd_pipeline_clock = "none";
    defparam mac_fp_wys.adder_input_clock = "0";
    defparam mac_fp_wys.accum_adder_clock = "0";
    defparam mac_fp_wys.output_clock = "0";     
    
    if (IS_STALL_FREE==1)
    begin
        // No need to add a FIFO, just exit.
        assign result = result_temp;
        assign valid_out = valid_reg[3];
        assign stall_out = valid_out & stall_in;
        assign stall_accum = stall_in;
    end
    else
    begin
        // Add a latency 0, capacity 3 FIFO using acl_staging_reg
        wire valid_sr1, valid_sr2;
        wire stall_sr1, stall_sr2;
        wire [31:0] result_temp1;
        wire [31:0] result_temp2;
        
        assign stall_out = stall_accum & valid_reg[3];
        
        acl_staging_reg stage_1(
           .clk(clock),
           .reset(~resetn_synchronized),
           .i_data(result_temp),
           .i_valid(valid_reg[3]),
           .o_stall(stall_accum),
           .o_data(result_temp1),
           .o_valid(valid_sr1),
           .i_stall(stall_sr1));
        defparam stage_1.ASYNC_RESET = 0;
        defparam stage_1.SYNCHRONIZE_RESET = 0;
           
        acl_staging_reg stage_2(
           .clk(clock),
           .reset(~resetn_synchronized),
           .i_data(result_temp1),
           .i_valid(valid_sr1),
           .o_stall(stall_sr1),
           .o_data(result_temp2),
           .o_valid(valid_sr2),
           .i_stall(stall_sr2));
        defparam stage_2.ASYNC_RESET = 0;
        defparam stage_2.SYNCHRONIZE_RESET = 0;

        acl_staging_reg stage_3(
           .clk(clock),
           .reset(~resetn_synchronized),
           .i_data(result_temp2),
           .i_valid(valid_sr2),
           .o_stall(stall_sr2),
           .o_data(result),
           .o_valid(valid_out),
           .i_stall(stall_in));
        defparam stage_3.ASYNC_RESET = 0;
        defparam stage_3.SYNCHRONIZE_RESET = 0;        
    end
    assign ecc_err_status = 2'h0;
end
else
begin
    // For feedback capacity > 1, we need more storage on the back-path, so we will use a FIFO.
    // Now, the accumulate input will no longer connect to the DSP block, but instead will
    // indicate if we should pop a value from the feedback fifo.
    // Now, because we can use the block in multiply-add or simply an add mode, we will use USING_MULTIPLIER
    // block to tell us if the multiplication is required. We will create a different circuit when
    // we need or do not need a multiplier.
    
    wire [31:0] result_r0;
    wire valid_feedback;
    wire [31:0] feedback_data;
    reg [ACCUM_LATENCY-1:0] valid_acc;
    reg [ACCUM_LATENCY-1:0] keep_going_reg;    
    wire stall_acc;
    wire acc_is_valid = valid_acc[ACCUM_LATENCY-1];
    wire not_last_iteration = keep_going_reg[ACCUM_LATENCY-1];
    wire enable = ~(acc_is_valid & stall_acc); 
    wire read_feedback = valid_in & !pop_feedback_n & ~stall_out;
    wire feedback_full;
    wire push_to_fifo;
    wire accumulator_output_consumed;
    logic [1:0] ecc_err_status_0;
        
    always @(posedge clock)
    begin
      if (enable) begin
          valid_acc <= {valid_acc[ACCUM_LATENCY-2:0], !pop_feedback_n ? (valid_feedback & valid_in) : valid_in};
          keep_going_reg <= {keep_going_reg[ACCUM_LATENCY-2:0], keep_going};
      end
      if(~sclrn) begin
         valid_acc <= {ACCUM_LATENCY{1'b0}};
         keep_going_reg <= {ACCUM_LATENCY{1'bx}};            
      end        
    end
        
    assign stall_out = (acc_is_valid & stall_acc) | !pop_feedback_n & ~valid_feedback;

    // FP MAC wysiwyg
    if ((USING_MULTIPLIER == 0) && (ACCUM_LATENCY < 4))
    begin
        // In this mode, the DSP block perfoms the operation ax+ay.
        fourteennm_fp_mac mac_fp_wys (
            // inputs
            .ax((valid_in & (~predicate)) ? dataa : 32'd0),
            .ay((clear_accumulator || pop_feedback_n)? 32'd0 : feedback_data), // ay is the running sum. Feed zeroes if we are supposed to clear the accumulator or if we're not popping from the feedback FIFO (ie. a new loop is being invoked)
            .az(),
            .clk({2'b00, clock}),
            .ena({2'b00, enable}),
            .clr(2'b00),
            // outputs
            .resulta(result_r0)
        );
        defparam mac_fp_wys.operation_mode = "SP_ADD"; 
        defparam mac_fp_wys.use_chainin = "false"; 
        defparam mac_fp_wys.adder_subtract = (SUBTRACTOR==0) ? "false" : "true"; 
        defparam mac_fp_wys.ax_clock = "0"; 
        defparam mac_fp_wys.ay_clock = "0"; 
        defparam mac_fp_wys.az_clock = "NONE"; 
        defparam mac_fp_wys.output_clock = "0"; 
        defparam mac_fp_wys.accumulate_clock = "NONE"; 
        defparam mac_fp_wys.ax_chainin_pl_clock = "NONE"; 
        defparam mac_fp_wys.accum_pipeline_clock = "NONE"; 
        defparam mac_fp_wys.mult_pipeline_clock = "NONE"; 
        defparam mac_fp_wys.adder_input_clock = (ACCUM_LATENCY >= 3) ? "0" : "NONE"; 
        defparam mac_fp_wys.accum_adder_clock = "NONE";
        defparam mac_fp_wys.ax_chainin_2nd_pl_clock = "none";
        defparam mac_fp_wys.accum_2nd_pipeline_clock = "none";
        defparam mac_fp_wys.mult_2nd_pipeline_clock = "none";
        defparam mac_fp_wys.clear_type = "none";        
    end
    else
    begin
        // In this mode, the DSP block perfoms the operation ax+ay*az.
        fourteennm_fp_mac mac_fp_wys (
            // inputs
            .ax((clear_accumulator || pop_feedback_n)? 32'd0 : feedback_data), // ax is the running sum. Feed zeroes if we are supposed to clear the accumulator or if we're not popping from the feedback FIFO (ie. a new loop is being invoked)
            .ay((valid_in & (~predicate)) ? (dataa ^ negation_mask) : 32'd0),
            .az((valid_in & (~predicate)) ? ((USING_MULTIPLIER == 0) ? 32'h3f800000 : datab) : 32'd0),
            .clk({2'b00, clock}),
            .ena({2'b00, enable}),
            .clr(2'b00),
            // outputs
            .resulta(result_r0)
        );
        defparam mac_fp_wys.operation_mode = "SP_MULT_ADD"; 
        defparam mac_fp_wys.use_chainin = "false"; 
        defparam mac_fp_wys.adder_subtract = "false"; 
        defparam mac_fp_wys.ax_clock = "0"; 
        defparam mac_fp_wys.ay_clock = "0"; 
        defparam mac_fp_wys.az_clock = "0"; 
        defparam mac_fp_wys.output_clock = "0"; 
        defparam mac_fp_wys.accumulate_clock = "NONE"; 
        defparam mac_fp_wys.ax_chainin_pl_clock = (ACCUM_LATENCY == 4) ? "0" : "NONE"; 
        defparam mac_fp_wys.accum_pipeline_clock = "NONE"; 
        defparam mac_fp_wys.mult_pipeline_clock = (ACCUM_LATENCY == 4) ? "0" : "NONE"; 
        defparam mac_fp_wys.adder_input_clock = (ACCUM_LATENCY >= 3) ? "0" : "NONE"; 
        defparam mac_fp_wys.accum_adder_clock = "NONE";
        defparam mac_fp_wys.ax_chainin_2nd_pl_clock = "none";
        defparam mac_fp_wys.accum_2nd_pipeline_clock = "none";
        defparam mac_fp_wys.mult_2nd_pipeline_clock = "none";
        defparam mac_fp_wys.clear_type = "none";                    
    end

    localparam TYPE = (FEEDBACK_CAPACITY < 5) ? "zl_reg" :
                        (FEEDBACK_CAPACITY < 7 ? "ll_reg" : "ram");
    
    // Feedback FIFO - latency should be 0 for FEEDBACK_CAPACITY <= 4.
    assign accumulator_output_consumed = acc_is_valid & ~stall_acc;
    assign push_to_fifo = accumulator_output_consumed & not_last_iteration;
    
    acl_data_fifo feedback_data_fifo(
      .clock(clock),
      .resetn(resetn_synchronized),
      .data_in(result_r0),
      .data_out(feedback_data),
      .valid_in(push_to_fifo),
      .valid_out(valid_feedback),
      .stall_in(~read_feedback),
      .stall_out(feedback_full),    
      .ecc_err_status(ecc_err_status_0));    
    defparam feedback_data_fifo.DATA_WIDTH = 32;
    defparam feedback_data_fifo.DEPTH = FEEDBACK_CAPACITY+1-IS_STALL_FREE;
    defparam feedback_data_fifo.ALLOW_FULL_WRITE = 0;
    defparam feedback_data_fifo.IMPL = TYPE;
    defparam feedback_data_fifo.ASYNC_RESET = 0;
    defparam feedback_data_fifo.SYNCHRONIZE_RESET = 0;
    defparam feedback_data_fifo.enable_ecc = enable_ecc;
    
    wire pipeline_stalled;
    
    // Stall accumulator if either path forward is blocked.
    assign stall_acc = feedback_full | pipeline_stalled;
    
    logic [1:0] ecc_err_status_1;
    // Capacity/pipeline balance the accumulator
    if (IS_STALL_FREE==1)
    begin
       if (ACCUM_LATENCY < 4)
       begin
       // When in an SFC, we don't need a true FIFO here, but just a shift register to ensure the latency through this module is 4. 
       // So set ALLOW_FULL_WRITE to 1 so that the implementation of acl_data_fifo is a clock-enabled shift-register. 
         acl_data_fifo pipeline_out(
           .clock(clock),
           .resetn(resetn_synchronized),
           .data_in(result_r0),
           .data_out(result),
           .valid_in(accumulator_output_consumed),
           .valid_out(valid_out),
           .stall_in(stall_in),
           .stall_out(pipeline_stalled),
           .ecc_err_status(ecc_err_status_1));    
         defparam pipeline_out.DATA_WIDTH = 32;
         defparam pipeline_out.DEPTH = 4-ACCUM_LATENCY;
         defparam pipeline_out.ALLOW_FULL_WRITE = 1;
         defparam pipeline_out.IMPL = "shift_reg";  
         defparam pipeline_out.ASYNC_RESET = 0;
         defparam pipeline_out.SYNCHRONIZE_RESET = 0;
         defparam pipeline_out.enable_ecc = enable_ecc;
       end
       else
       begin
         assign result = result_r0;
         assign valid_out = accumulator_output_consumed;
         assign pipeline_stalled = stall_in;
         assign ecc_err_status_1 = 2'h0;
       end
    end
    else
    begin
       acl_data_fifo pipeline_out(
         .clock(clock),
         .resetn(resetn_synchronized),
         .data_in(result_r0),
         .data_out(result),
         .valid_in(accumulator_output_consumed),
         .valid_out(valid_out),
         .stall_in(stall_in),
         .stall_out(pipeline_stalled),
         .ecc_err_status(ecc_err_status_1));
       defparam pipeline_out.DATA_WIDTH = 32;
       defparam pipeline_out.DEPTH = 3;
       defparam pipeline_out.ALLOW_FULL_WRITE = 0;
       defparam pipeline_out.IMPL = (ACCUM_LATENCY==4) ? "zl_reg" : "ll_reg";       
       defparam pipeline_out.ASYNC_RESET = 0;
       defparam pipeline_out.SYNCHRONIZE_RESET = 0;       
       defparam pipeline_out.enable_ecc = enable_ecc;     
    end
    assign ecc_err_status = ecc_err_status_0 | ecc_err_status_1;
end

endgenerate

endmodule
