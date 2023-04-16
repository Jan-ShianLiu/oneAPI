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


load_package flow
package require cmdline

proc make_all_pins_virtual { project_name project_rev } {

    project_open $project_name -revision $project_rev

    remove_all_instance_assignments -name VIRTUAL_PIN
    set name_ids [get_names -filter * -node_type pin]

    foreach_in_collection name_id $name_ids {
        set pin_name [get_name_info -info full_path $name_id]

        if { -1 == [lsearch -exact { clock clock2x } $pin_name] } {
            post_message "Making VIRTUAL_PIN assignment to $pin_name"
            set_instance_assignment -to $pin_name -name VIRTUAL_PIN ON
        } else {
            post_message "Skipping VIRTUAL_PIN assignment to $pin_name"
        }
    }
    export_assignments
}

set module       [lindex $quartus(args) 0]
set project_name [lindex $quartus(args) 1]
set project_rev  [lindex $quartus(args) 2]

if { [string match "quartus_map" $module] || [string match "quartus_syn" $module] } {
    make_all_pins_virtual $project_name $project_rev
}
