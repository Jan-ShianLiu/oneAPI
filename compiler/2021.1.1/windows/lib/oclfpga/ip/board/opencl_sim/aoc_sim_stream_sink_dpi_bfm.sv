

module aoc_sim_stream_sink_dpi_bfm
#(
    parameter STREAM_DATAWIDTH = 32,
    parameter FIRST_SYMBOL_IN_HIGH_ORDER_BITS = 0, // This parameter is ignored due to case:1409438337
    parameter STREAM_BITSPERSYMBOL = 0,
    parameter EMPTY_WIDTH = 0,
    parameter EMPTY_PORT_WIDTH = (EMPTY_WIDTH > 0) ? EMPTY_WIDTH : 1,
    parameter READY_LATENCY = 0,
    parameter COMPONENT_NAME = "dut",
    parameter INTERFACE_NAME = "a"
    )
    (
        input [STREAM_DATAWIDTH-1:0]    sink_data,
        output logic                    sink_ready,
        input                           sink_valid,
        input                           sink_startofpacket,
        input                           sink_endofpacket,
        input [EMPTY_PORT_WIDTH-1:0]    sink_empty,

        input               do_bind,
        input               enable,
        output              stream_active,

        input               clock,
        input               resetn,
        input               clock2x
    );

    import "DPI-C" context function chandle __ihc_aoc_get_stream_obj_ptr_for_component_interface(string component_name, string interface_name,  longint unsigned width,  longint unsigned direction);

    // check is the stream is ready (!full)
    import "DPI-C" context function int __ihc_aoc_stream_ready(chandle obj_ptr);

    // writes to the stream when 'valid' is 1; otherwise, report error
    import "DPI-C" context function void __ihc_aoc_stream_write(chandle obj_ptr, inout bit [STREAM_DATAWIDTH-1:0] data, inout bit sop, inout bit eop, inout bit [EMPTY_PORT_WIDTH-1:0] empty , input byte valid);
    import "DPI-C" context function void __ihc_hls_dbgs(string msg);

    string message;
    chandle stream_objptr;

    longint unsigned first_valid_time;
    longint unsigned last_valid_time;
    longint unsigned num_elements_in_stream;

    function bit stream_ready_sim();
      automatic int ready;
      ready = __ihc_aoc_stream_ready(stream_objptr);
      return (ready==1);
    endfunction

    function void stream_write_sim(input bit [STREAM_DATAWIDTH-1:0] in_data, input bit in_sop, input bit in_eop, input bit [EMPTY_PORT_WIDTH-1:0] in_empty, input byte valid);
      $sformat(message,
          "[%7t][msim][stream_sink_dpi_ctrl][%s][%s] writing to stream with data=%0d valid=%0d sop=%0d eop=%0d empty=%0d",
          $time, COMPONENT_NAME, INTERFACE_NAME, in_data, valid, in_sop, in_eop, in_empty);
      __ihc_hls_dbgs(message);
      __ihc_aoc_stream_write(stream_objptr, in_data, in_sop, in_eop, in_empty, valid);
    endfunction

    initial
    begin
      $sformat(message, "[%7t][msim][stream_sink_dpi_ctrl][%s][%s] start", $time, COMPONENT_NAME, INTERFACE_NAME);
      __ihc_hls_dbgs(message);
      $sformat(message, "[%7t][msim][stream_sink_dpi_ctrl] enable=%0d", $time, enable);
      __ihc_hls_dbgs(message);
      $sformat(message, "[%7t][msim][stream_sink_dpi_ctrl] clock=%0d", $time, clock);
      __ihc_hls_dbgs(message);
    end

    //////////////////////////////////////////////////////////////////////////
    // a shift register that saves the sink_ready signals in the past
    // READY_LATENCY cycles
    //////////////////////////////////////////////////////////////////////////

    reg real_sink_ready;

    generate

      if (READY_LATENCY == 0) begin

        always @(*) begin
          real_sink_ready = sink_ready;
        end

      end else begin

        // shift left
        reg [READY_LATENCY-1:0] ready_history;

        always @(*) begin
          real_sink_ready = ready_history[READY_LATENCY-1];
        end

        always @(posedge clock or negedge resetn) begin
          if (!resetn) begin
            ready_history <= {READY_LATENCY{1'b0}};
          end else if (do_bind == 1'b1) begin
            ready_history <= {READY_LATENCY{1'b0}};
          end else if (enable) begin
            if (READY_LATENCY == 1) begin
              ready_history <= sink_ready;
            end else begin
              ready_history <= {ready_history[READY_LATENCY-2:0], sink_ready};
            end
          end
        end

      end

    endgenerate


    //////////////////////////////////////////////////////////////////////////
    // change the order of symbols if necessary
    //////////////////////////////////////////////////////////////////////////
    
    logic [STREAM_DATAWIDTH-1:0] data_ordered;
	logic bound = 0;

    assign data_ordered = sink_data;

    always@(posedge clock or negedge resetn) begin
      if(!resetn) begin
        sink_ready <= 1'b0;
        first_valid_time       = 0;
        last_valid_time        = 0;
        num_elements_in_stream = 0;
		bound = 0;
      end else if(~bound) begin
        stream_objptr <= __ihc_aoc_get_stream_obj_ptr_for_component_interface(COMPONENT_NAME, INTERFACE_NAME, STREAM_DATAWIDTH, 0);
        $sformat(message, "[%7t][msim][stream_sink_dpi_ctrl][%s][%s] binding to stream_objptr=%0p", $time, COMPONENT_NAME, INTERFACE_NAME, stream_objptr);
        __ihc_hls_dbgs(message);
        sink_ready <= 1'b0;
        first_valid_time       = 0;
        last_valid_time        = 0;
        num_elements_in_stream = 0;
		bound = 1'b1;
      end else begin
        if (sink_valid && ((READY_LATENCY > 0) || real_sink_ready)) begin
          stream_write_sim(data_ordered, sink_startofpacket, sink_endofpacket, sink_empty, real_sink_ready);
          if (first_valid_time == 0) begin
            first_valid_time = $time;
          end
          last_valid_time        = $time;
          num_elements_in_stream = num_elements_in_stream + 1;
        end
        sink_ready <= stream_ready_sim();
      end
    end



    assign stream_active = sink_valid;

endmodule

// vim:set filetype=verilog: