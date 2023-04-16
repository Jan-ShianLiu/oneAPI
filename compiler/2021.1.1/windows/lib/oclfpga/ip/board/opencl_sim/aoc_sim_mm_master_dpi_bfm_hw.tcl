package require -exact qsys 14.0

set_module_property NAME                         aoc_sim_mm_master_dpi_bfm
set_module_property GROUP                        "OpenCL Simulation"
set_module_property DISPLAY_NAME                 "OpenCL Simulation Avalon-MM Master DPI BFM"
set_module_property DESCRIPTION                  "Interface between Simulator MMD and Kernel Interface and Memory Bank Divider"
set_module_property VERSION                      1.0
set_module_property AUTHOR                       "Intel Corporation"
set_module_property EDITABLE                     false               
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property INTERNAL                     false
set_module_property VALIDATION_CALLBACK          validate
set_module_property ELABORATION_CALLBACK         elaborate

# ---------------------------------------------------------------------
# Files
# ---------------------------------------------------------------------
# Define file set
add_fileset sim_verilog SIM_VERILOG sim_verilog_proc
set_fileset_property sim_verilog top_level aoc_sim_mm_master_dpi_bfm

add_fileset quartus_synth QUARTUS_SYNTH quartus_synth_proc

proc get_qsys_verification_ip_path {} {
   set QUARTUS_ROOTDIR $::env(QUARTUS_ROOTDIR)
   puts $QUARTUS_ROOTDIR         
   append QSYS_VERIFICATION_IP $QUARTUS_ROOTDIR "/../ip/altera/sopc_builder_ip/verification/"
   puts $QSYS_VERIFICATION_IP
   return $QSYS_VERIFICATION_IP
}

# SIM_VERILOG generation callback procedure
proc sim_verilog_proc {aoc_sim_mm_master_dpi_bfm} {
   set QSYS_VERIFICATION_IP [get_qsys_verification_ip_path]

   add_fileset_file verbosity_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/verbosity_pkg.sv"
   add_fileset_file avalon_mm_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_mm_pkg.sv"
   add_fileset_file avalon_utilities_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_utilities_pkg.sv"

   add_fileset_file altera_avalon_mm_master_bfm.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/altera_avalon_mm_master_bfm/altera_avalon_mm_master_bfm.sv"
   add_fileset_file aoc_sim_mm_master_dpi_bfm.sv SYSTEM_VERILOG PATH aoc_sim_mm_master_dpi_bfm.sv
   
   set_fileset_file_attribute verbosity_pkg.sv        COMMON_SYSTEMVERILOG_PACKAGE avalon_lvip_verbosity_pkg
   set_fileset_file_attribute avalon_mm_pkg.sv        COMMON_SYSTEMVERILOG_PACKAGE avalon_vip_avalon_mm_pkg
   set_fileset_file_attribute avalon_utilities_pkg.sv COMMON_SYSTEMVERILOG_PACKAGE avalon_vip_avalon_utilities_pkg
}

# This module is not synthesizable
proc quartus_synth_proc { } { }

proc log {base x} {
    expr {log($x)/log($base)}
}

#------------------------------------------------------------------------------
# Parameters names
#------------------------------------------------------------------------------
# Kernel Interface specific parameters
set KI_AV_ADDRESS_W           "KI_AV_ADDRESS_W"
set KI_AV_SYMBOL_W            "KI_AV_SYMBOL_W" 
set KI_AV_NUMSYMBOLS          "KI_AV_NUMSYMBOLS"
set KI_USE_WAIT_REQUEST       "KI_USE_WAIT_REQUEST"

# Memory Bank Divider specific parameters
set MBD_AV_ADDRESS_W          "MBD_AV_ADDRESS_W"
set MBD_AV_SYMBOL_W           "MBD_AV_SYMBOL_W"
set MBD_AV_NUMSYMBOLS         "MBD_AV_NUMSYMBOLS"
set MBD_AV_BURSTSIZE          "MBD_AV_BURSTSIZE"
set MBD_USE_WAIT_REQUEST      "MBD_USE_WAIT_REQUEST"

# Common parameters and properties between Kernel Interface and Memory Bank Divider
set USE_READ                  "USE_READ"
set USE_WRITE                 "USE_WRITE"
set USE_ADDRESS               "USE_ADDRESS"
set USE_BYTE_ENABLE           "USE_BYTE_ENABLE"
set USE_READ_DATA             "USE_READ_DATA"
set USE_READ_DATA_VALID       "USE_READ_DATA_VALID"
set USE_WRITE_DATA            "USE_WRITE_DATA"

set AV_BURST_LINEWRAP         "AV_BURST_LINEWRAP" 
set AV_BURST_BNDR_ONLY        "AV_BURST_BNDR_ONLY"

set AV_FIX_READ_LATENCY       "AV_FIX_READ_LATENCY"
set AV_READ_WAIT_TIME         "AV_READ_WAIT_TIME"
set AV_WRITE_WAIT_TIME        "AV_WRITE_WAIT_TIME"
set REGISTER_WAITREQUEST      "REGISTER_WAITREQUEST"

set ADDRESS_UNITS              "ADDRESS_UNITS"
set AV_REGISTERINCOMINGSIGNALS "AV_REGISTERINCOMINGSIGNALS"

#---------------------------------------------------------------------
# Kernel Interface specific parameters
#---------------------------------------------------------------------
add_parameter $KI_AV_ADDRESS_W Integer 32
set_parameter_property $KI_AV_ADDRESS_W DISPLAY_NAME "Address width"
set_parameter_property $KI_AV_ADDRESS_W AFFECTS_ELABORATION true
set_parameter_property $KI_AV_ADDRESS_W DESCRIPTION "The width of the address signal."
set_parameter_property $KI_AV_ADDRESS_W ALLOWED_RANGES {1:64}
set_parameter_property $KI_AV_ADDRESS_W HDL_PARAMETER true
set_parameter_property $KI_AV_ADDRESS_W GROUP "Kernel Interface Port Widths"

add_parameter $KI_AV_SYMBOL_W Integer 8
set_parameter_property $KI_AV_SYMBOL_W DISPLAY_NAME "Symbol width"
set_parameter_property $KI_AV_SYMBOL_W AFFECTS_ELABORATION true
set_parameter_property $KI_AV_SYMBOL_W DESCRIPTION "The width of an individual symbol. The default is an 8 bit Byte."
set_parameter_property $KI_AV_SYMBOL_W ALLOWED_RANGES {1:1024}
set_parameter_property $KI_AV_SYMBOL_W HDL_PARAMETER true
set_parameter_property $KI_AV_SYMBOL_W GROUP "Kernel Interface Port Widths"

add_parameter $KI_AV_NUMSYMBOLS Integer 4
set_parameter_property $KI_AV_NUMSYMBOLS DISPLAY_NAME "Number of Symbols"
set_parameter_property $KI_AV_NUMSYMBOLS AFFECTS_ELABORATION true
set_parameter_property $KI_AV_NUMSYMBOLS DESCRIPTION "The number of symbols in a word. The default is 4 bytes per word."
set_parameter_property $KI_AV_NUMSYMBOLS ALLOWED_RANGES {1,2,4,8,16,32,64,128}
set_parameter_property $KI_AV_NUMSYMBOLS HDL_PARAMETER true
set_parameter_property $KI_AV_NUMSYMBOLS GROUP "Kernel Interface Port Widths"

# default waitrequest is false to match with Kernel Interface IP
add_parameter $KI_USE_WAIT_REQUEST Integer 0
set_parameter_property $KI_USE_WAIT_REQUEST DISPLAY_NAME "Use the waitrequest signal"
set_parameter_property $KI_USE_WAIT_REQUEST AFFECTS_ELABORATION true
set_parameter_property $KI_USE_WAIT_REQUEST DESCRIPTION "Use the waitrequest signal"
set_parameter_property $KI_USE_WAIT_REQUEST DISPLAY_HINT boolean
set_parameter_property $KI_USE_WAIT_REQUEST HDL_PARAMETER true
set_parameter_property $KI_USE_WAIT_REQUEST GROUP "Kernel Interface Port Enables"

#---------------------------------------------------------------------
# Memory Bank Divider specific parameters
#---------------------------------------------------------------------
add_parameter $MBD_AV_ADDRESS_W Integer 32
set_parameter_property $MBD_AV_ADDRESS_W DISPLAY_NAME "Memory bank address width"
set_parameter_property $MBD_AV_ADDRESS_W AFFECTS_ELABORATION true
set_parameter_property $MBD_AV_ADDRESS_W DESCRIPTION "The width of the memory bank address signal."
set_parameter_property $MBD_AV_ADDRESS_W ALLOWED_RANGES {1:64}
set_parameter_property $MBD_AV_ADDRESS_W HDL_PARAMETER true
set_parameter_property $MBD_AV_ADDRESS_W GROUP "Memory Bank Divider Port Widths"

add_parameter $MBD_AV_SYMBOL_W Integer 8
set_parameter_property $MBD_AV_SYMBOL_W DISPLAY_NAME "Memory bank symbol width"
set_parameter_property $MBD_AV_SYMBOL_W AFFECTS_ELABORATION true
set_parameter_property $MBD_AV_SYMBOL_W DESCRIPTION "The width of an individual symbol. Only support 1 byte (8 bits)"
set_parameter_property $MBD_AV_SYMBOL_W ALLOWED_RANGES {8}
set_parameter_property $MBD_AV_SYMBOL_W HDL_PARAMETER true
set_parameter_property $MBD_AV_SYMBOL_W GROUP "Memory Bank Divider Port Widths"

add_parameter $MBD_AV_NUMSYMBOLS Integer 64
set_parameter_property $MBD_AV_NUMSYMBOLS DISPLAY_NAME "Memory bank number of Symbols"
set_parameter_property $MBD_AV_NUMSYMBOLS AFFECTS_ELABORATION true
set_parameter_property $MBD_AV_NUMSYMBOLS DESCRIPTION "Set this value equal to memory bank data width / 8, i.e. size is 64 for width 512."
set_parameter_property $MBD_AV_NUMSYMBOLS ALLOWED_RANGES {1,2,4,8,16,32,64,128}
set_parameter_property $MBD_AV_NUMSYMBOLS HDL_PARAMETER true
set_parameter_property $MBD_AV_NUMSYMBOLS GROUP "Memory Bank Divider Port Widths"

# Burstsize is what memory bank divider uses, since this port connects to memory bank divider, I want to make the identical
add_parameter $MBD_AV_BURSTSIZE Integer 16
set_parameter_property $MBD_AV_BURSTSIZE DISPLAY_NAME "Memory bank burst size"
set_parameter_property $MBD_AV_BURSTSIZE AFFECTS_ELABORATION true
set_parameter_property $MBD_AV_BURSTSIZE DESCRIPTION "This value is for memory transfer through memory bank divider for accurate memory model."
set_parameter_property $MBD_AV_BURSTSIZE ALLOWED_RANGES {1,2,4,8,16,32,64}
set_parameter_property $MBD_AV_BURSTSIZE HDL_PARAMETER true
set_parameter_property $MBD_AV_BURSTSIZE GROUP "Memory Bank Divider Port Widths"

# default  waitrequest is true to match with Memory Bank Divider IP
add_parameter $MBD_USE_WAIT_REQUEST Integer 1
set_parameter_property $MBD_USE_WAIT_REQUEST DISPLAY_NAME "Use the waitrequest signal"
set_parameter_property $MBD_USE_WAIT_REQUEST AFFECTS_ELABORATION true
set_parameter_property $MBD_USE_WAIT_REQUEST DESCRIPTION "Use the waitrequest signal"
set_parameter_property $MBD_USE_WAIT_REQUEST DISPLAY_HINT boolean
set_parameter_property $MBD_USE_WAIT_REQUEST HDL_PARAMETER true
set_parameter_property $MBD_USE_WAIT_REQUEST GROUP "Memory Bank Divider Port Enables"

#---------------------------------------------------------------------
# Common parameters between Kernel Interface and Memory Bank Divider
#---------------------------------------------------------------------
add_parameter $USE_READ Integer 1 
set_parameter_property $USE_READ DISPLAY_NAME "Use the read signal"
set_parameter_property $USE_READ AFFECTS_ELABORATION true
set_parameter_property $USE_READ DESCRIPTION "Use the read signal"
set_parameter_property $USE_READ DISPLAY_HINT boolean
set_parameter_property $USE_READ HDL_PARAMETER true
set_parameter_property $USE_READ GROUP "Common Port Enables"

add_parameter $USE_WRITE Integer 1 
set_parameter_property $USE_WRITE DISPLAY_NAME "Use the write signal"
set_parameter_property $USE_WRITE AFFECTS_ELABORATION true
set_parameter_property $USE_WRITE DESCRIPTION "Use the write signal"
set_parameter_property $USE_WRITE DISPLAY_HINT boolean
set_parameter_property $USE_WRITE HDL_PARAMETER true
set_parameter_property $USE_WRITE GROUP "Common Port Enables"

add_parameter $USE_ADDRESS Integer 1 
set_parameter_property $USE_ADDRESS DISPLAY_NAME "Use the address signal"
set_parameter_property $USE_ADDRESS AFFECTS_ELABORATION true
set_parameter_property $USE_ADDRESS DESCRIPTION "Use the address signal"
set_parameter_property $USE_ADDRESS DISPLAY_HINT boolean
set_parameter_property $USE_ADDRESS HDL_PARAMETER true
set_parameter_property $USE_ADDRESS GROUP "Common Port Enables"

add_parameter $USE_BYTE_ENABLE Integer 1 
set_parameter_property $USE_BYTE_ENABLE DISPLAY_NAME "Use the byteenable signal"
set_parameter_property $USE_BYTE_ENABLE AFFECTS_ELABORATION true
set_parameter_property $USE_BYTE_ENABLE DESCRIPTION "Use the byteenable signal"
set_parameter_property $USE_BYTE_ENABLE DISPLAY_HINT boolean
set_parameter_property $USE_BYTE_ENABLE HDL_PARAMETER true
set_parameter_property $USE_BYTE_ENABLE GROUP "Common Port Enables"

add_parameter $USE_READ_DATA Integer 1 
set_parameter_property $USE_READ_DATA DISPLAY_NAME "Use the readdata signal"
set_parameter_property $USE_READ_DATA AFFECTS_ELABORATION true
set_parameter_property $USE_READ_DATA DESCRIPTION "Use the readdata signal"
set_parameter_property $USE_READ_DATA DISPLAY_HINT boolean
set_parameter_property $USE_READ_DATA HDL_PARAMETER true
set_parameter_property $USE_READ_DATA GROUP "Common Port Enables"

add_parameter $USE_READ_DATA_VALID Integer 1
set_parameter_property $USE_READ_DATA_VALID DISPLAY_NAME "Use the readdatavalid signal"
set_parameter_property $USE_READ_DATA_VALID AFFECTS_ELABORATION true
set_parameter_property $USE_READ_DATA_VALID DESCRIPTION "Use the readdatavalid signal"
set_parameter_property $USE_READ_DATA_VALID DISPLAY_HINT boolean
set_parameter_property $USE_READ_DATA_VALID HDL_PARAMETER true
set_parameter_property $USE_READ_DATA_VALID GROUP "Common Port Enables"

add_parameter $USE_WRITE_DATA Integer 1 
set_parameter_property $USE_WRITE_DATA DISPLAY_NAME "Use the writedata signal"
set_parameter_property $USE_WRITE_DATA AFFECTS_ELABORATION true
set_parameter_property $USE_WRITE_DATA DESCRIPTION "Use writedata signal"
set_parameter_property $USE_WRITE_DATA DISPLAY_HINT boolean
set_parameter_property $USE_WRITE_DATA HDL_PARAMETER true
set_parameter_property $USE_WRITE_DATA GROUP "Common Port Enables"

add_parameter $AV_FIX_READ_LATENCY Integer 0 
set_parameter_property $AV_FIX_READ_LATENCY DISPLAY_NAME "Fixed read latency (cycles)"
set_parameter_property $AV_FIX_READ_LATENCY AFFECTS_ELABORATION true
set_parameter_property $AV_FIX_READ_LATENCY HDL_PARAMETER true
set_parameter_property $AV_FIX_READ_LATENCY DESCRIPTION "Cycles of fixed read latency"
set_parameter_property $AV_FIX_READ_LATENCY GROUP "Timing" 

add_parameter $AV_READ_WAIT_TIME Integer 0
set_parameter_property $AV_READ_WAIT_TIME DISPLAY_NAME "Fixed read wait time (cycles)"
set_parameter_property $AV_READ_WAIT_TIME AFFECTS_ELABORATION true
set_parameter_property $AV_READ_WAIT_TIME HDL_PARAMETER true
set_parameter_property $AV_READ_WAIT_TIME DESCRIPTION "Fixed read wait time (cycles)"
set_parameter_property $AV_READ_WAIT_TIME GROUP "Timing" 

add_parameter $AV_WRITE_WAIT_TIME Integer 0 
set_parameter_property $AV_WRITE_WAIT_TIME DISPLAY_NAME "Fixed write wait time (cycles)"
set_parameter_property $AV_WRITE_WAIT_TIME AFFECTS_ELABORATION true
set_parameter_property $AV_WRITE_WAIT_TIME HDL_PARAMETER true
set_parameter_property $AV_WRITE_WAIT_TIME DESCRIPTION "Fixed write wait time (cycles)"
set_parameter_property $AV_WRITE_WAIT_TIME GROUP "Timing" 

add_parameter $REGISTER_WAITREQUEST Integer 0 
set_parameter_property $REGISTER_WAITREQUEST DISPLAY_NAME "Registered waitrequest"
set_parameter_property $REGISTER_WAITREQUEST AFFECTS_ELABORATION true
set_parameter_property $REGISTER_WAITREQUEST HDL_PARAMETER true
set_parameter_property $REGISTER_WAITREQUEST DESCRIPTION "Add one pipeline stage to the waitrequest to improve timing."
set_parameter_property $REGISTER_WAITREQUEST DISPLAY_HINT boolean
set_parameter_property $REGISTER_WAITREQUEST GROUP "Timing" 

add_parameter $AV_REGISTERINCOMINGSIGNALS Integer 0 
set_parameter_property $AV_REGISTERINCOMINGSIGNALS DISPLAY_NAME "Registered Incoming Signals"
set_parameter_property $AV_REGISTERINCOMINGSIGNALS AFFECTS_ELABORATION true
set_parameter_property $AV_REGISTERINCOMINGSIGNALS HDL_PARAMETER true
set_parameter_property $AV_REGISTERINCOMINGSIGNALS DESCRIPTION "Indicate that incoming signals come from register."
set_parameter_property $AV_REGISTERINCOMINGSIGNALS DISPLAY_HINT boolean
set_parameter_property $AV_REGISTERINCOMINGSIGNALS GROUP "Timing"

add_parameter $ADDRESS_UNITS String "SYMBOLS"
set_parameter_property $ADDRESS_UNITS DISPLAY_NAME "Set master interface address type to symbols or words"
set_parameter_property $ADDRESS_UNITS AFFECTS_ELABORATION true
set_parameter_property $ADDRESS_UNITS DESCRIPTION "Set master interface address type to symbols or words. Only support SYMBOLS."
set_parameter_property $ADDRESS_UNITS ALLOWED_RANGES {"SYMBOLS"}
set_parameter_property $ADDRESS_UNITS HDL_PARAMETER false
set_parameter_property $ADDRESS_UNITS GROUP "Interface Address Type"

add_parameter COMPONENT_NAME STRING "dut" "The name of the component that contains the interface"
set_parameter_property COMPONENT_NAME DEFAULT_VALUE "dut"
set_parameter_property COMPONENT_NAME DISPLAY_NAME COMPONENT_NAME
set_parameter_property COMPONENT_NAME TYPE STRING
set_parameter_property COMPONENT_NAME UNITS None
set_parameter_property COMPONENT_NAME DESCRIPTION "The name of the component that contains the interface"
set_parameter_property COMPONENT_NAME HDL_PARAMETER true

set parameter_list [get_parameters]
foreach parameter $parameter_list {
   set_parameter_property $parameter AFFECTS_GENERATION false
}


#---------------------------------------------------------------------
proc validate {} {   }

#------------------------------------------------------------------------------
proc elaborate {} {
    global KI_AV_ADDRESS_W          
    global KI_AV_SYMBOL_W           
    global KI_AV_NUMSYMBOLS         
    global KI_USE_WAIT_REQUEST

    global MBD_AV_ADDRESS_W          
    global MBD_AV_SYMBOL_W           
    global MBD_AV_NUMSYMBOLS         
    global MBD_AV_BURSTSIZE
    global MBD_USE_WAIT_REQUEST

    global USE_READ                
    global USE_WRITE               
    global USE_ADDRESS             
    global USE_BYTE_ENABLE         
    global USE_READ_DATA           
    global USE_READ_DATA_VALID     
    global USE_WRITE_DATA          

    global AV_FIX_READ_LATENCY
    global AV_READ_WAIT_TIME   
    global AV_WRITE_WAIT_TIME  
    global REGISTER_WAITREQUEST
    global AV_REGISTERINCOMINGSIGNALS
    
    global ADDRESS_UNITS

    set KI_AV_ADDRESS_W_VALUE             [ get_parameter $KI_AV_ADDRESS_W ]
    set KI_AV_SYMBOL_W_VALUE              [ get_parameter $KI_AV_SYMBOL_W ] 
    set KI_AV_NUMSYMBOLS_VALUE            [ get_parameter $KI_AV_NUMSYMBOLS ]
    set KI_USE_WAIT_REQUEST_VALUE         [ get_parameter $KI_USE_WAIT_REQUEST ]  

    set MBD_AV_ADDRESS_W_VALUE            [ get_parameter $MBD_AV_ADDRESS_W ]
    set MBD_AV_SYMBOL_W_VALUE             [ get_parameter $MBD_AV_SYMBOL_W ] 
    set MBD_AV_NUMSYMBOLS_VALUE           [ get_parameter $MBD_AV_NUMSYMBOLS ]
    set MBD_AV_BURSTSIZE_VALUE            [ get_parameter $MBD_AV_BURSTSIZE ]
    set MBD_USE_WAIT_REQUEST_VALUE        [ get_parameter $MBD_USE_WAIT_REQUEST ]  

    set USE_READ_VALUE                    [ get_parameter $USE_READ ]  
    set USE_WRITE_VALUE                   [ get_parameter $USE_WRITE ] 
    set USE_ADDRESS_VALUE                 [ get_parameter $USE_ADDRESS ] 
    set USE_BYTE_ENABLE_VALUE             [ get_parameter $USE_BYTE_ENABLE ]
    set USE_READ_DATA_VALUE               [ get_parameter $USE_READ_DATA ]
    set USE_READ_DATA_VALID_VALUE         [ get_parameter $USE_READ_DATA_VALID ]
    set USE_WRITE_DATA_VALUE              [ get_parameter $USE_WRITE_DATA ]       

    set AV_FIX_READ_LATENCY_VALUE         [ get_parameter $AV_FIX_READ_LATENCY ] 
    set AV_READ_WAIT_TIME_VALUE           [ get_parameter $AV_READ_WAIT_TIME ]  
    set AV_WRITE_WAIT_TIME_VALUE          [ get_parameter $AV_WRITE_WAIT_TIME ]    
    set REGISTER_WAITREQUEST_VALUE        [ get_parameter $REGISTER_WAITREQUEST ] 
    set AV_REGISTERINCOMINGSIGNALS_VALUE  [ get_parameter $AV_REGISTERINCOMINGSIGNALS ] 

    set KI_AV_DATA_W_VALUE                [ expr {$KI_AV_SYMBOL_W_VALUE * $KI_AV_NUMSYMBOLS_VALUE}]
    set MBD_AV_DATA_W_VALUE               [ expr {$MBD_AV_SYMBOL_W_VALUE * $MBD_AV_NUMSYMBOLS_VALUE}]
    set AV_TRANSACTIONID_W_VALUE          8

    set ADDRESS_UNITS_VALUE               [ get_parameter $ADDRESS_UNITS ]

    set MBD_AV_BURSTCOUNT_W_VALUE         [ expr { int( [ log 2 $MBD_AV_BURSTSIZE_VALUE ] ) + 1 } ]

    # Interface Names
    #---------------------------------------------------------------------
    set CLOCK_INTERFACE      "clock"
    set RESET_INTERFACE      "reset"
    set KI_MASTER_INTERFACE  "m_ki"
    set MBD_MASTER_INTERFACE "m_mbd"

    #---------------------------------------------------------------------
    # Clock-Reset connection point
    #---------------------------------------------------------------------
    add_interface $CLOCK_INTERFACE clock end
    add_interface_port $CLOCK_INTERFACE clock clk Input 1

    add_interface $RESET_INTERFACE reset end
    set_interface_property $RESET_INTERFACE associatedClock clock    
    add_interface_port $RESET_INTERFACE reset_n reset_n Input 1
    
    #---------------------------------------------------------------------
    #  Avalon Master connection point
    #---------------------------------------------------------------------
    add_interface $KI_MASTER_INTERFACE avalon master
    add_interface $MBD_MASTER_INTERFACE avalon master

    # Interface Properties
    #---------------------------------------------------------------------
    set_interface_property $KI_MASTER_INTERFACE ENABLED true
    set_interface_property $KI_MASTER_INTERFACE ASSOCIATED_CLOCK $CLOCK_INTERFACE
    set_interface_property $KI_MASTER_INTERFACE addressUnits $ADDRESS_UNITS_VALUE

    set_interface_property $MBD_MASTER_INTERFACE ENABLED true
    set_interface_property $MBD_MASTER_INTERFACE ASSOCIATED_CLOCK $CLOCK_INTERFACE
    set_interface_property $MBD_MASTER_INTERFACE addressUnits $ADDRESS_UNITS_VALUE


    if {$KI_USE_WAIT_REQUEST_VALUE == 0 || $MBD_USE_WAIT_REQUEST_VALUE == 0} {
        set_parameter_property $AV_READ_WAIT_TIME  ENABLED true
        set_parameter_property $AV_WRITE_WAIT_TIME ENABLED true
    } else {
        set_parameter_property $AV_READ_WAIT_TIME  ENABLED false
        set_parameter_property $AV_WRITE_WAIT_TIME ENABLED false
    }

    set_interface_property $KI_MASTER_INTERFACE  maximumPendingWriteTransactions 0
    set_interface_property $KI_MASTER_INTERFACE  maximumPendingReadTransactions 0
    set_interface_property $MBD_MASTER_INTERFACE maximumPendingWriteTransactions 0
    set_interface_property $MBD_MASTER_INTERFACE maximumPendingReadTransactions 0
    
    if {$USE_READ_DATA_VALID_VALUE > 0} {
        set_interface_property $KI_MASTER_INTERFACE readLatency 0
        set_interface_property $MBD_MASTER_INTERFACE readLatency 0
        set_parameter_property $AV_FIX_READ_LATENCY ENABLED false
    } else {
        set_interface_property $KI_MASTER_INTERFACE readLatency $AV_FIX_READ_LATENCY_VALUE
        set_interface_property $MBD_MASTER_INTERFACE readLatency $AV_FIX_READ_LATENCY_VALUE
        set_parameter_property $AV_FIX_READ_LATENCY ENABLED true
    }

    # Interface Ports
    #---------------------------------------------------------------------
    add_interface_port $KI_MASTER_INTERFACE ki_avm_writedata writedata Output $KI_AV_DATA_W_VALUE
    set_port_property ki_avm_writedata VHDL_TYPE STD_LOGIC_VECTOR
    # kernel interface side does not support burstsize > 1
    add_interface_port $KI_MASTER_INTERFACE ki_avm_burstcount burstcount Output 1
    set_port_property ki_avm_burstcount VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $KI_MASTER_INTERFACE ki_avm_readdata readdata Input $KI_AV_DATA_W_VALUE
    set_port_property ki_avm_readdata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $KI_MASTER_INTERFACE ki_avm_address address Output $KI_AV_ADDRESS_W_VALUE
    set_port_property ki_avm_address VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $KI_MASTER_INTERFACE ki_avm_waitrequest waitrequest Input 1
    add_interface_port $KI_MASTER_INTERFACE ki_avm_write write Output 1
    add_interface_port $KI_MASTER_INTERFACE ki_avm_read read Output 1
    add_interface_port $KI_MASTER_INTERFACE ki_avm_byteenable byteenable Output $KI_AV_NUMSYMBOLS_VALUE
    set_port_property ki_avm_byteenable VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $KI_MASTER_INTERFACE ki_avm_readdatavalid readdatavalid Input 1

    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_writedata writedata Output $MBD_AV_DATA_W_VALUE
    set_port_property mbd_avm_writedata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_burstcount burstcount Output $MBD_AV_BURSTCOUNT_W_VALUE
    set_port_property mbd_avm_burstcount VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_readdata readdata Input $MBD_AV_DATA_W_VALUE
    set_port_property mbd_avm_readdata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_address address Output $MBD_AV_ADDRESS_W_VALUE
    set_port_property mbd_avm_address VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_waitrequest waitrequest Input 1
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_write write Output 1
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_read read Output 1
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_byteenable byteenable Output $MBD_AV_NUMSYMBOLS_VALUE
    set_port_property mbd_avm_byteenable VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MBD_MASTER_INTERFACE mbd_avm_readdatavalid readdatavalid Input 1


   # Terminate unused ports 
   #---------------------------------------------------------------------    

    if {$USE_WRITE_VALUE == 0} {
        set_port_property ki_avm_write             TERMINATION 1  
        set_port_property ki_avm_write             TERMINATION_VALUE 0
        set_port_property mbd_avm_write            TERMINATION 1  
        set_port_property mbd_avm_write            TERMINATION_VALUE 0
    }
    if {$USE_READ_VALUE == 0} {
        set_port_property ki_avm_read              TERMINATION 1
        set_port_property ki_avm_read              TERMINATION_VALUE 0
        set_port_property mbd_avm_read             TERMINATION 1
        set_port_property mbd_avm_read             TERMINATION_VALUE 0
    }
    if {$USE_ADDRESS_VALUE == 0} {
        set_port_property ki_avm_address           TERMINATION 1
        set_port_property ki_avm_address           TERMINATION_VALUE 0
        set_parameter_property $KI_AV_ADDRESS_W ENABLED false
        set_port_property mbd_avm_address          TERMINATION 1
        set_port_property mbd_avm_address          TERMINATION_VALUE 0
        set_parameter_property $MBD_AV_ADDRESS_W ENABLED false
    } else {
        set_parameter_property $KI_AV_ADDRESS_W ENABLED true
        set_parameter_property $MBD_AV_ADDRESS_W ENABLED true
    }
    if {$USE_BYTE_ENABLE_VALUE == 0} {
        set_port_property ki_avm_byteenable        TERMINATION 1
        set_port_property ki_avm_byteenable        TERMINATION_VALUE 0xffffffff
        set_port_property mbd_avm_byteenable       TERMINATION 1
        set_port_property mbd_avm_byteenable       TERMINATION_VALUE 0xffffffff
    }

    if {$USE_READ_DATA_VALUE == 0 } {
        set_port_property ki_avm_readdata          TERMINATION 1
        set_port_property mbd_avm_readdata         TERMINATION 1
    }
    if {$USE_READ_DATA_VALID_VALUE == 0} {
        set_port_property ki_avm_readdatavalid     TERMINATION 1
        set_port_property mbd_avm_readdatavalid    TERMINATION 1
    }
    if {$USE_WRITE_DATA_VALUE == 0 } {
        set_port_property ki_avm_writedata         TERMINATION 1
        set_port_property ki_avm_writedata         TERMINATION_VALUE 0
        set_port_property mbd_avm_writedata        TERMINATION 1
        set_port_property mbd_avm_writedata        TERMINATION_VALUE 0
    }

}


# vim:set filetype=tcl:
