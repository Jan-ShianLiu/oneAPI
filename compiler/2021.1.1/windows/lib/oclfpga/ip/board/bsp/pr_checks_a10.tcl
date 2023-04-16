post_message "Running pr_checks_a10.tcl script"

post_message "Checking for OpenCL SDK installation, environment should have INTELFPGAOCLSDKROOT defined"
if {[catch {set sdk_root $::env(INTELFPGAOCLSDKROOT)} result]} {
  post_message -type error "OpenCL SDK installation not found.  Make sure INTELFPGAOCLSDKROOT is correctly set"
  exit 2
} else {
  post_message "INTELFPGAOCLSDKROOT=$::env(INTELFPGAOCLSDKROOT)"
}

# Required packages
package require ::quartus::project
package require ::quartus::report
package require ::quartus::flow
package require ::quartus::atoms

# Load OpenCL BSP utility functions
source "$sdk_root/ip/board/bsp/opencl_bsp_util.tcl"

# Check if using qsys-less flow or qsys flow
if { [llength $quartus(args) ] == 1 } {
  set top_path [lindex $quartus(args) 0]
} else {
  set top_path {freeze_wrapper_inst|kernel_system_inst}
}

set project_name  [::opencl_bsp::get_project_name $quartus(args)]
set revision_name [::opencl_bsp::get_revision_name $quartus(args) $project_name]
set fast_compile  [::aocl_fast_compile::is_fast_compile]


##############################################################################
##############################       MAIN        #############################
##############################################################################

post_message "Project name: $project_name"
post_message "Revision name: $revision_name"

if {$fast_compile} {
  post_message "Warning: Fast compile does not have Fitter RAM summary. Skipping PR checks script"
  return 0
}

load_package design
project_open $project_name -revision $revision_name
design::load_design -writeable -snapshot final
load_report $revision_name

# Error out if initialized MLAB RAMs are found in kernel partition (not PR compatible)
set panel_name "Fitter||Place Stage||Fitter RAM Summary"
set panel_id [get_report_panel_id $panel_name] 
set num_rows [get_number_of_rows -id $panel_id]
for {set r 1} {$r < $num_rows} {incr r} {

  # Parse all instance paths and only process everything inside freeze_wrapper_inst|pr_region_inst|kernel_system_inst 
  set instance_path [get_report_panel_data -id $panel_id -row $r -col 0]
  if { [info exists instance_path_parsed] } { unset instance_path_parsed }

  set search_pattern [string map {{|} {\|}} $top_path];
  append search_pattern ".*$"
  regexp $search_pattern $instance_path instance_path_parsed

  if { [info exists instance_path_parsed] } {
    # Only scan through RAMs that consists of MLABs
    set mlabs [get_report_panel_data -id $panel_id -row $r -col 19]
    if { $mlabs != 0 } {

      #  Find MIF initialized MLAB RAMs
      set mif_file [get_report_panel_data -id $panel_id -row $r -col 20]
      if { $mif_file != "None" } {
        post_message -type error "Found MLAB with initialized memory content which is not supported in A10 Partial Reconfiguration." -submsgs [list "Instance path: $instance_path_parsed" "MLABs: $mlabs" "MIF file: $mif_file"]
        post_message -type error "Terminating post-flow script"
        exit 2
      }
    }
  }
}

design::unload_design
project_close

