package require -exact sopc 10.0

# +-----------------------------------
# | module timer_s10
# | 
set_module_property DESCRIPTION "Timer"
set_module_property NAME acl_timer_s10
set_module_property VERSION 17.0
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Altera OpenCL"
set_module_property DISPLAY_NAME "ACL timer for S10"
set_module_property TOP_LEVEL_HDL_FILE timer_s10.v
set_module_property TOP_LEVEL_HDL_MODULE acl_timer_s10
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE false
set_module_property ELABORATION_CALLBACK elaborate
set_module_property ANALYZE_HDL false
# | 
# +-----------------------------------

# +-----------------------------------
# | files
# | 
add_file timer_s10.v {SYNTHESIS SIMULATION}
# | 
# +-----------------------------------

# +-----------------------------------
# | parameters
# | 
add_parameter WIDTH INTEGER 64
set_parameter_property WIDTH DEFAULT_VALUE 64
set_parameter_property WIDTH DISPLAY_NAME "Counter Width"
set_parameter_property WIDTH UNITS "bits" 
set_parameter_property WIDTH AFFECTS_ELABORATION true
set_parameter_property WIDTH HDL_PARAMETER true
#set_parameter_property WIDTH SYSTEM_INFO {MAX_SLAVE_DATA_WIDTH m}

add_parameter S_WIDTH_A INTEGER 2
set_parameter_property S_WIDTH_A DEFAULT_VALUE 2
set_parameter_property S_WIDTH_A DISPLAY_NAME "Slave Address Width"
set_parameter_property S_WIDTH_A UNITS "bits"
set_parameter_property S_WIDTH_A AFFECTS_ELABORATION true
set_parameter_property S_WIDTH_A HDL_PARAMETER true
set_parameter_property S_WIDTH_A DERIVED true
set_parameter_property S_WIDTH_A VISIBLE false

# | 
# +-----------------------------------

proc elaborate {} {
    set width_d [ get_parameter_value WIDTH ]
    set byteenable_width [ expr $width_d / 8 ]
    set s_width_a [ get_parameter_value S_WIDTH_A ]

    # +-----------------------------------
    # | connection point clk
    # | 
    add_interface clk clock end
    set_interface_property clk ENABLED true
    add_interface_port clk clk clk Input 1
    # | 
    # +-----------------------------------

    # +-----------------------------------
    # | connection point clk2x
    # | 
    add_interface clk2x clock end
    set_interface_property clk2x ENABLED true
    add_interface_port clk2x clk2x clk Input 1
    # | 
    # +-----------------------------------
    
    # +-----------------------------------
    # | connection point clk_reset
    # | 
    add_interface clk_reset reset end
    set_interface_property clk_reset associatedClock clk
    set_interface_property clk_reset synchronousEdges DEASSERT
    set_interface_property clk_reset ENABLED true
    add_interface_port clk_reset resetn reset_n Input 1
    # | 
    # +-----------------------------------

    # +-----------------------------------
    # | connection point s
    # | 
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

    add_interface_port s slave_address address Input $s_width_a
    add_interface_port s slave_writedata writedata Input $width_d
    add_interface_port s slave_read read Input 1
    add_interface_port s slave_write write Input 1
    add_interface_port s slave_byteenable byteenable Input $byteenable_width
    add_interface_port s slave_waitrequest waitrequest Output 1
    add_interface_port s slave_readdata readdata Output $width_d
    add_interface_port s slave_readdatavalid readdatavalid Output 1
    # | 
    # +-----------------------------------

}

