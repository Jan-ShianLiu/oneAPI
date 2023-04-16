# TCL File Generated by Component Editor 11.0
# Tue Jul 19 13:15:52 PDT 2011


# +-----------------------------------
# | 
# | global_routing_clk "ACL Pipelined MM Bridge" v1.0
# | Altera OpenCL 2011.07.19.13:15:52
# | Pipelined clock crossing splitter
# | 
# | 
# |    ./global_routing_clk.v syn, sim
# | 
# +-----------------------------------

# +-----------------------------------
# | request TCL package from ACDS 11.0
# | 
package require -exact sopc 10.0
# | 
# +-----------------------------------

# +-----------------------------------
# | module global_routing_clk
# | 
set_module_property DESCRIPTION "Make signal use global routing"
set_module_property NAME global_routing_clk
set_module_property VERSION 10.0
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Altera OpenCL"
set_module_property DISPLAY_NAME "ACL Global Clk Signal"
set_module_property TOP_LEVEL_HDL_FILE global_routing.v
set_module_property TOP_LEVEL_HDL_MODULE global_routing
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE false
set_module_property ANALYZE_HDL false
# | 
# +-----------------------------------

# +-----------------------------------
# | files
# | 
add_file global_routing.v           {SYNTHESIS}
add_file global_routing_sim_model.v {SIMULATION}
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
