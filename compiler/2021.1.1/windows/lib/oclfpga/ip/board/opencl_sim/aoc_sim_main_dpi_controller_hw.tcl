package require -exact qsys 14.0

set_module_property NAME                         aoc_sim_main_dpi_controller
set_module_property GROUP                        "OpenCL Simulation"
set_module_property DISPLAY_NAME                 "OpenCL Simulation Main"
set_module_property DESCRIPTION                  "Handles kernel system reset and interrupt"
set_module_property VERSION                      1.0
set_module_property AUTHOR                       "Intel Corporation"
set_module_property EDITABLE                     true
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property INTERNAL                     false


# 
# file sets
#
# This module is not synthesizable
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""

# Also enable simulation
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL aoc_sim_main_dpi_controller
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file aoc_sim_main_dpi_controller.sv SYSTEM_VERILOG PATH aoc_sim_main_dpi_controller.sv


add_interface clock clock end
set_interface_property clock ENABLED true
add_interface_port clock clock clk Input 1

add_interface reset reset end
set_interface_property reset associatedClock clock
add_interface_port reset resetn reset_n Input 1

# TODO: case:525822: remove clock2x as this is not used anywhere
add_interface clock2x clock end
set_interface_property clock2x ENABLED true
add_interface_port clock2x clock2x clk Input 1

add_interface reset_ctrl conduit start
set_interface_property reset_ctrl ENABLED true
add_interface_port reset_ctrl trigger_reset conduit Output 1

#---------------------------------------------------------------------
#  OpenCL Kernel interrupt
#---------------------------------------------------------------------
add_interface kernel_interrupt interrupt receiver
add_interface_port kernel_interrupt kernel_interrupt irq Input 1
