=pod

=head1 NAME

acl::Command - Utility commands for the Intel(R) FPGA SDK for OpenCL(TM)

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

      BEGIN { 
         unshift @INC,
            (grep { -d $_ }
               (map { $ENV{"INTELFPGAOCLSDKROOT"}.$_ }
                  qw(
                     /host/windows64/bin/perl/lib/MSWin32-x64-multi-thread
                     /host/windows64/bin/perl/lib
                     /share/lib/perl
                     /share/lib/perl/5.8.8 ) ) );
      };


package acl::Command;
require Exporter;
@acl::Command::ISA        = qw(Exporter);
@acl::Command::EXPORT     = ();
@acl::Command::EXPORT_OK  = qw();
use strict;
use acl::Env;
use acl::Board_env;
use acl::Pkg;
use acl::Common;
use acl::AOCDriverCommon;
use acl::Report;
our $AUTOLOAD;

my @_valid_cmd_list = qw(
   version
   help
   do
   compile-config
   cflags
   link-config
   linkflags
   ldflags
   ldlibs
   board-path
   board-hw-path
   board-mmdlib
   board-libs
   board-link-flags
   board-default
   board-version
   board-name
   board-xml-test
   reprogram
   program
   flash
   diagnostic
   diagnose
   setup-fcd
   install
   uninstall
   list-devices
   example-makefile
   makefile
   binedit
   hash
   report
   htmlreport
   library
   xlibrary
   env
   profile
   initialize
);

my @_valid_list = @_valid_cmd_list, qw( pre_args args cmd prog );

my %_valid_cmd = map { ($_ , 1) } @_valid_cmd_list;

my %_valid = map { ($_ , 1) } @_valid_list;

# the 2*ith element is the physical device name, the 2*i+1th element is the board package path associated with the board
my @device_map_multiple_packages = ();
my @packages_without_devices = ();

my $acl_root = acl::Env::sdk_root();

sub populate_attached_devices {
  my ($self,@args) = @_;
  acl::Common::populate_installed_packages();
  # backward compatiblily, if no packages are saved, try AOCL_BOARD_PACKAGE_ROOT
  if ($#installed_packages < 0) {
    push @installed_packages, acl::Board_env::get_board_path();
  }

  # populate attached devices for each board package installed
  for (my $i=0; $i<scalar(@installed_packages); $i++) {
    my $board_package_path = @installed_packages[$i];
    my $num_of_devices = 0;
    $ENV{"AOCL_BOARD_PACKAGE_ROOT"} = $board_package_path;
    if ( acl::Board_env::get_board_version() < 15.1 ) {
      push(@packages_without_devices,$board_package_path);
      next;
    }
    my $mmd_lib = acl::Board_env::get_mmdlib_if_exists();
    acl::Common::load_mmd_libs($mmd_lib); # add the mmd library to either LD_LIBRARY_PATH or PATH
    my $utilbin = acl::Board_env::get_util_bin();
    my $util = ( acl::Board_env::get_board_version() < 14.1 ) ? "diagnostic" : "diagnose";
    check_board_utility_env($self) or return undef;
    my $probe_output = `$utilbin/$util -probe`;
    my @lines = split('\n', $probe_output);
    foreach my $line (@lines) {
      if ( $line =~ /^DIAGNOSTIC_/ ) {
        next;
      } else {
        #windows always populate 32 devices, this is the workaround
        my $check = `$utilbin/$util -probe $line`;
        if ($check =~ "DIAGNOSTIC_PASSED") {
          push(@device_map_multiple_packages,$line);
          push(@device_map_multiple_packages,$board_package_path);
          $num_of_devices += 1;
        }
      }
    }
    if ($num_of_devices == 0) {
      push(@packages_without_devices,$board_package_path);
    }
  }
}

sub new {
   my ($proto,$prog,@args) = @_;
   my $class = ref $proto || $proto;

   my @pre_args = ();
   my @post_args = ();
   my $subcommand = undef;
   my $first_arg = undef;
   while ( $#args >=0 ) {
      my $arg = shift @args;
      $first_arg = $arg unless defined $first_arg;
      if ( $arg =~ m/^[a-z]/ ) {
         $subcommand = $arg;
         last;
      } else {
         push @pre_args, $arg;
      }
   }
   if ( $_valid_cmd{$subcommand} ) {
      $subcommand =~ s/-/_/g;
      return bless {
         prog => $prog,
         pre_args => [ @pre_args ],
         cmd => $subcommand,
         args => [ @args ]
         }, $class;
   } else {
      if ( defined $first_arg ) {
         $subcommand = $first_arg unless defined $subcommand;
         $subcommand = '' unless defined $subcommand;
         print STDERR "$prog: Unknown subcommand '$subcommand'\n";
      }
      return undef;
   }
}

sub do {
   my $self = shift;
   # Using "exec" would be more natural, but it doesn't work as expected on Windows.
   # http://www.perlmonks.org/?node_id=724701
   if ( $^O =~ m/Win32/ ) {
      system(@{$self->args});
      # Need to post-process $? because it's a mix of status bits, and
      # it seems Windows only allows "main" to return up to 8b its.
      # The $? bottom 8 bits encodes signals, the upper bits encode return status.
      # So $? for the "false" program (returns 1), is actually 256, and then if we exit 256
      # then it's translated back into 0 by Windows. Thus making "false" look like it succeeded!
      my $raw_status = $?;
      # Fold in the signal error into our 8 bit error range.
      my $processed_status = ($raw_status>>8) | ($raw_status&255);
      exit $processed_status;
   } else {
      exec(@{$self->args});
      # exec() returns only if a command is not found, so the error message below doesn't need a condition
      print STDERR "aocl do: Cannot execute '@{$self->args}'\n";
   }
}


sub run {
   my $self = shift;
   my $cmd = $self->cmd;
   my @args = @{$self->args};
   my $result = eval "\$self->$cmd(\@args)";
   if ($@) {
      print $@; #don't supress errors.
      return 0;
   } else {
      return $result;
   }
}

sub env {
   my ($self,@args) = @_;
   if ( $#args == 0 && ($args[0] =~ m/.aocx$/i || $args[0] =~ m/.aoco$/i ) ) {
      my $result = $self->binedit($args[0],'print','.acl.compilation_env');
      print "\n";  # "binedit print" does not append the newline
      return $result;
   }
   print STDERR $self->prog." env: Unrecognized options: @args\nAn input .aocx file is needed.\n";
   return undef;
}

sub version {
   my ($self,@args) = @_;
   if ( $#args < 0 ) {
      my $banner = acl::Env::is_sdk() ? 'Intel(R) FPGA SDK for OpenCL(TM), Version 20.3.0 Build 168.5 Pro Edition, Copyright (c) 2020 Intel Corporation' : 'Intel(R) FPGA Runtime Environment for OpenCL(TM), Version 20.3.0 Build 168.5 Pro Edition, Copyright (c) 2020 Intel Corporation';
      print $self->prog." 20.3.0.168.5 ($banner)\n";
   } else {
      if ( $#args == 0 && $args[0] =~ m/.aocx$/i ) {
         my $result = $self->binedit($args[0],'print','.acl.version');
         print "\n";  # "binedit print" does not append the newline
         return $result;
      }
      print STDERR $self->prog." version: Unrecognized options: @args\n";
      return undef;
   }
   return $self;
}

sub report {
   print STDERR "aocl Error: The Intel FPGA Dynamic Profiler for OpenCL GUI has been deprecated. Use the Intel VTune Profiler to view FPGA profiling data.\n";
   return undef;
}


sub profile_gpp_and_move_files {
   my $no_json = shift;
   my $no_run = shift;
   my $aocx_dir = shift;
   my $mon_dir = shift;
   my $output_dir = shift;
   my $mon_name = shift;

   my $output_filename;

   if (!$output_dir) {
      $output_filename = "profile.json";
   } else {
      if (!acl::File::make_path($output_dir)) {
        print STDERR "aocl Error: cannot create the directory $output_dir.";
        exit 1;
      }
      $output_filename = "$output_dir/profile.json";
   }

   if ($no_json == 0){
      print "Starting post-processing of the profiler data...\n";
      # run GPP library - should output a profile.json file
      system(acl::Env::sdk_host_bin_dir().acl::Env::pathsep()."aocl-profile-gpp $aocx_dir $mon_dir $output_filename");
      
      my $raw_status = $?;
      if ($raw_status != 0 or (!open( IN,"<$output_filename"))) {
         print STDERR "aocl Error: the gpp library failed to create a profile.json file\n";
         exit 1;
      }

      # if VTune is set-up, want to move the .json file to the directory they need
      my $vtune_input_location = $ENV{AMPLXE_DATA_DIR};
      if (defined $ENV{AMPLXE_DATA_DIR}){
         acl::File::copy($output_filename, "$vtune_input_location/profile.json");
      }
      print "Post-processing complete.\n";
   }

   # if the user set up an output directory, move the profile.mon files there
   # Note: profile.json doesn't need to be moved as it is created by default in the provided output_dir or the current dir if output_dir is not set
   if ($output_dir){
     if ($no_run == 0 && $mon_name){
       acl::File::copy($mon_dir, "$output_dir/$mon_name");
       acl::File::remove_tree($mon_name); # not using mon_dir in small case where mon_dir is a dir and not a file
     } elsif ($no_run == 0){
       # Users should never get this since when no-run isn't set, either profile.mon or profile_shared.mon is given
       print STDERR "aocl Error: cannot move .mon file - name of .mon file not given.\n";
       exit 1;
     }
   }

   return;
}


sub profile_find_file_path {
   my $extension = shift;
   my $flag_name = shift;
   my $fat_binary = shift; # if no aocx given, need to check if executable is a fat binary with aocx inside
   my $executable_path = shift; # this is needed if the user passed in the fat binary with -e instead of passing it as an executable
   my $given_path = shift;

   if (!$given_path) {
      # first check if the executable is actually a fat binary that has the aocx in it
      if ($extension =~ "aocx" && $fat_binary){
         my $extract_executable = acl::Env::sdk_host_bin_dir().acl::Env::pathsep()."aocl-extract-aocx";
         # currently extract_output is unused (it is just used to hide any errors or warnings from the script) - may be useful later.
         my $extract_output = `$extract_executable -i $fat_binary -o temp.aocx 2>1`;
         if ($? == 0) {
            return ('1', "./temp.aocx");
         }
         if (!($fat_binary eq $executable_path)){
           # currently extract_output is unused (it is just used to hide any errors or warnings from the script) - may be useful later.
           my $extract_output = `$extract_executable -i $executable_path -o temp.aocx 2>1`;
           if ($? == 0) {
              return ('1', "./temp.aocx");
           }
         }
      }

      my @files = acl::File::simple_glob("*".$extension);
      if ($#files > 0){
         print STDERR "aocl Error: There are multiple $extension files in current directory, unable to determine which to use. \n     Please specify which $extension should be used with the '$flag_name' flag.\n";
         exit 1;
      } elsif ($#files < 0) {
         print STDERR "aocl Error: No .aocx file was found in current directory. \n     If you are running using the oneAPI Data Parallel C++ Compiler, ensure have compiled for FPGA hardware\n     and that you are passing in the compiled executable (directly or with -e). \n     Otherwise please specify where the .aocx file is with the '-x' flag. \n";
         exit 1;
      } else {
         my $file = $files[0];
         print "aocl Warning: No path was given for the $extension file, so using the file $file found in the current directory. \n";
         return (0, $file);
      }
   } elsif (!open( IN,"<$given_path")) {
      print STDERR "aocl Error: The path given for the $extension file was not valid. \n      $given_path cannot be opened. \n";
      exit 1;
   } else {
      return (0, $given_path);
   }
}

sub profile {
   my ($self,@args) = @_;

   my $print_help = 0;
   my $no_run = 0;
   my $no_json = 0;
   my $path_to_mon;
   my $executable;
   my $period_num = 1;
   my $mem_transf_on = 1;
   my $output_dir;
   my $executable_path; # note that sometimes what we call "executable" is actually a script that contains an executable run in it
   my $aocx_path;
   my $remove_temp_aocx = 0;
   my $shared_counters_on = 0;
   my @executable_args;

   my $default_mon_name = "profile.mon";
   my $default_shared_mon_name = "profile_shared.mon";

   # parse the arguments
   if (!@args) { $print_help = 1; }
   while ( @args ) {
      my $arg = shift @args;
      if (($arg eq '-h') or ($arg eq '-help') or ($arg eq '--help')) { $print_help = 1; last; }
      elsif (($arg eq '-no-run') or ($arg eq '--no-run')) { $no_run = 1; $path_to_mon = (shift @args); }
      elsif (($arg eq '-period') or ($arg eq '--period')) {
         if ($no_run) { print $self->prog." Warning: Cannot adjust period between data collection if not doing a run \n"; shift @args; }
         elsif ($period_num == 0) { print $self->prog." Warning: Temporal counters were turned off, so the -period option will be ignored \n"; shift @args; }
         else {
            $period_num = (shift @args);
            if (!($period_num =~ /^[0-9]+$/)){
               print $self->prog." Warning: value after -period option is $period_num which is not a whole positive integer. The '-period' option will be ignored \n";
               $period_num == 0
            }
         }
      }
      elsif (($arg eq '-no-temporal') or ($arg eq '--no-temporal')) {
         if ($no_run) { print $self->prog." Warning: Cannot turn off temporal counters if not doing a run \n"; }
         elsif ($period_num != 1) { print $self->prog." Warning: Temporal counters were turned off, so the -period option will be ignored \n"; $period_num = 0; }
         else { $period_num = 0; }
      }
      elsif (($arg eq '-no-mem-transfers') or ($arg eq '--no-mem-transfers')) {
         if ($no_run) { print $self->prog." Warning: Cannot turn off memory transfers if not doing a run \n"; }
         else { $mem_transf_on = 0; }
      }
      elsif (($arg eq '-no-json') or ($arg eq '--no-json')) {
         $no_json = 1;
      }
      elsif (($arg eq '-output-dir') or ($arg eq '--output-dir')) { $output_dir = (shift @args); }
      # note that the executable path will be the path to the actual binary, while the executable may be some other script
      elsif (($arg eq '-e') or ($arg eq '--executable')) {
         if ($executable_path) {
            print STDERR $self->prog." Error: Multiple executables passed in with -e not supported. Rerun with one executable. \n";
            exit 1;
         } else { $executable_path = (shift @args); }
      }
      elsif (($arg eq '-x') or ($arg eq '--aocx')) {
         if ($aocx_path) {
            print STDERR $self->prog." Error: Having multiple .aocx files is not currently supported. Only include the aocx file for the design you are running on hardware \n";
            exit 1;
         } else { $aocx_path = (shift @args); }
      }
      elsif (($arg eq '-sc') or ($arg eq '--shared-counters') or ($arg eq '-shared-counters')) {
         $shared_counters_on = 1;
      }
      # this code will assume the first thing that it sees that doesn't look like any other args is the executable
      elsif (!$executable) {
         if (!($arg =~ /^-/)){
            $executable = $arg;
            my $executable_arg = shift @args;
            while (defined $executable_arg){
               push(@executable_args, $executable_arg);
               $executable_arg = shift @args;
            }
         } else {
            print $self->prog." Error: the argument '$arg' started with a '-', yet was not one of the accepted 'aocl profile' options. \n";
            print "     For a list of accepted options, please run 'aocl profile -h'. \n";
            print "     If this is an executable, please rename it to not start with '-'. \n";
            exit 1;
         }
      }
   }

   if ($print_help){
      print "   aocl profile can be used to collect information about your host run if you compiled with -profile. \n";
      print "   To use it, please specify your host executable as follows: 'aocl profile path/to/executable'. \n\n";
      print "   If you are compiling with the oneAPI Data Parallel C++ Compiler and do not wish to pass in the \n";
      print "   compiled executable binary directly (but rather a script that calls it), the binary needs to be \n";
      print "   passed in using '--executable' or '-e'. \n\n";
      print "   It is also optional (but recommended) that you include the location of the *.aocx file \n";
      print "   using '--aocx' or '-x'. Note that this file is not needed when compiling with the \n";
      print "   oneAPI Data Parallel C++ Compiler. If no files are given, any aocx found \n";
      print "   in the current directory will be used (potentially incorrectly) \n\n";
      print "   OpenCL use case example: aocl profile -x kernel.aocx bin/host -host-arg1 -host-arg2 \n\n";
      print "   oneAPI use case example: aocl profile -e kernel.fpga executable_calling_kernel_fpga.sh -executable-arg1 \n\n";
      print "   You can also specify a few other options (after the executable if relevant): \n";
      print "     - Adjust the period between counter readbacks by specifying the number of clock cycles between subsequent\n";
      print "       readbacks with '-period ###': 'aocl profile path/to/executable -period 10000' \n";
      print "       If this flag is not passed in, the counters will be read back as often as possible. \n";
      print "     - Change counters to only read when the kernel finishes (not while it's running) with -no-temporal \n";
      print "     - Turn off memory transfer information with -no-mem-transfers \n";
      print "     - Turn on shared counter data (use when design compiled with '-profile-shared-counters' option) \n";
      print "     - Change the output directory where the .mon and .json file will be placed with '-output-dir /path/to/new/loc/' \n";
      print "     - Skip the actual compile and just point to a profile.mon file with '-no-run /path/to/profile.mon' \n";
      print "       Do this if you already have data, but want it in a format that VTune can display. \n";
      print "     - Do not create a profile.json file by setting the '-no-json' flag (no need for .aocx or .source files) \n";
      print "       Do this if you do not wish to visualize the profiler data with VTune, and want the profile.mon output\n\n";
      print "   Please ensure that the executable and its options are the last arguments. \n";
      exit 0;
   }

   if (!$no_json) {
      # for aocx, want to check if executable is part of a fat binary
      if (not defined $executable_path) {
        $executable_path = $executable;
      }
      ($remove_temp_aocx, $aocx_path) = profile_find_file_path(".aocx", "-x", $executable, $executable_path, $aocx_path);
   }

   if ($no_run){
      # If the user set the run off and the json off, just return after printing a warning
      if ($no_json){
         print $self->prog." Warning: If both the compile and the json outputting functionality is turned off, aocl profile does nothing \n";
         if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
         exit 0;
      }
      if ((!$path_to_mon) or ((!open( IN,"<",$path_to_mon)) and (!opendir( DIR,$path_to_mon)))){
         print STDERR $self->prog." Error: Could not find the path to the .mon file specified - ensure it is correct\n";
         if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
         exit 1;
      } else {
         # put a dot at the end if there is a trailing slash so that will look *inside* that directory for the profile.mon
         if( $path_to_mon =~ m/\/$/ or $path_to_mon =~ m/\\$/){ $path_to_mon .= '.'; }

         # if this is a directory, check if the mon file is inside it
         if ($path_to_mon =~ m/\.$/){
            my $dirname = acl::File::mydirname($path_to_mon);
            my $dir_w_mon = $dirname.$default_mon_name;
            my $dir_w_mon_shared = $dirname.$default_shared_mon_name;
            if (!-e $dir_w_mon && !-e $dir_w_mon_shared){
               if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
               print STDERR $self->prog." Error: Could not find a $default_mon_name file in the directory specified by '-no-run'. \n";
               exit 1;
            } elsif (!-e $dir_w_mon) {
               $path_to_mon = $dir_w_mon_shared;
               if (!$shared_counters_on) {
                 print $self->prog." Warning: Unable to find $default_mon_name but found $default_shared_mon_name. Include shared counter flag (-sc) or provide profile.mon to remove this warning.\n";
               }
            } elsif (!-e $dir_w_mon_shared) {
               $path_to_mon = $dir_w_mon;
               if ($shared_counters_on) {
                 print $self->prog." Warning: Unable to find $default_shared_mon_name but found $default_mon_name. Remove shared counter flag (-sc) or provide profile_shared.mon to remove this warning.\n";
               }
            } else { # Both files exist, choose which one depending on if shared_counters is turned on/off
              if ($shared_counters_on) {
                $path_to_mon = $dir_w_mon_shared;
              } else {
                $path_to_mon = $dir_w_mon;
              }
            }
         } elsif (!($path_to_mon =~ m/\.mon$/)) {
            my $basename = acl::File::mybasename($path_to_mon);
            if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
            # error out now because the gpp will not accept any mon file not ending with a .mon extension
            print $self->prog." Error: You are pointing to a file named '$basename' that does not have '.mon' extension - ensure you are pointing to the correct file\n";
            exit 1;
         }
         profile_gpp_and_move_files($no_json, $no_run, $aocx_path, $path_to_mon, $output_dir);
         if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
         exit 0;
      }
   } else {
      #set up the environment
      $ENV{ACL_RUNTIME_PROFILING_ACTIVE}='1';
      $ENV{ACL_PROFILE_START_CYCLE}= $period_num;
      if ($mem_transf_on) { $ENV{ACL_PROFILE_TIMER}='1'; }

      # add ./ at the start of the executable in case the user passed in just the executable without the full path
      my $exec_basename = acl::File::mybasename($executable);
      my $exec_dirname = acl::File::mydirname($executable);
      $executable = "$exec_dirname/$exec_basename";

      my $shared_counter_iterations = $shared_counters_on ? 3 : 0; # run once if no shared counters, 4 times if have shared counters
      for my $iter (0..$shared_counter_iterations){
         if ($shared_counters_on) {
           $ENV{ACL_INTERAL_SHARED_CONTROL_SETTING} = $iter;    # shared counter control setting
         }
         # run
         system($executable, @executable_args);

         # process the status from the run if on windows
         my $raw_status = $?;
         my $processed_status;
         if ( acl::Env::is_windows() ) {
            $processed_status = ($raw_status>>8) | ($raw_status&255);
         } else {
            $processed_status = $raw_status;
         }

         # status was ok
         if ($processed_status == 0){
            # check if find a profile.mon after the run - if not, tell users to recompile with -profile
            if (!open( IN,"< $default_mon_name")){
               print $self->prog." $default_mon_name file not found after execution. Please recompile the design with the -profile option in order to get data.\n";
               if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
               exit 0;
            }
            if ($shared_counters_on){
               if ($iter == 0){
                 # Create "profile_shared.mon"
                 acl::File::copy($default_mon_name,$default_shared_mon_name);
               } else {
                 acl::File::concat($default_shared_mon_name, $default_mon_name, $default_shared_mon_name); 
               }
               acl::File::remove_tree($default_mon_name);
               if ($iter  == 3){
                  profile_gpp_and_move_files($no_json, $no_run, $aocx_path, $default_shared_mon_name, $output_dir, $default_shared_mon_name);
                  if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
                  exit 0;
               }
            } else {
               profile_gpp_and_move_files($no_json, $no_run, $aocx_path, $default_mon_name, $output_dir, $default_mon_name);
               if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
               exit 0;
            }
         } else {
            print STDERR $self->prog." Error: The execution failed with an error. \n";
            print STDERR $self->prog." Returned status: $processed_status \n";
            if ($remove_temp_aocx != 0) { unlink "temp.aocx"; }
            exit 1;
         }
      }
   }
}

sub htmlreport {
  my ($self,@args) = @_;

  if (scalar(@args) != 2) {
    print STDERR $self->prog." Error: Usage expects 2 arguments.\n";
    print STDERR             "  Usage:\n    ".$self->prog." htmlreport <Directory containing JSON files> <Output directory>\n";
    print STDERR             "  Example:\n    ".$self->prog." htmlreport test/reports/lib/json newFolder\n";
    exit 1;
  }
  my $json_dir = $args[0];
  my $outputdir = $args[1];
  my $report_dir = $outputdir."/reports";
  acl::File::make_path($report_dir) or return;
  acl::Report::create_reporting_tool_from_json($json_dir, $report_dir);
}

sub _print_or_unrecognized(@) {
   my ($self,$name,$printval,@args) = @_;
   if ( $#args >= 0 ) {
      print STDERR $self->prog." $name: Unrecognized option: $args[0]\n";
      return undef;
   }
   print $printval,"\n";
   return $self;
}


sub get_ldflags{
  my ($direct_link, $smart_link, $icd_link) = get_link_args(@_);
  @_ = grep {$_ ne '--direct_link'} @_;
  @_ = grep {$_ ne '--smart_link'} @_;
  @_ = grep {$_ ne '--icd_link'} @_;

  acl::Common::populate_installed_packages();
  my ($result, @args);
  if(($icd_link) or (acl::Common::is_fcd_present() and acl::Common::is_icd_present() and $smart_link)){
    if (acl::Env::is_linux()){
      $result = "-L".acl::Env::sdk_root()."/host/".acl::Env::get_arch(@_)."/lib";
    }else{
      $result = "/libpath:$acl_root/host/windows64/lib";
    }
    @args = @_;
  } else{
    if ($#installed_packages <= 0) {
      if ($#installed_packages == 0) {
        $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = shift @installed_packages;
      }
      ($result, @args) = acl::Env::host_ldflags(@_);
    } else {
      die "Cannot find any fcd files when multiple board packages are installed\n";
    }
  }
  return $result, @args;
}


sub get_ldlibs{
  my ($direct_link, $smart_link, $icd_link) = get_link_args(@_);
  @_ = grep {$_ ne '--direct_link'} @_;
  @_ = grep {$_ ne '--smart_link'} @_;
  @_ = grep {$_ ne '--icd_link'} @_;

  acl::Common::populate_installed_packages();
  my ($result, @args);
  if(($icd_link) or (acl::Common::is_fcd_present() and acl::Common::is_icd_present() and $smart_link)){
    if (acl::Env::is_linux()) {
      $result = "-lOpenCL";
    } else {
      $result = "OpenCL.lib";
    }
    @args = @_;
  }else{
    if ($#installed_packages <= 0) {
      if ($#installed_packages == 0) {
        $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = shift @installed_packages;
      }
      ($result, @args) = acl::Env::host_ldlibs(@_);
    } else {
      die "Cannot find any fcd files when multiple board packages are installed\n";
    }
  }
  return $result, @args;
}


sub link_config {
  my ($ldflagsVal, @ldflagsArgs) = get_ldflags(@_);
  my ($ldlibsVal, @ldlibsArgs) = get_ldlibs(@_);
  shift->_print_or_unrecognized('link-config', $ldflagsVal." ".$ldlibsVal, @ldlibsArgs[ 1 .. $#ldlibsArgs ]);
}

sub linkflags {
  # This function produce the same result as link-config.
  my ($ldflagsVal, @ldflagsArgs) = get_ldflags(@_);
  my ($ldlibsVal, @ldlibsArgs) =  get_ldlibs(@_);
  shift->_print_or_unrecognized('linkflags', $ldflagsVal." ".$ldlibsVal, @ldlibsArgs[ 1 .. $#ldlibsArgs ]);
}

sub ldflags {
  my ($ldflagsVal, @ldflagsArgs) = get_ldflags(@_);
  shift->_print_or_unrecognized('ldflags', $ldflagsVal, @ldflagsArgs[ 1 .. $#ldflagsArgs ]);
}

sub ldlibs {
  my ($ldlibsVal, @ldlibsArgs) = get_ldlibs(@_);
  shift->_print_or_unrecognized('ldlibs', $ldlibsVal, @ldlibsArgs[ 1 .. $#ldlibsArgs ]);
}

sub get_link_args {
  #check if a direct_link or smart_linking is selected argument is given (choose the first one that shows up)
  my $direct_link = 0;
  my $smart_link = 1; #setting smart_link to default flow
  my $icd_link = 0;
  foreach my $arg (@_){
    if ($arg eq "--direct_link"){
      $smart_link = 0;
      $direct_link = 1;
      $icd_link = 0;
      last;
    }
    if ($arg eq "--smart_link"){
      $smart_link = 1;
      $direct_link = 0;
      $icd_link = 0;
      last;
    }
    if ($arg eq "--icd_link"){
      $smart_link = 0;
      $direct_link = 0;
      $icd_link = 1;
      last;
    }
  }
  return $direct_link, $smart_link, $icd_link;
}

sub board_hw_path {
   my ($self,$variant,@args) = @_;
   unless ( $variant ) {
      print STDERR $self->prog." board-hw-path: Missing a board variant argument\n";
      return undef;
   }
   $self->_print_or_unrecognized('board-hw-path',acl::Env::board_hw_path($variant,@args));
}
sub board_path { shift->_print_or_unrecognized('board-path',acl::Env::board_path(@_)); }
sub board_mmdlib { shift->_print_or_unrecognized('board-mmdlib',acl::Env::board_mmdlib(@_)); }
sub board_libs { shift->_print_or_unrecognized('board-libs',acl::Env::board_libs(@_)); }
sub board_link_flags { shift->_print_or_unrecognized('board-libs',acl::Env::board_link_flags(@_)); }
sub board_default { shift->_print_or_unrecognized('board-default',acl::Env::board_hardware_default(@_)); }
sub board_version { shift->_print_or_unrecognized('board-version',acl::Env::board_version(@_)); }
sub board_name { shift->_print_or_unrecognized('board-version',acl::Env::board_name(@_)); }


sub board_xml_test {
   my $self = shift;
   my $aocl = acl::Env::sdk_aocl_exe();
   print " board-path       = ".`$aocl board-path`."\n";
   my $board_version = `$aocl board-version`;
   print " board-version    = $board_version\n";
   print " board-name       = ".`$aocl board-name`."\n";
   my $bd_default = `$aocl board-default`;
   print " board-default    = ".$bd_default."\n";
   print " board-hw-path    = ".`$aocl board-hw-path $bd_default`."\n";
   print " board-link-flags = ".`$aocl board-link-flags`."\n";
   print " board-libs       = ".`$aocl board-libs`."\n";
   print " board-util-bin   = ".acl::Board_env::get_util_bin()."\n";
   if ( $board_version >= 15.1 ) {
     print " board-mmdlib     = ".`$aocl board-mmdlib`."\n";
   }
   return $self;
}



sub check_board_utility_env {
  my ($self) = @_;

  # Check that BSP is <= SDK version number
  # This call will force an exit if the version is illegal
  my $bsp_version =  acl::Board_env::get_board_version();

  # Check that we're not in emulator mode
  if (defined $ENV{CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA})
  {
    printf "%s %s: Can't run board utilities with CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA set\n", $self->prog, $self->cmd;
    return 0;
  }
  if (defined $ENV{CL_CONTEXT_EMULATOR_DEVICE_ALTERA})
  {
    printf "Warning: CL_CONTEXT_EMULATOR_DEVICE_ALTERA is deprecated. Use CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA instead\n";
    printf "%s %s: Can't run board utilities with CL_CONTEXT_EMULATOR_DEVICE_ALTERA set\n", $self->prog, $self->cmd;
    return 0;
  }
  return 1;
}

sub program {
  my ($self,@args) = @_;
  my $result = reprogram(@_);
  if ( $? ) { return undef; }
  if ( not defined $result ) { return undef; }
  return $self;
}

# checks if fpga.bin is fast-compile'd or not
sub check_fast_compile {
  my $binfile = shift @_;
  my $pkg = get acl::Pkg($binfile);
  if ( !defined($pkg) ) {
    print "Failed to open file: $binfile\n";
    return -1;
  }
  my $fast_compile_section = '.acl.fast_compile';
  if ($pkg->exists_section($fast_compile_section)) {
    if (`aocl binedit $binfile print $fast_compile_section` == 1) {
      return 1;
    }
  }
  return 0
}

# Return full path to fpga_temp.bin
sub get_fpga_temp_bin {
   my $arg = shift @_;
   my $pkg = get acl::Pkg($arg);
   if ( !defined($pkg) ) {
     print "Failed to open file: $arg\n";
     return -1;
   }
   my $hasbin = $pkg->exists_section('.acl.fpga.bin');
   if (not $hasbin )
   {  return ""; }
   my $tmpfpgabin = acl::File::mktemp();
   my $fpgabin = $tmpfpgabin;
   if ( length $tmpfpgabin == 0 ) {
     # In case we fail to get a temp file, use local dir.  Using PID
     # as a uniqifier is safe here since this function is called only
     # once by flash, or once by program, and not both in the same process.
     $fpgabin = "fpga_temp_$$.bin";
   } else {
     $fpgabin .= '_fpga_temp.bin';
   }
   my $gotbin = $pkg->get_file('.acl.fpga.bin', $fpgabin);
   if ( !defined( $gotbin )) {
     print "Failed to extract binary section from file: $arg\n";
     print "  Tried: $fpgabin and $tmpfpgabin\n";
     return "";
   }
   return $fpgabin;
}

sub reprogram {
   my ($self, @args) = @_;
   # Parse the arguments
   my $device = undef;
   my $aocx = undef;
   my $board_package_root = undef;
   my $num_args = @args;

   populate_attached_devices($self);
   my $is_old_board = 0;
   # Need to know the whether the board version is <15.1
   if ($#installed_packages == 0) {
     $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = @installed_packages[0];
     if ( acl::Board_env::get_board_version() < 15.1 ) {
       $is_old_board = 1;
     }
   }
   foreach my $arg ( @args ) {
     my ($ext) = $arg =~ /(\.[^.]+)$/;
     if ( $ext eq '.aocx' ) {
       $aocx = $arg;
     } else {
       my ($acl_num) = $arg =~ /^acl(\d+)$/;
       if ($is_old_board) {
         $device = $arg;
         $board_package_root = @installed_packages[0];
       }else{
         if ( !defined($acl_num) or $acl_num < 0) {
            print STDERR "Missing or invalid device \'$acl_num\'\n";
            my $help = new acl::Command($self->prog, qw(help program));
            $help->run();
            return undef;
         }

         if($acl_num < scalar(@device_map_multiple_packages)/2){
             $device = @device_map_multiple_packages[$acl_num * 2];
             $board_package_root = @device_map_multiple_packages[$acl_num * 2 + 1];
         }else{
           print STDERR "Device \'$acl_num\' not part of known packages.\n";
           diagnostic();
           return undef;
         }
       }
     }
   }
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $board_package_root;
   # If arguments not valid, print help/usage message.
   if ( $num_args != 2 or !defined($aocx) or !defined($device) or !defined($board_package_root)) {
      my $help = new acl::Command($self->prog, qw(help program));
      $help->run();
      return undef;
   }
   my $utilbin = acl::Board_env::get_util_bin();
   my $util = ( acl::Board_env::get_board_version() < 14.1 ) ? "reprogram" : "program";
   check_board_utility_env($self) or return undef;
   my $command = acl::File::which( "$utilbin","$util" );
   if ( defined $command ) {
     print $self->prog." program: Running $util from $utilbin\n";
     # Get .bin from the AOCX file and call reprogram with that
     my $fpgabin = get_fpga_temp_bin($aocx);
     if ( length $fpgabin == 0 ) {
       printf "%s program: Program failed. Error reading aocx file.\n", $self->prog;
       return undef;
     }

     if ( acl::Board_env::get_board_version() > 15.0)
     {
       # new 15.1 boards
       delete $ENV{CL_CONTEXT_COMPILER_MODE_INTELFPGA};
       # Delete the deprecated name also since developers may be using it
       delete $ENV{CL_CONTEXT_COMPILER_MODE_ALTERA};
       # setting the environment variable below is for A10 boards, it will cause
       # the runtime environment to ignore the board name when attempting to reprogram
       $ENV{ACL_PCIE_PROGRAM_SKIP_BOARDNAME_CHECK}='1';
       system("$utilbin/$util","$device",$fpgabin,$aocx);
     } else {
       # old pre-15.1 boards
       system("$utilbin/$util","$device",$fpgabin);
     }
     #remove the file we ouput
     unlink $fpgabin;

     if ( $? ) { printf "%s program: Program failed.\n", $self->prog; return undef; }
     return $self;
   } else {
     print "--------------------------------------------------------------------\n";
     print "No programming routine supplied.                                    \n";
     print "Please consult your board manufacturer's documentation or support   \n";
     print "team for information on how to load a new image on to the FPGA.     \n";
     print "--------------------------------------------------------------------\n";
     return undef;
   }
   return $self;
}

sub flash {
   my ($self, @args) = @_;
   # Parse the arguments
   my $device = undef;
   my $aocx = undef;
   my $board_package_root = undef;
   my $num_args = @args;

   populate_attached_devices($self);
   my $is_old_board = 0;
   # Need to know the whether the board version is <15.1
   if ($#installed_packages == 0) {
     $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = @installed_packages[0];
     if ( acl::Board_env::get_board_version() < 15.1 ) {
       $is_old_board = 1;
     }
   }
   foreach my $arg ( @args ) {
     my ($ext) = $arg =~ /(\.[^.]+)$/;
     if ( $ext eq '.aocx' ) {
       $aocx = $arg;
     } else {
       my ($acl_num) = $arg =~ /^acl(\d+)$/;
       if ($is_old_board) {
         $device = $arg;
         $board_package_root = @installed_packages[0];
       }else{
         if ( !defined($acl_num) or $acl_num < 0) {
            print STDERR "Missing or invalid device.\n";
            my $help = new acl::Command($self->prog, qw(help program));
            $help->run();
            return undef;
         }

         if($acl_num < scalar(@device_map_multiple_packages)/2){
             $device = @device_map_multiple_packages[$acl_num * 2];
             $board_package_root = @device_map_multiple_packages[$acl_num * 2 + 1];
         }else{
           print STDERR "Device not part of know packages.\n";
           diagnostic();
           return undef;
         }
       }
     }
   }
   # If arguments not valid, print help/usage message.
   if ( $num_args != 2 or !defined($aocx) or !defined($device) or !defined($board_package_root)) {
      my $help = new acl::Command($self->prog, qw(help flash));
      $help->run();
      return undef;
   }
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $board_package_root;
   my $utilbin = acl::Board_env::get_util_bin();
   my $util = "flash";
   check_board_utility_env($self) or return undef;
   my $command = acl::File::which( "$utilbin","$util" );
   if ( defined $command ) {
     print $self->prog." flash: Running $util from $utilbin\n";
     # Get .bin from the AOCX file and call flash with that
     my $fpgabin = get_fpga_temp_bin($aocx);
     my $fast_compile = check_fast_compile($fpgabin);
     if ( length $fpgabin == 0 || $fast_compile == -1 ) { printf "%s flash: Flashing failed. Error reading aocx file.\n", $self->prog; return undef; }
     if ( $fast_compile == 1 ) { printf "%s flash: Flashing failed. Cannot flash fast-compile'd aocx file.\n", $self->prog; return undef; }

     system("$utilbin/$util","$device",$fpgabin);
     #remove the file we ouput
     unlink $fpgabin;

     if ( $? ) { printf "%s flash: Program failed.\n", $self->prog; return undef; }
     return $self;
     print $self->prog." flash: Running flash from $utilbin\n";
     system("$utilbin/$util",$device,@args);
     if ( $? ) { printf "%s flash: Program failed.\n", $self->prog; return undef; }
   } else {
     print "--------------------------------------------------------------------\n";
     print "No flash routine supplied.                                    \n";
     print "Please consult your board manufacturer's documentation or support   \n";
     print "team for information on how to load a new image on to the FPGA.     \n";
     print "--------------------------------------------------------------------\n";
   }
   return $self;
}

sub diagnose {
  my ($self,@args) = @_;
  diagnostic(@_);
  if ( $? ) { return undef; }
  return $self;
}

sub diagnostic {
   my ($self,@args) = @_;
   my $icd_diagnostic_only = 0;
   # check if user would like to only diagnose the icd flow
   foreach (@args) {
     if ($_ eq "-icd-only" or $_ eq "--icd-only") {
       $icd_diagnostic_only = 1;
     }
   }
   # first carry out ICD diagnostics
   print "--------------------------------------------------------------------\n";
   print "ICD System Diagnostics                                              \n";
   print "--------------------------------------------------------------------\n";

   my $icd_diagnose_result = icd_diagnostic();
   if ($icd_diagnose_result == 0){
     print "--------------------------------------------------------------------\n";
     print "ICD diagnostics PASSED                                              \n";
     print "--------------------------------------------------------------------\n";
   }else{
     print "--------------------------------------------------------------------\n";
     print "ICD diagnostics FAILED                                              \n";
     print "--------------------------------------------------------------------\n";
   }
   if ($icd_diagnostic_only == 1) {
     # terminate if user just wanna diagnose icd-flow
     return $self;
   }
   print "--------------------------------------------------------------------\n";
   print "BSP Diagnostics                                                     \n";
   print "--------------------------------------------------------------------\n";

   # If no arguments, just list all the attached boards with their board packages
   if (scalar@args == 0) {
     list_devices(@_, "diagnose");
     print "\nCall \"aocl diagnose <device-names>\" to run diagnose for specified devices\n";
     print "Call \"aocl diagnose all\" to run diagnose for all devices\n";
   } else {
     populate_attached_devices($self);
     my $is_old_board = 0;
     # Need to know the whether the board version is <15.1
     if ($#installed_packages == 0) {
       $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = @installed_packages[0];
       if ( acl::Board_env::get_board_version() < 15.1 ) {
         $is_old_board = 1;
       }
     }
     # if aocl all, push all devices to @args
     if (scalar@args == 1 and @args[0] eq "all") {
       if ($is_old_board) {
         my $old_bsp = @installed_packages[0];
         die "\"diagnose all\" is not supported for $old_bsp\n";
       }
       shift @args;
       for (my $i=0;$i < scalar(@device_map_multiple_packages)/2;$i++) {
         push @args, "acl$i";
       }
     }
     # run diagnose for every device in @args
     while (@args) {
       my $logical_device = shift @args;
       my $phys_device = undef;
       my $package_root = undef;
       my ($acl_num) = $logical_device =~ /^acl(\d+)$/;

       # if acl(num) and num is within the range, logical device name
       if ( (!$is_old_board) and (defined $acl_num) and $acl_num >= 0 and $acl_num < scalar(@device_map_multiple_packages)/2 ) {
         $phys_device = @device_map_multiple_packages[$acl_num * 2];
         $package_root = @device_map_multiple_packages[$acl_num * 2 + 1];
         $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $package_root;
       # If old board, this is the physical device name
       } elsif ( $is_old_board ) {
         $phys_device = $logical_device;
         $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = @installed_packages[0];
       # else, physical device name
       } else {
         print "--------------------------------------------------------------------\n";
         print "$logical_device is not a valid device name.                         \n";
         print "--------------------------------------------------------------------\n";
         next;
       }
       my $utilbin = acl::Board_env::get_util_bin();
       my $util = ( acl::Board_env::get_board_version() < 14.1 ) ? "diagnostic" : "diagnose";
       check_board_utility_env($self) or return undef;
       my $command = acl::File::which( "$utilbin","$util" );
       if ( ! defined $command ) {
         print "--------------------------------------------------------------------\n";
         print "No board diagnose routine supplied.                                 \n";
         print "Please consult your board manufacturer's documentation or support   \n";
         print "team for information on how to debug board installation problems.   \n";
         print "--------------------------------------------------------------------\n";
         next;
       }
       system("$utilbin/$util", $phys_device);
     }
   }
   return $self;
}

# icd_diagnostic
# performs a series of tests to verify if icd and fcd is properly set up on this system to allow for dynamic linking
# input: none
# output: return status 0 (pass) or 1 (fail)
sub icd_diagnostic {

  # == find all icd files == #
  my $icd_override = 0;
  my $icd_dir = acl::Env::get_icd_dir();
  
  if (acl::Env::is_linux()) {
    if (defined $ENV{'OCL_ICD_VENDORS'} and !($ENV{'OCL_ICD_VENDORS'} eq "")){
      $icd_dir = $ENV{'OCL_ICD_VENDORS'};
      $icd_override = 1;
    }
  }

  print "\nUsing the following location for ICD installation: \n";
  print "\t$icd_dir\n";
  my @icd_files;
  my $command_output;
  my $ICDstatus = 1;
  my $FCDstatus = 1;
  my $Check_FILENAMES = 0;

  @icd_files = getFilesFromSearch($ICDstatus,$icd_dir,0);

  my $have_ocl_icd_filenames = (defined $ENV{'OCL_ICD_FILENAMES'} and !($ENV{'OCL_ICD_FILENAMES'} eq ""));

  my $opencl_lib = acl::Env::is_linux() ? 'libOpenCL.so' : 'OpenCL.dll';
  my $altera_lib = acl::Env::is_linux() ? 'libalteracl.so' : 'alteracl.lib';

  if ($have_ocl_icd_filenames & (!$ICDstatus)) {
    	print "\nWARNING: Skipping ICD validation since there is no ICD entry at this location \n";  
  } 

  elsif (!$ICDstatus) {
    print "\nERROR: No ICD entry at that location.  Without ICD and FCD, host executables\nmust be linked directly to Intel FPGA runtime ($altera_lib) and BSP MMD\nlibrary instead of to the Khronos ICD library ($opencl_lib).\n";
    print "\tTo use ICD, please reinstall the Intel OpenCL SDK.\n";
    return 1;
  }

  # print all icd files found
  if (@icd_files) {
    my $tmp = scalar @icd_files;
    my $i = 1;
    print "\nFound $tmp icd entry at that location:\n";
    foreach (@icd_files){
      print "\t$_\n";
      $i += 1;
    }
  }

  # == check if icd files are registered properly == #
  if ($ICDstatus) {
     validateICD(\@icd_files);
  }

  if ($have_ocl_icd_filenames) {
    print "\nUsing OCL_ICD_FILENAMES to search for ICD clients, it is set to $ENV{'OCL_ICD_FILENAMES'}\n";
    $Check_FILENAMES=1; 
    if (validateOCL_ICD_FILENAMES() & !$ICDstatus) {
       print "\nERROR: No ICD entry and all files specified by OCL_ICD_FILENAMES are invalid \n";
       return 1; 
    }
    $Check_FILENAMES=0;
  }


  # == find all fcd files == #
  my $fcd_override = 0;
  my $fcd_dir = acl::Env::get_fcd_dir();
  if (defined $ENV{'ACL_BOARD_VENDOR_PATH'} and !($ENV{'ACL_BOARD_VENDOR_PATH'} eq "") and acl::Env::is_linux()){
    $fcd_dir = $ENV{'ACL_BOARD_VENDOR_PATH'};
    $fcd_override = 1;
  }
  my @fcd_files;
  print "\nUsing the following location for fcd installations:\n";
  print "\t$fcd_dir\n";
  @fcd_files = getFilesFromSearch($FCDstatus,$fcd_dir,1);

  if (!$FCDstatus){
    print "\nERROR: No FCD entry at that location. Without ICD and FCD, host executables\nmust be linked directly to Intel FPGA runtime ($altera_lib) and BSP MMD\nlibrary instead of to the Khronos ICD library ($opencl_lib).\n";
    print "\tTo use FCD, please reinstall using 'aocl install'.\n";
    return 1;
  }
  # print all fcd files found
  if (@fcd_files){
    my $tmp = scalar @fcd_files;
    my $i = 1;
    print "\nFound $tmp fcd entry at that location:\n";
    foreach (@fcd_files){
      print "\t$_\n";
      $i += 1;
    }
  }

  # == check if fcd files are registered properly == #
  validateFCD(\@fcd_files);

  # == execute host to list all available platforms == #
  my $diagnose_host_dir = acl::Env::sdk_host_bin_dir().acl::Env::pathsep()."aocl-get-platform-diagnose";
  my $command_output = system("$diagnose_host_dir");
  if ($command_output != 0){
    print "\nERROR: OpenCL host failed\n";
    return 1;
  }
  # validateICD
  # iterate through the given ICD files and confirm if they are registered correctly
  # input: \@icd_files (list of strings, each representing an ICD fullpath/basename if linux, or registry entry if windows)
  # output: none
  sub validateICD{
    my @icd_files = @{$_[0]};
    if (acl::Env::is_linux()){ # Linux flow
      checkLibraryPath(\@icd_files, 0, $icd_dir);
    }else{ # Windows flow
      if (defined $ENV{OCL_ICD_FILENAMES}){
        checkLibraryPath(\@icd_files, 0, $icd_dir);
      }
      print "\nChecking validity of found icd entries:\n";
      checkRegistry(\@icd_files,0);
    }
  }

  # validateOCL_ICD_FILENAMES
  # make sure when ICDstatus = 0, OCL_ICD_FILENAMES point to AT LEAST ONE valid file
  # input: none 
  # output: 0 if at least one file specified by OCL_ICD_FILENAMES is valid
  #         1 otherwise
  sub validateOCL_ICD_FILENAMES {
    my $FILENAMESstatus = 0;
    my @libraries;
    if (acl::Env::is_linux()){
         push @libraries, (split /:/, $ENV{OCL_ICD_FILENAMES});
    } else {
         push @libraries, (split /;/, $ENV{OCL_ICD_FILENAMES});
    }
    check_LD_LIBRARY_PATH(\@libraries,$FILENAMESstatus);
    if (!$FILENAMESstatus) {
         return 1;
    }
    return 0;
  }

  # validateFCD
  # iterate through the given FCD files and confirm if they are registered correctly
  # input: \@fcd_files (list of strings, each representing an FCD fullpath/basename if linux, or registry entry if windows)
  # output: none
  sub validateFCD{
    my @fcd_files = @{$_[0]};
    if(acl::Env::is_linux()){ # Linux flow
      checkLibraryPath(\@fcd_files, 1, $fcd_dir);
    }else{ # Windows flow
      if ($fcd_override){ #in this case we are only checking for files and not registry
        checkLibraryPath(\@fcd_files, 1, $fcd_dir);
      }else{
        print "\nChecking validity of found fcd entries:\n";
        checkRegistry(\@fcd_files,1);
      }
    }
  }

  # checkLibraryPath
  # Iterate through the given FCD/ICD files, and check if they exist on the system. must be full path, or a file on LD_LIBRARY_PATH
  # The inputed search value must be either 0 (for icd) or 1 (for fcd)
  # The dir value must be the directory to search on windows (ACL_ICD_PATH for ICD, ACL_BOARD_VENDOR_PATH for FCD)
  # input: \@files (list of strings, each representing an FCD/ICD fullpath/basename if linux, or registry entry if windows),
  #   $search, $dir
  # output: none
  sub checkLibraryPath {
    my @files = @{$_[0]};
    my $search = $_[1];
    my $dir = $_[2];
    $search = $search == 0 ? "icd": "fcd";
    my @libraries;

    print "\nThe following OpenCL libraries are referenced in the $search files:\n";
    foreach my $file (@files){
      if (acl::Env::is_linux()){
        $command_output = `cat $file`;
      }else{
        my $tmp = $dir.'\\'.$file;
        $command_output = `type $tmp`;
      }
      chomp($command_output);
      @libraries = split("\n",$command_output);
      print "\t$command_output\n";
      if ($command_output eq ""){
        print "\tWARNING: $file is empty\n";
      }
    }
    check_LD_LIBRARY_PATH(\@libraries,1);
 }

 # check_LD_LIBRARY_PATH
 # Iterate files in input and check if they exist on the system. must be full path, or a file on LD_LIBRARY_PATH
 # change the second parameter to 1 if there exist at least one valid file specified by OCL_ICD_FILENAMES
 # input: \@libraries (parsed by checkLibraryPath)
 #        $_[1]: a flag with value 0 if we want to assert at least one file specified by OCL_ICD_FILENAMES is valid, 1 otherwise
 # output: none

 sub check_LD_LIBRARY_PATH {
    my @libraries = @{$_[0]};
    # check LD_LIBRARY_PATH for libraries
    my $env_var_to_check = acl::Env::is_linux() ? 'LD_LIBRARY_PATH' : 'PATH';
    if ($Check_FILENAMES) {
        print "\nChecking $env_var_to_check for registered libraries specified by OCL_ICD_FILENAMES\n";
    } else {
    	print "\nChecking $env_var_to_check for registered libraries:\n";
    }
    $command_output = $ENV{$env_var_to_check};
    chomp($command_output);
    my @ld_library_paths;
    if (acl::Env::is_linux()) {
      @ld_library_paths = split /:/, $command_output;
      # dlopen in the runtime automatically checks these two folders for libraries
      push(@ld_library_paths, "/usr/lib");
      push(@ld_library_paths, "/usr/lib64");
    } else {
      @ld_library_paths = split /;/, $command_output;
    }
    foreach my $library (@libraries) {
      if ( -e $library) {
        if (!$_[1] & $Check_FILENAMES) { $_[1] = 1; }
        print "\t$library was registered on the system.\n";
        next;
      } else {
        $library = acl::File::mybasename($library);
        foreach my $path (@ld_library_paths) {
          my $fullpath;
          if (acl::Env::is_linux()) {
            $fullpath = $path."/".$library;
          } else {
            $fullpath = $path."\\".$library;
          }
          if (-e $fullpath) {
            if (!$_[1] & $Check_FILENAMES) { $_[1] = 1; }
            print "\t$library was registered on the system at $path\n";
            last;
          }
          if ($path eq @ld_library_paths[-1]) {
            print "\tWARNING: $library NOT FOUND\n";
          }
        }
      }
    }
  }

  # getFilesFromSearch
  # Searches for either fcd or icd files/registry_keys in the given directory/registry.
  # Sets the status bit to 0 if files could not be found for any reason.
  # The inputed search value must be either 0 (for icd) or 1 (for fcd)
  # input: $status, $dir, $search
  # output: none
  sub getFilesFromSearch{
    my $status = $_[0];
    my @files;
    my $dir = $_[1];
    my $search = $_[2];
    EXIT_IF: {
      #finding linux files
      if (acl::Env::is_linux()){
        if ($search == 0){
          $search = ".icd";
        }
        else{
          $search = ".fcd";
        }

        $command_output = `find $dir -maxdepth 1 -name "*$search" 2>/dev/null`;
        chomp($command_output);
        if ($command_output eq "" or index($command_output,"No such file or directory") != -1){
          $status = 0;
          last EXIT_IF;
        }
        @files =  split /\n/, $command_output;

      #finding windows files
      }else{
        my $override = 0;
        if ($search == 0){
          $search = 'icd.dll';
        }
        else{
          $search = 'mmd.dll';
        }
        my $command_output = `reg query $dir /v *$search 2>&1`;
        chomp($command_output);
        my @lines = split /\n/,$command_output;

        foreach (@lines){
          if ($_ =~ $search){
            chomp ($_);
            $_ =~ s/^\s+//;
            push @files, $_;
          }

          if ($_ eq @lines[-1] and !@files){
            $status = 0;
            last EXIT_IF;
          }
        }
      }
    }
    $_[0] = $status;
    $_[1] = $dir;

    return @files;
  }

  # checkRegistry
  # Iterates through a list of windows registries and verifies if they are tpe DWORD, value of 0, and named after an existing file path.
  # Prints out whether each registry is valid or not.
  # The inputed search value must be either 0 (for icd) or 1 (for fcd)
  # input: \@files, $search
  # output: none
  sub checkRegistry{
    my @files = @{$_[0]};
    my $search = $_[1];

    if ($search == 0){
      $search = 'icd.dll';
    }
    else{
      $search = 'mmd.dll';
    }
    # get the directories from PATH
    my @paths = split(';',$ENV{"PATH"});

    foreach (@files){
      my $valid = 1;
      # check if path links correctly
      my @lines = split / /, $_;
      foreach (@lines){
        my $path_valid = 0;
        if ($_ =~ "$search"){
          # check if the dll file is either present in a (1) fullpath format or (2) relative format that is registerd in PATH
          if (!(-e $_)){
            # not in the fullpath format, need to check from directory registered in the PATH
            foreach my $_path (@paths) {
              if(-e $_path.'\\'.$_) {
                  $path_valid = 1;
                  last;
              }
            }
            if(! $path_valid){
              print "\tWARNING $_ was not found\n";
              $valid = 0;
            }
          }
        }
      }

      if ($_ !~ "DWORD"){
        print "\tWARNING: $_ registry type is not DWORD!\n";
        $valid = 0;
      }
      if ($_ !~ "0x0"){
        print "\tWARNING: $_ registry data is not 0x0!\n";
        $valid = 0;
      }
      if ($valid){
        print "\t$_ is correctly registered on the system\n";
      }

    }
  }
}

sub install {
   # The install subroutine will install the fcd and driver to the system
   # If flag -fcd-only is set, it will only install the fcd
   # If the board package has been installed already, the installation will be aborted
   # Two environmental variables are required:
   # AOCL_BOARD_PACKAGE_ROOT: the path of the bsp
   # ACL_BOARD_VENDOR_PATH: the path of the fcd file
   my ($self,@args) = @_;
   my $board_package_path = "";
   my $fcd_only = 0;
   my $bsp_path_cnt = 0;
   my $force_setup_fcd = 0;
   my $util = "install";
   # parsing the argument to get the using -fcd-only flag and bsp path
   foreach my $arg (@args) {
     if($arg eq "-fcd-only" or $arg eq "--fcd-only") {
       # user choose to fcd-setup only
       $fcd_only = 1;
     } elsif ($arg eq "-f" ){
       # user choose to install even if BSP with same name was already installed
       $force_setup_fcd = 1;
     } else{
       # The user specifies a board package name or path.
       my $tmp = acl::Common::get_board_package_path($arg);
       if (defined $tmp){
       if ($bsp_path_cnt == 0) {
           $board_package_path = $tmp;
         $bsp_path_cnt += 1;
       } else {
         # The user specifies too many board package paths
           print "Too many board package name or paths are provided, please install them one by one.\n";
         return undef
       }
     } else {
      # the user has garbage input
      die "Unrecognized argument: $arg Try aocl -help for the description of aocl install.\n";
     }
   }
   }

   if ($board_package_path eq "") {
     # use the bsp path from AOCL_BOARD_PACKAGE_ROOT
     my $acl_board_path = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
     # If the environment variable is not set or is empty, prompt the user to provide a board package root
     if (!defined $acl_board_path or $acl_board_path eq "") {
         # the board package path is not specified,
         print "Please specify a board package name or path to install:\n";
         my $tmp = <STDIN>; chomp $tmp;
         $board_package_path = acl::Common::get_board_package_path($tmp);
     # Otherwise, install the bsp specified in AOCL_BOARD_PACKAGE_ROOT
     } else {
         $board_package_path = $acl_board_path;
     }
   }

   # first check the path/registry key to install fcd
   if (! defined $ENV{ACL_BOARD_VENDOR_PATH}) {
     # need the user input for the directory to install fcd if ACL_BOARD_VEBDOR_PATH is not defined
     acl::Common::check_installed_fcd_path("install");
   }

   acl::Common::populate_installed_packages();
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = acl::File::abs_path($board_package_path);
   my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};

   my $board_package_full_path = acl::Board_env::get_board_path();

   # check the version/duplication of the installing and existing bsps
   if ($#installed_packages >= 0) {
     # firstly check if the installing bsp is with version >=15.1
     if ( acl::Board_env::get_board_version() < 15.1 ) {
       die "Mutiple board packages are not supported for $board_package_path\n";
     }
     # secondly populate the installed bsps and check the version >=15.1. We still need to check it for backward compatibility
     # the same time check if the bsp is already registered in the installed_packages
     foreach my $bsp (@installed_packages) {
       $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $bsp;
       if ( acl::Board_env::get_board_version() < 15.1 ) {
         die "Mutiple board packages are not supported for $bsp\n";
       }
       if ( $bsp eq $board_package_full_path ) {
         print $self->prog." $util: $board_package_full_path is already registered in the system, please uninstall it before reinstalling\n";
         return $self;
       }
     }
     # resume the env var AOCL_BOARD_PACKAGE_ROOT
     $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
   }
   # push the new bsp to the list of installed_packages if there is no version conflicting or duplication
   push @installed_packages, $board_package_full_path;

   # FCD and installed packages
   my $utilbin = acl::Board_env::get_util_bin();
   check_board_utility_env($self) or return undef;
   my $command = acl::File::which( "$utilbin","$util" );
   print $self->prog." $util: Setting up the FPGA Client Driver (FCD) to the system. This process may require admin privilege\n";
   acl::Common::setup_fcd($board_package_full_path, $force_setup_fcd);
   print $self->prog." $util: Adding the board package $board_package_full_path to the list of installed packages\n";
   acl::Common::save_to_installed($board_package_full_path);


   if (!$fcd_only) {
     # Install binary
     print "Installing the board package driver to the system.\n";
     if ( defined $command ) {
       print $self->prog." $util: Running $util from $utilbin\n";
       system("$utilbin/$util",@args);
       if ( $? ) {
         printf "%s $util: failed.\n", $self->prog;
         print $self->prog." $util: Removing FCD\n";
         acl::Common::remove_fcd($board_package_full_path);
         print $self->prog." $util: Removing the board package $board_package_full_path from the list of installed packages\n";
         acl::Common::remove_from_installed($board_package_full_path);
         return undef;
       }
     } else {
       print "--------------------------------------------------------------------\n";
       print "Warning: No board installation routine supplied.                    \n";
       print "Please consult your board manufacturer's documentation or support   \n";
       print "team for information on how to properly install your board.         \n";
       print "--------------------------------------------------------------------\n";
     }
   }
   return $self;
}

sub uninstall {
   my ($self,@args) = @_;
   my $board_package_path = "";
   my $fcd_only = 0;
   my $bsp_path_cnt = 0;
   my $util = "uninstall";
   # parsing the argument to get the using -fcd-only flag and bsp path
   foreach my $arg (@args) {
     if($arg eq "-fcd-only" or $arg eq "--fcd-only") {
       # user choose to fcd-setup only
       $fcd_only = 1;
     } else {
       # The user specifies a board package name or path.
       my $tmp = acl::Common::get_board_package_path($arg);
       if (defined $tmp){
       if ($bsp_path_cnt == 0) {
           $board_package_path = $tmp;
         $bsp_path_cnt += 1;
       } else {
         # The user specifies too many board package paths
           print "Too many board package name or paths are provided, please uninstall them one by one.\n";
         return undef
       }
     } else {
      # the user has garbage input
         die "Unrecognized argument: $arg Try aocl -help for the description of aocl uninstall.\n";
       }
     }
   }

   # uninstall FCD and installed_packages
   if (! defined $ENV{ACL_BOARD_VENDOR_PATH}) {
      acl::Common::check_installed_fcd_path("uninstall");
   }

   acl::Common::populate_installed_packages();
   if ($board_package_path eq "") {
     # no bsp is specified, need the user to specify the path of bsp to uninstall
     $board_package_path = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
     if (!defined $board_package_path or $board_package_path eq "") {
       # user need to specify the board package to uninstall if AOCL_BOARD_PACKAGE_ROOT is not provided
       print "Please call aocl uninstall <board package name or path> to uninstall the bsp\n";
       print "Installed board packages list:\n";
       if ($#installed_packages < 0) {
         print "No package installed\n";
       } else {
         foreach my $package (@installed_packages) {
           print "$package\n";
         }
       }
       return $self;
     }
   }

   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = acl::File::abs_path($board_package_path);
   my $board_package_full_path = acl::Board_env::get_board_path();
   if ( not grep { $_ eq $board_package_full_path } @installed_packages ) {
      print $self->prog." $util: $board_package_path is not installed, no need to uninstall.\n";
      return $self;
   }

   my $utilbin = acl::Board_env::get_util_bin();

   check_board_utility_env($self) or return undef;
   my $command = acl::File::which( "$utilbin","$util" );

   print $self->prog." $util: Removing the FPGA Client Driver (FCD) from the system\n";
   acl::Common::remove_fcd($board_package_full_path);
   print $self->prog." $util: Removing the board package $board_package_full_path from the list of installed packages. This process may require admin privilege\n";
   acl::Common::remove_from_installed($board_package_full_path);

   # Uninstall binary
   if (!$fcd_only) {
     if ( defined $command ) {
       print $self->prog." $util: Running $util from $utilbin\n";
       system("$utilbin/$util",@args);
       if ( $? ) {
         printf "%s $util: failed.\n", $self->prog;
         return undef;
       }
     } else {
       print "--------------------------------------------------------------------\n";
       print "No board uninstallation routine supplied.                           \n";
       print "Please consult your board manufacturer's documentation or support   \n";
       print "team for information on how to properly uninstall your board.       \n";
       print "--------------------------------------------------------------------\n";
     }
   }

   return $self;
}

sub binedit {
   my ($self,@args) = @_;
   system(acl::Env::sdk_pkg_editor_exe(),@args);
   return undef if $?;
   return $self;
}

# List available boards
sub list_devices {
   my ($self,@args) = @_;
   populate_attached_devices($self);
   my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
   for (my $i=0; $i<scalar(@device_map_multiple_packages)/2; $i++) {
     my $logical_device = "acl$i";
     my $phys_device = @device_map_multiple_packages[2 * $i];
     my $package_root = @device_map_multiple_packages[2 * $i +1];

     $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $package_root;
     my $utilbin = acl::Board_env::get_util_bin();
     my $util = ( acl::Board_env::get_board_version() < 14.1 ) ? "diagnostic" : "diagnose";
     check_board_utility_env($self) or next;
     my $probe_out = `$utilbin/$util -probe $phys_device`;

     print("--------------------------------------------------------------------\n");
     print("Device Name:\n");
     print("$logical_device\n\n");
     print("BSP Install Location:\n");
     print("$package_root\n\n");
     print("$probe_out");
     print("--------------------------------------------------------------------\n");
   }

   for (my $i=0; $i<scalar(@packages_without_devices); $i++) {
     my $package_root = @packages_without_devices[$i];
     $ENV{"AOCL_BOARD_PACKAGE_ROOT"} = $package_root;
     print("--------------------------------------------------------------------\n");
     print("Warning:\n");
     print("No devices attached for package:\n");
     print("$package_root\n");
     print("--------------------------------------------------------------------\n");

     if (scalar@args == 1 and @args[0] eq "diagnose") {
       my $utilbin = acl::Board_env::get_util_bin();
       my $util = ( acl::Board_env::get_board_version() < 14.1 ) ? "diagnostic" : "diagnose";
       check_board_utility_env($self) or next;
       my $probe_out = `$utilbin/$util`;
       print $probe_out;
       print("--------------------------------------------------------------------\n");
      }
   }
   # resume the env var AOCL_BOARD_PACKAGE_ROOT
   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
   return $self;
}

sub initialize {
   my ($self, @args) = @_;
   # Parse the arguments
   my $device = undef;
   my $board_variant = undef;
   my $board_package_root = undef;
   my $num_args = @args;

   populate_attached_devices($self);

   # If number of argumnets, print help/usage message.
   if ( $num_args > 2 or $num_args < 1 ) {
      print STDERR "Missing or extra arguments\n";
      my $help = new acl::Command($self->prog, qw(help initialize));
      $help->run();
      return undef;
   }

   # Sanitize first argument: device
   my ($acl_num) = $args[0] =~ /^acl(\d+)$/;
   if ( !defined($acl_num) or $acl_num < 0) {
      print STDERR "Missing or invalid device \'$acl_num\'\n";
      my $help = new acl::Command($self->prog, qw(help initialize));
      $help->run();
      return undef;
   }

   if($acl_num < scalar(@device_map_multiple_packages)/2){
       $device = @device_map_multiple_packages[$acl_num * 2];
       $board_package_root = @device_map_multiple_packages[$acl_num * 2 + 1];
   }else{
     print STDERR "Device \'$acl_num\' not part of known packages.\n";
     diagnostic();
     return undef;
   }

   # Sanitize optional second argument: board_variant
   if( $num_args == 2 ){
      # no way of verifying if the variant is sanitized or not.
      # acl::Board_env::board_hw_path() will return a path to where the board is
      # supposed to be if it can't find the variant provided. This is done for
      # backwards compatability
      $board_variant = $args[1];
   }else{
      ($board_variant) = acl::Env::board_hardware_default();
   }

   $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $board_package_root;

   my $utilbin = acl::Board_env::get_util_bin();
   my $util = "initialize";
   check_board_utility_env($self) or return undef;
   my $command = acl::File::which( "$utilbin","$util" );
   if ( defined $command ) {
      print $self->prog." initialize: Running $util from $utilbin\n";
      delete $ENV{CL_CONTEXT_COMPILER_MODE_INTELFPGA};
      delete $ENV{CL_CONTEXT_COMPILER_MODE_ALTERA}; # Delete the deprecated name also since developers may be using it
      # setting the environment variable below will cause the runtime environment to ignore the board name when attempting to reprogram
      $ENV{ACL_PCIE_PROGRAM_SKIP_BOARDNAME_CHECK}='1';
      system("$utilbin/$util","$device",$board_variant);

      if ( $? ) { printf "%s initialize: Program failed.\n", $self->prog; return undef; }
      return $self;
   } else {
      print "--------------------------------------------------------------------\n";
      print "No initializing routine supplied.                                    \n";
      print "--------------------------------------------------------------------\n";
      return undef;
   }
   return $self;
}

sub xlibrary {
   #to allow fpga_crossgen to call this without warnings
   my ($self,@args) = @_;
   system(acl::Env::sdk_libedit_exe(),@args);
   return undef if $?;
   return $self;
}

sub library {
   my ($self,@args) = @_;
   print "Warning: This way of creating libraries have been deprecated. Use fpga_crossgen instead.\n"; 
   system(acl::Env::sdk_libedit_exe(),@args);
   return undef if $?;
   return $self;
}

sub hash {
   my ($self,@args) = @_;
   system(acl::Env::sdk_hash_exe(),@args);
   return undef if $?;
   return $self;
}


sub _cflags_include_only {
   my $ACL_ROOT = acl::Env::sdk_root();
   return "-I$ACL_ROOT/host/include";
}

sub _get_cross_compiler_include_directories {
   my ($cross_compiler) = @_;

   my $includes = undef;
   my $ACL_ROOT = acl::Env::sdk_root();
   my $output = `$cross_compiler -v -c $ACL_ROOT/share/lib/c/includes.c -o /dev/null 2>&1`;
   $? == 0 or print STDERR "Error determing cross compiler default include directories\n";
   my $add_includes = 0;
   my @lines = split('\n', $output);
   foreach my $line (@lines) {
      if ($line =~ /^#include <\.\.\.> search starts here:/) {
         $add_includes = 1;
      } elsif ($line =~ /^End of search list./) {
         $add_includes = 0;
      } elsif ($add_includes) {
         $includes .= " -I".$line;
      }
   }
   return $includes." ";
}

sub compile_config {
   my ($self,@args) = @_;
   my $extra_flags = undef;
   while ( $#args >= 0 ) {
      my $arg = shift @args;
      if ( $arg eq '--arm-cross-compiler' ) {
         if (acl::Env::is_windows()) {
            print STDERR $self->prog." compile-config: --arm-cross-compiler is not supported on Windows.\n";
            return undef;
         }
         if ($#args >= 0) {
            my $cross_compiler = shift @args;
            $extra_flags = _get_cross_compiler_include_directories($cross_compiler);
         } else {
            print STDERR $self->prog." compile-config: --arm-cross-compiler requires an argument.\n";
            return undef;
         }
      } elsif ( $arg eq '--arm' ) {
         # Just swallow the arg.
      } else {
         print STDERR $self->prog." compile-config: unknown option $arg.\n";
         return undef;
      }
   }

   my $board_flags = "";
   acl::Common::populate_installed_packages();

   if (!acl::Common::is_fcd_present()) {
     if ($#installed_packages < 0) {
       $board_flags = acl::Board_env::get_xml_platform_tag_if_exists("compileflags");
     } elsif ($#installed_packages == 0) {
       $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = shift @installed_packages;
       $board_flags = acl::Board_env::get_xml_platform_tag_if_exists("compileflags");
     } else {
       die "Cannot find any fcd files when multiple board packages are installed\n";
     }
   }
   print $extra_flags . _cflags_include_only(). " $board_flags" . "\n";
   return $self;
}

sub cflags {
   my ($self,@args) = @_;
   compile_config(@_);
   return $self;
}


sub example_makefile {
   my ($self,@args) = @_;
   my $help = new acl::Command($self->prog, qw(help example-makefile), @args);
   $help->run();
   return $self;
}


sub makefile {
   my ($self,@args) = @_;
   my $help = new acl::Command($self->prog, qw(help example-makefile), @args);
   $help->run();
   return $self;
}

sub AUTOLOAD {
   my $self = shift;
   my $class = ref($self) or die "$self is not an object";
   my $name = $AUTOLOAD;
   $name =~ s/^.*:://;
   my $result = $${self}{$name};
   return $result;
}


sub help {
   my ($self,$topic,@args) = @_;
   my $prog = $self->prog;

   my $sdk_root_name = acl::Env::sdk_root_name();
   my $is_sdk = acl::Env::is_sdk();
   my $sdk = $is_sdk ? "SDK" : "RTE";
   my $sdk_first_mention = $is_sdk ? "SDK" : "Runtime Environment (RTE)";
   my $target_arm = ($#args >= 0) and ($args[0] eq '--arm');

   my $use_aoc_note= <<USE_AOC_NOTE;
Note: Use the separate "aoc" command to compile your OpenCL(TM) kernel programs.
USE_AOC_NOTE
   my $use_aoc_in_rte_note= <<USE_AOC_IN_RTE_NOTE;
Note: Use the "aoc" command from the Intel(R) FPGA SDK for OpenCL(TM) to compile
your OpenCL(TM) kernel programs.
USE_AOC_IN_RTE_NOTE
   my $aoc_note = $is_sdk ? $use_aoc_note : $use_aoc_in_rte_note;

   my $loader_advice =<<LOADER_ADVICE;
   Additionally, at runtime your host program must run in an enviornment
   where it can find the shared libraries provided by the Intel(R) FPGA $sdk for
   OpenCL(TM).

   For example, on Windows the PATH environment variable should include
   the directory %$sdk_root_name%/host/windows64/bin.

   For example, on Linux the LD_LIBRARY_PATH environment variable should
   include the directory \$$sdk_root_name/host/linux64/lib.

See also: $prog example-makefile
LOADER_ADVICE

   my $host_compiler_options = <<HOST_COMPILER_OPTIONS;
   --msvc, --windows       Show link line for Microsoft Visual C/C++.
   --gnu, -gcc, --linux    Show link line for GCC toolchain on Linux.
   --arm                   Show link line for cross-compiling to arm.
HOST_COMPILER_OPTIONS

my $makefile_help_arm = <<MAKEFILE_EXAMPLE_HELP_ARM_ONLY;

Example GNU makefile cross-compiling to ARM SoC from Linux or Windows, with
Linaro GCC cross-compiler toolchain:

CROSS-COMPILER=arm-linux-gnueabihf-
AOCL_COMPILE_CONFIG=\$(shell $prog compile-config --arm)
AOCL_LINK_CONFIG=\$(shell $prog link-config --arm)

host_prog : host_prog.o
    \$(CROSS-COMPILER)g++ -o host_prog host_prog.o \$(AOCL_LINK_CONFIG)

host_prog.o : host_prog.cpp
    \$(CROSS-COMPILER)g++ -c host_prog.cpp \$(AOCL_COMPILE_CONFIG)


MAKEFILE_EXAMPLE_HELP_ARM_ONLY

my $makefile_help = <<MAKEFILE_EXAMPLE_HELP;

The following are example Makefile fragments for compiling and linking
a host program against the host runtime libraries included with the
Intel(R) FPGA $sdk for OpenCL(TM).


Example GNU makefile on Linux, with GCC toolchain:

AOCL_COMPILE_CONFIG=\$(shell $prog compile-config)
AOCL_LINK_CONFIG=\$(shell $prog link-config)

host_prog : host_prog.o
    g++ -o host_prog host_prog.o \$(AOCL_LINK_CONFIG)

host_prog.o : host_prog.cpp
    g++ -c host_prog.cpp \$(AOCL_COMPILE_CONFIG)


Example GNU makefile on Windows, with Microsoft Visual C++ command line compiler:

AOCL_COMPILE_CONFIG=\$(shell $prog compile-config)
AOCL_LINK_CONFIG=\$(shell $prog link-config)

host_prog.exe : host_prog.obj
    link -nologo /OUT:host_prog.exe host_prog.obj \$(AOCL_LINK_CONFIG)

host_prog.obj : host_prog.cpp
    cl /MD /Fohost_prog.obj -c host_prog.cpp \$(AOCL_COMPILE_CONFIG)


MAKEFILE_EXAMPLE_HELP

   my %_help_topics = (

     'example-makefile', ($target_arm ? $makefile_help_arm : $makefile_help . $makefile_help_arm),

     'compile-config', <<COMPILE_CONFIG_HELP,

$prog compile-config - Show compilation flags for host programs


Usage: $prog compile-config


Example use in a GNU makefile on Linux:

   AOCL_COMPILE_CONFIG=\$(shell $prog compile-config)
   host_prog.o :
    g++ -c host_prog.cpp \$(AOCL_COMPILE_CONFIG)

See also: $prog example-makefile

COMPILE_CONFIG_HELP


      'link-config', <<LINK_CONFIG_HELP,

$prog link-config - Show linker flags and libraries for host programs.


Usage: $prog link-config [options]

   By default the link line for the current platform are shown.


Description:

   This subcommand shows the linker flags and the list of libraries
   required to link a host program with the runtime libraries provided
   by the Intel(R) FPGA $sdk for OpenCL(TM).

   This subcommand combines the functions of the "ldflags" and "ldlibs"
   subcommands.

$loader_advice

Options:
$host_compiler_options

LINK_CONFIG_HELP


      'ldflags', <<LDFLAGS_HELP,

$prog ldflags - Show linker flags for building a host program.


Usage: $prog ldflags [options]

   By default the linker flags for the current platform are shown.


Description:

   This subcommand shows the general linker flags required to link
   your host program with the runtime libraries provied by the
   Intel(R) FPGA $sdk for OpenCL(TM).

   Your link line also must include the runtime libraries from the Intel(R) FPGA
   $sdk for OpenCL(TM) as listed by the "ldlibs" subcommand.

$loader_advice

Options:
$host_compiler_options

LDFLAGS_HELP


      'ldlibs', <<LDLIBS_HELP,

$prog ldlibs - Show list of runtime libraries for building a host program.


Usage: $prog ldlibs [options]

   By default the libraries for the current platform are shown.


Description:

   This subcommand shows the list of libraries provided by the
   Intel(R) FPGA $sdk for OpenCL(TM) that are required link a host program.

   Your link line also must include the linker flags as listed by
   the "ldlfags" subcommand.

$loader_advice

Options:
$host_compiler_options

LDLIBS_HELP

      'program', <<BOARD_PROGRAM_HELP,

$prog program - Configures a new FPGA design onto your board


Usage: $prog program <device_name> <file.aocx>

   Supply the .aocx file for the design you wish to configure onto
   the FPGA. You need to provide <device_name> to specify the FPGA
   device to configure with.

Description:

   This command downloads a new design onto your FPGA.
   This utility should not normally be used, users should instead use
   clCreateProgramWithBinary to configure the FPGA with the .aocx file.

BOARD_PROGRAM_HELP

      'flash', <<BOARD_FLASH_HELP,

$prog flash - Initialize the FPGA with a specific startup configuration.


Usage: $prog flash <device_name> <file.aocx>

   Supply the .aocx file for the design you wish to set as the default
   configuration which is loaded on power up.

Description:

   This command initializes the board with a default configuration
   that is loaded onto the FPGA on power up. Not all boards will
   support this, check with your board vendor documentation.

BOARD_FLASH_HELP

      'diagnose', <<BOARD_DIAGNOSTIC_HELP,

$prog diagnose - Run your board vendor's test program for the board.


Usage: $prog diagnose
       $prog diagnose <device_name_1> [<device_name_2> ... ]
       $prog diagnose all

Description:

   This command executes a board vendor test utility to verify the
   functionality of the device specified by <device-names>.

   If <device-names> is not specified, it will show a list of currently
   installed devices that are supported by all the installed board packages.

   The utility should output the text DIAGNOSTIC_PASSED as the final
   line of output. If this is not the case (either that text is absent,
   the test displays DIAGNOSTIC_FAILED, or the test doesn't terminate),
   then there may be a problem with the board.


BOARD_DIAGNOSTIC_HELP

      'list-devices', <<BOARD_LIST_HELP,

$prog list-devices -  Lists all installed devices currently installed devices
                      that are supported by all the installed board packages.


Usage: $prog list-devices

Description:

   This command lists all the currently installed devices that are supported
   by the installed board packages.

BOARD_LIST_HELP

      'install', <<BOARD_INSTALL_HELP,

$prog install -  Installs a board onto your host system.


Usage: $prog install
       $prog install <name or path>

Description:

   This command installs a board's drivers and other necessary
   software for the host operating system to communicate with the
   board. For example this might install PCIe drivers.

BOARD_INSTALL_HELP

      'uninstall', <<BOARD_UNINSTALL_HELP,

$prog uninstall -  Installs a board onto your host system.


Usage: $prog uninstall
       $prog uninstall <name or path>

Description:

   This command uninstalls a board's drivers and other necessary
   software for the host operating system to communicate with the
   board. For example this might uninstall PCIe drivers.

BOARD_UNINSTALL_HELP

      'profile', <<PROFILE_HELP,

$prog profile - Runs the host code on the device, and collects
                device-side information about the run, then outputs
                it so it can be viewed using VTune.


Usage: $prog profile [-x <path/to/aocx>.aocx] [-e <path/to/oneAPI executable>] [options] <path/to/executable> [executable options]

Description:

   If the design was compiled with -profile enabled (see aoc options
   for information on -profile), this command runs the executable
   and collects device-side profiling information during the run,
   outputting a profile.mon file. It then parses this infomation into
   a profile.json file which can be opened and viewed using VTune.
   It requires an executable which runs the compiled design (unless
   no-run flag is set). If you are using the oneAPI Data Parallel
   C++ Compiler, you should pass in the dpcpp binary (which contains
   the device code) directly. If you wish to instead pass a script 
   that calls the binary, the dpcpp binary must also be passed 
   in with '-e'. In non-oneAPI compiles, the .aocx file needs to 
   be passed in with -x if it is not in the folder you are running from.

   There are several options that allow for more control over this
   command and the data it outputs:
      - '-period ###' can be used to adjust the number of cycles between
        readbacks from the device. The default behavior is to read back
        data as often as possible.
      - '-no-temporal' turns off readback during execution, and only reads
        back non-autorun kernels once they finish executing
      - '-no-mem-transfers' turns off the recording of memory transfer information
      - '-output-dir <new/output/dir>' changes where the profile.mon and
        profile.json files are placed
      - '-no-run <location/of/profile.mon>' will not run an executable (this
        option can be used without the executable input), it just generates
        a profile.json based on the profile.mon file.
      - '-no-json' will just run the executable and output the profile.mon file
        without parsing it into a profile.json

PROFILE_HELP

      'env', <<ENV_HELP,

$prog env - Show the compilation environment of a binary.


Usage: $prog env <file.aoco/aocx>

Description:

   This command takes the aoco or aocx file provided and displays
   the compiler's input arguments and environment for that design.

ENV_HELP

      'initialize', <<INITIALIZE_HELP,

$prog env - Configure a default FPGA image onto the board.


Usage: $prog initialize <device_name> [board_variant]

You need to provide <device_name> to specify the FPGA
device you want to configure. Optionally you can supply the
board variant, if no board variant is provided the
default board variant will be used. The board variant determines
the variant the board will be initalized with.

Description:

   This command downloads a default design onto your FPGA. The
   board variant argument defines which configuration your FPGA
   will be in after the initialization. You can find the list of
   available board variants by executing 'aoc -list-boards'

INITIALIZE_HELP

      help => <<GENERAL_HELP,

$prog - Intel(R) FPGA $sdk_first_mention for OpenCL(TM) utility command.


$aoc_note

Subcommands for building your host program:

   $prog example-makefile  Show Makefile fragments for compiling and linking
                          a host program.
   $prog makefile          Same as the "example-makefile" subcommand.

   $prog compile-config    Show the flags for compiling your host program.
   $prog link-config       Show the flags for linking your host program with the
                          runtime libraries provided by the Intel(R) FPGA $sdk for OpenCL(TM).
                          This combines the function of the "ldflags" and "ldlibs"
                          subcomands.
   $prog linkflags         Same as the "link-config" subcommand.

   $prog ldflags           Show the linker flags used to link your host program
                          to the host runtime libraries provided by the Intel(R) FPGA $sdk
                          for OpenCL(TM).  This does not list the libraries themselves.

   $prog ldlibs            Show the list of host runtime libraries provided by the
                          Intel(R) FPGA $sdk for OpenCL(TM).

Subcommands for managing an FPGA board:

   $prog program           Configure a new FPGA image onto the board.

   $prog flash             [If supported] Initialize the FPGA with a specified
                           powerup configuration.

   $prog initialize        Configure a default FPGA image onto the board.

   $prog install           Install your board into the current host system.

   $prog uninstall         Uninstall your board from the current host system.

   $prog diagnose          Run your board vendor's test program for the board.

   $prog list-devices      Lists all installed devices.

General:

   $prog profile           Run your profiled design and collect device-side profiling information
                          that can be displayed with VTune.
   $prog library           Manage OpenCL(TM) libraries. Run "$prog library help" for more info.
   $prog env               Show the compilation environment of a binary
   $prog version           Show version information.
   $prog help              Show this help.
   $prog help <subcommand> Show help for a particular subcommand.

GENERAL_HELP
   );

   $_help_topics{'linkflags'} = $_help_topics{'link-config'};
   $_help_topics{'linkflags'} =~ s/link-config/linkflags/g;

   $_help_topics{'cflags'} = $_help_topics{'compile-config'};
   $_help_topics{'cflags'} =~ s/compile-config/cflags/g;

   $_help_topics{'makefile'} = $_help_topics{'example-makefile'};

   if ( defined $topic ) {
      my $output = $_help_topics{$topic};
      if ( defined $output ) { print $output; }
      else { print $_help_topics{'help'}; return undef; }
   } else {
      print $_help_topics{'help'};
   }
   return $self;
}

1;
