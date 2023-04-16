# TCL File Generated by Component Editor 16.0.1
# Mon Jul 11 14:56:00 EDT 2016
# DO NOT MODIFY


# 
# burst_boundary_splitter "ACL Burst Boundary Splitter" v1.0
# Altera OpenCL 2016.07.11.14:56:00
# Split burst read/writes on burst word boundary
# 

# 
# request TCL package from ACDS 16.0
# 
package require -exact qsys 16.0


# 
# module burst_boundary_splitter
# 
set_module_property DESCRIPTION "Split burst read/writes on burst word boundary"
set_module_property NAME burst_boundary_splitter
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP "ACL Internal Components"
set_module_property AUTHOR "Altera OpenCL"
set_module_property DISPLAY_NAME "ACL Burst Boundary Splitter"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false
set_module_property ELABORATION_CALLBACK elaborate
set_module_property VALIDATION_CALLBACK validate


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL burst_boundary_splitter
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file burst_boundary_splitter.sv SYSTEM_VERILOG PATH burst_boundary_splitter.sv TOP_LEVEL_FILE

add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL burst_boundary_splitter
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file burst_boundary_splitter.sv SYSTEM_VERILOG PATH burst_boundary_splitter.sv TOP_LEVEL_FILE

# 
# parameters
# 
add_parameter WIDTH_D INTEGER 256
set_parameter_property WIDTH_D DEFAULT_VALUE 256
set_parameter_property WIDTH_D DISPLAY_NAME WIDTH_D
set_parameter_property WIDTH_D TYPE INTEGER
set_parameter_property WIDTH_D UNITS None
set_parameter_property WIDTH_D ALLOWED_RANGES -2147483648:2147483647
set_parameter_property WIDTH_D HDL_PARAMETER true

add_parameter BYTE_WIDTH_A INTEGER 31
set_parameter_property BYTE_WIDTH_A DEFAULT_VALUE 31
set_parameter_property BYTE_WIDTH_A DISPLAY_NAME BYTE_WIDTH_A
set_parameter_property BYTE_WIDTH_A TYPE INTEGER
set_parameter_property BYTE_WIDTH_A UNITS None
set_parameter_property BYTE_WIDTH_A ALLOWED_RANGES -2147483648:2147483647
set_parameter_property BYTE_WIDTH_A HDL_PARAMETER false

add_parameter WORD_WIDTH_A INTEGER 26
set_parameter_property WORD_WIDTH_A DEFAULT_VALUE 26
set_parameter_property WORD_WIDTH_A DISPLAY_NAME WORD_WIDTH_A
set_parameter_property WORD_WIDTH_A TYPE INTEGER
set_parameter_property WORD_WIDTH_A UNITS None
set_parameter_property WORD_WIDTH_A ALLOWED_RANGES -2147483648:2147483647
set_parameter_property WORD_WIDTH_A HDL_PARAMETER true
set_parameter_property WORD_WIDTH_A DERIVED true

add_parameter BURSTCOUNT_WIDTH INTEGER 6
set_parameter_property BURSTCOUNT_WIDTH DEFAULT_VALUE 6
set_parameter_property BURSTCOUNT_WIDTH DISPLAY_NAME BURSTCOUNT_WIDTH
set_parameter_property BURSTCOUNT_WIDTH TYPE INTEGER
set_parameter_property BURSTCOUNT_WIDTH UNITS None
set_parameter_property BURSTCOUNT_WIDTH ALLOWED_RANGES -2147483648:2147483647
set_parameter_property BURSTCOUNT_WIDTH HDL_PARAMETER true

add_parameter MAX_PENDING_READS INTEGER 32
set_parameter_property MAX_PENDING_READS DEFAULT_VALUE 32
set_parameter_property MAX_PENDING_READS DISPLAY_NAME MAX_PENDING_READS
set_parameter_property MAX_PENDING_READS TYPE INTEGER
set_parameter_property MAX_PENDING_READS UNITS None
set_parameter_property MAX_PENDING_READS ALLOWED_RANGES -2147483648:2147483647
set_parameter_property MAX_PENDING_READS AFFECTS_ELABORATION true
set_parameter_property MAX_PENDING_READS HDL_PARAMETER false

add_parameter BYTEENABLE_WIDTH INTEGER 32
set_parameter_property BYTEENABLE_WIDTH DEFAULT_VALUE 32
set_parameter_property BYTEENABLE_WIDTH DISPLAY_NAME BYTEENABLE_WIDTH
set_parameter_property BYTEENABLE_WIDTH TYPE INTEGER
set_parameter_property BYTEENABLE_WIDTH UNITS None
set_parameter_property BYTEENABLE_WIDTH HDL_PARAMETER true
set_parameter_property BYTEENABLE_WIDTH DERIVED true


# 
# display items
# 


proc elaborate {} {
  set max_pending_reads [ get_parameter_value MAX_PENDING_READS ]
  
  # 
  # connection point clock
  # 
  add_interface clock clock end
  set_interface_property clock clockRate 0
  set_interface_property clock ENABLED true
  set_interface_property clock EXPORT_OF ""
  set_interface_property clock PORT_NAME_MAP ""
  set_interface_property clock CMSIS_SVD_VARIABLES ""
  set_interface_property clock SVD_ADDRESS_GROUP ""

  add_interface_port clock clk clk Input 1


  # 
  # connection point master
  # 
  add_interface master avalon start
  set_interface_property master addressUnits WORDS
  set_interface_property master associatedClock clock
  set_interface_property master associatedReset reset
  set_interface_property master bitsPerSymbol 8
  set_interface_property master burstOnBurstBoundariesOnly false
  set_interface_property master burstcountUnits WORDS
  set_interface_property master doStreamReads false
  set_interface_property master doStreamWrites false
  set_interface_property master holdTime 0
  set_interface_property master linewrapBursts false
  set_interface_property master maximumPendingReadTransactions 0
  set_interface_property master maximumPendingWriteTransactions 0
  set_interface_property master readLatency 0
  set_interface_property master readWaitTime 0
  set_interface_property master setupTime 0
  set_interface_property master timingUnits Cycles
  set_interface_property master writeWaitTime 0
  set_interface_property master ENABLED true
  set_interface_property master EXPORT_OF ""
  set_interface_property master PORT_NAME_MAP ""
  set_interface_property master CMSIS_SVD_VARIABLES ""
  set_interface_property master SVD_ADDRESS_GROUP ""

  add_interface_port master m_waitrequest_i waitrequest Input 1
  add_interface_port master m_readdata_i readdata Input WIDTH_D
  add_interface_port master m_readdatavalid_i readdatavalid Input 1
  add_interface_port master m_addr_o address Output WORD_WIDTH_A
  add_interface_port master m_writedata_o writedata Output WIDTH_D
  add_interface_port master m_read_o read Output 1
  add_interface_port master m_write_o write Output 1
  add_interface_port master m_burstcount_o burstcount Output BURSTCOUNT_WIDTH
  add_interface_port master m_byteenable_o byteenable Output BYTEENABLE_WIDTH


  # 
  # connection point reset
  # 
  add_interface reset reset end
  set_interface_property reset associatedClock clock
  set_interface_property reset synchronousEdges DEASSERT
  set_interface_property reset ENABLED true
  set_interface_property reset EXPORT_OF ""
  set_interface_property reset PORT_NAME_MAP ""
  set_interface_property reset CMSIS_SVD_VARIABLES ""
  set_interface_property reset SVD_ADDRESS_GROUP ""

  add_interface_port reset resetn reset_n Input 1


  # 
  # connection point slave
  # 
  add_interface slave avalon end
  set_interface_property slave addressUnits WORDS
  set_interface_property slave associatedClock clock
  set_interface_property slave associatedReset reset
  set_interface_property slave bitsPerSymbol 8
  set_interface_property slave bridgedAddressOffset ""
  set_interface_property slave bridgesToMaster ""
  set_interface_property slave burstOnBurstBoundariesOnly false
  set_interface_property slave burstcountUnits WORDS
  set_interface_property slave explicitAddressSpan 0
  set_interface_property slave holdTime 0
  set_interface_property slave linewrapBursts false
  set_interface_property slave maximumPendingReadTransactions $max_pending_reads
  set_interface_property slave maximumPendingWriteTransactions 0
  set_interface_property slave readLatency 0
  set_interface_property slave readWaitTime 1
  set_interface_property slave setupTime 0
  set_interface_property slave timingUnits Cycles
  set_interface_property slave transparentBridge false
  set_interface_property slave writeWaitTime 0
  set_interface_property slave ENABLED true
  set_interface_property slave EXPORT_OF ""
  set_interface_property slave PORT_NAME_MAP ""
  set_interface_property slave CMSIS_SVD_VARIABLES ""
  set_interface_property slave SVD_ADDRESS_GROUP ""

  add_interface_port slave s_writedata_i writedata Input WIDTH_D
  add_interface_port slave s_read_i read Input 1
  add_interface_port slave s_write_i write Input 1
  add_interface_port slave s_burstcount_i burstcount Input BURSTCOUNT_WIDTH
  add_interface_port slave s_byteenable_i byteenable Input BYTEENABLE_WIDTH
  add_interface_port slave s_waitrequest_o waitrequest Output 1
  add_interface_port slave s_readdata_o readdata Output WIDTH_D
  add_interface_port slave s_readdatavalid_o readdatavalid Output 1
  add_interface_port slave s_addr_i address Input WORD_WIDTH_A
  set_interface_assignment slave embeddedsw.configuration.isFlash 0
  set_interface_assignment slave embeddedsw.configuration.isMemoryDevice 0
  set_interface_assignment slave embeddedsw.configuration.isNonVolatileStorage 0
  set_interface_assignment slave embeddedsw.configuration.isPrintableDevice 0
}

proc validate {} {
   set byte_width_a [ get_parameter_value BYTE_WIDTH_A ]
   set width_d [ get_parameter_value WIDTH_D ]
   set byteenable_width [ expr $width_d / 8 ]
   set word_width_a [ expr int ( $byte_width_a - (ceil(log($width_d / 8)/log(2))) ) ]

    set_parameter_value BYTEENABLE_WIDTH $byteenable_width
    set_parameter_value WORD_WIDTH_A $word_width_a
}
