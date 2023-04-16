
=pod

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


=head1 NAME

acl::AOCDriverCommon.pm - Common functions and glob vars used by OpenCL compiler driver

=head1 VERSION

$Header: //depot/sw/hld/rel/20.3api.11/acl/sysgen/lib/acl/AOCDriverCommon.pm#2 $

=head1 DESCRIPTION

Common functions and global vars used by OpenCL compiler driver

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


package acl::AOCDriverCommon;
use strict;
use Exporter;
use File::Spec;

require acl::Common;
require acl::Env;
require acl::Board_env;
require acl::File;
require acl::Pkg;
require acl::Report;
use acl::Report qw(escape_string);

our @ISA = qw(Exporter);
our @EXPORT_OK = qw ( check_if_msvc_2015_or_later hard_routing_error win_longpath_error_llc
                      win_longpath_error_quartus kernel_fit_error hard_routing_error_code
                      remove_duplicate save_profiling_xml mysystem move_to_err_and_log get_quartus_version_str
                      get_area_percent_estimates remove_named_files compilation_env_string
                      add_hash_sections save_pkg_section device_get_family_no_normalization version
                      find_board_spec get_board_hw_path_from_bsp get_acl_board_hw_path get_pkg_section get_kernel_list
                      filename_validation save_work_dir extract_work_dir add_rtl_hash_section check_quartus_env
                      check_input_file_for_compile_flow cleanup_object_files);

our @EXPORT = qw( $prog $emulatorDevice $return_status @given_input_files $output_file @srcfile_list
                  @objfile_list @output_objfile_list $linked_objfile $x_file $absolute_srcfile @absolute_srcfile_list
                  $absolute_profilerconf_file $marker_file $board_variant $bsp_variant $work_dir
                  @lib_files @lib_paths @resolved_lib_files @resolved_hdlspec_files @lib_bc_files @library_list @aoco_library_list
                  $create_library_flow $create_library_flow_for_sycl $library_version_override $library_skip_fastemulator_gen $created_shared_aoco $preproc_gcc_toolchain
                  $ocl_header_filename $ocl_header $clang_exe $opt_exe @link_exe $llc_exe $sysinteg_exe
                  $aocl_libedit_exe $ioc_exe $spirv_exe $fulllog $quartus_log $regtest_mode $internal_assert $regtest_errlog $parse_only
                  $opt_only $verilog_gen_only $ip_gen_only $high_effort $skip_qsys $compile_step
                  $aoco_to_aocr_aocx_only $aocr_to_aocx_only $griffin_flow $emulator_flow $emulator_fast
                  $run_quartus $standalone $change_output_folder_name $sycl_mode $sycl_hw_mode $hdl_comp_pkg_flow $use_aocr_input
                  $new_sim_mode $is_pro_mode $no_automigrate
                  $emu_optimize_o3 $emu_ch_depth_model $fast_compile $high_effort_compile $incremental_compile $empty_kernel_flow
                  $save_partition_file $set_partition_file $user_defined_board $user_defined_bsp $user_defined_flow
                  $soft_region_on $atleastoneflag $report_only $c_flag_only $error_on_timing_fail $allowed_timing_slack
                  $optarea $force_initial_dir $orig_force_initial_dir $use_ip_library $use_ip_library_override $do_env_check $dsploc
                  $ramloc $cpu_count $report $estimate_throughput $debug $time_log_filename $time_passes $dotfiles
                  $timing_slack_check $slack_value $tidy $pkg_save_extra $library_debug $save_last_bc
                  $disassemble $fit_seed $profile $profile_shared_counters $program_hash $triple_arg $dash_g $user_dash_g $ecc_protected
                  $ecc_max_latency $user_opt_hyper_optimized_hs @user_clang_args @user_opencl_args $opt_arg_after $llc_arg_after
                  $clang_arg_after $sysinteg_arg_after $max_mem_percent_with_replication @additional_migrations
                  @blocked_migrations $profilerconf_file
                  $device_spec $lmem_disable_split_flag @additional_qsf @additional_ini
                  $lmem_disable_replication_flag $qbindir $exesuffix $tmp_dir $emulator_arch $acl_root
                  $bsp_version $target_model @all_files @all_dep_files @clang_warnings
                  $reuse_aocx_file $reuse_exe_file $sycl_ioc_exe
                  $ACL_CLANG_IR_SECTION_PREFIX @CLANG_IR_TYPE_SECT_NAME $QUARTUS_VERSION $win_longpath_suggest aocr_exclude_list);

# Global Variables
our $prog = 'aoc';
our $emulatorDevice = 'EmulatorDevice'; #Must match definition in acl.h
our $return_status = 0;

#Filenames
our @given_input_files; # list of input files specified on command line.
our $output_file = undef; # -o argument
our @srcfile_list; # might be relative or absolute
our @objfile_list; # might be relative or absolute
our @output_objfile_list; # a list for the output aoco files
our $linked_objfile = undef;
our $x_file = undef; # might be relative or absolute
our $absolute_srcfile = undef; # absolute path
our @absolute_srcfile_list = ();
our $absolute_profilerconf_file = undef; # absolute path of the Profiler Config file
our $marker_file = ".project.marker"; # relative path of the marker file to the project working directory
our $board_variant = undef;
our $bsp_variant = undef;
our $reuse_aocx_file = undef;
our $reuse_exe_file = undef;

#directories
our $work_dir = undef; # absolute path of the project working directory

#library-related
our @lib_files;
our @lib_paths;
our @resolved_lib_files;
our @resolved_hdlspec_files;
our @lib_bc_files = ();
our @library_list; # list of the text files containing .aoco library files from sycl driver
our @aoco_library_list; # list of .aoco library files expanded from @library_list
our $created_shared_aoco = undef;
our $create_library_flow = undef;
our $create_library_flow_for_sycl = undef;
our $library_version_override = undef;
our $library_skip_fastemulator_gen = undef;
our $preproc_gcc_toolchain = undef;
our $ocl_header_filename = "opencl_lib.h";
our $ocl_header = $ENV{'INTELFPGAOCLSDKROOT'}."/share/lib/acl"."/".$ocl_header_filename;

# Executables
our $clang_exe = $ENV{'INTELFPGAOCLSDKROOT'}."/llvm/aocl-bin"."/aocl-clang";
our $opt_exe = $ENV{'INTELFPGAOCLSDKROOT'}."/llvm/aocl-bin"."/aocl-opt";
# article:1408742169 -irmover-type-merging=0 required to prevent incremental compile false positives.
our @link_exe = ($ENV{'INTELFPGAOCLSDKROOT'}."/llvm/aocl-bin"."/aocl-link", "-irmover-type-merging=0");
our $llc_exe = $ENV{'INTELFPGAOCLSDKROOT'}."/llvm/aocl-bin"."/aocl-llc";
our $sysinteg_exe = $ENV{'INTELFPGAOCLSDKROOT'}."/windows64/bin"."/system_integrator";
our $aocl_libedit_exe = "aocl xlibrary";
# this will be picked up from $PATH
our $ioc_exe = "aocl-ioc64";
our $sycl_ioc_exe = "ioc64";
our $spirv_exe = $ENV{'INTELFPGAOCLSDKROOT'}."/llvm/aocl-bin"."/aocl-llvm-spirv";

#Log files
our $fulllog = undef;
our $quartus_log = 'quartus_sh_compile.log';

our $regtest_mode = 0;
our $regtest_errlog = 'reg.err';

our $internal_assert = -1; # By default, internal assert is enabled only in regtest runs.
                           # This setting can be overwritten by
                           #   -internal-assert=true/false

#Flow control
our $parse_only = 0; # Hidden option to stop after clang.
our $opt_only = 0; # Hidden option to only run the optimizer
our $verilog_gen_only = 0; # Hidden option to only run the Verilog generator
our $ip_gen_only = 0; # Hidden option to only run up until ip-generate, used by sim
our $high_effort = 0;
our $skip_qsys = 0; # Hidden option to skip the Qsys generation of "system"
our $compile_step = 0; # stop after generating .aoco
our $aoco_to_aocr_aocx_only = 0; # start with .aoco file(s) and run through till system Integrator or quartus
our $aocr_to_aocx_only = 0; # start with .aocr file and run through quartus only
our $griffin_flow = 1; # Use DSPBA backend instead of HDLGeneration
our $emulator_flow = 0;
our $emulator_fast = 1;
our $run_quartus = 0;
our $standalone = 0;
our $change_output_folder_name = undef;
our $sycl_mode = 0;
our $sycl_hw_mode = 0;
our $hdl_comp_pkg_flow = 0; #Forward args from 'aoc' to 'aocl library'
our $use_aocr_input = 0; # compile the aocx file from an aocr file as input
our $new_sim_mode  = 0;
our $is_pro_mode = 0;
our $no_automigrate = 0; #Hidden option to skip BSP Auto Migration
our $emu_optimize_o3 = 0; #Apply -O3 optimizations for the emulator flow
our $emu_ch_depth_model = ''; #Channel depth mode in emulator flow
our $fast_compile = 0; #Allows user to speed up compile times while suffering performance hit
our $high_effort_compile = 0; #Allow user to specify compile with high effort on performance
our $incremental_compile = ''; #Internal flag for forcing partitions to be saved in incremental compile
our $empty_kernel_flow = 0; #Internal flag for compiling with kernel set to empty in Quartus
our $save_partition_file = ''; #Internal flag for forcing partitions to be created in incremental compile
our $set_partition_file = ''; #Allows user to speed compile times while suffering performance hit
our $user_defined_board = 0; # True if the user specifies -board option
our $user_defined_bsp = 0; # True if the user specifies -board-package option
our $user_defined_flow = 0; # True if the user specifies -march=emulator
our $soft_region_on = ''; #Add soft region settings
our $atleastoneflag = 0;
our $report_only = 0; # compile the cl file with '-rtl' flag
our $c_flag_only = 0;

#Flow modifiers
our $optarea = 0;
our $force_initial_dir = ''; # Absolute path of original working directory the user told us to use.
                             # This variable may be modified by the AOC driver.
our $orig_force_initial_dir = ''; # Absolute path of original working directory the user told us to use.
                                  # This variable will not change from the value that the user passed in.
our $use_ip_library = 1; # Should AOC use the soft IP library
our $use_ip_library_override = 1;
our $do_env_check = 1;
our $dsploc = '';
our $ramloc = '';
our $cpu_count = -1;
our @additional_qsf = ();
our @additional_ini = ();

#Output control
our $report = 0; # Show Throughput and area analysis
our $estimate_throughput = 0; # Show Throughput guesstimate
our $debug = 0; # Show debug output from various stages
our $time_log_filename = undef; # Filename from --time arg
our $time_passes = 0; # Time LLVM passes. Requires $time_log_fh to be valid.
# Should we be tidy? That is, delete all intermediate output and keep only the output .aclx file?
# Intermediates are removed only in the --hw flow
our $dotfiles = 0;
our $timing_slack_check = 0; #Detect slack timing violation and error out
our $slack_value = undef; #Default slack value is undefined
our $tidy = 0;
our $pkg_save_extra = 0; # Save extra items in the package file: source, IR, verilog
our $library_debug = 0;
our $error_on_timing_fail = 1; # this can be set by the users with the "-timing-failure-mode" flag
                               # default to warn (= 1) for OpenCL compiles
our $allowed_timing_slack = 0; # the amount of slack allowed before warning/erroring out on a timing failure

# Yet unclassfied
our $save_last_bc= 0; #don't remove final bc if we are generating profiles
our $disassemble = 0; # Hidden option to disassemble the IR
our $fit_seed = undef; # Hidden option to set fitter seed
our $profile = 0; # Option to enable profiling
our $profile_shared_counters = 0; # Option to enable shared counters in the profiler
our $program_hash = undef; # SHA-1 hash of program source, options, and board.
our $triple_arg = '';
our $dash_g = 1;      # Debug info enabled by default. Use -g0 to disable.
our $user_dash_g = 0; # Indicates if the user explictly compiled with -g.
our $ecc_protected = 0;
our $ecc_max_latency = 0;
our $user_opt_hyper_optimized_hs = 0;

# Regular arguments.  These go to clang, but does not include the .cl file.
our @user_clang_args = ();

# The compile options as provided by the clBuildProgram OpenCL API call.
# In a standard flow, the ACL host library will generate the .cl file name,
# and the board spec, so they do not appear in this list.
our @user_opencl_args = ();

our $opt_arg_after = ''; # Extra options for opt, after regular options.
our $llc_arg_after = '';
our $clang_arg_after = '';
our $sysinteg_arg_after = '';
our $max_mem_percent_with_replication = 100;
our @additional_migrations = ();
our @blocked_migrations = ();

our $profilerconf_file = undef;

# device spec differs from board spec since it
# can only contain device information (no board specific parameters,
# like memory interfaces, etc)
our $device_spec = "";

our $lmem_disable_split_flag = '-no-lms=1';
our $lmem_disable_replication_flag = ' -no-local-mem-replication=1';

# On Windows, always use 64-bit binaries.
# On Linux, always use 64-bit binaries, but via the wrapper shell scripts in "bin".
our $qbindir = ( $^O =~ m/MSWin/ ? 'bin64' : 'bin' );

# For messaging about missing executables
our $exesuffix = ( $^O =~ m/MSWin/ ? '.exe' : '' );

# temporary app data directory
our $tmp_dir = ( $^O =~ m/MSWin/ ? "$ENV{'USERPROFILE'}\\AppData\\Local\\aocl" : "/var/tmp/aocl/$ENV{USERNAME}" );

our $emulator_arch = acl::Env::get_arch();

our $acl_root = acl::Env::sdk_root();

# Variables used multiple times in aoc.pl
our $bsp_version = undef;
our $target_model = undef;

our @all_files = ();
our @all_dep_files = ();
our @clang_warnings = ();

# Types of IR that we may have
# AOCO sections in shared mode will have names of form:
#    $ACL_CLANG_IR_SECTION_PREFIX . $CLANG_IR_TYPE_SECT_NAME[ir_type]
our $ACL_CLANG_IR_SECTION_PREFIX = ".acl.clang_ir";
our @CLANG_IR_TYPE_SECT_NAME = (
  'spir64-unknown-unknown-intelfpga',
  'x86_64-unknown-linux-intelfpga',
  'x86_64-pc-windows-intelfpga'
);

our $QUARTUS_VERSION = undef; # Saving the output of quartus_sh --version globally to save time.
our $win_longpath_suggest = "\nSUGGESTION: Windows has a 260 limit on the length of a file name (including the full path). The error above *may* have occurred due to the compiler generating files that exceed that limit. Please trim the length of the directory path you ran the compile from and try again.\n\n";


 # that specifies the files/directories that should be excluded. must be the base name
our @aocr_exclude_list = ("reports/");

# Family used by simulator qsys-generate. Value from Quartus get_part_info API
my $family_from_quartus = undef;

# Local Functions

sub _mysystem_redirect($@) {
  # Run command, but redirect standard output to $outfile.
  my ($outfile,@cmd) = @_;
  return acl::Common::mysystem_full ({'stdout' => $outfile}, @cmd);
}

# Exported Functions

sub check_if_msvc_2015_or_later() {
  my $is_msvc_2015_or_later = 0;
  if (($emulator_arch eq 'windows64') && ($emulator_flow == 1) && ($emulator_fast == 0) ) {
    my $msvc_out = acl::Common::mybacktick('LINK 2>&1');
    chomp $msvc_out;

    if ($msvc_out !~ /Microsoft \(R\) Incremental Linker Version/ ) {
      acl::Common::mydie("$prog: Can't find VisualStudio linker LINK.EXE.\nEither use Visual Studio x64 Command Prompt or run %INTELFPGAOCLSDKROOT%\\init_opencl.bat to setup your environment.\n");
    }
    my ($linker_version) = $msvc_out =~ /(\d+)/;
    if ($linker_version >= 14 ){
      #FB:441273 Since VisualStudio 2015, the way printf is dealt with has changed.
      $is_msvc_2015_or_later = 1;
    }
  }
  return $is_msvc_2015_or_later;
}

#remove duplicate words in a strings
sub remove_duplicate {
  my ($word) = @_;
  my @words = split / /, $word;
  my @newwords;
  my %done;
  for (@words) {
  push(@newwords,$_) unless $done{$_}++;
  }
  join ' ', @newwords;
}

sub hard_routing_error_code($@)
{
  my $error_string = shift @_;
  if( $error_string  =~ /Critical Warning\s*\(188026\)/ ) {
    # 188026 is the signal ID of no-route issue
    # ref case:14010429278
    return 1;
  }
  return 0;
}

sub kernel_fit_error($@)
{
  my $error_string = shift @_;
  if( $error_string =~ /Error\s*\(11802\):/ ) {
    return 1;
  }
  return 0;
}

sub win_longpath_error_quartus($@)
{
  my $error_string = shift @_;
  if( $error_string =~ /Error\s*\(14989\):/ ) {
    return 1;
  }
  if( $error_string =~ /Error\s*\(19104\):/ ) {
    return 1;
  }
  if( $error_string =~ /Error\s*\(332000\):/ ) {
    return 1;
  }
  return 0;
}

sub win_longpath_error_llc($@)
{
  my $error_string = shift @_;
  if( $error_string =~ /Error:\s*Could not open file/ ) {
    return 1;
  }
  return 0;
}

sub hard_routing_error($@)
 { #filename
     my $infile = shift @_;
     open(ERR, "<$infile");  ## if there is no $infile, we just return 0;
     while( <ERR> ) {
       if( hard_routing_error_code( $_ ) ) {
         return 1;
       }
     }
     close ERR;
     return 0;
 }

sub save_profiling_xml($$) {
  my ($pkg,$basename) = @_;
  # Save the profile XML file in the aocx
  $pkg->add_file('.acl.profiler.xml',"$basename.bc.profiler.xml")
      or acl::Common::mydie("Can't save profiler XML $basename.bc.profiler.xml into package file: $acl::Pkg::error\n");
}

sub mysystem(@) {
  return _mysystem_redirect('',@_);
}

# This is called between a system call and check child error so
# it can NOT do system calls
sub move_to_err_and_log { #String filename ..., logfile
  my $string = shift @_;
  my $logfile = pop @_;
  foreach my $infile (@_) {
    open ERR, "<$infile"  or acl::Common::mydie("Couldn't open $infile for reading.");
    while(my $l = <ERR>) {
      print STDERR $l;
    }
    close ERR;
    acl::Common::move_to_log($string, $infile, $logfile);
  }
}

sub get_quartus_version_str() {
  # capture Version info (starting with "Version") and Edition info (ending up with "Edition")
  my ($quartus_version_str1) = $QUARTUS_VERSION =~ /Version (.* Build \d*)/;
  my ($quartus_version_str2) = $QUARTUS_VERSION =~ /( \w+) Edition/;
  my $quartus_version = $quartus_version_str1 . $quartus_version_str2;
  return $quartus_version;
}

sub get_area_percent_estimates {
  # Get utilization numbers (in percent) from area.json.
  # The file must exist when this function is called.

  open my $area_json, '<', $work_dir."/area.json";
  my $util = 0;
  my $les = 0;
  my $ffs = 0;
  my $rams = 0;
  my $dsps = 0;

  while (my $json_line = <$area_json>) {
    if ($json_line =~ m/\[([.\d]+), ([.\d]+), ([.\d]+), ([.\d]+), ([.\d]+)\]/) {
      # Round all percentage values to the nearest whole number.
      $util = int($1 + 0.5);
      $les = int($2 + 0.5);
      $ffs = int($3 + 0.5);
      $rams = int($4 + 0.5);
      $dsps = int($5 + 0.5);
      last;
    }
  }
  close $area_json;

  return ($util, $les, $ffs, $rams, $dsps);
}

sub remove_named_files {
    my $verbose = acl::Common::get_verbose();
    foreach my $fname (@_) {
      acl::File::remove_tree( $fname, { verbose => ($verbose == 1 ? 0 : $verbose), dry_run => 0 } )
         or acl::Common::mydie("Cannot remove intermediate files under directory $fname: $acl::File::error\n");
    }
}

sub compilation_env_string($$$$){
  my ($work_dir,$board_variant,$input_args,$bsp_flow_name) = @_;
  #Case:354532, not handling relative address for AOCL_BOARD_PACKAGE_ROOT correctly.
  my $starting_dir = acl::File::abs_path('.');  #keeping to change back to this dir after being done.
  my $orig_dir = acl::Common::get_original_dir();
  my $board_spec_xml = "";
  my $synthesize_cmd = "";
  chdir $orig_dir or acl::Common::mydie("Can't change back into directory $orig_dir: $!");

  # Gathering all options and tool versions.
  my $build_number = "168.5";
  my $acl_Version = "20.3.0";
  my $clang_version; my $llc_version; my $sys_integrator_version; my $ioc_version;
  if ($emulator_fast) {
    $ioc_version = acl::Common::mybacktick("$ioc_exe -version");
    $ioc_version =~ s/\s+/ /g; #replacing all white spaces with space
  } else {
    my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
    $board_spec_xml = acl::AOCDriverCommon::find_board_spec($acl_board_hw_path);
    $clang_version = acl::Common::mybacktick("$clang_exe --version");
    $clang_version =~ s/\s+/ /g; #replacing all white spaces with space
    $llc_version = acl::Common::mybacktick("$llc_exe --version");
    $llc_version =~ s/\s+/ /g; #replacing all white spaces with space
    $sys_integrator_version = acl::Common::mybacktick("$sysinteg_exe --version");
    $sys_integrator_version =~ s/\s+/ /g; #replacing all white spaces with space
    my $synthesize_cmd = ::acl::Env::aocl_boardspec( $acl_board_hw_path, "synthesize_cmd".$bsp_flow_name); # synthesize for bsp compile flow
    ( $target_model.$synthesize_cmd !~ /error/ ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");
  }
  my $lib_path = "$ENV{'LD_LIBRARY_PATH'}";
  my $board_pkg_root = "$ENV{'AOCL_BOARD_PACKAGE_ROOT'}";
  my $quartus_version = $QUARTUS_VERSION;
  $quartus_version =~ s/\s+/ /g; #replacing all white spaces with space

  # Quartus compile command
  my $acl_qsh_compile_cmd="$ENV{'ACL_QSH_COMPILE_CMD'}"; # Environment variable ACL_QSH_COMPILE_CMD can be used to replace default quartus compile command (internal use only).

  # Concatenating everything
  my $res = "";
  $res .= "INPUT_ARGS=".$input_args."\n";
  $res .= "BUILD_NUMBER=".$build_number."\n";
  $res .= "ACL_VERSION=".$acl_Version."\n";
  $res .= "OPERATING_SYSTEM=$^O\n";
  $res .= "BOARD_SPEC_XML=".$board_spec_xml."\n";
  $res .= "TARGET_MODEL=".$target_model."\n";
  if ($emulator_fast) {
    $res .= "IOC_VERSION=".$ioc_version."\n";
  } else {
    $res .= "CLANG_VERSION=".$clang_version."\n";
    $res .= "LLC_VERSION=".$llc_version."\n";
    $res .= "SYS_INTEGRATOR_VERSION=".$sys_integrator_version."\n";
  }
  $res .= "LIB_PATH=".$lib_path."\n";
  $res .= "AOCL_BOARD_PKG_ROOT=".$board_pkg_root."\n";
  $res .= "QUARTUS_VERSION=".$quartus_version."\n";
  $res .= "QUARTUS_OPTIONS=".$synthesize_cmd."\n";
  $res .= "ACL_QSH_COMPILE_CMD=".$acl_qsh_compile_cmd."\n";

  chdir $starting_dir or acl::Common::mydie("Can't change back into directory $starting_dir: $!"); # Changing back to the dir I started with
  return $res;
}

# Adds a unique hash for the compilation, and a section that contains 3 hashes for the state before quartus compile.
sub add_hash_sections($$$$$) {
  my ($work_dir,$board_variant,$pkg_file,$input_args,$bsp_flow_name) = @_;
  my $pkg = get acl::Pkg($pkg_file) or acl::Common::mydie("Can't find package file: $acl::Pkg::error\n");

  #Case:354532, not handling relative address for AOCL_BOARD_PACKAGE_ROOT correctly.
  my $starting_dir = acl::File::abs_path('.');  #keeping to change back to this dir after being done.
  my $orig_dir = acl::Common::get_original_dir();
  chdir $orig_dir or acl::Common::mydie("Can't change back into directory $orig_dir: $!");

  my $compilation_env = compilation_env_string($work_dir,$board_variant,$input_args,$bsp_flow_name);

  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.compilation_env',$compilation_env);

  # Random unique hash for this compile:
  my $hash_exe = acl::Env::sdk_hash_exe();
  my $temp_hashed_file="$work_dir/hash.tmp"; # Temporary file that is used to pass in strings to aocl-hash
  my $ftemp;
  my $random_hash_key;
  open($ftemp, '>', $temp_hashed_file) or die "Could not open file $!";
  my $rand_key = rand;
  print $ftemp "$rand_key\n$compilation_env";
  close $ftemp;


  $random_hash_key = acl::Common::mybacktick("$hash_exe \"$temp_hashed_file\"");
  unlink $temp_hashed_file;

  chomp $random_hash_key;
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.rand_hash',$random_hash_key);
  $sysinteg_arg_after .= " --rand-hash $random_hash_key";

  # The hash of inputs and options to quartus + quartus versions:
  my $before_quartus;

  my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
  my $quartus_version = $QUARTUS_VERSION;
  $quartus_version =~ s/\s+/ /g; #replacing all white spaces with space

  # Quartus compile command
  my $synthesize_cmd = ::acl::Env::aocl_boardspec( $acl_board_hw_path, "synthesize_cmd".$bsp_flow_name);
  ( $bsp_flow_name !~ /error/ ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");
  my $acl_qsh_compile_cmd="$ENV{'ACL_QSH_COMPILE_CMD'}"; # Environment variable ACL_QSH_COMPILE_CMD can be used to replace default quartus compile command (internal use only).

  open($ftemp, '>', $temp_hashed_file) or die "Could not open file $!";
  print $ftemp "$quartus_version\n$synthesize_cmd\n$acl_qsh_compile_cmd\n";
  close $ftemp;

  $before_quartus.= acl::Common::mybacktick("$hash_exe \"$temp_hashed_file\""); # Quartus input args hash
  $before_quartus.= acl::Common::mybacktick("$hash_exe -d \"$acl_board_hw_path\""); # All bsp directory hash
  $before_quartus.= acl::Common::mybacktick("$hash_exe -d \"$work_dir\" --filter .v --filter .sv --filter .hdl --filter .vhdl"); # HDL files hash

  unlink $temp_hashed_file;
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.quartus_input_hash',$before_quartus);

  chdir $starting_dir or acl::Common::mydie("Can't change back into directory $starting_dir: $!"); # Changing back to the dir I started with.
}

sub add_rtl_hash_section($$) {
  my ($work_dir, $pkg_file) = @_;
  my $pkg = get acl::Pkg($pkg_file) or acl::Common::mydie("Can't find package file: $acl::Pkg::error\n");

  my $rtl_hash_exe = acl::Env::sdk_host_bin_dir()."/aocl-rtl-hash";
  my $rtl_hash = acl::Common::mybacktick("$rtl_hash_exe \"$work_dir\"");
  my $retcode = $?;
  if ($retcode != 0) {
    if (($retcode >> 8) == 10) {
      # We have detected an unknown file extension.  This needs to be fixed in aocl-rtl-hash.
      acl::Common::mydie("Unknown file extension: $rtl_hash\n");
    }
    # We don't want to save the hash if we failed.
    return;
  }
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.rtl_hash',$rtl_hash);
}

sub save_pkg_section($$$) {
   my ($pkg,$section,$value) = @_;
   # The temporary file should be in the compiler work directory.
   # The work directory has already been created.
   my $file = $work_dir.'/value.txt';
   open(VALUE,">$file") or acl::Common::mydie("Can't write to $file: $!");
   binmode(VALUE);
   print VALUE $value;
   close VALUE;
   $pkg->set_file($section,$file)
       or acl::Common::mydie("Can't save value into package file: $acl::Pkg::error\n");
   unlink $file;
}

# Copied from i++.pl
sub device_get_family_no_normalization {  # DSPBA needs the original Quartus format
    my $local_start = time();
    my $qii_family_device = shift;

    return $family_from_quartus if (defined($family_from_quartus));

    # only query when we don't have one
    my $devinfo = acl::Common::mybacktick("devinfo $qii_family_device");
    my $return_code = $?;
    if ($return_code != 0) {
      # s10 specific problem: there is no 1sg280lu3f50e1vgs1 device, instead it is 1sg280lu3f50e1vg (without the "s1" at the end)
      # $family_from_quartus will be empty if an unrecognized device is provided, try again by cutting off the last 2 chars
      $qii_family_device = substr($qii_family_device, 0, -2);
      $devinfo = acl::Common::mybacktick("devinfo $qii_family_device");
      $return_code = $?;
    }
    if ($return_code == 0) {
      my ($dev_family,$dev_part,$dev_speed) = split(",", $devinfo);
      $family_from_quartus = $dev_family;
      return $family_from_quartus;
    }

    # Error out when we couldn't get anything, i.e. licence server not available
    acl::Common::mydie("$prog: Can't get family from devinfo for device: $qii_family_device.\n") if ($family_from_quartus eq "");
    # log_time ('Get device family', time() - $local_start) if ($time_log_fh);
    acl::Common::log_time ('Get device family', time() - $local_start) if ($time_log_filename);
    return $family_from_quartus;
}

sub version($) {
  my $outfile = $_[0];
  print $outfile "Intel(R) FPGA SDK for OpenCL(TM), 64-Bit Offline Compiler\n";
  print $outfile "Version 20.3.0 Build 168.5 Pro Edition\n";
  print $outfile "Copyright (C) 2020 Intel Corporation\n";
}

# Make sure the board specification file exists. Return directory of board_spec.xml
sub find_board_spec {
  my ($acl_board_hw_path) = @_;
  my ($board_spec_xml) = acl::File::simple_glob( $acl_board_hw_path."/board_spec.xml" );
  my $xml_error_msg = "Cannot find Board specification!\n*** No board specification (*.xml) file inside ".$acl_board_hw_path.". ***\n" ;
  if ( $device_spec ne "" ) {
    my $full_path =  acl::File::abs_path( $device_spec );
    $board_spec_xml = $full_path;
    $xml_error_msg = "Cannot find Device Specification!\n*** device file ".$board_spec_xml." not found.***\n";
  }
  -f $board_spec_xml or acl::Common::mydie( $xml_error_msg );
  return $board_spec_xml;
}

sub get_board_hw_path_from_bsp {
  my ($bsp, $bv) = @_;
  my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $bsp;
  my $result = get_acl_board_hw_path($bv);
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root;
  return $result;
}

sub get_acl_board_hw_path {
  my $bv = shift @_;
  my ($result) = acl::Env::board_hw_path($bv);
  return $result;
}

sub get_pkg_section($$) {
  my ($pkg,$section) = @_;
  # Work dir may not be created yet, but use name to prevent file collisions
  my $file = $work_dir.'_value.txt';
  my $value = undef;
  $pkg->get_file($section,$file)
      or acl::Common::mydie("Can't get value into file: $acl::Pkg::error\n");
  open(VALUE,"<$file") or acl::Common::mydie("Can't read from $file: $!");
  $value = <VALUE>;
  close VALUE;
  unlink $file;
  chomp $value;
  return $value;
}

sub get_kernel_list($) {
  # read the comma-separated list of components from a file
  my $base = $_[0];
  my $project_bc_xml_filename = "${base}.bc.xml";
  my $BC_XML_FILE;

  open (BC_XML_FILE, "<${project_bc_xml_filename}") or acl::Common::mydie "Couldn't open ${project_bc_xml_filename} for read!\n";
  my @kernel_array;
  my $num = 0;
  my $counter = 0;
  while(my $var =<BC_XML_FILE>) {
    if ($var =~ /<KERNEL_BUNDLE name=".*" compute_units="(.*)"/) {
      $num = $1;
      $counter = 0;
    } elsif ($var =~ /<KERNEL name=".*" kname="(.*)" filename=/) {
      if ($counter < $num) {
        my $kernel_str = $1 . "," . $counter;
        $counter++;
        push(@kernel_array,$kernel_str);
      } else {
        acl::Common::mydie( "The number of compute unit is wrong for ${1}, please check ${project_bc_xml_filename} for more details!\n" );
      }
    }
  }

  close BC_XML_FILE;
  return @kernel_array;
}

sub filename_validation($) {
  # check if the input file is valid that not contain the invalid characters
  my $input = $_[0];
  # resolve the path
  my $input_base = acl::File::mybasename($input);
  if ($input_base =~ m/[^a-zA-Z0-9_.-]/) {
    my $error_msg = "File: $input_base contains invalid characters.\n".
                    "Ensure the file name only contains alphanumeric characters, dash, underscore or dot.\n";
    acl::Common::mydie($error_msg);
  }
}

sub save_work_dir {
  # archive the work_dir and add it to the aocr
  my ($dirbase, $obj, $prog, $incremental_input_dir) = @_;
  my $outbase;
  my $temp_dir = acl::File::mktemp();
  my $temp_file = $temp_dir.acl::Env::pathsep().'.bin';

  if ($output_file and $report_only) {
    # get the basename for the work directory
    $outbase = acl::File::mybasename($output_file);
    $outbase =~ s{\.[^.]+$}{};
  } elsif ($incremental_input_dir and $report_only) {
    # if the output is not set, while the incremental-input-dir is set
    $outbase = acl::File::mybasename($incremental_input_dir);
  } else {
    $outbase = $dirbase;
  }
  my $pkg_file = $obj;
  my $pkg = get acl::Pkg($pkg_file) or acl::Common::mydie("Cannot find pkg file $obj: $acl::Pkg::error\n");
  acl::AOCDriverCommon::progress_message("$prog: Archiving the work directory\n");
  my $cur_dir = acl::File::cur_path();
  # go to the parent directory of work_dir
  if (! -e $work_dir or ! -d $work_dir) {
    acl::Common::mydie("Cannot find the work directory $work_dir\n");
  }
  chdir $work_dir.acl::Env::pathsep().".." or acl::Common::mydie("Cannot find the parent directory of the work directory $work_dir\n");
  if (! -e $outbase or ! -d $outbase) {
    acl::Common::mydie("Cannot find the work directory to pack\n");
  }

  # move the exclusive files or directories to a temporary directory
  # currently cannot use the modern perl in soc, have to implement the oldway.
  chdir $work_dir;
  # check if the excluded file/dir exists
  my @exist_exclude_list = ();
  my $need_exclude = 0;
  foreach (@aocr_exclude_list) {
    if (-e $_) {
      push(@exist_exclude_list,$_);
      $need_exclude = 1;
    }
  }
  if ($need_exclude) {
    # at least one file/dir need to be pickup and exclude from the aocr
    # create the temp_dir for the file/dir in the pickup list
    mkdir $temp_dir or acl::Common::mydie("Cannot create the temporary directory during rtl compilation\n");
    my $ex_file_dir_full_path = undef;
    my $dest_file_dir_full_path = undef;
    # package the exclusive file/directory from the list
    $pkg->package("$temp_file", @exist_exclude_list);
    # remove the file/directory from the exist list
    foreach my $ex_file_dir (@exist_exclude_list) {
      if (-f $ex_file_dir) {
        unlink $ex_file_dir; # the target is a file
      } else {
        acl::File::remove_tree($ex_file_dir); # the target is a dir
      }
    }

    # save to the temp dir
    -e $temp_file or acl::Common::mydie("Cannot package the work directory\n");
  }

  chdir $work_dir.acl::Env::pathsep().".." or acl::Common::mydie("Cannot find the parent directory of the work directory $work_dir\n");

  # package the modified work directory to the aocr file
  $pkg->package("$outbase-workdir.bin", "$outbase") == 0 or acl::Common::mydie("Cannot bundle work directory files\n");
  $pkg->set_file(".acl.workdir_package","$outbase-workdir.bin");
  # remove the temp file and folder
  unlink("$outbase-workdir.bin");
  my $orig_dir = acl::Common::get_original_dir();

  # restore the exclusive files back to the workdir if is needed
  if ($need_exclude) {
    chdir $work_dir;
    $pkg->unpackage($temp_file, "$work_dir");
    foreach (@exist_exclude_list) {
      -e $_ or acl::Common::mydie("Cannot package the work directory\n");
    }
  }

  # back to the original dir and clean up
  chdir $orig_dir or acl::Common::mydie("Cannot change back into directory $orig_dir: $!");
  acl::File::remove_tree($work_dir) if $tidy; # remove the work dir if "-tidy" is set

  # deleted the temp dir when the program exists
  acl::File::remove_tree($temp_dir)
}

sub extract_work_dir {
  # extract the archived work_dir from the aocr
  my ($dirbase, $obj, $final_work_dir, $board_variant) = @_;
  my $outbase = undef;
  my $expected_work_dir_for_hw = undef;
  my $need_overwrite_work_dir = 0;
  my $pkg_archivefile = undef;
  my $temp_dir = undef;
  my $cur_dir = acl::File::cur_path();
  my $final_out_dir = $cur_dir;
  my $need_extract = 0;

  # first check that the aocr file was compiled with a valid aoc version
  if ($do_env_check){
    check_aocr_aoc_version_compatible($obj);
  }

  if ($output_file) {
    # get rid of the path and extensions
    $outbase = acl::File::mybasename($output_file);
    $outbase =~ s{\.[^.]+$}{};
    $final_out_dir = acl::File::mydirname($output_file);
  } else {
    $outbase = $dirbase;
  }
  my $pkg_file = $obj;
  # check if the aocr file having the the pkg section
  my $pkg = get acl::Pkg($pkg_file) or acl::Common::mydie("Cannot find pkg file $obj: $acl::Pkg::error\n");
  my $section_exist = $pkg->exists_section('.acl.workdir_package') ? 1 : 0;
  # check if the work dir already exists
  $final_out_dir =~ s/\/$//;   # Remove trailing slashes
  $expected_work_dir_for_hw = $final_out_dir.acl::Env::pathsep().$outbase;

  my $board_xml = get_board_hw_path_from_bsp($bsp_variant, $board_variant)."/board_spec.xml";

  if (-d $expected_work_dir_for_hw) {
    print "$prog: Warning: the work directory $expected_work_dir_for_hw already exists. ".
    "Compilation will use the existing folder instead of the content in the $obj file. ".
    "To use the content in the $obj file, rename the existing directory $expected_work_dir_for_hw\n";

    # check that the files in the work directory were compiled with the same board version and variant as the current compile (error if not the case)
    if ($do_env_check and !check_workdir_board_compatible($expected_work_dir_for_hw, $board_variant, $board_xml)){
      acl::Common::mydie("$prog: The work directory used to continue was compiled with a different board version or variant than the current compilation. \n");
    }
  } else {
    # extract the work dir from the aocr file
    if (! $section_exist) {
      acl::Common::mydie("The aocr file does not contain the required work directory, please make sure the aocr file is compiled with the same version of the Intel(R) FPGA SDK for OpenCL(TM)\n") ;
    }
    $pkg_archivefile = $cur_dir.acl::Env::pathsep()."$outbase-workdir.bin";
    $pkg->get_file(".acl.workdir_package", $pkg_archivefile);
    $temp_dir = $cur_dir.acl::Env::pathsep()."temp_workdir_".time();
    $pkg->unpackage($pkg_archivefile, "$temp_dir") == 0 or acl::Common::mydie("Cannot unpack work directory\n");
    # move the file to final_work_dir
    my $target_work_dir = "";
    opendir my $dh, $temp_dir or acl::Common::mydie("$0: opendir: $!");
    my @dirs = grep {-d "$temp_dir/$_" && ! /^\.{1,2}$/} readdir($dh);
    if ($#dirs > 0) {
      acl::Common::mydie("Too many work directories\n");
    }
    $target_work_dir = $temp_dir.acl::Env::pathsep().$dirs[0];
    if (not -d $final_out_dir) {
      acl::File::make_path($final_out_dir);
    }
    # move the work directory to the designated output directory
    acl::File::movedir($target_work_dir, $expected_work_dir_for_hw);
  }
  if (! -e $expected_work_dir_for_hw or ! -d $expected_work_dir_for_hw) {
    acl::Common::mydie("Cannot create the work directory $expected_work_dir_for_hw\n");
  }
  # check that the directory packed in the aocr was compiled with the same board version and variant as the current compile (error if not the case)
  if ($do_env_check){
    chdir $expected_work_dir_for_hw;
    if (!$pkg->exists_section('.acl.board') or !check_aocr_board_compatible(get_pkg_section($pkg, '.acl.board'), $board_variant, $board_xml)) {
      acl::Common::mydie("$prog: The inputted aocr was compiled with a different board version or variant than the current compilation. \n");
    }
    chdir $cur_dir;
  }

  if (-e $pkg_archivefile) {
    # remove the intermdeiate package file
    unlink($pkg_archivefile);
  }
  if (-e $temp_dir) {
    # remove the intermediate work directory
    acl::File::remove_tree($temp_dir);
  }

  # overwrite the final_work_dir
  $$final_work_dir = $expected_work_dir_for_hw;

  return 0;
}

# check if the aoc version used to make the .aocr was the same as the current aoc version
sub check_aocr_aoc_version_compatible {
  my $aocr_file = shift;

  my $pkg = get acl::Pkg($aocr_file) or acl::Common::mydie("Cannot find pkg file $aocr_file: $acl::Pkg::error\n");

  if(!($pkg->exists_section('.acl.version'))){
    acl::Common::mydie("$prog: The inputted aocr was invalid or compiled using an old version of the Intel(R) FPGA SDK for OpenCL(TM).\n
      Please recompile the aocr with the currently installed SDK. \n");
    return 0; # if cannot confirm the versions are the same, something is really wrong with the .aocr
  }
  my $aocr_version = get_pkg_section($pkg, '.acl.version');
  my $sdk_version = acl::Env::sdk_version();
  if ($aocr_version != $sdk_version){
    acl::Common::mydie("$prog: The inputted aocr was compiled using a different release of the Intel(R) FPGA SDK for OpenCL(TM) than the one currently installed.\n  Current version is $sdk_version, while the aocr was compiled with $aocr_version. \n Please recompile the aocr with the currently installed SDK. \n");
    return 0;
  }
  return 1;
}

# Check if the same board that is currently being used was also used for compiling the .aocr
sub check_workdir_board_compatible {
  my $wordir = shift;
  my $board_variant = shift;
  my $board_xml = shift;

  my $workdir_boardspec = acl::File::find_first_occurence($wordir, "board_spec.xml");
  if (not defined $workdir_boardspec){
    acl::Common::mydie("$prog: The previous work directory lacks the board_spec.xml file needed for the compilation to continue.\n Please recompile the intermediate work directory or rename it to use the aocr file instead when continuing compilation. \n");
    return 0;
  }

  my $workdir_variant = acl::Env::aocl_boardspec( "$workdir_boardspec", "name");
  return compare_board_versions($workdir_boardspec, $board_xml, $workdir_variant, $board_variant, "work directory", 0);
}

# Check if the same board that is currently being used was also used for compiling the .aocr
sub check_aocr_board_compatible {
  my $board_variant_aocr = shift; 
  my $board_variant_bsp = shift;
  my $board_xml = shift;
  my $aocr_xml_file = $work_dir.'board_spec.xml';

  return compare_board_versions($aocr_xml_file, $board_xml, $board_variant_aocr, $board_variant_bsp, "aocr", 0);
}

# Check that the two board_spec.xml file match up
sub compare_board_versions {
  my ($board_spec1, $board_spec2, $board_variant1, $board_variant2, $message_chunk, $unlink_xml1) = @_;

  my $bsp1_version = acl::Env::aocl_boardspec( "$board_spec1", "version");
  my $bsp2_version = acl::Env::aocl_boardspec( "$board_spec2", "version");

  my $supplementary_message = ($message_chunk =~ /aocr/) ? "inputted" : "previously compiled";
  my $supplementary_message2 = ($message_chunk =~ /aocr/) ? "" : " or rename it to use the aocr file instead when continuing compilation";
  if (($bsp1_version != $bsp2_version)) {
    unlink $board_spec1 if ($unlink_xml1 == 1);
    acl::Common::mydie("$prog: The $supplementary_message $message_chunk was compiled with a different BSP than the one currently being compiled with.\n  The current version is $bsp2_version, while the $message_chunk was compiled with $bsp1_version. \n  Please recompile the $message_chunk with the currently installed BSP$supplementary_message2. \n");
    return 0;
  }

  if (not ($board_variant1 eq $board_variant2)) {
    unlink $board_spec1 if ($unlink_xml1 == 1);
    acl::Common::mydie("$prog: The $supplementary_message $message_chunk was compiled for a different board than the one currently being compiled for.\n  The current board is $board_variant2, while the $message_chunk was compiled for the $board_variant1 board. \n  Please recompile the $message_chunk with the currently installed BSP$supplementary_message2. \n");
    return 0;
  }

  if (not acl::File::compare_files($board_spec1, $board_spec2)) {
    unlink $board_spec1 if ($unlink_xml1 == 1);
    acl::Common::mydie("$prog: The $supplementary_message $message_chunk was compiled using a different BSP package\n than the one currently being compiled with.\n  Please recompile the $message_chunk with the currently installed BSP$supplementary_message2. \n");
    return 0;
  }
  unlink $board_spec1 if ($unlink_xml1 == 1);

  return 1;
}

sub get_board_files_for_quartus {
  my ($board_variant, $final_work_dir) = @_;
  # copy the rest of the board directory files, which are no longer in the .aocr, since quartus will need them
  my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
  my %options = ("do_not_replace" => 1);
  acl::File::copy_tree( $acl_board_hw_path."/*", $$final_work_dir, \%options)
    or acl::Common::mydie("Can't copy Board files: $acl::File::error");
  return 0;
}

sub quartus_has_correct_patches{
  my ($exe_path) = @_;
  my @patch_array = split(',', acl::Board_env::get_quartus_patches());
  my %patches = map { $_ => 0 } @patch_array;

  my $q_out = acl::Common::mybacktick($exe_path.' --version'); chomp $q_out;
  foreach my $line (split ('\n', $q_out)) {
    if ($line =~ m{Patches\s+([\w,\.]+)\s+}){
      my @ps = split(',', $1);
        foreach my $p (@ps){
          $patches{$p} = 1 if exists($patches{$p});
      }
    }
  }

  for my $found (values %patches){
    return 0 if $found eq '0';
  }
  return 1;
}

# In oneAPI Beta06+, Quartus-related environment variables (QUARTUS_ROOTDIR, PATH,
#  QUARTUS_64BIT, LD_LIBRARY_PATH) should be setup here, instead of in the entry wrapper:
#  aoc reads "quartus_version" from board_env.xml and
#  find Quartus in $INTEL_FPGA_ROOT/QuartusPrimePro/<quartus_version>/quartus.
#  Return possible error message if caller can't find Quartus after calling this subroutine.
sub setup_quartus_env {
  my ($set_quartus_java_only) = @_;

  # If INTEL_FPGA_ROOT has been set and INTEL_FPGA_ROOT/QuartusPrimePro/<quartus version>/quartus
  #   is a valid folder, use it as Quartus root.
  # Get Quartus version from board_env.xml
  if ( defined $ENV{'INTEL_FPGA_ROOT'} and !$ENV{'INTEL_FPGA_ROOT'} eq "" ){
    my $intel_fpga_root = $ENV{'INTEL_FPGA_ROOT'};
    my $current_board_package_root = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
    $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $bsp_variant;
    my $bsp_quartus_version = acl::Board_env::get_quartus_version();
    $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $current_board_package_root; # Restore AOCL_BOARD_PACKAGE_ROOT

    my $quartus_rootdir_override = $ENV{'QUARTUS_ROOTDIR_OVERRIDE'};
    if (!defined $bsp_quartus_version){
      if (!defined $quartus_rootdir_override) {
        return "Target board-package does not specify Quartus version. Please ensure you are targetting the correct board or use QUARTUS_ROOTDIR_OVERRIDE environment variable.";
      }
      return;
    }

    my $env_variable_sep = acl::Env::is_windows()? ';' : ':';
    my $quartus_rootdir = File::Spec->catfile($intel_fpga_root, 'QuartusPrimePro', $bsp_quartus_version, 'quartus');
    if ( -e $quartus_rootdir and -d $quartus_rootdir){
      my $java = acl::Env::is_windows()? 'bin64' : 'linux64';
      my $quartus_java = File::Spec->catfile($quartus_rootdir, $java, 'jre64', 'bin');
      if ( -e $quartus_java and -d $quartus_java){
        $ENV{'PATH'} = $quartus_java.$env_variable_sep.$ENV{'PATH'};
      }
      return if $set_quartus_java_only;

      $ENV{'QUARTUS_ROOTDIR_OVERRIDE'} = $quartus_rootdir;
      $ENV{'QUARTUS_ROOTDIR'} = $quartus_rootdir;
    }else{
      if (!defined $quartus_rootdir_override) {
        return "Could not find Quartus $bsp_quartus_version in Intel FPGA Add-on installation.";
      }
      return; # $quartus_rootdir is not a valid dir
    }

    my $quartus_bindir =  File::Spec->catfile($quartus_rootdir, 'bin');
    if ( -e $quartus_bindir and -d $quartus_bindir){
      # The Quartus must have the correct patches
      my $quartus_bin_path = File::Spec->catfile($quartus_bindir, 'quartus_sh');
      if ( !quartus_has_correct_patches($quartus_bin_path) ){
        my $required_patches = acl::Board_env::get_quartus_patches();
        my $err_msg = "The patches required to compile for the target board ($required_patches) is not installed for the following Quartus: \n$quartus_bin_path\n";
        acl::Common::mydie($err_msg);
      }
      $ENV{'QUARTUS_64BIT'} = 1;
      $ENV{'PATH'} = $quartus_bindir.$env_variable_sep.$ENV{'PATH'};
      $ENV{'LD_LIBRARY_PATH'} = $quartus_bindir.$env_variable_sep.$ENV{'LD_LIBRARY_PATH'};
    }

    my $qsys_bin = File::Spec->catfile($quartus_rootdir, '..', 'qsys', 'bin');
    if ( -e $qsys_bin and -d $qsys_bin){
      $ENV{'PATH'} = $qsys_bin.$env_variable_sep.$ENV{'PATH'};
    }

    my $sopc_builder = File::Spec->catfile($quartus_rootdir, 'sopc_builder', 'bin');
    if ( -e $sopc_builder and -d $sopc_builder){
      $ENV{'PATH'} = $sopc_builder.$env_variable_sep.$ENV{'PATH'};
    }
  }
}


# check quartus for HW flow. Currently only emulator doesn't need quartus
sub check_quartus_env() {

  my $verbose = acl::Common::get_verbose();

  my %q_info;
  #skip the quartus check for emulator flow
  if (not $emulator_flow)
  {
    my $error_string = setup_quartus_env(0);

    # Is Quartus on the path?
    $ENV{QUARTUS_OPENCL_SDK}=1; #Tell Quartus that we are OpenCL
    my $q_out = acl::Common::mybacktick('quartus_sh --version');
    $QUARTUS_VERSION = $q_out;

    chomp $q_out;
    if ($q_out eq "") {
      if ($sycl_mode) {
        if (defined $ENV{'INTEL_FPGA_ROOT'} and !$ENV{'INTEL_FPGA_ROOT'} eq "" ) {
          if (defined $error_string and !$error_string eq "") {
            print STDERR "$prog: error: Quartus could not be found. $error_string\n";
          } else {
            print STDERR "$prog: error: Quartus could not be found. Please run sys_check to ensure Intel(R) FPGA Add-on for oneAPI Base Toolkit is installed correctly.\n";
          }
        } else {
          print STDERR "$prog: error: Quartus could not be found. Please ensure Intel(R) FPGA Add-on for oneAPI Base Toolkit is installed.\n";
        }
      } else {
        print STDERR "$prog: Quartus is not on the path!\n";
        print STDERR "$prog: Is it installed on your system and quartus bin directory added to PATH environment variable?\n";
      }
      exit 1;
    }
    my $quartus_location = acl::File::which_full ("quartus_sh"); chomp $quartus_location;
    print "Quartus location: ".$quartus_location."\n" if acl::Common::get_verbose();

    # Is it right Quartus version?
    my $q_ok = 0;
    $q_info{version} = "";
    $q_info{pro} = 0;
    $q_info{prime} = 0;
    $q_info{internal} = 0;
    $q_info{site} = '';
    my $req_qversion_str = "20.3.0";
    my $req_qversion = acl::Env::get_quartus_version($req_qversion_str);

    foreach my $line (split ('\n', $q_out)) {
      # With QXP flow should be compatible with future versions

      # Do version check.
      my ($qversion_str) = ($line =~ m/Version (\S+)/);
      $q_info{version} = acl::Env::get_quartus_version($qversion_str);
      if(acl::Env::are_quartus_versions_compatible($req_qversion, $q_info{version})) {
        $q_ok++;
      }

      # check if Internal version
      if ($line =~ /Internal/) {
        $q_info{internal}++;
      }

      # check which site it is from
      if ($line =~ m/\s+([A-Z][A-Z])\s+/) {
        $q_info{site} = $1;
      }

      # Need this to bypass version check for internal testing with ACDS 15.0.
      if ($line =~ /Prime/) {
        $q_info{prime}++;
      }
      if ($line =~ /Pro Edition/) {
        $q_info{pro}++;
        $is_pro_mode = 1;
      }
    }
    if ($do_env_check && $q_ok != 1) {
      my $min_compatible_qver = acl::Env::get_min_compatible_quartus_ver($req_qversion);
      print STDERR "$prog: The Intel(R) FPGA SDK for OpenCL(TM) version $req_qversion_str only supports Intel(R) Quartus(R) Prime versions $min_compatible_qver to $req_qversion_str";
      print STDERR ". However, an unsupported Intel(R) Quartus(R) Prime version was found: \n$q_out\n";
      exit 1;
    }
    if ($do_env_check && $q_info{prime} == 1 && $q_info{pro} != 1) {
      print STDERR "$prog: This release of the Intel(R) FPGA SDK for OpenCL(TM) requires Quartus Prime Pro Edition.";
      print STDERR " However, the following version was found: \n$q_out\n";
      exit 1;
    }

    # Is it Quartus Prime Standard or Pro device?
    if ($do_env_check) {
      if (($q_info{prime} == 1) && ($q_info{pro} == 1) && ($target_model !~ /^arria10/ && $target_model !~ /^stratix10/ && $target_model !~ /^cyclone10/ && $target_model !~ /^agilex/)) {
        print STDERR "$prog: Use Quartus Prime Standard Edition for non-A10/S10/C10GX/Agilex devices.";
        print STDERR " Current Quartus Version is: \n$q_out\n";
        exit 1;
      }
    }
  }

  return %q_info;
}

# check if the object (aoco) or rtl (aocr) file is for the certain flow
# internal caller need to pass the flow name
# emulator:
#  - fast emulator:   emulator_fast
#  - legacy emulator: emulator
# simulator:          simulator
# HW:                 fpga
# library:            library
#
# return true if flow matches, otherwise false
sub check_input_file_for_compile_flow($$) {
  my ($input_file, $flow_name) = @_;
  my $pkg = get acl::Pkg($input_file) or acl::Common::mydie("Cannot find pkg file $input_file: $acl::Pkg::error\n");
  $pkg->exists_section('.acl.target') or acl::Common::mydie("Cannot determine the compile flow of $input_file\n");
  my $target_flow_name = $pkg->get_pkg_section('.acl.board');
  if ($target_model eq $flow_name) {
    return 1;
  } else {
    return 0;
  }
}

# Check that the .aoco file was compiled with the same aoc version as the current version
# (dont want to continue with an aoco compiled with a different aoc version)
sub check_aoco_file_version {
  my $aoco_file = shift;
  my $pkg = get acl::Pkg($aoco_file) or acl::Common::mydie("Cannot find pkg file $aoco_file: $acl::Pkg::error\n");
  my $aoco_version = get_pkg_section($pkg, '.acl.version') or acl::Common::mydie("The $aoco_file file is invalid or compiled using an old version of the Intel(R) FPGA SDK for OpenCL(TM).\n Please recompile the aoco with the currently installed SDK. \n");
  my $sdk_version = acl::Env::sdk_version();
  if ($aoco_version != $sdk_version){
    my $file_basename = acl::File::mybasename($aoco_file);
    acl::Common::mydie("aoc: The $file_basename file was compiled using a different release of the Intel(R) FPGA SDK for OpenCL(TM) than the one currently installed.\n Current version is $sdk_version, while the .aoco was compiled with $aoco_version. \n Please recompile the .aoco with the currently installed SDK. \n");
    return 0;
  }
  return 1;
}

sub progress_message($) {
  my $msg = shift;
  if (($sycl_mode and acl::Common::get_verbose()) or (!$sycl_mode and !acl::Common::get_quiet_mode())) {
    print $msg;
  }
}

sub cleanup_object_files(){
  # remove the generated aoco files for the rtl and hw flow if -save-temps and -c and -share is not provided
  my $save_temps = acl::Common::get_save_temps();
  if (not $save_temps and not $c_flag_only and not $created_shared_aoco) {
    # delete the intermediate object file for cleaning up the output
    foreach my $obj_file (@objfile_list) {
      if (not grep( /^$obj_file$/, @given_input_files)
          and not grep( /^$obj_file$/, @output_objfile_list)
          and -e $obj_file ) {
        # only when the object file is not given by user input and is generated on the fly as intermediate file will be deleted
        unlink($obj_file);
      }
    }
  }
}

1;
