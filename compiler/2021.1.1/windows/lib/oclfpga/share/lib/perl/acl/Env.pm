
=pod

=head1 NAME

acl::Env - Mediate read access to the environment used by the Intel(R) FPGA SDK for OpenCL(TM).

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



=head1 SYNOPSIS

   use acl::Env;

   my $sdk_root = acl::Env::sdk_root();
   print "Did you set environment variable ". acl::Env::sdk_root_name() ." ?\n";

=cut

package acl::Env;
require Exporter;
@acl::Pkg::ISA        = qw(Exporter);
@acl::Pkg::EXPORT     = ();
@acl::Pkg::EXPORT_OK  = qw();
use strict;
require acl::Board_env;
require acl::File;
require acl::Common;

# checks host OS, returns true for linux, false otherwise.
sub is_linux() {
    if ($^O eq 'linux') {
      return 1;
    }
    return 0;
}

# checks for Windows host OS. Returns true if Windows, false otherwise.
sub is_windows() { return $^O =~ m/Win/; }
sub pathsep() { return is_windows() ? '\\' : '/'; }

# On what platform are we running?
# One of windows64, linux64 (Linux on x86-64), ppc64 (Linux on Power), arm32 (SoC)
sub get_arch {
   my ($arg) = shift @_;
   my $arch = $ENV{'AOCL_ARCH_OVERRIDE'};
   chomp $arch;
   if ($arg eq "--arm") {
      return 'arm32';
   } elsif ( ( $arch && $arch =~ m/Win/) || is_windows() ) {
      return 'windows64';
   } else {
      # Autodetect architecture.
      # Can override with an env var, for testability.  Matches shell wrapper.
      $arch = $arch || acl::Common::mybacktick('uname -m');
      chomp $arch;
      $ENV{'AOCL_ARCH_OVERRIDE'} = $arch; # Cache the result.
      if ( $arch =~ m/^x86_64/ ) {
         return 'linux64';
      } elsif ( $arch =~ m/^ppc64le/ ) {
         return 'ppc64le';
      } elsif ( $arch =~ m/^ppc64/ ) {
         return 'ppc64';
      } elsif ( $arch =~ m/^armv7l/ ) {
         return 'arm32';
      }
   }
   return undef;
}

sub get_icd_dir() {return is_windows() ? 'HKEY_LOCAL_MACHINE\\Software\\Khronos\\OpenCL\\Vendors' : '/etc/OpenCL/vendors' ;}
sub get_fcd_dir() {
  my $default_fcd_path = is_windows() ? 'HKEY_LOCAL_MACHINE\\Software\\Intel\\OpenCL\\Boards' : '/opt/Intel/OpenCL/Boards' ;
  my $fcd_path;
  if(defined $ENV{ACL_BOARD_VENDOR_PATH}) {
    my $user_defined_fcd_path = $ENV{ACL_BOARD_VENDOR_PATH};
    # resolve the trailling slashes
    $user_defined_fcd_path =~ s/\/$//;
    $fcd_path = is_windows()? 'HKEY_CURRENT_USER\\Software\\Intel\\OpenCL\\Boards' : $user_defined_fcd_path;
  } else {
    $fcd_path = $default_fcd_path;
  }
  return $fcd_path
}

sub sdk_root() { return $ENV{'INTELFPGAOCLSDKROOT'} || $ENV{'ACL_ROOT'}; }
sub sdk_root_name() { return 'INTELFPGAOCLSDKROOT'; }
sub sdk_root_shellname() {
   return (is_windows() ? '%' : '$' ). 'INTELFPGAOCLSDKROOT' . (is_windows() ? '%' : '' );
}
sub sdk_dev_bin_dir_shellname() {
   return
      sdk_root_shellname()
      . pathsep()
      . (is_windows() ? 'windows64' : 'linux64' )
      . pathsep()
      . 'bin';
}
sub sdk_dev_bin_dir() {
   return
      sdk_root()
      . pathsep()
      . (is_windows() ? 'windows64' : 'linux64' )
      . pathsep()
      . 'bin';
}
sub sdk_bin_dir() {
   return
      sdk_root()
      . pathsep()
      . 'bin';
}
sub sdk_host_bin_dir() { return join(pathsep(), sdk_root(), 'host', get_arch(), 'bin'); }
sub is_sdk() {  return -e sdk_dev_bin_dir(); }
sub sdk_aocl_exe() { return sdk_bin_dir().pathsep().'aocl'; }
sub sdk_aoc_exe() { return sdk_dev_bin_dir().pathsep().'aoc'; }
sub sdk_pkg_editor_exe() { return sdk_host_bin_dir().pathsep().'aocl-binedit'; }
sub sdk_libedit_exe() { return sdk_host_bin_dir().pathsep().'aocl-libedit'; }
sub sdk_boardspec_exe() { return sdk_host_bin_dir().pathsep().'aocl-boardspec'; }
sub sdk_hash_exe() { return sdk_host_bin_dir().pathsep().'aocl-hash'; }
sub sdk_extract_aocx_exe() { return sdk_host_bin_dir().pathsep().'aocl-extract-aocx'; }
sub sdk_version() { return '20.3.0.168.5'; }

sub create_opencl_ipx($) {
  my $work_dir = shift;
  my $opencl_ipx = "$work_dir/opencl.ipx";
  open(my $fh, '>', $opencl_ipx) or die("Cannot open file $opencl_ipx $!");
  print $fh '<?xml version="1.0" encoding="UTF-8"?>
<library>
 <path path="${INTELFPGAOCLSDKROOT}/ip/*" />
</library>
';
  close $fh;
}

sub _get_host_compiler_type {
   # Returns 'msvc' or 'gnu' depending on argument selection.
   # Also return any remaining arguments after consuming an override switch.
   my @args = @_;
   my @return_args = ();
   my $is_msvc = ($^O =~ m/MSWin/i);
   my %msvc_arg = map { ($_,1) } qw( --msvc --windows );
   my %gnu_arg = map { ($_,1) } qw( --gnu --gcc --linux --arm );
   foreach my $arg ( @args ) {
      if ( $msvc_arg{$arg} ) { $is_msvc = 1; }
      elsif ( $gnu_arg{$arg} ) { $is_msvc = 0; }
      else { push @return_args, $arg; }
   }
   return ( $is_msvc ? 'msvc' : 'gnu' ), @return_args;
}

sub host_ldlibs(@) {
   # Return a string with the libraries to use,
   # followed by remaining arguments after consuming an override switch.
   my ($host_compiler_type,@args) = _get_host_compiler_type( @_ );
   acl::Board_env::override_platform($host_compiler_type);
   my $board_libs = acl::Board_env::get_xml_platform_tag("linklibs");
   my $result = '';
   my @return_args = ();
   foreach my $arg (@args) {
      push @return_args, $arg;
   }
   if ( $host_compiler_type eq 'msvc' ) {
      $result = "$board_libs alteracl.lib acl_emulator_kernel_rt.lib pkg_editor.lib libelf.lib acl_hostxml.lib";
   } else {
      my $host_arch = get_arch(@_);
      $result .= "-Wl,--no-as-needed -lalteracl";

      if (length $board_libs) {
         $result .= " $board_libs";
      }

      # We distribute libelf ourselves, which means it's not in a standard
      # search path and will be in a directory specified by one of the -L
      # options. Unfortunately, -L options are only used to find libraries
      # specifically given with -l options, and are NOT used when looking up
      # a library specified as a dependency (through a DT_NEEDED entry) of a
      # library specified with -l. Therefore until we start statically linking
      # to libelf, this option will have to be here (see case 222459).
      # UPDATE: Linux64 doesn't need this, since it will be picked up in runtime
      # from LD_LIBRARY_PATH. However, arm (c5soc) still fails without it. Probably
      # for similar reason that it needs lstdc++.
      $result .= " -lelf";

      # When using a cross compiler, the DT_NEEDED entry in libalteracl.so that
      # indicates that it depends on libstdc++.so does not seem to be enough
      # for the linker to actually find libstdc++.so. It does however work if
      # -lstdc++ is specified manually. It appears the reason is that the
      # list of search paths for -l options is different than for DT_NEEDED
      # entries. Specifically it seems like libraries from DT_NEEDED entries
      # are only searched for in the cross-compiler "sysroot", which seems to
      # not be where libstdc++ is. For now, just assume if the target
      # architecture is not linux64, than we are probably using a cross-compiler
      # (probably ARM).
      if ($host_arch ne 'linux64') {
         $result .= " -lstdc++";
      }
   }
   return $result,@return_args;
}


sub host_ldflags(@) {
   # Return a string with the linker flags to use (but not the list of libraries),
   # followed by remaining arguments after consuming an override switch.
   my ($host_compiler_type,@args) = _get_host_compiler_type( @_ );
   acl::Board_env::override_platform($host_compiler_type);
   my $board_flags = acl::Board_env::get_xml_platform_tag("linkflags");
   my $result = '';
   if ( $host_compiler_type eq 'msvc' ) {
      $result = "/libpath:".sdk_root()."/host/windows64/lib";
   } else {
      my $host_arch = get_arch(@_);
      $result = "$result-L".sdk_root()."/host/$host_arch/lib";
   }
   if ( ($board_flags ne '') && ($board_flags ne $result) ) { $result = $board_flags." ".$result; }
   return $result,@args;
}


sub host_link_config(@) {
   # Return a string with the link configuration, followed by remaining arguments.
   my ($host_compiler_type,@args) = _get_host_compiler_type( @_ );
   my ($ldflags) = host_ldflags(@_);
   my ($ldlibs, @return_args) = host_ldlibs(@_);
   return "$ldflags $ldlibs", @return_args;
}


sub set_board_override(@) {
   # Set the override board path
   acl::Board_env::set_board_path(@_);
}

sub board_path(@) {
   # Return a string with the board path,
   # followed by remaining arguments after consuming an override switch.
   return acl::Board_env::get_board_path(),@_;
}

sub aocl_boardspec(@) {
  my ($boardspec, $cmd ) = @_;
  $boardspec = acl::File::abs_path("$boardspec" . "/board_spec.xml") if ( -d $boardspec );
  if ( ! -f $boardspec ) {
    return "Error: Can't find board_spec.xml ($boardspec)";
  }
  my ($exe) = (sdk_boardspec_exe());
  my $result = acl::Common::mybacktick("$exe \"$boardspec\" $cmd");
  if( $result =~ /error/ ) {
    die( "Error: Parsing $boardspec ran into the following error:\n$result\n" );
  }
  return $result;
}

# Return a hash of name -> path
sub board_hw_list(@) {
  my (@args) = @_;
  my %boards = ();

  # We want to find $acl_board_path/*/*.xml, however acl::File::simple_glob
  # cannot handle the two-levels of wildcards. Do one at a time.
  my $acl_board_path = acl::Board_env::get_board_path();
  $acl_board_path .= "/";
  $acl_board_path .= acl::Board_env::get_hardware_dir();
  $acl_board_path = acl::File::abs_path($acl_board_path);
  my @board_dirs = acl::File::simple_glob($acl_board_path . "/*");
  foreach my $dir (@board_dirs) {
    my @board_spec = acl::File::simple_glob($dir . "/board_spec.xml");
    if(scalar(@board_spec) != 0) {
      my ($board) = aocl_boardspec($board_spec[0], "name");
      if ( defined $boards{ $board } ) {
        print "Error: Multiple boards named $board found at \n";
        print "Error:   $dir\n";
        print "Error:   $boards{ $board }\n";
        return (undef,@args);
      }
      $boards{ $board } = $dir; # E.g.: pac_s10 -> .../board/intel_s10sx_pac/hardware/pac_s10
    }
  }
  return (%boards,@args);
}

sub board_hw_path(@) {
   # Return a string with the path to a specified board variant,
   # followed by remaining arguments after consuming an override switch.
   my $variant = shift;
   my (%boards,@args) = board_hw_list(@_);
   if ( defined $boards{ $variant } ) {
     return ($boards{ $variant },@args);
   } else {
     # Maintain old behaviour - even if board doesn't exist, here is where
     # it would be
     my ($board_path,@args) = board_path(@args);
     my ($hwdir) = acl::Board_env::get_hardware_dir();
     return "$board_path/$hwdir/$variant",@args;
   }
}


sub board_mmdlib(@) {
   my ($host_compiler_type,@args) = _get_host_compiler_type( @_ );
   acl::Board_env::override_platform($host_compiler_type);
   my $board_libs = acl::Board_env::get_xml_platform_tag("mmdlib");
   return $board_libs,@args;
}

sub board_libs(@) {
   # Return a string with the libraries to compile a host program for the current board,
   # followed by remaining arguments after consuming an override switch.
   my ($host_compiler_type,@args) = _get_host_compiler_type( @_ );
   acl::Board_env::override_platform($host_compiler_type);
   my $board_libs = acl::Board_env::get_xml_platform_tag("linklibs");
   return $board_libs,@args;
}


sub board_link_flags(@) {
   # Return a string with the link flags to compile a host program for the current board,
   # followed by remaining arguments after consuming an override switch.
   my ($host_compiler_type,@args) = _get_host_compiler_type( @_ );
   acl::Board_env::override_platform($host_compiler_type);
   my $link_flags = acl::Board_env::get_xml_platform_tag("linkflags");
   return $link_flags,@args;
}


sub board_version(@) {
   # Return a string with the board version,
   # followed by remaining arguments after consuming an override switch.
   my @args = @_;
   my $board = acl::Board_env::get_board_version();
   return $board,@args;
}

sub board_name(@) {
   # Return a string with the board version,
   # followed by remaining arguments after consuming an override switch.
   my @args = @_;
   my $board = acl::Board_env::get_board_name();
   return $board,@args;
}

sub board_hardware_default(@) {
   # Return a string with the default board,
   # followed by remaining arguments after consuming an override switch.
   my @args = @_;
   my $board = acl::Board_env::get_hardware_default();
   return $board,@args;
}

sub board_mmdlib_if_exists() {
  # Return a string contains the mmdlibs. If no mmd found, return undef
  return acl::Board_env::get_mmdlib_if_exists();
}

sub board_post_qsys_script(@) {
  # Return a string with script to run after qsys. Sometimes needed to
  # fixup qsys output.
  return acl::Board_env::get_post_qsys_script();
}

# Extract Quartus version string from output of "quartus_sh --version"
# Returns {major => #, minor => #, update => #}.
#  E.g. "19.3.0" -> (major => 19, minor => 3, update => 0)
#  E.g. "19.3eap.0" -> (major => 19, minor => 3, update => 0)
#
# Returns undef if there is no version.
sub get_quartus_version($) {
   my $str = shift;
   if ( $str =~ /^(\d+)\.(\d+)([^\.]*)\.(\d+)$/ ) {
      # $3 is called variant, which is a special string for specially marked releases
      #  e.g. "19.3eap.0" -> variant = 'eap'
      # Variant has no effect on version check.
      #  e.g. acl/19.3eap.0 should work with acds/19.3.0 and vice-versa
      return {
        major => $1,
        minor => $2,
        update => $4,
      };
   }
   return undef;
}

# Examples of compatibility versions:
# For OpenCL 19.3 and 19.4:
# OpenCL    Quartus
# 19.3.0    18.1.0-19.3.0, plus 17.1.1 (for DCP)
# 19.4.0    18.1.0-19.4.0, plus 17.1.1 (for DCP)
#
# For OpenCL from 20.1:
# OpenCL    Quartus
# 20.1.0    19.1.0-20.1.0
# 20.2.0    19.2.0-20.2.0
# For OpenCL 20.3:
# OpenCL    Quartus
# 20.3.0    19.3.0-20.3.0, plus 17.1.1 and 19.2 (for DCP)

# 2 releases back (not counting the special case of Quartus 17.1.1)
sub get_min_compatible_quartus_ver($) {
    my $opencl_ver = shift;
    my $min_compatible_qver = get_quartus_version("0.0.0");

    if ($opencl_ver->{major} != 19) {
        $min_compatible_qver->{major} = $opencl_ver->{major} - 1;
        $min_compatible_qver->{minor} = $opencl_ver->{minor};
        $min_compatible_qver->{update} = $opencl_ver->{update};
    }
    elsif ($opencl_ver->{minor} <= 2) { # 19.1, 19.2
        $min_compatible_qver = get_quartus_version("18.0.0");
    }
    else {  # 19.3, 19.4
        $min_compatible_qver = get_quartus_version("18.1.0");
    }
    return "$min_compatible_qver->{major}.$min_compatible_qver->{minor}.$min_compatible_qver->{update}";
}

# Checks if $quartus_ver is compatible with $opencl_ver. Returns 1 if compatible, 0 otherwise.
#
# Conditions for compatibility:
#   0. Special cases:
#     0.1. If $ENV{ACL_ACDS_VERSION_OVERRIDE} exists, accept if $quartus_ver == $ACL_ACDS_VERSION_OVERRIDE
#     0.2. For $opencl_ver = 19.x or 20.x, accept $quartus_ver = 17.1.1
#     0.3  For $opencl_ver = 20.3, accept $quartus_ver = 17.1.1 and 19.2
#   1. $quartus_ver <= $opencl_ver
#   2. $quartus_ver >= $min_compatible_qver

sub are_quartus_versions_compatible($$) {
  my ($opencl_ver, $quartus_ver) = @_;
  return 0 if(!defined($opencl_ver) || !defined($quartus_ver));

  # Condition 0.1. Accept if $quartus_ver == $ACL_ACDS_VERSION_OVERRIDE
  if (exists($ENV{ACL_ACDS_VERSION_OVERRIDE})) {
    my $acl_acds_version_override = get_quartus_version($ENV{ACL_ACDS_VERSION_OVERRIDE});
    return 1 if(
       $quartus_ver->{major} == $acl_acds_version_override->{major} &&
       $quartus_ver->{minor} == $acl_acds_version_override->{minor} &&
       $quartus_ver->{update} == $acl_acds_version_override->{update}
    );
  }

  # Condition 0.2: For $opencl_ver = 19.x or 20.x, also accept $quartus_ver = 17.1.1
  return 1 if(
    ($opencl_ver->{major}  == 19 || $opencl_ver->{major}  == 20) &&
    ($quartus_ver->{major} == 17 && $quartus_ver->{minor} == 1 && $quartus_ver->{update} == 1)
  );

  # Condition 0.3: For $opencl_ver = 20.3+ and 21.*, accept $quartus_ver = 19.2.0 because of oneAPI Darby Creek
  return 1 if(
    ( ($opencl_ver->{major}  == 20 && $opencl_ver->{minor}  >= 3) || $opencl_ver->{major}  == 21) &&
    ($quartus_ver->{major} == 19 && $quartus_ver->{minor} == 2 && $quartus_ver->{update} == 0)
  );

  # Condition 1: Reject because $quartus_ver > $opencl_ver
  return 0 if(
    ($quartus_ver->{major} >  $opencl_ver->{major}) ||
    ($quartus_ver->{major} == $opencl_ver->{major} && $quartus_ver->{minor} >  $opencl_ver->{minor}) ||
    ($quartus_ver->{major} == $opencl_ver->{major} && $quartus_ver->{minor} == $opencl_ver->{minor} && $quartus_ver->{update} > $opencl_ver->{update})
  );

  #Condition 2: Reject because $quartus_ver < $min_compatible_qver
  my $min_qver = get_quartus_version(get_min_compatible_quartus_ver($opencl_ver));
  return 0 if(
    ($quartus_ver->{major} <  $min_qver->{major}) ||
    ($quartus_ver->{major} == $min_qver->{major} && $quartus_ver->{minor} <  $min_qver->{minor}) ||
    ($quartus_ver->{major} == $min_qver->{major} && $quartus_ver->{minor} == $min_qver->{minor} && $quartus_ver->{update} < $min_qver->{update})
  );

  return 1;
}

# Checks if the bsp version in the board_xml passed in is compatible with the aoc version.
# It is compatible if it is not newer and at oldest two releases older
# (eg. if the aoc version is 20.1, bsp versions 20.1, 19.3, and 19.1 are accepted)
# Versions 18.1 and 17.1 are exceptions needed to for DCP 1.2(a)
sub is_bsp_version_compatible($) {
  my $bsp_version = shift;
  my $skd_version = sdk_version();
  if ( $skd_version =~ /^(\d+)\.(\d+)([^\.]*)\.(\d+)\.(\d+)$/ ) {
    my $aoc_version = ($1 . $2)/10;
    # if the bsp version is higher somehow or if it is more than two releases old, error out
    if (($aoc_version < $bsp_version) or ($aoc_version > $bsp_version + 1)) {
      # this exception is needed to use the a10pac and s10pac board
      if ($bsp_version == 18.1 || $bsp_version == 17.1 || $bsp_version == 19.2) { return 1; }
      return 0;
    }
    return 1;
  }
  return 1; # something is wrong with the sdk version - can't do check, don't fail
}

1;
