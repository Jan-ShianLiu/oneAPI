# 
# acl_hyper_optimized_ccb "AVMM S10 Clock Crossing Bridge" v1.0
#  2017.02.15.14:38:56
# 
# 

# 
# request TCL package from ACDS 17.0
# 
package require -exact qsys 17.0

# 
# module acl_hyper_optimized_ccb
# 
set_module_property DESCRIPTION ""
set_module_property NAME acl_hyper_optimized_ccb
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME "AVMM S10 Clock Crossing Bridge"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false
set_module_property ELABORATION_CALLBACK elaborate


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL ccb
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false

add_fileset_file ccb.sv                                     SYSTEM_VERILOG PATH     ccb.sv TOP_LEVEL_FILE
add_fileset_file alt_hiconnect_bxor2w5t1.v                  VERILOG PATH            alt_hiconnect_bxor2w5t1.v
add_fileset_file alt_hiconnect_cnt5ic.v                     VERILOG PATH            alt_hiconnect_cnt5ic.v
add_fileset_file alt_hiconnect_cnt5il.v                     VERILOG PATH            alt_hiconnect_cnt5il.v
add_fileset_file alt_hiconnect_dc_fifo.sdc                  SDC PATH                alt_hiconnect_dc_fifo.sdc
add_fileset_file alt_hiconnect_dc_fifo.sv                   SYSTEM_VERILOG PATH     alt_hiconnect_dc_fifo.sv
add_fileset_file alt_hiconnect_dc_fifo_hw.tcl               TCL PATH                alt_hiconnect_dc_fifo_hw.tcl
add_fileset_file alt_hiconnect_gpx5.v                       VERILOG PATH            alt_hiconnect_gpx5.v
add_fileset_file alt_hiconnect_gray5t1.v                    VERILOG PATH            alt_hiconnect_gray5t1.v
add_fileset_file alt_hiconnect_lut6.v                       VERILOG PATH            alt_hiconnect_lut6.v
add_fileset_file alt_hiconnect_mlab.v                       VERILOG PATH            alt_hiconnect_mlab.v
add_fileset_file alt_hiconnect_mlab5a1r1w1.v                VERILOG PATH            alt_hiconnect_mlab5a1r1w1.v
add_fileset_file alt_hiconnect_sync5m.v                     VERILOG PATH            alt_hiconnect_sync5m.v
add_fileset_file alt_hiconnect_ungray5t1.v                  VERILOG PATH            alt_hiconnect_ungray5t1.v
add_fileset_file alt_hiconnect_wys_reg.v                    VERILOG PATH            alt_hiconnect_wys_reg.v
add_fileset_file alt_hiconnect_xor1t0.v                     VERILOG PATH            alt_hiconnect_xor1t0.v
add_fileset_file alt_hiconnect_xor1t1.v                     VERILOG PATH            alt_hiconnect_xor1t1.v
add_fileset_file alt_hiconnect_xor2t0.v                     VERILOG PATH            alt_hiconnect_xor2t0.v
add_fileset_file alt_hiconnect_xor2t1.v                     VERILOG PATH            alt_hiconnect_xor2t1.v
add_fileset_file alt_hiconnect_xor3t0.v                     VERILOG PATH            alt_hiconnect_xor3t0.v
add_fileset_file alt_hiconnect_xor3t1.v                     VERILOG PATH            alt_hiconnect_xor3t1.v
add_fileset_file alt_hiconnect_xor4t0.v                     VERILOG PATH            alt_hiconnect_xor4t0.v
add_fileset_file alt_hiconnect_xor4t1.v                     VERILOG PATH            alt_hiconnect_xor4t1.v
add_fileset_file alt_hiconnect_xor5t0.v                     VERILOG PATH            alt_hiconnect_xor5t0.v
add_fileset_file alt_hiconnect_xor5t1.v                     VERILOG PATH            alt_hiconnect_xor5t1.v
add_fileset_file alt_st_mlab_dcfifo.v                       VERILOG PATH            alt_st_mlab_dcfifo.v
add_fileset_file rsp_counter.v                              VERILOG PATH            rsp_counter.v           
add_fileset_file throttle_switch.v                          VERILOG PATH            throttle_switch.v
add_fileset_file bridge_throttle.v                          VERILOG PATH            bridge_throttle.v
add_fileset_file hld_fifo.sv                                SYSTEM_VERILOG PATH     ../../hld_fifo.sv
add_fileset_file acl_reset_handler.sv                       SYSTEM_VERILOG PATH     ../../acl_reset_handler.sv
add_fileset_file acl_fanout_pipeline.sv                     SYSTEM_VERILOG PATH     ../../acl_fanout_pipeline.sv
add_fileset_file acl_std_synchronizer_nocut.v               VERILOG PATH            ../../acl_std_synchronizer_nocut.v
add_fileset_file acl_zero_latency_fifo.sv                   SYSTEM_VERILOG PATH     ../../acl_zero_latency_fifo.sv
add_fileset_file acl_low_latency_fifo.sv                    SYSTEM_VERILOG PATH     ../../acl_low_latency_fifo.sv
add_fileset_file acl_high_speed_fifo.sv                     SYSTEM_VERILOG PATH     ../../acl_high_speed_fifo.sv
add_fileset_file acl_lfsr.sv                                SYSTEM_VERILOG PATH     ../../acl_lfsr.sv
add_fileset_file acl_tessellated_incr_decr_threshold.sv     SYSTEM_VERILOG PATH     ../../acl_tessellated_incr_decr_threshold.sv
add_fileset_file acl_tessellated_incr_lookahead.sv          SYSTEM_VERILOG PATH     ../../acl_tessellated_incr_lookahead.sv
add_fileset_file acl_parameter_assert.svh                   SYSTEM_VERILOG PATH     ../../acl_parameter_assert.svh


add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL ccb
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VERILOG ENABLE_FILE_OVERWRITE_MODE false

add_fileset_file ccb.sv                                     SYSTEM_VERILOG PATH     ccb.sv
add_fileset_file alt_hiconnect_bxor2w5t1.v                  VERILOG PATH            alt_hiconnect_bxor2w5t1.v
add_fileset_file alt_hiconnect_cnt5ic.v                     VERILOG PATH            alt_hiconnect_cnt5ic.v
add_fileset_file alt_hiconnect_cnt5il.v                     VERILOG PATH            alt_hiconnect_cnt5il.v
add_fileset_file alt_hiconnect_dc_fifo.sdc                  SDC PATH                alt_hiconnect_dc_fifo.sdc
add_fileset_file alt_hiconnect_dc_fifo.sv                   SYSTEM_VERILOG PATH     alt_hiconnect_dc_fifo.sv
add_fileset_file alt_hiconnect_dc_fifo_hw.tcl               TCL PATH                alt_hiconnect_dc_fifo_hw.tcl
add_fileset_file alt_hiconnect_gpx5.v                       VERILOG PATH            alt_hiconnect_gpx5.v
add_fileset_file alt_hiconnect_gray5t1.v                    VERILOG PATH            alt_hiconnect_gray5t1.v
add_fileset_file alt_hiconnect_lut6.v                       VERILOG PATH            alt_hiconnect_lut6.v
add_fileset_file alt_hiconnect_mlab.v                       VERILOG PATH            alt_hiconnect_mlab.v
add_fileset_file alt_hiconnect_mlab5a1r1w1.v                VERILOG PATH            alt_hiconnect_mlab5a1r1w1.v
add_fileset_file alt_hiconnect_sync5m.v                     VERILOG PATH            alt_hiconnect_sync5m.v
add_fileset_file alt_hiconnect_ungray5t1.v                  VERILOG PATH            alt_hiconnect_ungray5t1.v
add_fileset_file alt_hiconnect_wys_reg.v                    VERILOG PATH            alt_hiconnect_wys_reg.v
add_fileset_file alt_hiconnect_xor1t0.v                     VERILOG PATH            alt_hiconnect_xor1t0.v
add_fileset_file alt_hiconnect_xor1t1.v                     VERILOG PATH            alt_hiconnect_xor1t1.v
add_fileset_file alt_hiconnect_xor2t0.v                     VERILOG PATH            alt_hiconnect_xor2t0.v
add_fileset_file alt_hiconnect_xor2t1.v                     VERILOG PATH            alt_hiconnect_xor2t1.v
add_fileset_file alt_hiconnect_xor3t0.v                     VERILOG PATH            alt_hiconnect_xor3t0.v
add_fileset_file alt_hiconnect_xor3t1.v                     VERILOG PATH            alt_hiconnect_xor3t1.v
add_fileset_file alt_hiconnect_xor4t0.v                     VERILOG PATH            alt_hiconnect_xor4t0.v
add_fileset_file alt_hiconnect_xor4t1.v                     VERILOG PATH            alt_hiconnect_xor4t1.v
add_fileset_file alt_hiconnect_xor5t0.v                     VERILOG PATH            alt_hiconnect_xor5t0.v
add_fileset_file alt_hiconnect_xor5t1.v                     VERILOG PATH            alt_hiconnect_xor5t1.v
add_fileset_file alt_st_mlab_dcfifo.v                       VERILOG PATH            alt_st_mlab_dcfifo.v
add_fileset_file rsp_counter.v                              VERILOG PATH            rsp_counter.v           
add_fileset_file throttle_switch.v                          VERILOG PATH            throttle_switch.v
add_fileset_file bridge_throttle.v                          VERILOG PATH            bridge_throttle.v
add_fileset_file hld_fifo.sv                                SYSTEM_VERILOG PATH     ../../hld_fifo.sv
add_fileset_file acl_reset_handler.sv                       SYSTEM_VERILOG PATH     ../../acl_reset_handler.sv
add_fileset_file acl_fanout_pipeline.sv                     SYSTEM_VERILOG PATH     ../../acl_fanout_pipeline.sv
add_fileset_file acl_std_synchronizer_nocut.v               VERILOG PATH            ../../acl_std_synchronizer_nocut.v
add_fileset_file acl_zero_latency_fifo.sv                   SYSTEM_VERILOG PATH     ../../acl_zero_latency_fifo.sv
add_fileset_file acl_low_latency_fifo.sv                    SYSTEM_VERILOG PATH     ../../acl_low_latency_fifo.sv
add_fileset_file acl_high_speed_fifo.sv                     SYSTEM_VERILOG PATH     ../../acl_high_speed_fifo.sv
add_fileset_file acl_lfsr.sv                                SYSTEM_VERILOG PATH     ../../acl_lfsr.sv
add_fileset_file acl_tessellated_incr_decr_threshold.sv     SYSTEM_VERILOG PATH     ../../acl_tessellated_incr_decr_threshold.sv
add_fileset_file acl_tessellated_incr_lookahead.sv          SYSTEM_VERILOG PATH     ../../acl_tessellated_incr_lookahead.sv
add_fileset_file acl_parameter_assert.svh                   SYSTEM_VERILOG PATH     ../../acl_parameter_assert.svh

# 
# parameters
# 
add_parameter DATA_W INTEGER 512
set_parameter_property DATA_W DEFAULT_VALUE 512
set_parameter_property DATA_W DISPLAY_NAME "Data width"
set_parameter_property DATA_W TYPE INTEGER
set_parameter_property DATA_W UNITS None
set_parameter_property DATA_W HDL_PARAMETER true
set_parameter_property DATA_W ALLOWED_RANGES "8,16,32,64,128,256,512,1024"
add_parameter ADDRESS_W INTEGER 31
set_parameter_property ADDRESS_W DEFAULT_VALUE 31
set_parameter_property ADDRESS_W DISPLAY_NAME "Address width"
set_parameter_property ADDRESS_W TYPE INTEGER
set_parameter_property ADDRESS_W UNITS None
set_parameter_property ADDRESS_W HDL_PARAMETER true
set_parameter_property ADDRESS_W ALLOWED_RANGES 1:64
add_parameter MAX_BURST_SIZE INTEGER 16
set_parameter_property MAX_BURST_SIZE DEFAULT_VALUE 16
set_parameter_property MAX_BURST_SIZE DISPLAY_NAME "Max burst size"
set_parameter_property MAX_BURST_SIZE TYPE INTEGER
set_parameter_property MAX_BURST_SIZE UNITS None
set_parameter_property MAX_BURST_SIZE HDL_PARAMETER true
set_parameter_property MAX_BURST_SIZE ALLOWED_RANGES "1,2,4,8,16,32,64"
add_parameter BYTEENABLE_W INTEGER 64
set_parameter_property BYTEENABLE_W DISPLAY_NAME "Byteenable width"
set_parameter_property BYTEENABLE_W TYPE INTEGER
set_parameter_property BYTEENABLE_W UNITS None
set_parameter_property BYTEENABLE_W VISIBLE false
set_parameter_property BYTEENABLE_W DERIVED true
add_parameter BURSTCOUNT_W INTEGER 5
set_parameter_property BURSTCOUNT_W DISPLAY_NAME "Burstcount width"
set_parameter_property BURSTCOUNT_W TYPE INTEGER
set_parameter_property BURSTCOUNT_W UNITS None
set_parameter_property BURSTCOUNT_W VISIBLE false
set_parameter_property BURSTCOUNT_W DERIVED true

add_parameter CMD_SCFIFO_DEPTH INTEGER 32
set_parameter_property CMD_SCFIFO_DEPTH DEFAULT_VALUE 32
set_parameter_property CMD_SCFIFO_DEPTH ALLOWED_RANGES 5:512
set_parameter_property CMD_SCFIFO_DEPTH DISPLAY_NAME "Command FIFO depth"
set_parameter_property CMD_SCFIFO_DEPTH TYPE INTEGER
set_parameter_property CMD_SCFIFO_DEPTH UNITS None
set_parameter_property CMD_SCFIFO_DEPTH HDL_PARAMETER true
add_parameter RSP_SCFIFO_DEPTH INTEGER 512
set_parameter_property RSP_SCFIFO_DEPTH DEFAULT_VALUE 512
set_parameter_property RSP_SCFIFO_DEPTH ALLOWED_RANGES 5:512
set_parameter_property RSP_SCFIFO_DEPTH DISPLAY_NAME "Response FIFO depth"
set_parameter_property RSP_SCFIFO_DEPTH TYPE INTEGER
set_parameter_property RSP_SCFIFO_DEPTH UNITS None
set_parameter_property RSP_SCFIFO_DEPTH HDL_PARAMETER true
add_parameter USE_ERROR_PREVENTION BOOLEAN true
set_parameter_property USE_ERROR_PREVENTION DEFAULT_VALUE true
set_parameter_property USE_ERROR_PREVENTION DISPLAY_NAME "Enable error prevention"
set_parameter_property USE_ERROR_PREVENTION TYPE BOOLEAN
set_parameter_property USE_ERROR_PREVENTION UNITS None
set_parameter_property USE_ERROR_PREVENTION HDL_PARAMETER true
add_parameter FAST_TO_SLOW BOOLEAN 1
set_parameter_property FAST_TO_SLOW DEFAULT_VALUE true
set_parameter_property FAST_TO_SLOW DISPLAY_NAME "Fast to slow mode, or same clock ratio mode"
set_parameter_property FAST_TO_SLOW TYPE BOOLEAN
set_parameter_property FAST_TO_SLOW UNITS None
set_parameter_property FAST_TO_SLOW HDL_PARAMETER true
add_parameter SLAVE_PORT_WAITREQUEST_ALLOWANCE INTEGER 32
set_parameter_property SLAVE_PORT_WAITREQUEST_ALLOWANCE DEFAULT_VALUE 0
set_parameter_property SLAVE_PORT_WAITREQUEST_ALLOWANCE ALLOWED_RANGES 0:28
set_parameter_property SLAVE_PORT_WAITREQUEST_ALLOWANCE DISPLAY_NAME "Slave port waitrequest allowance"
set_parameter_property SLAVE_PORT_WAITREQUEST_ALLOWANCE TYPE INTEGER
set_parameter_property SLAVE_PORT_WAITREQUEST_ALLOWANCE UNITS None
set_parameter_property SLAVE_PORT_WAITREQUEST_ALLOWANCE HDL_PARAMETER true
add_parameter MASTER_PORT_WAITREQUEST_ALLOWANCE INTEGER 32
set_parameter_property MASTER_PORT_WAITREQUEST_ALLOWANCE DEFAULT_VALUE 0
set_parameter_property MASTER_PORT_WAITREQUEST_ALLOWANCE ALLOWED_RANGES 0:10
set_parameter_property MASTER_PORT_WAITREQUEST_ALLOWANCE DISPLAY_NAME "Master port waitrequest allowance"
set_parameter_property MASTER_PORT_WAITREQUEST_ALLOWANCE TYPE INTEGER
set_parameter_property MASTER_PORT_WAITREQUEST_ALLOWANCE UNITS None
set_parameter_property MASTER_PORT_WAITREQUEST_ALLOWANCE HDL_PARAMETER true
add_parameter SYMBOL_W INTEGER 8
set_parameter_property SYMBOL_W DEFAULT_VALUE 8
set_parameter_property SYMBOL_W DISPLAY_NAME "Symbol width"
set_parameter_property SYMBOL_W DESCRIPTION {Symbol (byte) width}
set_parameter_property SYMBOL_W TYPE INTEGER
set_parameter_property SYMBOL_W UNITS None
set_parameter_property SYMBOL_W AFFECTS_GENERATION false
set_parameter_property SYMBOL_W HDL_PARAMETER true

# 
# connection point slave_clk
# 
add_interface slave_clk clock end
set_interface_property slave_clk clockRate 0
set_interface_property slave_clk ENABLED true
set_interface_property slave_clk EXPORT_OF ""
set_interface_property slave_clk PORT_NAME_MAP ""
set_interface_property slave_clk CMSIS_SVD_VARIABLES ""
set_interface_property slave_clk SVD_ADDRESS_GROUP ""

add_interface_port slave_clk slave_clk clk Input 1

# 
# connection point master_clk
# 
add_interface master_clk clock end
set_interface_property master_clk clockRate 0
set_interface_property master_clk ENABLED true
set_interface_property master_clk EXPORT_OF ""
set_interface_property master_clk PORT_NAME_MAP ""
set_interface_property master_clk CMSIS_SVD_VARIABLES ""
set_interface_property master_clk SVD_ADDRESS_GROUP ""

add_interface_port master_clk master_clk clk Input 1

# 
# connection point master_reset
# 
add_interface master_reset reset end
set_interface_property master_reset associatedClock master_clk
set_interface_property master_reset synchronousEdges BOTH
set_interface_property master_reset ENABLED true
set_interface_property master_reset EXPORT_OF ""
set_interface_property master_reset PORT_NAME_MAP ""
set_interface_property master_reset CMSIS_SVD_VARIABLES ""
set_interface_property master_reset SVD_ADDRESS_GROUP ""

add_interface_port master_reset master_reset reset Input 1

# 
# connection point slave
# 
add_interface slave avalon end
set_interface_property slave addressUnits SYMBOLS
set_interface_property slave associatedClock slave_clk
set_interface_property slave associatedReset master_reset
set_interface_property slave bitsPerSymbol 8
set_interface_property slave bridgedAddressOffset ""
set_interface_property slave bridgesToMaster "master"
set_interface_property slave burstOnBurstBoundariesOnly false
set_interface_property slave burstcountUnits WORDS
set_interface_property slave explicitAddressSpan 0
set_interface_property slave holdTime 0
set_interface_property slave linewrapBursts false
set_interface_property slave maximumPendingReadTransactions 32
set_interface_property slave maximumPendingWriteTransactions 0
set_interface_property slave minimumResponseLatency 6
set_interface_property slave readLatency 0
set_interface_property slave readWaitTime 0
set_interface_property slave setupTime 0
set_interface_property slave timingUnits Cycles
set_interface_property slave transparentBridge false
set_interface_property slave waitrequestAllowance 0
set_interface_property slave writeWaitTime 0
set_interface_property slave ENABLED true
set_interface_property slave EXPORT_OF ""
set_interface_property slave PORT_NAME_MAP ""
set_interface_property slave CMSIS_SVD_VARIABLES ""
set_interface_property slave SVD_ADDRESS_GROUP ""

add_interface_port slave slave_address address Input "((ADDRESS_W - 1)) - (0) + 1"
add_interface_port slave slave_byteenable byteenable Input "((BYTEENABLE_W - 1)) - (0) + 1" 
add_interface_port slave slave_read read Input 1
add_interface_port slave slave_write write Input 1
add_interface_port slave slave_writedata writedata Input "((DATA_W - 1)) - (0) + 1"
add_interface_port slave slave_burstcount burstcount Input "((BURSTCOUNT_W - 1)) - (0) + 1"
add_interface_port slave slave_waitrequest waitrequest Output 1
add_interface_port slave slave_readdata readdata Output "((DATA_W - 1)) - (0) + 1"
add_interface_port slave slave_readdatavalid readdatavalid Output 1
set_interface_assignment slave embeddedsw.configuration.isFlash 0
set_interface_assignment slave embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment slave embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment slave embeddedsw.configuration.isPrintableDevice 0


# 
# connection point master
# 
add_interface master avalon start
set_interface_property master addressUnits SYMBOLS
set_interface_property master associatedClock master_clk
set_interface_property master associatedReset master_reset
set_interface_property master bitsPerSymbol 8
set_interface_property master burstOnBurstBoundariesOnly false
set_interface_property master burstcountUnits WORDS
set_interface_property master doStreamReads false
set_interface_property master doStreamWrites false
set_interface_property master holdTime 0
set_interface_property master linewrapBursts false
set_interface_property master maximumPendingReadTransactions 0
set_interface_property master maximumPendingWriteTransactions 0
set_interface_property master minimumResponseLatency 1
set_interface_property master readLatency 0
set_interface_property master readWaitTime 0
set_interface_property master setupTime 0
set_interface_property master timingUnits Cycles
set_interface_property master waitrequestAllowance 0
set_interface_property master writeWaitTime 0
set_interface_property master ENABLED true
set_interface_property master EXPORT_OF ""
set_interface_property master PORT_NAME_MAP ""
set_interface_property master CMSIS_SVD_VARIABLES ""
set_interface_property master SVD_ADDRESS_GROUP ""

add_interface_port master master_address address Output "((ADDRESS_W - 1)) - (0) + 1"
add_interface_port master master_byteenable byteenable Output "((BYTEENABLE_W - 1)) - (0) + 1" 
add_interface_port master master_read read Output 1
add_interface_port master master_write write Output 1
add_interface_port master master_writedata writedata Output "((DATA_W - 1)) - (0) + 1"
add_interface_port master master_burstcount burstcount Output "((BURSTCOUNT_W - 1)) - (0) + 1" 

add_interface_port master master_waitrequest waitrequest Input 1
add_interface_port master master_readdata readdata Input "((DATA_W - 1)) - (0) + 1"
add_interface_port master master_readdatavalid readdatavalid Input 1

proc elaborate { } {

    set data_w   [ get_parameter_value DATA_W   ]
    set symbol_w [ get_parameter_value SYMBOL_W ]
    set byteen_w [ expr $data_w / $symbol_w ]
    set burst_w  [log2ceil [expr [get_parameter_value MAX_BURST_SIZE ] + 1 ]]

    set cmd_scfifo_depth [ get_parameter_value CMD_SCFIFO_DEPTH]
    set rsp_scfifo_depth [ get_parameter_value RSP_SCFIFO_DEPTH]
    set max_burst_size [get_parameter_value MAX_BURST_SIZE]
    set master_port_waitrequest_allowance 0
    set bridge_throttle_latency 1
    set min_rsp_scfifo_depth [expr $max_burst_size*($master_port_waitrequest_allowance + $bridge_throttle_latency + 1)]

    # update partmeters
    set_port_property slave_byteenable WIDTH_EXPR $byteen_w
    set_port_property master_byteenable WIDTH_EXPR $byteen_w

    set_port_property slave_burstcount WIDTH_EXPR $burst_w
    set_port_property master_burstcount WIDTH_EXPR $burst_w

    # update interface properties
    set_interface_property master bitsPerSymbol [expr $symbol_w]
    set_interface_property slave  bitsPerSymbol [expr $symbol_w]
    set_interface_property slave  maximumPendingReadTransactions [expr $cmd_scfifo_depth + $rsp_scfifo_depth]

    # display error message if minimum response fifo depth is not met
    if {$rsp_scfifo_depth < $min_rsp_scfifo_depth} {
        set error_message "The minimum response FIFO depth must be $min_rsp_scfifo_depth when Max Burst Size is $max_burst_size"
        send_message ERROR $error_message
    }
}

proc log2ceil {num} {

    set val 0
    set i 1
    while {$i < $num} {
        set val [expr $val + 1]
        set i [expr 1 << $val]
    }

    return $val;
}

