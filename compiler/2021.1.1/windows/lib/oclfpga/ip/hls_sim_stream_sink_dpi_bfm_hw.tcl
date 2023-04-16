# (c) 1992-2020 Intel Corporation.                            
# Intel, the Intel logo, Intel, MegaCore, NIOS II, Quartus and TalkBack words    
# and logos are trademarks of Intel Corporation or its subsidiaries in the U.S.  
# and/or other countries. Other marks and brands may be claimed as the property  
# of others. See Trademarks on intel.com for full list of Intel trademarks or    
# the Trademarks & Brands Names Database (if Intel) or See www.Intel.com/legal (if Altera) 
# Your use of Intel Corporation's design tools, logic functions and other        
# software and tools, and its AMPP partner logic functions, and any output       
# files any of the foregoing (including device programming or simulation         
# files), and any associated documentation or information are expressly subject  
# to the terms and conditions of the Altera Program License Subscription         
# Agreement, Intel MegaCore Function License Agreement, or other applicable      
# license agreement, including, without limitation, that your use is for the     
# sole purpose of programming logic devices manufactured by Intel and sold by    
# Intel or its authorized distributors.  Please refer to the applicable          
# agreement for further details.                                                 


package require -exact qsys 14.0

set_module_property NAME hls_sim_stream_sink_dpi_bfm
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property GROUP Accelerators
set_module_property DISPLAY_NAME hls_sim_stream_sink_dpi_bfm
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true

set_module_property ELABORATION_CALLBACK elaborate


# 
# file sets
# 
# only sim, no synth
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL hls_sim_stream_sink_dpi_bfm
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file hls_sim_stream_sink_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_stream_sink_dpi_bfm.sv


add_parameter COMPONENT_NAME STRING "dut" "The name of the component that contains the interface"
set_parameter_property COMPONENT_NAME DEFAULT_VALUE "dut"
set_parameter_property COMPONENT_NAME DISPLAY_NAME COMPONENT_NAME
set_parameter_property COMPONENT_NAME TYPE STRING
set_parameter_property COMPONENT_NAME UNITS None
set_parameter_property COMPONENT_NAME DESCRIPTION "The name of the component that contains the interface"
set_parameter_property COMPONENT_NAME HDL_PARAMETER true


add_parameter INTERFACE_NAME STRING "a" "The component's stream interface this dpi bfm connects to"
set_parameter_property INTERFACE_NAME DEFAULT_VALUE "a"
set_parameter_property INTERFACE_NAME DISPLAY_NAME INTERFACE_NAME
set_parameter_property INTERFACE_NAME TYPE STRING
set_parameter_property INTERFACE_NAME UNITS None
set_parameter_property INTERFACE_NAME DESCRIPTION "The component's stream interface this dpi bfm connects to"
set_parameter_property INTERFACE_NAME HDL_PARAMETER true


add_parameter STREAM_DATAWIDTH INTEGER 1 "The datawidth of the component's stream interface"
set_parameter_property STREAM_DATAWIDTH DISPLAY_NAME STREAM_DATAWIDTH
set_parameter_property STREAM_DATAWIDTH UNITS None
set_parameter_property STREAM_DATAWIDTH HDL_PARAMETER true

add_parameter READY_LATENCY INTEGER 0 "The ready latency of the component's stream interface"
set_parameter_property READY_LATENCY DISPLAY_NAME READY_LATENCY
set_parameter_property READY_LATENCY UNITS None
set_parameter_property READY_LATENCY HDL_PARAMETER true

add_parameter STREAM_BITSPERSYMBOL INTEGER 1 "The symbol width in the component's stream data bus"
set_parameter_property STREAM_BITSPERSYMBOL DISPLAY_NAME STREAM_BITSPERSYMBOL
set_parameter_property STREAM_BITSPERSYMBOL UNITS None
set_parameter_property STREAM_BITSPERSYMBOL HDL_PARAMETER true

add_parameter USES_PACKETS INTEGER 0 "Uses startofpacket and endofpacket signals"
set_parameter_property USES_PACKETS DISPLAY_NAME USES_PACKETS
set_parameter_property USES_PACKETS UNITS None
set_parameter_property USES_PACKETS HDL_PARAMETER false

add_parameter EMPTY_WIDTH INTEGER 0 "The width of the empty signal"
set_parameter_property EMPTY_WIDTH DISPLAY_NAME EMPTY_WIDTH
set_parameter_property EMPTY_WIDTH UNITS None
set_parameter_property EMPTY_WIDTH HDL_PARAMETER true

add_parameter FIRST_SYMBOL_IN_HIGH_ORDER_BITS INTEGER 0 "First symbol in high order bits"
set_parameter_property FIRST_SYMBOL_IN_HIGH_ORDER_BITS DISPLAY_NAME FIRST_SYMBOL_IN_HIGH_ORDER_BITS
set_parameter_property FIRST_SYMBOL_IN_HIGH_ORDER_BITS UNITS None
set_parameter_property FIRST_SYMBOL_IN_HIGH_ORDER_BITS HDL_PARAMETER true

add_interface clock clock end
set_interface_property clock ENABLED true
add_interface_port clock clock clk Input 1

add_interface reset reset end
set_interface_property reset associatedClock clock
add_interface_port reset resetn reset_n Input 1

add_interface clock2x clock end
set_interface_property clock2x ENABLED true
add_interface_port clock2x clock2x clk Input 1

add_interface dpi_control_bind conduit end
set_interface_property dpi_control_bind ENABLED true
add_interface_port dpi_control_bind do_bind conduit Input 1

add_interface dpi_control_enable conduit end
set_interface_property dpi_control_enable ENABLED true
add_interface_port dpi_control_enable enable conduit Input 1

add_interface dpi_control_stream_active conduit start
set_interface_property dpi_control_stream_active ENABLED true
add_interface_port dpi_control_stream_active stream_active conduit Output 1

### Elaboration callback
proc elaborate {} {
  ### interface sink
  # case:298272 - restrictions on the maximum datawidth prevent us from going beyond 4k wide interfaces
  set data_width [ get_parameter_value STREAM_DATAWIDTH ]
  set ready_latency [ get_parameter_value READY_LATENCY ]
  set bits_per_symbol [ get_parameter_value STREAM_BITSPERSYMBOL ]
  set uses_packets [ get_parameter_value USES_PACKETS]
  set empty_width [ get_parameter_value EMPTY_WIDTH ]
  set first_symbol_in_high_order_bits [ get_parameter_value FIRST_SYMBOL_IN_HIGH_ORDER_BITS ]
  if { $data_width > 4096 } {
    add_interface sink conduit sink
    set_interface_property sink associatedClock clock
    set_interface_property sink associatedReset reset
  } else {
    add_interface sink avalon_streaming sink
    set_interface_property sink associatedClock clock
    set_interface_property sink associatedReset reset
    set_interface_property sink maxChannel 0
    set_interface_property sink readyLatency ${ready_latency}
    set_interface_property sink dataBitsPerSymbol $bits_per_symbol
    set_interface_property sink firstSymbolInHighOrderBits $first_symbol_in_high_order_bits
  }
  add_interface_port sink sink_data data Input $data_width
  add_interface_port sink sink_ready ready Output 1
  add_interface_port sink sink_valid valid Input 1
  add_interface_port sink sink_startofpacket startofpacket Input 1
  add_interface_port sink sink_endofpacket endofpacket Input 1
  add_interface_port sink sink_empty empty Input [expr $empty_width > 0 ? $empty_width : 1]

  if {$uses_packets == 0} {
    set_port_property sink_startofpacket TERMINATION 1
    set_port_property sink_startofpacket TERMINATION_VALUE 0

    set_port_property sink_endofpacket TERMINATION 1
    set_port_property sink_endofpacket TERMINATION_VALUE 0
  }

  if {$empty_width == 0} {
    set_port_property sink_empty TERMINATION 1
    set_port_property sink_empty TERMINATION_VALUE 0
  }
}

