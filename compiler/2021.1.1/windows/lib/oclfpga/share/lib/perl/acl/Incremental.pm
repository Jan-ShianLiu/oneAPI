=pod

=head1 NAME

acl::Incremental - Utility for incremental compile flows

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

package acl::Incremental;
require Exporter;
use strict;
require acl::Common;
require acl::File;
require acl::Report;

$acl::Incremental::warning = undef;

my $warning_prefix = "Compiler Warning: Incremental Compilation:";
my $warning_suffix = "Performing full recompilation.\n";

# Check if full incremental recompile is necessary
sub requires_full_recompile($$$$$$$$$$$$) {
  $acl::Incremental::warning = undef;

  my ($input_dir, $work_dir, $base, $all_aoc_args, $board_name, $board_variant, $devicemodel,
      $devicefamily, $quartus_version, $program, $aclversion, $bnum) = @_;
  my $acl_version       = "$aclversion Build $bnum";
  my $qdb_dir = $input_dir eq "$work_dir/prev" ? "$work_dir/qdb" : "$input_dir/qdb";

  local $/ = undef;

  my $info_obj = acl::Report_reader::parse_info("$input_dir/reports/lib/json/info.json");
  if (! -e "$input_dir/kernel_hdl") {
    $acl::Incremental::warning = "$warning_prefix change detection could not be completed because kernel_hdl ".
                                 "directory is missing in $input_dir. $warning_suffix";
    return 1;
  } elsif (! -e "$input_dir/$base.bc.xml") {
    $acl::Incremental::warning = "$warning_prefix change detection could not be completed because $base.bc.xml is ".
                                 "missing in $input_dir. $warning_suffix";
    return 1;
  } elsif ($info_obj) {
    # Check project name
    my $prev_proj_name = acl::Report_reader::get_name($info_obj);
    if ($prev_proj_name ne $base) {
      $acl::Incremental::warning = "$warning_prefix previous project name: $prev_proj_name differs ".
                                   "from current project name: ".acl::Report::escape_string($base).". $warning_suffix";
      return 1;
    }

    # Check target family, device, and board
    my ($prev_target_family, $prev_device, $prev_board_name) = acl::Report_reader::get_family_device($info_obj) =~ /(.+),\s+(.+),\s+(.+)/;
    if ($prev_target_family ne $devicefamily ||
        $prev_device ne $devicemodel ||
        $prev_board_name ne acl::Report::escape_string("$board_name:$board_variant")) {
      $acl::Incremental::warning = "$warning_prefix previous device information: $prev_target_family, $prev_device, $prev_board_name ".
                                   "differs from current device information: $devicefamily, $devicemodel, ".
                                   acl::Report::escape_string("$board_name:$board_variant").". $warning_suffix";
      return 1;
    }

    # Check ACDS version
    my $prev_ACDS_version = acl::Report_reader::get_quartus_version($info_obj);
    if ($prev_ACDS_version ne $quartus_version) {
      $acl::Incremental::warning = "$warning_prefix previous Quartus version: $prev_ACDS_version ".
                                   "differs from current Quartus version: $quartus_version. ".
                                   "$warning_suffix";
      return 1;
    }

    # Check AOC version
    my $prev_AOC_version = acl::Report_reader::get_version($info_obj);
    if ($prev_AOC_version ne $acl_version) {
      $acl::Incremental::warning = "$warning_prefix previous AOC version: $prev_AOC_version differs from current ".
                                   "AOC version: $acl_version. $warning_suffix";
      return 1;
    }

    # Check command line flags
    $program =~ s/#//g;
    my ($prev_command) = acl::Report_reader::get_command($info_obj) =~ /$program\s+(.*)/;
    my $prev_compile_rtl_only = index($prev_command, '-rtl') != -1;

    # Check if user deleted the qdb folder.
    if (!$prev_compile_rtl_only and ! -d $qdb_dir) {
      $acl::Incremental::warning = "$warning_prefix qdb directory missing in $work_dir. QDB files are required to perform ".
                                   "an incremental compilation. Files inside $work_dir should not be modified. $warning_suffix";
      return 1;
    } elsif (!$prev_compile_rtl_only and acl::File::is_empty_dir($qdb_dir)) {
      $acl::Incremental::warning = "$warning_prefix qdb directory in $work_dir is empty. QDB files are required to perform ".
                                   "an incremental compilation. Files inside $work_dir should not be modified. $warning_suffix";
      return 1;
    }

    my @prev_args = split(/\s+/, $prev_command);
    my @curr_args = split(/\s+/, acl::Report::escape_string($all_aoc_args));
    return 1 if (compare_command_line_flags(\@prev_args, \@curr_args));
  } else {
    $acl::Incremental::warning = "$warning_prefix change detection could not be completed because ".
                                 "$input_dir/reports/lib/json/info.json could not be opened. ".
                                 "$warning_suffix";
    return 1;
  }
  return 0;
}

# Check if important command line options/flags match.
# This check only needs to identify diffs in command line options/flags
# that won't be detected by other stages of change detection.
sub compare_command_line_flags($$) {
  my ($prev_args, $curr_args) = @_;

  my $index = 0;
  ++$index until $index == scalar @$prev_args || $prev_args->[$index] eq "-rtl";
  if ($index != scalar @$prev_args) {
    $index = 0;
    ++$index until $index == scalar @$curr_args || $curr_args->[$index] eq "-rtl";
    if ($index == scalar @$curr_args) {
      $acl::Incremental::warning = "$warning_prefix the previous compile was only run to the RTL stage. $warning_suffix";
      return 1;
    }
  }

  # incremental and incremental=aggressive are equivalent
  # when checking command line flags. The differences between the
  # two modes are handled in our full diff detection flow. Just need to check
  # that the previous compile ran one of the incremental modes.
  $index = 0;
  ++$index until $index == scalar @$prev_args || $prev_args->[$index] =~ /^(-)?-incremental(=aggressive)?$/;
  if ($index == scalar @$prev_args) {
    $acl::Incremental::warning = "$warning_prefix the previous compile was not an incremental compile. $warning_suffix";
    return 1;
  }

  my @ref = @$prev_args;
  my @cmp = @$curr_args;
  my $swapped = 0;
  my @libs = ();
  # Opt args are options with a mandatory argument.
  my @optargs_to_check = ('bsp-flow');
  while (scalar @ref) {
    my $arg  = shift @ref;

    # Check for matching library in both sets
    if ($arg =~ m!^-l(\S+)! || $arg eq '-l') {

      if ($arg =~ m!^-l(\S+)!) {
        # There are some aoc options that start with -l which are
        # detected as library filenames using the above regex
        # so need to skip checking those options.
        my $full_opt = '-l' . $1;
        foreach my $exclude_name (@acl::Common::l_opts_exclude) {
          if ($full_opt =~ m!^$exclude_name!) {
            goto END;
          }
        }
      }

      my $length = scalar @cmp;
      $index = 0;

      my $ref_lib = ($arg =~ m!^-l(\S+)!) ? $1 : shift @ref;
      my $cmp_lib = "";
      while ($index < $length) {
        ++$index until $index == $length || $cmp[$index] eq "-l" || $cmp[$index] =~ m!^-l(\S+)!;
        last if ($index == $length);
        $cmp_lib   = $cmp[$index+1]             if ($cmp[$index] eq "-l");
        ($cmp_lib) = $cmp[$index] =~ /^-l(\S+)/ if ($cmp[$index] =~ m/^-l\S+/);
        last if ($cmp_lib eq $ref_lib);
        ++$index;

      }

      if ($ref_lib ne $cmp_lib) {
        # Need to include the library name or else the warning will just say
        # '-l' flag is missing from one of the compiles which is not specific
        # enough.
        $arg .= " $ref_lib" if ($arg eq '-l');
        _add_differing_flag_warning($arg, $swapped);
        return 1;
      }

      push @libs, $ref_lib;
      splice(@cmp, $index, ($cmp[$index] eq '-l') ? 2 : 1);

    } else {
      # Check if important option/argument pairs match.
      foreach my $optarg (@optargs_to_check) {
        if ($arg eq "--$optarg" || $arg eq "-$optarg" || $arg =~ m!^-$optarg=(\S+)!) {
          my $full_arg = $arg;
          if ($arg eq "--$optarg" || $arg eq "-$optarg") {
            # Need to report the option name and the argument value
            # in the command line flag warning. $arg only contains
            # the option name.
            $full_arg .= " $ref[0]";
          }

          if (_compare_command_opt_arg($optarg, $arg, \@ref, \@cmp)) {
            _add_differing_flag_warning($full_arg, $swapped);
            return 1;
          }

          goto END;
        }
      }
    }

    # If we've checked all command line flags in @prev_args against @curr_args,
    # swap the arrays and check any left over command line flags in @curr_args
    END:
    if (! scalar @ref) {
      @ref = @cmp;
      @cmp = ();
      $swapped = 1;
    }
  }

  if (scalar @libs) {
    $acl::Incremental::warning .= "$warning_prefix the following libraries were used: " . join(', ', @libs) .
                                  ". Changes to libraries are not automatically detected in incremental compile.\n";
  }

  return 0;
}

# Get the previous project name
sub get_previous_project_name($) {
  $acl::Incremental::warning = undef;
  my ($json) = @_;
  # Get the contents of the hash table that contains Project Name
  # Get the value of the 'data' field, which will contain the project name in this case. The data field may appear before or after the Project Name.
  my $info_obj = acl::Report_reader::parse_info($json);
  my $prj_name = acl::Report_reader::get_name($info_obj);
  $acl::Incremental::warning = "$warning_prefix the previous project name in $json is empty. $warning_suffix" if ($prj_name eq "");
  return $prj_name;
}

# Compare a command line option with an argument between @$rref and @$rcmp.
# (eg. compares command line options of the form '--option-name arg' or
# any equivalent form)
# Return 0 if the option/argument pair in @$rref also exists in @$rcmp or $opt is on
# the @$whitelist. Then remove the matching option/argument pair from @$rcmp.
# Return 1 if the option/argument pair differs between @$rref and @$rcmp.
sub _compare_command_opt_arg($$$$;$) {
  my ($opt_name, $opt, $rref, $rcmp, $whitelist) = @_;

  my $cmp_length = scalar @$rcmp;
  my $cmp_index = 0;

  my $ref_arg = ($opt =~ m!^-$opt_name=(\S+)!) ? $1 : shift @$rref;
  my $cmp_arg = undef;

  # Some options can be specified multiple times on the command line so we need to
  # compare all instances to find one with the same value.
  while ($cmp_index < $cmp_length) {
    # There are currently 3 equivalent formats an option/argument pair can be specified.
    # 1) --option-name arg
    # 2) -option-name arg
    # 3) -option-name=arg
    ++$cmp_index until $cmp_index == $cmp_length ||
                       $rcmp->[$cmp_index] eq "--$opt_name" ||
                       $rcmp->[$cmp_index] eq "-$opt_name" ||
                       $rcmp->[$cmp_index] =~ m!^-$opt_name=(\S+)!;

    # Did not find the same option argument pair in @$rcmp.
    last if ($cmp_index == $cmp_length);

    $cmp_arg = $rcmp->[$cmp_index+1] if ($rcmp->[$cmp_index] eq "--$opt_name" ||
                                         $rcmp->[$cmp_index] eq "-$opt_name");
    ($cmp_arg) = $rcmp->[$cmp_index] =~ /^-$opt_name=(\S+)/ if ($rcmp->[$cmp_index] =~ m/^-$opt_name=\S+/);

    # Found matching option/argument pair in @$rcmp.
    last if ($cmp_arg eq $ref_arg);
    ++$cmp_index;
  }

  my $on_whitelist = 0;
  if (defined $whitelist) {
    my @ref_arg_split = split(/\s/, $ref_arg);
    foreach my $whitelist_opt (@$whitelist) {
      # Some ref_arg on the whitelist may take their own argument values.
      # eg. --sysinteg-arg "--const-cache-bytes 32000" would have
      # ref_arg -> "--const-cache-bytes 32000". Strip out the internal argument
      # value before checking the whitelist
      if ($ref_arg_split[0] =~ /^$whitelist_opt$/) {
        $on_whitelist = 1;
        last;
      }
    }
  }

  return 1 if ($ref_arg ne $cmp_arg && !$on_whitelist);

  # Remove the matching option/argument pair from @$rcmp.
  my $num_to_splice = $rcmp->[$cmp_index] eq "-$opt_name" || $rcmp->[$cmp_index] eq "--$opt_name" ? 2 : 1;
  splice(@$rcmp, $cmp_index, $num_to_splice);

  return 0;
}

# Add a warning specifying the flag that differs between the current and previous compile.
sub _add_differing_flag_warning($$) {
  my ($arg, $swapped) = @_;
  my $curr = $swapped ? "current" : "previous";
  my $prev = $swapped ? "previous" : "current";

  $acl::Incremental::warning .= "$warning_prefix the $curr compile uses the command line " .
                                "flag $arg which is missing in the $prev compile. $warning_suffix";
}

# Routines for handling Quartus/simulator compilation avoidance
#
# Return the contents of a file
sub read_file($) {
  my ($fname) = @_;
  my ($fh, $contents);
  my $verbose = acl::Common::get_verbose();
  if (!(open $fh, "<", $fname)) {
    print "Unable to get open $fname $!\n" if $verbose;
    close $fh;
    return undef;
  }
  if (-s $fh == 0) {
    $contents = "";
  } elsif (!(read $fh, $contents, -s $fh)) {
    print "Unable to get read $fname $!\n" if $verbose;
    close $fh;
    return undef;
  }
  close $fh;
  return $contents;
}

#
# Extract the section named 'section_name' in a package and return the contents.
# Note: temp_filename is used as a temporary file and will be removed before returning.
#
sub extract_section($$$) {
  my ($pkg, $section_name, $temp_filename) = @_;
  my $verbose = acl::Common::get_verbose();

  if (!$pkg->get_file($section_name,$temp_filename)) {
    print "Unable to get $section_name from file 1\n" if $verbose;
    unlink $temp_filename;
    return undef;
  }
  my $contents = read_file($temp_filename);
  unlink $temp_filename;
  return $contents;
}

# Ignore the random hash in the .acl.autodiscovery section
sub ignore_hash($) {
  my ($content) = @_;
  my $verbose = acl::Common::get_verbose();
  print "ignore_hash: before: $content\n" if $verbose > 1;
  my @all = split / /, $content;
  # Skip field 3 if we have a count of the fields, else skip field 2
  splice @all, ($all[0] >= 23 ? 2 : 1), 1;
  $content = join ' ', @all;
  print "ignore_hash: after $content\n" if $verbose > 1;
  return $content;
}

#
# Compare the sections named 'section_name' in the two packages for equality.
# Return true if they are identical.
# Note: temp_filename is used as a temporary file and will be removed before returning.
#
sub sections_same($$$$) {
  my ($pkg1, $pkg2, $section_name, $temp_filename) = @_;

  my $content1 = extract_section($pkg1, $section_name, $temp_filename);
  return undef if !defined $content1;
  my $content2 = extract_section($pkg2, $section_name, $temp_filename);
  return undef if !defined $content2;

  if ($section_name eq '.acl.autodiscovery') {
    # We have to ignore the random hash
    $content1 = ignore_hash($content1);
    $content2 = ignore_hash($content2);
  }

  my $verbose = acl::Common::get_verbose();
  if ($verbose > 1 and $content1 ne $content2) {
    print "Sections $section_name differ:\n";
    if ($verbose > 2) {
      print "File 1 contents:\n$content1\n";
      print "File 2 contents:\n$content2\n";
    }
  }
    
  return $content1 eq $content2;
}

#
# Remove options like -I name, -D xname=xxx from the argument string.
#
sub clean_options($) {
  my ($str) = @_;
  while($str =~ /(.*)(\s+)(-[LID][^-]*)(.*)/) {
    $str = $1.$2.$4;
  }
  # handle option mid-opt.
  while($str =~ /(.*)(-reuse-(aocx|exe)=[^ ]*)(.*)/) {
    $str = $1.$4;
  }
  # Handle trailing arg.
  while($str =~ /(.*)(-reuse-(aocx|exe)=.*)$/) {
    $str = $1;
  }
  # compress spaces
  $str =~ s/  +/ /g;
  # remove trailing space
  $str =~ s/ $//;
  return $str;
}

#
# Compare aoc arguments for equality.  Ignore known safe values.
#
sub arguments_same($$$) {
  my ($pkg1, $pkg2, $temp_filename) = @_;

  my $content1 = extract_section($pkg1, '.acl.compilation_env', $temp_filename);
  return undef if !defined $content1;
  my $content2 = extract_section($pkg2, '.acl.compilation_env', $temp_filename);
  return undef if !defined $content2;

  # Extract the INPUT_ARGS from each content string
  my ($args1, $args2);
  if ($content1 =~ /INPUT_ARGS=(.*)\n/) {
    $args1 = $1;
  }
  if ($content2 =~ /INPUT_ARGS=(.*)\n/) {
    $args2 = $1;
  }
  return undef if !defined($args1) || !defined($args2);

  $args1 = clean_options($args1);
  $args2 = clean_options($args2);
  my @all_args1 = split /\s+/, $args1;
  my @all_args2 = split /\s+/, $args2;

  # Remove harmless known options;
  my %ignore = (
    "-v" => "1",
    "-rtl" => "1",
    "-save-temps" => "1",
    "--save-temps" => "1",
    "-report" => "1",
    "--report" => "1",
    "-march=simulator" => "1", # checked separately
    "-g" => "1",
    "-g0" => "1",
    "-W" => "1",
    "-Werror" => "1",
    );
  @all_args1 = grep { !defined($ignore{$_}) } @all_args1;
  @all_args2 = grep { !defined($ignore{$_}) } @all_args2;
  
  # The number of arguments must be the same.
  my $verbose = acl::Common::get_verbose();
  if (scalar @all_args1 != scalar @all_args2) {
    if ($verbose > 1) {
      print "Reduced Args1: ", join(' ', @all_args1), "\n";
      print "Reduced Args2: ", join(' ', @all_args2), "\n";
    }
    return undef;
  }

  # Check the individual elements
  for (my $i = 0; $i < scalar @all_args1; $i++) {
    if ($all_args1[$i] != $all_args2[$i]) {
      if ($verbose > 1) {
        print "Reduced Args1: ", join(' ', @all_args1), "\n";
        print "Reduced Args2: ", join(' ', @all_args2), "\n";
      }
      return 0;
    }
  }

  return 1;
}

# Extract the board and kernel information
sub extract_kernel_info($) {
  my ($content) = @_;
  my (%kernels);
  open my $fh, '<', \$content || die "Unable to read string\n";
  while (<$fh>) {
    if (/(.*) (.*)/) {
      $kernels{$1} = $2;
    }
  }
  close $fh;
  return %kernels;
}

#
# The .acl.rtl-hash section differs
# This section contains per kernel checksums and a final top level checksum:
# Kernel1 cksum1
# Kernel2 cksum2
# ...
# {toplevel} cksumN
#
sub kernel_difference_msg($$$) {
  my ($old_content, $new_content, $temp_filename) = @_;
  my $changes = '';
  my $top_changed = 0;
  if (defined $old_content && defined $new_content) {
    my %old_kernels = extract_kernel_info($old_content);
    my %new_kernels = extract_kernel_info($new_content);

    # Do the kernels match?
    foreach my $kname (sort keys %old_kernels) {
      if (!defined $new_kernels{$kname}) {
        $changes .= "The kernel '$kname' has been removed.\n";
      }
      # The kernel is in both. Are the arguments identical?
      elsif ($old_kernels{$kname} ne $new_kernels{$kname}) {
        if ($kname eq '{toplevel}') {
          # Try not to display this message, as it doesn't tell the user anything terribly useful.
          $top_changed = 1;
        } else {
          $changes .= "The kernel '$kname' has changed.\n";
        }
      }
    }

    # Have we added a new kernel?
    foreach my $kname (sort keys %new_kernels) {
      if (!defined $old_kernels{$kname}) {
        $changes .= "The kernel '$kname' has been added.\n";
      }
    }
  }

  # Backup message if the top level interconnect changes and nothing else did.
  $changes = "The top level interconnect changed.\n" if $top_changed && $changes eq '';

  return $changes if $changes ne "";

  # We don't know why they are different.
  return "The previous compile was not identical to the current one.";
}

1;
