# Helper functions for incremental compile tcl scripts

# slurps contents of file into return value
# file_name: file who's contents are loaded
# Returns contents of file or empty string if file is not found
proc slurp_file {file_name} {
  if {[file exists $file_name]} {
    set fh [open $file_name "r"]
    set contents [read $fh]
    close $fh
    return $contents
  } else {
    return ""
  }
}

# dumps string into file, overwriting its contents
#
# file_name: target file to overwrite/create
# output: string to fill file with
#
# Returns nothing
proc dump_string {file_name output} {
  set fh [open $file_name "w"]
  puts -nonewline $fh $output
  close $fh
}

# uses list of h-paths in set_partition_list to generate partition-name => hpath pairs, then stores them
#
# set_partitions_file: file to newline separated list from
# saved_partitions_file: new lined separated list of partition name to hpath mappings
#
# Returns TCL array of name => hpath mapping
proc set_partitions {set_partitions_file saved_partitions_file} {
  set lines [load_list $set_partitions_file]
  array set partition_array {}
  set count 0
  set partition_prefix "kernel_system_partition"
  set partition_output ""
  set comma ","
  set newl "\n"
  foreach line $lines {
    if {[string length $line]} {
      set key [concat $partition_prefix$count]
      incr count
      set partition_array($key) $line
    }
  }
  store_array $saved_partitions_file [array get partition_array]
  return [array get partition_array]
}

# reverses a map, swapping keys with values, returning the reversed array - original is unchanged
#
# original_array: array to be reversed
#
# Returns reversed TCL array, such that original values are now keys => original keys
proc reverse_array {original_array} {
  array set orig $original_array
  array set reverse_res {}
  foreach {k v} [array get orig] {
    set reverse_res($v) $k
  }
  return [array get reverse_res]
}

# loads list from file (newline separated)
#
# saved_list_file: file that contains newline separated list
#
# Returns list of contents
proc load_list {saved_list_file} {
  set data [slurp_file $saved_list_file]
  set lines [split $data "\n"]
  set return_list [list]
  foreach line $lines {
    if {[string length $line]} {
      lappend return_list $line
    }
  }
  return $return_list
}

# loads partitions either from partition map (current_partitions.txt) or set_partitions method
# if saved_partitions_file already exists, its contents are loaded and compared such that any
# new partitions from set_partitions_file are added/removed - the others remaining unchanged
#
# saved_partitions_file: File to contain name,hpath mappings for each partition
# set_partitions_file: File containing newline separated list of hpaths to be added/removed from
#   saved_partitions_file
#
# Returns: TCL array of partition_names -> hpath mappings
proc load_partitions {saved_partitions_file {set_partitions_file "set_partitions.txt"}} {
  array set partition_array {}
  if {![file exists $saved_partitions_file]} {
    # generate the file
    array set partition_array [set_partitions $set_partitions_file $saved_partitions_file]
  } else {
    # compare file to set_partitions and see which have changed, then update the file
    set saved_list [load_list $set_partitions_file]
    array set current_partitions [load_array $saved_partitions_file]
    array set reverse [reverse_array [array get current_partitions]]
    set counter [array size reverse]
    set prefix "kernel_system_partition"
    # filter through set_partitions list for deleting/adding partitions
    foreach k $saved_list {
      if {[info exists reverse($k)]} {
        # exists, so keep it
        # reverse($k) is the partition name - $k is the h-path
        set partition_array($reverse($k)) $k
      } else {
        # new partition, add it
        set partition_array($prefix$counter) $k
        incr counter
      }
    }
    store_array $saved_partitions_file [array get partition_array]
  }
  return [array get partition_array]
}
# loads an array from a file. Assumes that file contains each entry as key1,value1
#
# array_file: file containing array to be parsed and loaded
#
# Returns TCL array of contents that were parsed
proc load_array {array_file} {
  set data [slurp_file $array_file]
  set lines [split $data "\n"]
  array set return_array {}
  foreach line $lines {
    if {[string length $line]} {
      set key_vals [split $line ","]
      set return_array([lindex $key_vals 0]) [lindex $key_vals 1]
    }
  }
  return [array get return_array]
}

# stores array into a target file in the format of key1,value1. Use with load_array to save arrays between
# compiles
#
# array_file: target file to store array into. Contents will be over-written
# array_val: array that will be stored
#
# Returns nothing
proc store_array {array_file array_val} {
  array set arr_to_store $array_val
  set output_string ""
  set newl "\n"
  set comma ","
  foreach {k v} [array get arr_to_store] {
    set output_string [concat $output_string$k$comma$v]
    set output_string $output_string$newl
  }
  dump_string $array_file $output_string
}

# Sets partitions in arrayList with the relative top-level path as a partition for the Quartus compile
#
# arrayList: TCL array containing partition_name => hpath mappings (relative to top_path)
# top_path: string containing the relative path to the partitions to be created
# soft_region_on: Set soft region settings on partition hierarchy
#
# Returns nothing
proc write_partitions_tcl {arrayList top_path soft_region_on} {
  array set partition_array $arrayList
  foreach {key value} [array get partition_array] {
    if {[string length $key] && [string length $value]} {
      set_instance_assignment -name PARTITION $key -to "$top_path|$value"

      if { $soft_region_on } {
        set_instance_assignment -name FLOATING_REGION AUTO -to "$top_path|$value"
        set_instance_assignment -name ATTRACTION_GROUP 100 -to "$top_path|$value"
        set_instance_assignment -name REGION_NAME ${key}_region -to "$top_path|$value"
      }
    }
  }
}

# Sets partitions in arrayList with the relative top-level path as a partition for the Quartus compile - to given state
#
# arrayList: TCL array containing partition_name => hpath mappings (relative to top_path)
# top_path: string containing the relative path to the partitions to be created
# state: state of preservation to set partitions
#
# Returns nothing
proc write_preserve_partitions_tcl {arrayList top_path state} {
  array set preserved_tcl $arrayList
  foreach {key value} [array get preserved_tcl] {
    if {[string length $key] && [string length $value]} {
      set_instance_assignment -name PRESERVE $state -to "$top_path|$value"
    }
  }
}

proc get_warning_codes {filename} {
  set lines [load_list $filename]
  array set codes {}
  foreach line $lines {
    regexp {Warning \(([0-9]*)\)} $line match code
    if {[info exists code]} {
      if {![info exists codes($code)]} {
        set codes($code) 1
      } 
    }
  }
  return [array get codes]
}

proc get_error_codes {filename} {
  set lines [load_list $filename]
  array set codes {}
  foreach line $lines {
    regexp {Error \(([0-9]*)\)} $line match code
    if {[info exists code]} {
      if {![info exists codes($code)]} {
        set codes($code) 1
      } 
    }
  }
  return [array get codes]
}

# Determines intersection of two TCL arrays based on keys. The second argument is used as the reference for
# this filter. If a key is found in arg1 but not arg2, it is placed in the non-intersection. If it is found
# in both, it is placed in the intersection
#
# arr1: first array, being filtered
# arr2: second array, acting as the filter
#
# Returns list of two arrays - first contains the keys that are common between the two arrays
#   second contains the keys are in arr1 but not arr2
proc intersect_arrays {arr1 arr2} {
  array set arr1_vals $arr1
  array set arr2_vals $arr2
  array set inter {}
  array set other {}
  foreach {k v} [array get arr1_vals] {
    if {[info exists arr2_vals($k)]} {
      set inter($k) $v
    } else {
      set other($k) $v
    }
  }
  return [list [array get inter] [array get other]]
}

# prints the contents of an array that have non-empty keys and vals
#
# arrayList: array to be printed
#
# Returns nothing
proc parray_no_nulls {arrayList} {
  array set array_out $arrayList
  foreach {k v} [array get array_out] {
    if {[string length $k] && [string length $v]} {
      post_message "  $k: $v"
    }
  }
}
