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

set_module_property NAME hls_sim_main_dpi_controller
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property GROUP Accelerators
set_module_property DISPLAY_NAME hls_sim_main_dpi_controller
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL hls_sim_main_dpi_controller
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file hls_sim_main_dpi_controller.sv SYSTEM_VERILOG PATH hls_sim_main_dpi_controller.sv

# Also enable simulation
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL hls_sim_main_dpi_controller
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file hls_sim_main_dpi_controller.sv SYSTEM_VERILOG PATH hls_sim_main_dpi_controller.sv


add_parameter NUM_COMPONENTS INTEGER 1 "The number of components in the sim system"
set_parameter_property NUM_COMPONENTS DEFAULT_VALUE 1
set_parameter_property NUM_COMPONENTS DISPLAY_NAME NUM_COMPONENTS
set_parameter_property NUM_COMPONENTS TYPE INTEGER
set_parameter_property NUM_COMPONENTS UNITS None
set_parameter_property NUM_COMPONENTS DESCRIPTION "The number of components in the sim system"
set_parameter_property NUM_COMPONENTS HDL_PARAMETER true

add_parameter COMPONENT_NAMES_STR STRING "dut" "A string containing a comma separated list of component names"
set_parameter_property COMPONENT_NAMES_STR DEFAULT_VALUE "dut"
set_parameter_property COMPONENT_NAMES_STR DISPLAY_NAME COMPONENT_NAMES_STR
set_parameter_property COMPONENT_NAMES_STR TYPE STRING
set_parameter_property COMPONENT_NAMES_STR UNITS None
set_parameter_property COMPONENT_NAMES_STR DESCRIPTION "A string containing a comma separated list of component names"
set_parameter_property COMPONENT_NAMES_STR HDL_PARAMETER true

add_interface clock clock end
set_interface_property clock ENABLED true
add_interface_port clock clock clk Input 1

add_interface reset reset end
set_interface_property reset associatedClock clock
add_interface_port reset resetn reset_n Input 1

add_interface clock2x clock end
set_interface_property clock2x ENABLED true
add_interface_port clock2x clock2x clk Input 1

add_interface component_enabled conduit start
set_interface_property component_enabled ENABLED true
add_interface_port component_enabled component_enabled conduit Output NUM_COMPONENTS

add_interface component_done conduit end
set_interface_property component_done ENABLED true
add_interface_port component_done component_done conduit Input NUM_COMPONENTS

add_interface component_wait_for_stream_writes conduit end
set_interface_property component_wait_for_stream_writes ENABLED true
add_interface_port component_wait_for_stream_writes component_wait_for_stream_writes conduit Input NUM_COMPONENTS

add_interface reset_ctrl conduit start
set_interface_property reset_ctrl ENABLED true
add_interface_port reset_ctrl trigger_reset conduit Output 1

