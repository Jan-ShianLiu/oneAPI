=pod

=head1 NAME

acl::Board_env - Utility to determine board parameters.

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

package acl::Board_env;
require Exporter;
use strict;
use File::Spec;
require acl::Env;

my $platform_override = undef;
sub override_platform($) {
   my $value = shift;
   if ( $value eq 'msvc' ) { $platform_override = 'windows64'; }
   else { $platform_override = acl::Env::get_arch(); }
}

our $board_path_override = undef;

sub set_board_path {
  $board_path_override = shift;
  $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $board_path_override;
}

# Check the following env variables/locations/folders:
#  AOCL_BOARD_PACKAGE_ROOT (not set in oneAPI Beta06+)
#  $board_path_override
#  INTEL_FPGA_ROOT/intel_a10gx_pac (only set by oneAPI FPGA add-on in Beta06+)
#  sdk_root/test/board/a10_ref
#  sdk_root/board/intel_a10gx_pac
# or sdk_root/board/s5_ref for std
sub get_board_path {
   my $acl_board_path = $ENV{'AOCL_BOARD_PACKAGE_ROOT'};
   my $intel_fpga_root = $ENV{'INTEL_FPGA_ROOT'}; # Set by FPGA add-on in oneAPI
   my $acl_root = acl::Env::sdk_root();

   if ( defined $acl_board_path and $acl_board_path ne "" ){
     # Use AOCL_BOARD_PACKAGE_ROOT
     # Allow %a in board root which gets expanded to acl_root
     $acl_board_path =~ s/%a/$acl_root/g;
   }elsif ( defined $board_path_override and $board_path_override ne ""){
     $acl_board_path = $board_path_override;
   }elsif ( defined $intel_fpga_root and $intel_fpga_root ne "" ){
     # Use FPGA add-on pac_a10 files
     my $pac_a10_path = File::Spec->catfile($intel_fpga_root, "board", "intel_a10gx_pac");
     if ( -e $pac_a10_path and -d $pac_a10_path ){
       $acl_board_path = $pac_a10_path;
     }
   }

   # If $acl_board_path still has not been set, means neither AOCL_BOARD_PACKAGE_ROOT
   #  nor INTEL_FPGA_ROOT has been set, or INTEL_FPGA_ROOT has no pac_a10.
   # Try to use sdk_root as the last resort.
   if ( ! defined $acl_board_path or $acl_board_path eq "" ) {
     # Use sdk_root
     my $q_pro = '1';

     if ($q_pro == 1) {
       # a10_ref only exists in testing environment
       $acl_board_path =  File::Spec->catfile($acl_root, "test", "board", "a10_ref");
       if (!-e $acl_board_path or !-d $acl_board_path){
         # Try pac_a10 in case we are in SYCL
         $acl_board_path = File::Spec->catfile($acl_root, "board", "intel_a10gx_pac");
         if (!-e $acl_board_path or !-d $acl_board_path){
           # Even pac_a10 does not exist. Set to empty
           $acl_board_path = "";
         }
       }
     } else {
       $acl_board_path =  File::Spec->catfile($acl_root, "board", "s5_ref");
     }
   }
   if (acl::Env::is_windows()) {
     # Replace forward slashes with back slashes:
     $acl_board_path =~ s/\//\\/g;
   }
   return $acl_board_path;
}

sub expand_board_env_field {
   my $parsed = shift;
   my $acl_root = acl::Env::sdk_root();
   $parsed =~ s/%a/$acl_root/g;
   my $acl_board_path = get_board_path();
   $parsed =~ s/%b/$acl_board_path/g;
   my $qroot = $ENV{'QUARTUS_ROOTDIR'};
   $parsed =~ s/%q/$qroot/g;
   return $parsed;
}

sub get_hardware_dir {
   my $result = get_xml_board_attrib("hardware","dir");
   return $result;
}

sub get_board_name {
   my $result = get_xml_board_attrib("board_env","name");
   return $result;
}

sub get_board_version {
   my $result = get_xml_board_attrib("board_env","version");
   ($result =~ /^-?\d+\.\d+$/ and $result >= 13.0 and $result <= '20.3') 
     or (print STDERR "Unsupported board_env version number: $result\n  Version must be between 13.0 and 20.3\n" and exit (1));
   return $result;
}

sub get_hardware_default {
   my $result = get_xml_board_attrib("hardware","default");
   return $result;
}

sub get_quartus_version {
  my $result = get_xml_board_attrib("quartus", "version");
  return $result;
}

sub get_quartus_patches {
  my $result = get_xml_board_attrib("quartus", "patches");
  return $result;
}

sub get_mmdlib_if_exists {
  return get_xml_platform_tag_if_exists("mmdlib");
}

sub get_util_bin {
  return get_xml_platform_tag("utilbindir");
}

sub get_post_qsys_script {
  get_xml_platform_tag_if_exists ("postqsys_script");
}

sub get_xml_board_attrib {
  my ($tag,$attrib) = @_;
  my $result = get_board_section($tag);
  $result =~ /<$tag(.*?)>(.*?)<\/$tag>/;
  $result = $1;

  if ($result =~ /$attrib\s*?=\s*?\"(.*?)\"/){
    $result = $1;
  }else{
    $result = undef;
  }

  # tag <quartus> is optional. Don't error out if <quartus> is not found
  return undef if $tag eq 'quartus' and !defined $result;
  defined $result or print STDERR "Failed to find xml tag $tag\n" and exit (1);
  return expand_board_env_field($result);
}


sub get_xml_platform_tag_if_exists {
  my $tag = shift;
  my $result = get_board_section("platform", get_platform_str());
  $result =~ /<$tag.*?>(.*?)<\/$tag>/;
  $result = $1;
  if (defined $result) {
    return expand_board_env_field($result);
  } else {
    return undef;
  }
}


sub get_xml_platform_tag {
  my $tag = shift;
  my $result = get_xml_platform_tag_if_exists ($tag);
  defined $result or print STDERR "Failed to find xml tag $tag\n" and exit (1);
  return expand_board_env_field($result);
}


sub get_platform_str {
  if ( defined $platform_override ) { return $platform_override; }
  my $platform;
  my $is_msvc = ($^O =~ m/MSWin/i);
  if ( $is_msvc) {
    $platform = "windows64";
  } else {
    $platform = acl::Env::get_arch();
  }
  return $platform;
}

sub get_board_section {
  my ($section, $field) = @_;
  my $section_str="";
  my $start=0;

  my $acl_board_path = get_board_path();
  if (! defined $acl_board_path or $acl_board_path eq "") {
    print STDERR "No board support packages installed.\n";
    print STDERR "Use aoc -board-package option to specify a board package for compilation. Use -list-boards option to see available boards in that board package.\n";
    exit 1;
  } else {
    open(F, "<$acl_board_path/board_env.xml") or print STDERR "Cannot find board_env.xml in $acl_board_path\n" and exit 1;
    while(<F>)
    {
      # Good practice to store $_ value because
      # subsequent operations may change it.
      my($line) = $_;

      # Good practice to always strip the trailing
      # newline from the line.
      chomp($line);

      # Convert the line to upper case.
      #$line =~ tr/[A-Z]/[a-z]/;
      if ( $line =~ m/<$section\s+.*$field/i  or ! defined $field and $line =~ m/<$section>/i)
      {
        $start = 1;
      }

      if ($start)
      {
        $section_str = $section_str . $line;
      }

      if ( $line =~ m/\/$section/i)
      {
        $start = 0;
      }

    }
  }

  return $section_str;
}

#sub get_board_env_xml {
#my $xml = new XML::Simple (KeyAttr=>[], suppressempty => '');
#my $acl_board_path = get_board_path();
#
## read XML file
#-f "$acl_board_path/board_env.xml" or print STDERR "Failed to parse board_env.xml\n" and exit 1;
#my $data = undef; 
#eval { $data = $xml->XMLin("$acl_board_path/board_env.xml") } or print "Failed to read board_env.xml\n" and exit 1;
#return $data
#}

#sub get_xml_platform_tag_usingxmlparse {
#my $tag = shift;
#my $platform;
#my $acl_board_path = get_board_path();
#my $is_msvc = ($^O =~ m/MSWin/i);
#if ( $is_msvc) {
#$platform = "windows64";
#} else {
#$platform = "linux64";
#}
#
#my $xmldata = get_board_env_xml();
##print Dumper($xmldata);
#my $result = undef;
#foreach my $p (@{$xmldata->{platform}}) {
#if ($p->{name} eq $platform ) {
#if ( defined $p->{$tag} ) { 
#$result = $p->{$tag};
#}
#}
#}
#defined $result or print STDERR "Failed to find xml tag $tag\n" and exit (1);
#return expand_board_env_field($result);
#}

1;
