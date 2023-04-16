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

#
# Given OpenCL/HLS design that ran through full Quartus compile, get single Quartus
# critical path (path with lowest setup slack) and correlate it back to OpenCL/HLS
# source code.
#
# Works with 'aoc' compile 16.1 and before.
# AOC compile must've been run with "-fmax-analysis" option
#
# User arguments:
# Device family : "Stratix V", "Arria 10" are supported at the moment
# CLOCK name    : Optional, can be automatically determined by this script if no user argument
#
# CLOCK will be automatically determined by this script. 
# You can set CLOCK here to over-ride automatic determination
#
# Stratix V s5_ref clock
#set CLOCK    {system_inst|board|kernel_clk_gen|kernel_pll|altera_pll_i|stratixv_pll|counter[0].output_counter|divclk}
#
# a10_ref clock
#set CLOCK {board_inst|kernel_clk_gen|kernel_clk_gen|kernel_pll|outclk0}
#
# HLS clock
#set CLOCK    {clock}

# How many Quartus paths to analyze. Many paths will map to the same user code path
# and these duplicates will not be shown.
set NUM_PATHS  100


package require ::quartus::project
package require ::quartus::report
package require ::quartus::sta



# handy constant
global UNKNOWN
set UNKNOWN "unknown"


# Global maps

# elements of critical path, as reported by sta
# this includes ports.
# indexed 0, 1, ...
global path_elems

# quartus nodes corresponding to elements. No ports. Fewer
# indexed by path_elems
global path_nodes

# file/line locations of the nodes
# indexed by path_nodes
global path_locs

# raw (unprocessed) cache of path_loc, as queried from Quartus
global get_name_info_cache

# content of RTL sources for path_locs
# indexed by path_locs
global rtl_lines

# LLVM IR instructions corresponding to rtl_lines
# indexed by rtl_lines
global llvm_insts

# OpenCL/HLS content file/line locations for llvm_insts
# indexed by llvm_insts
global llvm_locs

# Slack of each path
global path_slacks

# Total cell and interconnect delay for each path
global path_delays

global node_delays

# Set to 1 to enable additional debug output
global debug


set path_nodes($UNKNOWN) $UNKNOWN
set path_locs($UNKNOWN) $UNKNOWN
set get_name_info_cache($UNKNOWN) $UNKNOWN
set rtl_lines($UNKNOWN) $UNKNOWN
set llvm_insts($UNKNOWN) $UNKNOWN
set llvm_locs($UNKNOWN) $UNKNOWN
# set source_lines($UNKNOWN) $UNKNOWN

# Read content of filename and return as list of lines
proc slurp_file {filename} {
  catch {set fptr [open $filename r]}
  set contents [read -nonewline $fptr]
  close $fptr
  return [split $contents "\n"]
}

# Given a text file containing output of 
# report_timing -from_clock { my_clock } -to_clock { my_clock } -setup -npaths 1 -detail path_only -panel_name {Report Timing}
# Returns list of nodes on "data path" of "Data Arrival Path" panel.
# List of nodes will have ports removed.
proc get_list_of_verilog_names_on_data_path {file_name reqd_path_num} {

  global UNKNOWN
  global path_elems
  global path_nodes
  global path_slacks
  global path_delays
  global node_delays
  
  if { [string equal $file_name $UNKNOWN] } {
    return;
  }
  
  set splitCont [slurp_file $file_name]
  
  set path_num -1
  set parsing_panel 0
  set parsing_data_path 0
  set parsing_summary_of_paths 0
  set parsing_summary_cnt 0
  set line 0
  set elem_names {}
  foreach ele $splitCont {
  
    #Extract slack of each path from summary
    if {[string match {*Summary of Paths*} $ele]} {
      set parsing_summary_of_paths 1
    }
    
    if { $parsing_summary_of_paths && [regexp {; (\-[0-9]+\.[0-9]+) +;.*} $ele matched sub1] } {
      set path_slacks($parsing_summary_cnt) $sub1
      incr parsing_summary_cnt
    }
    
    #Start/stop panel parsing
    if {[string match {*Data Arrival Path*} $ele]} {
      incr path_num
      if { $path_num == $reqd_path_num } {
        set parsing_panel 1
        set total_ic_delay 0
        set total_cell_delay 0
      }
    }
    if {[string equal $ele ""]} {
      set parsing_panel 0
      set parsing_data_path 0
      set parsing_summary_of_paths 0
    }
    
    # Start/stop data_path parsing
    if {[string match {*; data path*} $ele] && $parsing_panel} {
      set parsing_data_path 1
    }
    if {[string match {\+------*} $ele] && $parsing_data_path} {
      # puts "Finished data path parsing"
      set parsing_panel 0
      set parsing_data_path 0
      
      # Process collected node names
      # First and last are actual names, the rest are input/output ports.
      # Remove first and last names, strip ports from the rest
#      set pruned_names [lreplace $elem_names 0 0]     
      set pruned_names $elem_names
      set pruned_names [lreplace $pruned_names end-1 end]
      set processed_names {}
      set name_num 0
      set idx 0
      foreach pruned_delay_name $pruned_names {
        set p [lindex $pruned_delay_name 0]
        set p_delay [lindex $pruned_delay_name 1]
        
        # get rid of / -- just confuses things
        set slash_pos [string last "/" $p]
        if {$slash_pos > 0} {
          continue
        }
        
        set pipe_pos [string last "|" $p]
        # last element doens't have a port
        set name_no_port [string replace $p $pipe_pos end]
        
        set path_elems($name_num) $p
        incr name_num
        
        # Most names will appear twice: once with input port, once with output port
        # To avoid duplicates, only add once
        if { ![string equal [lindex $processed_names end] $name_no_port] } {
          lappend processed_names $name_no_port
          set path_nodes($p) $name_no_port
        }
        
        if { [info exists node_delays($name_no_port)] } {
          # puts "Adding node_delays($p) to node_delays($name_no_port)"
          set node_delays($name_no_port) [expr $node_delays($name_no_port) + $node_delays($p)]
        } else {
          # puts "Setting node_delays($name_no_port) to node_delays($p)"
          set node_delays($name_no_port) $node_delays($p)
        }
      }
      
      # Add last unpruned delay
      set last_unpruned_name [lindex $elem_names end]
      # puts "Adding $node_delays($last_unpruned_name) to node_delays($p)"
      set node_delays($p) [expr $node_delays($p) + $node_delays($last_unpruned_name)]
      
      set path_delays($path_num) [list $total_ic_delay $total_cell_delay]
      return $processed_names
    }
    
    
    if {$parsing_data_path} {
      # ignore "data path" line itself
      if {$line > 0} {
        set cols [split $ele ";"]
        set elem_name [string trim [lindex $cols end-1]]
        set incr_delay [string trim [lindex $cols 2]]
        set delay_type [string trim [lindex $cols 4]]
        lappend elem_names $elem_name
        
        set node_delays($elem_name) $incr_delay
        
        if { [string equal $delay_type "IC"] } {
          set total_ic_delay [expr $total_ic_delay + $incr_delay]
        } else {
          set total_cell_delay [expr $total_cell_delay + $incr_delay]
        }
      }
      incr line
    }
  }
}

proc assemble_regexp_from_parts { name_parts last_index wildcard_last_part } {

  # wildcard first entity
  set name_pat "*"
  
  if { $wildcard_last_part } {
    # Wild-card all module names 
    append name_pat [join [lrange $name_parts 0 $last_index] "|*:"]
    
  } else {
  
    # wild-card module names in front of all instantiation names except last
    append name_pat [join [lrange $name_parts 0 [expr $last_index - 1] ] "|*:"]

    # very last part doesn't have module name
    append name_pat "|" [lindex $name_parts $last_index]
  }
  
  
  return $name_pat
}


# Return file path for single Quartus node name
# num_to_drop_at_end specifies how many lowest levels of hierarchy to drop when
# searching for the name.
proc get_file_path_for_name {name num_to_drop_at_end} {

  global UNKNOWN
  global path_locs
  global get_name_info_cache
  
  # Split name into hierarchy
  set name_parts [split $name "|"]
  
  set last_idx [expr [llength $name_parts] - 1 - $num_to_drop_at_end]
  
#  Had to do this for Stratix V. Don't need to do it for Arria 10
#  set wildcard_last_part [expr $num_to_drop_at_end > 0]
#  set name_pat [assemble_regexp_from_parts $name_parts $last_idx $wildcard_last_part]
  set name_pat [join [lrange $name_parts 0 $last_idx] "\|"]


  # Don't waste time if already have this
  if { [info exists path_locs($name)] } {
    return $path_locs($name);
  }

  set file_path UNKNOWN
  foreach_in_collection name_id [get_names -filter $name_pat -observable_type post_fitter] {

    # get_name_info lookups are very expensive, so caching them.
    if { [info exists get_name_info_cache($name_pat)] } {
      set file_path $get_name_info_cache($name_pat)
    } else {
      set file_path [get_name_info -info file_location $name_id]
      set get_name_info_cache($name_pat) $file_path
    }

   
    # Don't point to inside of our IPs, either acl or quartus ones. Go up a level instead.
    # Also go up a level for qnodes that couldn't be located by quartus to any RTL source.
    if {    [regexp {$::env(INTELFPGAOCLSDKROOT)/ip} $file_path matched]
         || [regexp {synth/acl_} $file_path matched]
         || [regexp {synth/lsu_} $file_path matched] 
         || [regexp {/tmp-clearbox/} $file_path matched] 
         || [regexp {/libraries/megafunctions/} $file_path matched]
         || [string equal $file_path ""] } {
         
      # Check below is not really needed but to guard against infinite recursion
      if { $num_to_drop_at_end < 10 } {
        set file_path [get_file_path_for_name $name [expr $num_to_drop_at_end + 1]]
      }
    }
    
    set path_locs($name) $file_path
  }
  
  if { ![info exists path_locs($name)] } {
    set path_locs($name) $UNKNOWN
    set file_path $UNKNOWN
  }
    
  return $file_path
}


# Given list of Quartus nodes, return list of {node name, file name(line numbers)}.
# The length of resulting list may be smaller than the list of crit_path_names
# because multiple Quartus nodes may come from one line of Verilog.
# That's why node name is returned -- so caller can tell which node name file name/line
# corresponds to.
proc get_file_path_list {crit_path_names} {

  global UNKNOWN
  global path_locs

  set file_path_list {}
  foreach name $crit_path_names {

    set file_path [get_file_path_for_name $name 0]
    
    # Prune duplicate file_path locations.
    # There could be multiple nodes from one line of Verilog (e.g. one 32-bit compare
    # implemented using many ALMs) but only one LLVM IR instruction will result from 
    # one line of Verilog.
    if { ![string equal [lindex [lindex $file_path_list end] 1] $file_path] } {
      lappend file_path_list [list $name $file_path]
    }
  }
  return $file_path_list
}


# Given an VHDL line, return {true, signal_name} if given line contains
# VHDL signal declaration. If not, returns {false, <undefined>}
proc VHDL_line_is_signal_declaration { rtl_line } {

  global debug
  
  if { [regexp {signal (.*) : STD_LOGIC_VECTOR } $rtl_line matched signal_name] } {
    
    if { $debug } {
      puts "rtl line $rtl_line is a declaration for $signal_name"
    }
    return [list 1 $signal_name]
  } else {
    return [list 0 "not_a_signal_name"]
  }
}


# Find line of code in VHDL output that defines given signal.
# Inputs: VHDL file name, line number where a signal is *declared*
# Output: line number where signal is defined.
proc get_VHDL_signal_definition_line { rtl_lines signal_name declaration_line_number } {
  
  for {set i $declaration_line_number} {$i < [llength $rtl_lines]} {incr i} {
    if { [string match "* $signal_name <= *" [lindex $rtl_lines $i]] } {
      return $i
    }
  }
  return -1
}


# Given list with elements {node_name file_name(line_num)}, return
# list of LLVM_IR instructions annotated in file_name corresponding
# to given locations.
# LLVM IR instructions are "computed" by traversing up from line_num
# until line starting wtih "// LLVM_IR:" is seen
proc get_list_of_LLVM_IR_instructions_from_file_path { name_fileloc } {

  global UNKNOWN
  global rtl_lines
  global llvm_insts
  global llvm_locs
  global debug
  
  set comment "//"
  if { [string match {\.vhd$} $name_fileloc ] } {
    set comment "--"
  }

  foreach loc $name_fileloc {
  
    set name    [lindex $loc 0]
    set fileloc [lindex $loc 1]
    
    # fileloc is of format filename(number)
    set fileloc_split [split $fileloc ()]
    set filename [lindex $fileloc_split 0]
    set linenum  [lindex $fileloc_split 1]

    # Don't waste time if already have this
    if { [info exists rtl_lines($name)] } {
      continue;
    }

    # set to unknown now, will over-write if found
    set llvm_insts($fileloc) $UNKNOWN
    set rtl_lines($fileloc) $UNKNOWN
    
    if { [string length $filename] > 0 && ![string equal $filename $UNKNOWN] } {
    
      set splitCont [slurp_file $filename]
      set RTL_line [lindex $splitCont $linenum-1]  
      set rtl_lines($fileloc) $RTL_line
      
      set rtl_line_info [VHDL_line_is_signal_declaration $RTL_line]
      if { [lindex $rtl_line_info 0] } {
        # D'oh! Quartus pointed us to declaration line, not the place
        # where signal is assigned value. So find that line below instead.
        set signal_name [lindex $rtl_line_info 1]
        set rline [get_VHDL_signal_definition_line $splitCont $signal_name $linenum]
        if { $rline != -1 } {
          if { $debug } {
            puts "Changing from declaration line $linenum to definition line $rline"
          }
          set linenum $rline
          set RTL_line [lindex $splitCont $rline]
          set rtl_lines($fileloc) $RTL_line
        }
      }
      
      # Search up, upto 500 lines, to find LLVM IR instruction
      set found 0
      set found_i_value 0
      set found_comment 0
      set last_comment ""
      set cur_llvm_inst ""
      for {set i [expr $linenum-1]} {$i > [expr $linenum-500] && $i > 0} {incr i -1} {
      
        set cur_line [lindex $splitCont $i]
        if { [string match {* LLVM IR:*} $cur_line ] } {

          set LLVM_IR_line $cur_line
          # strip "// LLVM IR" prefix
          set LLVM_IR_line [regsub -- {.* LLVM IR:   (.*)} $LLVM_IR_line "\\1"]
          # strip debug info part
          set LLVM_IR_line [regsub -- {(.*), !dbg.*} $LLVM_IR_line "\\1"]
          
          # puts "$RTL_line, $LLVM_IR_line"
          
          set llvm_insts($fileloc) $LLVM_IR_line
          
          set found_i_value $i
          set cur_llvm_inst $LLVM_IR_line
          incr found
          break
        }
        
        if { [string match ".* Register node:" $cur_line ] } {
        
          set LLVM_IR_line "Pipelining register"
          set llvm_insts($fileloc) $LLVM_IR_line
          
          set found_i_value $i
          set cur_llvm_inst $LLVM_IR_line
          incr found
          break
        }

        set trimmed_line [string trim $cur_line]
        if { [string equal $trimmed_line ""] && ![string equal $last_comment ""] } {
          # Hit a comment but didn't find an IR/Location line.
          # So use the comment
          set llvm_insts($fileloc) $last_comment
          incr found
          break
        }
      }
      
      if {$found} {
        # Check for lines with "// Location: " to get LLVM IR location info.
        # These would appear right after the "// LLVM IR:" line
        # There maybe multiple Locations. Pick at most first 10.
        for {set i [expr $found_i_value+1]} {$i < [expr $found_i_value+10]} {incr i} {
          if { [string match {* Location:*} [lindex $splitCont $i] ] } {

            set llvm_inst_loc [lindex $splitCont $i]
            # Strip "Location:" prefix
            set llvm_inst_loc [regsub -- {.* Location: (.*)} $llvm_inst_loc "\\1"]
            # Strip column number -- it's useless
            set llvm_inst_loc [regsub -- "(.*):.*" $llvm_inst_loc "\\1"]
            
            set llvm_locs($cur_llvm_inst) $llvm_inst_loc 
          }
        }
      }
    }
  }
}


# Produce nice LLVM instruction description. 
# So far, just grap instruction name and options
# e.g. "%0 = icmp ult i32 %x, %y, <debug data>" --> "icmp ult i32"
proc get_llvm_inst_desc { full_inst } {

  set equal_loc [string first " = " $full_inst]
  
  if { [string match "* = select *" $full_inst] } {
    # for selects, grab second argument's data type, as the first will always
    # be i1. Below is a 32-bit select, not 1-bit select:
    # e.g. "%_229 = select i1 %cmp53.i, i32 %shr.i11, i32 %or.i" --> "select i32"
    set comma_loc [string first "," $full_inst $equal_loc]
    set percent_loc [string first "%" $full_inst $comma_loc]
    set data_type [string range $full_inst [expr $comma_loc+2] [expr $percent_loc-1]]
    return "select $data_type"
    
  } elseif { [regexp {.* = call (.*) @acl\.load.*addrspace\(\d+\)} $full_inst matched type addrspace] } {
    return "load $type"
  
  } else {
  
    # All other instructions
    set percent_loc [string first "%" $full_inst $equal_loc]
    return [string trim [string range $full_inst [expr $equal_loc+3] [expr $percent_loc-1]]]
  }
}


# Generate user-friendly message based on known info about one node
proc get_user_msg { qnode qpath llvm_inst user_loc } {

  global UNKNOWN
  set result ""
  

  if { [string equal $qnode $UNKNOWN] } {
    return $result
  }

  # get kernel and block names from qnode
  if { [regexp {\|kernel_([^\|]+)_inst_(\d+)|kernel\|} $qnode matched sub1 sub2] } {
    set kernel_name $sub1
    set inst_num $sub2
    set result "Kernel $kernel_name"
    
  } elseif { [regexp {^([^\|]+)_inst\|} $qnode matched sub1] } {
    set result "Component $sub1"
  }


  # If have LLVM instruction, work on that.
  if { ![string equal $llvm_inst $UNKNOWN] } {

    if { ![string equal $user_loc $UNKNOWN] } {
      # Have location in user's code.
      set user_location "at $user_loc"
    } else {
      set user_location "at unknown location"
    }

    if { [regexp {.*acl.push_.*(<.*>).*} $llvm_inst matched sub1] } {
      return "$result, loop-carried write of size $sub1 $user_location"
    }
    if { [regexp {.*acl.pop_.*(<.*>).*} $llvm_inst matched sub1] } {
      return "$result, loop-carried read of size $sub1 $user_location"
    }
    if { [regexp {.*acl.store.*, i(\d+) addrspace} $llvm_inst matched sub1] } {
      return "$result, store of size $sub1 $user_location"
    }
    if { [regexp {.*acl.load.*, i(\d+) addrspace} $llvm_inst matched sub1] } {
      return "$result, load of size $sub1 $user_location"
    }
    if { ![string match {*%*} $llvm_inst] } {
       # if here, llvm instruction doesn't have an IR variable name and 
       # is not one of the recognized instructions. So probably a comment
       # that describes something internal.
       return $llvm_inst
    }

    set result "$result, instruction \"[get_llvm_inst_desc $llvm_inst]\" $user_location"
    return $result
  }
  
  # If here, don't have LLVM instruction and need to work with Quartus node name
  
  # Is this a local mem system?
  if { [regexp {.*local_mem_system_aspace.*} $qnode matched] } {
    if { [regexp {.*port[0-9]bank.*} $qnode matched] } {
      return "Local memory port arbitration"
    } elseif { [regexp {local_mem_group.*router.*router.*} $qnode matched] } {
      return "Local memory port arbitration"
    } else {
      return "Local memory access"
    }
  }
  
  # Is this some funny LLVM instruction?
  if { [regexp {.*vectorpush.*insert.*} $qnode matched] } {
    return "$result, loop-carried write"
  }
  if { [regexp {.*_acl_pop_(i[0-9]+)_.*} $qnode matched sub1] } {
    return "$result, loop-carried read of size $sub1"
  }
  if { [regexp {.*_notexitcond.*fifo.*} $qnode] ||
       [regexp {.*_last_initeration.*fifo.*} $qnode] ||
       [regexp {.*_next_initeration.*fifo.*} $qnode] ||
       [regexp {.*_next_cleanups.*fifo.*} $qnode] ||
       [regexp {.*_keep_going.*fifo.*} $qnode] ||
       [regexp {.*_throttle.*fifo.*} $qnode] } {
    return "$result, loop control logic"
  }
 
  # Is this part of pipelining?
  if { [regexp {.*rnode_\d+to\d+_.*\|r_data_NO_SHIFT_REG} $qnode] } {
    return "$result, exit from data pipelining fifo"
  }

  # Is this part of an LSU?
  if { [regexp {.*lsu_local_.*\|pipelined_} $qnode] } {
    return "$result, load-store unit internals"
  }
  
  # NEW BACKEND NAMES
  if { [regexp {.*sr_valid_q} $qnode] } {
    return "$result, stall-valid signal"
  }
  
  # some port, shows up on A10
  if { [regexp {.*\/labout} $qnode] } {
    return "$result, <ignore me>"
  }
 
  # No idea
  set result "$result, unmapped Quartus node $qnode"
  return $result
}

global printed_detailed_path
set printed_detailed_path 0

# Print both concise and detailed descriptions of the critical path
proc print_user_messages {} {

  global UNKNOWN
  global path_elems
  global path_nodes
  global path_locs
  global rtl_lines
  global llvm_insts
  global llvm_locs
  global node_delays
  global printed_detailed_path
  global debug

  set user_msg {}
  set detail_msg {}

  set prev_msg ""
  set prev_llvm_inst $UNKNOWN
  set cur_delay_acc 0
  
  foreach i [lsort -integer [array names path_elems]] {
    lappend detail_msg "------------"
    lappend detail_msg "i = $i"
    
    set cur_elem $UNKNOWN
    set cur_node  $UNKNOWN
    set cur_path_loc  $UNKNOWN
    set cur_rtl  $UNKNOWN
    set cur_llvm_inst $UNKNOWN
    set cur_llvm_loc  $UNKNOWN
    
    if { [info exists path_elems($i)] } {
      set cur_elem $path_elems($i)
      lappend detail_msg "Path elem: $cur_elem"
    }
    
    if { [info exists path_nodes($cur_elem)] } {
      set cur_node $path_nodes($cur_elem)
      lappend detail_msg "Quartus node: $cur_node"
    }
    
    if { [info exists path_locs($cur_node)] } {
      set cur_path_loc $path_locs($cur_node)
      lappend detail_msg "Quartus source: $cur_path_loc"
    }
    
    if { [info exists rtl_lines($cur_path_loc)] } {
      set cur_rtl $rtl_lines($cur_path_loc)
      lappend detail_msg "RTL line: $cur_rtl"
    }
    
    if { [info exists llvm_insts($cur_path_loc)] } {
      set cur_llvm_inst $llvm_insts($cur_path_loc)
      lappend detail_msg "LLVM inst: $cur_llvm_inst"
    }
    
    if { [info exists llvm_locs($cur_llvm_inst)] } {
      set cur_llvm_loc $llvm_locs($cur_llvm_inst)
      lappend detail_msg "OpenCL source line: $cur_llvm_loc"
    }
        
    # Generate user-friendly message based on
    # Quartus node, RTL filename, LLVM IR instruction, and user code location
    set cur_msg [get_user_msg $cur_node $cur_path_loc $cur_llvm_inst $cur_llvm_loc]
    
    if { ![string equal $cur_msg ""] && ![string equal $cur_msg $prev_msg] } {
      if { ![string equal $prev_msg ""] } {
        lappend user_msg "- ($cur_delay_acc ns) $prev_msg"
      }
      set prev_msg $cur_msg
      set cur_delay_acc 0
      set prev_llvm_inst $cur_llvm_inst
    }
    
    # Increment delay after checking for new instruction.
    # This way, routing delay will be attributed to destination, not the source.
    if { ![string equal $cur_elem $UNKNOWN] } {
      set cur_delay_acc [expr $cur_delay_acc + $node_delays($cur_elem)]
    }    
  }
  
  lappend user_msg "- ($cur_delay_acc ns) $prev_msg"


  if { $debug && !$printed_detailed_path} {
    puts "\n\nDetailed 1x Clock Fmax critical path summary:"
    puts [join $detail_msg "\n"]
    puts "-------------------"
    puts "End of Detailed 1x Clock Fmax critical path summary"
    set printed_detailed_path 1
  }
  
  return [join $user_msg "\n"]
}


# Infer clock name to analyze from timing netlist, if it's not set inside this script.
proc infer_clock_name {} {
  
  global CLOCK
  
  # if CLOCK is over-ridden manually in this script, figure it out!
  if { ![info exists CLOCK] } {
  
    puts "Clock to analyze is not set inside the script."
    puts "Determining it automatically."
    
    foreach_in_collection clk [get_clocks] {
      set clk_name [get_clock_info -name $clk]
      puts "   Considering clock named $clk_name"
      if { [string equal "$clk_name" "clock"] } {
        # this is the HLS 1x clock. Take it!
        puts "Found clock to use: $clk_name"
        set CLOCK $clk_name
        break
      }
      if { [string match {*kernel_clk_gen|kernel_pll*0*} $clk_name] } {
        # this should be 1x kernel clock in an OpenCL design. 
        puts "Found clock to use: $clk_name"
        set CLOCK $clk_name
        break
      }
    }
    
    if { ![info exists CLOCK] } {
      puts "Could NOT determine clock to use!!!"
      puts "You must manually set CLOCK variable at the top of this script."
      exit 1
    }
  }
}


##################################################
set debug 0
if { $argc > 0 } {
  if { [string equal [lindex $argv 0] "-debug"] } {
    puts "Found -debug argument!"
    set debug 1
  }
}

# Determine project name
set qpf_list [glob -nocomplain *.qpf]
if { [llength $qpf_list] == 0 } {
  puts "No Quartus project file (.qpf) found!"
  exit 1
} elseif { [llength $qpf_list] > 1 } {
  puts "More than one qpf file found!"
}
set PROJECT_NAME [string range $qpf_list 0 end-4]
puts "Found Quartus project named $PROJECT_NAME"

set REVISION_NAME "NOT_FOUND"
set qsf_list [glob -nocomplain *.qsf]
if { [llength $qsf_list] == 0 } {
  puts "No Quartus project settings file (.qsf) found!"
  exit 1
} elseif { [llength $qsf_list] > 1 } {
  puts "More than one qsf file found."
  if { [file exists top.qsf] } {
    puts "Using top.qsf revision"
    set REVISION_NAME top
  } else {
    exit 1
  }
} else {
  # Only one revision in the list.
  set REVISION_NAME [string range $qsf_list 0 end-4]
}


project_open -revision $REVISION_NAME $PROJECT_NAME
load_report


set crit_path_filename "crit_path.txt"

# For script development purposes, don't load timing netlist if critical path dump already exists
if { ![file exists $crit_path_filename] } { 
  # Need to do these three steps before running any timing report
  create_timing_netlist
  read_sdc
  update_timing_netlist

  if {$argc > 0} {
    set CLOCK [ lindex $argv 0 ]
    puts "INFO: Clock $CLOCK\n"
  } else {
    infer_clock_name
  }

  # puts "Running Timing Report on 1x clock. Output going to $crit_path_filename"
  report_timing -from_clock $CLOCK -to_clock $CLOCK -setup -npaths $NUM_PATHS -detail path_only  -panel_name {Report Timing} -file $crit_path_filename
} else {
  puts "Local folder already contains $crit_path_filename. Using it instead of re-running the timing report."
}

puts "Summarizing $NUM_PATHS Quartus critical paths\n" 

# Store all printed paths so don't show them more than once
set printed_path($UNKNOWN) $UNKNOWN

set num_printed 0
for {set i 0} {$i < $NUM_PATHS} {incr i} {
  array unset path_elems
  array unset path_nodes
  array unset node_delays
  
  # do NOT clear other global arrays. Some nodes will appear on multiple critical paths
  # and we do not want to re-query Quartus for their locations. Calls to get_names is the
  # slowest part of this script!
  
  set crit_path_names [get_list_of_verilog_names_on_data_path $crit_path_filename $i]
  set file_locs [get_file_path_list $crit_path_names]
  get_list_of_LLVM_IR_instructions_from_file_path $file_locs

  set crit_path [print_user_messages]
  
  # strip times from user message, as these can change slightly
  # for an otherwise identical-looking paths
  set stripped_user_msg [regsub -all {\(\d+\.\d+ ns\)} $crit_path "removed"]
  
  if { ![info exists printed_path($stripped_user_msg)] } {
    set cur_slack $path_slacks($i)
    set cur_delays $path_delays($i)
    set   ic_delay [lindex $cur_delays 0]
    set cell_delay [lindex $cur_delays 1]
    set total_delay [expr $ic_delay + $cell_delay]
    
    puts "Critical path #$i (slack $cur_slack ns)"
    puts "Total path delay is $total_delay ns ($cell_delay cell delay + $ic_delay interconnect delay):"
    puts $crit_path
    if { [expr $ic_delay > [expr 0.75 * $total_delay]] } {
      puts "WARNING: Total path delay is dominated by interconnect delay (routing)."
      puts "         This happens either because the design is too full to place and route well or "
      puts "         because the compiler incorrectly under-pipelined critical logic."
      puts "         Changing code on this path is unlikely to improve fmax."
    }
    puts "--------"
    flush stdout
    
    set printed_path($stripped_user_msg) 1
    incr num_printed
  }
  
  if { [expr $num_printed > 4] } {
    puts "Already shown $num_printed different critical paths. No point showing more." 
    break
  }
}

# Print helpful info at the very end
puts "\nNotes:"
puts "- Many Quartus critical paths map to the same path in user code."
puts "- See LLVM reference for help interpreting LLVM instructions: http://llvm.org/docs/LangRef.html."
puts "- Instructions with no source location are generated by compiler."
puts "- Delay for an LLVM instruction includes interconnect delay *to* this instruction."
puts "  This interconnect delay may often be larger than the cell delay of the logic associated with"
puts "  the instruction."


