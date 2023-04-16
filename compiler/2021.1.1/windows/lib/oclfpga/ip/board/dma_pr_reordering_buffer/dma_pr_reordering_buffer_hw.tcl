# +-----------------------------------
# | 
# | dma_pr_reordering_buffer "Avalon-MM DMA PR reordering buffer"
# | 
# +-----------------------------------

# +-----------------------------------
# | request TCL package from ACDS 10.0
# | 
package require -exact qsys 16.0
# | 
# +-----------------------------------

# +-----------------------------------
# | module dma_reordering_buffer
# | 
set_module_property DESCRIPTION "Inserts a reordering buffer. All incoming request will be reordered."
set_module_property NAME dma_pr_reordering_buffer
set_module_property VERSION 19.3
set_module_property HIDE_FROM_SOPC false 
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Intel Corporation"
set_module_property DISPLAY_NAME "Avalon-MM DMA PR Reordering Buffer"
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
set_fileset_property synth   TOP_LEVEL dma_pr_reordering_buffer
add_fileset_file dma_reordering_buffer.v SYSTEM_VERILOG PATH dma_pr_reordering_buffer.sv

add_fileset          sim     SIM_VERILOG
set_fileset_property sim     TOP_LEVEL dma_pr_reordering_buffer
add_fileset_file dma_reordering_buffer.v SYSTEM_VERILOG PATH dma_pr_reordering_buffer.sv

add_fileset          simvhdl SIM_VHDL
set_fileset_property simvhdl TOP_LEVEL dma_pr_reordering_buffer
add_fileset_file dma_reordering_buffer.v SYSTEM_VERILOG PATH dma_pr_reordering_buffer.sv
# | 
# +-----------------------------------

# +-----------------------------------
# | parameters
# |
add_parameter DATA_WIDTH INTEGER 256
set_parameter_property DATA_WIDTH DEFAULT_VALUE 256
set_parameter_property DATA_WIDTH DISPLAY_NAME {Data width}
set_parameter_property DATA_WIDTH TYPE INTEGER
set_parameter_property DATA_WIDTH UNITS None
set_parameter_property DATA_WIDTH DISPLAY_HINT ""
set_parameter_property DATA_WIDTH AFFECTS_GENERATION false
set_parameter_property DATA_WIDTH HDL_PARAMETER true
set_parameter_property DATA_WIDTH DESCRIPTION {Data width}

add_parameter ADDRESS_WIDTH INTEGER 7
set_parameter_property ADDRESS_WIDTH DEFAULT_VALUE 7
set_parameter_property ADDRESS_WIDTH DISPLAY_NAME {Address width}
set_parameter_property ADDRESS_WIDTH TYPE INTEGER
set_parameter_property ADDRESS_WIDTH UNITS None
set_parameter_property ADDRESS_WIDTH DISPLAY_HINT ""
set_parameter_property ADDRESS_WIDTH AFFECTS_GENERATION false
set_parameter_property ADDRESS_WIDTH HDL_PARAMETER true
set_parameter_property ADDRESS_WIDTH DESCRIPTION {Address width}

add_parameter BURSTCOUNT_WIDTH INTEGER 4
set_parameter_property BURSTCOUNT_WIDTH DEFAULT_VALUE 4
set_parameter_property BURSTCOUNT_WIDTH DISPLAY_NAME {Burstcount width}
set_parameter_property BURSTCOUNT_WIDTH TYPE INTEGER
set_parameter_property BURSTCOUNT_WIDTH UNITS None
set_parameter_property BURSTCOUNT_WIDTH DISPLAY_HINT ""
set_parameter_property BURSTCOUNT_WIDTH AFFECTS_GENERATION false
set_parameter_property BURSTCOUNT_WIDTH HDL_PARAMETER true
set_parameter_property BURSTCOUNT_WIDTH DESCRIPTION {Burstcount width}

add_display_item "Data" DATA_WIDTH parameter
add_display_item "Address" ADDRESS_WIDTH parameter
add_display_item "Burstcount" BURSTCOUNT_WIDTH parameter
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point clk
# | 
add_interface clk clock end
add_interface resetn reset end

set_interface_property clk ENABLED true
set_interface_property resetn ENABLED true
set_interface_property resetn ASSOCIATED_CLOCK clk
set_interface_property resetn synchronousEdges DEASSERT

add_interface_port clk clk clk Input 1
add_interface_port resetn resetn reset_n Input 1
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point s0
# | 
add_interface s avalon end
set_interface_property s associatedClock clk
set_interface_property s associatedReset resetn
set_interface_property s addressUnits WORDS
set_interface_property s maximumPendingReadTransactions 0
set_interface_property s readWaitTime 0
set_interface_property s ENABLED true

add_interface_port s s_waitrequest waitrequest Output 1
add_interface_port s s_burstcount burstcount Input BURSTCOUNT_WIDTH
set_port_property s_burstcount VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s s_writedata writedata Input DATA_WIDTH
set_port_property s_writedata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s s_address address Input ADDRESS_WIDTH
set_port_property s_address VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s s_write write Input 1
add_interface_port s s_byteenable byteenable Input DATA_WIDTH/8 
set_port_property s_byteenable VHDL_TYPE STD_LOGIC_VECTOR
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point m0
# | 
add_interface m avalon start
set_interface_property m associatedClock clk
set_interface_property m associatedReset resetn
set_interface_property m readWaitTime 0
set_interface_property m ENABLED true
set_interface_property m addressUnits "WORDS"

add_interface_port m m_waitrequest waitrequest Input 1
add_interface_port m m_burstcount burstcount Output BURSTCOUNT_WIDTH
set_port_property m_burstcount VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m m_writedata writedata Output DATA_WIDTH
set_port_property m_writedata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m m_address address Output ADDRESS_WIDTH
set_port_property m_address VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m m_write write Output 1
add_interface_port m m_byteenable byteenable Output DATA_WIDTH/8
set_port_property m_byteenable VHDL_TYPE STD_LOGIC_VECTOR
# | 
# +-----------------------------------

