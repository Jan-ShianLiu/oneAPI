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
package require altera_terp

set_module_property NAME avalon_split_multibit_conduit
set_module_property DISPLAY_NAME "Avalon Split Multi-bit Conduit into Single-bit Conduits"
set_module_property AUTHOR "Altera Corporation"
set_module_property GROUP "Accelerators"

set_module_property VERSION 1.0
set_module_property EDITABLE false
set_module_property HIDE_FROM_QUARTUS true

# +-----------------------------------
# | callbacks
# |
set_module_property ELABORATION_CALLBACK elaborate
add_fileset quartus_synth QUARTUS_SYNTH generate "Avalon Split Multi-bit Conduit Generation"
add_fileset sim_verilog SIM_VERILOG generate "Avalon Split Multi-bit Conduit Generation"

# +-----------------------------------
# | parameters
# |
add_parameter multibit_width    INTEGER 2 ""
set_parameter_property multibit_width AFFECTS_GENERATION true


# +-----------------------------------
# | display items, names, and descriptions
# |
set_parameter_property multibit_width DISPLAY_NAME "The width of the input multi-bit coduit"

proc createVerilogFile {output_name} {
    set this_dir                [ get_module_property MODULE_DIRECTORY ]
    set template_file           [ file join $this_dir "avalon_split_multibit_conduit.sv.terp" ]        
    set template                [ read [ open $template_file r ] ]    

    set multibit_width              [ get_parameter_value "multibit_width" ]
      
    set params(multibit_width)         $multibit_width
    set params(output_name)       $output_name
   
    set result          [ altera_terp $template params ]
    return $result
}

proc generate {output_name} {
    set result [createVerilogFile $output_name]
    set output_file     $output_name
    append output_file ".sv"

    add_fileset_file ${output_file} {SYSTEM_VERILOG} TEXT ${result}
}


proc elaborate {} {
    set multibit_width [ get_parameter_value "multibit_width" ]    

    add_interface "in_conduit" "conduit" "end" 
    add_interface_port "in_conduit" "in_conduit" "conduit" "input" $multibit_width
    
    for {set i 0} {$i < $multibit_width} {incr i} {
        add_interface "out_conduit_$i" "conduit" "start" 
        add_interface_port "out_conduit_$i" "out_conduit_$i" "conduit" "output" 1
    }
}