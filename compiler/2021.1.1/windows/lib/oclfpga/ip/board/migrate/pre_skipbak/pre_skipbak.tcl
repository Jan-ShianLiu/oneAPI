source $::env(INTELFPGAOCLSDKROOT)/ip/board/migrate/common_bak.tcl
set platform [lindex $quartus(args) 0]
set board_variant [lindex $quartus(args) 1]
set pr_base_id [lindex $quartus(args) 2]

set tmp_dir [preamble $platform $board_variant $pr_base_id $quartus(version) 0]
if {$tmp_dir == 1} {
  return
}

# checks if files exist
set copy_files [get_files_for_copy]
set continue_migration [check_file_state $copy_files 1 $tmp_dir]
if {!$continue_migration} {
  post_message "Missing files in cache! Exiting..."
  return 1
}
set copy_files [get_files_for_copy 0]

set lock_file "write.lock"
set read_lock "read.lock"
append read_lock [pseudo_random_string]

# check the lock
set lock_expire 45
if {[acquire_lock $lock_file $lock_expire $tmp_dir $read_lock] == 1} {
 return
}

# acquired the lock, meaning no other writes or read locks can be created
# make a read lock now
acquire_lock $read_lock $lock_expire $tmp_dir $read_lock

# release write.lock so that other reads can occur
release_lock $tmp_dir/$lock_file

# skip bak flow, copy it over for skipping
copy_from $copy_files $tmp_dir [pwd]

# release read lock
release_lock $tmp_dir/$read_lock
