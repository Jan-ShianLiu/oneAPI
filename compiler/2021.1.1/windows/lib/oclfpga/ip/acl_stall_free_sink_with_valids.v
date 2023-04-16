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


// overall latency of this IP
`define IP_PIPELINE_LATENCY_PLUS1    5

// to support the 0-latency stall free entry, add one more valid bit
`define ZERO_LATENCY_OFFSET 1

module acl_stall_free_sink_with_valids
#(
    parameter integer DATA_WIDTH = 32,         
    parameter integer PIPELINE_DEPTH = 32,
    parameter integer SHARINGII = 1,
    parameter integer SCHEDULEII = 1,
    parameter enable_ecc = "FALSE"    // Enable error correction coding
)
(
    input  logic clock,
    input  logic resetn,
    input  logic [DATA_WIDTH-1:0] data_in,  
    output logic [DATA_WIDTH-1:0] data_out,
    input  logic input_accepted,
    output logic valid_out,
    input  logic stall_in,
    output logic stall_entry,
    output logic [PIPELINE_DEPTH-`IP_PIPELINE_LATENCY_PLUS1+`ZERO_LATENCY_OFFSET:0] valids,
    output logic [SHARINGII-1:0] IIphases,
    input logic inc_pipelined_thread,
    input logic dec_pipelined_thread,
    output logic  [1:0] ecc_err_status  // ecc status signals
);
    (* altera_attribute = "-name auto_shift_register_recognition OFF" *) reg [PIPELINE_DEPTH-`IP_PIPELINE_LATENCY_PLUS1:0] shift_reg;
    reg [DATA_WIDTH-1:0] reg_data_in;

    localparam FIFO_DEPTH_LOG2 = CLogB2(PIPELINE_DEPTH);
    localparam FIFO_DEPTH = 1 << FIFO_DEPTH_LOG2;

    reg [FIFO_DEPTH_LOG2:0] counter;
    reg [SHARINGII-1:0] IIshreg;
    wire output_accepted;

    wire staging_reg_stall;
    wire fifo_valid;
    wire [DATA_WIDTH-1:0] fifo_data;
    wire throttle_pipelined_iterations;  

    assign stall_entry = counter[FIFO_DEPTH_LOG2] | (!IIshreg[0]) | throttle_pipelined_iterations;
    assign output_accepted = fifo_valid & ~staging_reg_stall;

    assign valids = {shift_reg, input_accepted};
    assign IIphases = IIshreg;

    generate 
    if (SHARINGII == 1) begin
       assign IIshreg = 1;
    end else begin
        always @(posedge clock or negedge resetn)
        begin
           if (!resetn)
           begin
              IIshreg <= {{(SHARINGII - 1){1'b0}},1'b1};
           end 
           else
           begin
              IIshreg <= {IIshreg,IIshreg[SHARINGII-1]};
           end
        end
    end
    endgenerate

    reg[$clog2(SCHEDULEII):0] IIschedcount;
    reg[$clog2(SCHEDULEII):0] threads_count;
   
    wire [1:0] threads_count_update;
    wire [$clog2(SCHEDULEII):0] threads_count_update_extended;

    always @(posedge clock or negedge resetn)
    begin
       if (!resetn) begin
          IIschedcount <= 0;
          threads_count <= 0;
       end else begin
          if (IIshreg[0]) begin
             // do not increase the counter if a thread is exiting
             // increasing threads_count is already decreasing the window
             // increasing IIschedcount ends up accepting the next thread too early
             IIschedcount <= (input_accepted && dec_pipelined_thread) ? IIschedcount : (IIschedcount == (SCHEDULEII - 1) ? 0 : (IIschedcount + 1));
          end
          if (input_accepted) begin
             threads_count <= threads_count + threads_count_update_extended;
          end
       end
    end 

    // allow threads in a window of the II cycles
    // this prevents the next iteration from entering too early
    assign throttle_pipelined_iterations = (IIschedcount >= (threads_count > 0 ? threads_count : 1));
    
    assign threads_count_update = inc_pipelined_thread - dec_pipelined_thread;
    assign threads_count_update_extended = $signed(threads_count_update);

    wire [1:0] counter_update; 
    wire [FIFO_DEPTH_LOG2:0] counter_update_extended; 
    
    always @(posedge clock or negedge resetn)
    begin
       if (!resetn)
       begin
          shift_reg <= {(PIPELINE_DEPTH-`IP_PIPELINE_LATENCY_PLUS1-1){1'b0}};
          counter <= {(FIFO_DEPTH_LOG2+1){1'b0}};
          reg_data_in <= 'x;
       end
       else
       begin
          shift_reg <= { shift_reg[PIPELINE_DEPTH-(`IP_PIPELINE_LATENCY_PLUS1+1):0], input_accepted };
          counter <= counter + counter_update_extended;
          reg_data_in <= data_in;
       end
    end 

    assign counter_update = input_accepted - output_accepted;
    assign counter_update_extended = $signed(counter_update);

    acl_fifo #(
      .DATA_WIDTH(DATA_WIDTH),
      .DEPTH(FIFO_DEPTH),
      .enable_ecc(enable_ecc)
    )
    fifo (
      .clock(clock),
      .resetn(resetn),
      .data_in(reg_data_in),
      .data_out(fifo_data),
      .valid_in(shift_reg[PIPELINE_DEPTH-`IP_PIPELINE_LATENCY_PLUS1]),
      .valid_out(fifo_valid),
      .stall_in(staging_reg_stall),
      .ecc_err_status(ecc_err_status)
    );

    acl_staging_reg #(
      .WIDTH(DATA_WIDTH)
    ) staging_reg (
      .clk(clock), 
      .reset(~resetn), 
      .i_data(fifo_data), 
      .i_valid(fifo_valid), 
      .o_stall(staging_reg_stall), 
      .o_data(data_out), 
      .o_valid(valid_out), 
      .i_stall(stall_in)
    );

    //ceil of the log base 2
    function integer CLogB2;
       input [31:0] Depth;
       integer i;
       begin
           i = Depth;        
           for(CLogB2 = 0; i > 0; CLogB2 = CLogB2 + 1)
              i = i >> 1;
       end
    endfunction
endmodule

