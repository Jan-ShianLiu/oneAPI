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
#           One or more object file
#           An optional object library file name
#  Output:  An library file suitable for the target platform
#
#
# Example:
#     Command:
#        fpga_libtool --create prim.aoclib --target aoc prim.aoco
#     Generates:
#        prim.aoclib - object file suitable for us by the aoc compiler
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
require acl::File;
require acl::Pkg;
require acl::Common;
use acl::AOCDriverCommon;
use acl::HLS_Pkg;

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

my $prog = acl::File::mybasename($0);
$prog =~ s/.pl$//;
my $usage =<<USAGE;
Usage: $prog --target [hls|ocl|sycl] --create <lib>.[a|aoclib] <file>.aoco +
Options:
    -h, --help        Print this help, then exit successfully.
    -n, --dry-run     Don\'t actually do anything, just check command line
    --create <lib>.[a|aoclib]
                      Create a library named <lib>.[a|aoclib]. Existing
                      file will be overwritten
    --target [hls|ocl|sycl]
                      Generate a library suitable for the target type
Note that the input files and the library must be for the same platform.
So .aoco -> .aoclib
   .o    -> .a
USAGE

sub create_opencl_library {
  my ($library_name, $objects, $verbose) = @_;
  print "Verify input tool versions...\n" if $verbose;
  my $acl_version = undef;
  my $version_check_failed = 0;
  foreach my $file (@$objects) {
    my $pkg = get acl::Pkg($file) or mybail "Can't find pkg file $file: $acl::Pkg::error";
    my $version = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.version');
    if (!$acl_version){
      $acl_version = $version;
      print "Found $acl_version, expecting $acl_version for all files\n" if $verbose;
    } else {
      if (!($acl_version eq $version)){
      print "Tool version $version in $file mismatch expected value\n";
      $version_check_failed = 1;
      }
    }
  }
  if ($version_check_failed){
    mybail "All objects must be built with the same tool version";
  } else {
    my $lib = create acl::Pkg($library_name);
    print "Version check succedded. Adding input files to $library_name.\n" if $verbose;
    while (@$objects) {
      my $file = (shift @$objects);
      my $archive_name = acl::File::mybasename($file);
      print "Add $file to $library_name\n" if $verbose;
      $lib->set_file(".archive.$archive_name",$file);
    }
    # AOCDriverCommon functions are dependent on global variables being
    # correctly set before they are called :-(
    print "Set library tool version\n" if $verbose;
    chomp($work_dir=`pwd`);
    acl::AOCDriverCommon::save_pkg_section($lib,'.acl.version', $acl_version);
    acl::AOCDriverCommon::save_pkg_section($lib,'.acl.comp_table', undef);
  }
}

sub create_hls_library {
  my ($library_name, $objects, $verbose) = @_;
  print "Verify input tool versions... \n" if $verbose;
  my $acl_version = undef;
  my $version_check_failed = 0;
  my $section_name = (isLinux())? '.acl.version' : '.acl.ver';
  foreach my $file (@$objects) {
    my $acl_version_file = $file.'_version.txt';
    my $version = undef;
    if (acl::HLS_Pkg::get_section_from_object_file({'verbose' => $verbose},$file,$section_name,$acl_version_file)){
      open(my $fh_acl_version_file, '<', $acl_version_file) or mybail "Could not create temporary version file\n";
      $version = <$fh_acl_version_file>;
      close $fh_acl_version_file;
      if (!$acl_version){
        $acl_version = $version;
        print "Found $acl_version, expecting $acl_version for all files\n" if $verbose;
      } else {
        if (!($acl_version eq $version)){
        print "Tool version $version in $file mismatch expected value. Will not compile $library_name\n";
        $version_check_failed = 1;
        }
      }
    } else {
      print "Could not find tool version in $file\n" if $verbose;
      $version_check_failed = 1;
    }
    unlink $acl_version_file;
  }
  if ($version_check_failed){
    mybail "All objects must be built with the same tool version";
  }
  if (isLinux()) {
    (acl::Common::mysystem_full({},"ar rc $library_name @$objects")==0) or mybail "Not able to create library $library_name";
  } else {
    (acl::Common::mysystem_full({},"lib /NOLOGO /OUT:$library_name @$objects")==0) or mybail "Not able to create library $library_name";
  }
}

sub create_sycl_library {
  my ($library_name, $objects) = @_;
  if (isLinux()) {
    (acl::Common::mysystem_full({},"ar rc $library_name @$objects")==0) or mybail "Not able to create library $library_name";
  } else {
    (acl::Common::mysystem_full({},"lib /NOLOGO /OUT:$library_name @$objects")==0) or mybail "Not able to create library $library_name";
  }
}

sub run() {
  my $target_type = undef;
  my $library_name = undef;
  my @objects;
  my $verbose = undef;
  my $dry_run = 0;

  #Parse input
  while (@ARGV) {
    my $arg = shift @ARGV;

    if ( ($arg eq '-n') || ($arg eq '--dry-run') ) {
      $dry_run = 1;
    } elsif ( ($arg eq '-h') || ($arg eq '--help') ) {
      print $usage; exit 0;
    } elsif ($arg eq '-v') {
      $verbose +=1;
    } elsif ( $arg eq '--target' ) {
      ($#ARGV >= 0 and $ARGV[0] !~ m/^-./) or mybail "Option $arg requires a file argument.";
      $arg = shift @ARGV;

      if ( $arg eq 'ocl' or $arg eq 'hls' or $arg eq 'sycl' ) {
        $target_type = $arg;
      } elsif ( $arg eq 'aoc') {
        print "Warning: Target 'aoc' is deprecated. Use 'ocl'.\n";
        $target_type = 'ocl';
      } else {
        mybail("Unsupported target type: $arg");
      }
    } elsif ( $arg eq '--create' ) {
      ($#ARGV >= 0 and $ARGV[0] !~ m/^-./) or mybail "Option $arg requires a file argument.";
      $library_name = shift @ARGV;
    } else {
      push @objects, $arg;
    }
  }

  acl::Common::set_verbose($verbose);

  # Check arguments
  ($target_type) or mybail "--target argument is required";
  ($library_name) or mybail "--create argument is required";
  ($#objects>=0) or mybail "at least one object file is required";

  my $input_suffix = undef;
  my $library_suffix = $library_name;
  $library_suffix =~ s/.*\.//;
  if ($target_type eq 'ocl') {
    $input_suffix = 'aoco';
    ( $library_suffix eq 'aoclib') or mybail "aoc library must have the suffix .aoclib";
  } elsif ($target_type eq 'hls' or $target_type eq 'sycl') {
    if ( isLinux() ) {
      $input_suffix = 'o';
      ($library_suffix eq 'a') or mybail "$target_type library must have the suffix .a";
    } else {
      $input_suffix = 'obj';
      ($library_suffix eq 'lib') or mybail "$target_type library must have the suffix .lib";
    }
  }

  foreach my $input (@objects) {
    my $tmp = $input;
    $tmp =~ s/.*\.//;
    if ($tmp ne $input_suffix) {
      mybail "Cannot generate library of type $library_suffix based on object files of type $tmp";
    }
  }

  # Stop before packaging
  if ($dry_run) {
  exit (0);
  }

  if ($target_type eq 'ocl') {
    create_opencl_library($library_name, \@objects, $verbose);
  } elsif ($target_type eq 'hls') {
    create_hls_library($library_name, \@objects, $verbose);
  } elsif ($target_type eq 'sycl') {
    create_sycl_library($library_name, \@objects);
  }
}

run();
