# 
# global_clockena "A10 Periph Clock Enable" v1.0
#  2017.02.15.14:38:56
# 
# 

# 
# request TCL package from ACDS 19.1
# 
package require -exact qsys 19.1
# 


# +-----------------------------------
# | module global_routing_clk
# | 
set_module_property DESCRIPTION "A10 Periph Clock Enable"
set_module_property NAME periph_clkena
set_module_property VERSION 1.0
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Altera OpenCL"
set_module_property DISPLAY_NAME "ACL Periph Clk Enable"


# | 
# +-----------------------------------
# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL periph_clkena
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false

add_fileset_file periph_clkena.v                                     VERILOG PATH    periph_clkena.v TOP_LEVEL_FILE

# | 
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point clk
# | 
add_interface clk clock end
set_interface_property clk ENABLED true
add_interface_port clk s clk Input 1
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point global_clk
# | 
add_interface global_clk clock start
set_interface_property global_clk ENABLED true
add_interface_port global_clk g clk Output 1
# | 
# +-----------------------------------
