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

set_module_property NAME hls_sim_stream_source_dpi_bfm
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property GROUP Accelerators
set_module_property DISPLAY_NAME hls_sim_stream_source_dpi_bfm
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true

set_module_property ELABORATION_CALLBACK elaborate

# 
# file sets
# 
# only sim, no synth
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL hls_sim_stream_source_dpi_bfm
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file hls_sim_stream_source_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_stream_source_dpi_bfm.sv


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
set_parameter_property STREAM_DATAWIDTH DEFAULT_VALUE 1
set_parameter_property STREAM_DATAWIDTH DISPLAY_NAME STREAM_DATAWIDTH
set_parameter_property STREAM_DATAWIDTH TYPE INTEGER
set_parameter_property STREAM_DATAWIDTH UNITS None
set_parameter_property STREAM_DATAWIDTH DESCRIPTION "The datawidth of the component's stream interface"
set_parameter_property STREAM_DATAWIDTH HDL_PARAMETER true

add_parameter FORCE_STREAM_CONDUIT INTEGER 0 "Force this stream source to expose individual conduits instead of an Avalon Streaming interface"
set_parameter_property FORCE_STREAM_CONDUIT DEFAULT_VALUE 0
set_parameter_property FORCE_STREAM_CONDUIT DISPLAY_NAME FORCE_STREAM_CONDUIT
set_parameter_property FORCE_STREAM_CONDUIT TYPE INTEGER
set_parameter_property FORCE_STREAM_CONDUIT UNITS None
set_parameter_property FORCE_STREAM_CONDUIT DESCRIPTION "Force this stream source to expose individual conduits instead of an Avalon Streaming interface"
set_parameter_property FORCE_STREAM_CONDUIT HDL_PARAMETER false

add_parameter STREAM_BITSPERSYMBOL INTEGER 1 "The symbol width in the component's stream data bus"
set_parameter_property STREAM_BITSPERSYMBOL DISPLAY_NAME STREAM_BITSPERSYMBOL
set_parameter_property STREAM_BITSPERSYMBOL UNITS None
set_parameter_property STREAM_BITSPERSYMBOL HDL_PARAMETER true

add_parameter USES_PACKETS INTEGER 0 "Uses startofpacket and endofpacket signals"
set_parameter_property USES_PACKETS DISPLAY_NAME USES_PACKETS
set_parameter_property USES_PACKETS UNITS None
set_parameter_property USES_PACKETS HDL_PARAMETER false

add_parameter EMPTY_WIDTH INTEGER 0 "Width of the empty signal"
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

### Elaboration callback
proc elaborate {} {
  ### interface source
  # case:298272 - restrictions on the maximum datawidth prevent us from going beyond 4k wide interfaces
  set data_width [ get_parameter_value STREAM_DATAWIDTH ]
  set force_conduit [ get_parameter_value FORCE_STREAM_CONDUIT ]
  set bits_per_symbol [ get_parameter_value STREAM_BITSPERSYMBOL ]
  set uses_packets [ get_parameter_value USES_PACKETS]
  set empty_width [ get_parameter_value EMPTY_WIDTH ]
  set first_symbol_in_high_order_bits [ get_parameter_value FIRST_SYMBOL_IN_HIGH_ORDER_BITS ]

  set data_intf source
  set ready_intf source
  set valid_intf source
  set sop_intf source
  set eop_intf source
  set empty_intf source

  set data_type data
  set ready_type ready
  set valid_type valid
  set sop_type startofpacket
  set eop_type endofpacket
  set empty_type empty

  #setup interfaces
  if { $force_conduit == 1 } {
    set data_intf source_data
    set ready_intf source_ready
    set source_intf source_valid
    set sop_intf source_sop
    set eop_intf source_eop
    set empty_intf source_empty

    set data_type  conduit
    set ready_type conduit
    set valid_type conduit
    set sop_type   conduit
    set eop_type   conduit
    set empty_type conduit

    add_interface $data_intf conduit source
    add_interface $ready_intf conduit source
    add_interface $valid_intf conduit source

    if {$uses_packets == 1} {
      add_interface $sop_intf conduit source
      add_interface $eop_intf conduit source

      if {$empty_width > 0} {
        add_interface $empty_intf conduit source
      }
    }

    set_interface_property $data_intf associatedClock clock
    set_interface_property $data_intf associatedReset reset


  } elseif { $data_width > 4096 } {
    add_interface $data_intf conduit source
    set_interface_property $data_intf associatedClock clock
    set_interface_property $data_intf associatedReset reset
  } else {
    add_interface $data_intf avalon_streaming source
    set_interface_property $data_intf associatedClock clock
    set_interface_property $data_intf associatedReset reset
    set_interface_property $data_intf maxChannel 0
    set_interface_property $data_intf readyLatency 0
    set_interface_property $data_intf dataBitsPerSymbol $bits_per_symbol
    set_interface_property $data_intf firstSymbolInHighOrderBits $first_symbol_in_high_order_bits
  }

  # add ports
  add_interface_port $data_intf source_data data Output $data_width
  add_interface_port $ready_intf source_ready $ready_type Input 1
  add_interface_port $valid_intf source_valid $valid_type Output 1

  if {$uses_packets == 1} {
    add_interface_port $sop_intf source_startofpacket startofpacket Output 1
    add_interface_port $eop_intf source_endofpacket endofpacket Output 1
    
    if {$empty_width > 0} {
      add_interface_port $empty_intf source_empty empty Output $empty_width
    }
  }
}

