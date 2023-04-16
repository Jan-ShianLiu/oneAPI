
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

acl::AOCOpenCLStage.pm - OpenCL Compiler Invocations. Stage 1

=head1 VERSION

$Header: //depot/sw/hld/rel/20.3api.11/acl/sysgen/lib/acl/AOCOpenCLStage.pm#3 $

=head1 DESCRIPTION

This module provides methods that run the Stage 1 of the compiler.
They take user source code and process it through CLang, LLVM,
the Backend, and finally System Integrator

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


package acl::AOCOpenCLStage;
use strict;
use Exporter;

require acl::Common;
require acl::Env;
require acl::File;
require acl::Incremental;
require acl::Pkg;
require acl::Report;
use acl::AOCDriverCommon;
use acl::AOCInputParser;

our @ISA = qw(Exporter);
our @EXPORT_OK = qw ( link_objects create_system );

# Exported Functions

sub rewrite_sycl_ir {
  my ($input_file, $output_file, $board_option) = @_;
  my $save_temps = acl::Common::get_save_temps();
  my $return_status = acl::Common::mysystem_full(
      {'time' => 1, 'time-label' => 'opt', 'stdout' => 'opt.log', 'stderr' => 'opt.err'},
      "$opt_exe -rewrite-sycl-ir $board_option \"$input_file\" -o \"$output_file\"");
  acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
  my $banner = '!========== [opt] rewrite SYCL IR fpga ==========';
  acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog") if $regtest_mode;
  acl::Common::mydie("Unable to rewrite SYCL IR file\n") if $return_status != 0;
  acl::AOCDriverCommon::remove_named_files($input_file) unless $save_temps;
  if ($disassemble) {
    acl::AOCDriverCommon::mysystem("llvm-dis \"$output_file\"" ) == 0 or
    acl::Common::mydie("Cannot disassemble: \"$output_file\" \n");
  }
  if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
  if (($return_status==0 or $regtest_mode==0) and !$save_temps) { unlink 'opt.log'; }
  return $return_status;
}

sub fast_emulator_link_build($$$$@) {
  my ($spv_file, $output, $build_options, $cleanup_list, @bcs) = @_;
  my $ioc_cmd = "-cmd=link";
  my $ioc_dev = "-device=fpga_fast_emu";
  my $ioc_inp;
  if (defined($spv_file)) {
    $ioc_inp = "-spv=$spv_file";
  } else {
    $ioc_inp = "-binary=".join(",", @bcs) ;
  }
  my $ioc_out = "-ir=$output";
  my $output_temp = $output;
  $output_temp =~ s/\\/\//g;
  my @split_output = split('/', $output_temp);

  my $temp_name = $split_output[@split_output - 1];
  $temp_name =~ s/.aocx//;

  my $log_file = "ioc_link_$temp_name.log";
  my $err_file = "ioc_link_$temp_name.err";

  my @cmd_list = (
      $ioc_exe,
      $ioc_cmd,
      $ioc_dev,
      $ioc_inp,
      $ioc_out);
  if (defined($build_options)) {
    my $ioc_bo;
    if (acl::Env::is_windows()) {
      $ioc_bo = "-bo=\"$build_options\"";
    } else {
      $ioc_bo = "-bo=$build_options";
    }
    push(@cmd_list, $ioc_bo);
  }

  $return_status = acl::Common::mysystem_full(
    { 'stdout' => $log_file,
      'stderr' => $err_file,
      'title' => defined($spv_file) ? 'Compiling SPIR-V file' : 'Linking Emulator Files',
      'time' => 1,
      'time-label' => 'ioc link'},
      @cmd_list);

  acl::Report::append_to_err($err_file);
  if ($return_status==0 or $regtest_mode==0) { unlink $err_file; }
        
  if ($return_status != 0 and $regtest_mode and -s $err_file) {
    acl::Common::move_to_log('!========== Fast Emulator - Link ==========', $err_file, "$work_dir/../$regtest_errlog");
  }

  # Go through ioc_link.log and print any errors or warnings.
  open(INPUT, '<', $log_file) or acl::Common::mydie("Can't open $log_file $!");
  my $start_printing = acl::Common::get_verbose() > 1;
  my $link_failed = $return_status;
  while (my $line = <INPUT>) {
    $link_failed = 1 if ($line =~ m/^(Compilation|Linkage|Linking|Device build) failed!?$/);
    if (acl::Common::get_verbose() > 2) {
      print $line;
    } elsif ($line =~ m/^(Compilation|Linkage|Linking|Device build) started$/) {
      $start_printing = 1;
    } elsif ($line =~ m/^(Compilation|Linkage|Linking|Device build) failed$/ and $start_printing == 0) {
      $start_printing = 1;
    } elsif ($line =~ m/^(Compilation|Linkage|Linking|Device build) failed!?$/) {
      $start_printing = 0 unless acl::Common::get_verbose();
    } elsif ($line =~ m/^(Compilation|Linkage|Linking|Device build) done$/) {
      $start_printing = 0;
    } elsif ($line =~ m/^Options used by backend compiler/) {
      print $line if acl::Common::get_verbose() > 1;
    } elsif ($start_printing) {
      print $line;
    }
  }
  close INPUT;

  acl::Common::mydie("Compiler Error: OpenCL kernel compile/link FAILED") if ($link_failed);
  push @$cleanup_list, $log_file unless acl::Common::get_save_temps();
}

sub link_objects {
  my $quiet_mode = acl::Common::get_quiet_mode();
  if ($sycl_mode == 0 or acl::Common::get_verbose() > 0){
    print "$prog: Linking Object files....\n" if (!$quiet_mode);  
  }
  my $bsp_path = undef;
  my $board_name = undef;
  my $target = undef;
  my $version = undef;
  my $compileoptions = undef;

  my $has_RTL = undef;
  my $lib_handle = "$$-lib";
  my @bc_temp_list = ();
  my @ioc_obj_temp_list = ();
  
  # Unpack files from new "plain archive" aoclibs
  # Have to do this here even though the project directory haven't been established yet.
  # All the other code also just creates temporary files in the current working directory. 
  # So I will do the same. Since there is some magic that copies incremental stuff before 
  # we even consider creating the project directory if it doesn't exists, some significant 
  # refactoring is needed.
  # Probably: do some incremental stuff -> create/cleanup project directory -> run this 
  # section so it can use the project directory as schratch pad.
  # TODO: extraction of HDL and XML files skips files containing certain characters that
  # are valid in filenames (e.g. '-') and cannot cope with spaces in filenames. 
  # This code ONLY unpacks archive aoclibs; these are removed from the list of resolved lib
  # files so that create_system() does not attempt to link their contents. 
  my @old_style_lib_files;
  my @aoco_from_new_style_lib_files;
  while (@resolved_lib_files) {
    my $libfile = shift @resolved_lib_files;
    my $exit_code=system("aocl binedit $libfile exists .acl.info" );
    if ( $exit_code == 0 ) {
      # old style library file which will be passed to create_system
      push @old_style_lib_files, $libfile;
    } else {
      # Unpack new style library file remove it from @resolved_lib_files
      $libfile =~ qr/([^.]*)[\/\\]([^.]*)\.[^.]*/;
      my $libname = $2;
      my $libdir = "$$-lib/$libname";
      acl::File::make_path($libdir) or acl::Common::mydie("Can't create temporary directory $libdir: $!");
      my $cwd = acl::File::cur_path();
      chdir $libdir or acl::Common::mydie("Can't change dir into $libdir: $!");
      my $contents = acl::Common::mybacktick("aocl binedit $libfile list");
      foreach (split(/\n/,$contents)) {
        $_ =~ /\.archive\.(.+)\.aoco,\s+[0-9]+\s+bytes/ || next;
        my $objfile = $1.'.aoco';
        system("aocl binedit $libfile get .archive.$objfile $objfile");
        push @aoco_from_new_style_lib_files, acl::File::abs_path($objfile);
      }
      chdir $cwd or acl::Common::mydie("Can't change dir into: $cwd: $!");
    }
  }
  # @resolved_lib_files will be used later in create_system
  @resolved_lib_files = @old_style_lib_files;

  # deal with .aoco files in the new style library files and the .aoco files
  # from -library-list flag. These files are stored in abs path format
  foreach my $aoco_file (@aoco_from_new_style_lib_files, @aoco_library_list) {
    my $cwd = acl::File::cur_path();
    my $aoco_dir = acl::File::mydirname($aoco_file);
    chdir $aoco_dir or acl::Common::mydie("Can't change dir into $aoco_dir : $!");
    my $object_contents = acl::Common::mybacktick("aocl binedit $aoco_file list");
        if ($object_contents =~ /\.xml_spec\./) { # HDL container
          $has_RTL = 1;
          if ($object_contents =~/\.clang_ir\./ and $emulator_flow) {
        push @objfile_list, $aoco_file;
          }
      (my $objname_hdl = acl::File::mybasename($aoco_file)) =~ s/\..*/_hdl/;
          foreach (split(/\n/,$object_contents)) {
            $_ =~ /\.(source|xml_spec)\.(.+\.[a-z]+),\s+[0-9]+\s+bytes/ || next;
            my $section_type = $1;
            my $section_filename = $2;
        my $target_filename = "$$-lib/" . $objname_hdl."/$section_filename";
            acl::File::make_path_to_file($target_filename) or acl::Common::mydie("Can't create subdirectory for $target_filename: $!");;
        system("aocl binedit $aoco_file get .$section_type.$section_filename $target_filename");
            if ($section_type eq "xml_spec") {
              push @resolved_hdlspec_files, acl::File::abs_path("$target_filename");
            }
          }
        }
        else { # aoco is not an HDL bundle
      push @objfile_list, $aoco_file;
        }
    chdir $cwd or acl::Common::mydie("Can't change dir into: $cwd: $!");
    }

  my $fpga_triple = $sycl_mode ? 'spir64-unknown-unknown-sycldevice' : 'spir64-unknown-unknown-intelfpga';
  my $emulator_triple = ($emulator_arch eq 'windows64') ? 'x86_64-pc-windows-intelfpga' : 'x86_64-unknown-linux-intelfpga';
  my $cur_flow_triple = $emulator_flow ? $emulator_triple : $fpga_triple;
  
  # Compile options which do not have to be the same for all obj files
  my @compile_options_whitelist = qw(-ffp-reassoc -ffp-contract=fast);

  # Local copy of dependency files list
  my @my_dep_files = ();

  for (my $i = 0; $i <= $#objfile_list; $i++) {
    my $obj = $objfile_list[$i];
    my $dep_file_temp = $obj.".d.temp";
    my $bc_file_temp = $obj.".temp.bc";
    my $ioc_bc_file_temp = $obj.".temp.ioc.bc";
    my $ioc_obj_file_temp = $obj.".temp.ioc.obj";
    my $log_file_temp = $obj.".temp.log";

    # read information from package file
    my $pkg = get acl::Pkg($obj) or die "Can't find pkg file $obj: $acl::Pkg::error\n";   
    my $obj_target = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.target');
    my $obj_version = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.version');
    #check to make sure the targets for all the files are same
    if (!defined $target) {
      $target = $obj_target;
    } elsif ($obj_target ne 'library' and $target ne $obj_target) {
      acl::Common::mydie("Invalid target for $obj");
    }

    #check to make sure the versions for all the files are same
    unless ($library_version_override) {
      if (!defined $version) {
        $version = $obj_version;
      } elsif ($version ne $obj_version) {
        acl::Common::mydie("Invalid version for $obj");
      }
    }
    
    if ($obj_target ne 'library') {
      my $obj_compileoptions = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.compileoptions');
      
      $obj_compileoptions =~ s/[ ]+//ig;
      foreach (@compile_options_whitelist) {
        $obj_compileoptions =~ s/$_//ig;
      }
      #handle cases where the argument can be '-' or '--'
      $obj_compileoptions =~ s/[-]+//ig;
      my $obj_sort_compileoptions = join '', sort split(//, $obj_compileoptions);

      #check to make sure the compile options for all the files are same
      if (!defined $compileoptions) {
        $compileoptions = $obj_sort_compileoptions;
      } elsif ($compileoptions ne $obj_sort_compileoptions) {
        acl::Common::mydie("Invalid compileoptions for $obj");
      }
    }
    if ( !($obj_target eq 'emulator' or $obj_target eq 'simulator' or $obj_target eq 'emulator_fast' or $obj_target eq 'library' ) ) {
      my $obj_board_package = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.board_package');
      my $obj_board_name = acl::AOCDriverCommon::get_pkg_section($pkg,'.acl.board');
      if (!defined $bsp_path) {
        $bsp_path = $obj_board_package;
      } elsif ($bsp_path ne $obj_board_package) {
        acl::Common::mydie("Invalid board package path for $obj");
      }
      if (!defined $board_name) {
        $board_name = $obj_board_name;
      } elsif ($board_name ne $obj_board_name) {
        acl::Common::mydie("Invalid board name for $obj");
      }
    }
    my $llvm_board_option = "";
    if (defined($bsp_variant)) {
      my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
      my $board_spec_xml = acl::AOCDriverCommon::find_board_spec($acl_board_hw_path);
      $llvm_board_option = "-board \"$board_spec_xml\"";   # To be passed to LLVM executables.
    }

    if ($obj_target eq 'emulator_fast') {
      $pkg->get_file('.acl.ioc_obj',$ioc_obj_file_temp);
      push @ioc_obj_temp_list, $ioc_obj_file_temp;
    } elsif ($obj_target eq 'library') {
      # The SYCL library fpga and fast-emulator objects have the triple
      # 'spir64-unknown-unknown-sycldevice'. Rewrite the triple
      # to 'spir64-unknown-unknown-intelfpga' so that the object extracted from
      # the library can be linked properly without warning of triple mismatch
      if ($sycl_mode) {
        (my $bc_file_temp_for_sycl = $bc_file_temp) =~ s/.bc/.sycl.bc/;
        $pkg->get_file('.acl.clang_ir.'.$cur_flow_triple, $bc_file_temp_for_sycl);
        rewrite_sycl_ir($bc_file_temp_for_sycl, $bc_file_temp, "$llvm_board_option");
      } else {
        $pkg->get_file('.acl.clang_ir.'.$cur_flow_triple, $bc_file_temp);
      }
      push @bc_temp_list, $bc_file_temp;

      #A library can contain either OpenCL based source (ioc_obj), C++ based 
      #source (ioc_bc) or neither (RTL library without emulation model).
      if ( $pkg->exists_section('.acl.ioc_obj')) {
        $pkg->get_file('.acl.ioc_obj',$ioc_obj_file_temp);
        push @ioc_obj_temp_list, $ioc_obj_file_temp;
      } elsif ( $pkg->exists_section('.acl.ioc_bc')) {
        if ($sycl_mode) {
          (my $ioc_bc_file_temp_for_sycl = $ioc_bc_file_temp) =~ s/.bc/.sycl.bc/;
          $pkg->get_file('.acl.ioc_bc', $ioc_bc_file_temp_for_sycl);
          rewrite_sycl_ir($ioc_bc_file_temp_for_sycl, $ioc_bc_file_temp, "$llvm_board_option");
        } else {
          $pkg->get_file('.acl.ioc_bc',$ioc_bc_file_temp);
        }
        push @ioc_obj_temp_list, $ioc_bc_file_temp;
      }
    } else {
      $pkg->get_file('.acl.aoco',$bc_file_temp);
      push @bc_temp_list, $bc_file_temp;
    }

    if ($obj_target ne 'emulator_fast' and $obj_target ne 'library') {
      if ( $pkg->exists_section('.acl.dep')) {
        $pkg->get_file('.acl.dep',$dep_file_temp);
        push @my_dep_files, $dep_file_temp;
      }

      # Deal with clang logs
      $pkg->get_file('.acl.clang_log',$log_file_temp);
      open(INPUT,"<$log_file_temp") or acl::Common::mydie("Can't open $log_file_temp: $!");
      push @clang_warnings, <INPUT>;
      close INPUT;
      unlink $log_file_temp;
    }
  }
  push @all_dep_files, @my_dep_files;

  if ($user_defined_flow eq 1) {
    if($emulator_flow eq 1 && $target ne 'emulator' && $target ne 'emulator_fast') {
      acl::Common::mydie("Object target does not match 'emulator'");
    }
    if($new_sim_mode eq 1 && $target ne 'simulator'){
      acl::Common::mydie("Object target does not match 'simulator'");
    }
  }

  if ($target eq 'emulator') {
    $emulator_flow = 1;
  } elsif ($target eq 'emulator_fast') {
    $emulator_flow = 1;
    $emulator_fast = 1;
  } elsif ($target eq 'simulator') {
    $new_sim_mode = 1;
    $ip_gen_only = 1;
    $atleastoneflag = 1;
  } else {
    $ENV{'AOCL_BOARD_PACKAGE_ROOT'} = $bsp_path;
    if( $user_defined_board == 1 && $board_name ne $board_variant ) {
      acl::Common::mydie("Board specified '$board_variant' does not match the board '$board_name' in aoco package\n");
    } else {
      $board_variant = $board_name;
    }
  }

  # clean up bc and ioc_obj files and their disassembled ll files if applicable
  my @cleanup_list = (@bc_temp_list, @ioc_obj_temp_list) unless acl::Common::get_save_temps();
  push @cleanup_list, map{ (my $tmp = $_) =~ s/.bc$/.ll/; $tmp; } @cleanup_list if $disassemble;

  if ($emulator_fast) {
    fast_emulator_link_build(undef, $linked_objfile, undef, \@cleanup_list, @ioc_obj_temp_list);
  } else {
    my @cmd_list = ();

    my $result_file = shift @bc_temp_list;
    my $next_res = undef;
    my $indexnum = 0;

    foreach (@bc_temp_list) {
      # Just add one file at the time since llvm-link has some issues
      # with unifying types otherwise. Introduces small overhead if 3
      # source files or more
      $next_res = $linked_objfile.'.'.$indexnum++;
      @cmd_list = (
          @link_exe,
          $result_file,
          $_,
          '-o',$next_res );

      acl::Common::mysystem_full( {'title' => 'Link IR'}, @cmd_list) == 0 or acl::Common::mydie();
      push @cleanup_list, $next_res;
      $result_file = $next_res;
    }

    my $opt_input = defined $next_res ? $next_res : $result_file;
    rename $opt_input, $linked_objfile;
  }

  if (!$has_RTL) {
    acl::File::remove_tree($lib_handle)
    or acl::Common::mydie("Cannot remove files under temporary directory $lib_handle: $!\n");
  }

  # clean up the clean list in the end. All the file/dir in the clean list should be added because save_temp is not set
  acl::AOCDriverCommon::remove_named_files(@cleanup_list);
}

sub create_system {
  my ($base, $bsp_variant, $final_work_dir, $obj, $all_aoc_args,$bsp_flow_name,$incremental_input_dir, $absolute_srcfile_list) = @_;
  my $pkg_file_final = $obj;
  my $pkg_file = acl::Common::set_package_file_name($pkg_file_final.".tmp");
  my $verbose = acl::Common::get_verbose();
  my $quiet_mode = acl::Common::get_quiet_mode();
  my $save_temps = acl::Common::get_save_temps();
  $fulllog = "$base.log"; #definition moved to global space
  my $run_copy_skel = 1;
  my $run_copy_ip = 1;
  my $run_opt = 1;
  my $run_verilog_gen = 1;
  my $fileJSON;
  my @move_files = ();
  my @save_files = ();
  if ($incremental_compile) {
    push (@save_files, 'qdb');
    push (@save_files, 'current_partitions.txt');
    push (@save_files, 'new_floorplan.txt');
    push (@save_files, 'io_loc.loc');
    push (@save_files, 'partition.*.qdb');
    push (@save_files, 'prev');
    push (@save_files, 'soft_regions.txt');
    push (@move_files, ('previous_partition_grouping_incremental.txt', '*_sys.v', '*_system.v', '*.bc.xml', 'reports', 'kernel_hdl', $marker_file));
  }
  my $finalize = sub {
     unlink( $pkg_file_final ) if -f $pkg_file_final;
     rename( $pkg_file, $pkg_file_final )
         or acl::Common::mydie("Can't rename $pkg_file to $pkg_file_final: $!");
     my $orig_dir = acl::Common::get_original_dir();
     chdir $orig_dir or acl::Common::mydie("Can't change back into directory $orig_dir: $!");
  };

  if ( $parse_only || $opt_only || $verilog_gen_only || $emulator_flow ) {
    $run_copy_ip = 0;
    $run_copy_skel = 0;
  }

  my $stage1_start_time = time();
  #Create the new direcory verbatim, then rewrite it to not contain spaces
  $work_dir = $final_work_dir;
  # If there exists a file with the same name as work_dir
  if (-e $work_dir and -f $work_dir) {
    acl::Common::mydie("Can't create project directory $work_dir because file with the same name exists\n");
  }
  #If the work_dir exists, check whether it was created by us
  if (-e $work_dir and -d $work_dir) {
  # If the marker file exists, this was created by us
  # Cleaning up the whole project directory to avoid conflict with previous compiles. This behaviour should change for incremental compilation.
    if (-e "$work_dir/$marker_file" and -f "$work_dir/$marker_file") {
      print "$prog: Cleaning up existing temporary directory $work_dir\n" if ($verbose >= 2);

      if ($incremental_compile && !$incremental_input_dir) {
        $acl::Incremental::warning = "$prog: Found existing directory $work_dir, basing incremental compile off this directory.\n";
        print $acl::Incremental::warning if ($verbose);
        $incremental_input_dir = $work_dir;
      }

      # If incremental, copy over all incremental files before removing anything (in case of failure or force stop)
      if ($incremental_compile && acl::File::abs_path($incremental_input_dir) eq acl::File::abs_path($work_dir)) {
        # Check if prev directory exists and that the marker file exists inside it. The marker file is added after all the necessary
        # previous files are copied over. This indicates that we have a valid set of previous files. The prev directory should automatically
        # be removed after a successful compile, so this directory should only be left over in the case where an incremental compile has failed
        # If an incremental compile has failed, then we should keep the contents of this directory since the kernel_hdl and .bc.xml file in the project 
        # directory may have been already been overwritten
        $incremental_input_dir = "$work_dir/prev";
        if (! -e $incremental_input_dir || ! -d $incremental_input_dir || ! -e "$incremental_input_dir/$marker_file") {
          acl::File::make_path($incremental_input_dir) or acl::Common::mydie("Can't create temporary directory $incremental_input_dir: $!");
          foreach my $reg (@move_files) {
            foreach my $f_match ( acl::File::simple_glob( "$work_dir/$reg") ) {
              my $file_base = acl::File::mybasename($f_match);
              acl::File::copy_tree( $f_match, "$incremental_input_dir/" );
            }
          }
        }
      }

      foreach my $file ( acl::File::simple_glob( "$work_dir/*", { all => 1 } ) ) {
        if ( $file eq "$work_dir/." or $file eq "$work_dir/.." or $file eq "$work_dir/$marker_file" ) {
          next;
        }
        my $next_check = undef;
        foreach my $reg (@save_files) {
          if ( $file =~ m/$reg/ ) { $next_check = 1; last; }
        }
        # if the file matches one of the regexps, skip its removal
        if( defined $next_check ) { next; }

        acl::File::remove_tree( $file )
          or acl::Common::mydie("Cannot remove files under temporary directory $work_dir: $!\n");
      }
    } else {
      acl::Common::mydie("Please rename the existing directory $work_dir to avoid name conflict with project directory\n");
    }
  }

  acl::File::make_path($work_dir) or acl::Common::mydie("Can't create temporary directory $work_dir: $!");
  if ($incremental_input_dir ne '' && $incremental_input_dir ne "$work_dir/prev") {
    foreach my $reg (@save_files) {
      foreach my $f_match ( acl::File::simple_glob( "$incremental_input_dir/$reg") ) {
        my $file_base = acl::File::mybasename($f_match);
        acl::File::copy_tree( $f_match, $work_dir."/" );
      }
    }
    $incremental_input_dir = acl::File::abs_path("$incremental_input_dir");
  }

  # Create a marker file
  my @cmd = acl::Env::is_windows() ? ("type nul > \"$work_dir/$marker_file\""):("touch", "$work_dir/$marker_file");
  acl::Common::mysystem_full({}, @cmd);
  # First, try to delete the log file
  if (!unlink "$work_dir/$fulllog") {
    # If that fails, just try to erase the existing log
    open(LOG, ">$work_dir/$fulllog") or acl::Common::mydie("Couldn't open $work_dir/$fulllog for writing.");
    close(LOG);
  }
  open(my $TMPOUT, ">$work_dir/$fulllog") or acl::Common::mydie ("Couldn't open $work_dir/$fulllog to log version information.");
  print $TMPOUT "Compiler Command: " . $prog . " " . $all_aoc_args . "\n";
  if (defined $acl::Incremental::warning) {
    print $TMPOUT $acl::Incremental::warning;
  }
  if ($regtest_mode){
      acl::AOCDriverCommon::version($TMPOUT);
  }
  close($TMPOUT);

  # If just packaging an HDL library component, call 'aocl library' and be done with it.
  if ($hdl_comp_pkg_flow) {
    print "$prog: Packaging HDL component for library inclusion\n" if $verbose||$report;
    foreach my $absolute_srcfile (@absolute_srcfile_list){
      $return_status = acl::Common::mysystem_full(
        {'stdout' => "$work_dir/aocl_libedit.log", 
         'stderr' => "$work_dir/aocl_libedit.err",
         'time' => 1, 'time-label' => 'aocl library'},
        "$aocl_libedit_exe -c \"$absolute_srcfile\" -o \"$output_file\"");
      my $banner = '!========== [aocl library] ==========';
      acl::AOCDriverCommon::move_to_err_and_log($banner, "$work_dir/aocl_libedit.log", "$work_dir/$fulllog"); 
      acl::Report::append_to_log("$work_dir/aocl_libedit.err", "$work_dir/$fulllog");
      acl::Report::append_to_err("$work_dir/aocl_libedit.err");
      if ($return_status==0 or $regtest_mode==0) { unlink "$work_dir/aocl_libedit.err"; }
      if ($return_status != 0) {
        if ($regtest_mode) {
          acl::Common::move_to_log($banner, "$work_dir/aocl_libedit.err", "$work_dir/../$regtest_errlog");
        }
        acl::Common::mydie("Packing of HDL component FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");  
      }
    }
    
    return $return_status;
  }
  
  # If it's fast emulator flow, it dosen't need to do the quartus and bsp check, finish it first
  # For emulator and non-emulator flows, extract clang-ir for library components
  # that were written using OpenCL
  # This ONLY applies to regular aoclib files; the archive aoclibs are unpacked and linked in sub link_files()
  # Figure out the compiler triple for the current flow.
  my $fpga_triple = 'spir64-unknown-unknown-intelfpga';
  my $emulator_triple = ($emulator_arch eq 'windows64') ? 'x86_64-pc-windows-intelfpga' : 'x86_64-unknown-linux-intelfpga';
  my $cur_flow_triple = $emulator_flow ? $emulator_triple : $fpga_triple;
  if ($#resolved_lib_files > -1) {
    foreach my $libfile (@resolved_lib_files) {
      if ($verbose >= 2) { print "Executing: $aocl_libedit_exe extract_clang_ir \"$libfile\" $cur_flow_triple $work_dir\n"; }
      my $new_files = acl::Common::mybacktick("$aocl_libedit_exe extract_clang_ir \"$libfile\" $cur_flow_triple $work_dir");
      if ($? == 0) {
        if ($verbose >= 2) { print "  Output: $new_files\n"; }
        push @lib_bc_files, split /\n/, $new_files;
      }
    }
  }

  # Create package file in directory, and save compile options.
  my $pkg = create acl::Pkg($pkg_file);

  if ($emulator_fast) {
    if ($sycl_mode == 0 or $verbose > 0){
      print "$prog: Compiling for Emulation ....\n" if (!$quiet_mode);
    }
    chdir $work_dir or acl::Common::mydie("Can't change dir into $work_dir: $!");
    if ($emulator_arch eq 'windows64') {
      $pkg->set_file('.acl.fast_emulator_object.windows',$linked_objfile)
        or acl::Common::mydie("Can't save emulated kernel into package file: $acl::Pkg::error\n");
    } else {     
      $pkg->set_file('.acl.fast_emulator_object.linux',$linked_objfile)
        or acl::Common::mydie("Can't save emulated kernel into package file: $acl::Pkg::error\n");
    }
    acl::AOCDriverCommon::remove_named_files($linked_objfile) unless $save_temps;
    foreach my $lib_bc_file (@lib_bc_files) {
      acl::AOCDriverCommon::remove_named_files($lib_bc_file) unless $save_temps;
    }
    my $compilation_env = acl::AOCDriverCommon::compilation_env_string($work_dir,$board_variant,$all_aoc_args,$bsp_flow_name);
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.compilation_env',$compilation_env);

    # Compute runtime.
    my $stage1_end_time = time();
    acl::Common::log_time ("emulator compilation", $stage1_end_time - $stage1_start_time);

    print "$prog: Emulator Compilation completed successfully.\n" if $verbose;
    &$finalize();
    return;
  }

  # Make sure the board specification file exists. This is needed by multiple stages of the compile.
  my $acl_board_hw_path = acl::AOCDriverCommon::get_board_hw_path_from_bsp($bsp_variant, $board_variant);
  my $board_spec_xml = acl::AOCDriverCommon::find_board_spec($acl_board_hw_path);
  my $llvm_board_option = "-board \"$board_spec_xml\"";   # To be passed to LLVM executables.
  my $llvm_profilerconf_option = (defined $absolute_profilerconf_file ? "-profile-config $absolute_profilerconf_file" : ""); # To be passed to LLVM executables
  my $llvm_library_option = join(' ',map { (qw(-libfile), "\"$_\"") } @resolved_lib_files);
  my $llvm_hdlspec_option = join(' ',map { (qw(-hdlspec), "\"$_\"") } @resolved_hdlspec_files);

  if(defined $absolute_profilerconf_file) {
    print "$prog: Selected profiler conf $absolute_profilerconf_file\n" if $verbose||$report;
  }

  if ( $run_copy_skel ) {
    # Copy board skeleton, unconditionally.
    # Later steps update .qsf and .sopc in place.
    # You *will* get SOPC generation failures because of double-add of same
    # interface unless you get a fresh .sopc here.
    acl::File::copy_tree( $acl_board_hw_path."/*.qsf", $work_dir )
      or acl::Common::mydie("Can't copy Board qsf files: $acl::File::error");
    acl::File::copy_tree( $acl_board_hw_path."/board_spec.xml", $work_dir )
      or acl::Common::mydie("Can't copy Board board_spec.xml file: $acl::File::error");
    acl::File::copy_tree( $acl_board_hw_path."/quartus.ini", $work_dir )
      or acl::Common::mydie("Can't copy Board quartus.ini file: $acl::File::error");
    my $device_model_file = acl::Env::aocl_boardspec( $board_spec_xml, "devicemodel");
    acl::File::copy_tree( $acl_board_hw_path."/".$device_model_file, $work_dir )
      or acl::Common::mydie("Can't copy Board dm.xml files: $acl::File::error");
    map { acl::File::make_writable($_) } (
      acl::File::simple_glob( "$work_dir/*.qsf" ),
      acl::File::simple_glob( "$work_dir/*.sopc" ) );
  }

  if ( $run_copy_ip ) {
    # Rather than copy ip files from the SDK root to the kernel directory, 
    # generate an opencl.ipx file to point Qsys to hw.tcl components in 
    # the IP in the SDK root when generating the system.
    acl::Env::create_opencl_ipx($work_dir);

    # Also generate an assignment in the .qsf pointing to this IP.
    # We need to do this because not all the hdl files required by synthesis
    # are necessarily in the hw.tcl (i.e., not the entire file hierarchy).
    #
    # For example, if the Qsys system needs A.v to instantiate module A, then
    # A.v will be listed in the hw.tcl. Every file listed in the hw.tcl also
    # gets copied to system/synthesis/submodules and referenced in system.qip,
    # and system.qip is included in the .qsf, therefore synthesis will be able
    # to find the file A.v. 
    #
    # But if A instantiates module B, B.v does not need to be in the hw.tcl, 
    # since Qsys still is able to find B.v during system generation. So while
    # the Qsys generation will still succeed without B.v listed in the hw.tcl, 
    # B.v will not be copied to submodules/ and will not be included in the .qip,
    # so synthesis will fail while looking for this IP file. This happens in the 
    # virtual fabric flow, where the full hierarchy is not included in the hw.tcl.
    #
    # Since we are using an environment variable in the path, move the
    # assignment to a tcl file and source the file in each qsf (done below).
    my $ip_include = "$work_dir/ip_include.tcl";
    open(my $fh, '>', $ip_include) or die "Cannot open file '$ip_include' $!";
    print $fh 'set_global_assignment -name SEARCH_PATH "$::env(INTELFPGAOCLSDKROOT)/ip"
';
    close $fh;

    if ( scalar @additional_ini ) {
      open (QUARTUS_INI_FILE, ">>$work_dir/quartus.ini");
      foreach my $add_i (@additional_ini) {
        open (INI_FILE, "<$add_i") or die "Couldn't open $add_i for read\n";
        print QUARTUS_INI_FILE "# Copied from $add_i:\n";
        print QUARTUS_INI_FILE (do {local $/; <INI_FILE> });
        print QUARTUS_INI_FILE "\n\n";
      }
      close (INI_FILE);
    }
    close (QUARTUS_INI_FILE);

    # Add Deterministic overuse avoidance INI
    # For incremental compiles, avoid using nodes that are likely to be congested and not routable
    # case:491598
    if ($incremental_compile) {
      if (open( QUARTUS_INI_FILE, ">>$work_dir/quartus.ini" )) {
        print QUARTUS_INI_FILE <<AOC_INCREMENTAL_INI;

# case:491598
aoc_incremental_aware_placer=on
AOC_INCREMENTAL_INI
        close (QUARTUS_INI_FILE);
      }
    }

    # Set soft region INI and exported qsf setting from previous compile to current one
    # Soft region is a Quartus feature to mitigate swiss cheese problem in incremental compile.
    # When below INIs and soft region qsf settings in ip/board/incremental are applied,
    # Fitter exports ATTRACTION_GROUP_SOFT_REGION qsf settings per partition.
    # This region is approximate area the partition's logic was placed in.
    # If these settings are then set in incremental compile, fitter will try to place the partition in the same area.
    if ( $soft_region_on ) {
      if (open( QUARTUS_INI_FILE, ">>$work_dir/quartus.ini" )) {
        if (! -e "$work_dir/soft_regions.txt") {
          print QUARTUS_INI_FILE <<SOFT_REGION_SETUP_INI;

# Apl blobby
apl_partition_gamma_factor=10
apl_ble_partition_bin_size=4
apl_cbe_partition_bin_size=6
apl_use_partition_based_spreading=on
SOFT_REGION_SETUP_INI
          # Create empty soft_regions.txt file so that
          # ip/board/incremental scripts set soft region qsf settings
          open( SOFT_REGION_FILE, ">$work_dir/soft_regions.txt" ) or die "Cannot open file 'soft_regions.txt' $!";
          print SOFT_REGION_FILE "";
          close (SOFT_REGION_FILE);
        } else {
          # Add exported soft region qsf settings from previous compile to current one
          push @additional_qsf, "$work_dir/soft_regions.txt";
        }

        print QUARTUS_INI_FILE <<SOFT_REGION_INI;

# Apl attraction groups
apl_floating_region_aspect_ratio_factor=100
apl_discrete_dp=off
apl_ble_attract_regions=on
apl_region_attraction_weight=100

# DAP attraction groups
dap_attraction_group_cost_factor=10
dap_attraction_group_use_soft_region=on
dap_attraction_group_v_factor=3.0

# Export soft regions filename
vpr_write_soft_region_filename=soft_regions.txt
SOFT_REGION_INI
        close (QUARTUS_INI_FILE);
      }
    }

    # append users qsf to end to overwrite all other settings
    my $final_append = '';
    if( scalar @additional_qsf ) {
      foreach my $add_q (@additional_qsf){
        open (QSF_FILE, "<$add_q") or die "Couldn't open $add_q for read\n";
        $final_append .= "# Contents automatically added from $add_q\n";
        $final_append .= do { local $/; <QSF_FILE> };
        $final_append .= "\n";
        close (QSF_FILE);
      }
    }

    my $qsys_file = ::acl::Env::aocl_boardspec("$board_spec_xml", "qsys_file".$bsp_flow_name);

    if ($fast_compile) {
      # env varaible will be used by scripts in acl/ip during BSP compile
      $ENV{'AOCL_FAST_COMPILE'} = 1;
      print "$prog: Adding Quartus fast-compile settings.\nWarning: Circuit performance will be significantly degraded.\n";
    }

    # Writing flags to *qsf files
    foreach my $qsf_file (acl::File::simple_glob( "$work_dir/*.qsf" )) {
      open (QSF_FILE, ">>$qsf_file") or die "Couldn't open $qsf_file for append!\n";

      if ($cpu_count ne -1) {
        print QSF_FILE "\nset_global_assignment -name NUM_PARALLEL_PROCESSORS $cpu_count\n";
      }

      # Add SEARCH_PATH for ip/$base and qip to the QSF file
      # .qip file contains all file dependencies listed in <foo>_sys_hw.tcl
      if ($qsys_file eq "none" and $bsp_version >= 18.0) {
        print QSF_FILE "\nset_global_assignment -name QIP_FILE kernel_system.qip\n";
      }

      # Source a tcl script which points the project to the IP directory
      print QSF_FILE "\nset_global_assignment -name SOURCE_TCL_SCRIPT_FILE ip_include.tcl\n";

      # Case:149478. Disable auto shift register inference for appropriately named nodes
      print "$prog: Adding wild-carded AUTO_SHIFT_REGISTER_RECOGNITION assignment to $qsf_file\n" if $verbose>1;
      print QSF_FILE "\nset_instance_assignment -name AUTO_SHIFT_REGISTER_RECOGNITION OFF -to *_NO_SHIFT_REG*\n";

      # allow for generate loops with bounds over 5000
      print QSF_FILE "\nset_global_assignment -name VERILOG_CONSTANT_LOOP_LIMIT 10000\n";

      if ($fast_compile) {
        # Adding fast-compile specific flags in *qsf
        open( QSF_FILE_READ, "<$qsf_file" ) or print "Couldn't open $qsf_file again - overwriting whatever INI_VARS are there\n";
        my $ini_vars = '';
        while( <QSF_FILE_READ> ) {
          if( $_ =~ m/INI_VARS\s+[\"|\'](.*)[\"|\']/ ) {
            $ini_vars = $1;
          }
        }
        close( QSF_FILE_READ );
        print QSF_FILE <<FAST_COMPILE_OPTIONS;
# The following settings were added by --fast-compile
# umbrella fast-compile setting
set_global_assignment -name OPTIMIZATION_TECHNIQUE Balanced
set_global_assignment -name OPTIMIZATION_MODE "Aggressive Compile Time"
FAST_COMPILE_OPTIONS

        my %new_ini_vars = (
        );
        if( $ini_vars ) {
          $ini_vars .= ";";
        }
        keys %new_ini_vars;
        while( my($k, $v) = each %new_ini_vars) {
          $ini_vars .= "$k=$v;";
        }
        if($ini_vars ne '') {
          print QSF_FILE "\nset_global_assignment -name INI_VARS \"$ini_vars\"\n";
        }
      }
      if ($high_effort_compile) {
        print QSF_FILE "\nset_global_assignment -name OPTIMIZATION_MODE \"High Performance Effort\"\n";
      }

      # Enable BBIC if doing incremental compile. 
      # Note that the FAST_OPENCL_COMPILE QSF is needed to enable BBIC on A10 (See HSD-ES 1409706116)
      if ($incremental_compile){
        print QSF_FILE <<INCREMENTAL_OPTIONS;
# Contents appended by -incremental flow
set_global_assignment -name FAST_OPENCL_COMPILE ON
set_global_assignment -name FAST_PRESERVE AUTO
INCREMENTAL_OPTIONS
      }

      if( scalar @additional_qsf ) {
        print QSF_FILE "\n$final_append\n";
      }

      close (QSF_FILE);
    }
  }

  # Set up for incremental change detection
  my $devicemodel = uc acl::Env::aocl_boardspec( "$board_spec_xml", "devicemodel");
  ($devicemodel) = $devicemodel =~ /(.*)_.*/;
  my $devicefamily = acl::AOCDriverCommon::device_get_family_no_normalization($devicemodel);
  my $run_change_detection = $incremental_compile && $incremental_input_dir ne "" &&
                             !acl::Incremental::requires_full_recompile($incremental_input_dir, $work_dir, $base, $all_aoc_args,
                                                                        acl::Env::board_name(), $board_variant, $devicemodel, $devicefamily,
                                                                        acl::AOCDriverCommon::get_quartus_version_str(), $prog,
                                                                        "20.3.0", "168.5");
  warn $acl::Incremental::warning if (defined $acl::Incremental::warning && !$quiet_mode);
  if ($incremental_compile && $run_change_detection) {
    $llc_arg_after .= " -incremental-input-dir=$incremental_input_dir -incremental-project-name=$base ";
  }

  my $optinfile = "$base.1.bc";

  if ($ecc_protected == 1){
    $llc_arg_after .= " -ecc ";
  }

  # Late environment check IFF we are using the emulator
  my $is_msvc_2015_or_later = acl::AOCDriverCommon::check_if_msvc_2015_or_later();

  my @cmd_list = ();


  if ( defined $program_hash ){ 
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.hash',$program_hash);
  }
  if ($emulator_flow) {
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board',$emulatorDevice);
  } elsif ($new_sim_mode) {
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board',"SimulatorDevice");
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.simulator_object',"");
  } else {
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board',$board_variant);
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.board_package',acl::Board_env::get_board_path());
  }

  # Store a random hash, and the inputs to quartus hash, in pkg. Should be added before quartus adds new HDL files to the working dir.
  acl::AOCDriverCommon::add_hash_sections($work_dir,$board_variant,$pkg_file,$all_aoc_args,$bsp_flow_name);

  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.compileoptions',join(' ',@user_opencl_args));
  # Set version of the compiler, for informational use.
  # It will be set again when we actually produce executable contents.
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.version',acl::Env::sdk_version());

  # TODO: Move get_file_list_from_dependency_files into create_report_tool when we consolidate the Profiler to use HTML GUI
  # Get a list of all source files from all the dependency files
  my @files = acl::Report::get_file_list_from_dependency_files(@all_dep_files);
  acl::AOCDriverCommon::remove_named_files(@all_dep_files) unless $save_temps;

  # do not enter to the work directory before this point, 
  # $pkg->add_file above may be called for files with relative paths
  chdir $work_dir or acl::Common::mydie("Can't change dir into $work_dir: $!");

  # the legacy emulator flow
  if ($emulator_flow) {
    if ($sycl_mode == 0 or $verbose > 0){
      print "$prog: Compiling for Emulation ....\n" if (!$quiet_mode);
    }

    # Link with standard library.
    my $emulator_lib = acl::File::abs_path( acl::Env::sdk_root()."/share/lib/acl/acl_emulation.bc");
    my $ocl_early_bc = acl::File::abs_path( acl::Env::sdk_root()."/share/lib/acl/acl_ocl_earlyx86.bc");
    @cmd_list = (
        @link_exe,
        $linked_objfile,
        @lib_bc_files,
        $emulator_lib,
        $ocl_early_bc,
        '-o',
        $optinfile );
    $return_status = acl::Common::mysystem_full(
        {'stdout' => "$work_dir/clang-link.log", 
         'stderr' => "$work_dir/clang-link.err",
         'time' => 1, 'time-label' => 'link (early)'},
        @cmd_list);
    my $banner = '!========== [link] early link ==========';
    acl::Common::move_to_log($banner, "$work_dir/clang-link.log", "$work_dir/$fulllog");
    acl::Report::append_to_err("$work_dir/clang-link.err");
    if ($return_status==0 or $regtest_mode==0) { unlink "$work_dir/clang-link.err"; }
    acl::AOCDriverCommon::remove_named_files($linked_objfile) unless $save_temps;

    foreach my $lib_bc_file (@lib_bc_files) {
      acl::AOCDriverCommon::remove_named_files($lib_bc_file) unless $save_temps;
    }
    
    if ($return_status != 0) {
     acl::Common::move_to_log($banner, "$work_dir/clang-link.err", "$work_dir/../$regtest_errlog") if $regtest_mode;
      acl::Common::mydie("OpenCL parser FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
    }

    my $debug_option = ( $debug ? '-debug' : '');
    my $opt_optimize_level_string = ($emu_optimize_o3) ? "-O3" : "";

    if ( !(($emu_ch_depth_model eq 'default' ) || ($emu_ch_depth_model eq 'strict') || ($emu_ch_depth_model eq 'ignore-depth')) ) {
      acl::Common::mydie("Invalid argument for option --emulator-channel-depth-model, must be one of <default|strict|ignore-depth>. \n");
    }

    $return_status = acl::Common::mysystem_full(
        {'time' => 1, 
         'time-label' => 'opt (opt (emulator tweaks))'},
         "$opt_exe -translate-library-calls -reverse-library-translation -insert-ip-library-calls -create-emulator-wrapper -generate-emulator-sys-desc -VPODirectiveCleanup $opt_optimize_level_string $llvm_library_option $llvm_hdlspec_option $debug_option $opt_arg_after \"$optinfile\" -o \"$base.bc\" >>$fulllog 2>opt.err" );
    acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
    $banner = '!========== [aocl-opt] Emulator specific messages ==========';
    acl::Common::move_to_log($banner, $fulllog);
    acl::Report::append_to_log('opt.err', $fulllog);
    acl::Report::append_to_err('opt.err');
    if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
    if ($return_status != 0) {
      if ($regtest_mode) {
        acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
      }
      acl::Common::mydie("Optimizer FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
    }
    acl::AOCDriverCommon::remove_named_files($optinfile) unless $save_temps;

    $pkg->set_file('.acl.llvmir',"$base.bc")
        or acl::Common::mydie("Can't save optimized IR into package file: $acl::Pkg::error\n");

    #Issue an error if autodiscovery string is larger than 4k (only for version < 15.1).
    my $bsp_version = acl::Env::aocl_boardspec( "$board_spec_xml", "version");
    if( (-s "sys_description.txt" > 4096) && ($bsp_version < 15.1) ) {
      acl::Common::mydie("System integrator FAILED.\nThe autodiscovery string cannot be more than 4096 bytes\n");
    }
    $pkg->set_file('.acl.autodiscovery',"sys_description.txt")
        or acl::Common::mydie("Can't save system description into package file: $acl::Pkg::error\n");

    my $arch_options = ();
    if ($emulator_arch eq 'windows64') {
      $arch_options = "-cc1 -triple x86_64-pc-win32 -emit-obj -o libkernel.obj";
    } else {
      $arch_options = "-fPIC -shared -Wl,-soname,libkernel.so -L\"$ENV{\"INTELFPGAOCLSDKROOT\"}/host/linux64/lib/\" -lacl_emulator_kernel_rt -lpsg_mpfr -lpsg_mpir -lhls_fixed_point_math_x86 -lhls_vpfp_library -o libkernel.so";
    }
    
    my $clang_optimize_level_string = ($emu_optimize_o3) ? '-O3' : '-O0';

    $return_status = acl::Common::mysystem_full(
        {'time' => 1, 
         'time-label' => 'clang (executable emulator image)'},
        "$clang_exe $arch_options $clang_optimize_level_string \"$base.bc\" >>$fulllog 2>opt.err" );
    acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
    $banner = '!========== [clang compile kernel emulator] Emulator specific messages ==========';
    acl::Common::move_to_log($banner, $fulllog);
    acl::Report::append_to_log('opt.err', $fulllog);
    acl::Report::append_to_err('opt.err');
    if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
    if ($return_status != 0) {
      if ($regtest_mode) {
        acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
      }
      acl::Common::mydie("Optimizer FAILED.\nRefer to $base/$fulllog for details.\n");
    }

    if ($emulator_arch eq 'windows64') {
      my $legacy_stdio_definitions = $is_msvc_2015_or_later ? 'legacy_stdio_definitions.lib' : '';
      $return_status = acl::Common::mysystem_full(
          {'time' => 1, 
           'time-label' => 'clang (executable emulator image)'},
          "link /DLL /EXPORT:__kernel_desc,DATA /EXPORT:__channels_desc,DATA /libpath:$ENV{\"INTELFPGAOCLSDKROOT\"}\\host\\windows64\\lib acl_emulator_kernel_rt.lib psg_mpfr.lib psg_mpir.lib hls_fixed_point_math_x86.lib hls_vpfp_library.lib msvcrt.lib $legacy_stdio_definitions libkernel.obj>>$fulllog 2>opt.err" );
      acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
      $banner = '!========== [Create kernel loadbable module] Emulator specific messages ==========';
      acl::Common::move_to_log($banner, $fulllog);
      acl::Report::append_to_log('opt.err', $fulllog);
      acl::Report::append_to_err('opt.err');
      if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
      if ($return_status != 0) {
        if ($regtest_mode) {
          acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
        }
        acl::Common::mydie("Linker FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
      }

      $pkg->set_file('.acl.emulator_object.windows',"libkernel.dll")
          or acl::Common::mydie("Can't save emulated kernel into package file: $acl::Pkg::error\n");
    } else {
      $pkg->set_file('.acl.emulator_object.linux',"libkernel.so")
        or acl::Common::mydie("Can't save emulated kernel into package file: $acl::Pkg::error\n");
    }

    if(-f "kernel_arg_info.xml") {
      $pkg->set_file('.acl.kernel_arg_info.xml',"kernel_arg_info.xml");
      unlink 'kernel_arg_info.xml' unless $save_temps;
    } else {
      print "Cannot find kernel arg info xml.\n" if $verbose;
    }

    my $compilation_env = acl::AOCDriverCommon::compilation_env_string($work_dir,$board_variant,$all_aoc_args,$bsp_flow_name);
    acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.compilation_env',$compilation_env);

    # Compute runtime.
    my $stage1_end_time = time();
    acl::Common::log_time ("emulator compilation", $stage1_end_time - $stage1_start_time);

    print "$prog: Emulator Compilation completed successfully.\n" if $verbose;
    &$finalize();
    return;
  } 

  # Link with standard library.
  my $early_bc = acl::File::abs_path( acl::Env::sdk_root()."/share/lib/acl/acl_early.bc");
  my $ocl_early_bc = acl::File::abs_path( acl::Env::sdk_root()."/share/lib/acl/acl_ocl_early.bc");
  @cmd_list = (
      @link_exe,
      $linked_objfile,
      @lib_bc_files,
      $early_bc,
      $ocl_early_bc,
      '-o',
      $optinfile );
  $return_status = acl::Common::mysystem_full(
      {'stdout' => "$work_dir/clang-link.log", 
       'stderr' => "$work_dir/clang-link.err",
       'time' => 1, 
       'time-label' => 'link (early)'},
      @cmd_list);
  my $banner = '!========== [link] early link ==========';
  acl::Common::move_to_log($banner, "$work_dir/clang-link.log", "$work_dir/$fulllog");
  acl::Report::append_to_err("$work_dir/clang-link.err");
  if ($return_status==0 or $regtest_mode==0) { unlink "$work_dir/clang-link.err"; }
  acl::AOCDriverCommon::remove_named_files($linked_objfile) unless $save_temps;
  foreach my $lib_bc_file (@lib_bc_files) {
    acl::AOCDriverCommon::remove_named_files($lib_bc_file) unless $save_temps;
  }
  if ($return_status != 0) {
    acl::Common::move_to_log($banner, "$work_dir/clang-link.err", "$work_dir/../$regtest_errlog") if $regtest_mode;
    acl::Common::mydie("OpenCL linker FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
  }


  chdir $work_dir or acl::Common::mydie("Can't change dir into $work_dir: $!");

  my $yaml_file = 'pass-remarks.yaml';
  my $disabled_lmem_replication = 0;
  my $opt_passes = " -march=fpga -O3";

  if ($griffin_flow) {
    # For the Griffin flow, we need to enable a few passes and change a few flags.
    $opt_arg_after .= " --soft-elementary-math=false ";
  }

  # For FPGA, we need to rewrite the triple.  Unfortunately, we can't do this in the regular -O3 opt, as
  # there are immutable passes (TargetLibraryInfo) that check the triple before we can run.  Run this
  # pass first as a standalone pass.  The alternate (better compile time) would be to run this as the last
  # part of clang, but that would also need changes to cllib.  See FB568473.
  my $triple_output = "$base.fpga.bc";
  $return_status = acl::Common::mysystem_full(
      {'time' => 1, 'time-label' => 'opt', 'stdout' => 'opt.log', 'stderr' => 'opt.err'},
      "$opt_exe -transform-fpga-reg -rewritetofpga \"$optinfile\" -o \"$triple_output\"");
  acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
  $banner = '!========== [opt] fpga ==========';
  acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog") if $regtest_mode;
  acl::Common::mydie("Unable to switch to FPGA triples\n") if $return_status != 0;
  acl::AOCDriverCommon::remove_named_files($optinfile) unless $save_temps;
  if ($disassemble) { acl::AOCDriverCommon::mysystem("llvm-dis \"$base.fpga.bc\" -o \"$base.fpga.ll\"" ) == 0 or acl::Common::mydie("Cannot disassemble: \"$base.bc\" \n"); }
  $optinfile = $triple_output;
  if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
  ## END FPGA Triple support.

  unlink "llvm_warnings.log"; # Prevent duplicate errors in report

  if ( $run_opt ) {
    acl::AOCDriverCommon::progress_message("$prog: Optimizing and doing static analysis of code...\n");
    my $debug_option = ( $debug ? '-debug' : '');
    my $profile_option = ( $profile ? ($profile_shared_counters ? "-profile $profile -shared-counters" : "-profile $profile") : '');
    my $opt_remarks_option = "-pass-remarks-output=$yaml_file";

    # Opt run
    $return_status = acl::Common::mysystem_full(
        {'time' => 1, 'time-label' => 'opt', 'stdout' => 'opt.log', 'stderr' => 'opt.err'},
        "$opt_exe $opt_passes $llvm_board_option $llvm_library_option $llvm_hdlspec_option $debug_option $profile_option $opt_arg_after $opt_remarks_option \"$optinfile\" -o \"$base.kwgid.bc\"");
    acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
    $banner = '!========== [opt] optimize ==========';
    acl::Common::move_to_log($banner, 'opt.log', $fulllog);
    acl::Report::append_to_log('opt.err', $fulllog);
    acl::Report::append_to_err('opt.err');
    acl::Report::append_to_log('opt.err', 'llvm_warnings.log');
    if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }

    if ($return_status != 0) {
      if ($regtest_mode) {
        acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
      }
      acl::Report::move_to_err('opt.err');
      # If we errored out because the area estimates were very large, then
      # create the report so users can see what used so much area.
      acl::Report::create_minimal_report($fulllog, $prog, $work_dir,
        \&_create_opencl_reporting_tool, $base, $all_aoc_args,
        $board_variant, $disabled_lmem_replication, $devicemodel, $devicefamily,
        \@files);
      acl::Common::mydie("Optimizer FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
    }

    if ( $use_ip_library && $use_ip_library_override ) {
      print "$prog: Linking with IP library ...\n" if $verbose;
      # Lower instructions to IP library function calls
      $return_status = acl::Common::mysystem_full(
          {'time' => 1, 'time-label' => 'opt (ip library prep)', 'stdout' => 'opt.log', 'stderr' => 'opt.err'},
          "$opt_exe -insert-ip-library-calls $opt_arg_after \"$base.kwgid.bc\" -o \"$base.lowered.bc\"");
      acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
      $banner = '!========== [opt] ip library prep ==========';
      acl::Common::move_to_log($banner, 'opt.log', $fulllog);
      acl::Report::append_to_log('opt.err', $fulllog);
      acl::Report::append_to_err('opt.err');
      acl::Report::append_to_log('opt.err', 'llvm_warnings.log');      
      if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
      if ($return_status != 0) {
        if ($regtest_mode) {
          acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
        }
        acl::Report::move_to_err('opt.err');
        acl::Common::mydie("Optimizer FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
      }
      acl::AOCDriverCommon::remove_named_files("$base.kwgid.bc") unless $save_temps;

      # Link with the soft IP library 
      my $late_bc = acl::File::abs_path( acl::Env::sdk_root()."/share/lib/acl/acl_late.bc");
      @cmd_list = (
        @link_exe,
        "$base.lowered.bc",
        $late_bc,
        '-o',
        "$base.linked.bc" );
      $return_status = acl::Common::mysystem_full(
          {'time' => 1,
           'time-label' => 'link (ip library)',
           'stdout' => 'opt.log',
           'stderr' => 'opt.err'},
          @cmd_list);
      acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
      $banner = '!========== [link] ip library link ==========';
      acl::Common::move_to_log($banner, 'opt.log', $fulllog);
      acl::Report::append_to_log('opt.err', $fulllog);
      acl::Report::append_to_err('opt.err');
      acl::Report::append_to_log('opt.err', 'llvm_warnings.log');
      if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
      if ($return_status != 0) {
        if ($regtest_mode) {
          acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
        }
        acl::Report::move_to_err('opt.err'); 
        acl::Common::mydie("Optimizer FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
      }
      acl::AOCDriverCommon::remove_named_files("$base.lowered.bc") unless $save_temps;

      # Inline IP calls, simplify and clean up
      $return_status = acl::Common::mysystem_full(
          {'time' => 1, 'time-label' => 'opt (ip library optimize)', 'stdout' => 'opt.log', 'stderr' => 'opt.err'},
          "$opt_exe $llvm_board_option $llvm_library_option $llvm_hdlspec_option $debug_option -always-inline -phase-3-inst-simplify -dce -stripnk -rename-basic-blocks $opt_arg_after \"$base.linked.bc\" -o \"$base.bc\"");
      acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
      $banner = '!========== [opt] ip library optimize ==========';
      acl::Common::move_to_log($banner, 'opt.log', $fulllog);
      acl::Report::append_to_log('opt.err', $fulllog);
      acl::Report::append_to_err('opt.err');
      acl::Report::append_to_log('opt.err', 'llvm_warnings.log');
      if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
      if ($return_status != 0) {
        if ($regtest_mode) {
          acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
        }
        acl::Report::move_to_err('opt.err'); 
        acl::Common::mydie("Optimizer FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
      }
      acl::AOCDriverCommon::remove_named_files("$base.linked.bc") unless $save_temps;
    } else {
      # In normal flow, lower the acl kernel workgroup id last
      $return_status = acl::Common::mysystem_full(
          {'time' => 1, 'time-label' => 'opt (post-process)', 'stdout' => 'opt.log', 'stderr' => 'opt.err'},
          "$opt_exe $llvm_board_option $llvm_library_option $llvm_hdlspec_option $debug_option \"$base.kwgid.bc\" -o \"$base.bc\"");
      acl::Report::filter_llvm_time_passes("opt.err", $time_passes);
      $banner = '!========== [opt] post-process ==========';
      acl::Common::move_to_log($banner, 'opt.log', $fulllog);
      acl::Report::append_to_log('opt.err', $fulllog);
      acl::Report::append_to_err('opt.err');
      acl::Report::append_to_log('opt.err', 'llvm_warnings.log');      
      if ($return_status==0 or $regtest_mode==0) { unlink 'opt.err'; }
      if ($return_status != 0) {
        if ($regtest_mode) {
          acl::Common::move_to_log($banner, 'opt.err', "$work_dir/../$regtest_errlog");
        }
        acl::Report::move_to_err('opt.err'); 
        acl::Common::mydie("Optimizer FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
      }
      acl::AOCDriverCommon::remove_named_files("$base.kwgid.bc") unless $save_temps;
    }
  
    # Finish up opt-like steps.
    if ( $disassemble ) { acl::AOCDriverCommon::mysystem("llvm-dis \"$base.bc\" -o \"$base.ll\"" ) == 0 or acl::Common::mydie("Cannot disassemble: \"$base.bc\" \n"); }
    if ( $pkg_save_extra ) {
      $pkg->set_file('.acl.llvmir',"$base.bc")
         or acl::Common::mydie("Can't save optimized IR into package file: $acl::Pkg::error\n");
    }
    if ( $opt_only ) { return; }
  }

  if ( $run_verilog_gen ) {
    my $debug_option = ( $debug ? '-debug' : '');
    my $profile_option = ( $profile ? "-profile $profile" : '');
    $profile_option = ( ($profile && $profile_shared_counters) ? "-profile $profile -shared-counters" : $profile_option);
    my $llc_remarks_option = "-pass-remarks-input=$yaml_file";

    my $dependency_files_list_filename = "temp_dependencies_list.txt.temp";
    if ( $profile ) {
      open (DEPENDENCY_FILES, "> $dependency_files_list_filename");
      foreach my $file (@files) {
        # "Unknown" files are included when opaque objects (such as image objects) are in the source code
        if ($file =~ m/\<unknown\>$/ or $file =~ m/$ocl_header_filename$/) {
          next;
        }
        print DEPENDENCY_FILES "$file\n";
      }
      close DEPENDENCY_FILES;
      $llc_arg_after .= " --dependendency-list-file=$dependency_files_list_filename ";
    }

    # Run LLC
    $return_status = acl::Common::mysystem_full(
        {'time' => 1, 'time-label' => 'llc', 'stdout' => 'llc.log', 'stderr' => 'llc.err'},
        "$llc_exe -march=fpga $llvm_board_option $llvm_library_option $llvm_hdlspec_option $llvm_profilerconf_option $debug_option $profile_option $llc_remarks_option $llc_arg_after \"$base.bc\" -o \"$base.v\"");
    acl::Report::filter_llvm_time_passes("llc.err", $time_passes);
    unlink $dependency_files_list_filename;
    $banner = '!========== [llc] ==========';
    acl::File::copy('llc.err','llc.err.tmp'); #make a copy of llc.err to stop it from being deleted by move_to_log since llc.err is being used after the move_to_log as well
    acl::Common::move_to_log($banner, 'llc.log', 'llc.err.tmp', $fulllog);
    if ($return_status != 0) {
      acl::Report::append_to_err('llc.err');
      acl::Report::append_to_log('llc.err', 'llvm_warnings.log');
      open (LOG, "<$fulllog");
      while (defined(my $line = <LOG>)) {
        print $win_longpath_suggest if (acl::AOCDriverCommon::win_longpath_error_llc($line) and acl::Env::is_windows());
      }
      close LOG;
      # If we errored out because the area estimates were very large, then
      # create the report so users can see what used so much area.
      acl::Report::create_minimal_report($fulllog, $prog, $work_dir,
        \&_create_opencl_reporting_tool, $base, $all_aoc_args,
        $board_variant, $disabled_lmem_replication, $devicemodel, $devicefamily,
        \@files);
      if ($regtest_mode) {
        acl::Common::move_to_log($banner, 'llc.err', "$work_dir/../$regtest_errlog");
      } else {
        unlink('llc.err');
      }
      acl::Common::mydie("Verilog generator FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
    }
    #llc has been run already, so the yaml file used for reporting is not needed anymore. Cleanup
    acl::AOCDriverCommon::remove_named_files($yaml_file) unless $save_temps;

    # If estimate > $max_mem_percent_with_replication of block ram, emit warning to user and continue
    print "$prog: Checking if memory usage is larger than $max_mem_percent_with_replication%...\n" if $verbose;
    my $area_rpt_file_path = $work_dir."/area.json";
    if (-e $area_rpt_file_path) {
      my @area_util = acl::AOCDriverCommon::get_area_percent_estimates();
      if ( $area_util[3] > $max_mem_percent_with_replication ) {
        print "$prog: Warning RAM Utilization is at $area_util[3]%!\n";
      } else {
        print "$prog: Memory usage is not above $max_mem_percent_with_replication.\n" if $verbose;
      } 

    } else {
      print "$prog: Cannot find area.json.\n";
    }
  }


  foreach my $qsf_file (acl::File::simple_glob( "$work_dir/*.qsf" )) {
      open (QSF_FILE, ">>$qsf_file") or die "Couldn't open $qsf_file for append!\n";
    # Workaround fix for ECC, since the memory init files will be instantiated through 
    # a wrapper, in the ip directory, causing the synthesis failing to find the .hex 
    # files for some reason. (FB:544479)
    if ($ecc_protected) {
      foreach my $dir ( acl::File::simple_glob( "$work_dir/kernel_hdl/*") ) {
        if (!(-d $dir)) {
          next;
        }
        $dir=~ s/$work_dir\///e; # Make it a relative path
        print QSF_FILE "set_global_assignment -name SEARCH_PATH \"$dir\"\n";
      }
    }
    close (QSF_FILE);
  }
  
  acl::AOCDriverCommon::remove_named_files($optinfile) unless $save_temps;

  #Put after loop so we only store once
  if ( $pkg_save_extra ) {
    $pkg->set_file('.acl.verilog',"$base.v")
      or acl::Common::mydie("Can't save Verilog into package file: $acl::Pkg::error\n");
  }

  # Save the profile XML file in the aocx
  if ( $profile ) {
    acl::AOCDriverCommon::save_profiling_xml($pkg,$base);
  }

  # Move over the Optimization Report to the log file
  if ( -e "opt.rpt" ) {
    acl::Report::append_to_log( "opt.rpt", $fulllog );
    unlink "opt.rpt" unless $save_temps;
  }

  unlink "report.out";
  if ( $estimate_throughput ) {
      print "Estimating throughput since \$estimate_throughput=$estimate_throughput\n";
    $return_status = acl::Common::mysystem_full(
        {'time' => 1, 'time-label' => 'opt (throughput)', 'stdout' => 'report.out', 'stderr' => 'report.err'},
        "$opt_exe -print-throughput -throughput-print $llvm_board_option $opt_arg_after \"$base.bc\" -o $base.unused" );
    acl::Report::filter_llvm_time_passes("report.err", $time_passes);
    acl::AOCDriverCommon::move_to_err_and_log("Throughput analysis","report.err",$fulllog);
  }
  unlink "$base.unused";

  # Guard probably deprecated, if we get here we should have verilog, was only used by vfabric
  if ( $run_verilog_gen) {
    open LOG, ">>report.out";
    close LOG;
    acl::Report::append_to_log ("report.out", $fulllog);
  }
  if ($report) {
    open LOG, "<report.out";
    print STDOUT <LOG>;
    close LOG;
  }
  unlink "report.out" unless $save_temps;

  if ($save_last_bc) {
    $pkg->set_file('.acl.profile_base',"$base.bc")
      or acl::Common::mydie("Can't save profiling base listing into package file: $acl::Pkg::error\n");
  }
  acl::AOCDriverCommon::remove_named_files("$base.bc") unless $save_temps or $save_last_bc;

  my $xml_file = "$base.bc.xml";
  my $sysinteg_debug .= ($debug ? "-v" : "" );

  if ($run_change_detection) {
    #pass previous _sys.v file as an argument to system integrator. used for incremental compile change detection
    my $prev_sysv = $base."_sys.v";
    $sysinteg_arg_after .= " --incremental-previous-systemv $incremental_input_dir/$prev_sysv";
    #also add previous bc.xml file as system integrator argument
    $sysinteg_arg_after .= " --incremental-previous-bcxml $incremental_input_dir/$xml_file";
    #add partition grouping file written out by system integrator during the previous compile
    $sysinteg_arg_after .= " --incremental-previous-partition-grouping $incremental_input_dir/previous_partition_grouping_incremental.txt";
  }
  # Simulation BFM mode does not support snooping on host transfers
  if($new_sim_mode  && $skip_qsys==0) {
    $sysinteg_arg_after .= " --cic-const-cache-disable-snoop ";
  }
  # Add what are we compiling OpenCL or SYCL
  $sysinteg_arg_after .= " --sycl $sycl_mode";

  my $version = ::acl::Env::aocl_boardspec( ".", "version");
  my $generic_kernel = ::acl::Env::aocl_boardspec( ".", "generic_kernel".$bsp_flow_name);
  my $qsys_file = ::acl::Env::aocl_boardspec( ".", "qsys_file".$bsp_flow_name);
  ( $generic_kernel.$qsys_file !~ /error/ ) or acl::Common::mydie("BSP compile-flow $bsp_flow_name not found\n");
  
  my $system_script = ($qsys_file eq "none") ? "none" : "system.tcl";
  my $kernel_system_script = (($new_sim_mode) or ($qsys_file ne "none") or ($bsp_version < 18.0)) ? "kernel_system.tcl" : "";
  my $sysinteg_arg_scripts = $system_script . " " . $kernel_system_script;

  #remove warning log generated by previous system integrator execution, if any.
  #this will prevent duplicated warning/error msg
  unlink "system_integrator_warnings.log";
  if ( $generic_kernel or ($version eq "0.9" and -e "base.qsf")) {
    $return_status = acl::Common::mysystem_full(
      {'time' => 1, 'time-label' => 'system integrator', 'stdout' => 'si.log', 'stderr' => 'si.err'},
      "$sysinteg_exe $sysinteg_debug $sysinteg_arg_after \"$board_spec_xml\" \"$xml_file\" $sysinteg_arg_scripts" );
  } else {
    if ($qsys_file eq "none") {
      acl::Common::mydie("A board with 'generic_kernel' set to \"0\" and 'qsys_file' set to \"none\" is an invalid combination in board_spec.xml! Please revise your BSP for errors!\n");  
    }
    $return_status = acl::Common::mysystem_full(
      {'time' => 1, 'time-label' => 'system integrator', 'stdout' => 'si.log', 'stderr' => 'si.err'},
      "$sysinteg_exe $sysinteg_debug $sysinteg_arg_after \"$board_spec_xml\" \"$xml_file\" system.tcl" );
  }
  $banner = '!========== [SystemIntegrator] ==========';
  acl::Common::move_to_log($banner, 'si.log', $fulllog);
  acl::Report::append_to_log('si.err', $fulllog);
  acl::Report::append_to_err('si.err');
  if ($return_status==0 or $regtest_mode==0) { unlink 'si.err'; }
  if ($return_status != 0) {
    if ($regtest_mode) {
      acl::Common::move_to_log($banner, 'si.err', "$work_dir/../$regtest_errlog");
    }
    acl::Common::mydie("System integrator FAILED.\nRefer to ".acl::File::mybasename($work_dir)."/$fulllog for details.\n");
  }

  #Issue an error if autodiscovery string is larger than 4k (only for version < 15.1).
  if( (-s "sys_description.txt" > 4096) && ($bsp_version < 15.1) ) {
    acl::Common::mydie("System integrator FAILED.\nThe autodiscovery string cannot be more than 4096 bytes\n");
  }
  $pkg->set_file('.acl.autodiscovery',"sys_description.txt")
    or acl::Common::mydie("Can't save system description into package file: $acl::Pkg::error\n"); 

  if(-f "board_spec.xml") {
    $pkg->set_file('.acl.board_spec.xml',"board_spec.xml")
      or acl::Common::mydie("Can't save boardspec.xml into package file: $acl::Pkg::error\n");
  } else {
     print "Cannot find board spec xml\n";
  } 

  if(-f "kernel_arg_info.xml") {
    $pkg->set_file('.acl.kernel_arg_info.xml',"kernel_arg_info.xml");
    unlink 'kernel_arg_info.xml' unless $save_temps;
  } else {
     print "Cannot find kernel arg info xml.\n" if $verbose;
  }

  my $report_time = time();
  my $proj_name =
    $sycl_mode
    ? ( split /\./, acl::File::mybasename($change_output_folder_name) )[0]
    : $base;
  _create_opencl_reporting_tool($proj_name, $all_aoc_args, $board_variant, $disabled_lmem_replication, $devicemodel, $devicefamily, \@files);
  # Prepare data for Quartus data to show in report.html. Note that this is not needed for fast-compile, but it's output for consistency reason.
  # get_kernel_list must be invoke in create_system so the $base argument is same with LLC which creates the .bc.xml file
  # TODO: Add has_2x_clock, channels/pipes, sysROM, Global interconnect for more accurate fitter result correlations
  my @kernel_list = acl::AOCDriverCommon::get_kernel_list($base);
  # file name and proc name need to match with quartus_resource_json_writer.tcl
  open( my $kl_fh, ">kernel_report.tcl" ) or acl::Common::mydie("Unexpected Compiler Error, not create metadata for report.");
  print $kl_fh "proc getEntityNames {} {\n";
  print $kl_fh "  return [list " . join(" ", ("kernel_system", @kernel_list)) . "]\n";
  print $kl_fh "}";
  close $kl_fh;
  acl::Common::log_time("Generate static reports", time()-$report_time);

  # Get '.acl.target' from .aoco file, check if it is fpga, and save the target into .aocr file
  my $temp_obj = $objfile_list[0];
  my $obj_pkg = get acl::Pkg($temp_obj) or die "Can't find pkg file $temp_obj: $acl::Pkg::error\n";   
  my $obj_target = acl::AOCDriverCommon::get_pkg_section($obj_pkg,'.acl.target');
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.target', $obj_target);
  
  my $compilation_env = acl::AOCDriverCommon::compilation_env_string($work_dir,$board_variant,$all_aoc_args,$bsp_flow_name);
  acl::AOCDriverCommon::save_pkg_section($pkg,'.acl.compilation_env',$compilation_env);
  if ($sycl_mode == 0 or $verbose > 0){
    print "$prog: First stage compilation completed successfully.\n" if (!$quiet_mode); 
  }
  # Compute aoc runtime WITHOUT Quartus time or integration, since we don't control that
  my $stage1_end_time = time();
  acl::Common::log_time ("first compilation stage", $stage1_end_time - $stage1_start_time);

  if ($incremental_compile && -e "prev") {
    acl::File::remove_tree("prev")
      or acl::Common::mydie("Cannot remove files under temporary directory prev: $!\n");
  }

  # Generate an RTL hash section for caching SYCL compiles and avoiding Quartus recompiles.
  acl::AOCDriverCommon::add_rtl_hash_section($work_dir, $pkg_file);

  if ( $verilog_gen_only ) { return; }

  &$finalize();
}

sub _create_opencl_reporting_tool {
  my $base = shift;
  my $all_aoc_args = shift;
  my $board_variant = shift;
  my $disabled_lmem_repl = shift;
  my $devicemodel = shift;
  my $devicefamily = shift;
  my $files = shift;

  my @log_files = ("llvm_warnings.log", "system_integrator_warnings.log");
  # Create clang_warnings file
  if (open(OUTPUT,">", "clang_warnings.log")) {
    unshift @log_files, "clang_warnings.log";
    foreach my $line (@clang_warnings) {
      print OUTPUT "$line";
    }
    close OUTPUT;
  }

  (my $mProg = $prog) =~ s/#//g;
  my @patterns_to_skip = ($ocl_header_filename);
  my $prog_name = ($sycl_mode == 1) ? 'SYCL' : 'AOC';
  acl::Report::create_reporting_tool($prog_name, ".", $base, 
                                     $devicefamily, $devicemodel, acl::AOCDriverCommon::get_quartus_version_str(), $mProg." ".$all_aoc_args, $board_variant,
                                     \@log_files, $disabled_lmem_repl,
                                     $files, \@patterns_to_skip, $dash_g);

  # Clean up
  acl::AOCDriverCommon::remove_named_files(@log_files);
}
