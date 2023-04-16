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


# This TCL script creates a qsys simulation system which includes all the OpenCL simulation board BFM's
# It can generate with and without the kernel system
#
# Default settings: Simulation board only, single 2GB memory bank
#  GLB_MEM_AV_ADDRESS_WIDTH : if defined, sets global memory address width. Default is 2GB 
#  USE_SNOOP                : if 1, kernel system can use snoop port in memory bank divider. Depends on BSP
#  HOST_TO_DEV_READ_WIDTH   : set to 0 unless board supports host channel
#  DEV_TO_HOST_WRITE_WIDTH  : set to 0 unless board supports host channel
#  INCL_KERNEL_SYSTEM       : if defined, it means OpenCL invoked qsys on kernel_system.tcl to create a kernel_system.qsys
#                           : expected to be obseleted in future release
#  GLB_MEM_AV_WAITREQUEST_ALLOWANCE : if defined, sets the wait request allowance value on memory side and use S10 Kernel Interface.
#                                     Depends on device. Default is 0.
# TODO's
# - Host-device memory transfers via Memory bank divider

package require -exact qsys 17.0

proc assert {cond {msg "assertion failed"}} {
    if {![uplevel 1 expr $cond]} {error $msg}
}

proc instantiate_dpi_bfm { bfm_inst_name bfm_shortname bfm_type } {
    add_component ${bfm_inst_name} ${bfm_shortname}.ip ${bfm_type} ${bfm_shortname}
}

proc get_cosim_component_name {component } {

    load_component ${component}
    set cosim_component [get_component_assignment hls.cosim.name]

}

proc dpi_bfm_set_param { instance_name param param_value args} {
    load_component $instance_name
    set_component_parameter_value $param $param_value
    foreach {name value} $args {
        set_component_parameter_value $name $value
    }
    save_component
}

proc instantiate_dpi_bfm_and_connect_to_avalon_master { component interface index} {

    set readwrite_mode "rw"
    
    set cosim_component "avs"
    set instance_name osm_${index}_inst
    instantiate_dpi_bfm $instance_name mm_slave_${component}_${interface} aoc_sim_mm_slave_dpi_bfm

    set AV_ADDRESS_W [get_instance_interface_port_property ${component} ${interface} ${interface}_address WIDTH]

    # symbol width in bits, symbol is one byte
    set AV_SYMBOL_W 8
    set USE_WAIT_REQUEST 0
    set USE_BURSTCOUNT 0
    set USE_READ_DATA_VALID 0
    set AV_MAX_PENDING_READS 0
    set USE_READ 1
    set USE_READ_DATA 1
    set USE_WRITE 1
    set USE_WRITE_DATA 1

    if {$readwrite_mode == "w"} {
        set AV_DATA_W [get_instance_interface_port_property ${component} ${interface} ${interface}_writedata WIDTH]
        set USE_READ 0 
        set USE_READ_DATA 0
    } else {
        set AV_DATA_W [get_instance_interface_port_property ${component} ${interface} ${interface}_readdata WIDTH]
        if {$readwrite_mode == "r"} {
            set USE_WRITE 0
            set USE_WRITE_DATA 0
        }
        
        # only used if the interface is variable latency
        set AV_FIX_READ_LATENCY [get_instance_interface_parameter_value ${component} $interface readLatency]
        dpi_bfm_set_param $instance_name AV_FIX_READ_LATENCY $AV_FIX_READ_LATENCY        
    }
    set AV_NUMSYMBOLS [ expr {$AV_DATA_W / $AV_SYMBOL_W}]
    set AV_NUMSYMBOLS_REM [ expr {$AV_DATA_W % $AV_SYMBOL_W}]
    assert {$AV_NUMSYMBOLS>0 && $AV_NUMSYMBOLS_REM==0} "AV_DATA_W must be a multiple of AV_SYMBOL_W=$AV_SYMBOL_W"

    set AV_BYTEENABLE_W [get_instance_interface_port_property ${component} ${interface} ${interface}_byteenable WIDTH]
    assert {$AV_NUMSYMBOLS==$AV_BYTEENABLE_W} "AV_NUMSYMBOLS=$AV_NUMSYMBOLS must be equal to the width of the byteenable signal ($AV_BYTEENABLE_W)"
    
    set ADDRESS_UNITS [get_instance_interface_parameter_value ${component} $interface addressUnits]

    set portArr [get_instance_interface_ports ${component} ${interface}]
    set burstcountPort ${interface}_burstcount
    set waitrequestPort ${interface}_waitrequest
    set readdatavalidPort ${interface}_readdatavalid
    foreach elem $portArr {
        if {$elem == $burstcountPort} {
            set AV_BURSTCOUNT_W [get_instance_interface_port_property ${component} ${interface} ${interface}_burstcount WIDTH]
            dpi_bfm_set_param $instance_name AV_BURSTCOUNT_W $AV_BURSTCOUNT_W 
            set USE_BURSTCOUNT 1
        } elseif {$elem == $waitrequestPort} {
            set USE_WAIT_REQUEST 1
        } elseif {$elem == $readdatavalidPort} {
            set USE_READ_DATA_VALID 1
            set AV_MAX_PENDING_READS 256
        }
    }

    dpi_bfm_set_param $instance_name INTERFACE_ID   [expr $index] COMPONENT_NAME $cosim_component AV_ADDRESS_W $AV_ADDRESS_W AV_NUMSYMBOLS $AV_NUMSYMBOLS AV_SYMBOL_W $AV_SYMBOL_W ADDRESS_UNITS $ADDRESS_UNITS USE_WAIT_REQUEST $USE_WAIT_REQUEST USE_BURSTCOUNT $USE_BURSTCOUNT \
                                     USE_READ_DATA_VALID $USE_READ_DATA_VALID USE_READ $USE_READ USE_READ_DATA $USE_READ_DATA USE_WRITE $USE_WRITE USE_WRITE_DATA $USE_WRITE_DATA USE_READ_DATA_VALID $USE_READ_DATA_VALID AV_MAX_PENDING_READS $AV_MAX_PENDING_READS

    add_connection clk_rst.clock         $instance_name.clock
    add_connection clk_rst.reset         $instance_name.reset
    add_connection ${component}.${interface} $instance_name.s0

}

create_system mpsim

# Basic clock and reset for the simulation
# generate the whole system - put clock reset as part of testbench
add_instance clk_rst hls_sim_clock_reset
set_instance_parameter_value clk_rst RESET_CYCLE_HOLD    120

# Main OpenCL controller for the Kernel Interface and Memory Divider
add_instance oirq aoc_sim_main_dpi_controller


# Kernel Interface that controls the OpenCL kernels
if [info exists GLB_MEM_AV_WAITREQUEST_ALLOWANCE] {
  add_instance ki kernel_interface_s10
} else {
  add_instance ki kernel_interface
}

# OpenCL Simulator Interface master BFM to control:
#   - Kernel Interface (will poll host process) 
#   - Memory Bank Divider (will poll host-device memory transfers)
# Default values for USE_WAIT_REQUEST's, USE_READ_DATA_VALID, USE_BURSTCOUNT
# matches with associated interface. We only set values that is different
add_instance osi aoc_sim_mm_master_dpi_bfm
set_instance_parameter_value osi KI_AV_ADDRESS_W    14
set_instance_parameter_value osi KI_AV_SYMBOL_W      8
set_instance_parameter_value osi KI_AV_NUMSYMBOLS    4
set_instance_parameter_value osi MBD_AV_ADDRESS_W   31
set_instance_parameter_value osi MBD_AV_SYMBOL_W     8
set_instance_parameter_value osi MBD_AV_NUMSYMBOLS   4


# Setup all connections for clock and resets to all the components and connections to kernel system
# The generated OpenCL kernels or the empty kernel system
add_instance ks kernel_system

# Set parameter for snoop port and connect it
if [info exists USE_SNOOP] {
  if ![info exists INCL_KERNEL_SYSTEM] {
    # Empty kernel system
    set_instance_parameter_value ks USE_SNOOP  $USE_SNOOP
    add_connection clk_rst.clock ks.cc_snoop_clk_clk
  } else {
    # 18.0 and 18.1 flow - use kernel_system.qsys
    add_connection clk_rst.clock ks.cc_snoop_clk 
  }
}

# Connect the AOC main controller
add_connection clk_rst.clock oirq.clock
add_connection clk_rst.clock2x oirq.clock2x
add_connection clk_rst.reset oirq.reset
add_connection oirq.reset_ctrl clk_rst.reset_ctrl

# Connect all the OCL kernel system
if ![info exists INCL_KERNEL_SYSTEM] {
  add_connection clk_rst.clock ks.clock_reset_clk
  add_connection clk_rst.clock2x ks.clock_reset2x_clk
  add_connection clk_rst.reset ks.clock_reset_reset_reset_n
  add_connection ki.kernel_cra ks.kernel_cra
  add_connection ki.kernel_irq_from_kernel ks.kernel_irq_irq
  # Workaround for host channel support as empty kernel system is not dynamically generated
  set_instance_parameter_value ks HOST_TO_DEV_READ_WIDTH $HOST_TO_DEV_READ_WIDTH
  set_instance_parameter_value ks DEV_TO_HOST_WRITE_WIDTH $DEV_TO_HOST_WRITE_WIDTH
} else {
  add_connection clk_rst.clock ks.clock_reset
  add_connection clk_rst.clock2x ks.clock_reset2x
  add_connection clk_rst.reset ks.clock_reset_reset
  add_connection ki.kernel_cra ks.kernel_cra
  add_connection ki.kernel_irq_from_kernel ks.kernel_irq
}

# Connect the kernel interface (KI)
add_connection clk_rst.clock ki.clk
add_connection clk_rst.clock ki.kernel_clk
add_connection clk_rst.reset ki.reset
add_connection clk_rst.reset ki.sw_reset_in


# Connect the Simulator Interface BFM master
add_connection clk_rst.clock osi.clock
add_connection clk_rst.reset osi.reset

auto_assign_irqs osi

add_connection osi.m_ki ki.ctrl
add_connection oirq.kernel_interrupt ki.kernel_irq_to_host


set mem_index 0
# iterate through interfaces to find streaming connections, which are connected to simulation BFMs
foreach interface [get_instance_interfaces ks] {     
  set class_name [get_instance_interface_property ks $interface CLASS_NAME]

  if [string compare $interface cc_snoop]!=0 {
    #dont connect BFM to snoop port
    if { $class_name=="avalon_master"} {
       instantiate_dpi_bfm_and_connect_to_avalon_master ks $interface $mem_index
       incr mem_index 1
    }
    
    if [string compare $class_name avalon_streaming_sink]==0 {      
      # add streaming source BFM
      set symbolsPerBeat [get_instance_interface_parameter_value ks $interface symbolsPerBeat]
      set data_width [ expr $symbolsPerBeat * 8]
      set firstSymbolInHighOrderBits [get_instance_interface_parameter_value ks $interface firstSymbolInHighOrderBits]
     
      set instance_name sso_${interface}_inst
      set instance_shortname sso_${interface}
      
      add_component ${instance_name} ${instance_shortname}.ip aoc_sim_stream_source_dpi_bfm ${instance_shortname}
      
      add_connection clk_rst.clock      ${instance_name}.clock
      add_connection clk_rst.clock2x    ${instance_name}.clock2x
      add_connection clk_rst.reset      ${instance_name}.reset
      
      load_component ${instance_name}
      set_component_parameter_value  COMPONENT_NAME $interface
      set_component_parameter_value  INTERFACE_NAME "sink"
      set_component_parameter_value  STREAM_DATAWIDTH $data_width
      set_component_parameter_value  STREAM_BITSPERSYMBOL 8
      set_component_parameter_value  FIRST_SYMBOL_IN_HIGH_ORDER_BITS $firstSymbolInHighOrderBits
      save_component ${instance_name}
      
      add_connection   ${instance_name}.source  ks.$interface 
    }
    if [string compare $class_name avalon_streaming_source]==0 {
      set symbolsPerBeat [get_instance_interface_parameter_value ks $interface symbolsPerBeat]
      set data_width [ expr $symbolsPerBeat * 8]
      set firstSymbolInHighOrderBits [get_instance_interface_parameter_value ks $interface firstSymbolInHighOrderBits]
      # add streaming sink BFM
      set instance_name ssi_${interface}_inst
      set instance_shortname ssi_${interface}
      
      add_component ${instance_name} ${instance_shortname}.ip aoc_sim_stream_sink_dpi_bfm ${instance_shortname}
      
      add_connection clk_rst.clock      ${instance_name}.clock
      add_connection clk_rst.clock2x    ${instance_name}.clock2x
      add_connection clk_rst.reset      ${instance_name}.reset
      
      load_component ${instance_name}
      set_component_parameter_value  COMPONENT_NAME $interface
      set_component_parameter_value  INTERFACE_NAME "sink"
      set_component_parameter_value  STREAM_DATAWIDTH $data_width
      set_component_parameter_value  STREAM_BITSPERSYMBOL 8
      set_component_parameter_value  FIRST_SYMBOL_IN_HIGH_ORDER_BITS $firstSymbolInHighOrderBits
      save_component ${instance_name}
      
      add_connection   ks.$interface  ${instance_name}.sink
    }   
  }
}

# Generate memory bank divider if we arent using the default simulation flow, 
# and may need to use front door loading for memory model
if ![info exists INCL_KERNEL_SYSTEM] {
  # Memory Bank Divider that handles multiple banks of memory.
  add_instance mbd acl_memory_bank_divider
  set_instance_parameter_value mbd NUM_BANKS 1
  set_instance_parameter_value mbd DATA_WIDTH 512
  # Connect the memory bank divider
  add_connection ki.kernel_reset mbd.kernel_reset
  # add_connection mbd.acl_bsp_memorg_host ki.acl_bsp_memorg_host0x018
  if [info exists USE_SNOOP] {
    add_connection mbd.acl_bsp_snoop ks.cc_snoop
  }
  # TODO: this is only for front door loading
  #add_connection mbd.bank1 osm0.s0
  # TODO: add more banks to support interleaving
  set_interface_property mbd_bank1 EXPORT_OF mbd.bank1
  add_connection clk_rst.clock mbd.clk
  add_connection clk_rst.clock mbd.kernel_clk
  add_connection clk_rst.reset mbd.reset
  set_instance_parameter_value mbd NUM_BANKS 1
  add_connection osi.m_mbd mbd.s
}

# And save the whole thing
sync_sysinfo_parameters
save_system mpsim
