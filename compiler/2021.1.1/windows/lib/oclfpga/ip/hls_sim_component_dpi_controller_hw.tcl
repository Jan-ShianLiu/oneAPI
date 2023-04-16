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

set_module_property NAME hls_sim_component_dpi_controller
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property GROUP Accelerators
set_module_property DISPLAY_NAME hls_sim_component_dpi_controller
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true

set_module_property ELABORATION_CALLBACK elaborate

# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL hls_sim_component_dpi_controller
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file hls_sim_stream_sink_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_stream_sink_dpi_bfm.sv
add_fileset_file hls_sim_stream_source_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_stream_source_dpi_bfm.sv
add_fileset_file hld_sim_latency_tracker.sv SYSTEM_VERILOG PATH hld_sim_latency_tracker.sv
add_fileset_file hls_sim_component_dpi_controller.sv SYSTEM_VERILOG PATH hls_sim_component_dpi_controller.sv

# Also enable simulation
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL hls_sim_component_dpi_controller
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file hls_sim_stream_sink_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_stream_sink_dpi_bfm.sv
add_fileset_file hls_sim_stream_source_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_stream_source_dpi_bfm.sv
add_fileset_file hld_sim_latency_tracker.sv SYSTEM_VERILOG PATH hld_sim_latency_tracker.sv
add_fileset_file hls_sim_component_dpi_controller.sv SYSTEM_VERILOG PATH hls_sim_component_dpi_controller.sv


add_parameter COMPONENT_NAME STRING "dut" "The compressed component name the dpi controller connects to"
set_parameter_property COMPONENT_NAME DEFAULT_VALUE "dut"
set_parameter_property COMPONENT_NAME DISPLAY_NAME COMPONENT_NAME
set_parameter_property COMPONENT_NAME TYPE STRING
set_parameter_property COMPONENT_NAME UNITS None
set_parameter_property COMPONENT_NAME DESCRIPTION "The compressed component name the dpi controller connects to"
set_parameter_property COMPONENT_NAME HDL_PARAMETER true

add_parameter COMPONENT_MANGLED_NAME STRING "dut" "The mangled component name the dpi controller connects to"
set_parameter_property COMPONENT_MANGLED_NAME DEFAULT_VALUE "dut"
set_parameter_property COMPONENT_MANGLED_NAME DISPLAY_NAME COMPONENT_MANGLED_NAME
set_parameter_property COMPONENT_MANGLED_NAME TYPE STRING
set_parameter_property COMPONENT_MANGLED_NAME UNITS None
set_parameter_property COMPONENT_MANGLED_NAME DESCRIPTION "The mangled component name the dpi controller connects to"
set_parameter_property COMPONENT_MANGLED_NAME HDL_PARAMETER true

add_parameter RETURN_DATAWIDTH INTEGER 64 "The datawidth of the default (implicit) output stream interface"
set_parameter_property RETURN_DATAWIDTH DEFAULT_VALUE 64
set_parameter_property RETURN_DATAWIDTH DISPLAY_NAME RETURN_DATAWIDTH
set_parameter_property RETURN_DATAWIDTH TYPE INTEGER
set_parameter_property RETURN_DATAWIDTH UNITS None
set_parameter_property RETURN_DATAWIDTH DESCRIPTION "The datawidth of the default (implicit) output stream interface"
set_parameter_property RETURN_DATAWIDTH HDL_PARAMETER true

add_parameter COMPONENT_NUM_SLAVES INTEGER 0 "The number of slave interfaces on the component."
set_parameter_property COMPONENT_NUM_SLAVES DEFAULT_VALUE 0
set_parameter_property COMPONENT_NUM_SLAVES DISPLAY_NAME COMPONENT_NUM_SLAVES
set_parameter_property COMPONENT_NUM_SLAVES TYPE INTEGER
set_parameter_property COMPONENT_NUM_SLAVES UNITS None
set_parameter_property COMPONENT_NUM_SLAVES DESCRIPTION "The number of slave interfaces on the component."
set_parameter_property COMPONENT_NUM_SLAVES HDL_PARAMETER true

add_parameter COMPONENT_HAS_SLAVE_RETURN INTEGER 0 "If set, wait for the slave_done signal to assert before declaring the component to be done."
set_parameter_property COMPONENT_HAS_SLAVE_RETURN DEFAULT_VALUE 0
set_parameter_property COMPONENT_HAS_SLAVE_RETURN DISPLAY_NAME COMPONENT_HAS_SLAVE_RETURN
set_parameter_property COMPONENT_HAS_SLAVE_RETURN TYPE INTEGER
set_parameter_property COMPONENT_HAS_SLAVE_RETURN UNITS None
set_parameter_property COMPONENT_HAS_SLAVE_RETURN DESCRIPTION "If set, wait for the slave_done signal to assert before declaring the component to be done."
set_parameter_property COMPONENT_HAS_SLAVE_RETURN HDL_PARAMETER true

add_parameter COMPONENT_NUM_OUTPUT_STREAMS INTEGER 0 "The number of stream_out interfaces on the component."
set_parameter_property COMPONENT_NUM_OUTPUT_STREAMS DEFAULT_VALUE 0
set_parameter_property COMPONENT_NUM_OUTPUT_STREAMS DISPLAY_NAME COMPONENT_NUM_OUTPUT_STREAMS
set_parameter_property COMPONENT_NUM_OUTPUT_STREAMS TYPE INTEGER
set_parameter_property COMPONENT_NUM_OUTPUT_STREAMS UNITS None
set_parameter_property COMPONENT_NUM_OUTPUT_STREAMS DESCRIPTION "The number of stream_out interfaces on the component."
set_parameter_property COMPONENT_NUM_OUTPUT_STREAMS HDL_PARAMETER true


add_interface clock clock end
set_interface_property clock ENABLED true
add_interface_port clock clock clk Input 1

add_interface reset reset end
set_interface_property reset associatedClock clock
add_interface_port reset resetn reset_n Input 1

add_interface clock2x clock end
set_interface_property clock2x ENABLED true
add_interface_port clock2x clock2x clk Input 1

add_interface dpi_control_bind conduit start
set_interface_property dpi_control_bind ENABLED true
add_interface_port dpi_control_bind bind_interfaces conduit Output 1

add_interface dpi_control_enable conduit start
set_interface_property dpi_control_enable ENABLED true
add_interface_port dpi_control_enable enable_interfaces conduit Output 1

add_interface dpi_control_slaves_ready conduit end
set_interface_property dpi_control_slaves_ready ENABLED true

add_interface dpi_control_slaves_done conduit end
set_interface_property dpi_control_slaves_done ENABLED true

add_interface dpi_control_stream_writes_active conduit end
set_interface_property dpi_control_stream_writes_active ENABLED true

add_interface component_enabled conduit end
set_interface_property component_enabled ENABLED true
add_interface_port component_enabled component_enabled conduit Input 1

add_interface component_done conduit start
set_interface_property component_done ENABLED true
add_interface_port component_done component_done conduit Output 1

add_interface component_wait_for_stream_writes conduit start
set_interface_property component_wait_for_stream_writes ENABLED true
add_interface_port component_wait_for_stream_writes component_wait_for_stream_writes conduit Output 1

add_interface slave_busy conduit end
set_interface_property slave_busy ENABLED true
add_interface_port slave_busy slave_busy conduit Input 1 

add_interface read_implicit_streams conduit start
set_interface_property read_implicit_streams ENABLED true
add_interface_port read_implicit_streams read_implicit_streams conduit Output 1

add_interface readback_from_slaves conduit start
set_interface_property readback_from_slaves ENABLED true
add_interface_port readback_from_slaves readback_from_slaves conduit Output 1

# Component call interface
add_interface component_call conduit source
set_interface_property component_call associatedClock clock
set_interface_property component_call associatedReset reset
add_interface_port component_call start valid Output 1
add_interface_port component_call busy stall Input 1

# Component return interface
add_interface component_return conduit sink
set_interface_property component_return associatedClock clock
set_interface_property component_return associatedReset reset
add_interface_port component_return done valid Input 1
add_interface_port component_return stall stall Output 1

add_interface component_irq interrupt start
set_interface_property component_irq associatedClock clock
set_interface_property component_irq associatedReset reset
set_interface_property component_irq ENABLED true
add_interface_port component_irq done_irq irq Input 1

### Elaboration callback
proc elaborate {} {
  set o_data_width [ get_parameter_value RETURN_DATAWIDTH ]

  add_interface returndata conduit sink
  set_interface_property returndata associatedClock clock
  set_interface_property returndata associatedReset reset
  add_interface_port returndata returndata data Input $o_data_width

  set num_slaves [get_parameter_value COMPONENT_NUM_SLAVES]
  if { $num_slaves > 0 } {
    add_interface_port dpi_control_slaves_ready slaves_ready conduit Input $num_slaves
    add_interface_port dpi_control_slaves_done slaves_done conduit Input $num_slaves
  }

  set num_stream_outs [get_parameter_value COMPONENT_NUM_OUTPUT_STREAMS]
  if { $num_stream_outs > 0 } {
    add_interface_port dpi_control_stream_writes_active stream_writes_active conduit Input $num_stream_outs
  }

  if { [get_parameter_value COMPONENT_HAS_SLAVE_RETURN] == 1 } {
    set_port_property busy TERMINATION 1
    set_port_property busy TERMINATION_VALUE 0
  }

}

# vim:set filetype=tcl:
