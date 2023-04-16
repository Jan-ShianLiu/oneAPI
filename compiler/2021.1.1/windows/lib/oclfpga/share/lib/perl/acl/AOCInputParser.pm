
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

acl::AOCInputParser.pm - Process user input

=head1 VERSION

$Header: //depot/sw/hld/rel/20.3api.11/acl/sysgen/lib/acl/AOCInputParser.pm#1 $

=head1 DESCRIPTION

This module provides the method to parse user input. It also
contains the global variables that the compiler driver uses.

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


package acl::AOCInputParser;
use strict;
use Exporter;

require acl::Env;
require acl::File;
require acl::Simulator;
require acl::Report;
use acl::AOCDriverCommon;
use acl::Common;

our @ISA = qw(Exporter);
our @EXPORT_OK = qw ( parse_args process_args );

# Global variables
my $sycl_sim_mode = 0;

# Helper Functions

#Converting frequency to MHZ
my $qii_fmax_constraint = undef;
sub clk_get_exp {
    my $var = shift;
    my $exp = $var;
    $exp=~ s/[\.0-9 ]*//;
    return $exp;
}

sub clk_get_mant {
    my $var = shift;
    my $mant = $var;
    my $exp = clk_get_exp($mant);
    $mant =~ s/$exp//g;
    return $mant;
}

sub clk_get_fmax {
    my $clk = shift;
    my $exp = clk_get_exp($clk);
    my $mant = clk_get_mant($clk);

    my $fmax = undef;

    if ($exp =~ /^GHz/) {
        $fmax = 1000000000 * $mant;
    } elsif ($exp =~ /^MHz/) {
        $fmax = 1000000 * $mant;
    } elsif ($exp =~ /^kHz/) {
        $fmax = 1000 * $mant;
    } elsif ($exp =~ /^Hz/) {
        $fmax = $mant;
    } elsif ($exp =~ /^ms/) {
        $fmax = 1000/$mant;
    } elsif ($exp =~ /^us/) {
        $fmax = 1000000/$mant;
    } elsif ($exp =~ /^ns/) {
        $fmax = 1000000000/$mant;
    } elsif ($exp =~ /^ps/) {
        $fmax = 1000000000000/$mant;
    } elsif ($exp =~ /^s/) {
        $fmax = 1/$mant;
    }
    if (defined $fmax) {
        $fmax = $fmax/1000000;
    }
    return $fmax;
}


# Deal with multiple specified source files
sub _process_input_file_arguments {
  my $num_extracted_c_model_files = shift;
  my $verbose = acl::Common::get_verbose();
  if ($#given_input_files == -1) {
    # No input files are given
    return "";
  }

  # Only multiple .cl or .aoco files are allowed. Can't mix
  my %suffix_cnt = ();
  foreach my $gif (@given_input_files) {
    my $suffix = $gif;
    $suffix =~ s/.*\.//;
    $suffix =~ tr/A-Z/a-z/;
    $suffix_cnt{$suffix}++;
  }

  # Error checks, even for one file

  if ($suffix_cnt{'c'} > 0) {
    # Pretend we never saw it i.e. issue the same message as we would for
    # other not recognized extensions. Not the clearest message,
    # but at least consistent
    acl::Common::mydie("No recognized input file format on the command line");
  }

  # If multiple aocr file is given as input then error out
  if ($suffix_cnt{'aocr'} > 1) {
    acl::Common::mydie("Cannot compile more than one .aocr file. \n");
  }

  # If multiple spv file is given as input then error out
  if ($suffix_cnt{'spv'} > 1) {
    acl::Common::mydie("Cannot compile more than one .spv file. \n");
  }

  # If have multiple files, they should either all be .cl files or all be .aoco files
  if ($#given_input_files > 0 and
      (($suffix_cnt{'cl'} < $#given_input_files+1) and ($suffix_cnt{'aoco'} < $#given_input_files+1))) {
    # Have some .cl files but not ALL .cl files. Not allowed.
    acl::Common::mydie("If multiple input files are specified, either all must be .cl files or all must be .aoco files .\n");
  }

  # check if the input file contains invalid characters
  # we only support the alphanumeric characters, dash, underscore and dot as valid char.
  foreach my $infile (@given_input_files) {
    acl::AOCDriverCommon::filename_validation($infile);
  }

  # Make sure aoco file is not an HDL component package
  if ($suffix_cnt{'aoco'} > 0) {
    $aoco_to_aocr_aocx_only = 1;
    foreach my $object (@given_input_files) {
      system(acl::Env::sdk_pkg_editor_exe(), $object, 'exists', '.comp_header');
      if ($? == 0) {
        acl::Common::mydie("$object is a HDL component package. It cannot be used by itself to do hardware compiles!\n");
      }
    }
  }

  # If aocr file is given as input then move directly to third step (quartus)
  if ($suffix_cnt{'aocr'} eq 1) {
    $aocr_to_aocx_only = 1;
  }

  # if library(ies) are specified,
  # extract all C model files and add them to the input file list.
  if ($emulator_flow and $#resolved_lib_files > -1) {
    # C model files from libraries will be extracted to this folder
    my $c_model_folder = ".emu_models";
    acl::File::remove_tree $c_model_folder;
    mkdir $c_model_folder or die $!;

    my @source_files;
    foreach my $libfile (@resolved_lib_files) {
        my $new_files = `$aocl_libedit_exe extract_c_models \"$libfile\" $c_model_folder`;
      push @source_files, split /\n/, $new_files;
    }

    # Add library source files to the end of file list.  They must go at the end
    # because in process_args(), if an output file is specified, we use the
    # output file name to generate the name of the first object file.  If the
    # specified input and output files have the same basename, this can lead to
    # a collision in the case where the library c-model files appear at the
    # front of the list of input files (and therefore object files).
    # e.g. aoc -l mylib.aoclib foo.cl -o foo.aocr
    # mylib.aoclib:c_model -> foo.aoco (from output file basename)
    # foo.cl -> foo.aoco (from input file basename)
    if ($verbose) {
      print "All OpenCL C models were extracted from specified libraries and added to compilation\n";
    }
    $$num_extracted_c_model_files = scalar @source_files;
    @given_input_files = (@given_input_files, @source_files);
  }

  # Make 'base' name for all naming purposes (subdir, aoco/aocx files) to
  # be based on the last source file. Otherwise, it will be __all_sources,
  # which is unexpected.
  my $last_src_file = $given_input_files[-1];

  return acl::File::mybasename($last_src_file);
}



sub _usage() {
  my $default_board_text;
  my $board_env = &acl::Board_env::get_board_path() . "/board_env.xml";
  my @help_list;

  # In help_list, for each pair, the first item is used to indicate
  # whether it should be displayed in SYCL mode:
  #  - 0 = OpenCL Only
  #  - 1 = OpenCL as is, SYCL but prepend -Xs if starting with "-"
  #  - 2 = SYCL only but preprend -Xs if starting with "-"
  push(@help_list, [0, "aoc -- Intel(R) FPGA SDK for OpenCL(TM) Kernel Compiler\n"]);
  push(@help_list, [2, "aoc -- oneAPI Intel(R) FPGA Compiler Backend\n"]);
  push(@help_list, [0, "Usage: aoc <options> <file>.[cl|aoco|aocr]\n"]);
  push(@help_list, [2, "Usage: aoc <options> <file>.[aocr]\n"]);
  push(@help_list, [0, "Outputs:
       <file>.aocx - FPGA programming file, OR
       <file>.aocr - RTL file (intermediate), OR
       <file>.aoco - object file (intermediate)\n"]);
  push(@help_list, [2, "Outputs:
       <file>.aocx - FPGA programming file, OR
       <file>.aocr - RTL file (intermediate)\n"]);
  my $examples = "Examples:
       aoc mykernels.cl
              Compile kernel code into .aocx programming file.  This is the
              full end-to-end compile flow.

       aoc -rtl mykernels.cl
       aoc mykernels.aocr
              First, compile kernel code into .aocr and generate compiler
              reports.
              Second, compile RTL into .aocx programming file for execution
              in hardware.

       aoc -c mykernels.cl
       aoc mykernels.aoco
              First, compile kernel code to .aoco object file.
              Second, compile object file to .aocx programming file.

       aoc -c mykernels1.cl
       aoc -c mykernels2.cl
       aoc -rtl mykernels1.aoco mykernels2.aoco -o mykernels.aocr
       aoc mykernels.aocr
              First, compile two seperate kernel .cl files into .aoco object
              files.
              Second, combine object files into .aocr RTL programming file
              and generate compiler reports.
              Third, compile RTL into .aocx programming file for execution
              in hardware.";
  push(@help_list, [0, $examples]);
  push(@help_list, [1, "-version
            Print out version infomation and exit\n"]);
  push(@help_list, [1, "-v
            Verbose mode. Report progress of compilation\n"]);
  push(@help_list, [1, "-q
            Quiet mode. Progress of compilation is not reported\n"]);
  push(@help_list, [1, "-report
            Print area estimates to screen after initial compilation. The report
            is always written to the log file.  This option only has an effect
            during the RTL generation stage (generally this means generating an
            '.aocr' or '.aocx' file).\n"]);
  push(@help_list, [0, "-h
-help
            Show this message\n"]);
  push(@help_list, [1, "Overall Options:\n"]);
  push(@help_list, [0, "-c
            Stop after generating a <file>.aoco\n"]);
  push(@help_list, [1, "-rtl
            Stop after generating reports and a <file>.aocr\n"]);
  push(@help_list, [1, "-o <output>
            Use <output> as the name for the output.
            If running with the '-c' option the output file extension should be
            '.aoco'; if running with the '-rtl' option the output file extension
            should be '.aocr'.  Otherwise the file extension should be '.aocx'.
            If no extension is specified, the appropriate extension will be added
            automatically.\n"]);
  push(@help_list, [0, "-g
            Add debug data to kernels. Also, makes it possible to symbolically
            debug kernels created for the emulator on an x86 machine (Linux only).
            This behavior is enabled by default. This flag may be used to override
            the -g0 flag.\n"]);
  push(@help_list, [0, "-g0
            Don't add debug data to kernels.\n"]);
  push(@help_list, [0, "-ghdl
            Enable collection of simulation waveforms when using march=simulator.\n"]);
  push(@help_list, [2, "-profile
            Enable profile support when generating aocx file
            Note that this does have a small performance penalty since profile
            counters will be instantiated and take some FPGA resources.\n"]); # Hiding autorun option from SYCL until autorun kernels are enabled -- HSD-ES Case:1409984863
  push(@help_list, [0, "-profile(=<all|autorun|enqueued>)
          Enable profile support when generating aocx file:
          all: profile all kernels.
          autorun: profile only autorun kernels.
          enqueued: profile only non-autorun kernels.
          If there is no argument provided, then the mode defaults to 'all'.
          Note that this does have a small performance penalty since profile
          counters will be instantiated and take some FPGA resources.\n"]);
  push(@help_list, [1, "-profile-shared-counters
          Enable shared counters for profiling when generating aocx file.
          Profiling must be enabled (include profile flag) to enable shared counters.\n"]);
  push(@help_list, [0, "-march=<emulator|simulator>
            emulator: create kernels that can be executed on x86
            simulator: create kernels that can be executed by ModelSim\n"]);
  push(@help_list, [1, "-L <directory>
            Add directory to OpenCL library search path.\n"]);
  push(@help_list, [1, "-l <library.aoclib>
            Specify OpenCL library file.\n"]);
  push(@help_list, [0, "Optimization Control:\n"]);
  push(@help_list, [0, "-board=<board name>
            Compile for the specified board. $default_board_text\n"]);
  push(@help_list, [0, "-list-board-packages
            Prints a list of installed board packages, as well as the available
            board packages shipped with the SDK.\n"]);
  push(@help_list, [0, "-list-boards
            Prints the board variants that the compiler can target.  This
            includes boards from both installed board packages and any board
            packages shipped with the SDK.  When used in conjunction with
            -board-package=<board package name or path>, only board variants from the
            specified board package are listed.
            The default board listed will be used for compilation if -board is
            not specified.\n"]);
  push(@help_list, [0, "-board-package=<board package name or path>
            Specify the name or path of board package to use for compilation. For
            example: -board-package=/home/user/intel_opencl/board/a10_ref, or
                     -board-package=a10_ref
            If none given, the compiler will attempt to resolve the board package based
            on the -board argument.
            This argument is required when multiple installed board packages
            include board variants with the same name.
            Note that board packages are available as a separate download.\n"]);
  push(@help_list, [1, "-bsp-flow=<flow name>
            Specify the bsp compilation flow by name. If none given, the board's
            default flow is used.\n"]);
  push(@help_list, [0, "Incremental Compilation:\n"]);
  push(@help_list, [0, "-incremental[=aggressive]
            Enable incremental compilation mode, preserving sections of the
            design in partitions for future compilations to reduce compile time.

            Incremental compilation reduces compilation time but degrades
            efficiency. Use this feature for internal development only.

            Aggressive incremental compilation mode enables more extensive
            preservation techniques to reduce compilation time at the cost of
            further efficiency degradation.\n"]);
  push(@help_list, [0, "-incremental-input-dir=<path>
            Specify the location of the previous incremental compilation project
            directory, to be used as this compilation's base. If this flag is not
            specified, aoc will look in the default project directory.\n"]);
  push(@help_list, [0, "-incremental-flow=[retry|no-retry]
            Control how the OpenCL compiler reacts to compilation failures in
            incremental compilation mode. Default: retry.

            retry:    In the event of a compilation failure, recompile the project
            without using previously preserved kernel partitions.

            no-retry: Do not retry upon experiencing a compilation failure.\n"]);
  push(@help_list, [0, "-incremental-grouping=<partition file>
            Specify how aoc should group kernels into partitions. Each line
            specifies a new partition with a semicolon (;) delimited list of
            kernel names. Each unspecified kernel will be assigned its own
            partition.\n"]);
  push(@help_list, [1, "Optimization Control:\n"]);
  push(@help_list, [1, "-clock=<clock_spec>
            Optimize the design for the specified clock frequency or period.\n"]);
  push(@help_list, [1, "-parallel=<num_procs>
            Set the degree of parallelism used in the Quartus compile.\n"]);
  push(@help_list, [1, "-seed=<value>
            Run the Quartus compile with a seed value of <value>. Default is '1'.\n"]);
  push(@help_list, [1, "-no-interleaving=<global memory name>
            Configure a global memory as separate address spaces for each
            DIMM/bank.  User should then use the Intel specific cl_mem_flags
            (E.g.  CL_CHANNEL_2_INTELFPGA) to allocate each buffer in one DIMM or
            the other. The argument 'default' can be used to configure the default
            global memory. Consult your board's documentation for the memory types
            available. See the Best Practices Guide for more details.\n"]);
  push(@help_list, [1, "-global-ring
            Force use of the ring topology for the global memory interconnect.
            This may improve kernel fmax and possibly increase area.
            See Programming Guide and Best Practices Guide for more details.\n"]);
  push(@help_list, [1, "-force-single-store-ring
            When the ring topology is used for the global memory interconnect, force
            only one ring for global store operations to be created (the default behaviour
            is to create one store ring per global memory interface). This will reduce the
            available global memory store bandwidth, but save area. See Programming Guide
            and Best Practices Guide for more details.\n"]);
    push(@help_list, [1, "-ring-root-arb-balanced-rw
            When the ring topology is used for the global memory interconnect, force
            a balanced priority between read and write requests (the default behaviour
            is to prioritize writes over reads). This will increase the latency to
            resolve global memory dependencies but may improve global memory
            throughput in some applications. See Programming Guide
            and Best Practices Guide for more details.\n"]);
  push(@help_list, [1, "-const-cache-bytes=<N>
            Configure the constant cache size (rounded up to closest 2^n).
            If none of the kernels use the __constant address space, this
            argument has no effect.\n"]);
  push(@help_list, [2, "-fp-relaxed
            Allow the compiler to relax the order of arithmetic operations,
            possibly affecting the precision\n"]);
  push(@help_list, [0, "-ffp-reassoc
            Allow the compiler to relax the order of arithmetic operations,
            possibly affecting the precision\n"]);
  push(@help_list, [0, "-ffp-contract=fast
            Removes intermediary roundings and conversions when possible,
            and changes the rounding mode to round towards zero for
            multiplies and adds\n"]);
  push(@help_list, [2, "-fpc
            Removes intermediary roundings and conversions when possible,
            and changes the rounding mode to round towards zero for
            multiplies and adds\n"]);
  push(@help_list, [1, "-daz
            Flushes double precision denormalized numbers to zero.\n"]);
  push(@help_list, [1, "-rounding=<ieee|faithful>
            Set floating point rounding scheme:
            ieee     : Set FP rounding scheme to IEEE RNE (0.5 ULP)
            faithful : Set FP rounding scheme to faithful (1 ULP)\n"]);
  push(@help_list, [1, "-fast-compile
            Compiles the design with reduced effort for a faster compile time but
            reduced fmax and lower power efficiency. Compiled aocx should only be
            used for internal development and not for deploying in final product.\n"]);
  push(@help_list, [1, "-high-effort
            Increases aocx compile effort to improve ability to fit
            kernel on the device.\n"]);
  push(@help_list, [1, "-no-hardware-kernel-invocation-queue
            Reduce area by removing kernel invocation queue in OpenCL kernels.
            Setting this flag may increase kernel execution time.\n"]);
  push(@help_list, [1, "-hyper-optimized-handshaking=<auto|off>
            Disabling hyper-optimized handshaking may reduce area at the cost of lower fmax. Currently only applicable for Stratix 10:
            auto:         Enable hyper-optimized handshaking if possible (default)
            off:          Disable hyper-optimized handshaking\n"]);
  push(@help_list, [2, "-no-accessor-aliasing
            Informs the compiler that pointer arguments in all kernels and all
            pointers derived from them never refer to the same memory location.\n"]);
  push(@help_list, [2, "-disable-auto-loop-fusion
            Informs the compiler to disable automatic loop fusion.\n"]);
  push(@help_list, [2, "-enable-unequal-tc-fusion
            Informs the compiler to enable fusing loops with unequal trip counts.\n"]);

  for (my $i = 0; $i < @help_list; $i++) {
    if (($help_list[$i][0] != 2 and $sycl_mode == 0) or
        ($help_list[$i][0] != 0 and $sycl_mode == 1)) {
      # Prepend -Xs for sycl options
      $help_list[$i][1] =~ s/^-/-Xs/ if ($sycl_mode);
      print $help_list[$i][1] . "\n";
    }
  }

  if (-e $board_env) {
    my $default_board;
    ($default_board) = &acl::Env::board_hardware_default();
    $default_board_text = "Default is $default_board.";
  } else {
    $default_board_text = "Cannot find default board location or default board name.";
  }


  if($sycl_mode == 0){
    print <<STDOPTIONS;
OpenCL Standard Options:

-D <name>
-D <name>=<definition>
-I <dir>
-cl-single-precision-constant
-cl-denorms-are-zero
-cl-opt-disable
-cl-strict-aliasing
-cl-mad-enable
-cl-no-signed-zeros
-cl-unsafe-math-optimizations
-cl-finite-math-only
-cl-fast-relaxed-math
-W
-Werror
-cl-std=<CL1.1|CL1.2>
-cl-kernel-arg-info
           OpenCL required options. See OpenCL specification for clBuildProgram.
STDOPTIONS
  }
#-initial-dir=<dir>
#          Run the parser from the given directory.
#          The default is to run the parser in the current directory.

#          Use this option to properly resolve relative include
#          directories when running the compiler in a directory other
#          than where the source file may be found.
#-save-extra
#          Save kernel program source, optimized intermediate representation,
#          and Verilog into the program package file.
#          By default, these items are not saved.
#
#-no-env-check
#          Skip environment checks at startup.
#          Use this option to save a few seconds of runtime if you
#          already know the environment is set up to run the Intel(R) FPGA SDK
#          for OpenCL(TM) compiler.
#-dot
#          Dump out DOT graph of the kernel pipeline.

}


sub _powerusage() {
  print <<POWERUSAGE;

aoc -- Intel(R) FPGA SDK for OpenCL(TM) Kernel Compiler

Usage: aoc <options> <file>.[cl|aoco]

Help Options:

-powerhelp
          Show this message

Modifiers:
-dsploc=<compile directory>
          Extract DSP locations from given <compile directory> post-fit netlist
          and use them in current Quartus compile

-ramloc=<compile directory>
          Extract RAM locations from given <compile directory> post-fit netlist
          and use them in current Quartus compile

-timing-threshold=<slackvalue>
          Allow the compiler to generate an error if the slack from quartus STA
          is more than the value specified

POWERUSAGE

}

# Check $arg against $match_string; warn if deprecated double '-' version used.
sub _process_arg {
  my ($arg, $match_string) = @_;
  if ( ($arg eq '-'.$match_string) or ($arg eq '--'.$match_string) ) {
    if ($arg eq '--'.$match_string) {
      print "Warning: Command has been deprecated. Please use -$match_string instead of --$match_string\n";
    }
    return 1;
  }
  return 0;
}

# Check $arg against $match_string; issue warning about deprecated flag use and
# suggest using -<arg>=<value> version. Error out if value not found or invalid.
sub _process_arg_with_value {
  my ($arg, $input_argv_ref, $match_string, $value, $value_may_be_flag, $requirement) = @_;
  my @input_argv = @{ $input_argv_ref };
  # Generally we disallow values that start with '-', but some options require
  # values that begin with '-', e.g. -opt-arg.
  $value_may_be_flag = (defined $value_may_be_flag) ? $value_may_be_flag : 0;
  $requirement = 'an argument' if not defined $requirement;
  if ( ($arg eq '-'.$match_string) or ($arg eq '--'.$match_string) ) {
    print "Warning: Command has been deprecated. Please use -$match_string=<$value> instead of $arg <$value>\n";
    ($#input_argv >= 0 and ($value_may_be_flag or $input_argv[0] !~ m/^-./)) or acl::Common::mydie("Option $arg requires $requirement");
    return 1;
  }
  return 0;
}

# Some aoc args translate to args to many underlying exes.
sub _process_meta_args {
  my ($cur_arg, $argv) = @_;
  my $processed = 0;
  if ( _process_arg($cur_arg, '1x-clock-for-local-mem') ) {
    # TEMPORARY: don't actually enforce this flag
    #$opt_arg_after .= ' -force-1x-clock-local-mem';
    #$llc_arg_after .= ' -force-1x-clock-local-mem';
    $processed = 1;
  }
  elsif ( _process_arg($cur_arg, 'sw_dimm_partition') or _process_arg($cur_arg, 'sw-dimm-partition') ) {
    # TODO need to do this some other way
    # this flow is incompatible with the dynamic board selection (--board)
    # because it overrides the board setting
    $sysinteg_arg_after .= ' --cic-global_no_interleave ';
    $llc_arg_after .= ' -use-swdimm=default';
    $processed = 1;
  }

  return $processed;
}

# Exported Functions

sub parse_args {

  my ( $args_ref,
       $bsp_flow_name_ref,
       $regtest_bak_cache_ref,
       $incremental_input_dir_ref,
       $verbose_ref,
       $quiet_mode_ref,
       $save_temps_ref,
       $sim_accurate_memory_ref,
       $sim_kernel_clk_frequency_ref,
       @input_argv ) = @_;

  my $print_help = 0;
  my $print_power_help = 0;
  while (@input_argv) {
    my $arg = shift @input_argv;

    # case:492114 treat options that start with -l as a special case.
    # By putting this code at the top we enforce that all options
    # starting with -l must be added to the l_opts_exclude array or else
    # they won't work because they'll be treated as a library name.
    if ( ($arg =~ m!^-l(\S+)!) ) {
      my $full_opt = '-l' . $1;
      my $excluded = 0;

      # If you add an option that starts with -l you must update the
      # l_opts_exclude list.
      foreach my $opt_name (@acl::Common::l_opts_exclude) {
        if ( ($full_opt =~ m!^$opt_name!) ) {
          # Options on the exclusion list are parsed in the long
          # if/elsif chain below like every other option.
          $excluded = 1;
          last;
        }
      }

      # -l<libname>
      if (!$excluded) {
          push (@lib_files, $1);
          next;
      }
    }

    # -h / -help
    if ( ($arg eq '-h') or _process_arg($arg, 'help') ) {
      $print_help = 1;
    }
    # -powerhelp
    elsif ( _process_arg($arg, 'powerhelp') ) {
      $print_power_help = 1;
    }
    # -version / -V
    elsif ( ($arg eq '-V') or _process_arg($arg, 'version') ) {
      acl::AOCDriverCommon::version(\*STDOUT);
      exit 0;
    }
    # -list-deps
    elsif ( _process_arg($arg, 'list-deps') ) {
      print join("\n",values %INC),"\n";
      exit 0;
    }
    # -list-board-packages
    elsif ( _process_arg($arg, 'list-board-packages') ) {
      acl::Common::list_packages();
      exit 0;
    }
    # handle the case of -list-board-package as a typo
    elsif ( ($arg eq '-list-board-package') or ($arg eq '--list-board-package') ) {
      print "error: unrecognized command line option \'$arg\'; did you mean \'-list-board-packages\'?\n";
      exit 1;
    }
    # -list-boards
    elsif ( _process_arg($arg, 'list-boards') ) {
      acl::Common::list_boards($bsp_variant);
      exit 0;
    }
    # handle the case of -list-board as a typo
    elsif ( ($arg eq '-list-board') or ($arg eq '--list-board') ) {
      print "error: unrecognized command line option \'$arg\'; did you mean \'-list-boards\'?\n";
      exit 1;
    }
    # -v
    elsif ( ($arg eq '-v') ) {
      $$verbose_ref += 1;
      acl::Common::set_verbose($$verbose_ref);
      if ($$verbose_ref > 1) {
        $prog = "#$prog";
      }
    }
    # -q
    elsif ( ($arg eq '-q') ) {
      $$quiet_mode_ref = 1;
      acl::Common::set_quiet_mode($$quiet_mode_ref);
    }
    # -hw
    elsif ( _process_arg($arg, 'hw') ) {
      $run_quartus = 1;
    }
    # -quartus
    elsif ( _process_arg($arg, 'quartus') ) {
      $skip_qsys = 1;
      $run_quartus = 1;
    }
    # -standalone
    elsif ( ($arg eq '-standalone') or ($arg eq '--standalone') ) {
      # Don't complain about --standalone since aoc wrapper doesn't accept -standalone.
      $standalone = 1;
    }
    # -d
    elsif ( ($arg eq '-d') ) {
      $debug = 1;
    }
    # -march=simulator
    elsif ($arg eq '-march=simulator') {
      $new_sim_mode = 1;
      $user_defined_flow = 1;
      $ip_gen_only = 1;
      $atleastoneflag = 1;
    }
    # -sycl
    elsif ($arg eq '-sycl'){
      $sycl_mode = 1;
    }
    elsif ($arg =~ /-output-report-folder(=(\S+))?/){
      $change_output_folder_name = acl::File::abs_path($2);
    }
    # -simulation
    elsif ($arg eq '-simulation'){
      $sycl_sim_mode = 1;
    }
    # -hardware
    elsif ($arg eq '-hardware'){
      $sycl_hw_mode = 1;
    }
    # -ghdl / -ghdl=<value>
    elsif ($arg =~ /-ghdl(=(\S+))?/) {
      acl::Simulator::set_sim_debug(1);
      if (defined $2) {
        # error check for 0
        my $depth_val = $2;
        if ($depth_val =~ /\d+/ && $depth_val > 0) {
          acl::Simulator::set_sim_debug_depth($depth_val);
        }
        else {
          acl::Common::mydie("Option -ghdl= requires an integer argument greater than or equal to 1\n");
        }
      }
      else {
        acl::Simulator::set_sim_debug_depth(undef);
      }
    }
    # -sim-acc-mem  Hidden option for accurate memory model from the board
    elsif ( $arg eq '-sim-acc-mem' ) {
      $$sim_accurate_memory_ref = 1;
      acl::Simulator::set_sim_accurate_memory($$sim_accurate_memory_ref);
    }
    # -sim-clk-freq <value>  Hidden option for simulating kernel system with a different frequency in MHz
    elsif ( _process_arg_with_value($arg, \@input_argv, 'sim-clk-freq', 'option', 1) ) {
      my $argument_value = shift @input_argv;
      if (!defined($argument_value)) {
        acl::Common::mydie("Option -sim-clk-freq requires an argument with value between 100 and 1000\n");
      } elsif ($argument_value < 100 || $argument_value > 1000) {
        # do some error checking, i.e. a number between 100 and 1000
        acl::Common::mydie("Option -sim-clk-freq value must be between 100 and 1000\n");
      } else {
        $$sim_kernel_clk_frequency_ref = $argument_value;
        acl::Simulator::set_sim_kernel_clk_frequency($$sim_kernel_clk_frequency_ref);
      }
    }
    # -sim-clk-freq=<value>  Hidden option for simulating kernel system with a different frequency in MHz
    elsif ( $arg =~ /-sim-clk-freq(=(\d+))?/ ) {
      my $argument_value = $1;
      if (!defined($argument_value)) {
        acl::Common::mydie("Option -sim-clk-freq= requires an argument with value between 100 and 1000\n");
      } elsif ($2 < 100 || $2 > 1000) {
        # do some error checking, i.e. a number between 100 and 1000
        acl::Common::mydie("Option -sim-clk-freq= value must be between 100 and 1000\n");
      } else {
        $$sim_kernel_clk_frequency_ref = $2;
        acl::Simulator::set_sim_kernel_clk_frequency($$sim_kernel_clk_frequency_ref);
      }
    }
    # -sim-enable-warnings  Hidden option for internal regression test so suppressible
    # warnings are caught at compile stage, not run
    elsif ( $arg eq '-sim-enable-warnings' ) {
      acl::Simulator::set_sim_enable_warnings(1);
    }
    # -sim-elab-arg  Hidden option for external customer to workaround any last minute
    # simulator bugs introduced by compiler.
    elsif ( $arg =~ /-sim-elab-arg=(.*)$/ ) {
      my $argument_value = $1;
      acl::Simulator::set_sim_elab_options($argument_value);
    }
    # -sim-input-dir  Hidden option to avoid regenerating simulation files to save compile time
    elsif ( $arg =~ /-sim-input-dir(=(\S+))?/) {
      my $argument_value = $1;
      if (defined($argument_value)) {
        # overwrite the default simulation folder name
        my $sim_dir = $2;
        acl::Simulator::set_sim_dir_path($sim_dir);
      }
      else {
              acl::Simulator::set_sim_dir_path(undef);
      }
    }
    # -high-effort
    elsif ( _process_arg($arg, 'high-effort') ) {
      $high_effort = 1;
    }
    # -add-ini=file1,file2,file3,...
    elsif ( $arg =~ /^-add-ini=(.*)$/ ) {
      my @input_files = split(/,/, $1);
      $#input_files >= 0 or acl::Common::mydie("Option -add-ini= requires at least one argument");
      push @additional_ini, @input_files;
    }
    # -report
    elsif ( _process_arg($arg, 'report') ) {
      $report = 1;
    }
    # -g
    elsif ( ($arg eq '-g') ) {
      $dash_g = 1;
      $user_dash_g = 1;
    }
    # -g0
    elsif ( ($arg eq '-g0') ) {
      $dash_g = 0;
    }
    # -profile
    elsif ( _process_arg($arg, 'profile') ) {
      if ($sycl_mode == 0) {
        # Hiding profiling options from SYCL until autorun kernels are supported
        # HSD-ES Case:1409984863
        print "$prog: Warning: no argument provided for the option -profile, will enable profiling for all kernels by default\n";
      }
      $profile = 'all'; # Default is 'all'
      $save_last_bc = 1;
    }
    # -profile=<name>
    elsif ( ( $arg =~ /^-profile=(.*)$/ ) and ($sycl_mode == 0)) {
      # Hiding profiling options from SYCL until autorun kernels are supported
      # HSD-ES Case:1409984863
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -profile= requires an argument");
      } else {
        $profile = $argument_value;
        if ( !(($profile eq 'all' ) || ($profile eq 'autorun') || ($profile eq 'enqueued')) ) {
          print "$prog: Warning: invalid argument '$profile' for the option --profile, will enable profiling for all kernels by default\n";
          $profile = 'all'; # Default is "all"
        }
        $save_last_bc = 1;
      }
    }
    # -profile-shared-counters
    elsif ( _process_arg($arg, 'profile-shared-counters') ) {
      $profile_shared_counters = 1;
    }
    # -save-extra
    elsif ( _process_arg($arg, 'save-extra') ) {
      $pkg_save_extra = 1;
    }
    # -no-env-check
    elsif ( _process_arg($arg, 'no-env-check') ) {
      $do_env_check = 0;
    }
    # -no-auto-migrate
    elsif ( _process_arg($arg, 'no-auto-migrate') ) {
      $no_automigrate = 1;
    }
    # -initial-dir=<value>
    elsif ( $arg =~ /^-initial-dir=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -initial-dir= requires an argument");
      } else {
        $force_initial_dir = $argument_value;
        # orig_force_initial_dir stores the original value of this argument given by the user since
        # $force_initial_dir is eventually modified in other places of the AOC driver.
        $orig_force_initial_dir = $force_initial_dir;
      }
    }
    # -o <value>
    elsif ( ($arg eq '-o') ) {
      # Absorb -o argument, and don't pass it down to Clang
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option $arg requires a file argument.");
      $output_file = shift @input_argv;
    }
    # -hash <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'hash', 'value') ) {
      $program_hash = shift @input_argv;
    }
    # -hash=<value>
    elsif ( $arg =~ /^-hash=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -hash= requires an argument");
      } else {
        $program_hash = $argument_value;
      }
    }
    # -clang-arg <option>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'clang-arg', 'option', 1) ) {
      # Just push onto @$args_ref!
      push @$args_ref, shift @input_argv;
    }
    # -clang-arg=<option>
    elsif ( $arg =~ /^-clang-arg=(.*)$/ ) {
      my $argument_value = $1;
      $argument_value ne "" or acl::Common::mydie("Option -clang-arg=<value> requires an argument");
      push @$args_ref, $argument_value;
    }
    # -opt-arg <option>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'opt-arg', 'option', 1) ) {
      $opt_arg_after .= " ".(shift @input_argv);
    }
    # -opt-arg=<option>
    elsif ( $arg =~ /^-opt-arg=(.*)$/ ) {
      my $argument_value = $1;
      $argument_value ne "" or acl::Common::mydie("Option -opt-arg=<value> requires an argument");
      $opt_arg_after .= " " . $argument_value;
    }
    # -llc-arg <option>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'llc-arg', 'option', 1) ) {
      $llc_arg_after .= " ".(shift @input_argv);
    }
    # -llc-arg=<option>
    elsif ( $arg =~ /^-llc-arg=(.*)$/ ) {
      my $argument_value = $1;
      $argument_value ne "" or acl::Common::mydie("Option -llc-arg=<value> requires an argument");
      $llc_arg_after .= " " . $argument_value;
    }
    # -reuse-aocx=<name>
    elsif ( $arg =~ /^-reuse-aocx=(.*)$/ ) {
      my $argument_value = $1;
      $argument_value ne "" or acl::Common::mydie("Option -reuse-aocx=<aocx> requires a filename");
      -r $argument_value or acl::Common::mydie("Option -reuse-aocx=<aocx> requires the file $1 to exist");
      $reuse_aocx_file = acl::File::abs_path($argument_value);
    }
    # -reuse-exe=<name>
    elsif ( $arg =~ /^-reuse-exe=(.*)$/ ) {
      my $argument_value = $1;
      $argument_value ne "" or acl::Common::mydie("Option -reuse-exe=<exe> requires a filename");
      if (-r $argument_value) {
        $reuse_exe_file = acl::File::abs_path($argument_value);
      } else {
        print "warning: -reuse-exe file '$1' not found; ignored\n";
      }
    }
    # -optllc-arg <option>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'optllc-arg', 'option', 1) ) {
      my $optllc_arg = (shift @input_argv);
      $opt_arg_after .= " ".$optllc_arg;
      $llc_arg_after .= " ".$optllc_arg;
    }
    # -optllc-arg=<option>
    elsif ( $arg =~ /^-optllc-arg=(.*)$/ ) {
      my $argument_value = $1;
      $argument_value ne "" or acl::Common::mydie("Option -optllc-arg=<value> requires an argument");
      $llc_arg_after .= " " . $argument_value;
      $opt_arg_after .= " " . $argument_value;
    }
    # -sysinteg-arg <option>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'sysinteg-arg', 'option', 1) ) {
      $sysinteg_arg_after .= " ".(shift @input_argv);
    }
    # -sysinteg-arg=<option>
    elsif ( $arg =~ /^-sysinteg-arg=(.*)$/ ) {
      my $argument_value = $1;
      $argument_value ne "" or acl::Common::mydie("Option -sysinteg-arg=<value> requires an argument");
      $sysinteg_arg_after .= " " . $argument_value;
    }
    # -max-mem-percent-with-replication <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'max-mem-percent-with-replication', 'value') ) {
      $max_mem_percent_with_replication = (shift @input_argv);
    }
    # -max-mem-percent-with-replication=<value>
    elsif ( $arg =~ /^-max-mem-percent-with-replication=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -max-mem-percent-with-replication= requires an argument");
      } else {
        $max_mem_percent_with_replication = $argument_value;
      }
    }
    # -parse-only
    elsif ( _process_arg($arg, 'parse-only') ) {
      $parse_only = 1;
      $atleastoneflag = 1;
    }
    # -opt-only
    elsif ( _process_arg($arg, 'opt-only') ) {
      $opt_only = 1;
      $atleastoneflag = 1;
    }
    # -v-only
    elsif ( _process_arg($arg, 'v-only') ) {
      $verilog_gen_only = 1;
      $atleastoneflag = 1;
    }
    # -ip-only
    elsif ( _process_arg($arg, 'ip-only') ) {
      $ip_gen_only = 1;
      $atleastoneflag = 1;
    }
    # -dump-csr
    elsif ( _process_arg($arg, 'dump-csr') ) {
      $llc_arg_after .= ' -csr';
    }
    # -skip-qsys
    elsif ( _process_arg($arg, 'skip-qsys') ) {
      $skip_qsys = 1;
      $atleastoneflag = 1;
    }
    # -c
    # note that -c is also parsed in entry_wrapper for the Quartus-less rtl flow.
    elsif ( ($arg eq '-c') ) {
      $compile_step = 1;
      $atleastoneflag = 1;
      $c_flag_only = 1;
    }
    # -rtl
    # note that -rtl is also parsed in entry_wrapper for the Quartus-less rtl flow.
    elsif ( ($arg eq '-rtl') ) {
      $report_only = 1;
      $atleastoneflag = 1;
    }
    # -alpha-tool
    elsif ( _process_arg($arg, 'alpha-tool') ) {
      acl::Report::enable_alpha_viewer();  # shows an extra menu on the top
    }
    # -no-hardware-kernel-invocation-queue
    elsif ( ($arg eq '-no-hardware-kernel-invocation-queue') ) {
      $opt_arg_after .= ' -fast-relaunch-depth=0';
      $llc_arg_after .= ' -fast-relaunch-depth=0';
    }
    # -incremental[=aggressive]
    elsif( ($arg =~ /^-incremental(=aggressive)?$/) or ($arg =~ /^--incremental(=aggressive)?$/) ){
      if ($arg =~ /=aggressive$/) {
        $incremental_compile = 'aggressive';
      } else {
        $incremental_compile = 'default';
      }
      $ENV{'AOCL_INCREMENTAL_COMPILE'} = $incremental_compile;
    }
    # -dis
    elsif ( _process_arg($arg, 'dis') ) {
      $disassemble = 1;
    }
    # -tidy
    elsif ( _process_arg($arg, 'tidy') ) {
      $tidy = 1;
    }
    # -save-temps
    elsif ( _process_arg($arg, 'save-temps') ) {
      $$save_temps_ref = 1;
      acl::Common::set_save_temps($$save_temps_ref);
    }
    # -use-ip-library
    elsif ( _process_arg($arg, 'use-ip-library') ) {
      $use_ip_library = 1;
    }
    # -no-link-ip-library
    elsif ( _process_arg($arg, 'no-link-ip-library') ) {
      $use_ip_library = 0;
    }
    # -regtest_mode
    elsif ( _process_arg($arg, 'regtest_mode') ) {
      $regtest_mode = 1;
    }
    # -internal-assert=<value> for internal use only
    elsif ( ($arg =~ /^-internal-assert=(.*)$/) ) {
      my $argument_value = $1;
      # uppercase the string
      $argument_value = uc $argument_value;
      if ($argument_value eq "TRUE") {
        $internal_assert = 1;
      } elsif ($argument_value eq "FALSE") {
        $internal_assert = 0;
      } else {
        acl::Common::mydie("Option $arg requires true or false");
      }
    }
    # -regtest-bsp-bak-cache
    elsif ( _process_arg($arg, 'regtest-bsp-bak-cache') ) {
      $$regtest_bak_cache_ref = 1;
    }
    # -no-read-bsp-bak-cache
    elsif ( _process_arg($arg, 'no-read-bsp-bak-cache') ) {
      push @blocked_migrations, 'pre_skipbak';
    }
    # -incremental-input-dir=<path>
    elsif ( $arg =~ /^-incremental-input-dir=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -incremental-input-dir= requires a path to a previous compile directory");
      } else {
        $$incremental_input_dir_ref = $argument_value;
        ( -e $$incremental_input_dir_ref && -d $$incremental_input_dir_ref ) or acl::Common::mydie("Option -incremental-input-dir= must specify an existing directory");
      }
    }
    # -incremental-save-partitions <filename>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'incremental-save-partitions', 'filename', 0, 'a file containing partitions you wish to partition') ) {
      # assume target dir is the incremental dir
      $save_partition_file = shift @input_argv;
      $incremental_compile = 'default';
      ( -e $save_partition_file && -f $save_partition_file ) or acl::Common::mydie("Option -incremental-save-partitions must specify an existing file");
    }
    # -incremental-save-partitions=<filename>
    elsif ( $arg =~ /^-incremental-save-partitions=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option incremental-save-partitions= requires a file containing partitions you wish to partition");
      } else {
        $save_partition_file = $argument_value;
        $incremental_compile = 'default';
        ( -e $save_partition_file && -f $save_partition_file ) or acl::Common::mydie("Option -incremental-save-partitions= must specify an existing file");
      }
    }
    # -incremental-set-partitions <filename>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'incremental-set-partitions', 'filename', 0, 'a file containing partitions you wish to partition') ) {
      # assume target dir is the incremental dir
      $set_partition_file = shift @input_argv;
      $incremental_compile = 'default';
      ( -e $set_partition_file && -f $set_partition_file ) or acl::Common::mydie("Option -incremental-set-partitions must specify an existing file");
    }
    # -incremental-set-partitions=<filename>
    elsif ( $arg =~ /^-incremental-set-partitions=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option incremental-set-partitions= requires a file containing partitions you wish to partition");
      } else {
        $set_partition_file = $argument_value;
        $incremental_compile = 'default';
        ( -e $set_partition_file && -f $set_partition_file ) or acl::Common::mydie("Option -incremental-set-partitions= must specify an existing file");
      }
    }
    # -floorplan <filename>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'floorplan', 'filename') ) {
      my $floorplan_file = acl::File::abs_path(shift @input_argv);
      ( -e $floorplan_file && -f $floorplan_file ) or acl::Common::mydie("Option --floorplan must specify an existing file");
      $sysinteg_arg_after .= ' --floorplan '.$floorplan_file;
    }
    # -floorplan=<filename>
    elsif ( $arg =~ /^-floorplan=(.*)$/ ) {
      my $floorplan_file = acl::File::abs_path($1);
      ( -e $floorplan_file && -f $floorplan_file ) or acl::Common::mydie("Option --floorplan must specify an existing file");
      $sysinteg_arg_after .= ' --floorplan '.$floorplan_file;
    }
    # -incremental-flow=<flow-name>
    elsif ( $arg =~ /^-incremental-flow=(.*)$/ ) {
      my $retry_option = $1;
      my %incremental_flow_strats = (
        'retry' => 1,
        'no-retry' => 1
      );
      $retry_option ne "" or acl::Common::mydie("Usage: -incremental-flow=<" . join("|", keys %incremental_flow_strats) . ">");
      if (exists $incremental_flow_strats{$retry_option}) {
        $ENV{'INCREMENTAL_RETRY_STRATEGY'} = $retry_option;
      } else {
        die "$retry_option is not a valid -incremental-flow selection! Select from: <" . join("|", keys %incremental_flow_strats) . ">";
      }
    }
    # -parallel=<num_procs>
    elsif ( ($arg =~ /^-parallel=(\d+)$/) ) {
      $cpu_count = $1;
    }
    # -add-qsf "file1 file2 file3 ..."
    elsif ( _process_arg_with_value($arg, \@input_argv, 'add-qsf', 'filenames', 0, 'a space-separated list of files') ) {
      my @qsf_files = split(/ /, (shift @input_argv));
      push @additional_qsf, @qsf_files;
    }
    # -add-qsf=file1,file2,file3,...
    elsif ( $arg =~ /^-add-qsf=(.*)$/ ) {
      my @input_files = split(/,/, $1);
      $#input_files >= 0 or acl::Common::mydie("Option -add-qsf= requires at least one argument");
      push @additional_qsf, @input_files;
    }
    # -empty-kernel=<filename>
    # Use Quartus to remove logic inside the kernel while preserving its input and output ports
    # File should contain names of kernels separated by newline
    elsif ( $arg =~ /^-empty-kernel=(.*)$/ ) {
      my $quartus_emptied_kernel_file = acl::File::abs_path($1);
      ( -e $quartus_emptied_kernel_file && -f $quartus_emptied_kernel_file ) or acl::Common::mydie("Option -empty-kernel must specify an existing file");
      $empty_kernel_flow = 1;
      $sysinteg_arg_after .= ' --empty-kernel '.$quartus_emptied_kernel_file;
    }
    # -high-effort-compile
    elsif ( ($arg eq '-high-effort-compile') ) {
      $high_effort_compile = 1;
    }
    # -fast-compile
    elsif ( _process_arg($arg, 'fast-compile') ) {
      $fast_compile = 1;
    }
    # -1x-clk-for-const-cache
    elsif ( ($arg eq '-1x-clk-for-const-cache') ) {
      $sysinteg_arg_after .= ' --cic-1x-const-cache';
    }
    # -incremental-soft-region
    elsif ( ($arg eq '-incremental-soft-region') ) {
      $soft_region_on = 1;
    }
    # -fmax <value>
    elsif ( ($arg eq '-fmax') or ($arg eq '--fmax') ) {
      print "Warning: Command has been deprecated. Please use -clock=<value> instead of $arg <value>\n";
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option -fmax requires an argument");
      $opt_arg_after .= ' -scheduler-fmax=';
      $llc_arg_after .= ' -scheduler-fmax=';
      my $fmax_constraint = (shift @input_argv);
      $opt_arg_after .= $fmax_constraint;
      $llc_arg_after .= $fmax_constraint;
    }
    # -fmax=<value>
    elsif ( $arg =~ /^-fmax=(.*)$/ ) {
      print "Warning: Command has been deprecated. Please use -clock=<value> instead of -fmax<value>\n";
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -fmax= requires an argument");
      } else {
        $opt_arg_after .= " -scheduler-fmax=$argument_value";
        $llc_arg_after .= " -scheduler-fmax=$argument_value";
      }
    }
    # -clock=<value>
    elsif ( $arg =~ /^-clock=(.*)$/ ) {
      my $clk_option = $1;
      if ($clk_option eq "") {
        acl::Common::mydie("Option -clock= requires an argument");
      } else {
        $qii_fmax_constraint = clk_get_fmax($clk_option);
        if (!defined $qii_fmax_constraint) {
              acl::Common::mydie("Error: bad value ($clk_option) for -clock argument\n");
        }
        $opt_arg_after .= " -scheduler-fmax=$qii_fmax_constraint";
        $llc_arg_after .= " -scheduler-fmax=$qii_fmax_constraint";
      }
    }
    # -dont-error-if-large-area-est
    elsif ( ($arg eq '-dont-error-if-large-area-est') ) {
      $opt_arg_after .= ' -cont-if-too-large';
      $llc_arg_after .= ' -cont-if-too-large';
    }
    # -timing-failure-mode=... (choice of ignore, warn and error)
    elsif ($arg eq '-timing-failure-mode=(.*)$') {
      my $mode = $1;
      if ($mode eq "ignore"){
        $error_on_timing_fail = 0;
      } elsif ($mode eq "warn"){
        $error_on_timing_fail = 1;
      } elsif ($mode eq "error"){
        $error_on_timing_fail = 2;
      } else {
        print "Warning: Invalid mode $mode passed in with the -timing-failure-mode flag. Valid modes are: warn, error, ignore\n";
      }
    }
    # -timing-failure-allowed-slack=<value>
    elsif ($arg =~ /^-timing-failure-allowed-slack=(.*)$/) {
      my $argument_value = $1;
      if ($argument_value eq ""){
        $argument_value = 0;
      }
      $allowed_timing_slack = $argument_value;
      # increase the mode by 1 - if the mode was "ignore", warn for values larger than the allowed
      # if the mode was "warn", error for values larger than the allowed
      $error_on_timing_fail += 1; # increase the mode by 1
    }
    # -seed <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'seed', 'value') ) {
      $fit_seed = (shift @input_argv);
    }
    # -seed=<value>
    elsif ( $arg =~ /^-seed=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -seed= requires an argument");
      } else {
        $fit_seed = $argument_value;
      }
    }
    # -no-lms
    elsif ( _process_arg($arg, 'no-lms') ) {
      $opt_arg_after .= " ".$lmem_disable_split_flag;
    }
    # -fp-relaxed
    elsif ( _process_arg($arg, 'fp-relaxed') ) {
      # temporary fix - sycl uses a different version of clang that does not support ffp-reassoc
      # remove sycl-exempt warning when sycl supports ffp-reassoc
      if ($sycl_mode == 0) {
          print "Warning: $arg will override all instances of #pragma clang fp reassoc\n";
      }
      $opt_arg_after .= " -fp-relaxed=true";
    }
    # -daz
    elsif ( _process_arg($arg, 'daz') ) {
      $opt_arg_after .= " -daz";
      $llc_arg_after .= " -daz";
    }
    # -rounding=<ieee|faithful>
    elsif ( ($arg =~ /^-rounding=(.*)$/) or ($arg =~ /^--rounding=(.*)$/) ) {
      if ($arg =~ /^--rounding=(.*)$/) {
        print "Warning: Command has been deprecated. Please use -rounding instead of --rounding\n";
      }
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -rounding= requires an argument");
      } elsif ($argument_value eq 'ieee' or $argument_value eq 'faithful') {
        $llc_arg_after .= ' -rounding=' . $argument_value;
      } else {
        acl::Common::mydie("Unsupported option \'" . $argument_value ."\' for \'-rounding\'");
      }
    }
    # -cl-denorms-are-zero
    elsif ( ($arg eq '-cl-denorms-are-zero') ) {
      # clang supports this flag natively, but not clang cc1. Since we are using
      # cc1, use the correct flags for that
      push @$args_ref, ('-fdenormal-fp-math=preserve-sign');
      push @$args_ref, ('-fdenormal-fp-math-f32=preserve-sign');
    }
    # -Os
    # enable sharing flow
    elsif ( ($arg eq '-Os') ) {
      $opt_arg_after .= ' -opt-area=true';
      $llc_arg_after .= ' -opt-area=true';
    }
    # -fpc
    elsif ( _process_arg($arg, 'fpc') ) {
      # temporary fix -  sycl uses a different version of clang that does not fully support
      # ffp-contract=fast as a replacement for -fpc
      # remove sycl-exempt warning when sycl supports ffp-contract
      if ($sycl_mode == 0) {
        print "Warning: $arg will override all instances of #pragma clang fp contract(fast)\n";
      }
      $opt_arg_after .= " -fpc=true";
    }
    # -const-cache-bytes <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'const-cache-bytes', 'value') ) {
      $sysinteg_arg_after .= ' --cic-const-cache-bytes';
      $opt_arg_after .= ' --cic-const-cache-bytes=';
      $llc_arg_after .= ' --cic-const-cache-bytes=';
      my $const_cache_size = (shift @input_argv);
      my $actual_const_cache_size = 16384;
      # Allow for positive Real Numbers Only
      if (!($const_cache_size =~ /^\d+(?:\.\d+)?$/)) {
        acl::Common::mydie("Invalid argument for option --const-cache-bytes,<N> must be a positive real number.");
      }
      while ($actual_const_cache_size < $const_cache_size ) {
        $actual_const_cache_size = $actual_const_cache_size * 2;
      }
      $sysinteg_arg_after .= " ".$actual_const_cache_size;
      $opt_arg_after .= $actual_const_cache_size;
      $llc_arg_after .= $actual_const_cache_size;
    }
    # -const-cache-bytes=<value>
    elsif ( $arg =~ /^-const-cache-bytes=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -const-cache-bytes= requires an argument");
      } else {
        my $const_cache_size = $argument_value;
        my $actual_const_cache_size = 16384;
        while ($actual_const_cache_size < $const_cache_size ) {
          $actual_const_cache_size = $actual_const_cache_size * 2;
        }
        $sysinteg_arg_after .= " --cic-const-cache-bytes $actual_const_cache_size";
        $opt_arg_after .= " --cic-const-cache-bytes=$actual_const_cache_size";
        $llc_arg_after .= " --cic-const-cache-bytes=$actual_const_cache_size";
      }
    }
    # -board <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'board', 'value') ) {
      ($board_variant) = (shift @input_argv);
      $user_defined_board = 1;
    }
    # -board=<value> or (SYCL-only) -board=<bsp name>:<board name>
    elsif ( $arg =~ /^-board=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -board= requires an argument");
      } else {
        $board_variant = $argument_value;
        $user_defined_board = 1;
      }
    }
    # -board-package=<path> or <name>
    elsif ( $arg =~ /^-board-package=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -board-package= requires an argument");
      } else {
        $bsp_variant = acl::Common::get_board_package_path($argument_value);
        $user_defined_bsp = 1;
      }
    }
    # -L <path>
    elsif ($arg eq '-L') {
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option -L requires a directory name");
      push (@lib_paths, (shift @input_argv));
    }
    # -L<path>
    elsif ($arg =~ m!^-L(\S+)!) {
      push (@lib_paths, $1);
    }
    # -l <libname>
    elsif ($arg eq '-l') {
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option -l requires a path/filename");
      push (@lib_files, (shift @input_argv));
    }
    # -library-debug
    elsif ( _process_arg($arg, 'library-debug') ) {
      $opt_arg_after .= ' -debug-only=libmanager';
      $library_debug = 1;
    }
    # -shared
    elsif ( _process_arg($arg, 'shared') ) {
      print "Warning: Command has been deprecated. Use fpga_crossgen instead.\n";
      $created_shared_aoco = 1;
      $compile_step = 1; # '-shared' implies '-c'
      $atleastoneflag = 1;
      # Enabling -g causes problems when compiling resulting
      # library for emulator (crash in 2nd clang invocation due
      # to debug info inconsistencies). Disabling for now.
      #push @$args_ref, '-g'; #  '-shared' implies '-g'

    }
    # -createlibobject <file>, internal only command
    elsif ($arg eq '-createlibobject' or $arg eq '-createlibobjectforsycl') {
      #internal flag used for creating aoco's suitable for use in the new library flow
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option -createlibobject requires a filename");

      if ($input_argv[0] !~ m/\.cl$|\.cpp$/) {
        acl::Common::mydie("Only .cl and .cpp files are allowed for -createlibobject");
      }
      $create_library_flow = 1;
      $create_library_flow_for_sycl = 1 if $arg eq '-createlibobjectforsycl';
      push @given_input_files, (shift @input_argv);
    }
    # -preproc-gcc-toolchain
    # Flag to let users parse files dependent on C++ system headers
    # (and compilers specific definition headers)
    elsif ($arg eq '-preproc-gcc-toolchain') {
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option -preproc-gcc-toolchain requires a filename");
      $preproc_gcc_toolchain = shift @input_argv;
    }
    # -override-library-version
    # Internal flag to let users use libraries built with a different acds build at there own risk.
    # Flag was created to give PIPE a way out on their 2019 deliverables, that are out of cycle
    elsif ($arg eq '-override-library-version') {
      $library_version_override = 1;
    }
    # -override-emulator-gen
    # Internal flag to allow creation of libraries even if the new emulator
    # don't support the features (yet)
    elsif ($arg eq '-override-emulator-gen') {
      $library_skip_fastemulator_gen = 1;
    }
    # -profile-config <file>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'profile-config', 'filename', 0, 'a path/filename') ) {
      !defined $profilerconf_file or acl::Common::mydie("Too many profiler config files provided\n");
      $profilerconf_file = (shift @input_argv);
    }
    # -profile-config=<file>
    elsif ( $arg =~ /^-profile-config=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -profile-config= requires a path/filename");
      } else {
        !defined $profilerconf_file or acl::Common::mydie("Too many profiler config files provided\n");
        $profilerconf_file = $argument_value;
      }
    }
    # -bsp-flow <flow-name>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'bsp-flow', 'flow-name', 0, 'a flow-name') ) {
      !defined $$bsp_flow_name_ref or acl::Common::mydie("Too many bsp-flows defined.\n");
      $$bsp_flow_name_ref = (shift @input_argv);
      $sysinteg_arg_after .= " --bsp-flow $$bsp_flow_name_ref";
      $$bsp_flow_name_ref = ":".$$bsp_flow_name_ref;
    }
    # -bsp-flow=<flowname>
    elsif ( $arg =~ /^-bsp-flow=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -bsp-flow= requires a flow-name");
      } else {
        !defined $$bsp_flow_name_ref or acl::Common::mydie("Too many bsp-flows defined.\n");
        $$bsp_flow_name_ref = $argument_value;
        $sysinteg_arg_after .= " --bsp-flow $$bsp_flow_name_ref";
        $$bsp_flow_name_ref = ":".$$bsp_flow_name_ref;
      }
    }
    # -oldbe
    elsif ( _process_arg($arg, 'oldbe') ) {
      $griffin_flow = 0;
      $sysinteg_arg_after .= " --oldbe";
    }
    # -ggdb / -march=emulator
    elsif ($arg eq '-ggdb' || $arg eq '-march=emulator') {
      $emulator_flow = 1;
      $user_defined_flow = 1;
      if ($arg eq '-ggdb') {
        $dash_g = 1;
        $user_dash_g = 1;
      }
    }
    # -fast-emulator
    elsif ($arg eq '-fast-emulator') {
      print "Warning: Command has been deprecated. The Fast Emulator is the default emulator;";
      print " use -legacy-emulator if you wish to target the Legacy Emulator.\n";
    }
    # -legacy-emulator
    elsif ($arg eq '-legacy-emulator') {
      $emulator_flow = 1;
      $emulator_fast = 0;
    }
    # -ecc
    elsif ($arg eq '-ecc') {
      $ecc_protected = 1;
    }
    # -ecc-max-latency <value>
    elsif ($arg eq '-ecc-max-latency') {
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option -ecc-max-latency requires a value");
      if ($ecc_protected != 1){
        acl::Common::mydie("Option -ecc-max-latency requires an -ecc flag provided");
      } else {
        $ecc_max_latency = (shift @input_argv);
      }
    }
    # -ecc-max-latency=<value>
    elsif ( $arg =~ /^-ecc-max-latency=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -ecc-max-latency= requires a value");
      } elsif ($ecc_protected != 1){
        acl::Common::mydie("Option -ecc-max-latency requires an -ecc flag provided");
      } else {
        $ecc_max_latency = $argument_value;
      }
    }
    # -device-spec <filename>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'device-spec', 'filename', 0, 'a path/filename') ) {
      $device_spec = (shift @input_argv);
    }
    # -device-spec=<filename>
    elsif ( $arg =~ /^-device-spec=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -device-spec= requires a path/filename");
      } else {
        $device_spec = $argument_value;
      }
    }
    # -dot
    elsif ( _process_arg($arg, 'dot') ) {
      $dotfiles = 1;
    }
    # -timing-threshold=<value>
    elsif ( $arg =~ /^-timing-threshold=(.*)$/ ) {
      my $argument_value = $1;
      #check for argument value to be a valid number. (C float)
      if ($argument_value=~ /^([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?$/ ) {
          $timing_slack_check = 1;
          $ENV{AOCL_TIMING_SLACK}= $argument_value;
      } else {
        acl::Common::mydie("Option -timing-threshold=<value> requires a valid positive number in nano seconds");
      }
    }
    # -time
    elsif ( _process_arg($arg, 'time') ) {
      if($#input_argv >= 0 && $input_argv[0] !~ m/^-./) {
        $time_log_filename = shift(@input_argv);
      }
      else {
        $time_log_filename = "-"; # Default to stdout.
      }
    }
    # -time=<file>
    elsif ( $arg =~ /^-time=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -time requires a filename");
      } else {
        $time_log_filename = $argument_value;
      }
    }
    # -time-passes
    elsif ( _process_arg($arg, 'time-passes') ) {
      $time_passes = 1;
      $opt_arg_after .= ' --time-passes';
      $llc_arg_after .= ' --time-passes';
      if(!$time_log_filename) {
        $time_log_filename = "-"; # Default to stdout.
      }
    }
    # -un
    # Temporary test flag to enable Unified Netlist flow.
    elsif (_process_arg($arg, 'un')) {
      $opt_arg_after .= ' --un-flow';
      $llc_arg_after .= ' --un-flow';
    }
    # -usm
    # This compile will use USM - disable alignment assumptions for global pointer arguments
    elsif ($arg eq '-usm') {
      $opt_arg_after .= ' -default-global-pointer-alignment 1';
      $llc_arg_after .= ' -default-global-pointer-alignment 1';
    }
    # -no-interleaving <name>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'no-interleaving', 'name', 0, 'a memory name or \'default\'') ) {
      $llc_arg_after .= ' -use-swdimm=';
      if($input_argv[0] ne 'default' ) {
        my $argument_value = shift(@input_argv);
        $sysinteg_arg_after .= ' --no-interleaving '.$argument_value;
        $llc_arg_after .= $argument_value;
      }
      else {
        #non-heterogeneous sw-dimm-partition behaviour
        #this will target the default memory
        shift(@input_argv);
        $sysinteg_arg_after .= ' --cic-global_no_interleave ';
        $llc_arg_after .= 'default';
      }
    }
    # -no-interleaving=<name>
    elsif ( $arg =~ /^-no-interleaving=(.*)$/ ) {
      my $argument_value = $1;
      $llc_arg_after .= ' -use-swdimm=';
      if ($argument_value eq "") {
        acl::Common::mydie("Option -no-interleaving requires a memory name or 'default'");
      } elsif ($argument_value eq 'default') {
        $sysinteg_arg_after .= ' --cic-global_no_interleave ';
        $llc_arg_after .= 'default';
      } else {
        $sysinteg_arg_after .= ' --no-interleaving '.$argument_value;
        $llc_arg_after .= $argument_value;
      }
    }
    # -global-tree
    elsif ( _process_arg($arg, 'global-tree') ) {
      $sysinteg_arg_after .= ' --global-tree';
      $llc_arg_after .= ' -global-tree';
    }
    # -global-ring
    elsif ( _process_arg($arg, 'global-ring') ) {
      $sysinteg_arg_after .= ' --global-ring';
    }
    # -ring-root-arb-balanced-rw
    elsif ( _process_arg($arg, 'ring-root-arb-balanced-rw') ) {
      $sysinteg_arg_after .= ' --ring-root-arb-balanced-rw';
    }    
    # -duplicate-ring
    elsif ( ($arg eq '-duplicate-ring') or ($arg eq '--duplicate-ring') ) {
      print "Warning: Command has been deprecated. The compiler now duplicates the store ring by default. To force use of a single store ring, please use -force-single-store-ring.\n";
    }
    # -force-single-store-ring
    elsif ($arg eq '-force-single-store-ring') {
      $sysinteg_arg_after .= ' --force-single-store-ring';
    }
    # -num-reorder <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'num-reorder', 'value') ) {
      $sysinteg_arg_after .= ' --num-reorder '.(shift @input_argv);
    }
    #-incremental-grouping=<path>
    elsif( $arg =~ /^-incremental-grouping=(.*)$/ ){
      my $partition_file = $1;
      if ($partition_file eq "") {
        acl::Common::mydie("Option -incremental-grouping= requires a path to the partition grouping file");
      } else {
        $partition_file = acl::File::abs_path($partition_file);
        (-e $partition_file) or acl::Common::mydie("-incremental-grouping file $partition_file does not exist.");
        $sysinteg_arg_after .= ' --incremental-grouping '.$partition_file;
      }
    }
    # -num-reorder=<value>
    elsif ( $arg =~ /^-num-reorder=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -num-reorder= requires an argument");
      } else {
        $sysinteg_arg_after .= ' --num-reorder '.$argument_value;
      }
    }
    elsif ( _process_meta_args ($arg, \@input_argv) ) { }
    # -input=kernel_1.cl,kernel_2.cl,kernel_3.cl,...
    elsif ( $arg =~ /^-input=(.*)$/ ) {
      my @input_files = split(/,/, $1);
    }
    elsif ( $arg =~ m/\.cl$|\.c$|\.aoco$|\.aocr$|\.xml$|\.spv$/ ) {
      # if initial-dir is not specified, add the full absolute path of the given input
      # else keep the original input.
      # no impact on user behavior since -initial-dir is used only in conformance
      # based on test, aoco files (as input) are not compatible with -initial-dir
      push @given_input_files, $force_initial_dir ? $arg : acl::File::abs_path($arg);
    }
    elsif ( $arg =~ m/\.aoclib$|\.fpgalib$/ ) {
      acl::Common::mydie("Library file $arg specified without -l option");
    }
    # -library-list <value>
    elsif ( $arg eq '-library-list' ) {
      # @library_list stores files from the library-list flag
      ($#input_argv >= 0 and $input_argv[0] !~ m/^-./) or acl::Common::mydie("Option -library-list requires an argument");
      push @library_list, shift @input_argv;
    }
    # -library-list=<value>
    elsif ( $arg =~ /^-library-list=(.*)$/ ) {
      # @library_list stores files from the library-list flag
      if (!$1) {
        acl::Common::mydie("Option -library-list= requires an argument");
      }
      push @library_list, $1;
    }
    # -dsploc <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'dsploc', 'value') ) {
      $dsploc = (shift @input_argv);
    }
    # -dsploc=<value>
    elsif ( $arg =~ /^-dsploc=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -dsploc= requires an argument");
      } else {
        $dsploc = $argument_value;
      }
    }
    # -ramloc <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'ramloc', 'value') ) {
      $ramloc = (shift @input_argv);
    }
    # -ramloc=<value>
    elsif ( $arg =~ /^-ramloc=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -ramloc= requires an argument");
      } else {
        $ramloc = $argument_value;
      }
    }
    # -O3
    elsif ($arg eq '-O3') {
      $emu_optimize_o3 = 1;
    }
    # -emulator-channel-depth-model <value>
    elsif ( _process_arg_with_value($arg, \@input_argv, 'emulator-channel-depth-model', 'value') ) {
      $emu_ch_depth_model = (shift @input_argv);
    }
    # -emulator-channel-depth-model=<value>
    elsif ( $arg =~ /^-emulator-channel-depth-model=(.*)$/ ) {
      my $argument_value = $1;
      if ($argument_value eq "") {
        acl::Common::mydie("Option -emulator-channel-depth-model= requires an argument");
      } else {
        $emu_ch_depth_model = $argument_value;
      }
    }
    # -D__IHC_USE_DEPRECATED_NAMES
    elsif ($arg eq '-D__IHC_USE_DEPRECATED_NAMES') {
      print "$prog: Warning: Turning on use of deprecated names!\n";
      push @$args_ref, $arg;
    }
    # -hyper-optimized-handshaking=<off|auto>
    elsif ( $arg =~ /^(-hyper-optimized-handshaking)=(.*)$/ ) { # customer facing name
      my $arg_name = $1;
      my $argument_value = $2;
      if ($argument_value eq "") {
        acl::Common::mydie("Option " . $arg_name . "= requires an argument");
      } elsif ($argument_value eq 'off' or $argument_value eq 'auto') {
        $llc_arg_after .= ' --force-stall-latency=' . $argument_value; # internal griffin name
        $opt_arg_after .= ' --force-stall-latency=' . $argument_value;
        $user_opt_hyper_optimized_hs = 1;
      } else {
        acl::Common::mydie("Unsupported option \'" . $argument_value . "\' for \'" . $arg_name . "\'");
      }
    }
    # -no-accessor-aliasing
    elsif ( $arg eq "-no-accessor-aliasing") {
      $opt_arg_after .= ' -force-restrict-all-kernels';
    }
    # -disable-auto-loop-fusion
    elsif ($arg eq "-disable-auto-loop-fusion") {
      $opt_arg_after .= ' -disable-auto-loop-fusion';
    }
    # -enable-unequal-tc-fusion
    elsif ($arg eq "-enable-unequal-tc-fusion") {
      $opt_arg_after .= ' -enable-unequal-tc';
    }
    # -dep-files=<file1.d|@list_file1>[,fileN.d|@list_fileN]*
    # @list_file is a file listing .d file paths
    elsif ( $arg =~ /^(-dep-files)=(.*)$/ ) {
      for my $dep_file (split(',', $2)) {
        if ($dep_file =~ /^@(.*)$/) {
          open(my $fh, $1) or acl::Common::mydie("Cannot open dep-files list $1");
          while (my $row = <$fh>) {
            chomp $row;
            push @all_dep_files, split(' ', $row);
          }
          close ($fh);
        } else  {
          push @all_dep_files, $dep_file;
        }
      }
    }
    # Unrecognized Option
    else {
      push @$args_ref, $arg;
    }
  }

  # Defer printing help until all of the flags are set
  if ($print_help) {
    _usage();
    exit 0;
  } elsif ($print_power_help) {
    _powerusage();
    exit 0;
  }
}

sub process_args {

  my ( $args_ref,
       $using_default_board_ref,
       $dirbase_ref,
       $base_ref,
       $sim_accurate_memory,
       $sim_kernel_clk_frequency,
       $regtest_bak_cache,
       $verbose,
       $incremental_input_dir) = @_;

  # Process SYCL-related parameters
  if (!$sycl_mode && ($sycl_hw_mode || $sycl_sim_mode)){
    acl::Common::mydie("'-hardware' or '-simulation' must be used together with '-sycl'");
  }
  if ($sycl_hw_mode && $sycl_sim_mode){
    acl::Common::mydie("'-hardware' and '-simulation' cannot be used at the same time");
  }
  if ($sycl_mode){
    if (!$sycl_hw_mode && $report_only){
      acl::Common::mydie("Early device link flow is not supported in FPGA emulation flow");
    }
    # In SYCL mode, pass "-sycl" as a parameter to opt/llc stage to instruct
    #  them to output SYCL-specific error msg.
    $opt_arg_after .= " -sycl";
    $llc_arg_after .= " -sycl";

    if ($sycl_sim_mode){
      # SYCL simulation flow
      $new_sim_mode = 1;
      $user_defined_flow = 1;
      $ip_gen_only = 1;
      $atleastoneflag = 1;
    }elsif ($sycl_hw_mode){
      # SYCL hardware flow. Nothing need to be done here
    }else{
      # If only '-sycl' is used, use emulation flow
      $emulator_flow = 1;
      $user_defined_flow = 1;
    }

    $error_on_timing_fail = 0; # make sure there is no timing failure warnings in sycl
  }

  if (!$user_defined_bsp and $user_defined_board){
    # The user has provide -board=<board package name or path>:<board name>, -board=<board package name or path>: or -board=<board name>
    ($bsp_variant, $board_variant, $user_defined_bsp, $user_defined_board) = acl::Common::parse_board_arg($board_variant);
  }


  # Add incremental flags here instead of when parsing the AOC flag because we want to allow
  # users to specify the incremental mode multiple times and use the last value.
  # The boolean flags cannot be turned off after they're turned on so we only
  # add the internal flags once after parsing all the AOC flags.
  if ($incremental_compile) {
    $sysinteg_arg_after .= ' --incremental ';
    $llc_arg_after .= ' -incremental ';
    if ($incremental_compile eq 'aggressive') {
      $sysinteg_arg_after .= ' --use-partial-arbitration ';
      $llc_arg_after .= ' -incremental-cdi-recompile-off ';
    }
  }

  if ($fast_compile and $timing_slack_check) {
    acl::Common::mydie("Cannot have timing slack check when fast-compile is set");
  }

  if (!$sim_accurate_memory && defined($sim_kernel_clk_frequency)) {
    # Issue warning as sim-clk-freq will not take any effect for sim flow with no clock crosser.
    print "$prog: Warning: -sim-clk-freq=$sim_kernel_clk_frequency is ignored because -sim-acc-mem is not used.\n";
  }

  # Process $time_log_filename. If defined, then treat it as a file name
  # (including "-", which is stdout).
  # Do this right after argument parsing, so that following code is able to log times.
  if ($time_log_filename) {
    acl::Common::open_time_log($time_log_filename, $run_quartus);
  }

  # Don't add -g to user_opencl_args because -g is now enabled by default.
  # Instead add -g0 if the user explicitly disables debug info.
  push @user_opencl_args, @$args_ref;
  if (!$dash_g) {
    push @user_opencl_args, '-g0';
  }

  if ($c_flag_only) {
    my $mixed_args = $opt_arg_after.$llc_arg_after.$sysinteg_arg_after;
    $mixed_args = acl::AOCDriverCommon::remove_duplicate($mixed_args);
    if ($mixed_args) {
      print "$prog: Warning: The following linker args will be ignored in this flow:$mixed_args \n";
    }
  }

  if (not $emulator_flow) {
    if (!$emulator_fast) {
      # The only way $emulator_fast can be 0 at this point is if -legacy-emulator was specified.
      acl::Common::mydie("-march=emulator must be specified when targeting the legacy emulator.");
    } else {
      # If not emulating, turn the $emulator_fast flag off.
      $emulator_fast = 0;
    }
  }

  if ($emulator_flow) {
    if ($emulator_fast) {
      if (!$emu_ch_depth_model eq '') {
        # Argument not valid for fast emulator
        print <<WARNING_TEXT;
$prog: Warning: The -emulator-channel-depth-model flag is only valid when
targeting the Legacy Emulator. Use the CL_CONFIG_CHANNEL_DEPTH_EMULATION_MODE
environment variable to set the channel depth model for the emulator. Consult
the Intel FPGA SDK for OpenCL Programming Guide for more details.
WARNING_TEXT
      }
    } elsif ($emu_ch_depth_model eq '') {
      # Legacy emulator - set the argument to default if it was previously unset.
      $emu_ch_depth_model = 'default';
    }
  }

  # Propagate -g to clang, opt, and llc
  if ($dash_g || $profile) {
    if ($created_shared_aoco) {
      print "$prog: Debug symbols are not supported for shared object files, ignoring -g.\n" if $user_dash_g;
    } else {
      if (not $emulator_fast) {
        push @$args_ref, ('-debug-info-kind=limited', '-dwarf-version=4');
      }
    }
    $opt_arg_after .= ' -dbg-info-enabled';
    $llc_arg_after .= ' -dbg-info-enabled';
  }

  push (@$args_ref, "-Wunknown-pragmas") unless $emulator_fast;
  @user_clang_args = @$args_ref;

  if ($regtest_mode){
      my $save_temps = 1;
      acl::Common::set_save_temps($save_temps);
      $report = 1;
      $sysinteg_arg_after .= ' --regtest_mode ';
      # temporary app data directory
      if (defined $ENV{"ARC_PICE"}) {
        $tmp_dir = ( $^O =~ m/MSWin/ ? "P:/psg/flows/sw/aclboardpkg/.platform/BAK_cache/windows": "/p/psg/flows/sw/aclboardpkg/.platform/BAK_cache/linux" );
      } else {
        $tmp_dir = ( $^O =~ m/MSWin/ ? "S:/tools/aclboardpkg/.platform/BAK_cache/windows": "/tools/aclboardpkg/.platform/BAK_cache/linux" );
      }
      if(!$regtest_bak_cache) {
        push @blocked_migrations, 'post_skipbak';
      }
      $llc_arg_after .= " -dump-hld-area-debug-files";
  }

  #Enable internal assert if
  #  1. internal-assert is set to true
  #  2. in regtest mode and internal-assert is not set to false
  if ($internal_assert == 1 || ($regtest_mode == 1 && $internal_assert == -1)){
      $llc_arg_after .= " -enable-internal-asserts";
      $opt_arg_after .= " -enable-internal-asserts";
  }

  if ($dotfiles) {
    $opt_arg_after .= ' --dump-dot ';
    $llc_arg_after .= ' --dump-dot ';
    $sysinteg_arg_after .= ' --dump-dot ';
  }

  # $orig_dir = acl::File::abs_path('.');
  my $orig_dir = acl::Common::set_original_dir( acl::File::abs_path('.') );
  $force_initial_dir = acl::File::abs_path( $force_initial_dir || '.' );

  # Resolve library args to absolute paths
  if($#lib_files > -1) {
     if ($verbose or $library_debug) { print "Resolving library filenames to full paths\n"; }
     foreach my $libpath (@lib_paths, ".") {
        if (not defined $libpath) { next; }
        if ($verbose or $library_debug) { print "  lib_path = $libpath\n"; }

        chdir $libpath or next;
        for (my $i=0; $i <= $#lib_files; $i++) {
          my $libfile = $lib_files[$i];
          if ($libfile !~ m/\.aoclib$|\.fpgalib$/) {
            #Fix conditonal once we have the sycl flag implemented
            acl::Common::mydie("Library file has to have extension ".($sycl_mode ? "fpgalib":"aoclib"));
          }
          if (not defined $libfile) { next; }
          if ($verbose or $library_debug) { print "    lib_file = $libfile\n"; }
          if (-f $libfile) {
            my $abs_libfile = acl::File::abs_path($libfile);
            if ($verbose or $library_debug) { print "Resolved $libfile to $abs_libfile\n"; }
            push (@resolved_lib_files, $abs_libfile);
            # Remove $libfile from @lib_files
            splice (@lib_files, $i, 1);
            $i--;
          }
        }
        chdir $orig_dir;
     }

     # Make sure resolved all lib files
     if ($#lib_files > -1) {
        acl::Common::mydie ("Cannot find the following specified library files: " . join (' ', @lib_files));
     }
  }
  # expand the object files listed in the library_list.txt to .aoco and put
  # them in @aoco_library_list
  if(@library_list) {
    foreach (@library_list) {
      open(my $fh, '<', $_) or acl::Common::mydie("Cannot open -library-list file: $_\n");
      while(my $obj_file = <$fh>) {
        chomp $obj_file;
        -f $obj_file or acl::Common::mydie("Cannot open library file: $obj_file\n");
        my $aoco_file = $obj_file;
        if ( $aoco_file =~ s/\.o|\.obj/\.aoco/) {
          # the unbundler names object files with .o or .obj. We need to
          # rename them to .aoco
          `mv $obj_file $aoco_file`;
        }
        push @aoco_library_list, acl::File::abs_path($aoco_file);
      }
      close($fh);
    }
  }

  # print debug message for @all_dep_files
  if (@all_dep_files && $verbose) {
    print 'Dependency files for SYCL source and SYCL-source library: ' . join(" ", @all_dep_files) . "\n";
  }

  my $num_extracted_c_model_files;
  $$base_ref = _process_input_file_arguments(\$num_extracted_c_model_files);

  if ($aoco_to_aocr_aocx_only && @user_opencl_args) {
    print "$prog: Warning: The following parser args will be ignored in this flow: @user_opencl_args \n";
  }

  my $suffix = $$base_ref;
  $suffix =~ s/.*\.//;
  $$base_ref =~ s/\.$suffix//;

  # default name of the .aocx file and final project directory
  # dirbase_ref is used for generate the workdir. If the workdir need to be overried in HW flow, it will be resolved during extracting workdir
  $$dirbase_ref = $$base_ref;
  $linked_objfile = $$base_ref.".linked.aoco.bc";
  $x_file = $$base_ref.".aocx";

  # parse the base_ref to replace the dot and dash with underscore
  # this is for creating a valid module name
  $$base_ref =~ s/[^a-z0-9_]/_/ig;

  if($#given_input_files eq -1){
    acl::Common::mydie("No input file detected");
  }

  #in emulator flow we add the library files to the list of given_input_files and thus need an additional check when we are in the emulator flow
  my $diff_input_files = scalar @given_input_files - $num_extracted_c_model_files;

  if ($output_file and ($#given_input_files gt 0 ) and !$aoco_to_aocr_aocx_only and !($emulator_flow and ($diff_input_files eq 1)) ){
    acl::Common::mydie("Cannot specify -o with multiple input files\n");
  }

  foreach my $input_file (@given_input_files) {
    my $input_base = acl::File::mybasename($input_file);
    my $input_suffix = $input_base;
    $input_suffix =~ s/.*\.//;
    $input_base=~ s/\.$input_suffix//;
    my $input_base_ref = $input_base;
    $input_base =~ s/[^a-z0-9_]/_/ig;

    if ( $input_suffix =~ m/^cl$|^c$|^spv$|^cpp/ ) {
      push @srcfile_list, $input_file;
      my $filename = $input_base_ref.".aoco";
      # It is possible we'll have multiple source files with the same basename.
      # Uniquify the .aoco names since they all end up in the same directory.
      my $unique_index = 0;
      while (grep(/^$filename$/, @objfile_list)) {
        $filename = $input_base_ref.".$unique_index".".aoco";
        $unique_index = $unique_index + 1;
      }
      push @objfile_list, $filename;
    } elsif ( $input_suffix =~ m/^aoco$/ ) {
      push @objfile_list, acl::File::abs_path($input_file);
      if(acl::AOCDriverCommon::check_input_file_for_compile_flow($input_file, 'emulator_fast')) {
        $emulator_fast = 1;
      }
      if ($do_env_check){
        acl::AOCDriverCommon::check_aoco_file_version($input_file);
      }
    } elsif ( $input_suffix =~ m/^aocr$/ ) {
      $use_aocr_input = 1;
      $run_quartus = 1;
      push @objfile_list, acl::File::abs_path($input_file);
      if(acl::AOCDriverCommon::check_input_file_for_compile_flow($input_file, 'emulator_fast')) {
        $emulator_fast = 1;
      }
    } elsif ( $input_suffix =~ m/^xml$/ ) {
      # xml suffix is for packaging HDL components into aoco files, to be
      # included into libraries later.
      # The flow is the same as for "aoc -shared -c" for OpenCL components
      # but currently handled by "aocl-libedit" executable
      $hdl_comp_pkg_flow = 1;
      $run_quartus = 0;
      $compile_step = 1;
      push @srcfile_list, $input_file;
      push @objfile_list, $input_base_ref.".aoco";
    } else {
      acl::Common::mydie("No recognized input file format on the command line : $input_file");
    }
  }

  if ( $output_file ) {
    acl::AOCDriverCommon::filename_validation($output_file);
    my $outsuffix = $output_file;
    my $is_output_dir = 0;
    $outsuffix =~ s/.*\.//;
    # Did not find a suffix. Use default for option.
    if ($outsuffix ne "aocx" && $outsuffix ne "aocr" && $outsuffix ne "aoco") {
      if ($compile_step == 0 && $report_only == 0) {
        $outsuffix = "aocx";
      } elsif ($report_only == 1 && $hdl_comp_pkg_flow == 0){
        $outsuffix = "aocr";
      } else {
        $outsuffix = "aoco";
      }
      $is_output_dir = 1;
      $output_file .= "."  . $outsuffix;
    }
    my $outbasedir = $output_file;
    $outbasedir =~  s/\.$outsuffix//; # the directory of output file basename. e.g., <output_dir>/<file_basename>
    my $outbase = acl::File::mybasename($outbasedir); # only the basename
    if ($outsuffix eq "aoco") {
      ($run_quartus == 0 && $compile_step != 0) or acl::Common::mydie("Option -o argument cannot end in .aoco when used to name final output");
      # At this point, we know that there is only one item in @objfile_list
      $objfile_list[0] = acl::File::abs_path($outbasedir.".".$outsuffix);
      push @output_objfile_list, acl::File::abs_path($outbasedir.".".$outsuffix);
      $$dirbase_ref = undef;
      $x_file = undef;
      $linked_objfile = undef;
    } elsif ($outsuffix eq "aocr"){
      ($compile_step == 0) or acl::Common::mydie("Option -o argument cannot end in .aocr when used with -c");
      $report_only = 1;
      # We still need to either generate an aoco package for first step or read from aoco for second step so objfile_list will still have aoco
      if ($suffix ne "aoco") {
        $objfile_list[0] = acl::File::abs_path($outbasedir.".aoco"); # redirect the object file to the output dir
      }
      $$dirbase_ref = $outbasedir;
      if (! $is_output_dir) {
        $$base_ref = $outbase;
        $$base_ref =~ s/[^a-z0-9_]/_/ig; # replace all the dot and dash with underscore
      }
      $linked_objfile = $outbasedir.".linked.aoco.bc";
      $x_file = $output_file;
    } elsif ($outsuffix eq "aocx") {
      $compile_step == 0 or acl::Common::mydie("Option -o argument cannot end in .aocx when used with -c");
      $report_only == 0 or  acl::Common::mydie("Option -o argument cannot end in .aocx when used with -rtl");
      # check the valid input suffix
      if ($suffix ne "aocr") {
        if ($suffix ne "aoco") {
          # the input is source file, need to create the aoco file
          $objfile_list[0] = acl::File::abs_path($outbasedir.".aoco"); # redirect the object file to the output dir
          # only override the dirbase_ref if it is cl -> aocx flow
          $$dirbase_ref = $outbasedir;
        }
        if(! $is_output_dir) {
          $$base_ref = $outbase;
          $$base_ref =~ s/[^a-z0-9_]/_/ig; # replace all the dot and dash with underscore
        }
      }
      $linked_objfile = $outbasedir.".linked.aoco.bc";
      $x_file = $output_file;
    } elsif ($compile_step == 0) {
      acl::Common::mydie("Option -o argument must be a filename ending in .aocx when used to name final output");
    } else {
      acl::Common::mydie("Option -o argument must be a filename ending in .aoco when used with -c");
    }
     $output_file = acl::File::abs_path( $output_file );
  }

  # Parse the board package and board if not fast emulator
  if (not $emulator_fast) {
    # (1) Handle the board package, use the default one if no board or bsp specified by -board or -board-package
    if (defined $bsp_variant) {
      $bsp_variant = acl::File::abs_path($bsp_variant); # automatically change to absolute path
    } else {
      if ( not defined $board_variant or $board_variant eq $emulatorDevice ) {
        # Use the default BSP
        $bsp_variant = acl::Common::get_default_board_package();
      } else {
        $bsp_variant = acl::Common::get_bsppath_by_boardname($board_variant, $sycl_mode);
      }
    }
    # an early check if the bsp exists. A more elegant check if that bsp is valid will be in the env check
    my $env_var_to_check = $sycl_mode? "INTELFPGAOCLSDKROOT and INTEL_FPGA_ROOT" : "INTELFPGAOCLSDKROOT";
    my $no_bsp_err_str = "Cannot find the board package: $bsp_variant\n".
                         (($user_defined_bsp)?
                           "Please make sure the option -board-package points to the path to a valid board\npackage. " :
                           "Please make sure the environmental variable $env_var_to_check is set\ncorrectly. ") .
                         "You can use the option -list-board-packages to find out the available\nboard packages. Refer to \"aoc -help\" for more information.\n";
    acl::Common::mydie($no_bsp_err_str) if (not -d $bsp_variant);
    $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $bsp_variant;

    # (2) Handle the board variant, use the default one if board is not specified by the option -board
    #     Note: treat EmulatorDevice as undefined so we get a valid board, while don't mark it as using_default_board
    if ( not defined $board_variant or $board_variant eq $emulatorDevice ) {
        $board_variant = acl::Common::get_default_board($bsp_variant);
        $$using_default_board_ref = 1 if ($board_variant eq $emulatorDevice);
    }
    # an early check if the board exists in the bsp in use.
    my $no_board_err_str = "Cannot find the board variant: $board_variant".
                           (($user_defined_bsp) ?
                             " in the board package: $bsp_variant\n" .
                             "Please make sure the option -board points to a valid board variant in the\npackage $bsp_variant.\n" :
                             (($user_defined_board)?
                               "\nPlease make sure the option -board points to a valid board variant in the\ninstalled or shipped board packages,".
                               " or use -board-package=<board package name or path>\nto point to a board package that defines $board_variant.\n" :
                               "\n")) .
                           "You can use the option -list-boards to find out the available boards. Refer to\n\"aoc -help\" for more information.\n";
    acl::Common::mydie($no_board_err_str) if( not acl::Common::is_board_in_the_bsp($board_variant, $bsp_variant));
  }

  # For incremental compile to preserve partitions correctly, project name ($base) must be the same as
  # the previous compile. The $base name will be used in the hpath, so it is required to preserve the
  # previous partitions.
  # The $dirbase, .aoco, and .aocx file names will not be changed.
  if ($incremental_compile) {
    my $prev_info = "";
    if ($incremental_input_dir && -e "$incremental_input_dir/reports/lib/json/info.json") {
      $prev_info = "$incremental_input_dir/reports/lib/json/info.json";
    } elsif ($$dirbase_ref && -e "$$dirbase_ref/reports/lib/json/info.json") {
      $prev_info = "$$dirbase_ref/reports/lib/json/info.json";
    }
    $$base_ref = acl::Incremental::get_previous_project_name($prev_info) if $prev_info;
  }

  for (my $i = 0; $i <= $#objfile_list; $i++) {
    $objfile_list[$i] = acl::File::abs_path($objfile_list[$i]);
  }
  $x_file = acl::File::abs_path( $x_file );
  $linked_objfile = acl::File::abs_path( $linked_objfile );

  if ($#srcfile_list >= 0) { # not necesaarily set for "aoc file.aoco"
    chdir $force_initial_dir or acl::Common::mydie("Can't change into dir $force_initial_dir: $!\n");
    foreach my $src (@srcfile_list) {
      -f $src or acl::Common::mydie("Invalid kernel file $src: $!");
      my $absolute_src = acl::File::abs_path($src);
      -f $absolute_src or acl::Common::mydie("Internal error. Can't determine absolute path for $src");
      push @absolute_srcfile_list, $absolute_src;
    }
    chdir $orig_dir or acl::Common::mydie("Can't change into dir $orig_dir: $!\n");
  }

  if (acl::Env::is_windows() and $#absolute_srcfile_list >= 0) {
    foreach my $abs_src (@absolute_srcfile_list) {
      # Check file first line, if equal to new encryption line then error out
      my $check_str = "`pragma protect begin_protected";
      open my $abs_src_file, '<', $abs_src;
      my $first_line = <$abs_src_file>;
      chomp($first_line);
      if ($check_str eq $first_line) {
        acl::Common::mydie("Your design contains encrypted source not supported by this version. Please contact your sales support team to ensure you are using the correct software version to support this flow.");
      }
      close $abs_src_file;
    }
  }

  # get the absolute path for the Profiler Config file
  if(defined $profilerconf_file) {
      chdir $force_initial_dir or acl::Common::mydie("Can't change into dir $force_initial_dir: $!\n");
      -f $profilerconf_file or acl::Common::mydie("Invalid profiler config file $profilerconf_file: $!");
      $absolute_profilerconf_file = acl::File::abs_path($profilerconf_file);
      -f $absolute_profilerconf_file or acl::Common::mydie("Internal error. Can't determine absolute path for $profilerconf_file");
      chdir $orig_dir or acl::Common::mydie("Can't change into dir $orig_dir: $!\n");
  }

  # Output file must be defined for this flow
  if ($hdl_comp_pkg_flow) {
    defined $output_file or acl::Common::mydie("Output file must be specified with -o for HDL component packaging step.\n");
  }
  if ($created_shared_aoco and $emulator_flow) {
    acl::Common::mydie("-shared is not compatible with emulator flow.");
  }
  if ($compile_step == 1 and $incremental_compile) {
    acl::Common::mydie("-c flow not compatible with incremental flow");
  }
  if ($aocr_to_aocx_only == 1 && $user_defined_flow == 1) {
    if ( $emulator_flow or $new_sim_mode ) {
      my $flow_name = $emulator_flow ? "emulator" : "simulator";
      acl::Common::mydie("-march=$flow_name can not be used in this flow");
    }
  }

  # Deal with the path length (only for Windows)
  # AOC will calculate the budget and pass it to llc. The budget comes from:
  # limit - (max(len(abs_path(work_dir)), len(abs_path(report_dir))) + len(kernel_hdl/))
  if (acl::Env::is_windows()) {
    my $maxWinLength = 259;
    my $workdirLength = length(acl::File::abs_path($$dirbase_ref));
    my $redirectReportFolderLength = $change_output_folder_name? length(acl::File::abs_path($change_output_folder_name)) : 0; 
    my $maxWorkdirFileLength = (($redirectReportFolderLength > $workdirLength) ? $redirectReportFolderLength : $workdirLength) + 1; # including the "\"
    my $exceed_limits = 0;
    my $path_budget = 0;
    # Get longest relative report path length
    my @stack = ("$ENV{INTELFPGAOCLSDKROOT}/share/lib/acl_report/lib");
    my $longest_absolute_report_path_length = 0;
    while(@stack) {
      my $cur_path = pop(@stack);
      if (length $cur_path > $longest_absolute_report_path_length) {
        $longest_absolute_report_path_length = length $cur_path;
      }
      my @children = glob("$cur_path/*");
      push(@stack, @children);
    }
    # Absolute path: $INTELFPGAOCLSDKROOT/share/lib/acl_report/...
    # Relative path: a.prj/reports/...
    # To get relative report path under prj dir based on absolute path under $INTELFPGAOCLSDKROOT dir:
    my $longest_relative_report_path_length = $longest_absolute_report_path_length - length($ENV{INTELFPGAOCLSDKROOT}) - length("/share/lib/acl_report") + length("/reports");

    # Firstly check if report path exceeds the limit already
    if ($maxWorkdirFileLength + $longest_relative_report_path_length > $maxWinLength) {
      $exceed_limits = 1;
    } else {
      # Secondly check if the budget is at least greater than the minimum length
      # the minimum_length = len("kernel_hdl/") + len(min_prefix) + len(min_suffix) + len(min_unique_id) + len(max_extension)
      my ($min_prefix, $min_suffix, $min_unique_id, $max_extension) = (1, 1, 1, 4);
      my $minimum_length = length("kernel_hdl/") + $min_prefix + $min_suffix + $min_unique_id + $max_extension;
      $path_budget = $maxWinLength - $maxWorkdirFileLength;
      $exceed_limits = 1 if ($path_budget < $minimum_length);
    }
    if ($exceed_limits) {
      acl::Common::mydie("The length of current path is too long. Please relocate your design under a shorter path, or enable Windows long path. For more details please refer to the Windows documentation.\n");
    } else {
      # Pass the budget to llc, and let llc to truncate the kernel name according to the budget
      $llc_arg_after .= " -path-budget $path_budget";
      # Pass the length of project directory name to llc
      my $prj_dir_len = length(acl::File::mybasename($$dirbase_ref));
      $llc_arg_after .= " -prj-dir-len $prj_dir_len";
    }
  }

  if ($change_output_folder_name and -d $change_output_folder_name) {
    # try to remove the redirect report folder before aoc compilation begins
    # this cannot be performed before the check of long path is done
    if (not acl::File::remove_tree($change_output_folder_name) or -d $change_output_folder_name) {
      acl::Common::mydie("Failed to remove the existing $change_output_folder_name. Please first remove it before compiling.\n");
    }
  }


  # Can't do multiple flows at the same time
  if ($compile_step + $run_quartus + $aoco_to_aocr_aocx_only > 1 ) {
      acl::Common::mydie("Cannot have more than one of -c and --hw on the command line,\n cannot combine -c with *.aoco or *.aocr and -rtl with *.aocr either\n");
  }

  # Can't do -c and -rtl at the same time
  if ($c_flag_only + $report_only + $run_quartus > 1 ) {
      acl::Common::mydie("Cannot have more than one of -c, -rtl, -hw on the command line,\n cannot combine -rtl with *.aocr either\n");
  }

  # No -reuse-exe and -reuse-aocx
  if (defined $reuse_exe_file && defined $reuse_aocx_file) {
    acl::Common::mydie("Cannot have -reuse-exe and -reuse-aocx on the command line\n");
  }
}

1;
