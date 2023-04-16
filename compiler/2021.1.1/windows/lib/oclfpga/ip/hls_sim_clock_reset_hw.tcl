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

set_module_property NAME hls_sim_clock_reset
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property GROUP Accelerators
set_module_property DISPLAY_NAME hls_sim_clock_reset
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true

add_parameter RESET_CYCLE_HOLD INTEGER 4
set_parameter_property RESET_CYCLE_HOLD DEFAULT_VALUE 4
set_parameter_property RESET_CYCLE_HOLD DISPLAY_NAME RESET_CYCLE_HOLD
set_parameter_property RESET_CYCLE_HOLD TYPE INTEGER
set_parameter_property RESET_CYCLE_HOLD UNITS None
set_parameter_property RESET_CYCLE_HOLD DESCRIPTION "The number of cycles to hold resetn low in simulation"
set_parameter_property RESET_CYCLE_HOLD HDL_PARAMETER true

# 
# file sets
# 
# Only sim, no synth
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL hls_sim_clock_reset
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file hls_sim_clock_reset.sv SYSTEM_VERILOG PATH hls_sim_clock_reset.sv


add_interface clock clock start
set_interface_property clock ENABLED true
add_interface_port clock clock clk Output 1

add_interface reset reset start
set_interface_property reset ENABLED true
set_interface_property reset associatedClock clock
add_interface_port reset resetn reset_n Output 1

add_interface clock2x clock start
set_interface_property clock2x ENABLED true
add_interface_port clock2x clock2x clk Output 1

add_interface reset_ctrl conduit start
set_interface_property reset_ctrl ENABLED true
add_interface_port reset_ctrl trigger_reset conduit Input 1

