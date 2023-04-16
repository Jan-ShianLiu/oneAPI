# $File: //depot/sw/hld/rel/20.3api.11/acl/ip/src/board/acl_hyper_optimized_ccb/alt_hiconnect_dc_fifo_hw.tcl $
# $Revision: #1 $
# $Date: 2020/09/28 $
# $Author: sys_hld $
#--------------------------------------------------------------------------
# Avalon-ST DCFIFO component description
#--------------------------------------------------------------------------

package require -exact qsys 15.0

set_module_property NAME alt_hiconnect_dc_fifo
set_module_property DISPLAY_NAME "Avalon-ST Dual Clock FIFO"
set_module_property AUTHOR "Altera Corporation"
set_module_property GROUP "Qsys Interconnect/Memory-Mapped Alpha"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property VERSION __ACDS_VERSION_SHORT__
set_module_property EDITABLE false
set_module_property DATASHEET_URL "http://www.altera.com/literature/hb/nios2/qts_qii55014.pdf"
set_module_property HIDE_FROM_QUARTUS true

add_fileset synth QUARTUS_SYNTH syn
add_fileset sim_verilog SIM_VERILOG syn
add_fileset sim_vhdl SIM_VHDL syn

set_fileset_property synth TOP_LEVEL alt_hiconnect_dc_fifo
set_fileset_property sim_verilog TOP_LEVEL alt_hiconnect_dc_fifo
set_fileset_property sim_vhdl TOP_LEVEL alt_hiconnect_dc_fifo

proc syn { name } {
    add_fileset_file "alt_hiconnect_dc_fifo.sv"    SYSTEM_VERILOG PATH "alt_hiconnect_dc_fifo.sv"
    add_fileset_file "alt_hiconnect_dc_fifo.sdc"   SDC     PATH "alt_hiconnect_dc_fifo.sdc"
    add_fileset_file "alt_hiconnect_bxor2w5t1.v"   VERILOG PATH "alt_hiconnect_bxor2w5t1.v"  
    add_fileset_file "alt_hiconnect_cnt5ic.v"      VERILOG PATH "alt_hiconnect_cnt5ic.v"     
    add_fileset_file "alt_hiconnect_cnt5il.v"      VERILOG PATH "alt_hiconnect_cnt5il.v"     
    add_fileset_file "alt_hiconnect_gpx5.v"        VERILOG PATH "alt_hiconnect_gpx5.v"       
    add_fileset_file "alt_hiconnect_gray5t1.v"     VERILOG PATH "alt_hiconnect_gray5t1.v"    
    add_fileset_file "alt_hiconnect_lut6.v"        VERILOG PATH "alt_hiconnect_lut6.v"       
    add_fileset_file "alt_hiconnect_mlab5a1r1w1.v" VERILOG PATH "alt_hiconnect_mlab5a1r1w1.v"
    add_fileset_file "alt_hiconnect_mlab.v"        VERILOG PATH "alt_hiconnect_mlab.v"       
    add_fileset_file "alt_hiconnect_sync5m.v"      VERILOG PATH "alt_hiconnect_sync5m.v"     
    add_fileset_file "alt_hiconnect_ungray5t1.v"   VERILOG PATH "alt_hiconnect_ungray5t1.v"  
    add_fileset_file "alt_hiconnect_wys_reg.v"     VERILOG PATH "alt_hiconnect_wys_reg.v"    
    add_fileset_file "alt_hiconnect_xor1t0.v"      VERILOG PATH "alt_hiconnect_xor1t0.v"     
    add_fileset_file "alt_hiconnect_xor1t1.v"      VERILOG PATH "alt_hiconnect_xor1t1.v"     
    add_fileset_file "alt_hiconnect_xor2t0.v"      VERILOG PATH "alt_hiconnect_xor2t0.v"     
    add_fileset_file "alt_hiconnect_xor2t1.v"      VERILOG PATH "alt_hiconnect_xor2t1.v"     
    add_fileset_file "alt_hiconnect_xor3t0.v"      VERILOG PATH "alt_hiconnect_xor3t0.v"     
    add_fileset_file "alt_hiconnect_xor3t1.v"      VERILOG PATH "alt_hiconnect_xor3t1.v"     
    add_fileset_file "alt_hiconnect_xor4t0.v"      VERILOG PATH "alt_hiconnect_xor4t0.v"     
    add_fileset_file "alt_hiconnect_xor4t1.v"      VERILOG PATH "alt_hiconnect_xor4t1.v"     
    add_fileset_file "alt_hiconnect_xor5t0.v"      VERILOG PATH "alt_hiconnect_xor5t0.v"     
    add_fileset_file "alt_hiconnect_xor5t1.v"      VERILOG PATH "alt_hiconnect_xor5t1.v"     
    add_fileset_file "alt_st_mlab_dcfifo_ack.v"    VERILOG PATH "alt_st_mlab_dcfifo_ack.v"   
    add_fileset_file "alt_st_mlab_dcfifo.v"        VERILOG PATH "alt_st_mlab_dcfifo.v"       
}


## --------------------------------------------
#|
#| Module parameters
#|
add_parameter DATA_WIDTH                INTEGER 32 ""
add_parameter CHANNEL_WIDTH             INTEGER 0  ""
add_parameter USE_PACKETS               INTEGER 0  ""
add_parameter PREVENT_UNDERFLOW         INTEGER 0  ""
add_parameter SHOWAHEAD                 INTEGER 0  ""

set_parameter_property DATA_WIDTH        HDL_PARAMETER true 
set_parameter_property CHANNEL_WIDTH     HDL_PARAMETER true
set_parameter_property USE_PACKETS       HDL_PARAMETER true 
set_parameter_property PREVENT_UNDERFLOW HDL_PARAMETER true 
set_parameter_property SHOWAHEAD         HDL_PARAMETER true 

## --------------------------------------------
#|
#| Callback routines
#|
set_module_property ELABORATION_CALLBACK "elaborate"

proc elaborate {} {

    set data_width          [ get_parameter_value "DATA_WIDTH" ]
    set channel_width       [ get_parameter_value "CHANNEL_WIDTH" ]
    set use_packets         [ get_parameter_value "USE_PACKETS" ]

    # In clock interface
    add_interface in_clk "clock" "sink" 
    add_interface_port in_clk in_clk clk Input 1

    # Out clock interface
    add_interface out_clk "clock" "sink"
    add_interface_port out_clk out_clk clk Input 1
    
    # Out reset interface
    add_interface out_reset "reset" "sink" "out_clk"
    add_interface_port out_reset out_reset reset Input 1                                                                                                                                                                                                                                                                                            
    set_interface_property out_reset synchronousEdges BOTH

    # Avalon-ST sink interface
    add_interface "in" "avalon_streaming" "sink" "in_clk"
    set_interface_property "in" symbolsPerBeat 1
    set_interface_property "in" dataBitsPerSymbol $data_width
    set_interface_property "in" readyLatency 0
    set_interface_property "in" maxChannel 0

    # Avalon-ST source interface
    add_interface "out" "avalon_streaming" "source" "out_clk"
    set_interface_property "out" symbolsPerBeat 1
    set_interface_property "out" dataBitsPerSymbol $data_width
    set_interface_property "out" readyLatency 0
    set_interface_property "out" maxChannel 0

    add_interface_port "in" "in_data" "data" Input $data_width
    add_interface_port "in" "in_valid" "valid" Input 1
    add_interface_port "in" "in_ready" "ready" Output 1

    add_interface_port "out" "out_data" "data" Output $data_width
    add_interface_port "out" "out_valid" "valid" Output 1
    add_interface_port "out" "out_ready" "ready" Input 1

    add_interface_port "in"  "in_startofpacket"  "startofpacket" Input 1
    add_interface_port "in"  "in_endofpacket"    "endofpacket"   Input 1
    add_interface_port "out" "out_startofpacket" "startofpacket" Output 1
    add_interface_port "out" "out_endofpacket"   "endofpacket"   Output 1

    if {$use_packets == "0"} {
        set_port_property  "in_startofpacket"   termination true
        set_port_property  "in_startofpacket"   termination_value 0
        set_port_property  "in_endofpacket"     termination true
        set_port_property  "in_endofpacket"     termination_value 0
        set_port_property  "out_startofpacket"  termination true
        set_port_property  "out_endofpacket"    termination true
    }

    if { $channel_width > 0 } {
        add_interface_port in  in_channel  channel Input  $channel_width
        add_interface_port out out_channel channel Output $channel_width
    } else {
        add_interface_port in  in_channel  channel Input  1
        add_interface_port out out_channel channel Output 1

        set_port_property  "in_channel"   termination true
        set_port_property  "in_channel"   termination_value 0
        set_port_property  "out_channel"   termination true
    }

}

