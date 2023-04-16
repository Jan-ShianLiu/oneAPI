set board "$::env(INTELFPGAOCLSDKROOT)/ip/board"

source "$board/incremental/aocl_incremental.tcl"
source "$board/fast_compile/aocl_fast_compile.tcl"
source "$board/floorplanner/floorplanner_utils.tcl"
source "$board/bsp/empty_flow_utils.tcl"

namespace eval ::opencl_bsp {
}

# Initialization to be done during pre_flow stage
# Should be applicable to all revisions
proc ::opencl_bsp::pre_flow_init {project_name revision_name top_path} {
  ::aocl_empty_flow::set_empty_setting $project_name $revision_name $top_path
}


proc ::opencl_bsp::pre_synth_init {project_name revision_name pr_base_rev top_path} {
  ::aocl_incremental::init_partitions $project_name $revision_name $top_path
  ::aocl_floorplanner::set_floorplan $project_name $revision_name $pr_base_rev $top_path
}

proc ::opencl_bsp::get_project_name {quartus_args} {
  if { [llength $quartus_args ] <= 1 } {
    # If this script is run manually, just compile the default revision
    set qpf_files [glob -nocomplain *.qpf]

    if {[llength $qpf_files] == 0} {
      error "No QSF detected"
    } elseif {[llength $qpf_files] > 1} {
      post_message "Warning: More than one QSF detected. Picking the first one."
    }
    set qpf_file [lindex $qpf_files 0]
    set project_name [string range $qpf_file 0 [expr [string first . $qpf_file] - 1]]
  } else {
    set project_name [lindex $quartus_args 1]
  }
  return $project_name
}

proc ::opencl_bsp::get_revision_name {quartus_args project_name} {
  if { [llength $quartus_args ] <= 1 } {
    set revision_name [get_current_revision $project_name]
  } else {
    set revision_name [lindex $quartus_args 2]
  }
  return $revision_name
}

proc ::opencl_bsp::get_device_name {project_name revision_name} {
  project_open $project_name -revision $revision_name
  set part_name [get_global_assignment -name DEVICE]
  project_close
  return $part_name
}

proc ::opencl_bsp::create_base_id {} {
  # Create unique base revision compile ID
  ########################################
  # The unique ID is constructed by using 
  # a) a high-resolution counter (clicks)
  # b) the current working directory
  #
  # 1. both are written into a file (hash_temp.txt)
  # 2. 'aocl hash hash_temp.txt' produces the md5 hash
  # 3. the md5 hash is truncated to the 32 MSBs
  # 4. the final unique ID is the 31 LSBs of this truncated hash
  #    (to match QSys component unsigned int representation)

  # get high resolution counter value
  set time_in_clicks [clock clicks]

  # current current working directory
  set current_work_dir [pwd]

  # write both out to temporary file
  set hash_temp_filename "hash_temp.txt"
  set hash_temp_file [open $hash_temp_filename w]
  puts $hash_temp_file $time_in_clicks
  puts $hash_temp_file $current_work_dir
  close $hash_temp_file

  # use 'aocl hash' to get the md5 hash value
  if {[catch {exec aocl hash hash_temp.txt} result] == 0} { 
    set result_step2 [string range $result 0 7]
    set result_step3 [expr 0x$result_step2]
    set base_id [expr {$result_step3 & 0x7FFFFFFF }]

    # delete temporary file
    file delete -force $hash_temp_filename 
    return $base_id

  } else {
    post_message "Error running ::opencl_bsp::create_base_id 'aocl hash'"
    exit -1
  } 
    
}

proc ::opencl_bsp::correct_sdc_ordering {sdc_name} {
  # Correct sdc generated clocks ordering by sorting alphanumerically.
  # SDC file generated from write_sdc_file in Quartus can be
  # non-deterministically out-of-order that could cause fitter 
  # to infer a clock, which leads to timing violations.

  # Read sdc
  set origin_fp [open $sdc_name r]
  set lines [split [read $origin_fp] "\n"]
  close $origin_fp

  # Write to temp.sdc
  set temp_fp [open "temp.sdc" w]

  set index 0

  # Process base.sdc line by line
  while {$index < [llength $lines]} {
    set clocks_found {}
    set non_clock_found {}

    # Check if it is in create_generated_clock code block
    if {[string match "create_generated_clock*" [lindex $lines $index]]} {

      # Read into buffer until it is not code block of create_generated_clock
      while {$index < [llength $lines] && [lindex $lines $index]!="\n"} {

        if {[string match "create_generated_clock*" [lindex $lines $index]]} {  
          lappend clocks_found [lindex $lines $index]
        } else {
          lappend non_clock_found [lindex $lines $index]
        }
        incr index
      }

      # Sort the buffer
      set sorted_clocks [lsort $clocks_found]
      
      # Dump code block of create_generated_clock
      puts $temp_fp [join $sorted_clocks "\n"]
      puts $temp_fp [join $non_clock_found "\n"]

    } else {
      # Not in create_generated_clock code block, skip
      puts $temp_fp [lindex $lines $index]
      incr index
    }
  }
  close $temp_fp

  # Replace sdc file
  file delete -force "$sdc_name"
  file rename -force "temp.sdc" "$sdc_name"  
}

proc ::opencl_bsp::get_acds_version {revision_name} {
  # Returns the version of acds database used on the revision

  set db_version [get_database_version $revision_name]
  regexp {Version ([0-9,.]*)} $db_version full sub

  # Return the acds version in float
  set sub [join [lrange [split $sub "."] 0 1] "."]
  return $sub
}

proc ::opencl_bsp::remove_kernel_clk_sdc {sdc_name} {
  # For Stratix 10 the derive_pll_clocks functionality for STA was removed
  # Therefore the kernel PLL SDC files are required to determine the PLL output
  # clocks and will be included in base.qar in all Stratix 10 BSPs
  # In order for Timing Analyzer to correctly determine the kernel PLL clocks
  # we have to remove the obsolete create_generated_clocks for the kernel PLL outputs from base.sdc

  # Read sdc
  set origin_fp [open $sdc_name r]
  set lines [split [read $origin_fp] "\n"]
  close $origin_fp

  # Write to temp.sdc
  set temp_fp [open "temp.sdc" w]

  set index 0

  # Process base.sdc line by line
  while {$index < [llength $lines]} {
    if {[string match "create_generated_clock*kernel_clk_gen|kernel_clk_gen|kernel_pll_outclk*"    [lindex $lines $index]] ||
        [string match "create_generated_clock*kernel_clk_gen|kernel_clk_gen|kernel_pll_n_cnt_clk*" [lindex $lines $index]] } {
      # don't add kernel PLL output clock to SDC file	
    } else {
      puts $temp_fp [lindex $lines $index]
    }
    incr index
  }
  close $temp_fp

  # Replace sdc file
  file delete -force "$sdc_name"
  file rename -force "temp.sdc" "$sdc_name"  
}

