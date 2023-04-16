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


#  Inputs:  A target
#           A source file
#           An optional object file name
#  Output:  An object file sutable for the target platform
#
#
# Example:
#     Command:
#        fpga_crossgen --target ocl -o prim.aoco prim.cpp
#        fpga_crossgen --target ocl -o prim.aoco prim.xml --emulation_model prim.cpp
#     Generates:
#        prim.aoco - object file suitable for inclusion into libraries
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
use Exporter;
require acl::Env;
require acl::Common;
require acl::File;
require acl::Pkg;
require acl::HLS_Pkg;
use acl::AOCDriverCommon;

# Executables
my $llc_exe = $ENV{'INTELFPGAOCLSDKROOT'}."/llvm/aocl-bin"."/aocl-llc";

my $prog = acl::File::mybasename($0);
$prog =~ s/.pl$//;
my $usage =<<USAGE;
Usage: $prog -o <lib>.aoco --target [ocl|hls|sycl] --source [ocl|hls|sycl] <file>.[cpp|cl|xml] [opt]
Options:
    -h, --help         Print this help, then exit successfully.
    -n, --dry-run      Don\'t actually do anything, just check command line
    --target [ocl|hls|sycl] Generate a object file suitable for the target type
    --source [ocl|hls|sycl] Generate a object file from the source type
    --emulation_model <file>.[cpp|cl]
                       Add cpu model for emulation. Only valid if main file is xml
    -o <object name>   Make the new object file be named <object name>
    [opt]              Anything not recognized will be sent to the
                       underlying compiler
Note that the object files and the target must be for the same platform.
So target=aoc -> object file .aoco
So target=hls -> object file .o
USAGE

# checks host OS, returns true for linux, false for windows.
sub isLinux {
  if ($^O eq 'linux') {
    return 1;
  }
  return;
}

# Prefered to die, since die always prints where we bailed out (line number) 
sub myexit {
  my $exit_code = shift;
  if (scalar @_){
    my $message = shift;
    print STDERR "Error: $message\n";
  }
  exit $exit_code;
}

sub mybail {
  myexit (1, @_);
}

my $SYCL_AOCO_SECTION = 'sycl-fpga_aoco-intel-unknown-sycldevice';
my $SYCL_X86_SECTION = isLinux() ? 'host-x86_64-unknown-linux-gnu' : 'host-x86_64-pc-windows-msvc';
my $AOC_X86_SECTION = isLinux() ? '.acl.clang_ir.x86_64-unknown-linux-intelfpga' : '.acl.clang_ir.x86_64-pc-windows-intelfpga';

sub create_opencl_library_object {
  my ($source_file, $object_file, $source_type, $rtl_flow, $emulation_model, $compiler_args, $toolchain, $verbose, $target_sycl) = @_;
  if(length $toolchain){
    push @$compiler_args, ('-preproc-gcc-toolchain', $toolchain);
  }
  my $createlibobject_mode = '-createlibobject' . $target_sycl;
  if (!$rtl_flow and ($source_type eq 'hls' or $source_type eq 'ocl')) {
    print "Creating OpenCL Library object from $source_file for $object_file\n" if $verbose;
    (acl::Common::mysystem_full({}, "aoc -q -c $createlibobject_mode $source_file -o $object_file -I/usr/include @$compiler_args")==0) or myexit ($?>>8);
  } elsif ($rtl_flow) {
    print "Packaging RTL described by $source_file into OpenCL Library object $object_file\n" if $verbose;
    (acl::Common::mysystem_full({}, "aocl xlibrary createobject $source_file -o $object_file")==0) or myexit ($?>>8);
    my $handle = get acl::Pkg($object_file);
    chomp($work_dir=`pwd`);
    acl::AOCDriverCommon::save_pkg_section($handle,'.acl.version',acl::Env::sdk_version());
    acl::AOCDriverCommon::save_pkg_section($handle,'.acl.target','library');
    if (!$emulation_model) {
      print "Warning: No emulation model supplied, library will not work in emulation\n";
    } else {
      print "Creating Emulation model from $source_file and packaging it into OpenCL Library object $object_file\n" if $verbose;
      (acl::Common::mysystem_full({}, "aoc -q -c $createlibobject_mode $emulation_model -o tmp.aoco @$compiler_args")==0) or myexit ($?>>8);
      my $pkg = get acl::Pkg("tmp.aoco");
      $pkg->get_file($AOC_X86_SECTION,'tmp.bc');
      $handle->set_file($AOC_X86_SECTION,'tmp.bc');
      if ($pkg->exists_section('.acl.ioc_obj')){
        $pkg->get_file('.acl.ioc_obj','tmp.bc');
        $handle->set_file('.acl.ioc_obj','tmp.bc');
      } elsif ($pkg->exists_section('.acl.ioc_bc')){
        $pkg->get_file('.acl.ioc_bc','tmp.bc');
        $handle->set_file('.acl.ioc_bc','tmp.bc');
      } else {
        print "Warning: No library image found for fast emulator\n";
      }
      acl::File::remove_tree('temp.aoco');
      acl::File::remove_tree('tmp.bc');
    }
  } elsif ($source_type eq 'sycl') {
    mybail "sycl source with target ocl not supported\n"
  }
}

sub create_hls_library_object {
  my ($source_file, $object_file, $source_type, $rtl_flow, $emulation_model, $compiler_args, $toolchain_arg, $verbose) = @_;
  if (!$rtl_flow and ($source_type eq 'hls' or $source_type eq 'ocl')) {
    print "Creating HLS Library object from $source_file for $object_file\n" if $verbose;
    (acl::Common::mysystem_full({}, "i++ $toolchain_arg -c --createlibobject $source_file -o $object_file @$compiler_args")==0) or myexit ($?>>8);
    exit(0);
  } elsif ($rtl_flow) {
    my $tmp_transfer_file = ".tmp";
    my $tmp_aoco = ".tmp.aoco";
    if (!$emulation_model) {
      print "Warning: No emulation model supplied, library will not work in emulation\n";
      my $dummy = "dummy.cpp";
      open FILE, ">$dummy"; binmode FILE; close FILE;
      (acl::Common::mysystem_full({}, "i++ $toolchain_arg -c -march=x86-64 --createlibobject $dummy -o $object_file @$compiler_args")==0) or myexit ($?>>8);

      unlink $dummy;
    } else {
      print "Creating Emulation model from $emulation_model and packaging it into HLS Library object $object_file\n" if $verbose;
      (acl::Common::mysystem_full({}, "i++ $toolchain_arg -c -march=x86-64 --createlibobject $emulation_model -o $object_file @$compiler_args")==0) or myexit ($?>>8);
    }
    print "Packaging RTL described by $source_file into HLS Library object $object_file\n" if $verbose;
    (acl::Common::mysystem_full({}, "aocl xlibrary createobject $source_file -o $tmp_aoco")==0) or myexit ($?>>8);
    my $handle = get acl::Pkg($tmp_aoco);
    ##Put Xml spec (from cmd line) into native object file
    if (isLinux()) {
      my $contents = `aocl binedit $tmp_aoco list`;
      foreach (split(/\n/,$contents)) {
        $_ =~ /\.(source|xml_spec)\.(.+\.[a-z]+),\s+[0-9]+\s+bytes/ || next;
        my $section = ".$1\.$2";
        $handle->get_file($section,$tmp_transfer_file);
        if ($1 eq 'xml_spec') {
          $section = ".xml";
        }
        acl::HLS_Pkg::add_section_to_object_file({'verbose' => $verbose}, $object_file, $section, $tmp_transfer_file);
        unlink $tmp_transfer_file;
      }
    } else {
      my $idx_file = "_index.txt";
      open INDEX, ">$idx_file";
      my $shortname = ".xidx";
      my $index_nr = 0;
      my $contents = `aocl binedit $tmp_aoco list`;
      foreach (split(/\n/,$contents)) {
        $_ =~ /\.(source|xml_spec)\.(.+\.[a-z]+),\s+[0-9]+\s+bytes/ || next;
        my $section = ".$1\.$2";
        $handle->get_file($section,$tmp_transfer_file);
        if ($1 eq 'xml_spec') {
          $section = ".xml";
        } else {
          print INDEX "$section => $shortname$index_nr\n";
          $section = $shortname.$index_nr;
        }
        acl::HLS_Pkg::add_section_to_object_file({'verbose' => $verbose}, $object_file, $section, $tmp_transfer_file);
        unlink $tmp_transfer_file;
        $index_nr = $index_nr + 1;
      }
      close INDEX;
      acl::HLS_Pkg::add_section_to_object_file({'verbose' => $verbose}, $object_file, ".xmlidx", $idx_file);
      unlink $idx_file;
    }
    unlink $tmp_aoco;
    # add section to describe tool version
    my $acl_version_file = "_acl_version.txt";
    open(ACL_VERSION, ">$acl_version_file") or mybail "Could not create version file\n";
    print ACL_VERSION acl::Env::sdk_version();
    close ACL_VERSION;
    acl::HLS_Pkg::add_section_to_object_file({'verbose' => $verbose},$object_file,'.acl.version', $acl_version_file);
    unlink $acl_version_file;
  } elsif ($source_type eq 'sycl') {
    mybail "sycl source with target hls not supported\n"
  }
}

sub create_sycl_library_object {
  my ($source_file, $object_file, $object_suffix, $source_type, $rtl_flow, $emulation_model, $compiler_args, $toolchain, $toolchain_arg, $verbose) = @_;
  print "Creating SYCL Library object from $source_file for $object_file\n" if $verbose;

  if ($source_type eq 'sycl' and !$rtl_flow) {
    (acl::Common::mysystem_full({}, "dpcpp -fintelfpga -c $source_file -o $object_file @$compiler_args $toolchain_arg")==0) or myexit ($?>>8);
  } elsif ($source_type eq 'sycl' and $rtl_flow) {
    mybail "SYCL source emulation model not supported for RTL libraries"
  } else {
    # Non-SYCL source
    # aoc can handle HLS, OpenCL, and RTL source libraries, so use OpenCL target flow then build SYCL object

    my $tmp_aoco_file = $object_file;
    my $tmp_x86_bc_file = $object_file;
    my $tmp_x86_obj_file = $object_file;

    $tmp_aoco_file =~ s/\.$object_suffix/\.tmp\.aoco/;
    $tmp_x86_bc_file =~ s/\.$object_suffix/_x86\.tmp\.bc/;
    $tmp_x86_obj_file =~ s/\.$object_suffix/_x86\.tmp\.$object_suffix/;

    # Create .aoco that we will put in SYCL object
    my $target_sycl = 'forsycl';
    create_opencl_library_object($source_file, $tmp_aoco_file, $source_type, $rtl_flow, $emulation_model, $compiler_args, $toolchain, $verbose, $target_sycl);

    # Extract x86 section from .aoco then put x86 part and .aoco in sections of SYCL object
    if($rtl_flow and !$emulation_model) {
      # No emulation model means no x86 section in the .aoco
      # But SYCL still requires one so make it empty
      (acl::Common::mysystem_full({}, "touch $tmp_x86_bc_file")==0) or myexit ($?>>8);
    } else {
      (acl::Common::mysystem_full({}, "aocl binedit $tmp_aoco_file get $AOC_X86_SECTION $tmp_x86_bc_file")==0) or myexit ($?>>8);
    }
    (acl::Common::mysystem_full({}, "$llc_exe -filetype=obj -o $tmp_x86_obj_file $tmp_x86_bc_file")==0) or myexit ($?>>8);
    (acl::Common::mysystem_full({}, "clang-offload-bundler -type=o -targets=$SYCL_X86_SECTION,$SYCL_AOCO_SECTION -inputs=$tmp_x86_obj_file,$tmp_aoco_file -outputs=$object_file")==0) or myexit ($?>>8);

    # Clean up temp files
    acl::File::remove_tree($tmp_aoco_file);
    acl::File::remove_tree($tmp_x86_bc_file);
    acl::File::remove_tree($tmp_x86_obj_file);
  }
}

sub run() {
  my $source_type = undef;
  my $target_type = undef;
  my $source_file = undef;
  my $object_file = undef;
  my $object_suffix = undef;
  my $rtl_flow = undef;
  my $emulation_model = undef;
  my @compiler_args;
  my $verbose = undef;
  my $dry_run = 0;

  my $toolchain_dir= $ENV{'INTELFPGAOCLSDKROOT'}."/windows64/bin"."/../../../gcc";
  # Toolchain priority:
  # 1. OVERRIDE:  Command line argument
  # 2. STANDARD:  Installed directory
  # 3. FALL-BACK: Clang analysis
  # 
  # Make "directory" override "analysis"
  # (this variable can still be overwritten by "argument" parsing)
  my $toolchain = (isLinux() && -d $toolchain_dir) ? $toolchain_dir : undef;

  while (@ARGV) {
    my $arg = shift @ARGV;
    if ( ($arg eq '-n') || ($arg eq '--dry-run') ) {
        $dry_run = 1;
    } elsif ( ($arg eq '-h') || ($arg eq '--help') ) {
        print $usage; exit 0;
    } elsif ($arg eq '-v') {
        $verbose +=1;
    } elsif ( $arg eq '--source' ) {
      ($#ARGV >= 0 and $ARGV[0] !~ m/^-./) or mybail "Option $arg requires a source argument.";
      $arg = shift @ARGV;

      if ( $arg eq 'ocl' or $arg eq 'hls' or $arg eq 'sycl') {
        $source_type = $arg;
      } else {
        mybail "Unsupported source type: $arg";
      }
    } elsif ( $arg eq '--target' ) {
      ($#ARGV >= 0 and $ARGV[0] !~ m/^-./) or mybail "Option $arg requires a target argument.";
      $arg = shift @ARGV;

      if ( $arg eq 'ocl' or $arg eq 'hls' or $arg eq 'sycl') {
        $target_type = $arg;
      } elsif ( $arg eq 'aoc') {
        print "Warning: Target 'aoc' is deprecated. Use 'ocl'.\n";
        $target_type = 'ocl';
      } else {
        mybail "Unsupported target type: $arg";
      }
    } elsif ( $arg eq '--emulation_model') {
      ($#ARGV >= 0 and $ARGV[0] !~ m/^-./) or mybail "Option $arg requires a file argument.";
      $arg = shift @ARGV;
      if ($arg =~ m/\.cpp$|\.cl$/) {
        (!$emulation_model) or mybail "Only one emulation model file is allowed\n";
        $emulation_model = $arg;
      } else {
        mybail "Emulation model file has to be C++ or OpenCL\n";
      }
    } elsif ( $arg =~ m/^--gcc-toolchain=/ ) {
      if (acl::Env::is_windows()) {
        print "Warning: --gcc-toolchain argument ignored\n";
      } else {
        $toolchain = $arg;
        $toolchain =~ s/--gcc-toolchain=//;
      }
    } elsif ( $arg eq '-o' ) {
      ($#ARGV >= 0 and $ARGV[0] !~ m/^-./) or mybail "Option $arg requires a file argument.";
      $object_file = shift @ARGV;
    } elsif ($arg =~ m/\.cpp$|\.cl$|\.xml$/) {
      (!$source_file) or mybail "Only one source file is allowed\n";
      $source_file = $arg;
    } else {
      # Unknown arguments forward to underlying compiler
      push @compiler_args, $arg;
    }
  }

  acl::Common::set_verbose($verbose);
  # Peel of the two -v's we used here and apply any remaining ones to aoc/i++
  for my $i (3..$verbose) {
      push @compiler_args, '-v';
  }

  # Check arguments
  ($target_type) or mybail "--target argument is required";
  ($source_file) or mybail "Exactly one one source file is required";
  my $toolchain_arg = (length $toolchain) ? "--gcc-toolchain=\"$toolchain\"" : "";

  my $stem = acl::File::mybasename($source_file);
  my $suffix = $stem;
  $suffix =~ s/.*\.//;
  $stem =~ s/\.$suffix//;

  # Verify that source file extension matches source type, and determine source type if none given
  if($suffix eq 'cl') {
    ($source_type eq 'ocl') or (not $source_type) or mybail ".cl source file must use source type ocl\n";
    if(not $source_type) {
      $source_type = 'ocl';
    }
  } elsif($suffix eq 'cpp') {
    ($source_type eq 'hls') or ($source_type eq 'sycl') or (not $source_type) or mybail ".cpp source file must use source type hls or sycl\n";
    if(not $source_type) {
      $source_type = 'hls'; # Assume HLS if no source specified, for legacy support
    }
  } elsif($suffix eq 'xml') {
    $rtl_flow = 1;
    if(not $source_type) {
      if($emulation_model =~ m/\.cpp/) {
        $source_type = 'hls';
      } elsif($emulation_model =~ m/\.cl/) {
        $source_type = 'ocl';
      }
    }
  } else {
    mybail "Unexpected source file extension: " . $suffix . "\n";
  }

  # Set object file name if none given
  if (!$object_file) {
    if ($target_type eq 'ocl') {
      $object_file = $stem.'.aoco';
    } elsif ($target_type eq 'hls' or $target_type eq 'sycl') {
      if (isLinux()) {
        $object_suffix = 'o';
      } else {
        $object_suffix = 'obj';
      }
      
      $object_file = $stem.'.'.$object_suffix;
    }
  }

  # Verify that object suffix matches target
  $object_suffix = $object_file;
  $object_suffix =~ s/.*\.//;
  if ($target_type eq 'ocl') {
    ( $object_suffix eq 'aoco') or mybail "ocl object files must have the suffix .aoco\n";
  } elsif ($target_type eq 'hls' or $target_type eq 'sycl') {
    if ( isLinux() ) {
      ($object_suffix eq 'o') or mybail "$target_type object file must have the suffix .o on linux\n";
    } else {
      ($object_suffix eq 'obj') or mybail "$target_type object file must have the suffix .obj on windows\n";
    }
  }

  # Stop before compilation
  if ($dry_run) {
    exit (0);
  }

  if ($target_type eq 'ocl') {
    create_opencl_library_object($source_file, $object_file, $source_type, $rtl_flow, $emulation_model, \@compiler_args, $toolchain, $verbose);
  } elsif ($target_type eq 'hls') {
    create_hls_library_object($source_file, $object_file, $source_type, $rtl_flow, $emulation_model, \@compiler_args, $toolchain_arg, $verbose);
  } elsif ($target_type eq 'sycl') {
    create_sycl_library_object($source_file, $object_file, $object_suffix, $source_type, $rtl_flow, $emulation_model, \@compiler_args, $toolchain, $toolchain_arg, $verbose);
  }
}

run();
