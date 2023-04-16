# +-----------------------------------
# | 
# | constant_address_bridge "Avalon-MM Constant Address Bridge"
# | 
# +-----------------------------------

# +-----------------------------------
# | request TCL package from ACDS 10.0
# | 
package require -exact qsys 16.0
# | 
# +-----------------------------------

# +-----------------------------------
# | module constant_address_bridge
# | 
set_module_property DESCRIPTION "Inserts a non-registered constant address bridge. All incoming request will be directed to a single address on the Avalon-MM master side of the IP."
set_module_property NAME constant_address_bridge
set_module_property VERSION 18.1
set_module_property HIDE_FROM_SOPC false 
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Intel Corporation"
set_module_property DISPLAY_NAME "Avalon-MM Constant Address Bridge"
set_module_property AUTHOR "Intel Corporation"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property HIDE_FROM_QUARTUS false
# | 
# +-----------------------------------

# +-----------------------------------
# | files
# | 
add_fileset          synth   QUARTUS_SYNTH 
set_fileset_property synth   TOP_LEVEL constant_address_bridge
add_fileset_file constant_address_bridge.v SYSTEM_VERILOG PATH constant_address_bridge.v

add_fileset          sim     SIM_VERILOG
set_fileset_property sim     TOP_LEVEL constant_address_bridge
add_fileset_file constant_address_bridge.v SYSTEM_VERILOG PATH constant_address_bridge.v

add_fileset          simvhdl SIM_VHDL
set_fileset_property simvhdl TOP_LEVEL constant_address_bridge
add_fileset_file constant_address_bridge.v SYSTEM_VERILOG PATH constant_address_bridge.v
# | 
# +-----------------------------------

# +-----------------------------------
# | parameters
# |
add_parameter DATA_WIDTH INTEGER 32
set_parameter_property DATA_WIDTH DEFAULT_VALUE 32
set_parameter_property DATA_WIDTH DISPLAY_NAME {Data width}
set_parameter_property DATA_WIDTH TYPE INTEGER
set_parameter_property DATA_WIDTH UNITS None
set_parameter_property DATA_WIDTH DISPLAY_HINT ""
set_parameter_property DATA_WIDTH AFFECTS_GENERATION false
set_parameter_property DATA_WIDTH HDL_PARAMETER true
set_parameter_property DATA_WIDTH DESCRIPTION {Bridge data width}

add_parameter ADDRESS_WIDTH INTEGER 28
set_parameter_property ADDRESS_WIDTH DEFAULT_VALUE 28
set_parameter_property ADDRESS_WIDTH DISPLAY_NAME {Address width}
set_parameter_property ADDRESS_WIDTH TYPE INTEGER
set_parameter_property ADDRESS_WIDTH UNITS None
set_parameter_property ADDRESS_WIDTH DISPLAY_HINT ""
set_parameter_property ADDRESS_WIDTH AFFECTS_GENERATION false
set_parameter_property ADDRESS_WIDTH HDL_PARAMETER true
set_parameter_property ADDRESS_WIDTH DESCRIPTION {User-defined bridge address width}

add_display_item "Data" DATA_WIDTH parameter
add_display_item "Address" ADDRESS_WIDTH parameter
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point clk
# | 
add_interface clk clock end
add_interface reset reset end

set_interface_property clk ENABLED true
set_interface_property reset ENABLED true
set_interface_property reset ASSOCIATED_CLOCK clk
set_interface_property reset synchronousEdges DEASSERT

add_interface_port clk clk clk Input 1
add_interface_port reset reset reset Input 1
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point s0
# | 
add_interface s0 avalon end
set_interface_property s0 associatedClock clk
set_interface_property s0 associatedReset reset
set_interface_property s0 addressUnits WORDS
set_interface_property s0 maximumPendingReadTransactions 0
set_interface_property s0 readWaitTime 0
set_interface_property s0 ENABLED true

add_interface_port s0 s0_waitrequest waitrequest Output 1
add_interface_port s0 s0_burstcount burstcount Input 4
set_port_property s0_burstcount VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_writedata writedata Input DATA_WIDTH
set_port_property s0_writedata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_address address Input ADDRESS_WIDTH
set_port_property s0_address VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_write write Input 1
add_interface_port s0 s0_byteenable byteenable Input 4
set_port_property s0_byteenable VHDL_TYPE STD_LOGIC_VECTOR
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point m0
# | 
add_interface m0 avalon start
set_interface_property m0 associatedClock clk
set_interface_property m0 associatedReset reset
set_interface_property m0 readWaitTime 0
set_interface_property m0 ENABLED true
set_interface_property m0 addressUnits "WORDS"

add_interface_port m0 m0_waitrequest waitrequest Input 1
add_interface_port m0 m0_burstcount burstcount Output 4
set_port_property m0_burstcount VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_writedata writedata Output DATA_WIDTH
set_port_property m0_writedata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_address address Output ADDRESS_WIDTH
set_port_property m0_address VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_write write Output 1
add_interface_port m0 m0_byteenable byteenable Output 4
set_port_property m0_byteenable VHDL_TYPE STD_LOGIC_VECTOR
# | 
# +-----------------------------------

