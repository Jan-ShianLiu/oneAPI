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


//////////////////////////////////////////////////////////////////////////////
// Stall logic from the exit(s) of Stall-Free Kernel Set  to its entry
//
// This module implements a pipelined version (balanced tree) of NOR:
//
//      entry_ready = ~(|exit_almostfull)
//
// TREE_FANIN -- max number of children per tree node
// NUM_LEAF_DELAY_CYCLES -- exit_almostfull is delayed by this amount of
//                          cycles before entering the tree
//
//////////////////////////////////////////////////////////////////////////////

`default_nettype none

module acl_sfks_stall
#(
  parameter integer NUM_EXITS = 1,
  parameter integer PIPELINED = 1,
  parameter integer NUM_LEAF_DELAY_CYCLES = 0,
  parameter integer TREE_FANIN = 5,
  parameter integer GEN_ENTRY_READY = 1,  // otherwise, generate entry almost_full
  parameter bit ASYNC_RESET = 1,          // how do the registers CONSUME reset: 1 means registers are reset asynchronously, 0 means registers are reset synchronously
  parameter bit SYNCHRONIZE_RESET = 0     // before consumption, do we SYNCHRONIZE the reset: 1 means use a synchronizer (assume reset arrived asynchronously), 0 means passthrough (assume reset was already synchronized)
)
(
  input wire clock,
  input wire resetn,

  input wire [(NUM_EXITS-1):0] exit_almostfull,
  output logic entry_ready
);

localparam integer BASE_NUM_EXITS_PER_CHILD = (NUM_EXITS / TREE_FANIN);
localparam integer NUM_CHILDREN_WITH_EXTRA_EXIT = (NUM_EXITS - BASE_NUM_EXITS_PER_CHILD * TREE_FANIN);
localparam integer NUM_CHILDREN_WITH_BASE_EXIT = (TREE_FANIN - NUM_CHILDREN_WITH_EXTRA_EXIT);

    // Reset
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

  generate

  if ((PIPELINED == 0) || (NUM_EXITS <= TREE_FANIN)) begin

    // Delay the leaf signals if necessary
    logic [(NUM_EXITS-1):0] delayed_exit_almostfull;

    if (NUM_LEAF_DELAY_CYCLES > 0) begin

      logic [(NUM_EXITS-1):0] exit_almostfull_reg [(NUM_LEAF_DELAY_CYCLES-1):0];

      always @(posedge clock or negedge aclrn) begin
        if (!aclrn) begin
          for (integer i = 0; i < NUM_LEAF_DELAY_CYCLES; i = i + 1) begin : delay_reset
            exit_almostfull_reg[i] <= {(NUM_EXITS){1'b0}};
          end
        end
        else begin
          exit_almostfull_reg[0] <= exit_almostfull;
          for (integer i = 1; i < NUM_LEAF_DELAY_CYCLES; i = i + 1) begin : delay
            exit_almostfull_reg[i] <= exit_almostfull_reg[i-1];
          end
          if (!sclrn) begin
            for (integer i = 0; i < NUM_LEAF_DELAY_CYCLES; i = i + 1) begin : delay_reset_sclrn
              exit_almostfull_reg[i] <= {(NUM_EXITS){1'b0}};
            end
          end
        end
      end

      assign delayed_exit_almostfull = exit_almostfull_reg[NUM_LEAF_DELAY_CYCLES-1];

    end
    else begin

      assign delayed_exit_almostfull = exit_almostfull;

    end

    // base case: non-pipelined
    // NOTE: only used the delayed signal here
    if (GEN_ENTRY_READY) begin
      assign entry_ready = ~(|delayed_exit_almostfull);
    end
    else begin
      assign entry_ready = |delayed_exit_almostfull;
    end

  end
  else begin

    // recursive case: max the fan-in here, leaving holes toward leaves
    logic [(TREE_FANIN-1):0] child_ready;
    genvar i;
    for (i = 0; i < TREE_FANIN; i = i + 1) begin : children
      if (i < NUM_CHILDREN_WITH_BASE_EXIT) begin
        // child instance with the base # of exits
        acl_sfks_stall #(
          .NUM_EXITS(BASE_NUM_EXITS_PER_CHILD),
          .PIPELINED(1),
          .NUM_LEAF_DELAY_CYCLES(NUM_LEAF_DELAY_CYCLES),
          .TREE_FANIN(TREE_FANIN),
          .GEN_ENTRY_READY(GEN_ENTRY_READY),
          .ASYNC_RESET(ASYNC_RESET),
          .SYNCHRONIZE_RESET(0)
        ) base_inst (
          .clock(clock),
          .resetn(resetn_synchronized),
          .exit_almostfull(exit_almostfull[(BASE_NUM_EXITS_PER_CHILD*(i+1)-1):(BASE_NUM_EXITS_PER_CHILD*i)]),
          .entry_ready(child_ready[i]));
      end
      else begin
        // child instance with the base # of exits + 1
        acl_sfks_stall #(
          .NUM_EXITS(BASE_NUM_EXITS_PER_CHILD + 1),
          .PIPELINED(1),
          .NUM_LEAF_DELAY_CYCLES(NUM_LEAF_DELAY_CYCLES),
          .TREE_FANIN(TREE_FANIN),
          .GEN_ENTRY_READY(GEN_ENTRY_READY),
          .ASYNC_RESET(ASYNC_RESET),
          .SYNCHRONIZE_RESET(0)
        ) extra_inst (
          .clock(clock),
          .resetn(resetn_synchronized),
          .exit_almostfull(exit_almostfull[((BASE_NUM_EXITS_PER_CHILD+1)*i-NUM_CHILDREN_WITH_BASE_EXIT+BASE_NUM_EXITS_PER_CHILD):((BASE_NUM_EXITS_PER_CHILD+1)*i-NUM_CHILDREN_WITH_BASE_EXIT)]),
          .entry_ready(child_ready[i]));
      end
    end
    // pipeline register at child instance output
    reg [(TREE_FANIN-1):0] child_ready_reg;
    always @(posedge clock or negedge aclrn) begin
      if (!aclrn) begin
        child_ready_reg <= {(TREE_FANIN){1'b1}};
      end
      else begin
        child_ready_reg <= child_ready;
        if (!sclrn) begin
          child_ready_reg <= {(TREE_FANIN){1'b1}};
        end
      end
    end
    if (GEN_ENTRY_READY) begin
      // combinational AND of register outputs
      assign entry_ready = &child_ready_reg;
    end
    else begin
      // combinational OR of register outputs
      assign entry_ready = |child_ready_reg;
    end

  end

  endgenerate

endmodule

`default_nettype wire
