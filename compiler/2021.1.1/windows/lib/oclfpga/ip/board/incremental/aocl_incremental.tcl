# Sets up intial parition settings for incremental flow before synthesis
# Synth_partition.tcl is used for setting the partitions to be preserved for a subsequent compile
# Partitions set to be unchanged during compile are specified in synth_preserve_partition.tcl
# and imported in import_preserved_designs.tcl

source "$::env(INTELFPGAOCLSDKROOT)/ip/board/incremental/incremental_utils.tcl"
source "$::env(INTELFPGAOCLSDKROOT)/ip/board/fast_compile/aocl_fast_compile.tcl"

# create the namespace
namespace eval ::aocl_incremental {
  # exports
  load_package report

  # initialization
  variable soft_region_file "soft_regions.txt"
  variable import_designs_tcl "import_preserved_designs.tcl"
  variable synth_preserve_final_tcl "synth_preserve_partition.tcl"
  variable package_root "$::env(INTELFPGAOCLSDKROOT)/ip/board/incremental"
  variable generate_source "generate_partition_assignments.tcl"
  variable export_source "export_incremental_partitions.tcl"
  variable incremental_compile
  variable soft_region_on
  variable util_source "util.tcl"
  variable current_partitions
  variable acl_incr_retry_strategy
  variable acl_incr_initial_state
  variable preserved_partitions_target_state "final"

  variable saved_partitions
  variable not_saved_partitions
  array set saved_partitions {}
  array set not_saved_partitions {}

  variable retry_strategies
  array set retry_strategies {}
  set retry_strategies("retry") "final synthesized"
  set retry_strategies("no-retry") "final"

  if {[catch {set incremental_compile $::env(AOCL_INCREMENTAL_COMPILE)} result]} {
    set incremental_compile 0
  } elseif {[expr ![string equal $incremental_compile "default"] && ![string equal $incremental_compile "aggressive"]]} {
    set incremental_compile 0
  } else {
    set incremental_compile 1
  }
}

proc ::aocl_incremental::is_incremental_compile {} {
  variable incremental_compile
  return $incremental_compile
}

# Performs an initial fit attempt and performs a fit retry attempt, erroring-out on failure
proc ::aocl_incremental::fit_retry {base rev top_path} {
  variable incremental_compile
  variable acl_incr_retry_strategy
  variable acl_incr_initial_state
  variable saved_partitions
  variable retry_strategies

  set errors_to_retry "170084 170143"
  set warnings_to_retry "17889"
  set selected_strategy $retry_strategies("$acl_incr_retry_strategy")

  if {[expr $incremental_compile && [expr [array size saved_partitions] != 0]]} {

    post_message "Initiating fitting sequence ($acl_incr_retry_strategy)"
    set selected_strategy $retry_strategies("$acl_incr_retry_strategy")

    foreach step [split $selected_strategy] {

      set fit_res [::aocl_incremental::qsf_fit $top_path $step $base $rev]

      if {[expr $fit_res == 0]} {
        post_message "Fit attempt @ $step successful!"
        break
      } else {
        array set errors [get_error_codes "top.fit.rpt"]
        array set warnings [get_warning_codes "top.fit.rpt"]

        post_message "Fit attempt @ $step failed!"

        set perform_retry 0
        foreach err [split $errors_to_retry] {
          if {[expr [lsearch [array names errors] $err] != -1]} {
            set perform_retry 1
          }
        }
        foreach warn [split $warnings_to_retry] {
          if {[expr [lsearch [array names warnings] $warn] != -1]} {
            set perform_retry 1
          }
        }

        if {[expr $perform_retry == 0]} {
          break
        }

        if {$step eq [lindex [split $selected_strategy] end]} {
          post_message -type critical_warning "BSP_MSG: Fitter has failed to find a solution for this incremental fit. A full recompile with neither incremental nor fast-compile may avoid this issue."
        } else {
          post_message -type critical_warning "BSP_MSG: Fitter has failed to find a solution for this incremental fit. To recover, your entire design will be re-fit."
          if {[::aocl_fast_compile::is_fast_compile]} {
            post_message -type critical_warning "BSP_MSG: You have enabled fast-compile. Subsequent incremental compiles will have a higher probability of exhibiting the same issue. A full recompile without fast-compile is recommended. If this warning frequently recurs on your design it is recommended to use fast-compile without incremental for faster compilation speeds."
          }
        }
      }

      post_message "Contents of saved_partitions: [array get saved_partitions]"
    }
  } else {
    # No special QSF settings here
    post_message "Performing a fit attempt"
    set fit_res [catch {qexec "quartus_fit $base -c $rev"}]
  }

  if {$fit_res} {
    post_message -type error "Quartus Fitter has failed! Breaking execution..."
    return 1
  }

  ::aocl_incremental::verify $top_path "Fitter"
  return $fit_res
}

proc ::aocl_incremental::qsf_fit {top_path state base rev} {
  variable saved_partitions
  variable preserved_partitions_target_state
  variable soft_region_on

  post_message "Performing a fit attempt with QSF state: $state"
  set preserved_partitions_target_state $state

  project_open $base -revision $rev
  if { $state eq "synthesized" && $soft_region_on } {
    ::aocl_incremental::remove_soft_settings
  }
  write_preserve_partitions_tcl [array get saved_partitions] $top_path $state
  project_close

  set fit_res [catch {qexec "quartus_fit $base -c $rev"}]
  return $fit_res
}

proc ::aocl_incremental::remove_soft_settings {} {
  post_message "Removing soft region setting"
  remove_all_instance_assignments -name ATTRACTION_GROUP_SOFT_REGION
}

proc ::aocl_incremental::init_incr_strategy {} {
  variable acl_incr_retry_strategy
  variable acl_incr_initial_state
  variable retry_strategies

  # Set default
  set acl_incr_retry_strategy "retry"
  set acl_incr_initial_state "final"

  if {[info exists ::env(INCREMENTAL_RETRY_STRATEGY)]} {
    if {[expr [lsearch [array names retry_strategies] $::env(INCREMENTAL_RETRY_STRATEGY)] != -1]} {
      set acl_incr_retry_strategy $::env(INCREMENTAL_RETRY_STRATEGY)
      set acl_incr_initial_state [lindex $retry_strategies($acl_incr_retry_strategy) 0]
    } else {
      post_message -type critical_warning [concat $::env(INCREMENTAL_RETRY_STRATEGY) "is presently unsupported: defaulting to retry"]
    }
  }

  post_message "Retry strategy set to \"$acl_incr_retry_strategy\""
  post_message "Initial preservation set to \"$acl_incr_initial_state\""
}

proc ::aocl_incremental::init_partitions {project_name revision_name top_path} {
  variable incremental_compile
  variable soft_region_on
  variable import_designs_tcl
  variable synth_preserve_final_tcl
  variable package_root
  variable soft_region_file
  variable generate_source
  variable saved_partitions
  variable not_saved_partitions
  variable current_partitions
  variable acl_incr_retry_strategy
  variable acl_incr_initial_state
  variable retry_strategies

  ::aocl_incremental::init_incr_strategy

  set soft_region_on [file exists $soft_region_file]
  
  # if incremental compile, continue
  if {$incremental_compile} {

    # Fix namespace issue
    set acl_incr_initial_state_ $acl_incr_initial_state

    # Sets saved_partitions and not_saved_partitions
    project_open $project_name -revision $revision_name
    source "$package_root/$generate_source"
    project_close
  }
}

# prefix: the string to prefix report searches with. Leave blank "" if no prefix.
# file_to_iterate: the file to check the content of
# report_to_analyze: the report to compare the file against
# target_state: the value that each entry in the file has to have in the report
# error_msg: what to say when an entry does not match its target_value
# file_to_exclude: if content from $file is in here, it won't be analyzed (leave blank "" if no such need)
# stream: the pipe to report to
proc ::aocl_incremental::verify_step {prefix file_to_iterate report_to_analyze target_state error_msg file_to_exclude stream} {

  # Initialize arrays
  array set file $file_to_iterate
  array set report $report_to_analyze

  foreach {k line} [array get file] {
    set unwanted 0

    # Unwanted if line is empty or is included in the exclusion file
    if {[string trim $line] == ""} {
        set unwanted 1
    } elseif {$file_to_exclude != ""} {
      if {$line in $file_to_exclude} {
        set unwanted 1
      }
    }

    if {$unwanted == 0} {
      set index -1
      set counter 0

      if {[info exists report($prefix$line)]} {
        if {$target_state == $report($prefix$line)} {
          post_message -type info "$line,$target_state,correct"
          puts $stream "$line,$target_state,correct"
        } else {
          set err "Incremental partition verification has failed. $line $error_msg \"$report($prefix$line)\", should be \"$target_state\""
          post_message -type error $err
          puts $stream $err
        }
      } else {
        post_message -type error "Could not find entry \"$prefix$line\"."
        exit 1
      }
    }
  }

  return TCL_OK
}

# Returns the name of the file to output for each process
proc ::aocl_incremental::get_report_name { process } {
  if {$process == "Synthesis"} {
    return "partitions.syn.rpt"
  } elseif {$process == "Fitter"} {
    return "partitions.fit.rpt"
  } else {
    post_message -type error "Tried to get report name for unknown process. Must be \"Synthesis\" or \"Fitter\"."
    exit 3
  }
}

proc ::aocl_incremental::verify { prefix process } {

  variable incremental_compile
  variable saved_partitions
  variable not_saved_partitions
  variable preserved_partitions_target_state

  post_message -type info "Executing post-[string tolower $process] verification for OpenCL Incremental partitions"

  if {$incremental_compile} {

    if {[string index $prefix [expr [string length $prefix] - 1]] ne "|"} {
      set prefix "$prefix\|"
    }

    # Opens an output stream by the appointed name
    set report_name [::aocl_incremental::get_report_name $process]
    set report_stream [open $report_name w]

    post_message -type info "Incremental compile detected - continuing..."

    # Get report
    project_open top
    load_report
    set panel "$process||$process Partition Summary"
    set id [get_report_panel_id $panel]
    set num_of_rows [get_number_of_rows -id $id]
    set col_name {Preservation}
    set col_id [get_report_panel_column_index -id $id $col_name]
    if {$col_id < 0} {
      post_message -type error "Could not find column named \"$col_name\" in Partition Summary report panel"
      unload_report
      project_close
      return 1
    }
    array set partition_summary {}
    for {set i 1} {$i < $num_of_rows} {incr i} {
      set s [get_report_panel_row -row $i -id $id]
      set partition_summary([lindex $s 1]) [lindex $s $col_id]
    }
    unload_report
    project_close

    ::aocl_incremental::verify_step $prefix [array get saved_partitions]      [array get partition_summary] $preserved_partitions_target_state "state in $process||$process Partition Summary is" ""                $report_stream
    ::aocl_incremental::verify_step $prefix [array get not_saved_partitions]  [array get partition_summary] ""                                 "state in $process||$process Partition Summary is" saved_partitions  $report_stream

    close $report_stream

  } else {
    post_message -type info "Incremental compilation not enabled - automatically succeeding [string tolower $process] partition verification"
  }

  return TCL_OK
}
