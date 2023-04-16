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


# This TCL script creates a qsys simulation system which includes all the hls generated components
# The qsys system also includes the main DPI controller and per-component DPI cosimulation drivers

package require -exact qsys 17.0

# "quartus_pro", "num_reset_cycles", "sim_qsys", and "component_list" variables have already been set by a previous TCL command

# -----------------------------------------------------------------------------
# Debugging
# -----------------------------------------------------------------------------
set DEBUG_MSGS 0

# -----------------------------------------------------------------------------
# Define procedures used to instantiate and connect the DPI controllers/drivers
# -----------------------------------------------------------------------------

proc debug { s } {
    global DEBUG_MSGS
    if { $DEBUG_MSGS } {
        puts $s
    }
}

proc assert {cond {msg "assertion failed"}} {
    if {![uplevel 1 expr $cond]} {error $msg}
}

proc get_cosim_component_name {component } {
    global quartus_pro
    if {$quartus_pro == 1} {
        load_component ${component}_inst
        set cosim_component [get_component_assignment hls.cosim.name]
    } else {
        set cosim_component [get_composed_instance_assignment ${component}_inst ${component}_internal_inst hls.cosim.name]
    }
    return $cosim_component
}

proc get_compressed_component_name {component } {
    global quartus_pro
    if {$quartus_pro == 1} {
        load_component ${component}_inst
        set compressed_component [get_component_assignment hls.compressed.name]
    } else {
        set compressed_component [get_composed_instance_assignment ${component}_inst ${component}_internal_inst hls.compressed.name]
    }
    return $compressed_component
}

proc instantiate_dpi_bfm { bfm_inst_name bfm_shortname bfm_type } {
    global quartus_pro
    if {$quartus_pro == 1} {
        add_component ${bfm_inst_name} ${bfm_shortname}.ip ${bfm_type} ${bfm_shortname}
    } else {
        add_instance ${bfm_inst_name} ${bfm_type}
    }
}

proc dpi_bfm_set_param { instance_name param param_value args} {
    global quartus_pro
    if {$quartus_pro == 1} {
        load_component $instance_name
        set_component_parameter_value $param $param_value
        foreach {name value} $args {
            set_component_parameter_value $name $value
        }
        save_component
    } else {
        set_instance_parameter_value $instance_name $param $param_value
        foreach {name value} $args {
            set_instance_parameter_value $instance_name $name $value
        }
    }
}

proc instantiate_dpi_bfm_and_connect_to_implicit_stream_sink { component interface } {
    set compressed_component [get_compressed_component_name $component]
    set cosim_interface [get_instance_interface_assignment ${component}_inst ${interface} hls.cosim.name]
    set instance_name stream_source_dpi_bfm_${component}_${interface}_inst
    instantiate_dpi_bfm ${instance_name} sso_${component}_${interface} hls_sim_stream_source_dpi_bfm

    add_connection clock_reset_inst.clock      ${instance_name}.clock
    add_connection clock_reset_inst.clock2x    ${instance_name}.clock2x
    add_connection clock_reset_inst.reset      ${instance_name}.reset

    set stream_width [get_instance_interface_port_property ${component}_inst ${interface} ${interface} WIDTH]
    dpi_bfm_set_param $instance_name FORCE_STREAM_CONDUIT 1 STREAM_DATAWIDTH $stream_width INTERFACE_NAME $cosim_interface COMPONENT_NAME $compressed_component

    add_connection stream_source_dpi_bfm_${component}_${interface}_inst.source_data ${component}_inst.${interface}
}

proc instantiate_dpi_bfm_and_connect_to_stream_sink { component interface } {
    set compressed_component [get_compressed_component_name $component]
    set cosim_interface [get_instance_interface_assignment ${component}_inst ${interface} hls.cosim.name]

    # Save the instance name in a variable, for multiple references later
    set instance_name stream_source_dpi_bfm_${component}_${interface}_inst
    instantiate_dpi_bfm ${instance_name} sso_${component}_${interface} hls_sim_stream_source_dpi_bfm

    add_connection clock_reset_inst.clock      ${instance_name}.clock
    add_connection clock_reset_inst.clock2x    ${instance_name}.clock2x
    add_connection clock_reset_inst.reset      ${instance_name}.reset

    set stream_width [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_data WIDTH]
    set stream_ports [get_instance_interface_ports ${component}_inst ${interface}]
    if { [lsearch $stream_ports ${interface}_startofpacket] >= 0 } {
    	dpi_bfm_set_param ${instance_name} USES_PACKETS 1
        if { [lsearch $stream_ports ${interface}_empty] >= 0 } {
            set stream_bitspersymbol [get_instance_interface_parameter_value ${component}_inst ${interface} dataBitsPerSymbol]
            set empty_width [expr int(ceil(log($stream_width / $stream_bitspersymbol) / log(2)))]
            dpi_bfm_set_param ${instance_name} EMPTY_WIDTH $empty_width
        }
    }

    dpi_bfm_set_param ${instance_name} STREAM_DATAWIDTH $stream_width INTERFACE_NAME $cosim_interface COMPONENT_NAME $compressed_component
    if {$stream_width <= 4096} {
        set stream_bitspersymbol [get_instance_interface_parameter_value ${component}_inst ${interface} dataBitsPerSymbol]
        dpi_bfm_set_param ${instance_name} STREAM_BITSPERSYMBOL $stream_bitspersymbol
        
        set firstsymbolinhighorderbits [get_instance_interface_parameter_value ${component}_inst ${interface} firstSymbolInHighOrderBits]
        dpi_bfm_set_param ${instance_name} FIRST_SYMBOL_IN_HIGH_ORDER_BITS $firstsymbolinhighorderbits
    }

    add_connection ${instance_name}.source ${component}_inst.${interface}
}

proc instantiate_dpi_bfm_and_connect_to_stream_source { component interface } {
    set compressed_component [get_compressed_component_name $component]
    set cosim_interface [get_instance_interface_assignment ${component}_inst ${interface} hls.cosim.name]

    # Save the instance name in a variable, for multiple references later
    set stream_sink_inst stream_sink_dpi_bfm_${component}_${interface}_inst
    set stream_width [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_data WIDTH]
    set ready_latency [get_instance_interface_parameter_value ${component}_inst ${interface} readyLatency]
    set stream_ports [get_instance_interface_ports ${component}_inst ${interface}]

    instantiate_dpi_bfm ${stream_sink_inst} ssi_${component}_${interface} hls_sim_stream_sink_dpi_bfm

    add_connection clock_reset_inst.clock      ${stream_sink_inst}.clock
    add_connection clock_reset_inst.clock2x    ${stream_sink_inst}.clock2x
    add_connection clock_reset_inst.reset      ${stream_sink_inst}.reset

    if { [lsearch $stream_ports ${interface}_startofpacket] >= 0 } {
        dpi_bfm_set_param ${stream_sink_inst} USES_PACKETS 1
        if { [lsearch $stream_ports ${interface}_empty] >= 0 } {
            set stream_bitspersymbol [get_instance_interface_parameter_value ${component}_inst ${interface} dataBitsPerSymbol]
            set empty_width [expr int(ceil(log($stream_width / $stream_bitspersymbol) / log(2)))]
            dpi_bfm_set_param ${stream_sink_inst} EMPTY_WIDTH $empty_width
        }
    }

    dpi_bfm_set_param ${stream_sink_inst} STREAM_DATAWIDTH ${stream_width} READY_LATENCY ${ready_latency} INTERFACE_NAME ${cosim_interface} COMPONENT_NAME ${compressed_component}
    if {$stream_width <= 4096} { # since dataBitsPerSymbol is only defined when bus width <= 4096 bits
        set stream_bitspersymbol [get_instance_interface_parameter_value ${component}_inst ${interface} dataBitsPerSymbol]
    	dpi_bfm_set_param ${stream_sink_inst} STREAM_BITSPERSYMBOL $stream_bitspersymbol
        
        set firstsymbolinhighorderbits [get_instance_interface_parameter_value ${component}_inst ${interface} firstSymbolInHighOrderBits]
        dpi_bfm_set_param ${stream_sink_inst} FIRST_SYMBOL_IN_HIGH_ORDER_BITS $firstsymbolinhighorderbits
    }

    add_connection ${component}_inst.${interface} ${stream_sink_inst}.sink
}

proc instantiate_dpi_bfm_and_connect_to_avalon_master { component interface } {
    set compressed_component [get_compressed_component_name $component]

    #assert [regexp {(avm_memgmem0_(\d+)_port_.*)} $interface full tmp id] "Unexpected name for Avalon master interface"
    regexp {avmm_([0-9]+)_(.*)} $interface full id readwrite_mode

    set instance_name mm_slave_dpi_bfm_${component}_${interface}_inst
    instantiate_dpi_bfm $instance_name mm_slave_${component}_${interface} hls_sim_mm_slave_dpi_bfm

    set AV_ADDRESS_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_address WIDTH]

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
        set AV_DATA_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_writedata WIDTH]
        set USE_READ 0 
        set USE_READ_DATA 0
    } else {
        set AV_DATA_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_readdata WIDTH]
        if {$readwrite_mode == "r"} {
            set USE_WRITE 0
            set USE_WRITE_DATA 0
        }
        
        # only used if the interface is variable latency
        set AV_FIX_READ_LATENCY [get_instance_interface_parameter_value ${component}_inst $interface readLatency]
        dpi_bfm_set_param $instance_name AV_FIX_READ_LATENCY $AV_FIX_READ_LATENCY        
    }
    set AV_NUMSYMBOLS [ expr {$AV_DATA_W / $AV_SYMBOL_W}]
    set AV_NUMSYMBOLS_REM [ expr {$AV_DATA_W % $AV_SYMBOL_W}]
    assert {$AV_NUMSYMBOLS>0 && $AV_NUMSYMBOLS_REM==0} "AV_DATA_W must be a multiple of AV_SYMBOL_W=$AV_SYMBOL_W"

    set AV_BYTEENABLE_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_byteenable WIDTH]
    assert {$AV_NUMSYMBOLS==$AV_BYTEENABLE_W} "AV_NUMSYMBOLS=$AV_NUMSYMBOLS must be equal to the width of the byteenable signal ($AV_BYTEENABLE_W)"
    
    set ADDRESS_UNITS [get_instance_interface_parameter_value ${component}_inst $interface addressUnits]

    set portArr [get_instance_interface_ports ${component}_inst ${interface}]
    set burstcountPort ${interface}_burstcount
    set waitrequestPort ${interface}_waitrequest
    set readdatavalidPort ${interface}_readdatavalid
    foreach elem $portArr {
        if {$elem == $burstcountPort} {
            set AV_BURSTCOUNT_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_burstcount WIDTH]
            dpi_bfm_set_param $instance_name AV_BURSTCOUNT_W $AV_BURSTCOUNT_W 
            set USE_BURSTCOUNT 1
        } elseif {$elem == $waitrequestPort} {
            set USE_WAIT_REQUEST 1
        } elseif {$elem == $readdatavalidPort} {
            set USE_READ_DATA_VALID 1
            set AV_MAX_PENDING_READS 256
        }
    }

    dpi_bfm_set_param $instance_name INTERFACE_ID   [expr $id] COMPONENT_NAME $compressed_component AV_ADDRESS_W $AV_ADDRESS_W AV_NUMSYMBOLS $AV_NUMSYMBOLS AV_SYMBOL_W $AV_SYMBOL_W ADDRESS_UNITS $ADDRESS_UNITS USE_WAIT_REQUEST $USE_WAIT_REQUEST USE_BURSTCOUNT $USE_BURSTCOUNT \
                                     USE_READ_DATA_VALID $USE_READ_DATA_VALID USE_READ $USE_READ USE_READ_DATA $USE_READ_DATA USE_WRITE $USE_WRITE USE_WRITE_DATA $USE_WRITE_DATA USE_READ_DATA_VALID $USE_READ_DATA_VALID AV_MAX_PENDING_READS $AV_MAX_PENDING_READS

    add_connection clock_reset_inst.clock         $instance_name.clock
    add_connection clock_reset_inst.reset         $instance_name.reset
    add_connection ${component}_inst.${interface} $instance_name.s0

}

proc instantiate_dpi_bfm_and_connect_to_avalon_slave { component interface is_cra_slave } {
    set compressed_component [get_compressed_component_name $component]
    set cosim_interface [get_instance_interface_assignment ${component}_inst ${interface} hls.cosim.name]

    set instance_name mm_master_dpi_bfm_${component}_${interface}_inst
    instantiate_dpi_bfm $instance_name mm_master_${component}_${interface} hls_sim_mm_master_dpi_bfm

    dpi_bfm_set_param $instance_name COMPONENT_NAME $compressed_component COMPONENT_CRA_SLAVE $is_cra_slave

    # always use the wait request
    # while the component won't stall, the avalon interconnect might
    dpi_bfm_set_param $instance_name USE_WAIT_REQUEST 1

    if { $is_cra_slave == 0 } {
      # if this is a slave memory (not the CRA slave), then we need to specify the name of the stream to write data from the testbench to the memory
      # this name is the function argument name, so remove the avs_ prefix
      set wr_stream_name [string map {"avs_" ""} $cosim_interface]
      set rd_stream_name $wr_stream_name
      append rd_stream_name "_avs_readback"
      dpi_bfm_set_param $instance_name COMPONENT_SLAVE_WRITE_INTERFACE_NAME $wr_stream_name COMPONENT_SLAVE_READ_INTERFACE_NAME $rd_stream_name;
    }

    # symbol width in bits, symbol is one byte
    set AV_SYMBOL_W 8

    set AV_READDATA_W 8
    if { $is_cra_slave == 1 } {
      set AV_READDATA_W 64
    }

    set AV_NUMSYMBOLS [ expr {$AV_READDATA_W / $AV_SYMBOL_W}]
    set AV_NUMSYMBOLS_REM [ expr {$AV_READDATA_W % $AV_SYMBOL_W}]
    assert {$AV_NUMSYMBOLS>0 && $AV_NUMSYMBOLS_REM==0} "AV_READDATA_W must be a multiple of AV_SYMBOL_W=$AV_SYMBOL_W"

    set ports [get_instance_interface_ports ${component}_inst ${interface}]
    # Check if writedata or readata exist, otherwise error out
    if { [lsearch $ports ${interface}_writedata] >= 0 } {
        set AV_SLAVE_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_writedata WIDTH]
    } elseif { [lsearch $ports ${interface}_readdata] >= 0 } {
        set AV_SLAVE_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_readdata WIDTH]
    } else {
        send_message error "Can't find readdata or writedata signals on $component.$interface\n";
    }
    # the master uses byte address, so it's address width must be the slaves address width plus the number of address bits per word
    set AV_SLAVE_NUMSYMBOLS [ expr {$AV_SLAVE_W / $AV_SYMBOL_W}]
    set AV_ADDRESS_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_address WIDTH]
    set num_address_bytes_per_word [expr ceil(log($AV_SLAVE_NUMSYMBOLS)/log(2))]

    set AV_BYTEENABLE_W [get_instance_interface_port_property ${component}_inst ${interface} ${interface}_byteenable WIDTH]
    assert {$AV_SLAVE_NUMSYMBOLS==$AV_BYTEENABLE_W} "AV_NUMSYMBOLS=$AV_NUMSYMBOLS must be equal to the width of the byteenable signal ($AV_BYTEENABLE_W)"

    set AV_FIX_READ_LATENCY [get_instance_interface_parameter_value ${component}_inst $interface readLatency]

    dpi_bfm_set_param $instance_name AV_NUMSYMBOLS $AV_NUMSYMBOLS AV_SYMBOL_W $AV_SYMBOL_W AV_ADDRESS_W [expr $AV_ADDRESS_W + $num_address_bytes_per_word] AV_FIX_READ_LATENCY $AV_FIX_READ_LATENCY USE_READ_DATA_VALID 1

    # burst count width
    set AV_BURSTCOUNT_W 1

    add_connection clock_reset_inst.clock      $instance_name.clock
    add_connection clock_reset_inst.reset      $instance_name.reset
    add_connection $instance_name.m0 ${component}_inst.${interface}

}

proc add_dpi_controller_for_component { component num_iface comp_has_call_iface comp_has_return_iface num_implicit_iface num_slave_iface comp_has_slave_cra num_output_stream_iface} {
    set cosim_component [get_cosim_component_name $component]
    set compressed_component [get_compressed_component_name $component]

    # Add component_dpi_controller instance  (one per component so component name must be in the inst name)
    set instance_name component_dpi_controller_${component}_inst
    instantiate_dpi_bfm ${instance_name} dpic_${component} hls_sim_component_dpi_controller

    # Connect clock/reset to the component_dpi_controller instance
    add_connection clock_reset_inst.clock       component_dpi_controller_${component}_inst.clock
    add_connection clock_reset_inst.clock2x     component_dpi_controller_${component}_inst.clock2x
    add_connection clock_reset_inst.reset       component_dpi_controller_${component}_inst.reset

    # Connect implicit input/output streams between the component_dpi_controller and the component
    if { $comp_has_call_iface == 1 } {
        add_connection component_dpi_controller_${component}_inst.component_call ${component}_inst.call
    }

    if { $comp_has_return_iface == 1 } {
        add_connection ${component}_inst.return component_dpi_controller_${component}_inst.component_return

        if { [lsearch -exact [get_instance_interfaces ${component}_inst] returndata] >= 0 } {
          set stream_width [get_instance_interface_port_property ${component}_inst returndata returndata WIDTH]
          dpi_bfm_set_param $instance_name RETURN_DATAWIDTH $stream_width
          add_connection ${component}_inst.returndata component_dpi_controller_${component}_inst.returndata
        }
    }

    # must set COMPONENT_NAME parameter of the component_dpi_controller
    # set it to the original component name ($component) not ${component}_inst
    dpi_bfm_set_param $instance_name COMPONENT_NAME $compressed_component
    dpi_bfm_set_param $instance_name COMPONENT_MANGLED_NAME $cosim_component

    # add Avalon Conduit Fanout module for go_bind signal
    instantiate_dpi_bfm ${component}_component_dpi_controller_bind_conduit_fanout_inst ${component}_cfan avalon_conduit_fanout
    dpi_bfm_set_param ${component}_component_dpi_controller_bind_conduit_fanout_inst numFanOut $num_iface

    # add Avalon Conduit Fanout module for enable signal
    instantiate_dpi_bfm ${component}_component_dpi_controller_enable_conduit_fanout_inst ${component}_en_cfan avalon_conduit_fanout
    dpi_bfm_set_param ${component}_component_dpi_controller_enable_conduit_fanout_inst numFanOut $num_iface

    # add Avalon Conduit Fanout module for implicit stream ready signals
    if { $num_implicit_iface > 0 } {
        instantiate_dpi_bfm ${component}_component_dpi_controller_implicit_ready_conduit_fanout_inst ${component}_ir_cfan avalon_conduit_fanout
        dpi_bfm_set_param ${component}_component_dpi_controller_implicit_ready_conduit_fanout_inst numFanOut $num_implicit_iface

        add_connection component_dpi_controller_${component}_inst.read_implicit_streams  ${component}_component_dpi_controller_implicit_ready_conduit_fanout_inst.in_conduit
    }

    add_connection component_dpi_controller_${component}_inst.dpi_control_bind  ${component}_component_dpi_controller_bind_conduit_fanout_inst.in_conduit
    add_connection component_dpi_controller_${component}_inst.dpi_control_enable  ${component}_component_dpi_controller_enable_conduit_fanout_inst.in_conduit

    # there is only ever one Avalon Master BFM for the component's slave, so just connect the slave ready signal
    if { $num_slave_iface > 0 } {
      # wait for all slaves
      instantiate_dpi_bfm ${component}_component_dpi_controller_slave_ready_concatenate_inst ${component}_sl_rcat avalon_concatenate_singlebit_conduits
      dpi_bfm_set_param ${component}_component_dpi_controller_slave_ready_concatenate_inst multibit_width $num_slave_iface

      instantiate_dpi_bfm ${component}_component_dpi_controller_slave_done_concatenate_inst ${component}_sl_dcat avalon_concatenate_singlebit_conduits
      dpi_bfm_set_param ${component}_component_dpi_controller_slave_done_concatenate_inst multibit_width $num_slave_iface

      dpi_bfm_set_param component_dpi_controller_${component}_inst COMPONENT_NUM_SLAVES $num_slave_iface

      add_connection ${component}_component_dpi_controller_slave_ready_concatenate_inst.out_conduit component_dpi_controller_${component}_inst.dpi_control_slaves_ready
      add_connection ${component}_component_dpi_controller_slave_done_concatenate_inst.out_conduit component_dpi_controller_${component}_inst.dpi_control_slaves_done

      # control slaves' reads
      instantiate_dpi_bfm ${component}_component_dpi_controller_slave_readback_fanout_inst ${component}_sl_rbkfan avalon_conduit_fanout
      dpi_bfm_set_param ${component}_component_dpi_controller_slave_readback_fanout_inst numFanOut $num_slave_iface

      add_connection component_dpi_controller_${component}_inst.readback_from_slaves ${component}_component_dpi_controller_slave_readback_fanout_inst.in_conduit

      # if one of the slaves is the CRA slave, might need to wait for the return data to be read
      set has_slave_return [expr (($comp_has_return_iface == 0) && ($comp_has_slave_cra == 1))]
      
      # connect slave_busy_out to component dpi controller which needs to query this to determine if slave component is busy
      if { $comp_has_slave_cra == 1} {
        add_connection mm_master_dpi_bfm_${component}_avs_cra_inst.slave_busy_out component_dpi_controller_${component}_inst.slave_busy
      }

      if { $has_slave_return == 1} {
        dpi_bfm_set_param component_dpi_controller_${component}_inst COMPONENT_HAS_SLAVE_RETURN  $has_slave_return
        dpi_bfm_set_param mm_master_dpi_bfm_${component}_avs_cra_inst COMPONENT_HAS_SLAVE_RETURN $has_slave_return

        # connect the interrupt
        add_connection component_dpi_controller_${component}_inst.component_irq ${component}_inst.irq

        # connect slave memory done signals
        if { [expr ($num_slave_iface-1)] > 0 } {
          dpi_bfm_set_param mm_master_dpi_bfm_${component}_avs_cra_inst NUM_SLAVE_MEMORIES [expr ($num_slave_iface-1)]
          instantiate_dpi_bfm ${component}_component_cra_slave_memories_done_concatenate_inst ${component}_cra_slmem_done avalon_concatenate_singlebit_conduits
          dpi_bfm_set_param ${component}_component_cra_slave_memories_done_concatenate_inst multibit_width [expr ($num_slave_iface-1)]
          add_connection ${component}_component_cra_slave_memories_done_concatenate_inst.out_conduit mm_master_dpi_bfm_${component}_avs_cra_inst.cra_control_slave_memory_writes_done
        }
      }
    }

    if { $num_output_stream_iface > 0 } {
      instantiate_dpi_bfm ${component}_component_dpi_controller_stream_active_concatenate_inst ${component}_osacat avalon_concatenate_singlebit_conduits
      dpi_bfm_set_param ${component}_component_dpi_controller_stream_active_concatenate_inst multibit_width $num_output_stream_iface

      dpi_bfm_set_param component_dpi_controller_${component}_inst COMPONENT_NUM_OUTPUT_STREAMS $num_output_stream_iface
      add_connection ${component}_component_dpi_controller_stream_active_concatenate_inst.out_conduit component_dpi_controller_${component}_inst.dpi_control_stream_writes_active
    }

}

proc need_dpi_bfm_inst_for_interface { component interface } {
    # we already connected these
    if { $interface != "call" && $interface != "return" && $interface != "returndata" && $interface != "irq" &&
         $interface != "clock" && $interface != "clock2x" && $interface != "reset"  } {
        return 1
    } else {
        return 0
    }
}

proc instantiate_component_dpi_modules { component } {

    # Connect clock/reset to the component
    add_connection clock_reset_inst.clock       ${component}_inst.clock
    add_connection clock_reset_inst.reset       ${component}_inst.reset

    # Walk through component interfaces and instantiate correct dpi_bfm for each
    # Do not connect implicit interfaces and clock/reset (we already connected them)
    # Also, if the component has clock2x (optional) then connect it
    set num_iface 0
    set num_implicit_iface 0
    set num_slave_iface 0
    set num_output_stream_iface 0
    set comp_has_call_iface 0
    set comp_has_return_iface 0
    set comp_has_slave_cra 0
    foreach interface [get_instance_interfaces ${component}_inst] {
        set description [get_instance_interface_property ${component}_inst $interface DESCRIPTION]
        debug $description
        set class_name [get_instance_interface_property ${component}_inst $interface CLASS_NAME]
        debug $class_name
        set portlist [get_instance_interface_ports ${component}_inst $interface]
        set numports [llength $portlist]
        if {$interface == "clock2x"} {
            add_connection clock_reset_inst.clock2x     ${component}_inst.clock2x
        } elseif {$interface == "call"} {
            set comp_has_call_iface 1
        } elseif {$interface == "return"} {
            set comp_has_return_iface 1
        } elseif { [need_dpi_bfm_inst_for_interface $component $interface] } {
            if { $class_name=="avalon_streaming_sink" || ($class_name=="conduit_end" && $numports==3) } {
                instantiate_dpi_bfm_and_connect_to_stream_sink $component $interface
                set  num_iface [expr $num_iface + 1]
            } elseif { $class_name=="avalon_streaming_source" || ($class_name=="conduit_start" && $numports==3) } {
                instantiate_dpi_bfm_and_connect_to_stream_source $component $interface
                set  num_iface [expr $num_iface + 1]
                set num_output_stream_iface [expr $num_output_stream_iface + 1]
            } elseif { $class_name=="avalon_master"} {
                instantiate_dpi_bfm_and_connect_to_avalon_master $component $interface
                set  num_iface [expr $num_iface + 1]
            } elseif { $class_name=="avalon_slave"} {
                set is_cra_slave [expr [string compare -nocase $interface "avs_cra"] == 0]
                instantiate_dpi_bfm_and_connect_to_avalon_slave $component $interface $is_cra_slave
                set  num_slave_iface [expr $num_slave_iface + 1]
                set  num_iface [expr $num_iface + 1]
                set  num_implicit_iface [expr $num_implicit_iface + 1]
                if { $is_cra_slave } {
                    set comp_has_slave_cra 1
                }
            } elseif { $class_name == "conduit_end" && $numports == 1} {
                instantiate_dpi_bfm_and_connect_to_implicit_stream_sink $component $interface
                set  num_iface [expr $num_iface + 1]
                set  num_implicit_iface [expr $num_implicit_iface + 1]
            } else {
                send_message error "Unrecognized interface class on $component.$interface: $class_name\n";
            }
        }
    }

    add_dpi_controller_for_component $component $num_iface $comp_has_call_iface $comp_has_return_iface $num_implicit_iface $num_slave_iface $comp_has_slave_cra $num_output_stream_iface

    # Add control signals from the component DPI controller to the interface BFMs
    set num_iface 0
    set num_implicit_iface 0
    set num_slave_iface 0
    set num_output_stream_iface 0
    set num_non_cra_slave_iface 0
    foreach interface [get_instance_interfaces ${component}_inst] {
        set portlist [get_instance_interface_ports ${component}_inst $interface]
        set numports [llength $portlist]
        if { [need_dpi_bfm_inst_for_interface $component $interface] } {
            set class_name [get_instance_interface_property ${component}_inst $interface CLASS_NAME]
            if { $class_name=="avalon_streaming_sink" || ($class_name=="conduit_end" && $numports==3) } {
                add_connection ${component}_component_dpi_controller_bind_conduit_fanout_inst.out_conduit_$num_iface   stream_source_dpi_bfm_${component}_${interface}_inst.dpi_control_bind
                add_connection ${component}_component_dpi_controller_enable_conduit_fanout_inst.out_conduit_$num_iface stream_source_dpi_bfm_${component}_${interface}_inst.dpi_control_enable
                set  num_iface [expr $num_iface + 1]
            } elseif { $class_name=="avalon_streaming_source" || ($class_name=="conduit_start" && $numports==3) } {
                add_connection ${component}_component_dpi_controller_bind_conduit_fanout_inst.out_conduit_$num_iface   stream_sink_dpi_bfm_${component}_${interface}_inst.dpi_control_bind
                add_connection ${component}_component_dpi_controller_enable_conduit_fanout_inst.out_conduit_$num_iface stream_sink_dpi_bfm_${component}_${interface}_inst.dpi_control_enable
                add_connection stream_sink_dpi_bfm_${component}_${interface}_inst.dpi_control_stream_active ${component}_component_dpi_controller_stream_active_concatenate_inst.in_conduit_${num_output_stream_iface}
                set  num_iface [expr $num_iface + 1]
                set num_output_stream_iface [expr $num_output_stream_iface + 1]
            } elseif { $class_name=="avalon_master"} {
                add_connection ${component}_component_dpi_controller_bind_conduit_fanout_inst.out_conduit_$num_iface   mm_slave_dpi_bfm_${component}_${interface}_inst.dpi_control_bind
                add_connection ${component}_component_dpi_controller_enable_conduit_fanout_inst.out_conduit_$num_iface mm_slave_dpi_bfm_${component}_${interface}_inst.dpi_control_enable
                set  num_iface [expr $num_iface + 1]
            } elseif { $class_name=="avalon_slave"} {
                add_connection ${component}_component_dpi_controller_bind_conduit_fanout_inst.out_conduit_$num_iface   mm_master_dpi_bfm_${component}_${interface}_inst.dpi_control_bind
                add_connection ${component}_component_dpi_controller_enable_conduit_fanout_inst.out_conduit_$num_iface mm_master_dpi_bfm_${component}_${interface}_inst.dpi_control_enable
                add_connection ${component}_component_dpi_controller_implicit_ready_conduit_fanout_inst.out_conduit_$num_implicit_iface mm_master_dpi_bfm_${component}_${interface}_inst.dpi_control_component_started
                add_connection ${component}_component_dpi_controller_slave_readback_fanout_inst.out_conduit_$num_slave_iface mm_master_dpi_bfm_${component}_${interface}_inst.dpi_control_component_done
                add_connection mm_master_dpi_bfm_${component}_${interface}_inst.dpi_control_done_writes ${component}_component_dpi_controller_slave_ready_concatenate_inst.in_conduit_${num_slave_iface}
                add_connection mm_master_dpi_bfm_${component}_${interface}_inst.dpi_control_done_reads ${component}_component_dpi_controller_slave_done_concatenate_inst.in_conduit_${num_slave_iface}
                set  num_iface [expr $num_iface + 1]
                set  num_implicit_iface [expr $num_implicit_iface + 1]
                set  num_slave_iface [expr $num_slave_iface + 1]
                set is_cra_slave [expr [string compare -nocase $interface "avs_cra"] == 0]
                if { $is_cra_slave == 0 && $comp_has_slave_cra == 1 && $comp_has_return_iface == 0 } {
                  add_connection mm_master_dpi_bfm_${component}_${interface}_inst.cra_control_done_writes_to_cra ${component}_component_cra_slave_memories_done_concatenate_inst.in_conduit_${num_non_cra_slave_iface}
                  set num_non_cra_slave_iface [expr $num_non_cra_slave_iface + 1]
                }
            } elseif { $class_name == "conduit_end" && $numports == 1} {
                add_connection ${component}_component_dpi_controller_bind_conduit_fanout_inst.out_conduit_$num_iface   stream_source_dpi_bfm_${component}_${interface}_inst.dpi_control_bind
                add_connection ${component}_component_dpi_controller_enable_conduit_fanout_inst.out_conduit_$num_iface stream_source_dpi_bfm_${component}_${interface}_inst.dpi_control_enable
                add_connection ${component}_component_dpi_controller_implicit_ready_conduit_fanout_inst.out_conduit_$num_implicit_iface stream_source_dpi_bfm_${component}_${interface}_inst.source_ready
                set  num_iface [expr $num_iface + 1]
                set  num_implicit_iface [expr $num_implicit_iface + 1]
            } else {
                send_message error "Unrecognized interface class on $component.$interface: $class_name\n";
            }
        }
    }
}


# -----------------------------------------------------------------------------
# Create a new qsys system
# -----------------------------------------------------------------------------
create_system $sim_qsys
puts "Generating qsys simulation system: $sim_qsys"

# Create a TCL list from a comma-separated string
set components [split $component_list ","]
set compressed_components [list]

# Add the clock/reset instance, one per sim system
instantiate_dpi_bfm clock_reset_inst clock_reset hls_sim_clock_reset
dpi_bfm_set_param clock_reset_inst RESET_CYCLE_HOLD $num_reset_cycles

# Instantiate the components and build the cosim name string
foreach component $components {

    if {$quartus_pro == 1} {
        add_instance ${component}_inst altera_generic_component
        load_instantiation ${component}_inst
        set_instantiation_property IP_FILE ../components/${component}/${component}.ip
        set_instantiation_property HDL_ENTITY_NAME ${component}
        set_instantiation_property HDL_COMPILATION_LIBRARY ${component}
        save_instantiation
        reload_component_footprint ${component}_inst

        load_component ${component}_inst
        lappend compressed_components [get_component_assignment hls.compressed.name]
    } else {
        add_instance ${component}_inst $component
        lappend compressed_components [get_composed_instance_assignment ${component}_inst ${component}_internal_inst hls.compressed.name]
    }

}

# Add main_dpi_controller_inst, one per sim system
instantiate_dpi_bfm main_dpi_controller_inst main_dpi_controller hls_sim_main_dpi_controller
dpi_bfm_set_param main_dpi_controller_inst NUM_COMPONENTS [llength $components] COMPONENT_NAMES_STR [join $compressed_components ","]

# Connect clock/reset to the main_dpi_controller instance
add_connection clock_reset_inst.clock              main_dpi_controller_inst.clock
add_connection clock_reset_inst.clock2x            main_dpi_controller_inst.clock2x
add_connection clock_reset_inst.reset              main_dpi_controller_inst.reset
add_connection main_dpi_controller_inst.reset_ctrl clock_reset_inst.reset_ctrl

# -----------------------------------------------------------------------------
# For each component, instantiate and connect the controller and DPI interface drivers
# -----------------------------------------------------------------------------
foreach component $components {
    instantiate_component_dpi_modules $component
}

# -----------------------------------------------------------------------------
# Connect main_dpi_controller to all component_dpi_controllers (one per component)
# This is done using avalon_split_multibit_conduit and avalon_concatenate_singlebit_conduits adapters
# -----------------------------------------------------------------------------

instantiate_dpi_bfm split_component_start_inst sp_cstart avalon_split_multibit_conduit
dpi_bfm_set_param split_component_start_inst multibit_width [llength $components]

add_connection main_dpi_controller_inst.component_enabled split_component_start_inst.in_conduit
set i 0
foreach component $components {
    add_connection split_component_start_inst.out_conduit_$i component_dpi_controller_${component}_inst.component_enabled
    incr i
}

instantiate_dpi_bfm concatenate_component_done_inst cat_done avalon_concatenate_singlebit_conduits
dpi_bfm_set_param concatenate_component_done_inst multibit_width [llength $components]
add_connection concatenate_component_done_inst.out_conduit main_dpi_controller_inst.component_done

instantiate_dpi_bfm concatenate_component_wait_for_stream_writes_inst cat_cwfsw avalon_concatenate_singlebit_conduits
dpi_bfm_set_param concatenate_component_wait_for_stream_writes_inst multibit_width [llength $components]
add_connection concatenate_component_wait_for_stream_writes_inst.out_conduit main_dpi_controller_inst.component_wait_for_stream_writes

set i 0
foreach component $components {
    add_connection component_dpi_controller_${component}_inst.component_done concatenate_component_done_inst.in_conduit_$i
    add_connection component_dpi_controller_${component}_inst.component_wait_for_stream_writes concatenate_component_wait_for_stream_writes_inst.in_conduit_$i
    incr i
}

# Create the qsys system
save_system $sim_qsys.qsys

