# (c) 1992-2020 Intel Corporation.                            
# Intel, the Intel logo, Intel, MegaCore, NIOS II, Quartus and TalkBack words    
# and logos are trademarks of Intel Corporation or its subsidiaries in the U.S.  
# and/or other countries. Other marks and brands may be claimed as the property  
# of others. See Trademarks on intel.com for full list of Intel trademarks or    
# the Trademarks & Brands Names Database (if Intel) or See www.Intel.com/legal (if Altera) 
# Your use of Intel Corporation's design tools, logic functions and other        
# software and tools, and its AMPP partner logic functions, and any output       
# files any of the foregoing (including device programming or simulation         
# files), and any associated documentation or information are expressly subject  
# to the terms and conditions of the Altera Program License Subscription         
# Agreement, Intel MegaCore Function License Agreement, or other applicable      
# license agreement, including, without limitation, that your use is for the     
# sole purpose of programming logic devices manufactured by Intel and sold by    
# Intel or its authorized distributors.  Please refer to the applicable          
# agreement for further details.                                                 


package require -exact qsys 14.0

set_module_property NAME                         hls_sim_mm_master_dpi_bfm
set_module_property GROUP                        "HLS Simulation"
set_module_property DISPLAY_NAME                 "HLS Simulation Avalon-MM Master DPI BFM"
set_module_property DESCRIPTION                  "HLS Simulation Avalon-MM Master DPI BFM"
set_module_property VERSION                      1.0
set_module_property AUTHOR                       "Altera Corporation"
set_module_property EDITABLE                     false               
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property INTERNAL                     false
set_module_property VALIDATION_CALLBACK          validate
set_module_property ELABORATION_CALLBACK         elaborate

# ---------------------------------------------------------------------
# Files
# ---------------------------------------------------------------------
# Define file set
add_fileset sim_verilog SIM_VERILOG sim_verilog
set_fileset_property sim_verilog top_level hls_sim_mm_master_dpi_bfm

add_fileset quartus_synth QUARTUS_SYNTH quartus_synth_proc
set_fileset_property quartus_synth top_level hls_sim_mm_master_dpi_bfm

proc get_qsys_verification_ip_path {} {
   set QUARTUS_ROOTDIR $::env(QUARTUS_ROOTDIR)
   puts $QUARTUS_ROOTDIR         
   append QSYS_VERIFICATION_IP $QUARTUS_ROOTDIR "/../ip/altera/sopc_builder_ip/verification/"
   puts $QSYS_VERIFICATION_IP
   return $QSYS_VERIFICATION_IP
}

# SIM_VERILOG generation callback procedure
proc sim_verilog {hls_sim_mm_master_dpi_bfm} {
   set QSYS_VERIFICATION_IP [get_qsys_verification_ip_path]

   add_fileset_file verbosity_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/verbosity_pkg.sv"
   add_fileset_file avalon_mm_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_mm_pkg.sv"
   add_fileset_file avalon_utilities_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_utilities_pkg.sv"

   add_fileset_file altera_avalon_mm_master_bfm.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/altera_avalon_mm_master_bfm/altera_avalon_mm_master_bfm.sv"
   add_fileset_file hls_sim_mm_master_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_mm_master_dpi_bfm.sv
   
   set_fileset_file_attribute verbosity_pkg.sv        COMMON_SYSTEMVERILOG_PACKAGE avalon_lvip_verbosity_pkg
   set_fileset_file_attribute avalon_mm_pkg.sv        COMMON_SYSTEMVERILOG_PACKAGE avalon_vip_avalon_mm_pkg
   set_fileset_file_attribute avalon_utilities_pkg.sv COMMON_SYSTEMVERILOG_PACKAGE avalon_vip_avalon_utilities_pkg
}

proc quartus_synth_proc {hls_sim_mm_master_dpi_bfm} {
   set QSYS_VERIFICATION_IP [get_qsys_verification_ip_path]
   
   add_fileset_file verbosity_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/verbosity_pkg.sv"
   add_fileset_file avalon_mm_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_mm_pkg.sv"
   add_fileset_file avalon_utilities_pkg.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/lib/avalon_utilities_pkg.sv"

   add_fileset_file altera_avalon_mm_master_bfm.sv SYSTEM_VERILOG PATH "$QSYS_VERIFICATION_IP/altera_avalon_mm_master_bfm/altera_avalon_mm_master_bfm.sv"
   add_fileset_file hls_sim_mm_master_dpi_bfm.sv SYSTEM_VERILOG PATH hls_sim_mm_master_dpi_bfm.sv
}

#------------------------------------------------------------------------------
# Parameters
#------------------------------------------------------------------------------
set AV_ADDRESS_W              "AV_ADDRESS_W"
set AV_SYMBOL_W               "AV_SYMBOL_W" 
set AV_NUMSYMBOLS             "AV_NUMSYMBOLS"
set AV_BURSTCOUNT_W           "AV_BURSTCOUNT_W" 

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

set AV_BURST_LINEWRAP         "AV_BURST_LINEWRAP" 
set AV_BURST_BNDR_ONLY        "AV_BURST_BNDR_ONLY"

set AV_FIX_READ_LATENCY       "AV_FIX_READ_LATENCY"
set AV_READ_WAIT_TIME         "AV_READ_WAIT_TIME"
set AV_WRITE_WAIT_TIME        "AV_WRITE_WAIT_TIME"
set REGISTER_WAITREQUEST      "REGISTER_WAITREQUEST"

set ADDRESS_UNITS                "ADDRESS_UNITS"
set AV_REGISTERINCOMINGSIGNALS   "AV_REGISTERINCOMINGSIGNALS"

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

add_parameter $AV_BURSTCOUNT_W Integer 1
set_parameter_property $AV_BURSTCOUNT_W DISPLAY_NAME "Burstcount width"
set_parameter_property $AV_BURSTCOUNT_W AFFECTS_ELABORATION true
set_parameter_property $AV_BURSTCOUNT_W DESCRIPTION "The width of the Burstcount port determines the maximum burst length that can be specified for a transaction."
set_parameter_property $AV_BURSTCOUNT_W ALLOWED_RANGES {1:32}
set_parameter_property $AV_BURSTCOUNT_W HDL_PARAMETER true
set_parameter_property $AV_SYMBOL_W GROUP "Port Widths"

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

add_parameter $USE_WAIT_REQUEST Integer 0
set_parameter_property $USE_WAIT_REQUEST DISPLAY_NAME "Use the waitrequest signal"
set_parameter_property $USE_WAIT_REQUEST AFFECTS_ELABORATION true
set_parameter_property $USE_WAIT_REQUEST DESCRIPTION "Use the waitrequest signal"
set_parameter_property $USE_WAIT_REQUEST DISPLAY_HINT boolean
set_parameter_property $USE_WAIT_REQUEST HDL_PARAMETER true
set_parameter_property $USE_WAIT_REQUEST GROUP "Port Enables"

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
set_parameter_property $ADDRESS_UNITS DESCRIPTION "Set master interface address type to symbols or words. Default value is 'WORDS'"
set_parameter_property $ADDRESS_UNITS ALLOWED_RANGES {"SYMBOLS" "WORDS"}
set_parameter_property $ADDRESS_UNITS HDL_PARAMETER false
set_parameter_property $ADDRESS_UNITS GROUP "Interface Address Type"

add_parameter COMPONENT_NAME STRING "dut" "The name of the component that contains the interface"
set_parameter_property COMPONENT_NAME DEFAULT_VALUE "dut"
set_parameter_property COMPONENT_NAME DISPLAY_NAME COMPONENT_NAME
set_parameter_property COMPONENT_NAME TYPE STRING
set_parameter_property COMPONENT_NAME UNITS None
set_parameter_property COMPONENT_NAME DESCRIPTION "The name of the component that contains the interface"
set_parameter_property COMPONENT_NAME HDL_PARAMETER true
 
add_parameter COMPONENT_HAS_SLAVE_RETURN INTEGER 0 "If set, read the return value from the component's slave registers into the return output stream."
set_parameter_property COMPONENT_HAS_SLAVE_RETURN DEFAULT_VALUE 0
set_parameter_property COMPONENT_HAS_SLAVE_RETURN DISPLAY_NAME COMPONENT_HAS_SLAVE_RETURN
set_parameter_property COMPONENT_HAS_SLAVE_RETURN TYPE INTEGER
set_parameter_property COMPONENT_HAS_SLAVE_RETURN UNITS None
set_parameter_property COMPONENT_HAS_SLAVE_RETURN DESCRIPTION "If set, read the return value from the component's slave registers into the return output stream."
set_parameter_property COMPONENT_HAS_SLAVE_RETURN HDL_PARAMETER true

add_parameter COMPONENT_SLAVE_WRITE_INTERFACE_NAME STRING "__ihc_hls_avs_write_stream__" "The name of the testbench stream that passes the write data to the slave in the simulation"
set_parameter_property COMPONENT_SLAVE_WRITE_INTERFACE_NAME DEFAULT_VALUE "__ihc_hls_avs_write_stream__"
set_parameter_property COMPONENT_SLAVE_WRITE_INTERFACE_NAME DISPLAY_NAME COMPONENT_SLAVE_WRITE_INTERFACE_NAME
set_parameter_property COMPONENT_SLAVE_WRITE_INTERFACE_NAME TYPE STRING
set_parameter_property COMPONENT_SLAVE_WRITE_INTERFACE_NAME UNITS None
set_parameter_property COMPONENT_SLAVE_WRITE_INTERFACE_NAME DESCRIPTION "The name of the testbench stream that passes the write data to the slave in the simulation"
set_parameter_property COMPONENT_SLAVE_WRITE_INTERFACE_NAME HDL_PARAMETER true

add_parameter COMPONENT_SLAVE_READ_INTERFACE_NAME STRING {$return} "The name of the testbench stream that passes the read data from the slave in the simulation to the testbench"
set_parameter_property COMPONENT_SLAVE_READ_INTERFACE_NAME DEFAULT_VALUE {$return}
set_parameter_property COMPONENT_SLAVE_READ_INTERFACE_NAME DISPLAY_NAME COMPONENT_SLAVE_READ_INTERFACE_NAME
set_parameter_property COMPONENT_SLAVE_READ_INTERFACE_NAME TYPE STRING
set_parameter_property COMPONENT_SLAVE_READ_INTERFACE_NAME UNITS None
set_parameter_property COMPONENT_SLAVE_READ_INTERFACE_NAME DESCRIPTION "The name of the testbench stream that passes the read data from the slave in the simulation to the testbench"
set_parameter_property COMPONENT_SLAVE_READ_INTERFACE_NAME HDL_PARAMETER true

add_parameter COMPONENT_CRA_SLAVE INTEGER 0 "If set, the slave write stream contains address and byteenable information. Otherwise, it is jsut a data stream."
set_parameter_property COMPONENT_CRA_SLAVE DEFAULT_VALUE 0
set_parameter_property COMPONENT_CRA_SLAVE DISPLAY_NAME COMPONENT_CRA_SLAVE
set_parameter_property COMPONENT_CRA_SLAVE TYPE INTEGER
set_parameter_property COMPONENT_CRA_SLAVE UNITS None
set_parameter_property COMPONENT_CRA_SLAVE DESCRIPTION "If set, the slave write stream contains address and byteenable information. Otherwise, it is jsut a data stream."
set_parameter_property COMPONENT_CRA_SLAVE HDL_PARAMETER true

add_parameter NUM_SLAVE_MEMORIES INTEGER 0 "If this is the CRA slave, it needs to know how many other slaves to wait for before writing to its registers so as not to write the start bit too early."
set_parameter_property NUM_SLAVE_MEMORIES DEFAULT_VALUE 0
set_parameter_property NUM_SLAVE_MEMORIES DISPLAY_NAME NUM_SLAVE_MEMORIES
set_parameter_property NUM_SLAVE_MEMORIES TYPE INTEGER
set_parameter_property NUM_SLAVE_MEMORIES UNITS None
set_parameter_property NUM_SLAVE_MEMORIES DESCRIPTION "If this is the CRA slave, it needs to know how many other slaves to wait for before writing to its registers so as not to write the start bit too early."
set_parameter_property NUM_SLAVE_MEMORIES HDL_PARAMETER true

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


    global AV_BURST_LINEWRAP     
    global AV_BURST_BNDR_ONLY 

    global AV_FIX_READ_LATENCY
    global AV_READ_WAIT_TIME   
    global AV_WRITE_WAIT_TIME  
    global REGISTER_WAITREQUEST
    global AV_REGISTERINCOMINGSIGNALS
    
    global ADDRESS_UNITS

    set AV_ADDRESS_W_VALUE                [ get_parameter $AV_ADDRESS_W ]
    set AV_SYMBOL_W_VALUE                 [ get_parameter $AV_SYMBOL_W ] 
    set AV_NUMSYMBOLS_VALUE               [ get_parameter $AV_NUMSYMBOLS ]
    set AV_BURSTCOUNT_W_VALUE             [ get_parameter $AV_BURSTCOUNT_W ]
   
    set USE_READ_VALUE                    [ get_parameter $USE_READ ]  
    set USE_WRITE_VALUE                   [ get_parameter $USE_WRITE ] 
    set USE_ADDRESS_VALUE                 [ get_parameter $USE_ADDRESS ] 
    set USE_BYTE_ENABLE_VALUE             [ get_parameter $USE_BYTE_ENABLE ]
    set USE_BURSTCOUNT_VALUE              [ get_parameter $USE_BURSTCOUNT ] 
    set USE_READ_DATA_VALUE               [ get_parameter $USE_READ_DATA ]
    set USE_READ_DATA_VALID_VALUE         [ get_parameter $USE_READ_DATA_VALID ]
    set USE_WRITE_DATA_VALUE              [ get_parameter $USE_WRITE_DATA ]       
    set USE_WAIT_REQUEST_VALUE            [ get_parameter $USE_WAIT_REQUEST ]  
   
    set AV_BURST_LINEWRAP_VALUE           [ get_parameter $AV_BURST_LINEWRAP ] 
    set AV_BURST_BNDR_ONLY_VALUE          [ get_parameter $AV_BURST_BNDR_ONLY ] 
   
    set AV_FIX_READ_LATENCY_VALUE         [ get_parameter $AV_FIX_READ_LATENCY ] 
    set AV_READ_WAIT_TIME_VALUE           [ get_parameter $AV_READ_WAIT_TIME ]  
    set AV_WRITE_WAIT_TIME_VALUE          [ get_parameter $AV_WRITE_WAIT_TIME ]    
    set REGISTER_WAITREQUEST_VALUE        [ get_parameter $REGISTER_WAITREQUEST ] 
    set AV_REGISTERINCOMINGSIGNALS_VALUE  [ get_parameter $AV_REGISTERINCOMINGSIGNALS ] 
    
    set AV_DATA_W_VALUE                   [ expr {$AV_SYMBOL_W_VALUE * $AV_NUMSYMBOLS_VALUE}]
    set AV_TRANSACTIONID_W_VALUE          8
    
    set ADDRESS_UNITS_VALUE               [ get_parameter $ADDRESS_UNITS ]



    # Interface Names
    #---------------------------------------------------------------------
    set CLOCK_INTERFACE  "clock"
    set RESET_INTERFACE  "reset"
    set MASTER_INTERFACE "m0"

    #---------------------------------------------------------------------
    # Clock-Reset connection point
    #---------------------------------------------------------------------
    add_interface $CLOCK_INTERFACE clock end
    add_interface_port $CLOCK_INTERFACE clock clk Input 1
   
    add_interface $RESET_INTERFACE reset end
    set_interface_property $RESET_INTERFACE associatedClock clock    
    add_interface_port $RESET_INTERFACE reset_n reset_n Input 1
    
 
    # DPI control interfaces
    #---------------------------------------------------------------------
    add_interface dpi_control_bind conduit end
    set_interface_property dpi_control_bind ENABLED true
    add_interface_port dpi_control_bind do_bind conduit Input 1

    add_interface dpi_control_enable conduit end
    set_interface_property dpi_control_enable ENABLED true
    add_interface_port dpi_control_enable enable conduit Input 1

    add_interface dpi_control_done_writes conduit start
    set_interface_property dpi_control_done_writes ENABLED true
    add_interface_port dpi_control_done_writes done_writes conduit Output 1

    add_interface dpi_control_done_reads conduit end
    set_interface_property dpi_control_done_reads ENABLED true
    add_interface_port dpi_control_done_reads done_reads conduit Output 1

    add_interface dpi_control_component_started conduit end
    set_interface_property dpi_control_component_started ENABLED true
    add_interface_port dpi_control_component_started component_started conduit Input 1

    add_interface dpi_control_component_done conduit start
    set_interface_property dpi_control_component_done ENABLED true
    add_interface_port dpi_control_component_done component_done conduit Input 1

    # Slave Memory/CRA control interfaces
    #---------------------------------------------------------------------
    # only used by slave memories
    add_interface cra_control_done_writes_to_cra conduit start
    set_interface_property cra_control_done_writes_to_cra ENABLED true
    add_interface_port cra_control_done_writes_to_cra done_writes_to_cra conduit Output 1

    # only used by the CRA slave
    set num_slave_mems [get_parameter "NUM_SLAVE_MEMORIES"]
    add_interface cra_control_slave_memory_writes_done conduit start
    if { $num_slave_mems > 0 } {
      set_interface_property cra_control_slave_memory_writes_done ENABLED true
      add_interface_port cra_control_slave_memory_writes_done slave_memory_writes_done conduit Input [get_parameter "NUM_SLAVE_MEMORIES"]
    }

    add_interface slave_busy_out conduit start
    set_interface_property slave_busy_out ENABLED true
    add_interface_port slave_busy_out slave_busy_out conduit Output 1

    #---------------------------------------------------------------------
    #  Avalon Master connection point
    #---------------------------------------------------------------------
    add_interface $MASTER_INTERFACE avalon master

    # Interface Properties
    #---------------------------------------------------------------------
    set_interface_property $MASTER_INTERFACE ENABLED true
    set_interface_property $MASTER_INTERFACE ASSOCIATED_CLOCK $CLOCK_INTERFACE
    #set_interface_property $MASTER_INTERFACE readWaitTime $AV_READ_WAIT_TIME_VALUE
    #set_interface_property $MASTER_INTERFACE writeWaitTime $AV_WRITE_WAIT_TIME_VALUE
    set_interface_property $MASTER_INTERFACE addressUnits $ADDRESS_UNITS_VALUE

    if {$USE_WAIT_REQUEST_VALUE == 0} {
        set_parameter_property $AV_READ_WAIT_TIME ENABLED true
        set_parameter_property $AV_WRITE_WAIT_TIME ENABLED true
    } else {
        set_parameter_property $AV_READ_WAIT_TIME ENABLED false
        set_parameter_property $AV_WRITE_WAIT_TIME ENABLED false
    }
    
    set_interface_property $MASTER_INTERFACE maximumPendingWriteTransactions 0
    set_interface_property $MASTER_INTERFACE maximumPendingReadTransactions 0
    
    if {$USE_READ_DATA_VALID_VALUE > 0} {
        set_interface_property $MASTER_INTERFACE readLatency 0
        set_parameter_property $AV_FIX_READ_LATENCY ENABLED false
    } else {
        set_interface_property $MASTER_INTERFACE readLatency $AV_FIX_READ_LATENCY_VALUE
        set_parameter_property $AV_FIX_READ_LATENCY ENABLED true
    }
    
    # Interface Ports
    #---------------------------------------------------------------------
    add_interface_port $MASTER_INTERFACE avm_writedata writedata Output $AV_DATA_W_VALUE
    set_port_property avm_writedata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MASTER_INTERFACE avm_burstcount burstcount Output $AV_BURSTCOUNT_W_VALUE
    set_port_property avm_burstcount VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MASTER_INTERFACE avm_readdata readdata Input $AV_DATA_W_VALUE
    set_port_property avm_readdata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MASTER_INTERFACE avm_address address Output $AV_ADDRESS_W_VALUE
    set_port_property avm_address VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MASTER_INTERFACE avm_waitrequest waitrequest Input 1
    add_interface_port $MASTER_INTERFACE avm_write write Output 1
    add_interface_port $MASTER_INTERFACE avm_read read Output 1
    add_interface_port $MASTER_INTERFACE avm_byteenable byteenable Output $AV_NUMSYMBOLS_VALUE
    set_port_property avm_byteenable VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MASTER_INTERFACE avm_readdatavalid readdatavalid Input 1
 
   # Terminate unused ports 
   #---------------------------------------------------------------------    

    if {$USE_WRITE_VALUE == 0} {
        set_port_property avm_write                TERMINATION 1  
        set_port_property avm_write                TERMINATION_VALUE 0
    }
    if {$USE_READ_VALUE == 0} {
        set_port_property avm_read                 TERMINATION 1
        set_port_property avm_read                 TERMINATION_VALUE 0
    }
    if {$USE_ADDRESS_VALUE == 0} {
        set_port_property avm_address              TERMINATION 1
        set_port_property avm_address              TERMINATION_VALUE 0
        set_parameter_property $AV_ADDRESS_W ENABLED false
    } else {
        set_parameter_property $AV_ADDRESS_W ENABLED true
    }
    if {$USE_BYTE_ENABLE_VALUE == 0} {
        set_port_property avm_byteenable           TERMINATION 1
        set_port_property avm_byteenable           TERMINATION_VALUE 0xffffffff
    }

    if {$USE_READ_DATA_VALUE == 0 } {
        set_port_property avm_readdata             TERMINATION 1
    }
    if {$USE_READ_DATA_VALID_VALUE == 0} {
        set_port_property avm_readdatavalid        TERMINATION 1
    }
    if {$USE_WRITE_DATA_VALUE == 0 } {
        set_port_property avm_writedata            TERMINATION 1
        set_port_property avm_writedata            TERMINATION_VALUE 0
    }

}


# vim:set filetype=tcl:
