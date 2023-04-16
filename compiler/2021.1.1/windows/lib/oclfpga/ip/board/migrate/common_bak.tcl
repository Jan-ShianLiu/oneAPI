# forms the cache directory based on the the platform, variant, id, and q version
# platform: General reference platform (eg a10_ref)
# board_variant: variant specific to this compile cache (eg a10gx)
# pr_base_id: ID of the PR region
# version: Quartus Version string (eg Version 17.1.0 Build 104 TO Version)
# Returns: [full_path path_without_cache]
proc get_cache_dir {platform board_variant pr_base_id version} {
  if {[string match $pr_base_id "-1"]} {
    set pr_base_id [get_pr_base_id]
  }
  # determine current version of quartus and use to make version directory
  if {[regexp {Version (\d*.\d*.\d*).*Build (\d*).*Patches (\d*.\d*)} $version -> version_num build_num patch_num]} {
    append version_num "_" $build_num
    append version_num "_" $patch_num
    append version_num "_" $pr_base_id
  } elseif {[regexp {Version (\d*.\d*.\d*).*Build (\d*)} $version -> version_num build_num]} {
    append version_num "_" $build_num
    append version_num "_" $pr_base_id
  }
  # remove spaces from name
  string map {" " _} $version_num
  set specific_dir $platform/$board_variant/$version_num
  return [list $::env(AOCL_TMP_DIR)/$specific_dir $specific_dir]
}

# iterates over list of files to ensure state is as expected
# files_exist: list of files to iterate over
# compare: state of existence - 1 checks if they all exist, 0 checks if all missing
# dir: optional argument to specify relative directory path to use as base for files
# Returns 1 if all match expected state, or 0 if one does not
proc check_file_state {files_exist compare {dir .}} {
  foreach f $files_exist {
    if {[file exists $dir/$f] != $compare} {
      # no match
      return 0
    }
  }
  # all match state
  return 1
}

# Makes a directory given a relative path as well. Returns 1 on error
# directory: directory to make
# root: base for that directory
# Returns: 1 if directory could not be created, 0 if created or exists as directory
proc recursive_mkdir {directory root} {
  if {![file exists $root/$directory]} {
    if {[catch {file mkdir "$root/$directory"} result]} {
      post_message "Could not create directory $root/$directory: $result"
      return 1
    }
  } elseif {[file exists $root/$directory] && ![file isdirectory $root/$directory]} {
      post_message "Found a file of the same name as the cache directory: $root/$directory."
      post_message "Please delete it or override the default cache directory ($::env(AOCL_TMP_DIR)) using AOCL_TMP_DIR"
      return 1
  }
  return 0
}

# Simple helper that unqars files that contain pr_base.id, which is then extracted
# Returns: pr_based_id if it is found
proc get_pr_base_id {} {
  source "scripts/qar_ip_files.tcl"
  unqar_ip_files
  set pr_handle [open "pr_base.id" r]
  set pr_base_id [read $pr_handle]
  close $pr_handle
  set pr_base_id [string trim $pr_base_id]
  return $pr_base_id
}

# Helper that generates a pseudo-random-string based on PID and time
# Returns: string contatenation of PID and time
proc pseudo_random_string {} {
  set ret [pid]
  append ret [clock seconds]
  return $ret
}

# Creates a file that acts as a lock. If the lock is already created,
# spins in a loop until the lock has been alive for $timeout time. If so,
# it is considered stale, and the contents of the directory is wiped
# This is the for the case that a write-back is occurring and the process
# is cancelled.
# lock_file: name of lock
# timeout: time before lock is considered stale
# tmp_dir: directory in which to create the lock
# test_lock: randomly-named file to test if tmp_dir is writeable. If it
# fails to be created, then exit
# Returns 1 if failed to create the lock or 0 if successful
proc acquire_lock {lock_file timeout tmp_dir test_lock} {
  if {[catch {close [open $tmp_dir/$test_lock {RDWR CREAT}]} error_message]} {
    post_message "Cannot create a lock! $error_message\nExiting..."
    return 1
  } else {
    file delete $tmp_dir/$test_lock
  }
  while {[catch {close [open $tmp_dir/$lock_file {RDWR CREAT EXCL}]} a_message]} {
    # lock is present. Check age and delete contents of tmp_dir
    # if old, because cache has not been able to be updated correctly
    set file_time 0
    set current [clock seconds]

    if {[file exists $tmp_dir/$lock_file]} {
      set file_time [file mtime $tmp_dir/$lock_file]
    } else {
      continue
    }
    if {[expr {$current - $file_time > $timeout}]} {
      post_message "Detected stale write.lock, which indicates the cache could not be updated correctly. Clearing the cache $tmp_dir"
      foreach name [glob $tmp_dir/*] {
        file delete -force $name
      }
    }
  }
  return 0
}

# Deletes a lock file if it is found
# Return 0 on success
proc release_lock {lock_file} {
  if {[file exists $lock_file]} {
    file delete $lock_file
  }
   return 0
}

# Copies files in a list from a src directory to the target directory
# if the files exist in the source directory
# files: list of files to copy
# src: source directory contaning the files to be copied
# target: destination of the files to be copied
# Returns nothing
proc copy_from {files src target} {
  foreach f $files {
    if {[file exists $src/$f]} {
      if {[catch {file copy -force $src/$f $target} copyError]} {
        post_message "File copy for $f failed."
      } else {
        post_message "File copy for $f succeeded"
      }
    } else {
      post_message "File does not exist: $f"
    }
  }
}

# Contains list of files needed for BAK Caching
# Returns this list only
proc get_files_for_copy {{necessary 1}} {
  if {$necessary} {
    return [list "qdb.qar" "base_bak.qar" "root_partition.qdb"]
  } else {
    return [list "qdb.qar" "base_bak.qar" "root_partition.qdb" "kernel_pll_refclk_freq.txt"]
  }
}

# Helper that perform common tasks for skip-bak TCL scripts. It creates
# the tmp_dir if necessary and then returns it. If it fails to do so,
# indicates failure
# See get_cache_dir for details on arguments (they are the same)
# create_dir: indicates whether or not to create the temporary directory
# Returns: 1 on failure to create the dir and the tmp_dir if it was created
proc preamble {platform board_variant pr_base_id version {create_dir 1}} {
  # set temporary dir and list of files to copy over
  set dir_list [get_cache_dir $platform $board_variant $pr_base_id $version]
  set tmp_dir [lindex $dir_list 0]
  set dir_path [lindex $dir_list 1]
  if {$create_dir && [recursive_mkdir $dir_path $tmp_dir] == 1} {
    return 1
  }

  return $tmp_dir
}
