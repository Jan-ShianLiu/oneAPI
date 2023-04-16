package require -exact qsys 14.0

set_module_property NAME                         aoc_sim_mm_slave_dpi_bfm
set_module_property GROUP                        "OpenCL Simulation"
set_module_property DISPLAY_NAME                 "OpenCL Simulation Avalon-MM Slave DPI BFM"
set_module_property DESCRIPTION                  "Interface between Simulator MMD and kernel system global memory interconnect"
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
set_fileset_property sim_verilog top_level aoc_sim_mm_slave_dpi_bfm

add_fileset quartus_synth QUARTUS_SYNTH quartus_synth_proc

proc get_qsys_verification_ip_path {} {
   set QUARTUS_ROOTDIR $::env(QUARTUS_ROOTDIR)
   puts $QUARTUS_ROOTDIR         
   append QSYS_VERIFICATION_IP $QUARTUS_ROOTDIR "/../ip/altera/sopc_builder_ip/verification/"
   puts $QSYS_VERIFICATION_IP
   return $QSYS_VERIFICATION_IP
}

# SIM_VERILOG generation callback procedure
proc sim_verilog_proc {aoc_sim_mm_slave_dpi_bfm} {
   set QSYS_VERIFICATION_IP [get_qsys_verification_ip_path]

   add_fileset_file verbosity_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/verbosity_pkg.sv"
   add_fileset_file avalon_mm_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_mm_pkg.sv"
   add_fileset_file avalon_utilities_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_utilities_pkg.sv"

   add_fileset_file altera_avalon_mm_slave_bfm.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/altera_avalon_mm_slave_bfm/altera_avalon_mm_slave_bfm.sv"
   add_fileset_file aoc_sim_mm_slave_dpi_bfm.sv SYSTEM_VERILOG PATH aoc_sim_mm_slave_dpi_bfm.sv
   
   set_fileset_file_attribute verbosity_pkg.sv        COMMON_SYSTEMVERILOG_PACKAGE avalon_lvip_verbosity_pkg
   set_fileset_file_attribute avalon_mm_pkg.sv        COMMON_SYSTEMVERILOG_PACKAGE avalon_vip_avalon_mm_pkg
   set_fileset_file_attribute avalon_utilities_pkg.sv COMMON_SYSTEMVERILOG_PACKAGE avalon_vip_avalon_utilities_pkg
}

# This module is not synthesizable
proc quartus_synth_proc { } { }

#------------------------------------------------------------------------------
# Parameters
#------------------------------------------------------------------------------
set AV_ADDRESS_W              "AV_ADDRESS_W"
set AV_SYMBOL_W               "AV_SYMBOL_W" 
set AV_NUMSYMBOLS             "AV_NUMSYMBOLS"
set AV_BURSTCOUNT_W           "AV_BURSTCOUNT_W" 
set AV_WRITERESPONSE_W        "AV_WRITERESPONSE_W"
set AV_READRESPONSE_W         "AV_READRESPONSE_W"

set USE_READ                  "USE_READ"
set USE_WRITE                 "USE_WRITE"
set USE_ADDRESS               "USE_ADDRESS"
set USE_BYTE_ENABLE           "USE_BYTE_ENABLE"
set USE_BURSTCOUNT            "USE_BURSTCOUNT"
set USE_READ_DATA             "USE_READ_DATA"
set USE_READ_DATA_VALID       "USE_READ_DATA_VALID"
set USE_WRITE_DATA            "USE_WRITE_DATA"
set USE_BEGIN_TRANSFER        "USE_BEGIN_TRANSFER"
set USE_BEGIN_BURST_TRANSFER  "USE_BEGIN_BURST_TRANSFER"
set USE_WAIT_REQUEST          "USE_WAIT_REQUEST"
set USE_ARBITERLOCK           "USE_ARBITERLOCK"
set USE_LOCK                  "USE_LOCK"
set USE_DEBUGACCESS           "USE_DEBUGACCESS"

set USE_CLKEN                 "USE_CLKEN"

set ASSERT_HIGH_WAITREQUEST   "ASSERT_HIGH_WAITREQUEST"
set ASSERT_HIGH_READ          "ASSERT_HIGH_READ"  
set ASSERT_HIGH_WRITE         "ASSERT_HIGH_WRITE"         
set ASSERT_HIGH_BYTEENABLE    "ASSERT_HIGH_BYTEENABLE"
set ASSERT_HIGH_READDATAVALID "ASSERT_HIGH_READDATAVALID"
set ASSERT_HIGH_ARBITERLOCK   "ASSERT_HIGH_ARBITERLOCK" 
set ASSERT_HIGH_LOCK          "ASSERT_HIGH_LOCK" 

set AV_BURST_LINEWRAP         "AV_BURST_LINEWRAP" 
set AV_BURST_BNDR_ONLY        "AV_BURST_BNDR_ONLY"

set AV_MAX_PENDING_READS      "AV_MAX_PENDING_READS"
set AV_MAX_PENDING_WRITES     "AV_MAX_PENDING_WRITES"
set AV_FIX_READ_LATENCY       "AV_FIX_READ_LATENCY"
set AV_READ_WAIT_TIME         "AV_READ_WAIT_TIME"
set AV_WRITE_WAIT_TIME        "AV_WRITE_WAIT_TIME"
set REGISTER_WAITREQUEST      "REGISTER_WAITREQUEST"

set ADDRESS_UNITS                "ADDRESS_UNITS"
set AV_REGISTERINCOMINGSIGNALS   "AV_REGISTERINCOMINGSIGNALS"
set AV_WAITREQUEST_ALLOWANCE     "AV_WAITREQUEST_ALLOWANCE"

#---------------------------------------------------------------------
add_parameter $AV_ADDRESS_W Integer 32
set_parameter_property $AV_ADDRESS_W DISPLAY_NAME "Address width"
set_parameter_property $AV_ADDRESS_W AFFECTS_ELABORATION true
set_parameter_property $AV_ADDRESS_W DESCRIPTION "The width of the address signal."
set_parameter_property $AV_ADDRESS_W ALLOWED_RANGES {1:64}
set_parameter_property $AV_ADDRESS_W HDL_PARAMETER true
set_parameter_property $AV_ADDRESS_W GROUP "Port Widths"

add_parameter $AV_SYMBOL_W Integer 8
set_parameter_property $AV_SYMBOL_W DISPLAY_NAME "Symbol width"
set_parameter_property $AV_SYMBOL_W AFFECTS_ELABORATION true
set_parameter_property $AV_SYMBOL_W DESCRIPTION "The width of an individual symbol. The default is an 8 bit Byte."
set_parameter_property $AV_SYMBOL_W ALLOWED_RANGES {1:1024}
set_parameter_property $AV_SYMBOL_W HDL_PARAMETER true
set_parameter_property $AV_SYMBOL_W GROUP "Port Widths"

add_parameter $AV_NUMSYMBOLS Integer 4
set_parameter_property $AV_NUMSYMBOLS DISPLAY_NAME "Number of Symbols"
set_parameter_property $AV_NUMSYMBOLS AFFECTS_ELABORATION true
set_parameter_property $AV_NUMSYMBOLS DESCRIPTION "The number of symbols in a word. The default is 4 bytes per word."
set_parameter_property $AV_NUMSYMBOLS ALLOWED_RANGES {1,2,4,8,16,32,64,128}
set_parameter_property $AV_NUMSYMBOLS HDL_PARAMETER true
set_parameter_property $AV_SYMBOL_W GROUP "Port Widths"

add_parameter $AV_BURSTCOUNT_W Integer 3
set_parameter_property $AV_BURSTCOUNT_W DISPLAY_NAME "Burstcount width"
set_parameter_property $AV_BURSTCOUNT_W AFFECTS_ELABORATION true
set_parameter_property $AV_BURSTCOUNT_W DESCRIPTION "The width of the Burstcount port determines the maximum burst length that can be specified for a transaction."
set_parameter_property $AV_BURSTCOUNT_W ALLOWED_RANGES {1:32}
set_parameter_property $AV_BURSTCOUNT_W HDL_PARAMETER true
set_parameter_property $AV_SYMBOL_W GROUP "Port Widths"

add_parameter $AV_READRESPONSE_W Integer 8
set_parameter_property $AV_READRESPONSE_W DISPLAY_NAME "Read Response width"
set_parameter_property $AV_READRESPONSE_W AFFECTS_ELABORATION true
set_parameter_property $AV_READRESPONSE_W DESCRIPTION "This is the word length in bits of the read response port."
set_parameter_property $AV_READRESPONSE_W ALLOWED_RANGES {1:1024}
set_parameter_property $AV_READRESPONSE_W HDL_PARAMETER true
set_parameter_property $AV_READRESPONSE_W GROUP "Port Widths"
set_parameter_property $AV_READRESPONSE_W VISIBLE false

add_parameter $AV_WRITERESPONSE_W Integer 8
set_parameter_property $AV_WRITERESPONSE_W DISPLAY_NAME "Write Response width"
set_parameter_property $AV_WRITERESPONSE_W AFFECTS_ELABORATION true
set_parameter_property $AV_WRITERESPONSE_W DESCRIPTION "This is the word length in bits of the read response port."
set_parameter_property $AV_WRITERESPONSE_W ALLOWED_RANGES {1:1024}
set_parameter_property $AV_WRITERESPONSE_W HDL_PARAMETER true
set_parameter_property $AV_WRITERESPONSE_W GROUP "Port Widths"
set_parameter_property $AV_WRITERESPONSE_W VISIBLE false

add_parameter $USE_READ Integer 1 
set_parameter_property $USE_READ DISPLAY_NAME "Use the read signal"
set_parameter_property $USE_READ AFFECTS_ELABORATION true
set_parameter_property $USE_READ DESCRIPTION "Use the read signal"
set_parameter_property $USE_READ DISPLAY_HINT boolean
set_parameter_property $USE_READ HDL_PARAMETER true
set_parameter_property $USE_READ GROUP "Port Enables"

add_parameter $USE_WRITE Integer 1 
set_parameter_property $USE_WRITE DISPLAY_NAME "Use the write signal"
set_parameter_property $USE_WRITE AFFECTS_ELABORATION true
set_parameter_property $USE_WRITE DESCRIPTION "Use the write signal"
set_parameter_property $USE_WRITE DISPLAY_HINT boolean
set_parameter_property $USE_WRITE HDL_PARAMETER true
set_parameter_property $USE_WRITE GROUP "Port Enables"

add_parameter $USE_ADDRESS Integer 1 
set_parameter_property $USE_ADDRESS DISPLAY_NAME "Use the address signal"
set_parameter_property $USE_ADDRESS AFFECTS_ELABORATION true
set_parameter_property $USE_ADDRESS DESCRIPTION "Use the address signal"
set_parameter_property $USE_ADDRESS DISPLAY_HINT boolean
set_parameter_property $USE_ADDRESS HDL_PARAMETER true
set_parameter_property $USE_ADDRESS GROUP "Port Enables"

add_parameter $USE_BYTE_ENABLE Integer 1 
set_parameter_property $USE_BYTE_ENABLE DISPLAY_NAME "Use the byteenable signal"
set_parameter_property $USE_BYTE_ENABLE AFFECTS_ELABORATION true
set_parameter_property $USE_BYTE_ENABLE DESCRIPTION "Use the byteenable signal"
set_parameter_property $USE_BYTE_ENABLE DISPLAY_HINT boolean
set_parameter_property $USE_BYTE_ENABLE HDL_PARAMETER true
set_parameter_property $USE_BYTE_ENABLE GROUP "Port Enables"

add_parameter $USE_BURSTCOUNT Integer 0 
set_parameter_property $USE_BURSTCOUNT DISPLAY_NAME "Use the burstcount signal"
set_parameter_property $USE_BURSTCOUNT AFFECTS_ELABORATION true
set_parameter_property $USE_BURSTCOUNT DESCRIPTION "Use the burstcount signal"
set_parameter_property $USE_BURSTCOUNT DISPLAY_HINT boolean
set_parameter_property $USE_BURSTCOUNT HDL_PARAMETER true
set_parameter_property $USE_BURSTCOUNT GROUP "Port Enables"

add_parameter $USE_READ_DATA Integer 1 
set_parameter_property $USE_READ_DATA DISPLAY_NAME "Use the readdata signal"
set_parameter_property $USE_READ_DATA AFFECTS_ELABORATION true
set_parameter_property $USE_READ_DATA DESCRIPTION "Use the readdata signal"
set_parameter_property $USE_READ_DATA DISPLAY_HINT boolean
set_parameter_property $USE_READ_DATA HDL_PARAMETER true
set_parameter_property $USE_READ_DATA GROUP "Port Enables"

add_parameter $USE_READ_DATA_VALID Integer 0
set_parameter_property $USE_READ_DATA_VALID DISPLAY_NAME "Use the readdatavalid signal"
set_parameter_property $USE_READ_DATA_VALID AFFECTS_ELABORATION true
set_parameter_property $USE_READ_DATA_VALID DESCRIPTION "Use the readdatavalid signal"
set_parameter_property $USE_READ_DATA_VALID DISPLAY_HINT boolean
set_parameter_property $USE_READ_DATA_VALID HDL_PARAMETER true
set_parameter_property $USE_READ_DATA_VALID GROUP "Port Enables"

add_parameter $USE_WRITE_DATA Integer 1 
set_parameter_property $USE_WRITE_DATA DISPLAY_NAME "Use the writedata signal"
set_parameter_property $USE_WRITE_DATA AFFECTS_ELABORATION true
set_parameter_property $USE_WRITE_DATA DESCRIPTION "Use writedata signal"
set_parameter_property $USE_WRITE_DATA DISPLAY_HINT boolean
set_parameter_property $USE_WRITE_DATA HDL_PARAMETER true
set_parameter_property $USE_WRITE_DATA GROUP "Port Enables"

add_parameter $USE_BEGIN_TRANSFER Integer 0
set_parameter_property $USE_BEGIN_TRANSFER DISPLAY_NAME "Use the begintransfer signal"
set_parameter_property $USE_BEGIN_TRANSFER AFFECTS_ELABORATION true
set_parameter_property $USE_BEGIN_TRANSFER DESCRIPTION "Use the begintransfer signal"
set_parameter_property $USE_BEGIN_TRANSFER DISPLAY_HINT boolean
set_parameter_property $USE_BEGIN_TRANSFER HDL_PARAMETER true
set_parameter_property $USE_BEGIN_TRANSFER GROUP "Port Enables"

add_parameter $USE_BEGIN_BURST_TRANSFER Integer 0
set_parameter_property $USE_BEGIN_BURST_TRANSFER DISPLAY_NAME "Use beginbursttransfer signal"
set_parameter_property $USE_BEGIN_BURST_TRANSFER AFFECTS_ELABORATION true
set_parameter_property $USE_BEGIN_BURST_TRANSFER DESCRIPTION "Use beginbursttransfer signal"
set_parameter_property $USE_BEGIN_BURST_TRANSFER DISPLAY_HINT boolean
set_parameter_property $USE_BEGIN_BURST_TRANSFER HDL_PARAMETER true
set_parameter_property $USE_BEGIN_BURST_TRANSFER GROUP "Port Enables"

add_parameter $USE_ARBITERLOCK Integer 0 
set_parameter_property $USE_ARBITERLOCK DISPLAY_NAME "Use the arbiterlock signal"
set_parameter_property $USE_ARBITERLOCK AFFECTS_ELABORATION true
set_parameter_property $USE_ARBITERLOCK DESCRIPTION "Use the arbiterlock signal"
set_parameter_property $USE_ARBITERLOCK DISPLAY_HINT boolean
set_parameter_property $USE_ARBITERLOCK HDL_PARAMETER false
set_parameter_property $USE_ARBITERLOCK GROUP "Port Enables"

add_parameter $USE_LOCK Integer 0 
set_parameter_property $USE_LOCK DISPLAY_NAME "Use the lock signal"
set_parameter_property $USE_LOCK AFFECTS_ELABORATION true
set_parameter_property $USE_LOCK DESCRIPTION "Use the lock signal"
set_parameter_property $USE_LOCK DISPLAY_HINT boolean
set_parameter_property $USE_LOCK HDL_PARAMETER false
set_parameter_property $USE_LOCK GROUP "Port Enables"

add_parameter $USE_DEBUGACCESS Integer 0 
set_parameter_property $USE_DEBUGACCESS DISPLAY_NAME "Use the debugaccess signal"
set_parameter_property $USE_DEBUGACCESS AFFECTS_ELABORATION true
set_parameter_property $USE_DEBUGACCESS DESCRIPTION "Use the debugaccess signal"
set_parameter_property $USE_DEBUGACCESS DISPLAY_HINT boolean
set_parameter_property $USE_DEBUGACCESS HDL_PARAMETER false
set_parameter_property $USE_DEBUGACCESS GROUP "Port Enables"

add_parameter $USE_WAIT_REQUEST Integer 0
set_parameter_property $USE_WAIT_REQUEST DISPLAY_NAME "Use the waitrequest signal"
set_parameter_property $USE_WAIT_REQUEST AFFECTS_ELABORATION true
set_parameter_property $USE_WAIT_REQUEST DESCRIPTION "Use the waitrequest signal"
set_parameter_property $USE_WAIT_REQUEST DISPLAY_HINT boolean
set_parameter_property $USE_WAIT_REQUEST HDL_PARAMETER true
set_parameter_property $USE_WAIT_REQUEST GROUP "Port Enables"

add_parameter $USE_CLKEN Integer 0 
set_parameter_property $USE_CLKEN DISPLAY_NAME "Use the clken signals"
set_parameter_property $USE_CLKEN AFFECTS_ELABORATION true
set_parameter_property $USE_CLKEN DESCRIPTION "Use the tightly coupled memory signals"
set_parameter_property $USE_CLKEN DISPLAY_HINT boolean
set_parameter_property $USE_CLKEN HDL_PARAMETER true
set_parameter_property $USE_CLKEN GROUP "Port Enables"

add_parameter $ASSERT_HIGH_WAITREQUEST Integer 1 
set_parameter_property $ASSERT_HIGH_WAITREQUEST DISPLAY_NAME "Assert waitrequest high"
set_parameter_property $ASSERT_HIGH_WAITREQUEST AFFECTS_ELABORATION true
set_parameter_property $ASSERT_HIGH_WAITREQUEST DESCRIPTION "Assert waitrequest high"
set_parameter_property $ASSERT_HIGH_WAITREQUEST DISPLAY_HINT boolean
set_parameter_property $ASSERT_HIGH_WAITREQUEST GROUP "Port Polarity"

add_parameter $ASSERT_HIGH_READ Integer 1 
set_parameter_property $ASSERT_HIGH_READ DISPLAY_NAME "Assert read high"
set_parameter_property $ASSERT_HIGH_READ AFFECTS_ELABORATION true
set_parameter_property $ASSERT_HIGH_READ DESCRIPTION "Assert read high"
set_parameter_property $ASSERT_HIGH_READ DISPLAY_HINT boolean
set_parameter_property $ASSERT_HIGH_READ GROUP "Port Polarity"

add_parameter $ASSERT_HIGH_WRITE Integer 1 
set_parameter_property $ASSERT_HIGH_WRITE DISPLAY_NAME "Assert write high"
set_parameter_property $ASSERT_HIGH_WRITE AFFECTS_ELABORATION true
set_parameter_property $ASSERT_HIGH_WRITE DESCRIPTION "Assert write high"
set_parameter_property $ASSERT_HIGH_WRITE DISPLAY_HINT boolean
set_parameter_property $ASSERT_HIGH_WRITE GROUP "Port Polarity"

add_parameter $ASSERT_HIGH_BYTEENABLE Integer 1 
set_parameter_property $ASSERT_HIGH_BYTEENABLE DISPLAY_NAME "Assert byteenable high"
set_parameter_property $ASSERT_HIGH_BYTEENABLE AFFECTS_ELABORATION true
set_parameter_property $ASSERT_HIGH_BYTEENABLE DESCRIPTION "Assert byteenable high"
set_parameter_property $ASSERT_HIGH_BYTEENABLE DISPLAY_HINT boolean
set_parameter_property $ASSERT_HIGH_BYTEENABLE GROUP "Port Polarity"

add_parameter $ASSERT_HIGH_READDATAVALID Integer 1 
set_parameter_property $ASSERT_HIGH_READDATAVALID DISPLAY_NAME "Assert readdatavalid high"
set_parameter_property $ASSERT_HIGH_READDATAVALID AFFECTS_ELABORATION true
set_parameter_property $ASSERT_HIGH_READDATAVALID DESCRIPTION "Assert readdatavalid high"
set_parameter_property $ASSERT_HIGH_READDATAVALID DISPLAY_HINT boolean
set_parameter_property $ASSERT_HIGH_READDATAVALID GROUP "Port Polarity"

add_parameter $ASSERT_HIGH_ARBITERLOCK Integer 1 
set_parameter_property $ASSERT_HIGH_ARBITERLOCK DISPLAY_NAME "Assert arbiterlock high"
set_parameter_property $ASSERT_HIGH_ARBITERLOCK AFFECTS_ELABORATION true
set_parameter_property $ASSERT_HIGH_ARBITERLOCK DESCRIPTION "Assert arbiterlock high"
set_parameter_property $ASSERT_HIGH_ARBITERLOCK DISPLAY_HINT boolean
set_parameter_property $ASSERT_HIGH_ARBITERLOCK GROUP "Port Polarity"

add_parameter $ASSERT_HIGH_LOCK Integer 1 
set_parameter_property $ASSERT_HIGH_LOCK DISPLAY_NAME "Assert lock high"
set_parameter_property $ASSERT_HIGH_LOCK AFFECTS_ELABORATION true
set_parameter_property $ASSERT_HIGH_LOCK DESCRIPTION "Assert lock high"
set_parameter_property $ASSERT_HIGH_LOCK DISPLAY_HINT boolean
set_parameter_property $ASSERT_HIGH_LOCK GROUP "Port Polarity"

add_parameter $AV_BURST_LINEWRAP Integer 1 
set_parameter_property $AV_BURST_LINEWRAP DISPLAY_NAME "Linewrap bursts"
set_parameter_property $AV_BURST_LINEWRAP AFFECTS_ELABORATION true
set_parameter_property $AV_BURST_LINEWRAP DESCRIPTION "Linewrap bursts"
set_parameter_property $AV_BURST_LINEWRAP DISPLAY_HINT boolean
set_parameter_property $AV_BURST_LINEWRAP HDL_PARAMETER true
set_parameter_property $AV_BURST_LINEWRAP GROUP "Burst Attributes"

add_parameter $AV_BURST_BNDR_ONLY Integer 1 
set_parameter_property $AV_BURST_BNDR_ONLY DISPLAY_NAME "Burst on Burst Boundaries only"
set_parameter_property $AV_BURST_BNDR_ONLY AFFECTS_ELABORATION true
set_parameter_property $AV_BURST_BNDR_ONLY DESCRIPTION "Burst on boundaries only"
set_parameter_property $AV_BURST_BNDR_ONLY DISPLAY_HINT boolean
set_parameter_property $AV_BURST_BNDR_ONLY HDL_PARAMETER true
set_parameter_property $AV_BURST_BNDR_ONLY GROUP "Burst Attributes"

add_parameter $AV_MAX_PENDING_READS Integer 0
set_parameter_property $AV_MAX_PENDING_READS DISPLAY_NAME "Maximum pending reads"
set_parameter_property $AV_MAX_PENDING_READS AFFECTS_ELABORATION true
set_parameter_property $AV_MAX_PENDING_READS HDL_PARAMETER true
set_parameter_property $AV_MAX_PENDING_READS DESCRIPTION "Maximum pending read transactions"
set_parameter_property $AV_MAX_PENDING_READS GROUP "Miscellaneous"

add_parameter $AV_MAX_PENDING_WRITES Integer 0
set_parameter_property $AV_MAX_PENDING_WRITES DISPLAY_NAME "Maximum pending writes"
set_parameter_property $AV_MAX_PENDING_WRITES AFFECTS_ELABORATION true
set_parameter_property $AV_MAX_PENDING_WRITES HDL_PARAMETER true
set_parameter_property $AV_MAX_PENDING_WRITES DESCRIPTION "Maximum pending write transactions"
set_parameter_property $AV_MAX_PENDING_WRITES GROUP "Miscellaneous"
set_parameter_property $AV_MAX_PENDING_WRITES VISIBLE false

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

add_parameter $AV_WAITREQUEST_ALLOWANCE Integer 0 
set_parameter_property $AV_WAITREQUEST_ALLOWANCE DISPLAY_NAME "Waitrequest allowance"
set_parameter_property $AV_WAITREQUEST_ALLOWANCE AFFECTS_ELABORATION true
set_parameter_property $AV_WAITREQUEST_ALLOWANCE HDL_PARAMETER true
set_parameter_property $AV_WAITREQUEST_ALLOWANCE DESCRIPTION "The number of transfers memory must accept after the waitrequest signal is asserted."
set_parameter_property $AV_WAITREQUEST_ALLOWANCE GROUP "Timing"

# DO NOT CHANGE THIS VALUE
# Only SYMBOLS is allowed as the IP shift the address based on symbols, not word
add_parameter $ADDRESS_UNITS String "SYMBOLS"
set_parameter_property $ADDRESS_UNITS DISPLAY_NAME "Set slave interface address type to symbols or words"
set_parameter_property $ADDRESS_UNITS AFFECTS_ELABORATION true
set_parameter_property $ADDRESS_UNITS DESCRIPTION "Set slave interface address type to symbols or words. Default value is 'WORDS'"
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
 
add_parameter INTERFACE_ID Integer 0 "The component's master interface this dpi bfm connects to"
set_parameter_property INTERFACE_ID DEFAULT_VALUE 0
set_parameter_property INTERFACE_ID DISPLAY_NAME INTERFACE_ID
set_parameter_property INTERFACE_ID UNITS None
set_parameter_property INTERFACE_ID DESCRIPTION "The component's master interface this dpi bfm connects to"
set_parameter_property INTERFACE_ID HDL_PARAMETER true

set parameter_list [get_parameters]
foreach parameter $parameter_list {
   set_parameter_property $parameter AFFECTS_GENERATION false
}

#---------------------------------------------------------------------
proc validate {} {   }

#------------------------------------------------------------------------------
proc elaborate {} {
    global AV_ADDRESS_W          
    global AV_SYMBOL_W           
    global AV_NUMSYMBOLS         
    global AV_BURSTCOUNT_W            
    global AV_READRESPONSE_W 
    global AV_WRITERESPONSE_W

    global USE_READ                
    global USE_WRITE               
    global USE_ADDRESS             
    global USE_BYTE_ENABLE         
    global USE_BURSTCOUNT              
    global USE_READ_DATA           
    global USE_READ_DATA_VALID     
    global USE_WRITE_DATA          
    global USE_BEGIN_TRANSFER      
    global USE_BEGIN_BURST_TRANSFER
    global USE_WAIT_REQUEST        
    global USE_ARBITERLOCK
    global USE_LOCK
    global USE_DEBUGACCESS
    global USE_TRANSACTIONID
    global USE_WRITERESPONSE
    global USE_READRESPONSE 
    global USE_CLKEN 

    global ASSERT_HIGH_WAITREQUEST  
    global ASSERT_HIGH_READ         
    global ASSERT_HIGH_WRITE        
    global ASSERT_HIGH_BYTEENABLE   
    global ASSERT_HIGH_READDATAVALID
    global ASSERT_HIGH_ARBITERLOCK
    global ASSERT_HIGH_LOCK

    global AV_BURST_LINEWRAP     
    global AV_BURST_BNDR_ONLY 

    global AV_MAX_PENDING_READS
    global AV_MAX_PENDING_WRITES
    global AV_FIX_READ_LATENCY
    global AV_READ_WAIT_TIME   
    global AV_WRITE_WAIT_TIME  
    global REGISTER_WAITREQUEST
    global AV_REGISTERINCOMINGSIGNALS
    global AV_WAITREQUEST_ALLOWANCE
    
    global ADDRESS_UNITS

    set AV_ADDRESS_W_VALUE                [ get_parameter $AV_ADDRESS_W ]
    set AV_SYMBOL_W_VALUE                 [ get_parameter $AV_SYMBOL_W ] 
    set AV_NUMSYMBOLS_VALUE               [ get_parameter $AV_NUMSYMBOLS ]
    set AV_BURSTCOUNT_W_VALUE             [ get_parameter $AV_BURSTCOUNT_W ]
    set AV_READRESPONSE_W_VALUE           [ get_parameter $AV_READRESPONSE_W ]
    set AV_WRITERESPONSE_W_VALUE          [ get_parameter $AV_WRITERESPONSE_W ]
   
    set USE_READ_VALUE                    [ get_parameter $USE_READ ]  
    set USE_WRITE_VALUE                   [ get_parameter $USE_WRITE ] 
    set USE_ADDRESS_VALUE                 [ get_parameter $USE_ADDRESS ] 
    set USE_BYTE_ENABLE_VALUE             [ get_parameter $USE_BYTE_ENABLE ]
    set USE_BURSTCOUNT_VALUE              [ get_parameter $USE_BURSTCOUNT ] 
    set USE_READ_DATA_VALUE               [ get_parameter $USE_READ_DATA ]
    set USE_READ_DATA_VALID_VALUE         [ get_parameter $USE_READ_DATA_VALID ]
    set USE_WRITE_DATA_VALUE              [ get_parameter $USE_WRITE_DATA ]       
    set USE_BEGIN_TRANSFER_VALUE          [ get_parameter $USE_BEGIN_TRANSFER ] 
    set USE_BEGIN_BURST_TRANSFER_VALUE    [ get_parameter $USE_BEGIN_BURST_TRANSFER ]
    set USE_WAIT_REQUEST_VALUE            [ get_parameter $USE_WAIT_REQUEST ]  
    set USE_ARBITERLOCK_VALUE             [ get_parameter $USE_ARBITERLOCK ] 
    set USE_LOCK_VALUE                    [ get_parameter $USE_LOCK ] 
    set USE_DEBUGACCESS_VALUE             [ get_parameter $USE_DEBUGACCESS ] 
    
    set USE_CLKEN_VALUE                   [ get_parameter $USE_CLKEN ] 
   
    set ASSERT_HIGH_WAITREQUEST_VALUE     [ get_parameter $ASSERT_HIGH_WAITREQUEST ]
    set ASSERT_HIGH_READ_VALUE            [ get_parameter $ASSERT_HIGH_READ ]   
    set ASSERT_HIGH_WRITE_VALUE           [ get_parameter $ASSERT_HIGH_WRITE ] 
    set ASSERT_HIGH_BYTEENABLE_VALUE      [ get_parameter $ASSERT_HIGH_BYTEENABLE ]
    set ASSERT_HIGH_READDATAVALID_VALUE   [ get_parameter $ASSERT_HIGH_READDATAVALID ]
    set ASSERT_HIGH_ARBITERLOCK_VALUE     [ get_parameter $ASSERT_HIGH_ARBITERLOCK ]
    set ASSERT_HIGH_LOCK_VALUE            [ get_parameter $ASSERT_HIGH_LOCK ]
    set AV_BURST_LINEWRAP_VALUE           [ get_parameter $AV_BURST_LINEWRAP ] 
    set AV_BURST_BNDR_ONLY_VALUE          [ get_parameter $AV_BURST_BNDR_ONLY ] 
   
    set AV_MAX_PENDING_READS_VALUE        [ get_parameter $AV_MAX_PENDING_READS ] 
    set AV_MAX_PENDING_WRITES_VALUE       [ get_parameter $AV_MAX_PENDING_WRITES ] 
    set AV_FIX_READ_LATENCY_VALUE         [ get_parameter $AV_FIX_READ_LATENCY ] 
    set AV_READ_WAIT_TIME_VALUE           [ get_parameter $AV_READ_WAIT_TIME ]  
    set AV_WRITE_WAIT_TIME_VALUE          [ get_parameter $AV_WRITE_WAIT_TIME ]    
    set REGISTER_WAITREQUEST_VALUE        [ get_parameter $REGISTER_WAITREQUEST ] 
    set AV_REGISTERINCOMINGSIGNALS_VALUE  [ get_parameter $AV_REGISTERINCOMINGSIGNALS ] 
    set AV_WAITREQUEST_ALLOWANCE_VALUE    [ get_parameter $AV_WAITREQUEST_ALLOWANCE ]
    
    set AV_DATA_W_VALUE                   [ expr {$AV_SYMBOL_W_VALUE * $AV_NUMSYMBOLS_VALUE}]
    set AV_TRANSACTIONID_W_VALUE          8
    
    set ADDRESS_UNITS_VALUE               [ get_parameter $ADDRESS_UNITS ]

 
    # Interface Names
    #---------------------------------------------------------------------
    set CLOCK_INTERFACE  "clock"
    set RESET_INTERFACE  "reset"
    set SLAVE_INTERFACE  "s0"

    #---------------------------------------------------------------------
    # Clock-Reset connection point
    #---------------------------------------------------------------------
    add_interface $CLOCK_INTERFACE clock end
    add_interface_port $CLOCK_INTERFACE clock clk Input 1
   
    add_interface $RESET_INTERFACE reset end
    set_interface_property $RESET_INTERFACE associatedClock clock    
    add_interface_port $RESET_INTERFACE reset_n reset_n Input 1
    
    
    #---------------------------------------------------------------------
    #  Avalon Slave connection point
    #---------------------------------------------------------------------
    add_interface $SLAVE_INTERFACE avalon end

    # Interface Properties
    #---------------------------------------------------------------------
    set_interface_property $SLAVE_INTERFACE ENABLED true
    set_interface_property $SLAVE_INTERFACE ASSOCIATED_CLOCK $CLOCK_INTERFACE
    set_interface_property $SLAVE_INTERFACE addressAlignment DYNAMIC
    set_interface_property $SLAVE_INTERFACE holdTime 0
    set_interface_property $SLAVE_INTERFACE isMemoryDevice false
    set_interface_property $SLAVE_INTERFACE isNonVolatileStorage false
    set_interface_property $SLAVE_INTERFACE minimumUninterruptedRunLength 1
    set_interface_property $SLAVE_INTERFACE printableDevice false
    set_interface_property $SLAVE_INTERFACE readWaitTime $AV_READ_WAIT_TIME_VALUE
    set_interface_property $SLAVE_INTERFACE setupTime 0
    set_interface_property $SLAVE_INTERFACE timingUnits Cycles
    set_interface_property $SLAVE_INTERFACE writeWaitTime $AV_WRITE_WAIT_TIME_VALUE
    set_interface_property $SLAVE_INTERFACE addressUnits $ADDRESS_UNITS_VALUE
    set_interface_property $SLAVE_INTERFACE waitRequestAllowance $AV_WAITREQUEST_ALLOWANCE_VALUE 

    if {$USE_WAIT_REQUEST_VALUE == 0} {
        set_parameter_property $AV_READ_WAIT_TIME ENABLED true
        set_parameter_property $AV_WRITE_WAIT_TIME ENABLED true
    } else {
        set_parameter_property $AV_READ_WAIT_TIME ENABLED false
        set_parameter_property $AV_WRITE_WAIT_TIME ENABLED false
    }
    
    if {$USE_WRITE_VALUE == 0} {
        set_interface_property $SLAVE_INTERFACE maximumPendingWriteTransactions 0
        set_parameter_property $AV_MAX_PENDING_WRITES ENABLED false
    } else {
        set_interface_property $SLAVE_INTERFACE maximumPendingWriteTransactions $AV_MAX_PENDING_WRITES_VALUE
        set_parameter_property $AV_MAX_PENDING_WRITES ENABLED true
    }
    
    if {$USE_READ_VALUE == 0} {
        set_interface_property $SLAVE_INTERFACE maximumPendingReadTransactions 0
        set_parameter_property $AV_MAX_PENDING_READS ENABLED false
    } else {
        set_interface_property $SLAVE_INTERFACE maximumPendingReadTransactions $AV_MAX_PENDING_READS_VALUE
        set_parameter_property $AV_MAX_PENDING_READS ENABLED true
    }
    
    if {$USE_READ_DATA_VALID_VALUE > 0} {
        set_interface_property $SLAVE_INTERFACE readLatency 0
        set_parameter_property $AV_FIX_READ_LATENCY ENABLED false
    } else {
        set_interface_property $SLAVE_INTERFACE readLatency $AV_FIX_READ_LATENCY_VALUE
        set_parameter_property $AV_FIX_READ_LATENCY ENABLED true
    }
    

   #  if { $AV_BURST_LINEWRAP_VALUE > 0 } {
   #      set_interface_property $SLAVE_INTERFACE linewrapBursts true
   #  } else {
   #      set_interface_property $SLAVE_INTERFACE linewrapBursts false
   #  }
   #  if { $AV_BURST_BNDR_ONLY_VALUE > 0 } {
   #      set_interface_property $SLAVE_INTERFACE burstOnBurstBoundariesOnly true
   #  } else {
   #      set_interface_property $SLAVE_INTERFACE burstOnBurstBoundariesOnly false
   #  }

   # if { $AV_REGISTERINCOMINGSIGNALS_VALUE == 0 } {
   #    set_interface_property $SLAVE_INTERFACE registerIncomingSignals false
   # } else {
   #    set_interface_property $SLAVE_INTERFACE registerIncomingSignals true
   # }
    
    # Interface Ports
    #---------------------------------------------------------------------
    add_interface_port $SLAVE_INTERFACE avs_writedata writedata Input $AV_DATA_W_VALUE
    set_port_property avs_writedata VHDL_TYPE STD_LOGIC_VECTOR
    
    # add_interface_port $SLAVE_INTERFACE avs_begintransfer begintransfer Input 1
    # add_interface_port $SLAVE_INTERFACE avs_beginbursttransfer beginbursttransfer Input 1
    if { $AV_FIX_READ_LATENCY_VALUE == 0 } {
        add_interface_port $SLAVE_INTERFACE avs_burstcount burstcount Input $AV_BURSTCOUNT_W_VALUE
        set_port_property avs_burstcount VHDL_TYPE STD_LOGIC_VECTOR
    }

    add_interface_port $SLAVE_INTERFACE avs_readdata readdata Output $AV_DATA_W_VALUE
    set_port_property avs_readdata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $SLAVE_INTERFACE avs_address address Input $AV_ADDRESS_W_VALUE
    set_port_property avs_address VHDL_TYPE STD_LOGIC_VECTOR

    if {$USE_WAIT_REQUEST_VALUE > 0} {
         add_interface_port $SLAVE_INTERFACE avs_waitrequest waitrequest Output 1
    }
    
    if {$ASSERT_HIGH_WRITE_VALUE > 0} {
        add_interface_port $SLAVE_INTERFACE avs_write write Input 1
    } else {
        add_interface_port $SLAVE_INTERFACE avs_write write_n Input 1
    }
    if {$ASSERT_HIGH_READ_VALUE > 0} {
        add_interface_port $SLAVE_INTERFACE avs_read read Input 1
    } else {
        add_interface_port $SLAVE_INTERFACE avs_read read_n Input 1
    }
    if {$ASSERT_HIGH_BYTEENABLE_VALUE > 0} {
        add_interface_port $SLAVE_INTERFACE avs_byteenable byteenable Input $AV_NUMSYMBOLS_VALUE
        set_port_property avs_byteenable VHDL_TYPE STD_LOGIC_VECTOR
    } else {
        add_interface_port $SLAVE_INTERFACE avs_byteenable byteenable_n Input $AV_NUMSYMBOLS_VALUE
        set_port_property avs_byteenable VHDL_TYPE STD_LOGIC_VECTOR
    }
    if {$ASSERT_HIGH_READDATAVALID_VALUE > 0} {
        add_interface_port $SLAVE_INTERFACE avs_readdatavalid readdatavalid Output 1
    } else {
        add_interface_port $SLAVE_INTERFACE avs_readdatavalid readdatavalid_n Output 1
    }
 
   # Terminate unused ports 
   #---------------------------------------------------------------------    

    if {$USE_WRITE_VALUE == 0} {
        set_port_property avs_write                TERMINATION 1  
        set_port_property avs_write                TERMINATION_VALUE 0
    }
    if {$USE_READ_VALUE == 0} {
        set_port_property avs_read                 TERMINATION 1
        set_port_property avs_read                 TERMINATION_VALUE 0
    }
    if {$USE_ADDRESS_VALUE == 0} {
        set_port_property avs_address              TERMINATION 1
        set_port_property avs_address              TERMINATION_VALUE 0
        set_parameter_property $AV_ADDRESS_W ENABLED false
    } else {
        set_parameter_property $AV_ADDRESS_W ENABLED true
    }
    if {$USE_BYTE_ENABLE_VALUE == 0} {
        set_port_property avs_byteenable           TERMINATION 1
        set_port_property avs_byteenable           TERMINATION_VALUE 0xffffffff
    }

    # if {$USE_BURSTCOUNT_VALUE == 0} {
    #     set_port_property avs_burstcount           TERMINATION 1
    #     set_port_property avs_burstcount           TERMINATION_VALUE 1
    #     set_parameter_property $AV_BURSTCOUNT_W ENABLED false
    # } else {
    #     set_parameter_property $AV_BURSTCOUNT_W ENABLED true
    # }

    if {$USE_READ_DATA_VALUE == 0 } {
        set_port_property avs_readdata             TERMINATION 1
    }
    if {$USE_READ_DATA_VALID_VALUE == 0} {
        set_port_property avs_readdatavalid        TERMINATION 1
    }
    if {$USE_WRITE_DATA_VALUE == 0 } {
        set_port_property avs_writedata            TERMINATION 1
        set_port_property avs_writedata            TERMINATION_VALUE 0
    }

    # if {$USE_BEGIN_TRANSFER_VALUE == 0 } {
    #     set_port_property avs_begintransfer        TERMINATION 1
    #     set_port_property avs_begintransfer        TERMINATION_VALUE 0
    # }
    # if { $USE_BEGIN_BURST_TRANSFER_VALUE == 0 } {
    #     set_port_property avs_beginbursttransfer   TERMINATION 1
    #     set_port_property avs_beginbursttransfer   TERMINATION_VALUE 0
    # }
    # if { $USE_CLKEN_VALUE == 0 } {
    #     set_port_property avs_clken   TERMINATION 1
    #     set_port_property avs_clken   TERMINATION_VALUE 1
    # }

}


# vim:set filetype=tcl:
