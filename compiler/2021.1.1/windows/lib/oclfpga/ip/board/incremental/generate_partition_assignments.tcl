# anything that must be done before an incremental compile begins should be done here
# fetches list of partition_names => hpath assignments (if pre-existing) and loads into memory
# if not, uses hpath assignments to generate this list, write to file
# once partition_names => hpath assignments are in memory, write out synth_partition.tcl files

source "$::env(INTELFPGAOCLSDKROOT)/ip/board/incremental/incremental_utils.tcl"
#### MAIN ####
# set the partitions
# if already set, they'll be loaded in current_partitions.txt. Else, set_partitions will have a
# list and the name => hpath map will be generated and stored in current_partitions.txt
array set current_partitions [load_partitions "current_partitions.txt" "set_partitions.txt"]

# create a map from hpath to partition name
array set reverse_current_partitions [reverse_array [array get current_partitions]]

# loads saved partitions that one wishes to preserve based on hpath only
set saved_list [load_list "saved_partitions.txt"]
array set saved_partitions {}
foreach s $saved_list {
  if {[info exists reverse_current_partitions($s)]} {
    set saved_partitions($reverse_current_partitions($s)) $s
  }
}

# guarantee they have partitions set
set split_saved_w_partitions [intersect_arrays [array get current_partitions] [array get saved_partitions]]
array set saved_partitions [lindex $split_saved_w_partitions 0]
array set not_saved_partitions [lindex $split_saved_w_partitions 1]

# set the partitions first
write_partitions_tcl [array get current_partitions] $top_path $soft_region_on

# set the preserved partitions now
write_preserve_partitions_tcl [array get saved_partitions] $top_path $acl_incr_initial_state_

# output the results of this
post_message ""
post_message "Preserving the following partitions:"
parray_no_nulls [array get saved_partitions]
post_message ""
post_message "Not preserving the following partitions:"
parray_no_nulls [array get not_saved_partitions]
post_message ""
post_message "Current partitions for this compile:"
parray_no_nulls [array get current_partitions]
post_message ""
