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


# Intel(R) FPGA SDK for OpenCL(TM) kernel compiler.
#  Inputs:  A .cl file containing all the kernels
#  Output:  A subdirectory containing:
#              Design template
#              Verilog source for the kernels
#              System definition header file
#
#
# Example:
#     Command:       aoc foobar.cl
#     Generates:
#        Subdirectory foobar including key files:
#           *.v
#           <something>.qsf   - Quartus project settings
#           <something>.sopc  - SOPC Builder project settings
#           kernel_system.tcl - SOPC Builder TCL script for kernel_system.qsys
#           system.tcl        - SOPC Builder TCL script
#
# vim: set ts=2 sw=2 et

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


use strict;

require acl::Board_migrate;
require acl::Common;
require acl::Env;
require acl::File;
require acl::Incremental;
require acl::Pkg;
require acl::Report;
require acl::Simulator;
use acl::AOCDriverCommon;
use acl::AOCInputParser;
use acl::AOCOpenCLStage;
use acl::Report qw(escape_string);

sub print_bsp_msgs($@)
 {
     my $infile = shift @_;
     my $verbose = acl::Common::get_verbose();
     open(IN, "<$infile") or acl::Common::mydie("Failed to open $infile");
     while( <IN> ) {
       # E.g. Error: BSP_MSG: This is an error message from the BSP
       if( $_ =~ /BSP_MSG:/ ){
         my $filtered_line = $_;
         $filtered_line =~ s/BSP_MSG: *//g;
         if( $filtered_line =~ /^ *Error/ ) {
           print STDERR "$filtered_line";
         } elsif ( $filtered_line =~ /^ *Critical Warning/ ) {
           print STDOUT "$filtered_line";
         } elsif ( $filtered_line =~ /^ *Warning/ && $verbose > 0) {
           print STDOUT "$filtered_line";
         } elsif ( $verbose > 1) {
           print STDOUT "$filtered_line";
         }
       }
     }
     close IN;
 }

sub print_quartus_errors($@)
{ #filename
  my $infile = shift @_;
  my $infileErr = shift @_;
  my $flag_recomendation = shift @_;
  my $win_longpath_flag = 0;

  open(ERR, "<$infile") or acl::Common::mydie("Failed to open $infile");
  while( my $line = <ERR> ) {
    if( ($line =~ /^Error/) || ($line =~ /^Critical Warning/)) {
      
      if( acl::AOCDriverCommon::hard_routing_error_code( $line )) {
        my $seed_setting_flag = $sycl_mode ? "-Xsseed=<S>\n" : "-seed=<S>\n";
        my $high_effort_flag  = $sycl_mode ? "-Xshigh-effort\n" : "-high-effort\n";

        print STDERR "Error: The fitter failed to successfully route the design. You may be able get this design to route by making design modifications.\n";
        print STDERR "Error: You can also try to fit your design by using different seed with flag $seed_setting_flag\n";
        print STDERR "Error: You can also try using the high-effort compile flag $high_effort_flag\n" if ($flag_recomendation);
      } 
      if( acl::AOCDriverCommon::kernel_fit_error( $line ) ) {
        print STDERR "Error: The design was too large to fit in the device.\n";
        print STDERR"For more detail, full Quartus compile output can be found in files $infileErr and $infile.\n";
        acl::Common::mydie("Cannot fit kernel(s) on device");
      }
      elsif ( acl::AOCDriverCommon::win_longpath_error_quartus( $line ) ) {
        $win_longpath_flag = 1;
        print $line;
      }
      elsif ($line =~ /Error\s*(?:\(\d+\))?:/) {
        print $line;
      }
    }
    if( $line =~ /Path name is too long/ ) {
      $win_longpath_flag = 1;
      print $line;
    }
  }
  close ERR;
  print $win_longpath_suggest if ($win_longpath_flag and acl::Env::is_windows());

  if (open(ERR, "<$infileErr")) {
    while( my $line = <ERR> ) {
      if ( $line =~ /^Out of memory/ ) {
        print "Error: Out of memory error detected.\n";
        print "This can occur if the design is too big or the machine does not meet the system\n";
        print "requirements. Make sure your system meets the system requirements specified on\n";
        print "the System and Software Requirements page of the Intel FPGA website\n";
        print "(https://fpgasoftware.intel.com/requirements/).\n";
      }
    }
    close ERR;
  }  
  print STDERR"For more details, full Quartus compile output can be found in files $infileErr and $infile.\n";

  acl::Common::mydie("Compiler Error, not able to generate hardware\n");
}

sub print_fast_emulator_messages($$) {
  my $log_file = shift @_;
  my $return_status = shift @_;
  # Go through ioc.log and print any errors or warnings.
  open(INPUT, '<', $log_file) or acl::Common::mydie("Cannot open $log_file $!");
  my $start_printing = acl::Common::get_verbose() > 1;
  my $compile_failed = $return_status;
  my $print_buffer = '';
  while (my $line = <INPUT>) {
    $compile_failed = 1 if ($line =~ m/^Compilation failed!?$/);
    if ($line =~ m/^Unable to load OpenCL platforms$/) {
      $compile_failed = 1;
      $print_buffer .= $line;
    } elsif (acl::Common::get_verbose() > 2) {
      $print_buffer .= $line;
    } elsif ($line =~ m/^Compilation started$/) {
      $start_printing = 1;
    } elsif ($line =~ m/^Compilation failed$/ and $start_printing == 0) {
      $start_printing = 1;
    } elsif ($line =~ m/^Compilation failed!?$/) {
      $start_printing = 0 unless acl::Common::get_verbose();
    } elsif ($line =~ m/^Compilation done$/) {
      $start_printing = 0;
    } elsif ($start_printing) {
      $print_buffer .= $line;
    }
  }
  close INPUT;
  if ($compile_failed) {
    print STDERR $print_buffer;
  } else {
    print $print_buffer;
  }
  return $compile_failed;
}

# Do setup checks:
# Since aoc now support Quartus less flow, quartus check will not be performed during setup stage
# Instead, at the setup stage aoc will only check the front end environments
# Quartus check will be pushed to compile design (aocx compilation) stage 
sub check_env {
  my ($bsp_variant, $board_variant, $bsp_flow_name) = @_;
  my $verbose = acl::Common::get_verbose();
  if ($do_env_check and not $emulator_fast) {
    # Is clang on the path?
    acl::Common::mydie ("$prog: The Intel(R) FPGA SDK for OpenCL(TM) compiler front end (aocl-clang$exesuffix) can not be found")  unless -x $clang_exe.$exesuffix;
    # Do we have a license?
    my $clang_output = acl::Common::mybacktick("$clang_exe --version 2>&1");
    chomp $clang_output;
    if ($clang_output =~ /Could not acquire OpenCL SDK license/ ) {
      acl::Common::mydie("$prog: Cannot find a valid license for the Intel(R) FPGA SDK for OpenCL(TM)\n");
    }
    if ($clang_output !~ /Intel\(R\) FPGA SDK for OpenCL\(TM\), Version/ ) {
      print "$prog: Clang version: $clang_output\n" if $verbose||$regtest_mode;
      if ($^O !~ m/MSWin/ and ($verbose||$regtest_mode)) {
        my $ld_library_path="$ENV{'LD_LIBRARY_PATH'}";
        print "LD_LIBRARY_PATH is : $ld_library_path\n";
        foreach my $lib_dir (split (':', $ld_library_path)) {
          if( $lib_dir =~ /dspba/){
            if (! -d $lib_dir ){
              print "The library path: $lib_dir does not exist\n";
            }
          }
        }
      }
      my $failure_cause = "The cause of failure cannot be determined. Run executable manually and watch for error messages.\n";
      # Common cause on linux is an old libstdc++ library. Check for this here.
      if ($^O !~ m/MSWin/) {
        my $clang_err_out = acl::Common::mybacktick("$clang_exe 2>&1 >/dev/null");
        if ($clang_err_out =~ m!GLIBCXX_!) {
          $failure_cause = "Cause: Available libstdc++ library is too old. You're probably using an unsupported version of Linux OS. " .
                           "A quick work-around for this is to get latest version of gcc (at least 4.4) and do:\n" .
                           "  export LD_LIBRARY_PATH=<gcc_path>/lib64:\$LD_LIBRARY_PATH\n";
        }
      }
      acl::Common::mydie("$prog: Executable $clang_exe exists but is not working!\n\n$failure_cause");
    }

    # Is /opt/llc/system_integrator on the path?
    acl::Common::mydie ("$prog: The Intel(R) FPGA SDK for OpenCL(TM) compiler front end (aocl-opt$exesuffix) can not be found")  unless -x $opt_exe.$exesuffix;
    my $opt_out = acl::Common::mybacktick("$opt_exe  --version 2>&1");
    chomp $opt_out;
    if ($opt_out !~ /Intel\(R\) FPGA SDK for OpenCL\(TM\), Version/ ) {
      acl::Common::mydie("$prog: Cannot find a working version of executable (aocl-opt$exesuffix) for the Intel(R) FPGA SDK for OpenCL(TM)\n");
    }
    acl::Common::mydie ("$prog: The Intel(R) FPGA SDK for OpenCL(TM) compiler front end (aocl-llc$exesuffix) can not be found")  unless -x $llc_exe.$exesuffix;
    my $llc_out = acl::Common::mybacktick("$llc_exe --version");
    chomp $llc_out;
    if ($llc_out !~ /Intel\(R\) FPGA SDK for OpenCL\(TM\), Version/ ) {
      acl::Common::mydie("$prog: Cannot find a working version of executable (aocl-llc$exesuffix) for the Intel(R) FPGA SDK for OpenCL(TM)\n");
    }
    acl::Common::mydie ("$prog: The Intel(R) FPGA SDK for OpenCL(TM) compiler front end (system_integrator$exesuffix) can not be found")  unless -x $sysinteg_exe.$exesuffix;
    my $system_integ = acl::Common::mybacktick("$sysinteg_exe --help");
    chomp $system_integ;
    if ($system_integ !~ /system_integrator - Create complete OpenCL system with kernels and a target board/ ) {
      acl::Common::mydie("$prog: Cannot find a working version of executable (system_integrator$exesuffix) for the Intel(R) FPGA SDK for OpenCL(TM)\n");
    }
  }

  if ($do_env_check and $emulator_fast) {
    # Is the Intel offline compiler (ioc64) on the path?
    acl::Common::mydie ("$prog: The Intel(R) Kernel Builder for OpenCL(TM) compiler ($ioc_exe$exesuffix)".
                        " can not be found") unless -x $ioc_exe.$exesuffix;
    my $ioc_output = acl::Common::mybacktick("$ioc_exe -version");
    chomp $ioc_output;
    if ($ioc_output !~ /Kernel Builder for OpenCL API/ && $ioc_output !~ /Intel\(R\) SDK for OpenCL\(TM\)/) {
      if ($^O !~ m/MSWin/ and ($verbose||$regtest_mode)) {
        my $ld_library_path="$ENV{'LD_LIBRARY_PATH'}";
        print "LD_LIBRARY_PATH is : $ld_library_path\n";
      }
      acl::Common::mydie("$prog: Executable $ioc_exe exists but is not working!\n\n");
    }
  }

  # get the bsp information (for both rtl and hw flow)
  my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
  my $board_spec_xml = acl::AOCDriverCommon::find_board_spec($acl_board_hw_path);
  if( ! $bsp_flow_name ) {
    $bsp_flow_name = ":".acl::Env::aocl_boardspec( "$board_spec_xml", "defaultname" );
  }
  $target_model = acl::Env::aocl_boardspec( "$board_spec_xml", "targetmodel".$bsp_flow_name);
  $bsp_version = acl::Env::aocl_boardspec( "$board_spec_xml", "version");
  if ($do_env_check and !acl::Env::is_bsp_version_compatible($bsp_version)){
    print "Warning: A potentially unsupported Intel(R) BSP version was found: $bsp_version.\n Confirm with your board vendor that this BSP is supported by your current installation of the Intel(R) FPGA SDK for OpenCL(TM)." if $verbose;
  }

  ( $target_model !~ /error/i ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");
  $target_model =~ s/_tm.xml//;

  # Compile Check: fast|aggressive compiles|incremental
  if ($fast_compile && $high_effort_compile) {
    acl::Common::mydie("Illegal argument combination: cannot specify both fast-compile and high-effort compile options\n");
  }

  if ($fast_compile) {
    if ($target_model !~ /(^arria10)|(^stratix10)/) {
      acl::Common::mydie("Fast compile is not supported on your device family.\n");
    }
    if ($target_model =~ /^stratix10/) {
      print "Warning: Fast compile on S10 device family is preliminary and has limited support.\n";
    }
  }
  if ($high_effort_compile) {
    if ($target_model !~ /(^arria10)|(^stratix10)/) {
      acl::Common::mydie("High effort compile is not supported on your device family.\n");
    }
  }
  if ($incremental_compile) {
    if ($target_model =~ /^stratix10/) {
      print "Warning: Incremental compile on S10 device family is preliminary and has limited support.\n";
    }

    # To support empty kernel flow on incremental, need to add check to see if user's empty-kernel file has changed
    # Error out unless someone has a use case for this
    if ($empty_kernel_flow) {
      acl::Common::mydie("-empty-kernel flag is not supported with incremental compiles\n");
    }
  }

  # Make sure that hyper-optimized handshaking is only specified for S10 devices
  if ($user_opt_hyper_optimized_hs) {
    if ($target_model !~ /^stratix10/ && $target_model !~ /^agilex/) {
      acl::Common::mydie("-hyper-optimized-handshaking can only be used with S10 and Agilex family.\n");
    }
  }

  # If here, everything checks out fine.
  print "$prog: Environment checks completed successfully.\n" if $verbose;

}

sub extract_atoms_from_postfit_netlist($$$$) {
  my ($base,$location,$atom,$bsp_flow_name) = @_;

   # Grab DSP location constraints from specified Quartus compile directory
    my $script_abs_path = acl::File::abs_path( acl::Env::sdk_root()."/ip/board/bsp/extract_atom_locations_from_postfit_netlist.tcl");

    # Pre-process relativ or absolute location
    my $location_dir = '';
    if (substr($location,0,1) eq '/') {
      # Path is already absolute
      $location_dir = $location;
    } else {
      # Path is currently relative
      $location_dir = acl::File::abs_path("../$location");
    }

    # Error out if reference compile directory not found
    if (! -d $location_dir) {
      acl::Common::mydie("Directory '$location' for $atom locations does not exist!\n");
    }

    # Error out if reference compile board target does not match
    my $current_board = ::acl::Env::aocl_boardspec( ".", "name");
    my $reference_board = ::acl::Env::aocl_boardspec( $location_dir, "name");
    if ($current_board ne $reference_board) {
      acl::Common::mydie("Reference compile board name '$reference_board' and current compile board name '$current_board' do not match!\n");
    };

    my $project = ::acl::Env::aocl_boardspec( ".", "project".$bsp_flow_name);
    my $revision = ::acl::Env::aocl_boardspec( ".", "revision".$bsp_flow_name);
    ( $project.$revision !~ /error/i ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");
    chomp $revision;
    if (defined $ENV{ACL_QSH_REVISION})
    {
      # Environment variable ACL_QSH_REVISION can be used
      # replace default revision (internal use only).
      $revision = $ENV{ACL_QSH_REVISION};
    }
    my $current_compile = acl::File::mybasename($location);
    my $cmd = "cd $location_dir;quartus_cdb -t $script_abs_path $atom $current_compile $base $project $revision;cd $work_dir";
    print "$prog: Extracting $atom locations from '$location' compile directory (from '$revision' revision)\n";
    my $locationoutput_full = `$cmd`;

    # Error out if project cannot be opened
    (my $locationoutput_projecterror) = $locationoutput_full =~ /(Error\: ERROR\: Project does not exist.*)/s;
    if ($locationoutput_projecterror) {
      acl::Common::mydie("Project '$project' and revision '$revision' in directory '$location' does not exist!\n");
    }

    # Error out if atom netlist cannot be read
    (my $locationoutput_netlisterror) = $locationoutput_full =~ /(Error\: ERROR\: Cannot read atom netlist.*)/s;
    if ($locationoutput_netlisterror) {
      acl::Common::mydie("Cannot read atom netlist from revision '$revision' in directory '$location'!\n");
    }

    # Add location constraints to current Quartus compile directory
    (my $locationoutput) = $locationoutput_full =~ /(\# $atom locations.*)\# $atom locations END/s;
    my @designs = acl::File::simple_glob( "*.qsf" );
    $#designs > -1 or acl::Common::mydie ("Internal Compiler Error. $atom location argument was passed but could not find any qsf files\n");
    foreach (@designs) {
      my $qsf = $_;
      open(my $fd, ">>$qsf");
      print $fd "\n";
      print $fd $locationoutput;
      close($fd);
    }
}

sub remove_intermediate_files($$) {
   my ($dir,$exceptfile) = @_;
   my $verbose = acl::Common::get_verbose();
   my $thedir = "$dir/.";
   my $thisdir = "$dir/..";
   my %is_exception = (
      $exceptfile => 1,
      "$dir/." => 1,
      "$dir/.." => 1,
   );
   foreach my $file ( acl::File::simple_glob( "$dir/*", { all => 1 } ) ) {
      if ( $is_exception{$file} ) {
         next;
      }
      if ( $file =~ m/\.aclx$/ ) {
         next if $exceptfile eq acl::File::abs_path($file);
      }
      acl::File::remove_tree( $file, { verbose => $verbose, dry_run => 0 } )
         or acl::Common::mydie("Cannot remove intermediate files under directory $dir: $acl::File::error\n");
   }
   # If output file is outside the intermediate dir, then can remove the intermediate dir
   my $files_remain = 0;
   foreach my $file ( acl::File::simple_glob( "$dir/*", { all => 1 } ) ) {
      next if $file eq "$dir/.";
      next if $file eq "$dir/..";
      $files_remain = 1;
      last;
   }
   unless ( $files_remain ) { rmdir $dir; }
}

sub create_object {
  my ($base, $input_work_dir, $src, $obj, $bsp_variant, $board_variant, $using_default_board, $is_spv, $all_aoc_args) = @_;

  my $pkg_file_final = $obj;
  (my $src_pkg_file_final = $obj) =~ s/aoco/source/;
  my $pkg_file = acl::Common::set_package_file_name($pkg_file_final);
  my $src_pkg_file = acl::Common::set_source_package_file_name($src_pkg_file_final.".tmp");
  my $verbose = acl::Common::get_verbose();
  my $quiet_mode = acl::Common::get_quiet_mode();
  my $save_temps = acl::Common::get_save_temps();
  $fulllog = "$base.log"; #definition moved to global space

  #Create the new directory verbatim, then rewrite it to not contain spaces
  $work_dir = $input_work_dir;
  acl::File::make_path($work_dir) or acl::Common::mydie("Cannot create temporary directory $work_dir: $!");
  
  # If just packaging an HDL library component, call 'aocl library' and be done with it.
  if ($hdl_comp_pkg_flow) {
    print "$prog: Packaging HDL component for library inclusion\n" if $verbose||$report;
    $return_status = acl::Common::mysystem_full(
        {'time' => 1, 'time-label' => 'aocl library'},
        "$aocl_libedit_exe -c \"$src\" -o \"$obj\"");
    $return_status == 0 or acl::Common::mydie("Packing of HDL component FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
    # remove temp directory
    acl::File::remove_tree($work_dir) unless $save_temps;
    return $return_status;
  }

  # Make sure the board specification file exists. This is needed by multiple stages of the compile.
  my $llvm_profilerconf_option = (defined $absolute_profilerconf_file ? "-profile-config $absolute_profilerconf_file" : ""); # To be passed to LLVM executables

  my $default_text;
  if ($using_default_board) {
      $default_text = "default ";
  } else {
      $default_text = "";
  }
  print "$prog: Selected target board package $bsp_variant\n" if $verbose||$report;
  print "$prog: Selected ${default_text}target board $board_variant\n" if $verbose||$report;

  my $pkg = undef;
  my $src_pkg = undef;

  # OK, no turning back remove the result file, so no one thinks we succedded
  unlink $obj;
  unlink $src_pkg_file_final;

  my $clangout = "$base.pre.bc";
  my @cmd_list = ();

  # Create package file in source directory, and save compile options.
  $pkg = create acl::Pkg($pkg_file);

  # Figure out the compiler triple for the current flow.


  my $fpga_triple = ($create_library_flow_for_sycl || $sycl_mode) ? 'spir64-unknown-unknown-sycldevice' : 'spir64-unknown-unknown-intelfpga';
  my $emulator_triple = ($emulator_arch eq 'windows64') ? 'x86_64-pc-windows-intelfpga' : 'x86_64-unknown-linux-intelfpga';
  my $cur_flow_triple = $emulator_flow ? $emulator_triple : $fpga_triple;
  my $aux_triple = ($emulator_arch eq 'windows64') ? 'x86_64-pc-windows-msvc' : 'x86_64-unknown-linux-gnu';

  my @triple_list;

  # Bail for spv cases we can't handle.
  acl::Common::mydie("-shared not supported for SPIR-V files.\n") if $is_spv && $created_shared_aoco;

  # Triple list to compute.
  if ($created_shared_aoco) {
    @triple_list = ($fpga_triple, 'x86_64-pc-windows-intelfpga', 'x86_64-unknown-linux-intelfpga');
  } elsif ($create_library_flow) {
    @triple_list = ($fpga_triple, $emulator_triple);
  } else {
    @triple_list = ($cur_flow_triple);
  }

  my @metadata_compile_unit_flag = ();
  if ($emulator_flow){
    my $suffix =$src;
    $suffix =~ s/.*\.//;
    my $outbase = $src;
    $outbase =~ s/\.$suffix//;
    push (@metadata_compile_unit_flag, "-main-file-name");
    my $just_file_name = substr $outbase, rindex($outbase, '/') +1 ;
    $just_file_name .='_metadata';
    push @metadata_compile_unit_flag, $just_file_name;
  }

  # clang args that should apply to all compile flows.
  my @clang_common_args = ();
  if ($orig_force_initial_dir ne '') {
    # When the -initial-dir argument is used we need to ensure
    # relative include paths can be resolved relative to $orig_force_initial_dir
    # instead of the location of the .cl file.
    push(@clang_common_args, "-I$orig_force_initial_dir");
  }

  push(@clang_common_args,'-Wuninitialized');

  # clang args that should apply to only the PSG flows: FPGA, legacy emulator, etc.
  # i.e. NOT the fast emulator.
  my @clang_psg_args = ();
  push(@clang_psg_args,'-Wno-ivdep-usage');

  my $dep_file = "$work_dir/$base.d";
  my $is_embedded_cpp = ($src =~ /\.cpp$/);
  my $acl_version_string = 20.3 * 10;
  push(@clang_common_args, ($is_embedded_cpp ? "-D__INTELFPGA_COMPILER__=":"-DINTELFPGA_CL=").$acl_version_string);
  my @clang_std_opts = qw(-cc1 -emit-llvm-bc -O3 -disable-llvm-passes);
  # Compile with -cl-std=CL1.2 by default. The user can still override this on the commandline by passing
  # -cl-std=<ver> to aoc since @user_clang_args are added later and the last value takes effect.
  my @lang_params = $is_embedded_cpp ? qw(-x c++ --std=c++17 -fhls -D_HLS_EMBEDDED_PROFILE) : qw(-x cl -cl-std=CL1.2);
  push(@clang_std_opts, @lang_params);
  # -dwarf-column-info is needed for cluster attributes to differentiate calls on the same line.
  push(@clang_std_opts, '-dwarf-column-info') if $dash_g;

  # Add Vfs lib used for decryption
  my @clang_decryption_vfs = "";
  if (acl::Env::is_linux()) {
    @clang_decryption_vfs = "-ivfsoverlay-lib".acl::Env::sdk_root()."/linux64/lib/libaoc_clang_decrypt.so";
  }
  my @clang_arg_after_array = split(/\s+/m,$clang_arg_after);
  my @debug_options = ( $debug ? qw(-mllvm -debug) : ());

  my @clang_cc1_msvc_args = ();
  push(@clang_cc1_msvc_args, '-fms-extensions');
  push(@clang_cc1_msvc_args, '-fms-compatibility');
  push(@clang_cc1_msvc_args, '-fms-compatibility-version=191025017'); # VS2017 v15.0
  push(@clang_cc1_msvc_args, '-fdelayed-template-parsing');
  
  my @clang_cc1_dependency_args = ('-MT', "$base.bc", '-sys-header-deps', '-dependency-file', $dep_file);
  my @clang_E_dependency_args = ('-Wp,'.join(',',@clang_cc1_dependency_args));
  if ( not $emulator_fast) {
    acl::AOCDriverCommon::progress_message($is_spv ? "$prog: Processing SPIR-V....\n" : "$prog: Running OpenCL parser....\n");
    chdir $force_initial_dir or acl::Common::mydie("Cannot change into dir $force_initial_dir: $!\n");

    # Emulated flows to cover
    my @emu_list = ($created_shared_aoco or $create_library_flow) ? (0, 1) : $emulator_flow;

    # These two nested loops should produce either one clang call for regular compiles
    # Or three clang calls for three triples if -shared was specified:
    #     (non-emulated, fpga), (emulated, linux), (emulated, windows)
    foreach my $emu_flow (@emu_list) {
      foreach my $cur_triple (@triple_list) {

        # Skip {emulated_flow, triple} combinations that don't make sense
        if ($emu_flow and ($cur_triple =~ /spir/)) { next; }
        if (not $emu_flow and ($cur_triple !~ /spir/)) { next; }
        my $cur_clangout;
        if ($cur_triple eq $cur_flow_triple) {
          $cur_clangout = "$work_dir/$base.pre.bc";
        } else {
          $cur_clangout = "$work_dir/$base.pre." . $cur_triple . ".bc";
        }

        my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
        my $board_spec_xml = acl::AOCDriverCommon::find_board_spec($acl_board_hw_path);
        my $llvm_board_option = "-board \"$board_spec_xml\"";   # To be passed to LLVM executables.
        my @board_options = map { ('-mllvm', $_) } split( /\s+/, $llvm_board_option );
        my @board_def = (
            "-DACL_BOARD_$board_variant=1", # Keep this around for backward compatibility
            "-DAOCL_BOARD_$board_variant=1",
            );
        my @clang_dependency_args = ( ($cur_triple eq $cur_flow_triple) ? @clang_cc1_dependency_args  : ());

        if ($is_spv) {
          my $rewrite_file = "$work_dir/spirv.fpga.bc";
          @cmd_list = (
              $spirv_exe,
              '-r',
              $src,
              '-o',
              $rewrite_file,
              );
          $return_status = acl::Common::mysystem_full(
              { 'stdout' => "$work_dir/spirv.log",
                'stderr' => "$work_dir/spirv.err",
                'time' => 1,
                'time-label' => 'spirv'},
              @cmd_list);

          my $banner = '!========== [spirv conversion] ==========';
          acl::Common::move_to_log($banner, "$work_dir/spirv.log", "$work_dir/$fulllog");
          acl::Report::append_to_log("$work_dir/spirv.err", "$work_dir/$fulllog");
          acl::Report::append_to_err("$work_dir/spirv.err");
          if ($return_status != 0 and $regtest_mode) {
            acl::Common::move_to_log($banner, "$work_dir/spirv.err", $regtest_errlog);
          } else {
            unlink "$work_dir/spirv.err";
          }

          if ($return_status == 0) {
            # We need to fixup the SYCL IR.
            $return_status = acl::AOCOpenCLStage::rewrite_sycl_ir($rewrite_file, $cur_clangout,
                                                                  $llvm_board_option);
          }

          # Copy over the dependency file (should have been generated by running
          # clang with -MMD).
          (my $dep_file_src = $src) =~ s/\.[^.]*$/.d/;
          if (-e $dep_file_src) {
            acl::File::copy($dep_file_src, $dep_file);
          } elsif (scalar(@all_dep_files) == 0 and $sycl_mode) {
            print "$prog: Warning: Cannot find dependency file \"";
            print "$dep_file_src\" for source file \"$src\". Source code will ";
            print "not be available in the HLD Reports. Ensure you ran dpcpp ";
            print "with the -fintelfpga flag.\n";
          }

          $return_status == 0 or acl::Common::mydie("SPIRV to LLVM IR FAILED");
        } else {
          my $preprocessed_src = $src.'.'.$$.'.i';
          if ($is_embedded_cpp) {
            ##Preprocess with native target to find headers
            @cmd_list = (
                $clang_exe,
                "-E", "-std=c++17", "-fno-exceptions",
                ($cur_triple eq $fpga_triple) ? "-fno-rtti" : "",
                $preproc_gcc_toolchain ? '--gcc-toolchain='.$preproc_gcc_toolchain : "",
                "-D__INTELFPGA_TYPE__=".($emu_flow ? "NONE" : "VERILOG"),
                "-D_HLS_EMBEDDED_PROFILE",
                ($emu_flow ? "-DHLS_X86" : "-DHLS_SYNTHESIS"),
                @debug_options,
                $src,
                @clang_arg_after_array,
                ("-I" . $ENV{'INTELFPGAOCLSDKROOT'} . "/include"),
                '-o', $preprocessed_src,
                grep (!/-dwarf-version|-debug-info-kind/,@user_clang_args),
                @metadata_compile_unit_flag,
                @clang_common_args,
                @clang_psg_args,
                @clang_E_dependency_args,
                @clang_decryption_vfs
                );
            $return_status = acl::Common::mysystem_full(
                { 'time' => 1,
                  'time-label' => 'clang pre-process'},
                @cmd_list);
          }
          ## Actual parse, possibly from preprocessed source
          @cmd_list = (
              $clang_exe,
              @clang_std_opts,
              ($cur_triple eq $fpga_triple) ? "-fno-rtti" : "",
              $is_embedded_cpp ? "-D__INTELFPGA_TYPE__=".($emu_flow ? "NONE" : "VERILOG"):"",
              (!$is_embedded_cpp && $emu_flow) ? "-D__FPGA_EMULATION_X86__" : "",
              $is_embedded_cpp ? ($emu_flow ? "-DHLS_X86" : "-DHLS_SYNTHESIS") : "",
              ('-triple', $cur_triple),
              ('-aux-triple', $aux_triple),
              @debug_options,
              ($is_embedded_cpp ? $preprocessed_src : $src),
              @clang_arg_after_array,
              !$is_embedded_cpp ? ('-include', $ocl_header):(),
              $is_embedded_cpp ? ("-I" . $ENV{'INTELFPGAOCLSDKROOT'} . "/include"):(),
              '-o',
              $cur_clangout,
              !$is_embedded_cpp ? @clang_dependency_args : (),
              @user_clang_args,
              @metadata_compile_unit_flag,
              @clang_common_args,
              @clang_psg_args,
              @clang_decryption_vfs,
              (acl::Env::is_windows() && $is_embedded_cpp) ? @clang_cc1_msvc_args : ""
              );
          $return_status = acl::Common::mysystem_full(
              { 'stdout' => "$work_dir/clang.log",
                'stderr' => "$work_dir/clang.err",
                'time' => 1,
                'time-label' => 'clang'},
              @cmd_list);
            unlink $preprocessed_src if $is_embedded_cpp && !$save_temps;

          # Only save warnings and errors corresponding to current flow triple.
          # Otherwise, will get all warnings in triplicate.
          my $banner = '!========== [clang] parse ==========';
          if (($cur_triple eq $cur_flow_triple) or ($return_status != 0)) {
            acl::Common::move_to_log($banner, "$work_dir/clang.log", "$work_dir/$fulllog");
            acl::Report::append_to_log("$work_dir/clang.err", "$work_dir/$fulllog");
            acl::Report::append_to_err("$work_dir/clang.err");
            if ($return_status != 0 and $regtest_mode) {
              acl::Common::move_to_log($banner, "$work_dir/clang.err", $regtest_errlog);
            } else {
              unlink "$work_dir/clang.err";
            }
          }

          $return_status == 0 or acl::Common::mydie("OpenCL parser FAILED");
        }

        # Save clang/spirv output to .aoco file. This will be used for creating
        # a library out of this file.
        # ".acl.clang_ir" section prefix name is also hard-coded into lib/libedit/inc/libedit.h!
        $pkg->set_file(".acl.clang_ir.$cur_triple", $cur_clangout)
             or acl::Common::mydie("Cannot save compiler object file $cur_clangout into package file: $acl::Pkg::error\n");
      }
    }
  }

  if ( $emulator_fast or ($create_library_flow and !$library_skip_fastemulator_gen) ) {
    my $spv_output_tmp = $work_dir."/".$base.".spv.tmp.obj";
    my $ioc_output = $work_dir."/".$base.".ioc.obj";
    my $log_file = $is_embedded_cpp ? "$work_dir/clang.log" : "$work_dir/ioc.log";
    my $err_file = $is_embedded_cpp ? "$work_dir/clang.err" : "$work_dir/ioc.err";

    if ($is_spv) {
      my $spirv_output = "$work_dir/$base.bc";
      @cmd_list = (
        $spirv_exe,
        '-r',
        $src,
        '-o',
        $spv_output_tmp);

      $return_status = acl::Common::mysystem_full(
          { 'stdout' => $log_file,
            'stderr' => $err_file,
            'time' => 1,
            'time-label' => $spirv_exe},
          @cmd_list);

      acl::Report::append_to_err($err_file);
      if ($return_status==0 or $regtest_mode==0) { unlink $err_file; }

      if ($return_status == 0) {
        # We need to fixup the SYCL IR.
        my $llvm_board_option = "";
        if (defined($bsp_variant)) {
          my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
          my $board_spec_xml = acl::AOCDriverCommon::find_board_spec($acl_board_hw_path);
          $llvm_board_option = "-board $board_spec_xml";   # To be passed to LLVM executables.
        }
        $return_status = acl::AOCOpenCLStage::rewrite_sycl_ir($spv_output_tmp, $ioc_output,
                                                              $llvm_board_option);
      }

      if ($return_status != 0 and $regtest_mode and -s $err_file) {
        acl::Common::move_to_log("!========== Fast Emulator - $spirv_exe ==========", $err_file, $regtest_errlog);
      }
    } elsif ($is_embedded_cpp) {
      # ioc64 cannot consume c++ source, we need to call clang directly for now
      # We also need to resolve system headers separated from the SPIR target
      # So we are adding a preprocessing step here as well.
      my $preprocessed_src = $src.'.'.$$.'.i';
      @cmd_list = (
          $clang_exe,
          "-E", "-std=c++17", "-fno-rtti", "-fno-exceptions",
          $preproc_gcc_toolchain ? '--gcc-toolchain='.$preproc_gcc_toolchain : "",
          "-D__INTELFPGA_TYPE__=NONE",
          "-DHLS_X86",
          "-D_HLS_EMBEDDED_PROFILE",
          @debug_options,
          $src,
          @clang_arg_after_array,
          ("-I" . $ENV{'INTELFPGAOCLSDKROOT'} . "/include"),
          '-o', $preprocessed_src,
          grep (!/-dwarf-version|-debug-info-kind/,@user_clang_args),
          @metadata_compile_unit_flag,
          @clang_common_args,
          @clang_E_dependency_args,
          @clang_decryption_vfs
          );
      $return_status = acl::Common::mysystem_full(
          { 'time' => 1,
            'time-label' => 'clang pre-process'},
          @cmd_list);
      
      @cmd_list = (
          $clang_exe,
          @clang_std_opts,
          "-fno-rtti",
          "-D__INTELFPGA_TYPE__=NONE",
          "-DHLS_X86",
          ('-triple',$fpga_triple),
          ('-aux-triple', $aux_triple),
          @debug_options,
          $preprocessed_src,
          @clang_arg_after_array,
          ("-I" . $ENV{'INTELFPGAOCLSDKROOT'} . "/include"),
          '-o',
          $ioc_output,
          @user_clang_args,
          @metadata_compile_unit_flag,
          @clang_common_args,
          @clang_decryption_vfs,
          acl::Env::is_windows() ? @clang_cc1_msvc_args : ""
          );
      $return_status = acl::Common::mysystem_full(
          { 'stdout' => $log_file,
            'stderr' => $err_file,
            'time' => 1,
            'time-label' => 'clang'},
          @cmd_list);
        unlink $preprocessed_src if !$save_temps;
    } else {
      my $ioc_cmd = "-cmd=compile";
      my $ioc_dev = "-device=fpga_fast_emu";
      my $ioc_inp = "-input=$src";
      my $ioc_out = "-ir=$ioc_output";

      my $ioc_opt;

      my @fast_emu_clang_args = ();

      # Debug Arguments
      # Add additional arguments to tell the compiler which path and filename
      # to use for debug info. Must be an absolute path.  For more info see:
      # https://software.intel.com/en-us/openclsdk-devguide-enabling-debugging-in-opencl-runtime
      # These arguments should NOT be passed directly to clang; these are ioc64
      # arguments.
      # | aoc flags | ioc64 flags | Resulting Binary              |
      # | ========= | =========== | ============================= |
      # | <none>    | -g          | debug symbols, unoptimized    |
      # | -g -O3    | -g -O3      | debug symbols, optimized      |
      # | -O0       | -g -O0      | debug symbols, unoptimized    |
      # | -g/-ggdb  | -g          | debug symbols, unoptimized    |
      # | -g0       | -O3         | no debug symbols, optimized   |
      # | -g0 -O0   | -O0         | no debug symbols, unoptimized |
      my $dbg_args = acl::Env::is_windows() ? "-s \\\"$src\\\"" : "-s \"$src\"";
      if (acl::Env::is_windows()) {
        if ($user_dash_g) {
          # This sometimes crashes at runtime with a segfault. TODO: open JIRA on DSE
          print "Warning: Use of debug symbols in the emulator flow on Windows is a beta feature.\n".
                "If you encounter issues, please disable debug by removing any '-g' arguments\n".
                "from your compile command.\n";
          $dbg_args = '-g -cl-opt-disable '.$dbg_args;
        }
      } else {
        $dbg_args = '-g '.$dbg_args if ($dash_g);
      }
      # This one goes to clang
      push(@fast_emu_clang_args, '-O3') unless $dash_g;

      # Pass -O3 if explicitly specified (other optimization levels handled in
      # @user_clang_args).
      push(@fast_emu_clang_args, '-O3') if $emu_optimize_o3;

      # Additional args to the fast emulator clang can be passed following a
      # -Xclang flag.
      push(@fast_emu_clang_args, @clang_common_args);

      # Add decryption library overlay if linux
      if (not acl::Env::is_windows()) {
        push (@fast_emu_clang_args, @clang_decryption_vfs);
      }

      push(@fast_emu_clang_args, grep (!/-dwarf-version|-debug-info-kind/, @user_clang_args));

      # All build options, except those used for debugging, should be prefixed
      # by '-Xclang' so that they get passed through to clang.
      @fast_emu_clang_args = map { "-Xclang ".$_} @fast_emu_clang_args;

      # Linux and Windows require slightly different quotes
      # Compile with -cl-std=CL1.2 by default. The user can still override this on the commandline by passing
      # -cl-std=<ver> to aoc since @user_clang_args are added later and the last value takes effect.
      if (acl::Env::is_windows()) {
        $ioc_opt = "-bo=\"-cl-std=CL1.2 $dbg_args ".join(' ', @fast_emu_clang_args).'"';
      } else {
        $ioc_opt = "-bo=-cl-std=CL1.2 $dbg_args ".join(' ', @fast_emu_clang_args);
      }

      @cmd_list = (
          $ioc_exe,
          $ioc_cmd,
          $ioc_dev,
          $ioc_opt,
          $ioc_inp,
          $ioc_out,
          );

      # For some reason the fast emulator won't compile a file in a subdirectory
      # if the current path contains a symlink.  Setting $PWD to the dirname of
      # the file to be compiled seems to work.  Don't ask me why.
      my $orig_pwd = $ENV{PWD};
      $ENV{PWD} = acl::File::mydirname($src) if (acl::Env::is_linux());

      $return_status = acl::Common::mysystem_full(
          { 'stdout' => $log_file,
            'stderr' => $err_file,
            'time' => 1,
            'time-label' => 'ioc'},
          @cmd_list);

      # Reset $PWD.
      $ENV{PWD} = $orig_pwd if (acl::Env::is_linux());
    }

    acl::Report::append_to_err($err_file);
    if ($return_status==0 or $regtest_mode==0) { unlink $err_file; }

    if ($return_status != 0 and $regtest_mode and -s $err_file) {
      acl::Common::move_to_log('!========== Fast Emulator - ioc ==========', $err_file, $regtest_errlog);
    }

    my $compile_failed = print_fast_emulator_messages($log_file, $return_status);

    acl::Common::mydie("OpenCL kernel compilation FAILED") if ($compile_failed);
    unlink $log_file unless acl::Common::get_save_temps();

    # Save output to .aoco file.
    my $section_name;
    if ($is_embedded_cpp) {
      $section_name = ".acl.ioc_bc";
    } else {
      $section_name =".acl.ioc_obj";
    }
    $pkg->set_file($section_name, $ioc_output)
      or acl::Common::mydie("Cannot save compiler object file $ioc_output into package file: $acl::Pkg::error\n");
  }

  my $done_parse_spv_msg = $is_spv ? "$prog: SPIR-V processing completed\n" : "$prog: OpenCL parser completed \n";
  if ( $parse_only ) {
    unlink $pkg_file;
    print $done_parse_spv_msg if (!$quiet_mode);
    return;
  }

  if ( defined $program_hash ){
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.hash',$program_hash);
  }
  if ($create_library_flow) {
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.target','library');
  } elsif ($emulator_flow) {
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board',$emulatorDevice);
    if ($emulator_fast) {
      acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.target','emulator_fast');
    } else {
      acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.target','emulator');
    }
  } elsif ($new_sim_mode) {
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board',"SimulatorDevice");
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.simulator_object',"");
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.target','simulator');
  } else {
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board',$board_variant);
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.target','fpga');
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board_package',acl::Board_env::get_board_path());
  }
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.compileoptions',join(' ',@user_opencl_args));

  # Set version of the compiler, for informational use.
  # It will be set again when we actually produce executable contents.
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.version',acl::Env::sdk_version());

  if ($emulator_fast) {
    acl::AOCDriverCommon::progress_message("$prog: OpenCL kernel compilation completed successfully.\n");
  } else {
    # pacakge clangout and .d files into package
    $pkg->add_file('.acl.aoco',"$work_dir/$clangout");
    if (-e $dep_file) {
      $pkg->add_file('.acl.dep',$dep_file);
    }
    $pkg->add_file('.acl.clang_log',"$work_dir/$fulllog");

    acl::AOCDriverCommon::progress_message("$done_parse_spv_msg");

    my $ll_file = $obj;
    $ll_file =~ s/.aoco/.pre.ll/g;
    # Clang already generates LLVM IR as text.
    acl::File::copy("$work_dir/$clangout", $ll_file) if $disassemble;
  }

  # remove temp directory
  acl::File::remove_tree($work_dir) unless $save_temps;
}

sub compile_design {
  my ($base,$final_work_dir,$obj,$x_file,$bsp_variant,$board_variant,$all_aoc_args,$bsp_flow_name) = @_;
  $fulllog = "$base.log"; #definition moved to global space
  my $pkgo_file = $obj; # Should have been created by first phase.
  my $x_file_base = acl::File::mybasename($x_file);
  my $pkg_file_final = $output_file || acl::File::abs_path("$x_file_base");
  my $pkg_file = acl::Common::set_package_file_name($pkg_file_final.".tmp");
  my $verbose = acl::Common::get_verbose();
  my $quiet_mode = acl::Common::get_quiet_mode();
  my $save_temps = acl::Common::get_save_temps();
  # copy partition file if it exists
  acl::File::copy( $save_partition_file, $work_dir."/saved_partitions.txt" ) if $save_partition_file ne '';
  acl::File::copy( $set_partition_file, $work_dir."/set_partitions.txt" ) if $set_partition_file ne '';

  # backup the workdir if redirect is needed
  my %new_dirs_to_backup;
  if ($change_output_folder_name) {
    $new_dirs_to_backup{"$final_work_dir"} = "$change_output_folder_name";
  }
  acl::Common::update_dirs_to_update(\%new_dirs_to_backup);

  # reuse-exe support
  my $aocx_temp_file = undef;
  if (defined $reuse_exe_file) {
    if (-f $reuse_exe_file) {
      print "Extracting aocx from exe '$reuse_exe_file' ...\n" if $verbose;
      $aocx_temp_file = acl::File::mktemp();
      my $cmd = acl::Env::sdk_extract_aocx_exe() . " -i $reuse_exe_file -o $aocx_temp_file";
      if (acl::AOCDriverCommon::mysystem($cmd) == 0) {
        # An aocx was extracted from the executable.
        print "Extraction succeeded\n" if $verbose;
        $reuse_aocx_file = $aocx_temp_file ;
      } else {
        print "Extraction failed\n" if $verbose;
      }
    }
  }

  # Is our RTL the same as the existing aocx? If so, we don't need to recompile, assuming all else
  # is the same.
  my $old_pkg = defined $reuse_aocx_file ? get acl::Pkg($reuse_aocx_file) : undef;
  if ($old_pkg && $old_pkg->exists_section('.acl.rtl_hash')) {
    # Old package exists with an RTL hash
    print "Checking previous aocx ($reuse_aocx_file) and current aocr ($pkgo_file) for equality\n" if $verbose;

    acl::AOCDriverCommon::progress_message("Checking to see if existing aocx was compiled with the same kernels and arguments\n");
    my $new_pkg = get acl::Pkg($pkgo_file)
       or acl::Common::mydie("Cannot find package file: $acl::Pkg::error\n");
    if ($old_pkg->exists_section('.acl.simulator_object') == $new_pkg->exists_section('.acl.simulator_object')) {
      # We have the same type of compilation
      my $temp_file = acl::File::mktemp();
      my @sections = qw(.acl.rtl_hash .acl.kernel_arg_info.xml .acl.board .acl.quartus_input_hash
                        .acl.version .acl.target .acl.autodiscovery);
      my $all_same = 1;
      for my $section_name (@sections) {
        if (!acl::Incremental::sections_same($old_pkg, $new_pkg, $section_name, $temp_file)) {
          print "Section $section_name differs.\n" if $verbose;
          if ($section_name eq '.acl.rtl_hash') {
            my $old = acl::Incremental::extract_section($old_pkg, $section_name, $temp_file);
            my $new = acl::Incremental::extract_section($new_pkg, $section_name, $temp_file);
            print acl::Incremental::kernel_difference_msg($old, $new, $temp_file);
          } else {
            print "The previous compile was not identical to the current one.\n" if !$quiet_mode;
          }
          $all_same = 0;
          last;
        }
      }
      if ($all_same && !acl::Incremental::arguments_same($old_pkg, $new_pkg, $temp_file)) {
        print "Arguments to aoc command differ from previous compile\n" if !$quiet_mode;
        $all_same = 0;
      }
      if ($all_same) {
        # We are the same.  Just use the old version.
        acl::AOCDriverCommon::progress_message("Previous compilation identical to current compilation: re-using $reuse_aocx_file for $pkg_file_final.\n");
        if ($reuse_aocx_file eq $pkg_file_final) {
          # Touch the file to update the modification time.
          my $current_time = time;
          utime $current_time, $current_time, $pkg_file_final;
        } else {
          # copy the previous aocx to the desired output file.
          unlink $pkg_file_final;
          acl::File::copy($reuse_aocx_file, $pkg_file_final) || die "Unable to copy $reuse_aocx_file to $pkg_file_final\n";
        }
        unlink $pkgo_file if !$save_temps;
        return;
      } elsif ($sycl_mode) {
        print "Unable to reuse previous FPGA binary.\n";
      }
    }
  }

  # Don't leave the extracted aocx file around.
  unlink $aocx_temp_file if defined $aocx_temp_file && !$save_temps;

  #Create the new directory verbatim, then rewrite it to not contain spaces
  $work_dir = $final_work_dir;
  # Get the absolute work dir without the base use by simulation temporary folder
  my $work_dir_no_base = acl::File::mydirname($work_dir);

  # Check if pkgo_file for simulation or emulation
  my $pkgo = get acl::Pkg($pkgo_file)
     or acl::Common::mydie("Cannot find package file: $acl::Pkg::error\n");
  my $simulator = $pkgo->exists_section('.acl.simulator_object');
  if ($simulator && $skip_qsys) {
    # Invoke QSys to create testbench into a temp folder before changing dir
    $new_sim_mode = 1;
    acl::Simulator::opencl_create_sim_system($bsp_variant, $board_variant, 1, $work_dir_no_base, $work_dir."/".$base.".bc.xml",$fulllog, $work_dir);
  }

  # If using the fast emulator, just extract the emulator binary and be done with it.
  if ($pkgo->exists_section('.acl.fast_emulator_object.linux') ||
      $pkgo->exists_section('.acl.fast_emulator_object.windows')) {
      unlink $pkg_file_final;
    if ($pkgo->exists_section('.acl.fast_emulator_object.linux')) {
      $pkgo->get_file('.acl.fast_emulator_object.linux',$pkg_file_final);
    } elsif ($pkgo->exists_section('.acl.fast_emulator_object.windows')) {
      $pkgo->get_file('.acl.fast_emulator_object.windows',$pkg_file_final);
    }
    print "Emulator flow is successful.\n" if $verbose;
    print "To execute emulated kernel, ensure host code selects the Intel(R) FPGA OpenCL\nemulator platform.\n" if $verbose;

    return;
  }

  # To support relative BSP paths, access this before changing dir
  my $postqsys_script = acl::Env::board_post_qsys_script();

  chdir $work_dir or acl::Common::mydie("Cannot change dir into $work_dir: $!");

  acl::File::copy( $pkgo_file, $pkg_file )
   or acl::Common::mydie("Cannot copy binary package file $pkgo_file to $pkg_file: $acl::File::error");
  my $pkg = get acl::Pkg($pkg_file)
     or acl::Common::mydie("Cannot find package file: $acl::Pkg::error\n");

  #Remember the reason we are here, cannot query pkg_file after rename
  my $emulator = $pkg->exists_section('.acl.emulator_object.linux') ||
      $pkg->exists_section('.acl.emulator_object.windows');

  if(!$emulator && !$simulator){
    $board_variant = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.board');
  }

  my $block_migrations_csv = join(',', @blocked_migrations);
  my $add_migrations_csv = join(',', @additional_migrations);
  if ( ! $no_automigrate && ! $emulator) {
    acl::Board_migrate::migrate_platform_preqsys($bsp_flow_name,$add_migrations_csv,$block_migrations_csv);
  }

  # Set version again, for informational purposes.
  # Do it again, because the second half flow is more authoritative
  # about the executable contents of the package file.
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.version',acl::Env::sdk_version());

  if ($emulator) {
    unlink( $pkg_file_final ) if -f $pkg_file_final;
    rename( $pkg_file, $pkg_file_final )
      or acl::Common::mydie("Cannot rename $pkg_file to $pkg_file_final: $!");

    print "Emulator flow is successful.\n" if $verbose;
    print "To execute emulated kernel, invoke host with \n\tenv CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 <host_program>\n For multi device emulations replace the 1 with the number of devices you wish to emulate\n" if $verbose;

    return;
  }

  # print the message to indicate long processing time
  if ($new_sim_mode) {
    print "$prog: Compiling for Simulator.\n" if (!$quiet_mode);
  } else {
    my $doc_name;
          if ($sycl_mode) {
                  $doc_name = "oneAPI FPGA Optimization Guide";
          } else {
                  $doc_name = "Intel FPGA SDK for OpenCL Best Practices Guide";
          }
          print "$prog: Compiling for FPGA. This process may take several hours to complete.  Prior to performing this compile, be sure to check the reports to ensure the design will meet your performance targets.  If the reports indicate performance targets are not being met, code edits may be required.  Please refer to the $doc_name for information on performance tuning applications for FPGAs.\n" if (!$quiet_mode);
  }

  if ( ! $skip_qsys) {
    #Ignore SOPC Builder's return value
    my $sopc_builder_cmd = "qsys-script";
    my $ip_gen_cmd = 'qsys-generate';

    # Make sure both qsys-script and ip-generate are on the command line
    my $qsys_location = acl::File::which_full ("qsys-script"); chomp $qsys_location;
    if ( not defined $qsys_location ) {
       acl::Common::mydie ("Error: qsys-script executable not found!\n".
              "Add quartus bin directory to the front of the PATH to solve this problem.\n");
    }
    my $ip_gen_location = acl::File::which_full ("ip-generate"); chomp $ip_gen_location;

    # Run Java Runtime Engine with max heap size 512MB, and serial garbage collection.
    my $jre_tweaks = "-Xmx512M -XX:+UseSerialGC";

    my $windows_longpath_flag = 0;
    open LOG, "<sopc.tmp";
    while (my $line = <LOG>) {
      if ($line =~ /Error\s*(?:\(\d+\))?:/) {
        print $line;
        # Is this a windows long-path issue?
        $windows_longpath_flag = 1 if acl::AOCDriverCommon::win_longpath_error_quartus($line);
      }
    }
    print $win_longpath_suggest if ($windows_longpath_flag and acl::Env::is_windows());
    close LOG;

    # Parse the board spec for information on how the system is built
    my $version = ::acl::Env::aocl_boardspec( ".", "version".$bsp_flow_name);
    my $generic_kernel = ::acl::Env::aocl_boardspec( ".", "generic_kernel".$bsp_flow_name);
    my $qsys_file = ::acl::Env::aocl_boardspec( ".", "qsys_file".$bsp_flow_name);
    my $project = ::acl::Env::aocl_boardspec( ".", "project".$bsp_flow_name);
    ( $version.$generic_kernel.$qsys_file.$project !~ /error/i ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n" );
    # Save the true project name for when we query the DEVICE from the Quartus project
    my $project_for_device = $project;

    # Simulation flow overrides
    if($new_sim_mode) {
      $project = "none";
      $generic_kernel = 1;
      $qsys_file = "none";
      $postqsys_script = "";
    }

    # Handle the new Qsys requirement for a --quartus-project flag from 16.0 -> 16.1
    my $qsys_quartus_project = ( $QUARTUS_VERSION =~ m/Version 16\.0/ ) ? "" : "--quartus-project=$project";

    # Build the kernel Qsys system
    my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);

    # Skip Qsys (kernel_system.tcl => kernel_system.qsys)
    my $skip_kernel_system_qsys = ((!$new_sim_mode) and ($qsys_file eq "none") and ($bsp_version >= 18.0)) ? 1 : 0;

    if(!$skip_kernel_system_qsys) {
      if ($generic_kernel or ($version eq "0.9" and -e "base.qsf"))
      {
        $return_status = acl::Common::mysystem_full(
          {'time' => 1, 'time-label' => 'sopc builder', 'stdout' => 'sopc.tmp', 'stderr' => '&STDOUT'},
          "$sopc_builder_cmd $qsys_quartus_project --script=kernel_system.tcl $jre_tweaks" );
        acl::Common::move_to_log("!========== Qsys kernel_system script ==========", "sopc.tmp", $fulllog);
        $return_status == 0 or  acl::Common::mydie("Qsys-script FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
        if (!($qsys_file eq "none"))
        {
          $return_status =acl::Common::mysystem_full(
            {'time' => 1, 'time-label' => 'sopc builder', 'stdout' => 'sopc.tmp', 'stderr' => '&STDOUT'},
            "$sopc_builder_cmd $qsys_quartus_project --script=system.tcl $jre_tweaks --system-file=$qsys_file" );
          acl::Common::move_to_log("!========== Qsys system script ==========", "sopc.tmp", $fulllog);
          $return_status == 0 or  acl::Common::mydie("Qsys-script FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
        }
      } else {
        $return_status = acl::Common::mysystem_full(
          {'time' => 1, 'time-label' => 'sopc builder', 'stdout' => 'sopc.tmp', 'stderr' => '&STDOUT'},
          "$sopc_builder_cmd $qsys_quartus_project --script=system.tcl $jre_tweaks --system-file=$qsys_file" );
        acl::Common::move_to_log("!========== Qsys script ==========", "sopc.tmp", $fulllog);
        $return_status == 0 or  acl::Common::mydie("Qsys-script FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
      }
    }

    # Generate HDL from the Qsys system
    if ($new_sim_mode) {
      acl::Simulator::opencl_create_sim_system($bsp_variant, $board_variant, 0, $work_dir_no_base, $work_dir."/".$base.".bc.xml",$fulllog,$work_dir);
    } else {
      my $generate_cmd = ::acl::Env::aocl_boardspec( ".", "generate_cmd".$bsp_flow_name);
      ( $generate_cmd !~ /error/i ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");
      $return_status = acl::Common::mysystem_full(
        {'time' => 1, 'time-label' => 'ip generate', 'stdout' => 'ipgen.tmp', 'stderr' => '&STDOUT'},
        "$generate_cmd" );
    }
    # Sim mode checks its own output
    if (!$new_sim_mode) {
      # Check the log file for errors
      $windows_longpath_flag = 0;
      open LOG, "<ipgen.tmp";
      while (my $line = <LOG>) {
        if ($line =~ /Error\s*(?:\(\d+\))?:/) {
          print $line;
          # Is this a windows long-path issue?
          $windows_longpath_flag = 1 if acl::AOCDriverCommon::win_longpath_error_quartus($line);
        }
      }
      print $win_longpath_suggest if ($windows_longpath_flag and acl::Env::is_windows());
      close LOG;

      acl::Common::move_to_log("!========== ip-generate ==========","ipgen.tmp",$fulllog);
      $return_status == 0 or acl::Common::mydie("ip-generate FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
    }
    # Some boards may post-process qsys output
    if (defined $postqsys_script and $postqsys_script ne "") {
      acl::AOCDriverCommon::mysystem( "$postqsys_script" ) == 0 or acl::Common::mydie("Couldn't run postqsys-script for the board!\n");
    }
    print_bsp_msgs($fulllog);
  }

  # For simulation flow, compile the simulation, package it into the aocx, and then exit
  if($new_sim_mode) {
    # Generate compile and run scripts, and compile the design
    acl::Simulator::compile_opencl_simulator($prog, $fulllog, $work_dir);
    # Bundle up the simulation directory and simulation information used by MMD into aocx
    my $sim_options_filename = acl::Simulator::get_sim_options();
    my @sim_dir = acl::Simulator::get_sim_package();
    $return_status = $pkg->package('fpga-sim.bin', 'sys_description.hex', @sim_dir, $sim_options_filename);
    $return_status == 0 or acl::Common::mydie("Bundling simulation files FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
    $pkg->set_file(".acl.fpga.bin","fpga-sim.bin");
    unlink("fpga-sim.bin");
    # Remove the generated verilog.
    if (!$save_temps) {
      unlink( $sim_options_filename );
      foreach my $dir (@sim_dir) {
        acl::File::remove_tree($dir)
          or acl::Common::mydie("Cannot remove files under temporary directory $dir: $!\n");
      }
    }
    else {
      # Output repackaging script for ease of use later
      my $pkg_relative_filepath = "../$x_file_base";
      if ($output_file) {
        my $outbase = $output_file;
        if ($outbase =~ /.*\/(\S+)/) {
          $outbase = $1;
        }
        $pkg_relative_filepath = "../$outbase";
      }
      acl::Simulator::write_sim_repackage_script($pkg_relative_filepath);
    }

    # Move temporary file to final location.
    unlink( $pkg_file_final ) if -f $pkg_file_final;
    rename( $pkg_file, $pkg_file_final )
      or acl::Common::mydie("Cannot rename $pkg_file to $pkg_file_final: $!");

    print "Simulator flow is successful.\n" if $verbose;
    print "To execute simulator, invoke host with \n\tenv CL_CONTEXT_MPSIM_DEVICE_INTELFPGA=1 <host_program>\n" if $verbose;

    return;
  }

  # Override the fitter seed, if specified.
  if ( $fit_seed ) {
    my @designs = acl::File::simple_glob( "*.qsf" );
    $#designs > -1 or acl::Common::mydie ("Internal Compiler Error.  Seed argument was passed but could not find any qsf files\n");
    foreach (@designs) {
      my $qsf = $_;
      open(my $append_fh, ">>", $qsf) or acl::Common::mydie("Internal Compiler Error.  Failed adding the seed argument to qsf files\n");
      print {$append_fh} "\nset_global_assignment -name SEED $fit_seed\n";
      close( $append_fh );
    }
  }

  # Add DSP location constraints, if specified.
  if ( $dsploc ) {
    extract_atoms_from_postfit_netlist($base,$dsploc,"DSP",$bsp_flow_name);
  }

  # Add RAM location constraints, if specified.
  if ( $ramloc ) {
    extract_atoms_from_postfit_netlist($base,$ramloc,"RAM",$bsp_flow_name);
  }

  if ( $ip_gen_only ) { return; }

  # "Old --hw" starting point
  # We are not compiling from original folder because there's no reports folder
  if ( ! -d "reports" ) {
    (my $mProg = $prog) =~ s/#//g;
    my $prog_name = ($sycl_mode == 1) ? 'SYCL' : 'AOC';
    my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
    my $board_spec_xml = acl::AOCDriverCommon::find_board_spec($acl_board_hw_path);
    my $devicemodel = uc acl::Env::aocl_boardspec( "$board_spec_xml", "devicemodel");
    ($devicemodel) = $devicemodel =~ /(.*)_.*/;
    my $devicefamily = acl::AOCDriverCommon::device_get_family_no_normalization($devicemodel);
    my @json_files = acl::Report::create_reporting_tool_from_object($prog_name, ".", $base, $devicefamily, $devicemodel,
                                                                    acl::AOCDriverCommon::get_quartus_version_str(), $mProg." ".$all_aoc_args, $board_variant);
    foreach (@json_files) {
      my $json_file = $_.".json";
      my $json_path = $work_dir."/".$json_file;
      if ( -e $json_path ) {
        # Clean up files
        acl::File::remove_tree($json_path, { verbose => ($verbose>2), dry_run => 0 }) unless (acl::Common::get_save_temps());
      }
    }
  }

  my $project = ::acl::Env::aocl_boardspec( ".", "project".$bsp_flow_name);
  ( $project !~ /error/i ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");
  my @designs = acl::File::simple_glob( "$project.qpf" );
  if ($#designs<0){
    # If no $project.qpf file found, a possible reason is we are using the partial BSP in oneAPI base toolkit
    if ($sycl_mode){
      # the partial BSP's "board_spec.xml"'s synthesize cmd has been "overloaded" to print meaningful error msg. run it
      my $synthesize_cmd = ::acl::Env::aocl_boardspec( ".", "synthesize_cmd".$bsp_flow_name);
      acl::Common::mysystem_full( {'time' => 1, 'time-label' => 'Quartus compilation'}, $synthesize_cmd);
      acl::Common::mydie("Exiting.\n");
    }else{
      acl::Common::mydie ("Internal Compiler Error.  BSP specified project name $project, but $project.qpf does not exist.\n");
    }
  }
  $#designs == 0 or acl::Common::mydie ("Internal Compiler Error.\n");
  my $design = shift @designs;

  my $synthesize_cmd = ::acl::Env::aocl_boardspec( ".", "synthesize_cmd".$bsp_flow_name);
  ( $synthesize_cmd !~ /error/i ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");

  my $retry = 0;
  my $MAX_RETRIES = 3;
  if ($high_effort) {
    print "High-effort hardware generation selected, compile time may increase signficantly.\n";
  }

  do {
    if (defined $ENV{ACL_QSH_COMPILE_CMD})
    {
      # Environment variable ACL_QSH_COMPILE_CMD can be used to replace default
      # quartus compile command (internal use only).
      my $top = acl::File::mybasename($design);
      $top =~ s/\.qpf//;
      my $custom_cmd = $ENV{ACL_QSH_COMPILE_CMD};
      $custom_cmd =~ s/PROJECT/$top/;
      $custom_cmd =~ s/REVISION/$top/;
      $return_status = acl::Common::mysystem_full(
        {'time' => 1, 'time-label' => 'Quartus compilation', 'stdout' => $quartus_log},
        $custom_cmd);
    } else {
      $return_status = acl::Common::mysystem_full(
        {'time' => 1, 'time-label' => 'Quartus compilation', 'stdout' => $quartus_log, 'stderr' => 'quartuserr.tmp'},
        $synthesize_cmd);
    }

    print_bsp_msgs($quartus_log);

    if ( $return_status != 0 ) {
      if ($high_effort && acl::AOCDriverCommon::hard_routing_error($quartus_log) && $retry < $MAX_RETRIES) {
        print " kernel fitting error encountered - retrying aocx compile with attempt: #".($retry+1)."\n";
        $retry = $retry + 1;

        # Override the fitter seed, if specified.
        my @designs = acl::File::simple_glob( "*.qsf" );
        $#designs > -1 or print_quartus_errors($quartus_log, 'quartuserr.tmp' ,  0);
        my $seed = $retry * 10;
        foreach (@designs) {
          my $qsf = $_;
          if ($retry > 1) {
            # Remove the old seed setting
            open( my $read_fh, "<", $qsf ) or acl::Common::mydie("Unexpected Compiler Error, not able to generate hardware in high effort mode.");
            my @file_lines = <$read_fh>;
            close( $read_fh );

            open( my $write_fh, ">", $qsf ) or acl::Common::mydie("Unexpected Compiler Error, not able to generate hardware in high effort mode.");
            foreach my $line ( @file_lines ) {
              print {$write_fh} $line unless ( $line =~ /set_global_assignment -name SEED/ );
            }
            print {$write_fh} "set_global_assignment -name SEED $seed\n";
            close( $write_fh );
          } else {
            $return_status = acl::AOCDriverCommon::mysystem( "echo \"\nset_global_assignment -name SEED $seed\n\" >> $qsf" );
          }
        }
      } else {
        $retry = 0;
        print_quartus_errors($quartus_log, 'quartuserr.tmp', $high_effort == 0);
      }
    } else {
      $retry = 0;
    }
  } while ($retry && $retry <= $MAX_RETRIES);

  # postcompile migration
  if( ! $no_automigrate && ! $emulator ) {
    acl::Board_migrate::migrate_platform_postcompile($bsp_flow_name,$add_migrations_csv,$block_migrations_csv);
  }

  my $fpga_bin = 'fpga.bin';
  if ( -f $fpga_bin ) {
    $pkg->set_file('.acl.fpga.bin',$fpga_bin)
       or acl::Common::mydie("Cannot save FPGA configuration file $fpga_bin into package file: $acl::Pkg::error\n");

  } else { #If fpga.bin not found, package up sof and core.rbf

    # Save the SOF in the package file.
    my @sofs = (acl::File::simple_glob( "*.sof" ));
    if ( $#sofs < 0 ) {
      print "$prog: Warning: Cannot find a FPGA programming (.sof) file\n";
    } else {
      if ( $#sofs > 0 ) {
        print "$prog: Warning: Found ".(1+$#sofs)." FPGA programming files. Using the first: $sofs[0]\n";
      }
      $pkg->set_file('.acl.sof',$sofs[0])
        or acl::Common::mydie("Cannot save FPGA programming file into package file: $acl::Pkg::error\n");
    }
    # Save the RBF in the package file, if it exists.
    # Sort by name instead of leaving it random.
    # Besides, sorting will pick foo.core.rbf over foo.periph.rbf
    foreach my $rbf_type ( qw( core periph ) ) {
      my @rbfs = sort { $a cmp $b } (acl::File::simple_glob( "*.$rbf_type.rbf" ));
      if ( $#rbfs < 0 ) {
        #     print "$prog: Warning: Cannot find a FPGA core programming (.rbf) file\n";
      } else {
        if ( $#rbfs > 0 ) {
          print "$prog: Warning: Found ".(1+$#rbfs)." FPGA $rbf_type.rbf programming files. Using the first: $rbfs[0]\n";
        }
        $pkg->set_file(".acl.$rbf_type.rbf",$rbfs[0])
          or acl::Common::mydie("Cannot save FPGA $rbf_type.rbf programming file into package file: $acl::Pkg::error\n");
      }
    }
  }

  my $pll_config = 'pll_config.bin';
  if ( -f $pll_config ) {
    $pkg->set_file('.acl.pll_config',$pll_config)
       or acl::Common::mydie("Cannot save FPGA clocking configuration file $pll_config into package file: $acl::Pkg::error\n");
  }

  my $acl_quartus_report = 'acl_quartus_report.txt';
  if ( -f $acl_quartus_report ) {
    $pkg->set_file('.acl.quartus_report',$acl_quartus_report)
       or acl::Common::mydie("Cannot save Quartus report file $acl_quartus_report into package file: $acl::Pkg::error\n");

    # Retrieve the target and check if it is fpga, if it is, execute tcl script to generate the report
    my $target = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.target');
    if ($target eq 'fpga') {
      my $json_dir = "reports/lib/json";
      my $acl_quartus_json = "${json_dir}/quartus.json";

      # Returns the clock frequencies from the board for Quartus report and STA late (if applicable)
      my ($qfit_clk, $qfit_res) = acl::Report::parse_acl_quartus_report($acl_quartus_report, $prog);
      if ($fast_compile) {  # don't reload the project
        my $quartusJSON = acl::Report::create_acl_quartusJSON($qfit_clk, $qfit_res);
        acl::Report::create_json_file("quartus", $quartusJSON, $work_dir);
        my $report_data_dir = $work_dir."/reports/lib";
        acl::Report::print_json_files_to_report("${report_data_dir}/quartus_data", ["quartus"], $work_dir);
        # Move the JSON file
        acl::File::copy("quartus.json", $acl_quartus_json);
        acl::File::remove_tree("quartus.json", { verbose => ($verbose>2), dry_run => 0 }) unless (acl::Common::get_save_temps() && $prog =~ /aoc/);
      }
      elsif ($qfit_clk->{"kernel clock"} != -1) {
        my $compile_report_script = acl::Env::sdk_root()."/share/lib/tcl/quartus_compile_report.tcl";
        my $project_name = ::acl::Env::aocl_boardspec( ".", "project".$bsp_flow_name);
        my $project_rev = ::acl::Env::aocl_boardspec( ".", "revision".$bsp_flow_name);
        my $orig_dir = acl::File::cur_path();
        my @paths_to_try = ($orig_dir);
        # FIXME: Factor report generation out of adjust_pll and add to post_flow_pr
        # See case:1409889350.
        push @paths_to_try, acl::File::abs_path(acl::File::mydirname(acl::File::find_first_occurence('.', '\.qdb')));
        my $clk2x_fmax = defined($qfit_clk->{"clock2x"}) ? $qfit_clk->{"clock2x"} : -1;
        # Use | as delimiter as values can contain comma
        my $resource_used = "\"Quartus Fitter: Total Used (Entire System)|".$qfit_res->{"alm"}."|".$qfit_res->{"reg"}."|".$qfit_res->{"dsp"}."|".$qfit_res->{"ram"}."\"";
        foreach my $try_path (@paths_to_try) {
          chdir $try_path;
          my @cmd_list = ();
          (my $mProg = $prog) =~ s/#//g;
          @cmd_list = (
            "quartus_sh",
            "-t",
            $compile_report_script,
            $mProg,
            $project_name,
            $project_rev,
            $work_dir,
            "\"kernel clock|".$qfit_clk->{"clock1x"}."|".$qfit_clk->{"kernel clock fmax"}."|".$qfit_clk->{"kernel clock"}."\"",  # clock1x name, Fmax of Kernel, Fmax on clock1x, # actual Frequency
            "\"clock2x|".$clk2x_fmax."\"",   # clock2x
            $resource_used
          );
          # Execute quartus_compile_report.tcl
          my $quartus_compile_report_cmd = join(' ', @cmd_list);
          $return_status = acl::Common::mysystem_full(
              {'time' => 1, 'time-label' => 'Quartus full compile', 'stdout' => 'quartus_compile_report.log', 'stderr' => 'quartus_compile_report.log'}, $quartus_compile_report_cmd);
          last if ($return_status == 0);
          # comment out temporarily for the fix that remove the report updating by quartus during aocx compilation
          #$return_status == 0 or acl::Common::mydie("Quartus full compile: generating Quartus compile report FAILED.\nRefer to quartus_compile_report.log for details.\n");
        }
        chdir $orig_dir;

        if ( !(-f $acl_quartus_json) ) {  # Generate empty data JSON for consistent purpose to feedback to user this is not supported in GUI
          acl::Report::create_json_file("quartus", "{}", $work_dir);
        }
      }
      $pkg->set_file('.acl.quartus_json',$acl_quartus_json);  # Don't die and don't error out as this an optional reporting feature
    }
  }

  # BSP Honor check: Incremental
  acl::Common::mydie("Incremental compile was requested but this BSP did not invoke the incremental-compile API.") if ($incremental_compile && ! -f 'partitions.fit.rpt');

  # BSP Honor check: Fast
  if ($fast_compile) {
    my $revision_of_interest = ::acl::Env::aocl_boardspec(".", "revision".$bsp_flow_name);
    my $fit_report = $revision_of_interest . ".fit.rpt";
    my $fit_report_path = acl::File::find_first_occurence('.', $fit_report);
    acl::Common::mydie("Could not find fit report $fit_report under compile directory\n") if (!defined $fit_report_path);

    # Read entire file
    open (my $fit_report_fh, '<', $fit_report_path) or acl::Common::mydie("Could not open $fit_report_path for reading.");
    my @lines = <$fit_report_fh>;
    close ($fit_report_fh);

    my $is_aggressive = 0;
    foreach my $line (@lines) {
      if ($line =~ /.*Optimization Mode.*/) {
        my @parts = split(";", $line);
        for (my $i = 0; $i < scalar @parts; $i++) {
            $parts[$i] =~ s/^\s+|\s+$//g;
        }
        if ($parts[2] eq "Aggressive Compile Time") {
          $is_aggressive = 1;
          last;
        }
      }
    }
    acl::Common::mydie("Fast compile was requested but this BSP did not invoke the fast-compile API") if (not $is_aggressive);
  }

  # BSP Honor check: Empty Kernel
  if ($empty_kernel_flow) {
    my $revision_of_interest = ::acl::Env::aocl_boardspec(".", "revision".$bsp_flow_name);
    my $fit_report = $revision_of_interest . ".fit.rpt";

    # Read entire fit report
    open (my $fit_report_fh, '<', $fit_report) or acl::Common::mydie("Could not open $fit_report for reading.");
    my @fit_lines = <$fit_report_fh>;
    close ($fit_report_fh);

    # Read entired empty_kernel_partition file
    my $empty_kernel_file = "empty_kernel_partition.txt";
    open (my $empty_kernels_fh, '<', $empty_kernel_file) or acl::Common::mydie("Could not open $empty_kernel_file for reading.");
    my @empty_lines = <$empty_kernels_fh>;
    close ($empty_kernels_fh);

    my $partition_summary_match;
    while ($partition_summary_match = shift (@fit_lines)) {
      last if ($partition_summary_match =~ /.*Fitter Partition Summary.*/);
    }

    foreach my $empty_line (@empty_lines) {
      chomp $empty_line;
      my $partition_emptied = 0;
      foreach my $fit_line (@fit_lines) {
        # Fit Partition Summary has "Yes" on Empty column
        if ($fit_line =~ m/\Q$empty_line\E .* Yes /) {
          $partition_emptied = 1;
          last;
        }
      }
      acl::Common::mydie("Empty kernel compile was requested but this BSP did not invoke the empty-kernel API") if (!$partition_emptied);
    }
  }

  unlink( $pkg_file_final ) if -f $pkg_file_final;
  rename( $pkg_file, $pkg_file_final )
    or acl::Common::mydie("Cannot rename $pkg_file to $pkg_file_final: $!");

  if ((! $incremental_compile || ! -e "prev") && ! $save_temps) {
    acl::File::remove_tree("prev")
      or acl::Common::mydie("Cannot remove files under temporary directory prev: $!\n");
  }

  # Check for timing violations
  my $number_of_violations = 0;
  my $largest_violation_found = 0;
  my $worst_failure_clock;
  my $worst_failure_type;
  my $allowed_slack = $allowed_timing_slack;
  foreach my $clk_failure_file (acl::File::simple_glob( "$work_dir/*.failing_clocks.rpt" )) {
    open (RPT_FILE, "<$clk_failure_file") or acl::Common::mydie("Could not open file $clk_failure_file for read.");
    while (<RPT_FILE>) {
      if ($_ =~ m/^;\s+-(\d+(?:\.\d+)?)\s+;.*;\s+(.*)\s+;.*;\s+([a-zA-Z]*)\s+;$/) {
        my $slack = $1;
        $slack *= 1000; # turn it into ps
        my $clock = $2;
        my $failure_type = $3;
        if ($slack > $largest_violation_found){
          $largest_violation_found = $slack;
          $worst_failure_clock = $clock;
          $worst_failure_type = $failure_type;
        }
        $number_of_violations += 1;
      }
    }
    close(RPT_FILE);
  }

  if ($number_of_violations > 0 and $error_on_timing_fail != 0){
    my $timing_failure_message;
    if ($number_of_violations == 1){
      $timing_failure_message = "A timing failure has been detected in the outputted binary. There was $number_of_violations timing violation,\nwhich was a $worst_failure_type failure of ".$largest_violation_found."ps on clock:\n$worst_failure_clock\n";
    } else {
      $timing_failure_message = "A timing failure has been detected in the outputted binary. There were $number_of_violations timing violations,\nwith the worst one being a $worst_failure_type failure of ".$largest_violation_found."ps on clock:\n $worst_failure_clock\n";
    }
    $timing_failure_message .= "Consider doing one or more of the following:\n  - Recompiling with a different seed ";
    $timing_failure_message .=  $sycl_mode ? "(using -Xsseed=<S>).\n" : "(using -seed=<S>).\n";
    $timing_failure_message .= "  - Reducing the area usage of your design.\n";
    if ($user_defined_bsp) { 
      $timing_failure_message .= "  - If you are using a custom BSP, discuss the failure with your BSP vendor.\n";
    }
    # TODO: parse the failing paths file to check if the 2x clock caused the worst failure - if it did tell users to fiddle with their 2x clocks (expose a 2x control flag for this)
    # if both of these are true, error
    if ($error_on_timing_fail >= 2 and $largest_violation_found > $allowed_slack) {
      my $failed_aocx_name = "$pkg_file.timing_failed";
      rename( $pkg_file_final, $failed_aocx_name ) or acl::Common::mydie("Cannot rename $pkg_file_final to $failed_aocx_name: $!");
      my $orig_dir = acl::Common::get_original_dir();
      chdir $orig_dir or acl::Common::mydie("Cannot change back into directory $orig_dir: $!");
      remove_intermediate_files($work_dir,$pkg_file_final) if $tidy;
      acl::Common::mydie("Error: $timing_failure_message");
    } elsif ($error_on_timing_fail == 1 and $largest_violation_found > $allowed_slack) {
      print "$prog: Warning: $timing_failure_message";
    }
  }

  # check sta log for timing not met warnings
  if ($timing_slack_check){
    my $slack_violation_filename = 'slackTime_violation.txt';
    if (open(my $fh, '<', $slack_violation_filename)) {
        my $line = <$fh>;
        acl::Common::mydie("Timing Violation detected: $line\n");
    }
  }

  print "$prog: Hardware generation completed successfully.\n" if $verbose;

  my $orig_dir = acl::Common::get_original_dir();
  chdir $orig_dir or acl::Common::mydie("Cannot change back into directory $orig_dir: $!");
  remove_intermediate_files($work_dir,$pkg_file_final) if $tidy;
}

sub main {
  my $all_aoc_args="@ARGV";
  my @args = (); # regular args.
  my $dirbase = undef;
  my $using_default_board = 0;
  my $bsp_flow_name = undef;
  my $regtest_bak_cache = 0;
  my $incremental_input_dir = '';
  my $verbose = acl::Common::get_verbose();
  my $quiet_mode = acl::Common::get_quiet_mode();
  my $save_temps = acl::Common::get_save_temps();
  # simulator controls
  my $sim_accurate_memory = 0;
  my $sim_kernel_clk_frequency = undef;
  my $base = undef;

  if (!@ARGV) {
    push @ARGV, qw(-help);
  }
  # Parse Input Arguments
  acl::AOCInputParser::parse_args( \@args,
                                   \$bsp_flow_name,
                                   \$regtest_bak_cache,
                                   \$incremental_input_dir,
                                   \$verbose,
                                   \$quiet_mode,
                                   \$save_temps,
                                   \$sim_accurate_memory,
                                   \$sim_kernel_clk_frequency,
                                   @ARGV );

  acl::AOCInputParser::process_args( \@args,
                                     \$using_default_board,
                                     \$dirbase,
                                     \$base,
                                     $sim_accurate_memory,
                                     $sim_kernel_clk_frequency,
                                     $regtest_bak_cache,
                                     $verbose,
                                     $incremental_input_dir);

  if ($emulator_fast && scalar(@absolute_srcfile_list) == 1 &&
    scalar(@resolved_lib_files) == 0 && scalar(@aoco_library_list) == 0) {
    # If we have only 1 source file, and no libaries, we can directly compile to AOCX from the SPV file.
    my $abs_srcfile = $absolute_srcfile_list[0];
    my $input_base = acl::File::mybasename($abs_srcfile);
    if ($input_base =~ /\.spv$/) {
      my @cleanup_list;
      acl::AOCDriverCommon::progress_message("$prog: Compiling SPIR-V for Emulation ....\n");
      acl::AOCOpenCLStage::fast_emulator_link_build($absolute_srcfile_list[0], $output_file,
                                                    join(' ', @user_clang_args), \@cleanup_list, undef);
      print "Emulator flow is successful.\n" if acl::Common::get_verbose();
      # cleanup any temp files.
      foreach my $temp (@cleanup_list) {
        unlink $temp;
      }
      if ($time_log_filename) {
        acl::Common::close_time_log();
      }
      return;
    }
  }

  # Check that this a valid board directory by checking for a board_spec.xml
  # file in the board directory.
  if (not $run_quartus and not $emulator_fast) {
    my $board_xml = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant)."/board_spec.xml";
    if (!-f $board_xml) {
      print "Board '$board_variant' not found.\n";
      my $board_path = acl::Board_env::get_board_path();
      print "Searched in the board package at: \n  $board_path\n";
      acl::Common::list_boards();
      print "If you are using a 3rd party board, please ensure:\n";
      print "  1) The board package is installed (contact your 3rd party vendor)\n";
      print "  2) You have used -board-package=<bsp-path> to specify the path to\n";
      print "     your board package installation\n";
      acl::Common::mydie("No board_spec.xml found for board '$board_variant' (Searched for: $board_xml).");
    }
    if( !$bsp_flow_name ) {
      # if the boardspec xml version is before 17.0, then use the default
      # flow for that board, which is the first and only flow
      if( "$ENV{'ACL_DEFAULT_FLOW'}" ne '' && ::acl::Env::aocl_boardspec( "$board_xml", "version" ) >= 17.0 ) {
        $bsp_flow_name = "$ENV{'ACL_DEFAULT_FLOW'}";
      } else {
        $bsp_flow_name = ::acl::Env::aocl_boardspec("$board_xml", "defaultname");
      }
      $sysinteg_arg_after .= " --bsp-flow $bsp_flow_name";
      $bsp_flow_name = ":".$bsp_flow_name;
    }
  }

  if (not $emulator_fast) {
    # check env (bsp and version, while no quartus check) first, skip the fast emulator
    check_env($bsp_variant, $board_variant, $bsp_flow_name);
  }

  # final work directory should always be aligned with the output file
  my $final_work_dir = acl::File::abs_path("$dirbase");

  my $QUARTUS_REQUIRED = (not ($compile_step || $report_only || $parse_only || $opt_only || $verilog_gen_only));
  if ($QUARTUS_REQUIRED) {
    # quartus is required, check quartus env
    my %quartus_info = acl::AOCDriverCommon::check_quartus_env();
    if ($regtest_mode) {
      $tmp_dir .= "/$quartus_info{site}";
    }
    if ($ENV{'AOCL_TMP_DIR'} ne '') {
      print "AOCL_TMP_DIR directory was specified at $ENV{'AOCL_TMP_DIR'}.\n";
      print "Ensure Linux and Windows compiles do not share the same directory as files may be incompatible.\n";
    }
    $ENV{'AOCL_TMP_DIR'} = "$tmp_dir" if ($ENV{'AOCL_TMP_DIR'} eq '');
    print "$prog: Cached files in $ENV{'AOCL_TMP_DIR'} may be used to reduce compilation time\n" if $verbose;
  }

  if (not $run_quartus && not $aoco_to_aocr_aocx_only) {
    if(!$atleastoneflag && $verbose) {
      print "You are now compiling the full flow!!\n";
    }
    # foreach source file, we need to create an object file and object directory
    for (my $i = 0; $i <= $#absolute_srcfile_list; $i++) {
      my $abs_srcfile = $absolute_srcfile_list[$i];
      my $abs_objfile = $objfile_list[$i];
      my $input_base = acl::File::mybasename($abs_srcfile);
      # Regex: looking for first character not to be alphanumeric
      if ($input_base =~ m/^[^a-zA-Z0-9]/){
        # Quartus will fail if filename does not begin with alphanumeric character
        # Preemptively catching the issue
        acl::Common::mydie("Bad file name: $input_base Ensure file name begins with alphanumeric character");
      }
      my $input_work_dir = $abs_objfile;
      $input_work_dir =~ s/\.aoco$//;
      my $is_spv = 0;
      if ($input_base =~ /(.*)\.spv$/) {
        $input_base = $1;
        $is_spv = 1;
      } else {
        $input_base =~ s/\.cl$//;
      }
      create_object ($input_base, $input_work_dir.".$$".".temp", $abs_srcfile, $abs_objfile, $bsp_variant, $board_variant, $using_default_board, $is_spv, $all_aoc_args);
    }
  }
  my $target_aocr_file = $final_work_dir.".aocr";
  if (not ($compile_step || $parse_only) && not $aocr_to_aocx_only) {
    acl::AOCOpenCLStage::link_objects();
    acl::AOCOpenCLStage::create_system ($base, $bsp_variant, $final_work_dir, $target_aocr_file, $all_aoc_args, $bsp_flow_name, $incremental_input_dir, @absolute_srcfile_list);
    # save the work directory to the aocr file if -rtl is specified
    if ($report_only) {
      acl::AOCDriverCommon::save_work_dir($dirbase, $target_aocr_file, $prog, $incremental_input_dir);
    }
  }
  if ($QUARTUS_REQUIRED) {
    # quartus compile flow
    # try extract the dir first
    if ($use_aocr_input) {
      # the aocr file should be listed in the objfile list. Should be only one aocr file
      # override the aocr_file
      $#objfile_list ==0 or acl::Common::mydie("Too many input files, cannot extract the work directory\n");
      $target_aocr_file = $objfile_list[0];
      # extract the work dir, if the name of aocx is not the same as aocr, override the final_work_dir with the aocx name
      acl::AOCDriverCommon::extract_work_dir($dirbase, $target_aocr_file, \$final_work_dir, $board_variant);
    }
    if (not ($emulator_flow || $new_sim_mode)) { acl::AOCDriverCommon::get_board_files_for_quartus($board_variant, \$final_work_dir); }
    # compile design
    compile_design ($base, $final_work_dir, $target_aocr_file, $x_file, $bsp_variant, $board_variant, $all_aoc_args, $bsp_flow_name);
  }

  # redirect the work dir if "-output-report-folder" is specified
  if ($change_output_folder_name) {
    acl::File::movedir($final_work_dir, $change_output_folder_name, {verbose => $verbose});
    
    if (not -d $change_output_folder_name) {
      acl::Common::mydie("Failed to move the report directory to $change_output_folder_name\n");
    }
  }

  unless ($save_temps){
    acl::File::remove_tree( "$work_dir/lib", { verbose => $verbose, dry_run => 0 } )
      or acl::Common::mydie("Cannot remove intermediate files under directory $work_dir/lib: $acl::File::error\n");
  }

  # clean the generated aoco files if not save temps
  acl::AOCDriverCommon::cleanup_object_files();

  if ($time_log_filename) {
    acl::Common::close_time_log();
  }
}

main();
exit 0;
