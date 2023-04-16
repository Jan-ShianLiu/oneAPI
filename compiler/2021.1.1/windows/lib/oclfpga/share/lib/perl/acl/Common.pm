=pod

=head1 NAME

acl::Common - Common utility functions and constants for aoc and acl perl libs

=head1 COPYRIGHT

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


=cut

package acl::Common;
require Exporter;
use strict;
use File::Spec;

require acl::Env;

our @ISA = qw(Exporter);
our @EXPORT_OK = qw ( is_fcd_present remove_fcd setup_fcd is_installed_present save_to_installed
                      remove_from_installed populate_installed_packages populate_boards list_boards
                      list_packages get_default_board get_default_board_package is_board_in_the_bsp
                      load_mmd_libs parse_board_arg get_board_package_path 
                      get_bsppath_by_boardname backup_dirs update_dirs_to_update );

our @EXPORT = qw( $installed_bsp_list_dir $installed_bsp_list_file @installed_packages
                  $installed_bsp_list_registry %board_boarddir_map %dirs_to_backup);

# case:492127 need to explicitly exclude aoc options starting with -l
# from being considered a library name.
@acl::Common::l_opts_exclude = ('-list-deps', '-list-boards', '-list-board-packages',
                                '-list-board', '-list-board-package', # include these two in case user forget to include "s" in the end
                                '-llc-arg', '-library-debug', '-legacy-emulator',
                                '-library-list');

# $installed_fcd_dir is the directory for install fcd
# on Linux, it will be the either specified by ACL_BOARD_VENDOR_PATH or default as /opt/Intel/OpenCL/Boards
# on Windows, it will be the registry key Software\Intel\OpenCL\Boards, either under HKLM or HKCU
our $installed_fcd_dir = acl::Env::get_fcd_dir();

our $installed_bsp_list_dir = $installed_fcd_dir; # default to the installed_bsp_dir
our $installed_bsp_list_file = $installed_bsp_list_dir."/installed_packages";
our @installed_packages = ();
our %board_boarddir_map = ();

# Global variables
my @shipped_board_packages = ();

# Windows will use registry for tracking installed bsps unless custom location was provided through ACL_BOARD_VENDOR_PATH
# In that case, file-based system will be used just as in Linux
our $installed_fcd_regkey = $installed_fcd_dir;
our $installed_bsp_list_registry = $installed_fcd_dir."\\installed_packages";


# global variables
my $orig_dir = undef;    # absolute path of original working directory.
my $time_log_fh = undef; # Time various stages of the flow; if not undef, it is a
                         # file handle (could be STDOUT) to which the output is printed to.
my $pkg_file = undef;
my $src_pkg_file = undef;

# backup folders/files
our %dirs_to_backup; # key: src dir/file need to be backup; value: dest to backup the dir/file

# verbosity and temporary files controls
my $verbose = 0; # Note: there are two verbosity levels now 1 and 2
my $quiet_mode = 0; # No messages printed if quiet mode is on
my $save_temps = 0;

# In oneAPI, base toolkit contains partial BSP. It only supports emulation/runtime, no Quartus hardware files needed
my @shipped_board_packages_runtime = ();

# In OpenCL and oneAPI FPGA add-on (Beta06+), BSP contains all its files to support full FPGA development
my @shipped_board_packages_develop = ();

# Local Functions

sub update_dirs_to_update {
  my $new_dirs_to_backup_ref = shift;
  # reset the backupdir
  %dirs_to_backup = %{$new_dirs_to_backup_ref};
}

sub backup_dirs {
  foreach my $srcdir (keys %dirs_to_backup) {
    my $destdir = $dirs_to_backup{$srcdir};
    if (! -e $srcdir and $verbose > 1) {
      print STDERR  "Error: $srcdir does not exist\n";
      next;
    }
    if (-d $srcdir and -f $destdir and $verbose > 1) {
      print STDERR "Error: Can not move a directory: $srcdir to a file: $destdir\n";
      next;
    }

    acl::File::movedir($srcdir, $destdir); # move the dir

    if (-e $srcdir or ! -e $destdir and $verbose > 1) {
      print STDERR "Error: Can not move $srcdir to $destdir\n";
    }
  }
}

# update the fcd path
sub update_fcd_path {
  my $new_fcd_path = shift;
  if (acl::Env::is_linux()) {
    $installed_bsp_list_dir = $new_fcd_path;
    $installed_bsp_list_file = $installed_bsp_list_dir."/installed_packages";
  } else {
    $installed_fcd_regkey = "HKEY_CURRENT_USER\\Software\\Intel\\OpenCL\\Boards";
    $installed_bsp_list_registry = $installed_fcd_regkey."\\installed_packages";
  }
}

# Check if the user want to use the default dir/reg key to setup install package and fcd
sub check_installed_fcd_path {
   my $is_install = shift;
   my $setup_str = ($is_install eq "install") ? "setup" : "unset";
   my $valid_input = 0;
   if (acl::Env::is_linux()) {
     print "Do you want to $setup_str the FCD at directory $installed_fcd_dir [Y/n] ";
     my $user_input = <STDIN>;
     chomp $user_input;
     while (!$valid_input) {
       if ($user_input eq "" or $user_input eq 'y' or $user_input eq 'Y') {
         # using the current installed_fcd_dir, doing nothing
         $valid_input = 1;
       } elsif ($user_input eq 'n' or $user_input eq 'N') {
         # using the dir specified by the user to override installed_fcd_dir
         print "Please specify a directory to $setup_str FCD:\n";
         $installed_fcd_dir = <STDIN>;
         # Remove the potential trailing backslash and \n
         $installed_fcd_dir =~ s/\/$//;
         chomp $installed_fcd_dir;
         # update the installed_bsp_list_dir, installed_bsp_list_file and installed_bsp_list_file_maker
         update_fcd_path($installed_fcd_dir);
         $valid_input = 1;
       } else {
         print "Invalid user input. Please input [Y/n] \n";
         $user_input = <STDIN>;
         chomp $user_input;
       }
     }
   } elsif (acl::Env::is_windows()) {
     print "Do you want to $setup_str FCD for all users (requiring administrative privileges) [Y/n]?\n";
     my $user_input = <STDIN>;
     chomp $user_input;
     while(!$valid_input) {
       if ($user_input eq "" or $user_input eq 'y' or $user_input eq 'Y') {
         # using the current installed_fcd_regkey, doing nothing
         $valid_input = 1;
       } elsif ($user_input eq 'n' or $user_input eq 'N') {
         print "$setup_str FCD to current user\n";
         update_fcd_path("");
         $valid_input = 1;
       } else {
         print "Invalid user input. Please input [Y/n] \n";
         $user_input = <STDIN>;
         chomp $user_input;
       }
     }
   } else {
     die "No FCD setup procedure defined for OS '$^O'.\n";
     return;
   }
}

# Exported Functions

# Check if file exists on Linux
# Chech if registry exists on Windows unless ACL_BOARD_VENDOR_PATH is provided
sub is_installed_present() {
   if (acl::Env::is_linux()) {
      if (!(-e $installed_bsp_list_dir and -d $installed_bsp_list_dir)) {
         return 0;
      }

      if (-e $installed_bsp_list_file and -f $installed_bsp_list_file) {
        return 1;
      } else {
        return 0;
      }
   } elsif (acl::Env::is_windows()) {
      my $output = mybacktick("reg query $installed_bsp_list_registry 2>&1");
      if ($output !~ "^ERROR") {
        return 1;
      } else {
        return 0;
      }
   } else {
      return 0;
   }
}

# Save the given board package path to storage file
# Write to the file on Linux
# Add value to the registry on Windows unless ACL_BOARD_VENDOR_PATH is provided
sub save_to_installed {
   my $board_package_path = shift;
   my @lines = undef;
   my $is_present = 0;
   my $bsp_fh = undef;

   if (!is_installed_present()) {
      if (acl::Env::is_linux()) {
         if (!(-e $installed_bsp_list_dir and -d $installed_bsp_list_dir)) {
            if (! acl::File::make_path($installed_bsp_list_dir)) {
               # if the dir failed to create, try sudo command, mute the stdout/stderr
               my $cmd = "sudo mkdir -p $installed_bsp_list_dir >null 2>&1";
               system($cmd) == 0 or die "Unable to create $installed_bsp_list_dir\n";
            }
         }
         # create the installed packages file, with empty content
         # try open the file. if not be able to open it, try sudo command
         # lock the file for preventing the process racing
         acl::File::acl_flock($installed_bsp_list_file);
         if (open($bsp_fh, '>', $installed_bsp_list_file) < 1) {
            # cannot be opened, try sudo
            my $cmd = "sudo touch $installed_bsp_list_file >/dev/null 2>&1";
            system ($cmd) == 0 or die "Unable to create $installed_bsp_list_file\n";
         } else {
            # no need for sudo, write the the file handler
            # need to lock the file since it can be accessed by multiple processes the sametime
            print $bsp_fh "";
            # the lock can be freed when handler is closed
            close $bsp_fh;
         }
         # unlock the file
         acl::File::acl_unflock($installed_bsp_list_file);
      }
   }
   if (acl::Env::is_linux()) {
      # read all the installed packages
      unless(open $bsp_fh, '<'.$installed_bsp_list_file) {
         die "Unable to open $installed_bsp_list_file\n";
      }
      # need to lock the file to prevent race condition among processes
      acl::File::acl_flock($installed_bsp_list_file);
      @lines = <$bsp_fh>;
      chomp(@lines);
      close $bsp_fh;

      # check if need to append the bsp to the list
      foreach my $line (@lines) {
         if ($line eq $board_package_path) {
            $is_present = 1;
         }
      }
      if (!$is_present) {
         # the bsp is not presented, append to the installed file
         if (open($bsp_fh, '>>', $installed_bsp_list_file) < 1){
            # cannot be opened, try sudo
            my $cmd = "echo \"$board_package_path\" | sudo tee -a $installed_bsp_list_file >/dev/null 2>&1";
            system ($cmd) == 0 or die "Unable to open $installed_bsp_list_file\n";

         } else {
            # no need for sudo, write the the file handler
            print $bsp_fh "$board_package_path\n";
            # the lock can be freed when handler is closed
            close $bsp_fh;
         }
         # unlock the install_packages file
         acl::File::acl_unflock($installed_bsp_list_file);
      }
   } elsif (acl::Env::is_windows()) {
      # Replace forward slashes with back slashes:
      $board_package_path =~ s/\//\\/g;
      my $board_name = (split(/\\/, $board_package_path))[-1];
      # populate the bsp values at the installed list regkey and delete value included in the board_mmd_path
      # error message will not be printed if $installed_bsp_list_registry is not presented
      my @reg_query_arr = mybacktick("reg query $installed_bsp_list_registry /t REG_SZ 2> nul");
      foreach (@reg_query_arr) {
         if ($_ =~ /\bREG_SZ\b/) {
            my $bsp_path = (split(' ', $_))[0];
            if ($bsp_path =~ /\b$board_name\b/)
            {
               system ("reg delete $installed_bsp_list_registry /v $bsp_path /f>Nul") == 0 or print "Unable to delete registry entry\n" and return;
     	      }
         }
      }

    #Adds the registry entry.
    #/v $board_package_path to specify a value name (in this case, the name of the value is the path to the dll)
    #/t REG_DWORD to specify the value type
    #/d 0000 for the value data
    #/f to force overwrite in case the value already exists
    system ("reg add $installed_bsp_list_registry /v $board_package_path /f>Nul") == 0 or print "Unable to edit registry entry\n" and return;
   }

}

# Remove the given board package path from storage file on Linux
# Remove the given board package path from the registry on Windows unless ACL_BOARD_VENDOR_PATH is provided
sub remove_from_installed {
   my $board_package_path = shift;
   my @lines = undef;
   my $bsp_fh = undef;

   if (!is_installed_present()) {
     return;
   }
   if (acl::Env::is_linux()) {
      # read all the installed packages
      unless(open $bsp_fh, '<'.$installed_bsp_list_file) {
         die "Unable to open $installed_bsp_list_file\n";
      }
      # need to lock the file to prevent race condition among processes
      acl::File::acl_flock($installed_bsp_list_file);
      @lines = <$bsp_fh>;
      chomp(@lines);
      close $bsp_fh;

      # write all the installed packages back to storage except for
      # the one that has been uninstalled
      if ((open $bsp_fh, '>', $installed_bsp_list_file) < 1) {
         # cannot be opened, try sudo
         # first need to clean (empty) the bsp file, then append to it
         my $cmd = "true | sudo tee $installed_bsp_list_file >/dev/null 2>&1";
         system ($cmd) == 0 or die "Unable to write to $installed_bsp_list_file\n";
         foreach my $line (@lines) {
            if (not $line eq $board_package_path) {
              $cmd = "echo $line | sudo tee -a $installed_bsp_list_file >/dev/null 2>&1";
              system ($cmd) == 0 or die "Unable to write to $installed_bsp_list_file\n";
            }
         }

      } else {
         # no need for sudo, write the the file handler
         # first clean the file
         print $bsp_fh "";
         close $bsp_fh;
         open $bsp_fh, '>>', $installed_bsp_list_file;
         foreach my $line (@lines) {
            print $bsp_fh "$line\n" unless ($line eq $board_package_path);
         }
         # the lock can be freed when handler is closed
         close $bsp_fh;
      }
      # unlock the install_packages file
      acl::File::acl_unflock($installed_bsp_list_file);

   } elsif (acl::Env::is_windows()) {
      # Replace forward slashes with back slashes:
       $board_package_path =~ s/\//\\/g;

       #Remove the registry entry.
       system ("reg delete $installed_bsp_list_registry /v $board_package_path /f>Nul") == 0 or print "Unable to delete registry entry\n" and return;
     }
}


sub setup_fcd($$) {
   my $board_package_path = shift;
   my $force_setup_fcd = shift;
   my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $board_package_path;
   my $generic_error_message = "Unable to set up FCD. Please contact your board vendor or see section \"Linking Your Host Application to the Khronos ICD Loader Library\" of the Programming Guide for instructions on manual setup.\n";

   my ($acl_board_path) = acl::Env::board_path();
   my $mmdlib = acl::Env::board_mmdlib_if_exists();
   if(not defined $mmdlib) {
      die("Error: 'mmdlib' is not defined in $acl_board_path/board_env.xml.\n$generic_error_message");
   }
   #split the paths out based on the comma separator, then insert the acl board path in place of '%b':
   my @lib_paths = split(/,/, $mmdlib);
   my $board_path_indicator = '%b';
   s/$board_path_indicator/$acl_board_path/ for @lib_paths;

   if (acl::Env::is_linux()) {
      #first check if the override is specified by ACL_BOARD_VENDOR_PATH
      my @target_path = (); #stores the path into tokens array
      print "Install the FCD file to $installed_fcd_dir \n";
      # parsing the fcd_file_path and stores the tokens
      @target_path = grep($_, split('/', $installed_fcd_dir));

      # create the target path (if it doesn't exist) and move there:
      chdir "/"; #start at the root
      my $full_dir = ""; #used for giving a sensible error message
      # build the target path.
      # note that the alternative path should be sudo privilege free.
      for my $dir (@target_path) {
         $full_dir = $full_dir . "/" . $dir;
         if(!(-d $dir)) {
            if(mkdir($dir) == 0) {
              # cannot make the $dir, try sudo
              my $cmd = "sudo mkdir  $dir >/dev/null 2>&1";
              system ($cmd) == 0 or die "Unable to create $installed_fcd_dir\n";
            }
         }
         chdir $dir;
      }

      # now print the paths to the .fcd file
      # note that the fcd file to each board should be unique. The fcd file should register
      # all the necessary mmd library (.so) within the bsp, but only for one single bsp.
      # it is not allowed to keep the same mmd libraries from different bsps within a single fcd file.
      # otherwise it will confuse the runtime and return the wrong information of the board
      my ($board_name) = acl::Env::board_name();
      my $fcd_list_file_path = "$board_name.fcd";

      if (-e $fcd_list_file_path) {
        # the file already exists, the user is supposed to inspect/uninstall it, or abort the current installation
        # should not let the user force to rewrite it, because it can be dangerous.
        # case:1409976488 - allow user to force rewrite it until case is resolved.
        #                   If user manually removed a BSP directory, user can't aocl uninstall it after.
        if($force_setup_fcd) {
          unlink $fcd_list_file_path;
          if (-e $fcd_list_file_path) {
            # lack of permission to delete, try sudo rm
            my $cmd = "sudo rm $fcd_list_file_path >/dev/null 2>&1";
            system ($cmd) == 0 or die "Cannot remove FCD file '$fcd_list_file_path': The file does not exist or is not accessible.\n";
          }
        } else {
          print "Error: FCD file for another version of $board_name board package already exists.\n";
          print "       Please uninstall it first via \"aocl uninstall <path to board package>\".\n";
          print "       If the board package files were removed without running aocl uninstall\n";
          print "       first, overwrite the FCD file via \"aocl install -f\".\n";
          print "       For more information, please refer to the description via \"aocl help\".\n";
          die "Abort FCD installation\n";
        }
      }

      # need to lock the file since it can be accessed by multiple processes the sametime
      acl::File::acl_flock($fcd_list_file_path);
      # try open the fcd file and write to it. if not be able to open it, try sudo command
      if (open(my $fcd_fh, '>', $fcd_list_file_path) < 1) {
        # cannot be opened, try sudo
        # don't need to override it first, because if the file exists, it will be failed before.
        foreach my $lib_path (@lib_paths) {
           chomp $lib_path; # remove the trailing newlines
           my $cmd = "echo \"$lib_path\" | sudo tee -a $fcd_list_file_path >/dev/null 2>&1";
           system ($cmd) == 0 or die "Unable to open $fcd_list_file_path\n";
        }
      } else {
        # no need for sudo, write the the file handler
        foreach my $lib_path (@lib_paths) {
           chomp $lib_path; # remove the trailing newlines
           print $fcd_fh $lib_path . "\n";
        }
        # the lock can be freed when handler is closed
        close $fcd_fh;
      }
      # unlock the install_packages file
      acl::File::acl_unflock($fcd_list_file_path);


   } elsif (acl::Env::is_windows()) {
      # if ACL_BOARD_VENDOR_PATH is defined on windows, the fcd will be installed to HKCU. Otherwise it will be installed to HKLM

      #Add the library paths: to be saved
      for my $lib_path (@lib_paths) {

         # Replace forward slashes with back slashes:
         $lib_path =~ s/\//\\/g;

         if($lib_path) {
            # first check the value with type REG_DWORD, which specifies the board name
            # if the board name is same as the board going to be installed, remove it.
            # get the name of the board
            my @board_name_array = split (/\\/, $lib_path);
			      my $find_board = 0;
			      my $board_mmd_name = @board_name_array[-1];

            # populate the bsp values at the board regkey and delete value having the same board mmd name
            my @reg_query_arr = mybacktick("reg query $installed_fcd_regkey /t REG_DWORD");
            my @board_mmd_path_array;
            foreach (@reg_query_arr) {
              if($_ =~ /\bREG_DWORD\b/ and $_ =~ /\b$board_mmd_name\b/) {
                my $board_mmd_path = (split(' ', $_))[0];
				        push(@board_mmd_path_array, $board_mmd_path);
				        system ("reg delete $installed_fcd_regkey /v $board_mmd_path /f>Nul") == 0 or print "Unable to remove registry entry $installed_fcd_regkey\\$lib_path";
              }
            }
            #Adds the registry entry.
            #/v $lib_path to specify a value name (in this case, the name of the value is the path to the dll)
            #/t REG_DWORD to specify the value type
            #/d 0000 for the value data
            #/f to force overwrite in case the value already exists
            system ("reg add $installed_fcd_regkey /v $lib_path /t REG_DWORD /d 0000 /f>Nul") == 0 or die "Unable to edit registry entry\n$generic_error_message";
         }
      }

   } else {
      die "No FCD setup procedure defined for OS '$^O'.\n$generic_error_message";
   }
   # resume the AOCL_BOARD_PACKAGE_ROOT if set
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
   return;
}

sub remove_fcd($) {
   my $board_package_path = shift;
   my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $board_package_path;
   my ($acl_board_path) = acl::Env::board_path();
   my $mmdlib = acl::Env::board_mmdlib_if_exists();
   if(not defined $mmdlib) {
      print "Warning: 'mmdlib' is not defined in $acl_board_path/board_env.xml.\n";
      return;
   }

   #split the paths out based on the comma separator, then insert the acl board path in place of '%b':
   my @lib_paths = split(/,/, $mmdlib);
   my $board_path_indicator = '%b';
   s/$board_path_indicator/$acl_board_path/ for @lib_paths;

   if (acl::Env::is_linux()){
      #The FCD directory
      my $full_dir = $installed_fcd_dir;
      #now remove the .fcd file:
      my ($board_name) = acl::Env::board_name();
      my $fcd_path = "$full_dir/$board_name.fcd";

      unlink $fcd_path;
      if (-e $fcd_path) {
        # lack of permission to delete, try sudo rm
        my $cmd = "sudo rm $fcd_path >/dev/null 2>&1";
        system ($cmd) == 0 or die "Cannot remove FCD file '$fcd_path': The file does not exist or is not accessible.\n";
      }
   } elsif (acl::Env::is_windows()) {

      #Add the library paths:
      foreach my $lib_path (@lib_paths){
         # Replace forward slashes with back slashes:
         $lib_path =~ s/\//\\/g;

         if($lib_path) {
            #Remove the registry entry.
            #/v $lib_path to specify a value name (in this case, the name of the value is the path to the dll)
            #/f to force remove, instead of prompting for [y/n]
            system ("reg delete $installed_fcd_regkey /v $lib_path /f>Nul") == 0 or print "Unable to remove registry entry $installed_fcd_regkey\\$lib_path";
         }
      }
   } else {
      print "No FCD uninstall procedure defined for OS '$^O'.\n";
   }
   # resume the AOCL_BOARD_PACKAGE_ROOT if it's set
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
   return;
}

# Will check to see if fcd is installed. Will prioritize ACL_BOARD_VENDOR_PATH, if it is not specified will check default fcd directory
# caution: this will not check if fcd installed properly, only checks if an fcd file exists. For a more thorough refer to aocl diagnose
sub is_fcd_present {
   if (acl::Env::is_linux()) {
      my $fcd_dir = acl::Env::get_fcd_dir();
      my @fcd_files = acl::File::simple_glob("$fcd_dir/*.fcd");
   if ($#fcd_files >= 0) {
        return 1;
      } else {
        return 0;
      }
   } elsif (acl::Env::is_windows()) {
      my $reg_key = acl::Env::get_fcd_dir();
      my $output = mybacktick("reg query $reg_key 2>&1");
      if ($output !~ "^ERROR") {
        return 1;
      } else {
        return 0;
      }
   } else {
      return 0;
   }
}

# Will check to see if icd is installed. Will prioritize OCL_ICD_VENDORS, if it is not specified will check default icd directory
# caution: this will not check if icd installed properly, only checks if an icd file exists. For a more thorough refer to aocl diagnose
sub is_icd_present {
   if (acl::Env::is_linux()) {
      my $icd_dir = acl::Env::get_icd_dir();
      if (defined $ENV{'OCL_ICD_VENDORS'} and !($ENV{'OCL_ICD_VENDORS'} eq "")){
        $icd_dir = $ENV{'OCL_ICD_VENDORS'};
      }
      my @icd_files = acl::File::simple_glob("$icd_dir/Altera.icd");
      if ($#icd_files >= 0) {
        return 1;
      } else {
        return 0;
      }
   } elsif (acl::Env::is_windows()) {
      my $reg_key = acl::Env::get_icd_dir();
      my $output = mybacktick("reg query $reg_key /v *alteracl_icd.dll 2>&1");
      if ($output !~ "0 match") {
        return 1;
      } else {
        return 0;
      }
   } else {
      return 0;
   }
}


# Add the mmd library to LD_LIBRARY_PATH (linux) or PATH (windows) from the fcd files
sub load_mmd_libs {
  my $mmd_lib_list = shift;
  my @libs = split(',', $mmd_lib_list);
  my $paths = "";
  my $path_delimiter = acl::Env::is_linux() ? ":" : ";";

  foreach my $lib (@libs) {
    $lib =~ s/^\s+|\s+$//g; #trim whitespace from both eneds
    $lib = acl::File::mydirname($lib);
    $paths .= $lib.$path_delimiter;
  }

  if (acl::Env::is_linux()) {
    # On Linux, update LD_LIBRARY_PATH
    $ENV{'LD_LIBRARY_PATH'} = $paths.$ENV{'LD_LIBRARY_PATH'};
  } elsif (acl::Env::is_windows()) {
    # On Windows, update PATH
    $paths = acl::File::file_backslashes($paths);
    $ENV{'PATH'} = $paths.$ENV{'PATH'};
  }
  return;
}

# Read the installed packages storage file on Linux
# Read the installed packages registry on Windows unless ACL_BOARD_VENDOR_PATH is provided
sub populate_installed_packages {
   if (!is_installed_present()) {
     return;
   }

   if (acl::Env::is_linux()) {
      # read all the installed packages
      unless(open FILE, '<'.$installed_bsp_list_file) {
        die "Unable to open $installed_bsp_list_file\n";
      }
      @installed_packages = ();
      my @installed_packages_file = <FILE>;
      chomp(@installed_packages_file);
      close FILE;

      # validate if any of the installed package is not accessible
      # since there is a risk the process can be exited due to permission issue
      # it is worth checking here since all the tools try populate_installed_packages first
      my $index = 0;
      foreach my $bsp (@installed_packages_file) {
        if (opendir (DH, $bsp)) {
          push @installed_packages, $bsp;
          close (DH);
        }
      }
   } elsif (acl::Env::is_windows()) {
        my $output = mybacktick("reg query  $installed_bsp_list_registry /s 2>&1");
        if ($output !~ "^ERROR") {
           @installed_packages =  split /\n/, $output;
           shift @installed_packages;
           shift @installed_packages;
           # Parse the output of reg query
           foreach my $bsp (@installed_packages) {
              $bsp =~ s/^\s+//;
              $bsp = (split /\s+/, $bsp)[0];
           }
        }
    }
}

# Populate $board_boarddir_map: board_name;bsp_path -> board_path
sub populate_boards {
  my %boards = undef;

  my @bsps_to_scan = get_all_board_packages();
  my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
  foreach my $bsp (@bsps_to_scan) {
    $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $bsp;
    %boards = acl::Env::board_hw_list();
    for my $b ( sort keys %boards ) {
      my $boarddir = $boards{$b};
      $board_boarddir_map{"$b;$bsp"} = $boarddir;
    }
  }
  # resume the AOCL_BOARD_PACKAGE_ROOT
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
}

# add the path of each BSP to $board_package_list in the $board_dir
sub populate_board_packages{
  my $board_dir = shift;  # sub-directory to search for the board inside INTELFPGAOCLSDKROOT
  my $board_package_list = shift;

  my @board_root_dirs = ();
  my $sdk_root = acl::Env::sdk_root(); # sdk root can always be set because of the entry_wrapper
  push @board_root_dirs, $sdk_root;
  if ( defined $ENV{'INTEL_FPGA_ROOT'} and !($ENV{'INTEL_FPGA_ROOT'} eq "") ){
    push @board_root_dirs, $ENV{INTEL_FPGA_ROOT};
  }

  foreach my $root_dir (@board_root_dirs) {
    my $board_package_root = File::Spec->catfile($root_dir, $board_dir);

    # go through the sub directories and count the one containing board_env.xml as a board
    opendir(my $dh, $board_package_root) || die "Can't opendir $board_package_root: $!";
    while ( my $entry = readdir $dh ) {
      my $board_package_path = File::Spec->catfile($board_package_root, $entry);
      next unless -d $board_package_path;
      next if $entry eq '.' or $entry eq '..';
      # check if board_env.xml is there
      opendir(my $sub_dh, $board_package_path) || die "Can't opendir $board_package_path: $!";
      my $is_board = 0;
      my $sub_path;
      while ( my $file_entry = readdir $sub_dh and ! $is_board ) {
        $sub_path = File::Spec->catfile($board_package_path, $file_entry);
        next if -d $sub_path;
        if($file_entry eq "board_env.xml") {
          $is_board = 1;

          # If $board_package_path has the pattern: $INTELFPGAOCLSDKROOT/board/[intel_a10gx_pac/intel_s10sx_pac],
          # it is in oneAPI base toolkit and thus a partial (runtime) BSP
          my $canonical_board_package_path = Cwd::abs_path($board_package_path);
          my $oneapi_board_root_pac_a10 = Cwd::abs_path(File::Spec->catfile($sdk_root, 'board', 'intel_a10gx_pac'));
          my $oneapi_board_root_pac_s10 = Cwd::abs_path(File::Spec->catfile($sdk_root, 'board', 'intel_s10sx_pac'));
          if ($canonical_board_package_path =~ /\Q$oneapi_board_root_pac_a10\E/ or 
            $canonical_board_package_path =~ /\Q$oneapi_board_root_pac_s10\E/){
            push @shipped_board_packages_runtime, $board_package_path;
          }else{
            push @shipped_board_packages_develop, $board_package_path;
          }
        }
      }
      close $sub_dh;
    }
    closedir $dh;
  }
  push @{$board_package_list}, @shipped_board_packages_develop;
  push @{$board_package_list}, @shipped_board_packages_runtime;
}

# Get all the possible board packages in the system:
# 1. AOCL_BOARD_PACKAGE_ROOT
# 2. default BSP
# 3. installed BSPs
# 4. shipped BSPs
# 5. not shipped BSPs
sub get_all_board_packages{
  my @bsps = ();

  populate_installed_packages();
  populate_board_packages('board', \@shipped_board_packages);  # in shipped folder

  my @not_shipped_board_packages = ();
  my $not_shipped_dir = File::Spec->catfile('test', 'board');  # include BSP in the not shipped folder if exist
  populate_board_packages($not_shipped_dir, \@not_shipped_board_packages) if (-d (File::Spec->catfile(acl::Env::sdk_root(), $not_shipped_dir)));

  # Only put AOCL_BOARD_PACKAGE_ROOT and default board into @bsps if 
  # they are not in @installed_packages, @shipped_board_packages, nor @not_shipped_board_packages
  my @tmp_bsps = ();
  if ( defined $ENV{'AOCL_BOARD_PACKAGE_ROOT'} and $ENV{'AOCL_BOARD_PACKAGE_ROOT'} ne ""){
    push @tmp_bsps, $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
  }
  my $default_bsp = get_default_board_package();
  push @tmp_bsps, $default_bsp;

  for my $tentative_bsp (@tmp_bsps){
    my $included = 0;
    for my $existing_bsp (@installed_packages, @shipped_board_packages, @not_shipped_board_packages){
      if ( Cwd::abs_path($tentative_bsp) eq Cwd::abs_path($existing_bsp) ){
        $included = 1;
        last;
      }
    }
    push @bsps, $tentative_bsp if !$included;
  }

  push @bsps, @installed_packages;
  if (scalar(@not_shipped_board_packages)) {
    push @bsps, @not_shipped_board_packages;  # never overlaps with shipped since they come from a different path
  }

  # @installed_packages and @shipped_board_packages may overlap.
  # only put !installed & shipped packages into @bsps
  for my $shipped (@shipped_board_packages){
    my $included = 0;
    for my $installed (@installed_packages){
      if ( Cwd::abs_path($shipped) eq Cwd::abs_path($installed) ){
        $included = 1;
        last;
      }
    }
    push @bsps, $shipped if !$included;
  }

  return @bsps;
}

# List the packages
sub list_packages {
  populate_installed_packages();
  populate_board_packages('board', \@shipped_board_packages);

  # get the default BSP
  my $default_bsp = get_default_board_package();

  # print the installed packages
  print "Installed board packages:\n";
  if  ($#installed_packages < 0) {
    # No installed package found. Print out an error message that no package is installed.
    print "  No board support package installed.\n";
  } else {
    foreach (@installed_packages) {
      my $default_str = ($_ eq $default_bsp)? " (default)" : "";
      print "  $_$default_str\n";
    }
  }

  # print the shipped board
  if ( $#shipped_board_packages_develop >= 0){
    print "Board packages shipped with Intel(R) FPGA SDK for OpenCL(TM):\n";
    foreach (@shipped_board_packages_develop) {
      my $default_str = ($_ eq $default_bsp)? " (default)" : "";
      print "  ".acl::File::mybasename($_).": $_$default_str\n";
    }
  }

  if ( $#shipped_board_packages_runtime >= 0){
    print "Board packages shipped with Intel(R) FPGA SDK for OpenCL(TM): (for report generation/emulation/runtime use only) :\n";
    foreach (@shipped_board_packages_runtime) {
      my $default_str = ($_ eq $default_bsp)? " (default)" : "";
      print "  ".acl::File::mybasename($_).": $_$default_str\n";
    }
  }
}

# List boards
# If -board-package is provided by the user, listing the boards within the board package specified
# If not, either list the boards within the default board package (a10_ref) if installed
#         or     print warning message that there are no board packages installed
sub list_boards {
  my $bsp_ref = shift;
  my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
  my $use_default_bsp = 0;
  if (! defined $bsp_ref or $bsp_ref eq "") {
    # user doesn't specify the bsp, try to populate installed board; if no board installed, using the default one
    populate_boards();
    # unset AOCL_BOARD_PACKAGE_ROOT since there could be multiple board package installed
    # So that acl::Env::board_hardware_default() could return the default board from the default bsp
    $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = undef;
    $use_default_bsp = 1;
  } else {
    # using the user specified bsp and populate
    my %boards = ();
    $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $bsp_ref;
    %boards = acl::Env::board_hw_list();
    for my $b ( sort keys %boards ) {
      my $boarddir = $boards{$b};
      $board_boarddir_map{"$b;$bsp_ref"} = $boarddir;
    }
  }
  # resume the AOCL_BOARD_PACKAGE_ROOT
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;

  # get the BSP to use and the default board from that BSP
  my $bsp_to_use = ($use_default_bsp)? get_default_board_package() : $bsp_ref;
  my $default_board = get_default_board($bsp_to_use);

  print "Board list:\n";

  if( keys( %board_boarddir_map ) == -1 ) {
    print "  none found\n";
  } else {
      for my $b ( sort keys %board_boarddir_map ) {
      my $boarddir = $board_boarddir_map{$b};
      my ($name,$bsp) = split(';',$b);
      my $default_str= ($bsp eq $bsp_to_use and $name eq $default_board)? " (default)" : "";
      print "  $name$default_str\n";
      print "     Board Package: $bsp\n";
      if ( ::acl::Env::aocl_boardspec( $boarddir, "numglobalmems") > 1 ) {
        my $gmemnames = ::acl::Env::aocl_boardspec( $boarddir, "globalmemnames");
        print "     Memories:      $gmemnames\n";
      }
      my $channames = ::acl::Env::aocl_boardspec( $boarddir, "channelnames");
      if ( length $channames > 0 ) {
        print "     Channels:      $channames\n";
      }
      print "\n";
    }
  }
  return;
}

# Query the default board package
# if no package installed, return acl::Env::board_path()
# if one package installed, return that specific one
# if multiple package installed, return the most recent( the last) one
sub get_default_board_package() {
  populate_installed_packages();

  if (scalar @installed_packages == 0) {
    my ($default_bsp) = acl::Env::board_path();
    return $default_bsp;
  } else {
    # because the bsps are pushed to installed_packages array one by one from the installed_packages file
    # the last bsp will be the most recent installed one
    return $installed_packages[-1];
  }
}

# Query the default board from a board package
# the default board should be registered in the board_env.xml
sub get_default_board($) {
  my $selected_board_package_path = shift;
  die "Cannot find the selected board package: $selected_board_package_path\n" if (not -d $selected_board_package_path);
  my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $selected_board_package_path;
  my ($default_board) = acl::Env::board_hardware_default();
  # resume the env var AOCL_BOARD_PACKAGE_ROOT if needed
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
  die "Cannot find default board\n" if (not $default_board);
  return $default_board;
}

# Check if a given board is in the given bsp
sub is_board_in_the_bsp($$) {
  my ($given_board, $given_bsp) = @_;
  my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} =  $given_bsp;
  my %boards_in_bsp = acl::Env::board_hw_list();
  my $has_board = 0;
  for my $b ( sort keys %boards_in_bsp ) {
    if($given_board eq $b) {
      $has_board = 1;
      last;
    }
  }
  # resume the env var AOCL_BOARD_PACKAGE_ROOT if needed
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
  return $has_board;
}

# Extract bsp and board value from "-board=..."
# Expecting a string in the format of <bsp name or path>:<board name>
# It can be in the following format:
#  1. <bsp name or path>:
#  2. <board name>
#  3. <bsp name or path>:<board name>
# <bsp path> can contain up to 1 colon (for Windows).
# <bsp name> and <board name> can only contain alphanumeric and -
sub parse_board_arg{
  my ($bsp_colon_board) = @_;

  my $bsp = undef;
  my $board = undef;
  my $defined_bsp = 0;
  my $defined_board = 0;

  if( $bsp_colon_board =~ m{(^[/\\:\w\.\+-]+):([\w-]*)$} ){
    # Input case 1 or 3
    $bsp = get_board_package_path($1);
    $defined_bsp = 1;
    if ( !defined $bsp ){
      mydie('Option "-board=..." argument value is in the format of <board package name or path>:<board name>,
        but <board package> is not a valid name or path');
    }
    if ( $2 ne "" ){
      $board = $2;
      $defined_board = 1;
    }
  }elsif ( $bsp_colon_board =~ m{^[\w-]+$}){
    # Input case 2
    # BSP will be figured out later
    $board = $bsp_colon_board;
    $defined_board = 1;
  }else{
    mydie('Invalid value "'.$bsp_colon_board.'". Please make sure a valid board package name or path or a valid board bariant has been specified.');
  }

  return ($bsp, $board, $defined_bsp, $defined_board);
}

sub get_board_package_path{
  my ($bsp_path_or_name) = @_;
  if (-e $bsp_path_or_name and -d $bsp_path_or_name){
    # It is a path
    return $bsp_path_or_name;
  }elsif ( $bsp_path_or_name =~ /^[\w-]+$/ ){
    # It is a name
    return get_bsppath_by_bspname($bsp_path_or_name);
  }else{
    mydie('Invalid value "'.$bsp_path_or_name.'". Please specify a valid board package name or path');
  }
}

sub get_bsppath_by_bspname{
  my ($bsp_name) = @_;

  my @bsps_to_scan = get_all_board_packages();
  foreach (@bsps_to_scan){
    if ($bsp_name eq acl::File::mybasename($_)){
      return $_;
    }
  }

  # No BSP path found for the provided name
  mydie('Could not find its path for the provided board package name: "'.$bsp_name.'". Please make sure the board package is under $INTEL_FPGA_ROOT/board or $INTELFPGAOCLSDKROOT/board, or specify the full path for the board package');
}

# Look for a BSP that includes the specified board
sub get_bsppath_by_boardname{
  my ($board, $is_sycl_mode) = @_;
  my $bsp = undef; 

  my @found_pkgs = ();
  my @found_pkgs_canonical = (); # only used for comparing canonicalized paths
  my @bsps_to_scan = get_all_board_packages();
  foreach my $pkg (@bsps_to_scan) {
    # Skip duplicates (e.g. shipped packages that are also installed).
    my $canonical_pkg = Cwd::abs_path($pkg);
    next if grep(/^\Q$canonical_pkg\E$/, @found_pkgs_canonical);
    if (is_board_in_the_bsp($board, $pkg)) {
      $bsp = $pkg;
      push @found_pkgs, $pkg;
      push @found_pkgs_canonical, $canonical_pkg;
    }
  }

  my $overlapping_board = (scalar @found_pkgs > 1)?1:0;
  # In SYCL mode, we allow a board to be included in two BSPs: 
  #  one in INTELFPGAOCLSDKROOT/board: partial BSP shipped with oneAPI base toolkit
  #  the other in INTEL_FPGA_ROOT/board: full BSP shipped with FPGA add-on
  if ( $overlapping_board and $is_sycl_mode and (scalar @found_pkgs == 2) and 
    defined $ENV{'INTELFPGAOCLSDKROOT'} and $ENV{'INTELFPGAOCLSDKROOT'} ne "" and 
    defined $ENV{'INTEL_FPGA_ROOT'} and $ENV{'INTEL_FPGA_ROOT'} ne "" ){
    my $sdk_board_root = Cwd::abs_path(File::Spec->catfile($ENV{'INTELFPGAOCLSDKROOT'}, 'board'));
    my $fpga_addon_board_root = Cwd::abs_path(File::Spec->catfile($ENV{'INTEL_FPGA_ROOT'}, 'board'));
    my $in_base_toolkit = 0;
    my $in_fpga_addon = 0;
    my $fpga_addon_bsp;
    foreach my $pkg (@found_pkgs){
      if ( Cwd::abs_path($pkg) =~ /\Q$sdk_board_root\E/ ){
        $in_base_toolkit = 1;
      }elsif ( Cwd::abs_path($pkg) =~ /\Q$fpga_addon_board_root\E/){
        $in_fpga_addon = 1;
        $fpga_addon_bsp = $pkg;
      }
    }
    if ($in_base_toolkit and $in_fpga_addon){
      $overlapping_board = 0;
      $bsp = $fpga_addon_bsp; # Make sure the BSP in FPGA add-on is used.
    }
  }

  my $option_str = $is_sycl_mode? "-board=<board package name or path>:<board>" : "-board-package=<board package name or path>";
  my $overlapping_board_err_str = "Multiple board packages found that define board '$board'.\nPlease".
                                  " use the option $option_str to specify".
                                  " the\npackage you would like to use.\nThe following packages define ".
                                  "'$board':\n\t".join("\n\t", @found_pkgs);
  mydie($overlapping_board_err_str) if $overlapping_board;
  my $no_bsp_err_for_board_str = "Cannot find the board package for board variant '$board'.\n".
                                  "Please make sure the option -board points to a valid board variant \n".
                                  "within the available board packages.\n".
                                  "You can use the option -list-board-packages to find out the available\n".
                                  "board packages. Refer to \"aoc -help\" for more information.\n";
  mydie($no_bsp_err_for_board_str) if (scalar @found_pkgs == 0);

  return $bsp;
}

# system utilities:
# set and get original directory
# set package file and source packge file names
# set and get verbose, quiet mode and save temps
# mydie
# move_to_log
sub set_original_dir($) {
  $orig_dir = shift;
  return $orig_dir;
}

sub get_original_dir {
  return $orig_dir;
}

sub set_package_file_name($) {
  $pkg_file = shift;
  return $pkg_file;
}

sub set_source_package_file_name($) {
  $src_pkg_file = shift;
  return $src_pkg_file;
}

sub set_verbose($) {
  $verbose = shift;
}

sub get_verbose {
  return $verbose;
}

sub set_quiet_mode($) {
  $quiet_mode = shift;
}

sub get_quiet_mode {
  return $quiet_mode;
}

sub set_save_temps($) {
  $save_temps = shift;
}

sub get_save_temps {
  return $save_temps;
}

sub mydie(@) {
  # consider updating this API to pass in $pkg_file and $src_pkg_file instead of keeping a global variable here
  print STDERR "Error: ".join("\n",@_)."\n";
  backup_dirs(); # backup the dirs/files
  chdir $orig_dir if defined $orig_dir;
  unlink $pkg_file;
  unlink $src_pkg_file;
  exit 1;
}

sub move_to_log { #string, filename ..., logfile
  my $string = shift @_;
  my $logfile= pop @_;
  open(LOG, ">>$logfile") or mydie("Couldn't open $logfile for appending.");
  print LOG $string."\n" if ($string && ($verbose > 1 || $save_temps));
  foreach my $infile (@_) {
    open(TMP, "<$infile") or mydie("Couldn't open $infile for reading.");;
    while(my $l = <TMP>) {
      print LOG $l;
    }
    close TMP;
    unlink $infile;
  }
  close LOG;
}

# Add quotes to SDK executables on Windows if they have been installed to a dir
# that has spaces in it.
sub autoquote_exe($) {
  my $cmd = shift @_;
  my $sdk_root = acl::Env::sdk_root();
  if (acl::Env::is_windows() and ($cmd =~ m/^(\Q$sdk_root\E[^\s]*)(.*)/)) {
    my $exe  = $1;
    my $args = $2;
    if ($exe =~ m/\s/) {
      $cmd = "\"$exe\"$args";
    }
  }
  return $cmd;
}

# Functions to execute external commands, with various wrapper capabilities:
#   1. Logging
#   2. Time measurement
# Arguments:
#   @_[0] = {
#       'stdout' => 'filename',   # optional
#       'stderr' => 'filename',   # optional
#       'time' => 0|1,            # optional
#       'time-label' => 'string'  # optional
#     }
#   @_[1..$#@_] = arguments of command to execute
sub mysystem_full($@) {
  my $opts = shift(@_);
  my @cmd = @_;

  my $out = $opts->{'stdout'};
  my $err = $opts->{'stderr'};

  # We may need to add quotes around the executable.
  $cmd[0] = autoquote_exe($cmd[0]);

  if ($verbose >= 2) {
    print join(' ',@cmd)."\n";
  }

  # Replace STDOUT/STDERR as requested.
  # Save the original handles.
  if($out) {
    open(OLD_STDOUT, ">&STDOUT") or mydie "Couldn't open STDOUT: $!";
    open(STDOUT, ">$out") or mydie "Couldn't redirect STDOUT to $out: $!";
    $| = 1;
  }
  if($err) {
    open(OLD_STDERR, ">&STDERR") or mydie "Couldn't open STDERR: $!";
    open(STDERR, ">$err") or mydie "Couldn't redirect STDERR to $err: $!";
    select(STDERR);
    $| = 1;
    select(STDOUT);
  }

  # Run the command.
  my $start_time = time();
  system(@cmd);
  my $end_time = time();

  # Restore STDOUT/STDERR if they were replaced.
  if($out) {
    close(STDOUT) or mydie "Couldn't close STDOUT: $!";
    open(STDOUT, ">&OLD_STDOUT") or mydie "Couldn't reopen STDOUT: $!";
  }
  if($err) {
    close(STDERR) or mydie "Couldn't close STDERR: $!";
    open(STDERR, ">&OLD_STDERR") or mydie "Couldn't reopen STDERR: $!";
  }

  # Dump out time taken if we're tracking time.
  if ($time_log_fh && $opts->{'time'}) {
  # if ($time_log_filename && $opts->{'time'}) {
    my $time_label = $opts->{'time-label'};
    if (!$time_label) {
      # Just use the command as the label.
      $time_label = join(' ',@cmd);
    }

    log_time ($time_label, $end_time - $start_time);
  }
  return $?
}

# On Windows add quotes around SDK executables that have whitespace in their
# path before running the command.
sub mybacktick($) {
  my $cmd = autoquote_exe(shift(@_));
  return `$cmd`;
}

# time log utilties
sub open_time_log($$) {
  my $time_log_filename = shift;
  my $run_quartus = shift;

    my $fh;
    if ($time_log_filename ne "-") {
      # If this is an initial run, clobber time_log_filename, otherwise append to it.
      if (not $run_quartus) {
        open ($fh, '>', $time_log_filename) or mydie ("Couldn't open $time_log_filename for time output.");
      } else {
        open ($fh, '>>', $time_log_filename) or mydie ("Couldn't open $time_log_filename for time output.");
      }
    }
    else {
      # Use STDOUT.
      open ($fh, '>&', \*STDOUT) or mydie ("Couldn't open stdout for time output.");
    }

    # From this point forward, $time_log_fh holds the file handle!
    $time_log_fh = $fh;
}

sub close_time_log {
  if ($time_log_fh) {
    close ($time_log_fh);
  }
}

sub write_time_log($) {
  my $s = shift;
  print $time_log_fh $s;
}

sub log_time($$) {
  my ($label, $time) = @_;
  if ($time_log_fh) {
    printf ($time_log_fh "[time] %s ran in %ds\n", $label, $time);
  }
}

1;
