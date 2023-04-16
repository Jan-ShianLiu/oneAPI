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


// This block is used to determine when all iterations of a loop have completed and it is safe to emit a valid.
// The block is used in cases where the loop iterations may complete out of order.

`default_nettype none

module hld_loop_exit_reorder_counter #(
    parameter  int COUNTER_WIDTH,                           // width of the counters, in bits, to track the number of valids 'stored' in this block
    parameter  int NUM_TOKENS,                              // number of tokens to support
    parameter  bit ASYNC_RESET,
    parameter  bit SYNCHRONIZE_RESET,
    parameter  bit PASSTHROUGH = 0,                        // When 1, the IP is just a pass through...
    
    //derived parameters
    parameter int TOKEN_WIDTH      = $clog2(NUM_TOKENS) > 0 ? $clog2(NUM_TOKENS) : 1   // width of the i_token port, always at least 1 bit wide, if NUM_TOKENS==1, just tie this one-bit port off to '0
) (
    input  wire                     clk,
    input  wire                     i_resetn,

    input  wire                     i_start_inner_loop,     // Asserted for each new invocation of the inner loop, increment inner_loop_invocation_count[token] when i_valid & !o_stall & i_start_inner_loop
    input  wire                     i_exit_inner_loop,      // Asserted when the last iteration of the inner loop has started for an invocation of the inner loop, decrement inner_loop_invocation_count[token] when i_valid & !o_stall & i_exit_inner_loop
    input  wire                     i_exit_outer_loop,      // asserted when the last iteration of the outer loop has started, set outer_loop_exit_cond[token] when i_valid & !o_stall & i_exit_outer_loop
    input  wire  [TOKEN_WIDTH-1:0]  i_outer_loop_token,     // We can track multiple outer loop invocations, this token determines which counter/flag to modify
    input  wire                     i_valid,
    output logic                    o_stall,
    
    output logic                    o_exit_outer_loop,      // will assert when the last iteration of the outer loop has started (flag[token] is true) and the last iteration of all invocations of the inner loop have started (counter[token_pipelined]==0), assertion causes flag[token] to be cleared
    output logic                    o_valid,
    input  wire                     i_stall
);
    ///////////////////////////////////////
    // Parameter checking
    //
    // Generate an error if any illegal parameter settings or combinations are used
    ///////////////////////////////////////
    initial /* synthesis enable_verilog_initial_construct */
    begin
        if (TOKEN_WIDTH != $clog2(NUM_TOKENS) > 0 ? $clog2(NUM_TOKENS) : 1)
            $fatal(1, "Illegal parameteriazation, TOKEN_WIDTH must be not be specified when instantiating this module, it must be left at its default value");
    end

generate
  if (PASSTHROUGH == 1) begin
    // The inner loop has II 1, and there is no need for this IP.  Unfortunately,
    // It has already been used.  Replace the IP with a passthrough.
    assign o_exit_outer_loop = i_exit_outer_loop;
    assign o_valid = i_valid;
    assign o_stall = i_stall;
  end
  else begin // PASSTHROUGH == 0
    /////////////
    //  Reset 
    logic aclrn, sclrn, resetn_synchronized;
    acl_reset_handler #(
        .ASYNC_RESET            (ASYNC_RESET),
        .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
        .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
        .PIPE_DEPTH             (3),
        .NUM_COPIES             (1) 
    ) acl_reset_handler_inst (
        .clk                    (clk),
        .i_resetn               (i_resetn),
        .o_aclrn                (aclrn),
        .o_resetn_synchronized  (resetn_synchronized),
        .o_sclrn                (sclrn)
    );
    
    // internal state
    logic [3:3][NUM_TOKENS-1:0]                     outer_loop_exit_cond;
    logic [2:2][NUM_TOKENS-1:0][COUNTER_WIDTH-1:0]  inner_loop_invocation_count;
    logic [3:3][NUM_TOKENS-1:0]                     inner_loop_invocation_count_zero;
    logic [1:1][NUM_TOKENS-1:0][COUNTER_WIDTH-1:0]  counter_inc_val;
    logic [1:3]                                     exit_outer_loop;
    logic [1:3][TOKEN_WIDTH-1:0]                    token;
    logic [1:3]                                     valid;
    logic [1:3]                                     stall;
    
    always_ff @(posedge clk or negedge aclrn) begin
        if (!aclrn) begin
            outer_loop_exit_cond <= '0;
            inner_loop_invocation_count <= '0;
            inner_loop_invocation_count_zero <= '1;
            counter_inc_val <= '0;
            exit_outer_loop <= '0;
            token <= '0;
            valid <= '0;
        end else begin
        
            /////////////////////////
            // pipeline stage 1
            
            // determine whether to increment, decrement, or leave unmodified each inner_loop_invocation_count
            for (int token_num = 0; token_num < NUM_TOKENS; token_num++) begin : GEN_RANDOM_BLOCK_NAME_R84
                if ( (i_valid==1'b1) && (o_stall==1'b0) && i_outer_loop_token == token_num) begin
                    if (i_start_inner_loop & ~i_exit_inner_loop) begin
                        counter_inc_val[1][token_num] <= {{(COUNTER_WIDTH-1){1'b0}},{1'b1}};     // +1
                    end else if (~i_start_inner_loop & i_exit_inner_loop) begin
                        counter_inc_val[1][token_num] <= {(COUNTER_WIDTH){1'b1}};              // -1
                    end else begin
                        counter_inc_val[1][token_num] <= '0;                                   // +0
                    end
                end else begin
                    counter_inc_val[1][token_num] <= '0;                                       // +0
                end
            end
            
            if (i_valid & ~o_stall) begin           // new valid is entering the block
                token[1] <= i_outer_loop_token;
                exit_outer_loop[1] <= i_exit_outer_loop & i_start_inner_loop;       // only look at the exit_outer_loop signal when we are starting a new inner loop iteration, otherwise ignore it
                valid[1] <= '1;
            end else begin
                valid[1] <= valid[1] & stall[1];    // if we are stalled at the next pipeline stage, hold the valid here, and don't change the other registers in this pipeline stage
            end
            
            /////////////////////////
            // pipeline stage 2
            
            // update each counter
            for (int token_num = 0; token_num < NUM_TOKENS; token_num++) begin : GEN_RANDOM_BLOCK_NAME_R85
                inner_loop_invocation_count[2][token_num] <= inner_loop_invocation_count[2][token_num] + counter_inc_val[1][token_num];
            end

            if (valid[1] & ~stall[1]) begin
                token[2] <= token[1];
                exit_outer_loop[2] <= exit_outer_loop[1];
                valid[2] <= '1;
            end else begin
                valid[2] <= valid[2] & stall[2];    // if we are stalled from the next pipeline stage, hold the valid here, and don't change the other registers in his pipeline stage
            end

            /////////////////////////
            // pipeline stage 3
            
            if (valid[2] & ~stall[2]) begin

                for (int token_num = 0; token_num < NUM_TOKENS; token_num++) begin : GEN_RANDOM_BLOCK_NAME_R86
                    inner_loop_invocation_count_zero[3][token_num] <= inner_loop_invocation_count[2][token_num] == '0 ? '1 : '0;
                    if ( token[2]==token_num && exit_outer_loop[2]==1'b1) begin
                        outer_loop_exit_cond[3][token_num] <= '1;       // the exit cod flag is cleared below
                    end
                end
                token[3] <= token[2];
                valid[3] <= '1;
                
            end else begin
                valid[3] <= valid[3] & stall[3];    // if we are stalled from the next pipeline stage, hold the valid here, and don't change the other registers in his pipeline stage
            end
            
            for (int token_num = 0; token_num < NUM_TOKENS; token_num++) begin : GEN_RANDOM_BLOCK_NAME_R87
                if (o_valid && ~i_stall && token[3] == token_num && inner_loop_invocation_count_zero[3][token_num] && outer_loop_exit_cond[3][token_num]) begin
                    outer_loop_exit_cond[3][token_num] <= '0;
                end
            end

            if (!sclrn) begin
                outer_loop_exit_cond <= '0;
                inner_loop_invocation_count <= '0;
                exit_outer_loop <= '0;
                valid <= '0;
            end
        
        end

    end
    
    assign o_exit_outer_loop = outer_loop_exit_cond[3][ token[3] ] && inner_loop_invocation_count_zero[3][ token[3] ];
    assign o_valid = valid[3];
    assign stall[3] = i_stall;
    assign stall[2] = stall[3] & valid[3];
    assign stall[1] = stall[2] & valid[2];
    assign o_stall = stall[1] & valid[1];
  end
endgenerate

endmodule

`default_nettype wire
