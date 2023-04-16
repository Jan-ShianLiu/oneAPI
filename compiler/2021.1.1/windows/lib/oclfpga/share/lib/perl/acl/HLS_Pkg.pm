
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

package acl::HLS_Pkg;
require Exporter;
@ISA        = qw(Exporter);
@EXPORT     = ();
@EXPORT_OK  = qw(
    create_empty_objectfile
    add_section_to_object_file
    add_projectname_to_object_file
    get_section_from_object_file
);

use strict;

#my $module = 'acl::HLS_Pkg';
#my $temp_count = 0;
#sub log_string(@) { }  # Dummy

=head1 NAME

acl::HLS_PKG - Native objectfile and library files manipulation tools

=head1 VERSION

$Header: //depot/sw/hld/rel/20.3api.11/acl/sysgen/lib/acl/HLS_Pkg.pm#1 $

=head1 SYNOPSIS


=head1 DESCRIPTION

This module provides general file handling utilities for creation, manipulation and extraction of data from native object files.

The module supports Elf64 on Linux and COFF on Windows


=head2 

=cut

# checks host OS, returns true for linux, false for windows.
sub isLinuxOS {
    if ($^O eq 'linux') {
      return 1; 
    }
    return;
}

sub mysystem($@) {
    # Default values for some parameters
    my $opts = shift(@_);
    
    my @cmd = @_;

    my $title = $opts->{'title'};
    my $verbose = $opts->{'verbose'};

    # Log the command to console if requested
    print STDOUT "============ ${title} ============\n" if $title && $verbose>1;
    print STDOUT "@cmd\n" if $verbose>1;

    # Run the command.
    my $retcode = system(@cmd);
 
    my $result = $retcode >> 8;

    if($retcode != 0) {
      if ($result == 0) {
        # We probably died on an assert, make sure we do not return zero
        $result = -1;
      }
    }

    return ($result);
}

sub create_empty_objectfile($$$) {
    my ($opts, $object_file, $dummy_file) = @_;
    my $verbose = $opts->{'verbose'};
    my @cmd_list = undef;
    if (isLinuxOS()) {
      # Create empty file by copying non-existing section from arbitrary 
      # non-empty file
      @cmd_list = ( 'objcopy',
                    '--binary-architecture=i386:x86-64',
                    '--only-section=.text',
                    '--input-target=binary',
                    '--output-target=elf64-x86-64',
                    $dummy_file,
                    $object_file
          );
    } else {
      @cmd_list = ('coffcopy.exe',
                   '--create-object-file',
                   $object_file
          );
    }
    my $return_status = mysystem({'title' => 'create object file',
                                  'verbose' => $verbose },
                                  @cmd_list);
    if ($return_status != 0) {
      die "Not able to create $object_file";
    }
#    push @object_list, $object_file;
}

sub add_section_to_object_file ($$$$) {
    my ($opts, $object_file, $scn_name, $content_file) = @_;
    my $verbose = $opts->{'verbose'};

    my @cmd_list = undef;
    unless (-e $object_file) {
      create_empty_objectfile({'verbose' => $verbose}, $object_file, $content_file);
    }
    if (isLinuxOS()) {
      @cmd_list = ('objcopy',
                   '--add-section',
                   $scn_name.'='.$content_file,
                   $object_file
          );
    } else {
      @cmd_list = ('coffcopy.exe',
                   '--add-section',
                   $scn_name,
                   $content_file,
                   $object_file
          );
    }
    my $return_status = mysystem({'title' => 'Add section to object file',
                                  'verbose' => $verbose },
                                  @cmd_list);
    if ($return_status != 0) {
      die "Not able to update $object_file";
    }
    if (isLinuxOS()) {
      @cmd_list = ('objcopy',
                   '--set-section-flags',
                   $scn_name.'=alloc',
                   $object_file
          );
      $return_status = mysystem({'title' => 'Change flags to object file',
                                 'verbose' => $verbose },
                                 @cmd_list);
      if ($return_status != 0) {
        die "Not able to update $object_file";
      }
    }
    return 1;
}

sub add_projectname_to_object_file ($$$$) {
    my ($opts, $object_file, $scn_name, $content_file) = @_;
    my $verbose = $opts->{'verbose'};

    unless (-e $object_file) {
      create_empty_objectfile({'verbose' => $verbose},$object_file, $content_file);
    }
    
    my @cmd_list = undef;
    if (isLinuxOS()) {
      @cmd_list = ('objcopy',
                   '--add-section',
                   $scn_name.'='.$content_file,
                   $object_file
          );
    } else {
      @cmd_list = ('coffcopy.exe',
                   '--add-section',
                   $scn_name,
                   $content_file,
                   $object_file );
    }
    my $return_status = mysystem({'title' => 'Add project dir name to object file',
                                  'verbose' => $verbose },
                                  @cmd_list);
    if ($return_status != 0) {
      die "Not able to update $object_file";
    }
    if (isLinuxOS()){
      @cmd_list = ('objcopy',
                   '--set-section-flags',
                   $scn_name.'=alloc',
                   $object_file);
      $return_status = mysystem({'title' => 'Change flags to object file',
				                 'verbose' => $verbose },
                                 @cmd_list);
      if ($return_status != 0) {
        die "Not able to update $object_file";
      }
    }
}

sub get_section_from_object_file ($$$$) {
    my ($opts, $object_file, $scn_name ,$dst_file) = @_;
    my $verbose = $opts->{'verbose'};
    my @cmd_list = undef;
    if (isLinuxOS()) {
      @cmd_list = ('objcopy',
                   ,'-O', 'binary', 
                   '--only-section='.$scn_name,
                   $object_file,
                   $dst_file
          );
    } else {
      @cmd_list = ( 'coffcopy.exe',
                    '--get-section',
                    $scn_name,
                    $object_file,
                    $dst_file
          );
    }
    my $return_status = mysystem({'title' => 'Get IR from object file',
                                  'verbose' => $verbose },
                                  @cmd_list);
    if ($return_status != 0) {
      die "Not able to extract from $object_file";
    }
    return (! -z $dst_file);
}

sub get_RTL_sections_from_object_file ($$$) {
    my ($opts, $object_file, $dest_dir) = @_;
    my $verbose = $opts->{'verbose'};
    my @cmd_list = undef;

    my $start_dir = $ENV{PWD};

    if (isLinuxOS()) {
      my $contents= `readelf -SW $object_file`;
      foreach (split(/\n/,$contents)) {
        $_ =~ /\.(source|xml_spec)\.(.+\.[a-z]+)\s+PROGBITS.*/ || next;
        my $filename = $2;
        if ($1 == 'source') {
          my $path = acl::File::mydirname($dest_dir.$filename);
          my $res=acl::File::make_path_to_file($dest_dir.$filename);
        } elsif ($1 == 'xml_spec') {
        } else {
          die "don't know how to deal with :$_\n";
        }
        get_section_from_object_file ($opts, $object_file, ".$1.$2", "$dest_dir$filename");
      }
    } else {
	  my $indexfile = $dest_dir.'xmlidx.txt';
      get_section_from_object_file ($opts, $object_file, ".xmlidx", $indexfile);
	  open(my $mapfile, "<", $indexfile) or die "Couldn't open $indexfile!";
	  while (my $row = <$mapfile>) {
        $row =~ /\.(source|xml_spec)\.(.+\.[a-z]+)\s+=>\s(.*)/ || next;
        my $filename = $dest_dir.$2;
        if ($1 == 'source') {
          my $path = acl::File::mydirname($filename);
          my $res=acl::File::make_path_to_file($filename);
        } elsif ($1 == 'xml_spec') {
        } else {
          die "don't know how to deal with :$row\n";
        }
        get_section_from_object_file ($opts, $object_file, $3, $filename);
      }
    }
}
