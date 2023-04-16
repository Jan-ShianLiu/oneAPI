# create the namespace
namespace eval ::aocl_floorplanner {

  # initialization
  variable package_root "$::env(INTELFPGAOCLSDKROOT)/ip/board/floorplanner"
  variable floorplan_compile

}

# Helper functions for floorplanner tcl script
# read contents of file into return value
proc ::aocl_floorplanner::floorplanner_read_file {file_name} {
  if {[file exists $file_name]} {
    set fh [open $file_name "r"]
    set contents [read $fh]
    close $fh
    return $contents
  } else {
    return ""
  }
}

# Parse through floorplan.txt generated by system integrator, and set floorplan
proc ::aocl_floorplanner::set_floorplan {project_name revision_name pr_base_revision_name top_path} {
  variable floorplan_compile

  # File name must match floorplan file generated in acl/sysgen/system_integrator/floorplan.cpp
  set floorplan_file "floorplan.txt"
  set floorplan_compile [file exists $floorplan_file]
  
  if {$floorplan_compile eq 1} {
    set ioport_loc [::aocl_floorplanner::get_ioport_loc_from_pr_base $project_name $pr_base_revision_name]
    project_open $project_name -revision $revision_name
    set data [::aocl_floorplanner::floorplanner_read_file $floorplan_file]
    set lines [split $data "\n"]
    foreach line $lines {
      if { [regexp {chip-size ([0-9]+) ([0-9]+)} $line matched max_X max_Y]} {
        set chip_size_X $max_X
        set chip_size_Y $max_Y
      } elseif { [regexp {routing-region ([^\s]*) (\".*\")} $line matched kernel constraint] } {
        set_instance_assignment -name ROUTE_REGION -to "$top_path|$kernel" $constraint
      } elseif { [regexp {([^\s]*) ([\"].*[\"])} $line matched kernel constraint] } {
        set io_excluded_constraint [::aocl_floorplanner::remove_ioports $chip_size_X $chip_size_Y $constraint $ioport_loc]
        set_instance_assignment -name PLACE_REGION "$io_excluded_constraint" -to "$top_path|$kernel"
        set_instance_assignment -name RESERVE_PLACE_REGION ON -to "$top_path|$kernel"
        set_instance_assignment -name CORE_ONLY_PLACE_REGION ON -to "$top_path|$kernel"
      } elseif { [regexp {([^\s]*) ([^\s]*)} $line matched kernel region] } {
        set_instance_assignment -name REGION_NAME "$region" -to "$top_path|$kernel"
      }
    }
    project_close
  }
}

# Get ioports from pr_base revision compile and output as single string
# Output is X and Y coordinates separated by space: X<x-coord>_Y<y_coord>
# At this point in compile, only pr_base has run fit
proc ::aocl_floorplanner::get_ioport_loc_from_pr_base {project_name revision_name} {
  set node_loc_file "node_loc.loc"
  set io_loc_file "io_loc.loc"
  set ioport_coord ""

  if {$revision_name eq ""} {
    return ""
  } 

  # For incremental compile, keep and use io_loc.loc file from setup compile
  if {[file exists $io_loc_file]} {
    post_message "Floorplanner using previously exported io_loc file."
    set data [::aocl_floorplanner::floorplanner_read_file $io_loc_file]
    set ioport_coord $data
  } else {
    post_message "Floorplanner generating io_loc file from \"$revision_name\" revision."
    project_open $project_name -revision $revision_name
    load_package chip_planner

    read_netlist
    get_node_loc
    close_chip_planner
    project_close

    set data [::aocl_floorplanner::floorplanner_read_file $node_loc_file]
    set lines [split $data "\n"]
    foreach line $lines {
      if { [regexp {~IPORT\t.*(X[0-9]+_Y[0-9]+)} $line matched coord] } {
        if { $ioport_coord eq ""} {
          set ioport_coord $coord
        } else {
          set ioport_coord "$ioport_coord $coord"
        }
      } elseif { [regexp {~OPORT\t.*(X[0-9]+_Y[0-9]+)} $line matched coord] } {
        if { $ioport_coord eq ""} {
          set ioport_coord $coord
        } else {
          set ioport_coord "$ioport_coord $coord"
        }
      }
    }

    set io_loc_output_strem [open $io_loc_file w]
    puts $io_loc_output_strem "$ioport_coord"
    close $io_loc_output_strem
  }

  return $ioport_coord
}

# Remove ioport locations in ioports string from user constraint
# Input:
#       max_X - Maximum width in chip-planner
#       max_Y - Maximum height in chip-planner
#       constraints - User constraint
#       ioports - IOports obtained from get_ioport_loc_from_pr_base
proc ::aocl_floorplanner::remove_ioports {max_X max_Y constraints ioports} {
  incr max_X
  incr max_Y
  array set floorplan {}

  # Create array that's max_X by max_Y and set each element to 0
  for {set x 0} {$x <= $max_X} {incr x} {
    for {set y 0} {$y <= $max_Y} {incr y} {
      set index [expr {$max_Y*$x + $y}]
      set floorplan($index) 0
    }
  }

  # In floorplan array, set 1 for all rectangles in constraints
  regsub -all {\"} $constraints "" constraints
  set constraint_list [split $constraints ";"]
  foreach box $constraint_list {
    if { [regexp {([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)} $box matched x1 y1 x2 y2]} {
      for {set x $x1} {$x <= $x2} {incr x} {
        for {set y $y1} {$y <= $y2} {incr y} {
          set index [expr {$max_Y*$x + $y}]
          set floorplan($index) 1
        }
      }
    }
  }

  # In floorplan array, set 0 to all the IO port locations
  set ioport_list [split $ioports " "]
  foreach ioport $ioport_list {
    if { [regexp {X([0-9]+)_Y([0-9]+)} $ioport matched x_coord y_coord]} {
      set index [expr {$max_Y*$x_coord + $y_coord}]
      set floorplan($index) 0
    }
  }

  # Go through the floorplan array to get new constraint
  set first_region 1
  set io_excluded_constraint ""
  for {set x 0} {$x < $max_X} {incr x} {
    for {set y 0} {$y < $max_Y} {incr y} {
      set index [expr {$max_Y*$x + $y}]
      if {$floorplan($index)} {
        # Found bottom left corner of new constraint
        if {!$first_region} {
          set io_excluded_constraint "$io_excluded_constraint; "
        }
        set io_excluded_constraint "${io_excluded_constraint}$x $y"

        set floorplan($index) 0
        set x_cont 1
        set y_cont 1

        # Find heigh of new constraint by increasing Y
        set index [expr {$max_Y*$x + $y + $y_cont}]
        while {[expr {($y + $y_cont < $max_Y) && ($floorplan($index) == 1)}]} {
          set floorplan($index) 0
          incr y_cont
          set index [expr {$max_Y*$x + $y + $y_cont}]
        }

        set y_second [expr {$y + $y_cont - 1}]
        set y_top [expr {($y + $y_cont - 1) == ($max_Y - 1)}]
        set y_bottom [expr {$y == 0}]
        set stop 0

        set index_top [ expr {($x + $x_cont)*$max_Y + $y + $y_cont}]

        if {!$y_bottom} {
          set index_bottom [ expr {($x + $x_cont)*$max_Y + $y - 1}]
        } else {
          set index_bottom 0
        }

        # Find width of new constraint by increasing x
        while {[expr {($x + $x_cont < $max_X) &&
              ($y_top || ($floorplan($index_top) == 0)) &&
              ($y_bottom || ($floorplan($index_bottom) == 0))} ]} {
          for {set y_check $y} {$y_check < [expr {$y + $y_cont}]} {incr y_check} {
            set index [expr {($x + $x_cont)*$max_Y + $y_check}]
            if {$floorplan($index) == 0} {
              set stop 1
              break
            }
          }
          if {!$stop} {
            for {set y_set $y} {$y_set < [expr {$y + $y_cont}]} {incr y_set} {
              set index [expr {($x + $x_cont)*$max_Y + $y_set}]
              set floorplan($index) 0
            }
          } else {
            break
          }
          incr x_cont
          set index_top [ expr {($x + $x_cont)*$max_Y + $y + $y_cont}]
          if {!$y_bottom} {
            set index_bottom [ expr {($x + $x_cont)*$max_Y + $y - 1}]
          } else {
            set index_bottom 0
          }
        }

        set x_second [expr {$x + $x_cont - 1}]

        set io_excluded_constraint "$io_excluded_constraint $x_second $y_second"
        set first_region 0 
      }
    }
  }

  return $io_excluded_constraint
}
