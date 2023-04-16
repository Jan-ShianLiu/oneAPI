# +-----------------------------------
# | 
# | acl_avalon_mm_bridge_s10 "Avalon-MM Pipeline Bridge for S10"
# | 
# +-----------------------------------

# +-----------------------------------
# | request TCL package from ACDS 10.0
# | 
package require -exact qsys 16.0
# | 
# +-----------------------------------

# +-----------------------------------
# | module acl_avalon_mm_bridge_s10
# | 
set_module_property DESCRIPTION "Inserts a register stage in the Avalon-MM command and response paths. Accepts commands on its Avalon-MM slave port and propagates them to its Avalon-MM master port."
set_module_property NAME acl_avalon_mm_bridge_s10
set_module_property VERSION 16.930
set_module_property HIDE_FROM_SOPC false 
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Intel Corporation"
set_module_property DISPLAY_NAME "Avalon-MM Pipeline Bridge for S10"
set_module_property AUTHOR "Intel Corporation"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ELABORATION_CALLBACK elaborate
set_module_property HIDE_FROM_QUARTUS false
# | 
# +-----------------------------------

# +-----------------------------------
# | files
# | 
add_fileset          synth   QUARTUS_SYNTH 
set_fileset_property synth   TOP_LEVEL acl_avalon_mm_bridge_s10
add_fileset_file acl_avalon_mm_bridge_s10.v SYSTEM_VERILOG PATH acl_avalon_mm_bridge_s10.v

add_fileset          sim     SIM_VERILOG
set_fileset_property sim     TOP_LEVEL acl_avalon_mm_bridge_s10
add_fileset_file acl_avalon_mm_bridge_s10.v SYSTEM_VERILOG PATH acl_avalon_mm_bridge_s10.v

add_fileset          simvhdl SIM_VHDL
set_fileset_property simvhdl TOP_LEVEL acl_avalon_mm_bridge_s10
add_fileset_file acl_avalon_mm_bridge_s10.v SYSTEM_VERILOG PATH acl_avalon_mm_bridge_s10.v
# | 
# +-----------------------------------

# +-----------------------------------
# | parameters
# |
add_parameter SYNCHRONIZE_RESET INTEGER 1
set_parameter_property SYNCHRONIZE_RESET DEFAULT_VALUE 0
set_parameter_property SYNCHRONIZE_RESET DISPLAY_NAME SYNCHRONIZE_RESET
set_parameter_property SYNCHRONIZE_RESET TYPE INTEGER
set_parameter_property SYNCHRONIZE_RESET UNITS None
set_parameter_property SYNCHRONIZE_RESET HDL_PARAMETER true

add_parameter DISABLE_WAITREQUEST_BUFFERING INTEGER 0
set_parameter_property DISABLE_WAITREQUEST_BUFFERING DEFAULT_VALUE 0
set_parameter_property DISABLE_WAITREQUEST_BUFFERING DISPLAY_NAME DISABLE_WAITREQUEST_BUFFERING
set_parameter_property DISABLE_WAITREQUEST_BUFFERING TYPE INTEGER
set_parameter_property DISABLE_WAITREQUEST_BUFFERING UNITS None
set_parameter_property DISABLE_WAITREQUEST_BUFFERING AFFECTS_GENERATION false
set_parameter_property DISABLE_WAITREQUEST_BUFFERING HDL_PARAMETER true
set_parameter_property DISABLE_WAITREQUEST_BUFFERING DESCRIPTION {Disable waitrequest buffering}
 
add_parameter READDATA_PIPE_DEPTH INTEGER 1
set_parameter_property READDATA_PIPE_DEPTH DEFAULT_VALUE 1
set_parameter_property READDATA_PIPE_DEPTH ALLOWED_RANGES 1:32
set_parameter_property READDATA_PIPE_DEPTH DISPLAY_NAME READDATA_PIPE_DEPTH
set_parameter_property READDATA_PIPE_DEPTH TYPE INTEGER
set_parameter_property READDATA_PIPE_DEPTH UNITS None
set_parameter_property READDATA_PIPE_DEPTH AFFECTS_GENERATION false
set_parameter_property READDATA_PIPE_DEPTH HDL_PARAMETER true
set_parameter_property READDATA_PIPE_DEPTH DESCRIPTION {Number of pipeline stages on readdata}

add_parameter CMD_PIPE_DEPTH INTEGER 1
set_parameter_property CMD_PIPE_DEPTH DEFAULT_VALUE 1
set_parameter_property CMD_PIPE_DEPTH ALLOWED_RANGES 1:32
set_parameter_property CMD_PIPE_DEPTH DISPLAY_NAME CMD_PIPE_DEPTH
set_parameter_property CMD_PIPE_DEPTH TYPE INTEGER
set_parameter_property CMD_PIPE_DEPTH UNITS None
set_parameter_property CMD_PIPE_DEPTH AFFECTS_GENERATION false
set_parameter_property CMD_PIPE_DEPTH HDL_PARAMETER true
set_parameter_property CMD_PIPE_DEPTH DESCRIPTION {Number of unstallable pipeline stages on the command path. Only set > 1 if DISABLE_WAITREQUEST_BUFFERING=1}

add_parameter DATA_WIDTH INTEGER 32
set_parameter_property DATA_WIDTH DEFAULT_VALUE 32
set_parameter_property DATA_WIDTH DISPLAY_NAME {Data width}
set_parameter_property DATA_WIDTH TYPE INTEGER
set_parameter_property DATA_WIDTH UNITS None
set_parameter_property DATA_WIDTH DISPLAY_HINT ""
set_parameter_property DATA_WIDTH AFFECTS_GENERATION false
set_parameter_property DATA_WIDTH HDL_PARAMETER true
set_parameter_property DATA_WIDTH DESCRIPTION {Bridge data width}

add_parameter SYMBOL_WIDTH INTEGER 8
set_parameter_property SYMBOL_WIDTH DEFAULT_VALUE 8
set_parameter_property SYMBOL_WIDTH DISPLAY_NAME {Symbol width}
set_parameter_property SYMBOL_WIDTH TYPE INTEGER
set_parameter_property SYMBOL_WIDTH UNITS None
set_parameter_property SYMBOL_WIDTH DISPLAY_HINT ""
set_parameter_property SYMBOL_WIDTH AFFECTS_GENERATION false
set_parameter_property SYMBOL_WIDTH HDL_PARAMETER true
set_parameter_property SYMBOL_WIDTH DESCRIPTION {Symbol (byte) width}

add_parameter ADDRESS_WIDTH INTEGER 10
set_parameter_property ADDRESS_WIDTH DEFAULT_VALUE 10
set_parameter_property ADDRESS_WIDTH DISPLAY_NAME {Address width}
set_parameter_property ADDRESS_WIDTH TYPE INTEGER
set_parameter_property ADDRESS_WIDTH UNITS None
set_parameter_property ADDRESS_WIDTH DISPLAY_HINT ""
set_parameter_property ADDRESS_WIDTH AFFECTS_GENERATION false
set_parameter_property ADDRESS_WIDTH HDL_PARAMETER false
set_parameter_property ADDRESS_WIDTH DESCRIPTION {User-defined bridge address width}

# ------------------------------------------
# System info address width, so that we can automatically determine how
# wide the bridge's width needs to be. Note that this is in units of
# bytes.
# ------------------------------------------
add_parameter SYSINFO_ADDR_WIDTH INTEGER 10
set_parameter_property SYSINFO_ADDR_WIDTH SYSTEM_INFO { ADDRESS_WIDTH m0 }
set_parameter_property SYSINFO_ADDR_WIDTH TYPE INTEGER
set_parameter_property SYSINFO_ADDR_WIDTH AFFECTS_GENERATION false
set_parameter_property SYSINFO_ADDR_WIDTH HDL_PARAMETER false
set_parameter_property SYSINFO_ADDR_WIDTH VISIBLE false

add_parameter USE_AUTO_ADDRESS_WIDTH INTEGER 0
set_parameter_property USE_AUTO_ADDRESS_WIDTH DEFAULT_VALUE 0
set_parameter_property USE_AUTO_ADDRESS_WIDTH DISPLAY_NAME {Use automatically-determined address width}
set_parameter_property USE_AUTO_ADDRESS_WIDTH TYPE INTEGER
set_parameter_property USE_AUTO_ADDRESS_WIDTH UNITS None
set_parameter_property USE_AUTO_ADDRESS_WIDTH DISPLAY_HINT "boolean"
set_parameter_property USE_AUTO_ADDRESS_WIDTH AFFECTS_GENERATION false
set_parameter_property USE_AUTO_ADDRESS_WIDTH HDL_PARAMETER false
set_parameter_property USE_AUTO_ADDRESS_WIDTH DESCRIPTION {Use automatically-determined bridge address width instead of the user-defined address width}

# ------------------------------------------
# The actual automatic address width of the bridge. Different from the system info,
# because this takes the address units into account.
# ------------------------------------------
add_parameter AUTO_ADDRESS_WIDTH INTEGER 10
set_parameter_property AUTO_ADDRESS_WIDTH DEFAULT_VALUE 10
set_parameter_property AUTO_ADDRESS_WIDTH DISPLAY_NAME {Automatically-determined address width}
set_parameter_property AUTO_ADDRESS_WIDTH TYPE INTEGER
set_parameter_property AUTO_ADDRESS_WIDTH UNITS None
set_parameter_property AUTO_ADDRESS_WIDTH DISPLAY_HINT ""
set_parameter_property AUTO_ADDRESS_WIDTH DERIVED true
set_parameter_property AUTO_ADDRESS_WIDTH AFFECTS_GENERATION false
set_parameter_property AUTO_ADDRESS_WIDTH HDL_PARAMETER false
set_parameter_property AUTO_ADDRESS_WIDTH DESCRIPTION {The minimum bridge address width that is required to address the downstream slaves}

# ------------------------------------------
# The actual value that gets passed to the HDL
# ------------------------------------------
add_parameter HDL_ADDR_WIDTH INTEGER 10
set_parameter_property HDL_ADDR_WIDTH TYPE INTEGER
set_parameter_property HDL_ADDR_WIDTH DERIVED true
set_parameter_property HDL_ADDR_WIDTH VISIBLE false
set_parameter_property HDL_ADDR_WIDTH AFFECTS_GENERATION false
set_parameter_property HDL_ADDR_WIDTH HDL_PARAMETER true

add_parameter ADDRESS_UNITS STRING "SYMBOLS"
set_parameter_property ADDRESS_UNITS DISPLAY_NAME {Address units}
set_parameter_property ADDRESS_UNITS UNITS None
set_parameter_property ADDRESS_UNITS DISPLAY_HINT ""
set_parameter_property ADDRESS_UNITS AFFECTS_GENERATION false
set_parameter_property ADDRESS_UNITS HDL_PARAMETER false
set_parameter_property ADDRESS_UNITS ALLOWED_RANGES "SYMBOLS,WORDS"
set_parameter_property ADDRESS_UNITS DESCRIPTION {Address units (Symbols[bytes]/Words)}

add_parameter BURSTCOUNT_WIDTH INTEGER 1
set_parameter_property BURSTCOUNT_WIDTH DEFAULT_VALUE 1
set_parameter_property BURSTCOUNT_WIDTH DISPLAY_NAME {Burstcount width}
set_parameter_property BURSTCOUNT_WIDTH VISIBLE false
set_parameter_property BURSTCOUNT_WIDTH DERIVED true
set_parameter_property BURSTCOUNT_WIDTH TYPE INTEGER
set_parameter_property BURSTCOUNT_WIDTH UNITS None
set_parameter_property BURSTCOUNT_WIDTH DISPLAY_HINT ""
set_parameter_property BURSTCOUNT_WIDTH AFFECTS_GENERATION false
set_parameter_property BURSTCOUNT_WIDTH HDL_PARAMETER true
set_parameter_property BURSTCOUNT_WIDTH DESCRIPTION {Bridge burstcount width}

add_parameter MAX_BURST_SIZE INTEGER 1
set_parameter_property MAX_BURST_SIZE DISPLAY_NAME {Maximum burst size (words)}
set_parameter_property MAX_BURST_SIZE AFFECTS_GENERATION true
set_parameter_property MAX_BURST_SIZE HDL_PARAMETER false
set_parameter_property MAX_BURST_SIZE DESCRIPTION {Specifies the maximum burst size}
set_parameter_property MAX_BURST_SIZE ALLOWED_RANGES "1,2,4,8,16,32,64,128,256,512,1024"

add_parameter MAX_PENDING_RESPONSES INTEGER 4
set_parameter_property MAX_PENDING_RESPONSES DISPLAY_NAME {Maximum pending read transactions}
set_parameter_property MAX_PENDING_RESPONSES TYPE INTEGER
set_parameter_property MAX_PENDING_RESPONSES UNITS None
set_parameter_property MAX_PENDING_RESPONSES DISPLAY_HINT ""
set_parameter_property MAX_PENDING_RESPONSES AFFECTS_GENERATION false
set_parameter_property MAX_PENDING_RESPONSES DESCRIPTION {Controls the Avalon-MM maximum pending read transactions interface property of the bridge}

add_parameter LINEWRAPBURSTS INTEGER 0
set_parameter_property LINEWRAPBURSTS DISPLAY_NAME "Line wrap bursts"
set_parameter_property LINEWRAPBURSTS TYPE INTEGER
set_parameter_property LINEWRAPBURSTS AFFECTS_ELABORATION true
set_parameter_property LINEWRAPBURSTS HDL_PARAMETER false
set_parameter_property LINEWRAPBURSTS DISPLAY_HINT BOOLEAN
set_parameter_property LINEWRAPBURSTS AFFECTS_GENERATION false
set_parameter_property LINEWRAPBURSTS DESCRIPTION "This parameter allows you to match the behavior of some memory devices that implement a wrapping burst instead of an incrementing burst. The difference between the two is that with a wrapping burst, when the address reaches a burst boundary, the address wraps back to the previous burst boundary so that only the low-order bits are required for address counting"

add_display_item "Data" DATA_WIDTH parameter
add_display_item "Data" SYMBOL_WIDTH parameter
add_display_item "Address" ADDRESS_WIDTH parameter
add_display_item "Address" USE_AUTO_ADDRESS_WIDTH parameter
add_display_item "Address" AUTO_ADDRESS_WIDTH parameter
add_display_item "Address" ADDRESS_UNITS parameter
add_display_item "Burst" MAX_BURST_SIZE parameter
add_display_item "Burst" LINEWRAPBURSTS parameter
add_display_item "Pipelining" MAX_PENDING_RESPONSES parameter
add_display_item "Pipelining" PIPELINE_COMMAND parameter
add_display_item "Pipelining" PIPELINE_RESPONSE parameter
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
set_interface_property s0 addressAlignment DYNAMIC
set_interface_property s0 associatedClock clk
set_interface_property s0 bridgesToMaster m0
set_interface_property s0 burstOnBurstBoundariesOnly false
set_interface_property s0 explicitAddressSpan 0
set_interface_property s0 holdTime 0
set_interface_property s0 isMemoryDevice false
set_interface_property s0 isNonVolatileStorage false
set_interface_property s0 linewrapBursts false
set_interface_property s0 maximumPendingReadTransactions 4
set_interface_property s0 printableDevice false
set_interface_property s0 readLatency 0
set_interface_property s0 readWaitTime 0
set_interface_property s0 setupTime 0
set_interface_property s0 timingUnits Cycles
set_interface_property s0 writeWaitTime 0

set_interface_property s0 ASSOCIATED_CLOCK clk
set_interface_property s0 associatedReset reset
set_interface_property s0 ENABLED true

add_interface_port s0 s0_waitrequest waitrequest Output 1
add_interface_port s0 s0_readdata readdata Output DATA_WIDTH
set_port_property s0_readdata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_readdatavalid readdatavalid Output 1
add_interface_port s0 s0_burstcount burstcount Input BURSTCOUNT_WIDTH
set_port_property s0_burstcount VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_writedata writedata Input DATA_WIDTH
set_port_property s0_writedata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_address address Input HDL_ADDR_WIDTH
set_port_property s0_address VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_write write Input 1
add_interface_port s0 s0_read read Input 1
add_interface_port s0 s0_byteenable byteenable Input 4
set_port_property s0_byteenable VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port s0 s0_debugaccess debugaccess Input 1
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point m0
# | 
add_interface m0 avalon start
set_interface_property m0 associatedClock clk
set_interface_property m0 burstOnBurstBoundariesOnly false
set_interface_property m0 doStreamReads false
set_interface_property m0 doStreamWrites false
set_interface_property m0 linewrapBursts false

set_interface_property m0 ASSOCIATED_CLOCK clk
set_interface_property m0 associatedReset reset
set_interface_property m0 ENABLED true

add_interface_port m0 m0_waitrequest waitrequest Input 1
add_interface_port m0 m0_readdata readdata Input DATA_WIDTH
set_port_property m0_readdata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_readdatavalid readdatavalid Input 1
add_interface_port m0 m0_burstcount burstcount Output BURSTCOUNT_WIDTH
set_port_property m0_burstcount VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_writedata writedata Output DATA_WIDTH
set_port_property m0_writedata VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_address address Output HDL_ADDR_WIDTH
set_port_property m0_address VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_write write Output 1
add_interface_port m0 m0_read read Output 1
add_interface_port m0 m0_byteenable byteenable Output 4
set_port_property m0_byteenable VHDL_TYPE STD_LOGIC_VECTOR
add_interface_port m0 m0_debugaccess debugaccess Output 1
# | 
# +-----------------------------------

proc elaborate { } {
    set data_width   [ get_parameter_value DATA_WIDTH ]
    set addr_width   [ get_parameter_value ADDRESS_WIDTH ]
    set sym_width    [ get_parameter_value SYMBOL_WIDTH ]
    set byteen_width [ expr {$data_width / $sym_width} ]
    set mprt         [ get_parameter_value MAX_PENDING_RESPONSES ]
    set aunits       [ get_parameter_value ADDRESS_UNITS ]
    set burst_size   [ get_parameter_value MAX_BURST_SIZE ]
    set linewrap     [ get_parameter_value LINEWRAPBURSTS ]

    set_port_property m0_byteenable WIDTH_EXPR $byteen_width
    set_port_property s0_byteenable WIDTH_EXPR $byteen_width

    set_interface_property m0 bitsPerSymbol $sym_width
    set_interface_property s0 bitsPerSymbol $sym_width

    set_interface_property m0 addressUnits $aunits
    set_interface_property s0 addressUnits $aunits

    set_interface_property s0 maximumPendingReadTransactions $mprt

    set burstcount_width [ expr int (ceil (log($burst_size) / log(2))) + 1 ]
    set_parameter_value BURSTCOUNT_WIDTH $burstcount_width

    set_interface_property m0 linewrapBursts $linewrap
    set_interface_property s0 linewrapBursts $linewrap

    set auto_address_width [ calculate_address_widths $data_width $sym_width $aunits ]
    set_parameter_value AUTO_ADDRESS_WIDTH $auto_address_width

    set use_auto_address_width [ get_parameter_value USE_AUTO_ADDRESS_WIDTH ]
    if { $use_auto_address_width == 1 } {
        set_parameter_property ADDRESS_WIDTH ENABLED false
        set_parameter_value HDL_ADDR_WIDTH $auto_address_width
    } else {
        set_parameter_property ADDRESS_WIDTH ENABLED true
        set_parameter_value HDL_ADDR_WIDTH $addr_width
    }
}

proc calculate_address_widths { data_width sym_width aunits } {

    set sysinfo_address_width [ get_parameter_value SYSINFO_ADDR_WIDTH ]
    set auto_address_width $sysinfo_address_width

    if { [ string equal $aunits "WORDS" ] } {
        set num_sym             [ expr $data_width / $sym_width ]
        set byte_to_word_offset [ expr int (ceil (log($num_sym) / log(2))) ]
        set auto_address_width  [ expr $sysinfo_address_width - $byte_to_word_offset ]
    }

    return $auto_address_width
}

## Add documentation links for user guide and/or release notes
add_documentation_link "User Guide" https://documentation.altera.com/#/link/mwh1409960181641/mwh1409959275749 
add_documentation_link "Release Notes" https://documentation.altera.com/#/link/hco1416836145555/hco1416836653221
