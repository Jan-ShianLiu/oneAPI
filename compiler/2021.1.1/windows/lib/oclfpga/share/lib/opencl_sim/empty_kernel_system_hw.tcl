package require -exact qsys 18.0
set_module_property NAME          kernel_system
set_module_property VERSION       14.0
set_module_property INTERNAL      false
set_module_property GROUP         "OpenCL Simulation"
set_module_property DISPLAY_NAME  "Empty Kernel System"
set_module_property EDITABLE      false
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property ELABORATION_CALLBACK         elaborate

set USE_SNOOP                      "USE_SNOOP"
add_parameter $USE_SNOOP Integer 0
set_parameter_property $USE_SNOOP AFFECTS_ELABORATION true
set_parameter_property $USE_SNOOP DISPLAY_HINT boolean
set_parameter_property $USE_SNOOP HDL_PARAMETER false

set HOST_TO_DEV_READ_WIDTH         "HOST_TO_DEV_READ_WIDTH"
add_parameter $HOST_TO_DEV_READ_WIDTH Integer 0
set_parameter_property $HOST_TO_DEV_READ_WIDTH AFFECTS_ELABORATION true
set_parameter_property $HOST_TO_DEV_READ_WIDTH DISPLAY_HINT integer
set_parameter_property $HOST_TO_DEV_READ_WIDTH HDL_PARAMETER false

set DEV_TO_HOST_WRITE_WIDTH         "DEV_TO_HOST_WRITE_WIDTH"
add_parameter $DEV_TO_HOST_WRITE_WIDTH Integer 0
set_parameter_property $DEV_TO_HOST_WRITE_WIDTH AFFECTS_ELABORATION true
set_parameter_property $DEV_TO_HOST_WRITE_WIDTH DISPLAY_HINT integer
set_parameter_property $DEV_TO_HOST_WRITE_WIDTH HDL_PARAMETER false

# This just helps get rid of unnecessary QSys generated empty file
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL kernel_system
add_fileset_file kernel_system.v SYSTEM_VERILOG PATH kernel_system.v TOP_LEVEL_FILE

proc elaborate {} {
  global USE_SNOOP
  global HOST_TO_DEV_READ_WIDTH
  global DEV_TO_HOST_WRITE_WIDTH
  set USE_SNOOP_VALUE            [ get_parameter $USE_SNOOP ]
  set HOST_TO_DEV_READ_W_VALUE   [ get_parameter $HOST_TO_DEV_READ_WIDTH ]
  set DEV_TO_HOST_WRITE_W_VALUE  [ get_parameter $DEV_TO_HOST_WRITE_WIDTH ]
  # Interface Names
  # values comes from boardspec.xml
  #---------------------------------------------------------------------
  #  <interface name="board" port="kernel_cra" type="master" width="64" misc="0" waitrequest_allowance="5"/> 
  #  <interface name="board" port="kernel_irq" type="irq" width="1"/>
  #  <kernel_clk_reset clk="board.kernel_clk" clk2x="board.kernel_clk2x" reset="board.kernel_reset"/>
  #
  # Global memory section, i.e. HBM port names
  #  <global_mem name="HBM" max_bandwidth="14928" interleaved_bytes="1024" config_addr="0x018">
  #    <interface name="board" port="kernel_slave_00" type="slave" width="256" maxburst="16" address="0x00000000" size="0x80000000" latency="240" waitrequest_allowance="6"/>
  set KERNEL_SLV_INTFC       "kernel_cra"
  set IRQ                    "kernel_irq_irq"
  set CLOCK_INTERFACE        "clock_reset_clk"
  set CLOCK2x_INTERFACE      "clock_reset2x_clk"
  set RESET_INTERFACE        "clock_reset_reset_reset_n"
  set MEM_SLV_ST_INTFC       "cc_snoop"
  set MEM_MST_INTFC_PREFIX   "kernel_mem"
	set NUM_BANKS              1
  set HOST_TO_DEV_INTERFACE  "host_to_dev_read"
  set DEV_TO_HOST_INTERFACE  "dev_to_host_write"

  # add clock, clock2x, and reset
  add_interface $CLOCK_INTERFACE clock end
  add_interface_port $CLOCK_INTERFACE $CLOCK_INTERFACE clk Input 1

  add_interface $CLOCK2x_INTERFACE clock end
  add_interface_port $CLOCK2x_INTERFACE $CLOCK2x_INTERFACE clk Input 1

  add_interface $RESET_INTERFACE reset end
  set_interface_property $RESET_INTERFACE associatedClock $CLOCK_INTERFACE    
  add_interface_port $RESET_INTERFACE $RESET_INTERFACE reset_n Input 1

  # add cra interface and set its properties
  set CRA_DATA_W_VALUE   64
  add_interface $KERNEL_SLV_INTFC avalon end
  set_interface_property $KERNEL_SLV_INTFC ASSOCIATED_CLOCK $CLOCK_INTERFACE
  set_interface_property $KERNEL_SLV_INTFC addressUnits "SYMBOLS"
  set_interface_property $KERNEL_SLV_INTFC maximumPendingWriteTransactions 0
  set_interface_property $KERNEL_SLV_INTFC maximumPendingReadTransactions 1
  set_interface_property $KERNEL_SLV_INTFC readLatency 0
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_writedata writedata Input $CRA_DATA_W_VALUE
  set_port_property ${KERNEL_SLV_INTFC}_writedata VHDL_TYPE STD_LOGIC_VECTOR
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_burstcount burstcount Input 1
  set_port_property ${KERNEL_SLV_INTFC}_burstcount VHDL_TYPE STD_LOGIC_VECTOR
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_readdata readdata Output $CRA_DATA_W_VALUE
  set_port_property ${KERNEL_SLV_INTFC}_readdata VHDL_TYPE STD_LOGIC_VECTOR
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_address address Input 30
  set_port_property ${KERNEL_SLV_INTFC}_address VHDL_TYPE STD_LOGIC_VECTOR
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_waitrequest waitrequest Output 1
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_write write Input 1
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_read read Input 1
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_byteenable byteenable Input 8
  set_port_property ${KERNEL_SLV_INTFC}_byteenable VHDL_TYPE STD_LOGIC_VECTOR
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_readdatavalid readdatavalid Output 1
  add_interface_port $KERNEL_SLV_INTFC ${KERNEL_SLV_INTFC}_debugaccess debugaccess Input 1

  # add irq
  add_interface $IRQ interrupt transmitter
  add_interface_port $IRQ $IRQ irq Output 1
  set_interface_property $IRQ ASSOCIATED_CLOCK $CLOCK_INTERFACE

  # add memory interface - base on the number of rolls in global_mem section
  set MEM_DATA_W_VALUE       512
  set MEM_BURSTCOUNT_W_VALUE 5
  set MEM_ADDRESS_W_VALUE    31
  set MEM_NUMSYMBOLS_VALUE   64
  set MEM_SLV_ST_W_VALUE     31
  for {set i 0} {$i < $NUM_BANKS} {incr i} {
    set MEM_MST_INTFC  "${MEM_MST_INTFC_PREFIX}${i}"
    add_interface $MEM_MST_INTFC avalon master

    set_interface_property $MEM_MST_INTFC ASSOCIATED_CLOCK $CLOCK_INTERFACE
    set_interface_property $MEM_MST_INTFC addressUnits "SYMBOLS"
    set_interface_property $MEM_MST_INTFC maximumPendingWriteTransactions 0
    set_interface_property $MEM_MST_INTFC maximumPendingReadTransactions 0
    set_interface_property $MEM_MST_INTFC readLatency 0
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_writedata writedata Output $MEM_DATA_W_VALUE
    set_port_property ${MEM_MST_INTFC}_writedata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_burstcount burstcount Output $MEM_BURSTCOUNT_W_VALUE
    set_port_property ${MEM_MST_INTFC}_burstcount VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_readdata readdata Input $MEM_DATA_W_VALUE
    set_port_property ${MEM_MST_INTFC}_readdata VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_address address Output $MEM_ADDRESS_W_VALUE
    set_port_property ${MEM_MST_INTFC}_address VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_waitrequest waitrequest Input 1
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_write write Output 1
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_read read Output 1
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_byteenable byteenable Output $MEM_NUMSYMBOLS_VALUE
    set_port_property ${MEM_MST_INTFC}_byteenable VHDL_TYPE STD_LOGIC_VECTOR
    add_interface_port $MEM_MST_INTFC ${MEM_MST_INTFC}_readdatavalid readdatavalid Input 1
  }

  # board supports snoop ports and snoop clk
  if {$USE_SNOOP_VALUE > 0} {
    set MEM_SLV_ST_CLK ${MEM_SLV_ST_INTFC}_clk_clk
    add_interface $MEM_SLV_ST_CLK clock end
    add_interface_port $MEM_SLV_ST_CLK $MEM_SLV_ST_CLK clk Input 1
    add_interface $MEM_SLV_ST_INTFC avalon_streaming end
    set_interface_property $MEM_SLV_ST_INTFC associatedClock $MEM_SLV_ST_CLK
    set_interface_property $MEM_SLV_ST_INTFC associatedReset $RESET_INTERFACE
    set_interface_property $MEM_SLV_ST_INTFC dataBitsPerSymbol $MEM_SLV_ST_W_VALUE
    set_interface_property $MEM_SLV_ST_INTFC errorDescriptor ""
    set_interface_property $MEM_SLV_ST_INTFC firstSymbolInHighOrderBits true
    set_interface_property $MEM_SLV_ST_INTFC maxChannel 0
    set_interface_property $MEM_SLV_ST_INTFC readyLatency 0
    set_interface_property $MEM_SLV_ST_INTFC ENABLED true
    add_interface_port $MEM_SLV_ST_INTFC ${MEM_SLV_ST_INTFC}_data data Input $MEM_SLV_ST_W_VALUE
    add_interface_port $MEM_SLV_ST_INTFC ${MEM_SLV_ST_INTFC}_valid valid Input 1
    add_interface_port $MEM_SLV_ST_INTFC ${MEM_SLV_ST_INTFC}_ready ready Output 1
  }

  if {$HOST_TO_DEV_READ_W_VALUE > 0} {
    #### Channel (Avalon_ST) interface avm_channel_id_host_to_dev_read_*(host_to_dev_*)
    add_interface $HOST_TO_DEV_INTERFACE avalon_streaming sink
    set_interface_property $HOST_TO_DEV_INTERFACE associatedClock $CLOCK_INTERFACE
    set_interface_property $HOST_TO_DEV_INTERFACE  symbolsPerBeat $HOST_TO_DEV_READ_W_VALUE
    set_interface_property $HOST_TO_DEV_INTERFACE  dataBitsPerSymbol 8
    set_interface_property $HOST_TO_DEV_INTERFACE  firstSymbolInHighOrderBits true
    set_interface_property $HOST_TO_DEV_INTERFACE  maxChannel 0
    set_interface_property $HOST_TO_DEV_INTERFACE  readyLatency 0
    add_interface_port $HOST_TO_DEV_INTERFACE ${HOST_TO_DEV_INTERFACE}_data data input 256
    add_interface_port $HOST_TO_DEV_INTERFACE ${HOST_TO_DEV_INTERFACE}_ready ready output 1
    add_interface_port $HOST_TO_DEV_INTERFACE ${HOST_TO_DEV_INTERFACE}_valid valid input 1
  }
  if {$DEV_TO_HOST_WRITE_W_VALUE > 0} {
    #### Channel (Avalon_ST) interface avm_channel_id_dev_to_host_write_*(dev_to_host_*)
    add_interface $DEV_TO_HOST_INTERFACE avalon_streaming source
    set_interface_property $DEV_TO_HOST_INTERFACE associatedClock $CLOCK_INTERFACE
    set_interface_property $DEV_TO_HOST_INTERFACE  symbolsPerBeat DEV_TO_HOST_WRITE_W_VALUE
    set_interface_property $DEV_TO_HOST_INTERFACE  dataBitsPerSymbol 8
    set_interface_property $DEV_TO_HOST_INTERFACE  firstSymbolInHighOrderBits true
    set_interface_property $DEV_TO_HOST_INTERFACE  maxChannel 0
    set_interface_property $DEV_TO_HOST_INTERFACE  readyLatency 0
    add_interface_port $DEV_TO_HOST_INTERFACE ${DEV_TO_HOST_INTERFACE}_data data output 256
    add_interface_port $DEV_TO_HOST_INTERFACE ${DEV_TO_HOST_INTERFACE}_ready ready input 1
    add_interface_port $DEV_TO_HOST_INTERFACE ${DEV_TO_HOST_INTERFACE}_valid valid output 1
  }
}
