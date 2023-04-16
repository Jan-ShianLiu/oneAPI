module aoc_sim_stream_source_dpi_bfm
#(
    parameter STREAM_DATAWIDTH = 32,
    parameter FIRST_SYMBOL_IN_HIGH_ORDER_BITS = 0, // This parameter is ignored due to case:1409438337
    parameter STREAM_BITSPERSYMBOL = 0,
    parameter EMPTY_WIDTH = 0,
    parameter EMPTY_PORT_WIDTH = (EMPTY_WIDTH > 0) ? EMPTY_WIDTH : 1,
    parameter COMPONENT_NAME = "dut",
    parameter INTERFACE_NAME = "a"
    )
    (
        output logic [STREAM_DATAWIDTH-1:0]     source_data,
        input                                   source_ready,
        output logic                            source_valid,
        output logic                            source_startofpacket,
        output logic                            source_endofpacket,
        output logic [EMPTY_PORT_WIDTH-1:0]     source_empty,

        input               do_bind,
        input               enable,

        input               clock,
        input               resetn,
        input               clock2x
    );

    import "DPI-C" context function void __ihc_hls_dbgs(string msg);
    import "DPI-C" context function chandle __ihc_aoc_get_stream_obj_ptr_for_component_interface(string component_name, string interface_name,  longint unsigned width,  longint unsigned direction);

    // serves to get data at the front of the stream (but not pop), also returns empty flag, if empty returns undefined (i.e., dummy) data

    import "DPI-C" context function int __ihc_aoc_stream_front(chandle obj_ptr, inout bit [STREAM_DATAWIDTH-1:0] data, inout bit sop, inout bit eop, inout bit [EMPTY_PORT_WIDTH-1:0] empty);
    // serves as a pop, it also returns empty flag for correctness check
    import "DPI-C" context function int __ihc_aoc_stream_read(chandle obj_ptr, inout bit [STREAM_DATAWIDTH-1:0] data, inout bit sop, inout bit eop, inout bit [EMPTY_PORT_WIDTH-1:0] empty);

    string message;
    chandle stream_objptr;

    longint unsigned first_valid_time;
    longint unsigned last_valid_time;
    longint unsigned num_elements_in_stream;

    function bit stream_front_sim(output bit [STREAM_DATAWIDTH-1:0] out_data, output bit out_sop, output bit out_eop, output bit [EMPTY_PORT_WIDTH-1:0] out_empty);
      automatic bit [STREAM_DATAWIDTH-1:0] data = 0;
      automatic int empty_queue = 1;
      automatic bit valid = 0;
      automatic bit sop = 0;
      automatic bit eop = 0;
      automatic bit [EMPTY_PORT_WIDTH-1:0] empty = 0;
      empty_queue = __ihc_aoc_stream_front(stream_objptr, data, sop, eop, empty);
      valid = (empty_queue==0);
      out_data = data;
      out_sop = sop;
      out_eop = eop;
      out_empty = empty;
      return valid;
    endfunction

    function bit stream_read_sim(output bit [STREAM_DATAWIDTH-1:0] out_data, output bit out_sop, output bit out_eop, output bit [EMPTY_PORT_WIDTH-1:0] out_empty);
      automatic bit [STREAM_DATAWIDTH-1:0] data = 0;
      automatic int empty_queue = 1;
      automatic bit valid = 0;
      automatic bit sop = 0;
      automatic bit eop = 0;
      automatic bit empty = 0;
      empty_queue = __ihc_aoc_stream_read(stream_objptr, data, sop, eop, empty);
      valid = (empty_queue==0);
      out_data = data;
      out_sop = sop;
      out_eop = eop;
      out_empty = empty;
      
      $sformat(message, "[%7t][msim][stream_source_dpi_ctrl][%s][%s] stream_read, valid=%0d, data=%0d, data=%x, sop=%0d, eop=%0d, empty=%0d", $time,  COMPONENT_NAME, INTERFACE_NAME, valid, data, data, sop, eop, empty);
      __ihc_hls_dbgs(message);
      return valid;
    endfunction

    initial
    begin
      $sformat(message, "[%7t][msim][stream_source_dpi_ctrl][%s][%s] sim start, STREAM_DATAWIDTH=%0d", $time, COMPONENT_NAME, INTERFACE_NAME, STREAM_DATAWIDTH);
      __ihc_hls_dbgs(message);
      $sformat(message, "[%7t][msim][stream_source_dpi_ctrl] enable=%0d", $time, enable);
      __ihc_hls_dbgs(message);
      $sformat(message, "[%7t][msim][stream_source_dpi_ctrl] clock=%0d", $time, clock);
      __ihc_hls_dbgs(message);
    end

    //////////////////////////////////////////////////////////////////////////
    // change the order of symbols if necessary
    //////////////////////////////////////////////////////////////////////////
    
    logic [STREAM_DATAWIDTH-1:0] data_ordered;
	
	logic [STREAM_DATAWIDTH-1:0] data_ordered_front;
	logic bound =0 ;

    assign source_data = data_ordered;

    always@(posedge clock or negedge resetn) begin
      if(!resetn) begin
        source_valid <= 1'b0;
        first_valid_time       = 0;
        last_valid_time        = 0;
        num_elements_in_stream = 0;
		bound = 0;
		
      end else if(!bound) begin
        $sformat(message, "[%7t][msim][stream_source_dpi_ctrl][%s][%s] seen do_bind", $time, COMPONENT_NAME, INTERFACE_NAME);
        __ihc_hls_dbgs(message);
        stream_objptr <= __ihc_aoc_get_stream_obj_ptr_for_component_interface(COMPONENT_NAME, INTERFACE_NAME,STREAM_DATAWIDTH, 1'b1 );
        $sformat(message, "[%7t][msim][stream_source_dpi_ctrl][%s][%s] stream_objptr=%0p", $time, COMPONENT_NAME, INTERFACE_NAME, stream_objptr);
        __ihc_hls_dbgs(message);
        source_valid <= 1'b0;
        first_valid_time       = 0;
        last_valid_time        = 0;
        num_elements_in_stream = 0;
		bound = 1;
      end else begin
        if(!source_valid) begin
          source_valid <= stream_front_sim(data_ordered, source_startofpacket, source_endofpacket, source_empty);
        end else if(source_ready) begin
          source_valid <= stream_read_sim(data_ordered, source_startofpacket, source_endofpacket, source_empty);
          source_valid <= stream_front_sim(data_ordered, source_startofpacket, source_endofpacket, source_empty);

          if (first_valid_time == 0) begin
            first_valid_time = $time;
          end
          last_valid_time        = $time;
          num_elements_in_stream = num_elements_in_stream + 1;
        end
	  end
	end


endmodule
