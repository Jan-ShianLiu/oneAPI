source $::env(INTELFPGAOCLSDKROOT)/ip/board/migrate/common_bak.tcl
set platform [lindex $quartus(args) 0]
set board_variant [lindex $quartus(args) 1]
set pr_base_id [lindex $quartus(args) 2]

set tmp_dir [preamble $platform $board_variant $pr_base_id $quartus(version)]
if {$tmp_dir == 1} {
  return
}

# checks if files exist
set copy_files [get_files_for_copy 0]
#
set necessary_copy_files [get_files_for_copy 1]
# check if all files to fill cache are present
set have_files [check_file_state $necessary_copy_files 1]
# see if cache is incomplete
set cache_complete [check_file_state $copy_files 1 $tmp_dir]
if {!$have_files || $cache_complete} {
  post_message "Missing files to fill cache or cache is full! Exiting..."
  return 1
}

# grab the lock file
set lock_file "write.lock"
set read_lock "read.lock"
set read_lock_rand $read_lock
append read_lock_rand [pseudo_random_string]

set lock_expire_write 45
set lock_expire_read 20
if {[acquire_lock $lock_file $lock_expire_write $tmp_dir $read_lock] == 1} {
 post_message "Cannot create a lock! Exiting..."
 return
}

# have write.lock. No other reads/writes can begin
# waiting for all reads to complete. If stale, delete
while {[llength [glob -nocomplain -directory $tmp_dir $read_lock*]]} {
  set current [clock seconds]
  foreach i [lsort [glob -nocomplain -directory $tmp_dir $read_lock*]] {
    set file_time $current
    set file_time [file mtime $i]
    if {[expr {$current - $file_time > $lock_expire_read}]} {
      post_message "Detected stale read.lock. Deleting"
      file delete -force $i
    }
  }
}

# copy back files
copy_from $copy_files [pwd] $tmp_dir

# release the lock
release_lock $tmp_dir/$lock_file
