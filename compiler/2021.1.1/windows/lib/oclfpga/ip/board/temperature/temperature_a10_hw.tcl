package require -exact sopc 10.0

set_module_property DESCRIPTION "ACL Temperature sensor - Arria 10"
set_module_property NAME acl_temperature_a10
set_module_property VERSION 15.1
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Altera OpenCL"
set_module_property DISPLAY_NAME "ACL temperature sensor for A10"
set_module_property TOP_LEVEL_HDL_FILE temperature_a10.v
set_module_property TOP_LEVEL_HDL_MODULE temperature_a10
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE false
set_module_property ANALYZE_HDL false

add_file temperature_a10.v {SYNTHESIS SIMULATION}
add_file temp_sense_a10.v {SYNTHESIS SIMULATION}
add_file temp_sense_a10.sdc {SYNTHESIS SIMULATION}

add_interface clk clock end
set_interface_property clk ENABLED true
add_interface_port clk clk clk Input 1

add_interface clk_reset reset end
set_interface_property clk_reset associatedClock clk
set_interface_property clk_reset synchronousEdges DEASSERT
set_interface_property clk_reset ENABLED true
add_interface_port clk_reset resetn reset_n Input 1

add_interface s avalon end
set_interface_property s addressAlignment DYNAMIC
set_interface_property s addressUnits WORDS
set_interface_property s associatedClock clk
set_interface_property s associatedReset clk_reset
set_interface_property s burstOnBurstBoundariesOnly false
set_interface_property s explicitAddressSpan 0
set_interface_property s holdTime 0
set_interface_property s isMemoryDevice false
set_interface_property s isNonVolatileStorage false
set_interface_property s linewrapBursts false
set_interface_property s maximumPendingReadTransactions 1
set_interface_property s printableDevice false
set_interface_property s readLatency 0
set_interface_property s readWaitTime 0
set_interface_property s setupTime 0
set_interface_property s timingUnits Cycles
set_interface_property s writeWaitTime 0
set_interface_property s ENABLED true

add_interface_port s slave_address address Input 1
add_interface_port s slave_writedata writedata Input 32
add_interface_port s slave_read read Input 1
add_interface_port s slave_write write Input 1
add_interface_port s slave_byteenable byteenable Input 4
add_interface_port s slave_waitrequest waitrequest Output 1
add_interface_port s slave_readdata readdata Output 32
add_interface_port s slave_readdatavalid readdatavalid Output 1
