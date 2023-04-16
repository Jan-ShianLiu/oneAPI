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


=cut

package acl::Report_reader;
require acl::Common;
require JSON::PP;

our @ISA        = qw(Exporter);
our @EXPORT     = qw( parse_loop_attribute_viewer
                      parse_loops_report
                      get_all_loop_attr_objs
                      get_all_loops
                      get_target_fmax
                      is_a_function
                      get_loop_function
                      get_loop_parent
                      get_subloops
                      is_loop_fully_unrolled
                      is_loop_pipelined
                      get_loop_ii
                      get_achieved_fmax
                      get_loop_interleaving
                      get_loop_trip_count
                      get_loop_threshold_no_delay
                      get_latency
                      parse_memory_view
                      get_all_aggregates
                      is_optimized_away
                      is_a_register
                      is_a_rom
                      get_unroll_copy
                      get_inline_copy
                      parse_resource_utilization
                      get_family_device
                      get_quartus_version
                      get_command
                      get_version
                      parse_info
                      parse_warning
                      parse_bottleneck_viewer
                      get_bottleneck_cause
                      get_bottleneck_concurrency
                      get_node_ids
                      get_node
                      get_predecessor_ids
                      get_successor_ids
                      parse_simulation_raw
                      get_clock_period
                      get_simulation_time
                      get_invocation_start_time
                      get_invocation_end_time
                      get_invocation_latency_time
                      is_component_invocation_enqueued
                      get_invocation_data
                      get_name
                      get_id
                      get_type
                      get_debug_object
                      get_debug_filename
                      get_debug_line_number
                      get_details_object
                      get_details_info
                  );
our @EXPORT_OK  = qw( parse_details
                      try_to_parse
                    );
our $VERSION = '1';

use strict;

=head1 NAME

acl::Report_reader - Report reader utilities

=head1 VERSION

$Header: //depot/sw/hld/rel/20.3api.11/acl/sysgen/lib/acl/Report_reader.pm#1 $

=head1 SYNOPSIS

  # Example: Get all the component names
  my @func_list = acl::Report_reader::parse_loops_report("loops.json");
  foreach my $func (@func_list) {
    print acl::Report_reader::get_name($func)."\n";
  }

  # Exampe: Get the loop II and debug locations

=head1 DESCRIPTION

This module provides utilities for anyone who wishes to parse the report json files.

All methods names may optionally be imported, e.g.:

   use acl::Report_reader qw( parse_loops_report );

=head1 METHODS

=cut 

# ==== Public utility functions ==== #

# ==== Generic for any reports ==== #

# Returns the name of the object, i.e. loop, dependency variable
sub get_name ($) {
  my $my_obj = shift;
  return $my_obj->{"name"};
}

sub get_id ($) {
  my $my_obj = shift;
  return $my_obj->{"id"};
}

sub get_type ($) {
  my $my_obj = shift;
  return $my_obj->{"type"};
}

# Returns a 2D array of debug locations object
# The first dimension is to keep track of inline functions
# The second dimention is to keep track of multipe debug locations
sub get_debug_object ($) {
  my $my_obj = shift;
  return @{$my_obj->{"debug"}};
}

# Returns the file name
sub get_debug_filename ($) {
  my $dbg_hash = shift;
  return $dbg_hash->{"filename"};
}

# Returns the line number
sub get_debug_line_number ($) {
  my $dbg_hash = shift;
  return $dbg_hash->{"line"};
}

# Returns the details object
sub get_details_object ($) {
  my $my_obj = shift;
  return $my_obj->{"details"};
}

# Returns the value of the detail info if info is defined
sub get_details_info($$) {
  my ($details, $info) = @_;
  return $details->{$info} if (defined($details->{$info}));
  return undef;
}

# TODO: may need to add get_type() since is_a_function() currently doesn't support global memory

# Return 0 for Kernel or component
# 1 if it's a function, applicable for HLS only.
# 2 if global/system memory
sub is_a_function($) {
  my $func_obj = shift;
  return $func_obj->{"type"};
}


# ==== helper functions ==== #

# parse a detail section and return a detail object
# only handles details that is in a table format
sub parse_details(@) {
  my ($details_list) = @_;
  my $details_obj = {};
  foreach my $d (@{$details_list}) {
    my %info_dict = %$d;
    foreach my $d_key (keys %info_dict) {
      # parser does not support properties of the details
      next if ($d_key eq "type" || $d_key eq "Reference");

      $details_obj->{$d_key} = $info_dict{$d_key};
    }
    last;
  }
  return $details_obj;
}

# Generic ACL JSON to string function
# return 0 when can't open the file instead of die because this parsing is used everywhere in the driver
sub try_to_parse($) {
  my $json_file = shift;
  open(my $json_fh, "<${json_file}") or return 0;
  my $json_txt = do {
    local $/;
    <$json_fh>
  };
  close $json_fh;
  
  my $json_to_perl;
  eval {
    $json_to_perl = JSON::PP::decode_json($json_txt);
  };
  if ($@ or !($json_to_perl)) {
    # deal with bad JSON format
    return undef;
  }
  my %ret = undef;
  eval {
    %ret = %{$json_to_perl};
  };
  if ($@ or !(%ret)) {
    return undef;
  } else {
    return %ret;
  }
}

# ==== Info specific  ==== #

sub get_family_device($) {
  my $my_obj = shift;
  return $my_obj->{"family"};
}

sub get_quartus_version($) {
  my $my_obj = shift;
  return $my_obj->{"quartus"};
}

sub get_command($) {
  my $my_obj = shift;
  return $my_obj->{"command"} if (defined($my_obj->{"command"}));
  return 0;
}

sub get_version($) {
  my $my_obj = shift;
  return $my_obj->{"version"};
}

=head2 parse_info($warning_json_file)

Returns the info object if exist and the API above can be used to get the individual information.

=cut

sub parse_info($) {
  my $info_json_file = shift;
  my %info_dict = try_to_parse($info_json_file);
  if (defined($info_dict{"compileInfo"}) && defined($info_dict{"compileInfo"}->{"nodes"})) {
    my @info_list = @{$info_dict{"compileInfo"}->{"nodes"}};
    foreach my $info_obj (@info_list) {
      return $info_obj;
    }
  }
  return 0;
}

# ==== Warning specific  ==== #

=head2 parse_warning($warning_json_file)

Returns a list warnings objects: { "name" => <function name>, {"debug"} => <debug obj> }

=cut

sub parse_warning($) {
  my $warning_json_file = shift;
  my %warning_dict = try_to_parse($warning_json_file);

  # JSON syntax error
  acl::Common::mydie("Failed to parse $warning_json_file due to syntax error.") if (!%warning_dict);

  return $warning_dict{"nodes"} if (defined($warning_dict{"nodes"}));

  return 0;
}

# ==== Loop specific  ==== #

=head2 get_target_fmax($func_obj)

Returns the target fmax of the function in MHz.

=cut

sub get_target_fmax {
  my $my_loop = shift;
  return $my_loop->{"target_fmax"};
}

=head2 get_loop_function($loop_obj)

Returns the Kernel or Component name the loop belongs to.

=cut

sub get_loop_function ($) {
  my $my_loop = shift;
  return $my_loop->{"function"} if (defined($my_loop->{"function"}));
  return undef;  # This should not happen
}

=head2 get_loop_parent($loop_obj)

Returns the a pointer to parent loop or undef if this is the outer most loop.

=cut

sub get_loop_parent ($) {
  my $my_loop = shift;
  if (defined($my_loop->{"parent"})) {
    return $my_loop->{"parent"};
  };
  return undef;
}

=head2 get_subloops($loop_obj)

Returns an array of pointers to subloop object or an empty array if this is the
most inner loop.

=cut

sub get_subloops ($) {
  my $my_loop = shift;
  if (defined($my_loop->{"subloops"})) {
    return @{$my_loop->{"subloops"}};
  }
  return ();
}

=head2 is_loop_fully_unrolled($loop_obj)

Returns 1 if loop is fully unrolled, 0 otherwise.

=cut

sub is_loop_fully_unrolled ($) {
  my $my_loop = shift;
  return $my_loop->{"fully_unrolled"} if (defined($my_loop->{"fully_unrolled"}));
  return undef;  # This should not happen
}

=head2 is_loop_hls_component_invocation($loop_obj)

Returns 1 if loop is the HLS component invocation loop, 0 for not. Undefined for incorrect object.

=cut

sub is_loop_hls_component_invocation {
  my $my_loop = shift;
  return $my_loop->{"hls_comp_invo"};
}

=head2 is_loop_pipelined($loop_obj)

Returns 1 for loop is pipelined, 0 for not pipelined. Undefind when this is a loop
in NDRange kernel or fully unrolled.

=cut

sub is_loop_pipelined ($) {
  my $my_loop = shift;
  return $my_loop->{"pipelined"};
}

=head2 get_loop_ii($loop_obj)

Returns the initiation interval (II) of the loop. Undefined if the loop is not pipelined.

=cut

sub get_loop_ii ($) {
  my $my_loop = shift;
  return $my_loop->{"ii"};
}

=head2 get_achieved_fmax($loop_obj)

Returns the achieved fmax of the block in MHz. Sometimes this is known as scheduled fmax.

=cut

sub get_achieved_fmax {
  my $my_loop = shift;
  return $my_loop->{"achieved_fmax"};
}

=head2 get_loop_interleaving($loop_obj)

Returns the max interleaving of the outer loop on this loop. Undefined if the loop is not pipelined.

=cut

sub get_loop_interleaving {
  my $my_loop = shift;
  return $my_loop->{"interleaving"};
}


=head2 get_loop_trip_count($loop_obj)

Returns the trip count of this loop. "n/a" if not a loop header.

=cut

sub get_loop_trip_count {
  my $my_loop = shift;
  return $my_loop->{"trip_count"};
}

=head2 get_loop_threshold_no_delay($loop_obj)

Returns whether this loop has a delay due to limiters.

=cut

sub get_loop_threshold_no_delay {
  my $my_loop = shift;
  return $my_loop->{"threshold_no_delay"};
}

=head2 get_latency($loop_obj)

Returns the latency of the block in clock cycles.

=cut

sub get_latency {
  my $my_loop = shift;
  return $my_loop->{"latency"};
}

sub get_loop_speculation ($) {
  my $my_loop = shift;
  return $my_loop->{"speculation"};
}

sub get_loop_layer {
  my $my_loop = shift;
  return $my_loop->{"layer"};
}

=head2 get_loop_bottleneck($loop_obj)

To be obsoleted. Use parse_bottleneck_viewer for bottleneck informaton.
Returns the type of bottleneck or undef when the loop has no bottleneck.

=cut

sub get_loop_bottleneck ($) {
  my $my_loop = shift;
  if (exists($my_loop->{"bottleneck"})) {
    return $my_loop->{"bottleneck"}->{"type"};
  }
  return undef;
}

# ==== Tree and Table viewer specific  ==== #
=head2 parse_loop_attribute_viewer($json_file_path)

Returns a list of functions that's in the data structure that can be used
with public API's.
Returns an empty list if fails to parse or file does not exist.

=cut

# Returns a list of functions
# Each loop is stored in the specified data structure below which can be access
# using methods in the public methods above
# a list of functions:
#   {"name"}            = <function name>
#   {"type"}            = <0|1>  # 0 means kernel or component
#   {"debug"}           = (no change)
#   {"loops"}           = ()     # list of loop
sub parse_loop_attribute_viewer {
  my $loop_attr_json_file = shift;
  if (-e $loop_attr_json_file) {
    my %loop_attr_hash = try_to_parse($loop_attr_json_file);
    if (exists($loop_attr_hash{"name"}) && 
        defined($loop_attr_hash{"name"}) && 
        $loop_attr_hash{"name"} eq "loop_attributes" && 
        defined($loop_attr_hash{"nodes"})) {
      # organize the data
      my @func_list;
      foreach my $func_obj (@{$loop_attr_hash{"nodes"}}) {
        my $func_info_obj = _parse_global_properties($func_obj);
        $func_info_obj->{"target_fmax"} = $func_obj->{"fmax"};
        my $func_name = get_name($func_info_obj);
        my @loops_list = @{$func_obj->{"children"}};
        if (scalar(@loops_list) > 0) {
          $func_info_obj->{"loops"} = ();
          _get_loop_attribute_recur(\@loops_list, $func_name, undef, \@{$func_info_obj->{"loops"}});
        }
        push(@func_list, $func_info_obj);
      }
      return @func_list;
    }
  }
  return [];
}

=head2 parse_loops_report($json_file_path)

To be obsoleted. Only use this to check fully unrolled loops.
Use parse_loop_attribute_viewer for loop pipeline informaton
Use parse_bottleneck_viewer for bottleneck informaton.
Returns a data structure that can be used with public API's

=cut

# Returns a list of functions
# Each loop is stored in the specified data structure below which can be access
# using methods in the public methods above
# a list of functions:
#   {"name"}            = <function name>
#   {"type"}            = <0|1>  # 0 means kernel or component
#   {"debug"}           = (no change)
#   {"loops"}           = ()     # list of loop
sub parse_loops_report ($) {
  my $loop_json_file = shift;
  my %loops_hash = try_to_parse($loop_json_file);

  # JSON syntax error
  acl::Common::mydie("Failed to parse $loop_json_file due to syntax error.") if (!%loops_hash);

  # organize the data
  my @func_list;
  foreach my $func_obj (@{$loops_hash{"children"}}) {
    # TODO: commonize $func_info_obj with _parse_global_properties()
    my $func_info_obj;
    $func_info_obj->{"name"} = $func_obj->{"name"};
    $func_info_obj->{"debug"} = $func_obj->{"debug"};
    my $func_name = $func_info_obj->{"name"};
    $func_info_obj->{"type"} = ($func_name =~ /Kernel|Component/i) ? 0 : 1;

    my @loops_list = @{$func_obj->{"children"}};
    if (scalar(@loops_list) > 0) {
      $func_info_obj->{"loops"} = ();
      get_loop_info_recur(\@loops_list, undef, $func_name, \@{$func_info_obj->{"loops"}});
    }
    push(@func_list, $func_info_obj);
  }
  return @func_list;
}

=head2 _get_loop_objs_recur(@loop object, $loops list)

# Helper for get_all_loop_attr_objs

=cut

sub _get_loop_objs_recur {
  my ($loop_obj, $loops_list) = @_;

  push(@{$loops_list}, $loop_obj);

  if (defined($loop_obj->{"subloops"})) {
    foreach my $subloop_obj (@{$loop_obj->{"subloops"}}) {
      _get_loop_objs_recur($subloop_obj, \@{$loops_list});
    }
  }
  return;
}

=head2 get_all_loop_attr_objs(@functions_list)

# Returns all the instances of loop in an array in the design (loop attributes version)

=cut

sub get_all_loop_attr_objs ($@) {
  my ($func_list) = @_; 
  
  my @loops_list;
  
  foreach my $func_obj (@{$func_list}) {
    foreach my $loop_obj (@{$func_obj->{"loops"}}) {
      _get_loop_objs_recur($loop_obj, \@loops_list);
    }
  }
  return @loops_list;
}

=head2 get_all_loops(@functions_list)

# Returns all the instances of loop in an array in the design

=cut

sub get_all_loops ($@) {
  my ($func_list, $options) = @_;  # options is for ways to filter loops
  my $filter_fully_unroll = $options->{ no_fully_unroll } || 0;
  my $filter_hls_invocation = $options->{ hls_invocation_loop };  # 0=no hls, 1=only hls, undef=all
  my @loops_list;
  foreach my $func_obj (@{$func_list}) {
    my @func_loop_queue;
    foreach my $loop (@{$func_obj->{"loops"}}) {
      next if ($filter_fully_unroll == 1 && is_loop_fully_unrolled($loop));

      push(@func_loop_queue, $loop);
      if (defined($filter_hls_invocation)) {
        # Handles HLS implicit infinite loop
        if (defined(is_loop_hls_component_invocation($loop))) {
          # 20.3 new schema for loop_attribute
          # Store it if filter and it is HLS implicit infinite loops or 
          # no filter and not HLS implicit infinite loop
          if (($filter_hls_invocation == 1 && is_loop_hls_component_invocation($loop) == 1) ||
              ($filter_hls_invocation == 0 && is_loop_hls_component_invocation($loop) == 0))
          {
            push(@loops_list, $loop);
          }
        }
        else {
          # To be obsoleted.
          # Support previous loops.json schema to store component invocation
          my @dbg = get_debug_object($loop);
          my $filename = get_debug_filename($dbg[0][0]);
          # Store it if filter and it is HLS implicit infinite loops or 
          # no filter and not HLS implicit infinite loop
          if ( ($filter_hls_invocation == 1 && $filename =~ /Component invocation/) ||
              ($filter_hls_invocation == 0 && $filename !~ /Component invocation/) )
          {
            push(@loops_list, $loop);
          }
        }
        next;
      }
      # just add if no hls invocation filer
      push(@loops_list, $loop);
    }

    # Early exit since I just want the HLS invocation loops
    return @loops_list if (defined($filter_hls_invocation) && $filter_hls_invocation == 1);

    # breath level adding of loops
    while (scalar(@func_loop_queue) > 0) {
      my $loop = shift (@func_loop_queue);
      # if (defined($loop->{"subloops"})) {
        # foreach my $subloop (@{$loop->{"subloops"}}) {
        foreach my $subloop (get_subloops($loop)) {
          next if ($filter_fully_unroll == 1 && is_loop_fully_unrolled($subloop));

          push(@func_loop_queue, $subloop);
          push(@loops_list, $subloop);
        # }
      }
    }
  }
  return @loops_list;
}

# ==== private functions ==== #

# populate $parent_subloops_list recursively
# Each loop is stored in the specified data structure below which can be access
# using methods in the public methods above
# {"name"}            = <block name>
# {"function"}        = <function name>
# {"parent"}          = <undef|block>  undef for top most loop
# {"debug"}           = (same as previous)
# {"subloops"}        = ()
# {"ii"}              = <undef|unsigned>  Positive integer for pipelined loop
# TODO: {"hyperoptimized"}  = <undef|0|1>       undef means not applied, 0 means not hyperoptimized
# TODO: {"latency"}         = <unsigned>        Positive integer for latency of the block
# {"layer"}           = <unsigned>        Positive integer. 1 means outermost loop.
# {"pipelined"}       = <0|1>             0 means not piplined, 1 means 1 pipelined
# TODO: {"speculation"}     = <undef|unsigned>  undef means unknown or positive integer
# TODO: {"stallable"}       = <0|1>             0 means stallfree, 1 means ii is not exact.
sub _get_loop_attribute_recur {
  my ($loops_list, $func_name, $parent, $parent_subloops_list) = @_;
  foreach my $cur_loop (@{$loops_list}) {
    next if ($cur_loop->{"type"} ne "loop");

    # Add loop data
    my $loop_info_obj = {};
    $loop_info_obj->{"name"}     = $cur_loop->{"name"};
    $loop_info_obj->{"function"} = $func_name;
    $loop_info_obj->{"debug"}    = $cur_loop->{"debug"};
    if (defined($parent)) {
      # save the parent and add itself to parent subloop
      $loop_info_obj->{"parent"}   = $parent;
    }

    $loop_info_obj->{"achieved_fmax"} = $cur_loop->{"af"};
    $loop_info_obj->{"hls_comp_invo"} = $cur_loop->{"ci"};
    $loop_info_obj->{"ii"} = ($cur_loop->{"ii"} =~ /\d+/) ? $cur_loop->{"ii"} : undef;
    $loop_info_obj->{"layer"} = $cur_loop->{"ll"};
    $loop_info_obj->{"latency"} = $cur_loop->{"lt"};
    $loop_info_obj->{"interleaving"} = ($cur_loop->{"mi"} =~ /\d+/) ? $cur_loop->{"mi"} : undef;
    $loop_info_obj->{"pipelined"} = ($cur_loop->{"pl"} eq "Yes") ? 1 : 0;
    $loop_info_obj->{"trip_count"} = $cur_loop->{"tc"};
    $loop_info_obj->{"threshold_no_delay"} = $cur_loop->{"tn"};
    $loop_info_obj->{"details"} = $cur_loop->{"details"};
    $loop_info_obj->{"hyperopt"} = $cur_loop->{"fo"};

    # populate it's own data to parent subloop
    push(@{$parent_subloops_list}, $loop_info_obj);

    # populate the children
    if (defined($cur_loop->{"children"})) {
      my $subloop_list = $cur_loop->{"children"};
      if (scalar(@{$subloop_list}) > 0) {
        # populates the "subloops"
        $loop_info_obj->{"subloops"} = ();
        _get_loop_attribute_recur($subloop_list, $func_name, $loop_info_obj, \@{$loop_info_obj->{"subloops"}});
      }
    }
  }
}

# populate $parent_subloops_list recursively
# Each loop is stored in the specified data structure below which can be access
# using methods in the public methods above
# {"name"}            = <block name>
# {"function"}        = <function name>
# {"parent"}          = <undef|block>  undef for top most loop
# {"debug"}           = (same as previous)
# {"subloops"}        = ()
# {"fully_unrolled"}  = <0|1>        0 means not unroll, how to handle trip count 1 unroll?
# {"pipelined"}       = <undef|0|1>  undef applies to fully unrolled, 0 means not pipelined
# {"ii"}              = <undef|#>    undef means unpipelined or fully unrolled
# {"speculation"}     = <#>          undef means unknown or positive integer
# {"concurrency"}     = <undef|#>    undef means unlimited or positive integer
# {"hyperoptimized"}  = <undef|0|1>  undef means not applied, 0 means not hyperoptimized
# {"stallable"}       = only if exist
#                     ->{"inst"} = <instructions>
# {"bottleneck"}      = only if exist
#                     ->{"type"} = <bottlenck type>
# To be obsoleted.
sub get_loop_info_recur ($$$$) {
  my ($loops_list, $func_name, $parent, $parent_subloops_list) = @_;
  foreach my $cur_loop (@{$loops_list}) {
    # figure its own loop data
    my $loop_name = {};
    $loop_name = $cur_loop->{"name"};
    my $loop_info_obj = {};
    $loop_info_obj->{"name"}     = $loop_name;
    $loop_info_obj->{"function"} = $func_name;
    $loop_info_obj->{"debug"}    = $cur_loop->{"debug"};
    if (defined($parent)) {
      # save the parent and add itself to parent subloop
      $loop_info_obj->{"parent"}   = $parent;
    }
    # Gather loop data: fully unrolled, pipelined, II, speculation
    $loop_info_obj->{"fully_unrolled"} = ($loop_name eq "Fully unrolled loop") ? 1 : 0;
    $loop_info_obj->{"pipelined"} = (${$cur_loop->{"data"}}[0] eq "Yes") ? 1 : 0;
    my $ii_string = ${$cur_loop->{"data"}}[1];
    $loop_info_obj->{"ii"} = ($ii_string =~ /(\d+)/) ? int($1) : undef;
    my $spec_string = ${$cur_loop->{"data"}}[2];
    $loop_info_obj->{"speculation"} = ($spec_string =~ /(\d+)/) ? int($1) : undef;

    # Gather loop data: hyperoptimzied, stallable instructions, concurrency
    # Gather loop carried dependencies when it has a bottleneck
    foreach my $detail (@{$cur_loop->{"details"}}) {
      if (defined($detail->{"type"}) && $detail->{"type"} eq "brief" && $detail->{"text"} !~ /^\s/ && $detail->{"text"} =~ /(.*)/) {
        $loop_info_obj->{"bottleneck"}->{"type"} = $1;
      }
      if (defined($detail->{"text"})) {
        if ($detail->{"text"} =~ /Hyper-Optimized loop structure: (\S+)/) {
          my $val = $1;
          $loop_info_obj->{"hyperoptimized"} = ($val eq "enabled.") ? 1 : ($val eq "disabled." ? 0 : undef);
        } elsif ($detail->{"text"} =~ /Maximum concurrent iterations: (\d+) /) {
          $loop_info_obj->{"concurrency"} = int($1);
        }
        # else stallable instruction
      }
    }
    # populate it's own data to parent subloop
    push(@{$parent_subloops_list}, $loop_info_obj);

    # populate the children
    if (defined($cur_loop->{"children"})) {
      my $subloop_list = $cur_loop->{"children"};
      if (scalar(@{$subloop_list}) > 0) {
        # populates the "subloops"
        $loop_info_obj->{"subloops"} = ();
        get_loop_info_recur($subloop_list, $func_name, $loop_info_obj, \@{$loop_info_obj->{"subloops"}});
      }
    }
  }
}


# ==== Memory viewer specific  ==== #

=head2 is_optimized_away($aggregate_object)

Returns 1 if aggregate has been optimized away, 0 otherwise

=cut

sub is_optimized_away {
  my $agg = shift;
  return 1 if ($agg->{"unsynth"} == 1);
  return 0;
}

=head2 is_a_register($aggregate_object)

Returns 1 if array has been is implemented as pipeline registers, 0 when it is not
undef if array is optimized away

=cut

sub is_a_register {
  my $agg = shift;
  if (defined($agg->{"register"})) {
    return 1 if ($agg->{"register"} == 1);
    return 0;
  }
  return undef;
}

=head2 is_a_rom($aggregate_object)

Returns 1 if aggregate is implemented as read-only memory, 0 when it is not
undef if array is optimized away or a register

=cut

sub is_a_rom {
  my $agg = shift;
  if (defined($agg->{"readonly"})) {
    return 1 if ($agg->{"readonly"} == 1);
    return 0;
  }
  return undef;
}

=head2 get_unroll_copy($aggregate_object)

Returns the index for this memory copy caused by unroll loop
undef if array does not have copies by unrolling loops

=cut

sub get_unroll_copy {
  my $agg = shift;
  return $agg->{"unroll"} if (defined($agg->{"unroll"}));
  return undef;
}

=head2 get_inline_copy($aggregate_object)

Returns the index for this memory copy caused by function inline
undef if array does not have copies from inline function

=cut

sub get_inline_copy {
  my $agg = shift;
  return $agg->{"inline"} if (defined($agg->{"inline"}));
  return undef;
}

=head2 parse_memory_view($json_file_path)

Returns a data structure that can be used with public API's

=cut

# Returns a list of user aggregates
# a list of functions:
#   {"name"}            = <function name>
#   {"type"}            = <type>
#   {"debug"}           = (no change)
#   {"aggregates"}      = ()     # list of aggregates
sub parse_memory_view ($) {
  my $mem_json_file = shift;
  my %agg_hash = try_to_parse($mem_json_file);

  # JSON syntax error
  acl::Common::mydie("Failed to parse $mem_json_file due to syntax error.") if (!%agg_hash);

  # organize the dataa
  my $nodes_list = $agg_hash{"nodes"};
  my @func_list;
  foreach my $func_obj (@{$nodes_list}) {
    my $func_info_obj = _parse_global_properties($func_obj);
    my @agg_list = @{$func_obj->{"children"}};
    foreach my $node (@agg_list) {
      if ($node->{"type"} eq "memtype") {
        # TODO: data structure looks like its for backward compitability which is suboptimal for rendering
        $func_info_obj->{"aggregates"} = ();
        my @aggregates_list = @{$node->{"children"}};
        get_aggregate_info(\@aggregates_list, get_name($func_info_obj), \@{$func_info_obj->{"aggregates"}});
      }
      # TODO: parse LSU and arbitration nodes
    }
    push(@func_list, $func_info_obj);
  }
  return @func_list;
}

=head2 get_all_aggregates(@functions_list)

# Returns all types of aggregates, i.e. array, struct, or union

=cut

sub get_all_aggregates ($@) {
  my ($func_list, $options) = @_;  # options is for ways to filter aggregates
  my $keep_only_memory = $options->{ memory_only } || 0;  # only RAM and ROM
  my $var_name = $options->{ name };  # user defined variable name
  my @mem_list;
  foreach my $func_obj (@{$func_list}) {
    foreach my $agg (@{$func_obj->{"aggregates"}}) {
      # check the name, type
      my $to_keep = 1;

      # don't keep if user ask for memory but it's not memory
      $to_keep = 0 if ($keep_only_memory && !defined(is_a_rom($agg)));

      # don't keep if user ask for a memory but the doesn't match
      $to_keep = 0 if (defined($var_name) && get_name($agg) ne $var_name);

      push(@mem_list, $agg) if ($to_keep);
    }
  }
  return @mem_list;
}

# ==== private functions ==== #

# populate aggregates_list where each element is structured as follows:
# {"name"}            = <block name>
# {"unroll"}          = <undef|#>   the copy instance number from loop unrolling
# {"inline"}          = <undef|#>   the copy instance number from inlining
# {"function"}        = <function name>  the name of the function it belongs to
# {"unsynth"}         = <0|1>
# {"register"}        = <undef|0|1>
# {"readonly"}        = <undef|0|1>
# {"banks"}           = list of bank object, each bank object contains a list of
#                       replicates in {"replicates"}, and each replicate object
#                       contains a list of ports in {"ports"}. 
# {"attributes"}      = {"width" => <value>, "depth" => <value>, ...}
# {"details"}         = {<key> => <value>}
#
# bank object:
# TODO: all children details should be added to memory object for ease of access
sub get_aggregate_info($$$) {
  my ($aggregates_list, $func_name, $parent_memory_list) = @_;
  foreach my $cur_agg (@{$aggregates_list}) {
    my $agg_info_obj = {};  # the object that holds all the information about the aggregates
    my $agg_name = $cur_agg->{"name"};
    if ($agg_name =~ /(\S+)\.unroll#(\d+)/) {
      # parse unroll copies
      $agg_name = $1;
      $agg_info_obj->{"unroll"} = $2;
    }
    if ($agg_name =~ /(\S+)\.inline#(\d+)/) {
      # parse inline copies
      $agg_name = $1;
      $agg_info_obj->{"inline"} = $2;
    }
    $agg_info_obj->{"name"}     = $agg_name;
    $agg_info_obj->{"function"} = $func_name;
    $agg_info_obj->{"debug"}    = $cur_agg->{"debug"};

    # Gather aggregates optimized properties
    $agg_info_obj->{"unsynth"} = ($cur_agg->{"type"} eq "unsynth") ? 1 : 0;
    if (!is_optimized_away($agg_info_obj)) {
      # not optimize away
      $agg_info_obj->{"register"} = ($cur_agg->{"type"} eq "reg") ? 1 : 0;
      if (!is_a_register($agg_info_obj)) {
        # not implemented as pipeline registers
        $agg_info_obj->{"readonly"} = ($cur_agg->{"type"} eq "romsys") ? 1 : 0;
        $agg_info_obj->{"details"} = parse_details(\@{$cur_agg->{"details"}});
      }
      # TODO: get details for every type of implementations
    }
    # populate it's own data to parent object
    push(@{$parent_memory_list}, $agg_info_obj);

    # add banks, replicates and their ports
    if (defined($cur_agg->{"children"})) {
      my @bank_list = @{$cur_agg->{"children"}};
      $agg_info_obj->{"banks"} = [];
      foreach my $bank (@bank_list) {
        next if ($bank->{"type"} ne "bank");
        my $bank_info_obj = {};  # the object that holds all the information about banks
        my $num_ports = 0;
        $bank_info_obj->{"name"} = $bank->{"name"} if (defined($bank->{"name"}));
        $bank_info_obj->{"details"} = parse_details(\@{$bank->{"details"}}) if (defined($bank->{"details"}));

        if (defined($bank->{"children"})) {
          $bank_info_obj->{"replicates"} = [];
          my @repl_list = @{$bank->{"children"}};
          foreach my $repl (@repl_list) {
            next if ($repl->{"type"} ne "replicate");
            my $repl_info_obj = {};  # the object that holds all the information about replicates
            $repl_info_obj->{"name"} = $repl->{"name"} if (defined($repl->{"name"}));
            $repl_info_obj->{"details"} = parse_details(\@{$repl->{"details"}}) if (defined($repl->{"details"}));

            if (defined($repl->{"children"})) {
              $repl_info_obj->{"ports"} = [];
              my @port_list = @{$repl->{"children"}};
              foreach my $port (@port_list) {
                next if ($repl->{"type"} ne "port");
                my $port_info_obj = {};  # the object that holds all the information about ports
                $port_info_obj->{"name"} = $port->{"name"} if (defined($port->{"name"}));
                #TODO: read Ids from ports so that we can figure how they connect to the LSUs
                push(@{$repl_info_obj->{"ports"}}, $port_info_obj);
                $num_ports++;
              }
            }
            push(@{$bank_info_obj->{"replicates"}}, $repl_info_obj);
          }
        }
        $bank_info_obj->{"num_ports"} = $num_ports;
        push(@{$agg_info_obj->{"banks"}}, $bank_info_obj);
      }
    }
  }
}

# ==== Resource utilization estimate specific  ==== #

=head2 parse_resource_utilization($json_file_path)

Returns a data structure that can be used with public API's

=cut

# The input can be an area estimate area.json or Quartus data quartus.json
# Returns a list of functions
# a list of functions:
#   {"name"}  = <name>
#   {"type"}  = <#>  # see _parse_global_properties()
#   {"debug"} = (no change)
#   {"alut"}  = <#>, Only from Est for now
#   {"reg"}   = <#>, Only from Est for now
#   {"mlab"}  = <#>, N/A when fast compile
#   {"ram"}   = <#>,
#   {"dsp"}   = <#>,
#   {"alm"}   = <#>  only from Quartus for now
sub parse_resource_utilization($) {
  my $res_json_file = shift;
  my %res_hash = try_to_parse($res_json_file);

  # JSON syntax error
  acl::Common::mydie("Failed to parse $res_json_file due to syntax error.") if (!%res_hash);

  # decide if this is Quartus
  if (defined($res_hash{"quartusFitResourceUsageSummary"})) {
    my $nodes_list = $res_hash{"quartusFitResourceUsageSummary"}->{"nodes"};
    my @func_list;
    foreach my $func_obj (@{$nodes_list}) {
      my $func_info_obj = _parse_global_properties($func_obj);
      foreach my $res_type (keys %{$func_obj}) {
        next if ($res_type eq 'name' || $res_type eq 'type');
        $func_info_obj->{$res_type} = $func_obj->{$res_type};
      }
      push(@func_list, $func_info_obj);
    }
    return @func_list;
  }

  # organize estimated resource data
  my @columns = @{$res_hash{"columns"}};
  shift @columns;  # ignore the first one
  my @func_list;
  foreach my $func_obj (@{$res_hash{"children"}}) {
    my $func_info_obj = _parse_global_properties($func_obj);
    my @dataList;
    if (defined($func_obj->{"data"})) {
      @dataList = @{$func_obj->{"data"}};
    }
    elsif (defined($func_obj->{"total_kernel_resources"})) {
      # 20.1 and before - the JSON format is inconsistent with the children
      @dataList = @{$func_obj->{"total_kernel_resources"}};
    }
    # TODO: Error checking, no negative number allowed
    for(my $dataIdx=0; $dataIdx<scalar(@dataList) ; $dataIdx++) {
      # Code to make it consistent with quartus.json
      if ($columns[$dataIdx] =~ /FF/i) {
        # TODO: rename to reg as short name for register to follow datasheet instead of flip-flop
        $func_info_obj->{'reg'} = $dataList[$dataIdx];
      } else {
        # TODO: make it lower case in compiler in future and strip 's' from key
        $func_info_obj->{lc(substr($columns[$dataIdx], 0, -1))} = $dataList[$dataIdx];
      }
    }
    # TODO: get all function children as well
    push(@func_list, $func_info_obj);
  }
  return @func_list;
}

# ==== private helper functions ==== #

# _parse_global_properties converts global nodes such as kernel or component to a internal object
# The context of global here It has some similarities with GV in compiler such as function, 
# global memory, channels or streams. And it also have board interface, system, or overhead.
# More specific example:
#  Type board interface are everything inside static region
#  Type system contains everything in kernel system or all components in the design
#  Type overhead contains Sys ROM, global interconnect. Things that are part of kernel system
#  which are design dependent but does not count towards the control flow, memory, and data path.
#  Type kernel is for OpenCL and SYCL
#  Type component is HLS only
#  TODO: Type function is HLS only with System of Tasks
sub _parse_global_properties($) {
  my $func_obj = shift;
  my $func_info_obj;
  my $func_name = $func_obj->{"name"};
  $func_info_obj->{"name"} = $func_name;
  $func_info_obj->{"debug"} = $func_obj->{"debug"} if (defined($func_obj->{"debug"}));

  # Version 19.1 and above type definition mapping
  if ($func_obj->{"type"} =~ /Kernel|Component/i) {
    # TODO: obsolete this part as new schema have proper types
    if ($func_name !~ /Kernel|Component/i) {
      # HLS functions name would not have Kernel or Component prefix
      $func_info_obj->{"type"} = "function";
    } else {
      $func_info_obj->{"type"} = $func_obj->{"type"};
    }
  }
  elsif ($func_obj->{"type"} eq "memtype") {
    $func_info_obj->{"type"} = "global memory";
  }
  elsif ($func_obj->{"type"} eq "system" || $func_obj->{"type"} eq "aclsystem") {
    $func_info_obj->{"type"} = $func_obj->{"type"};
  }
  elsif ($func_obj->{"type"} eq "resource" || $func_obj->{"type"} eq "partition") {
    # 20.1 and before area.json, type is not consistent
    $func_info_obj->{"type"} = $func_name;
  }
  elsif ($func_obj->{"type"} eq "function") {
    # 20.1 and before area.json, type is not consistent, check name for Kernel
    if ($func_name =~ /Kernel/i) {
      $func_info_obj->{"type"} = "kernel";
    }
    else {
      # TODO: area.json require fixing to differential component and function
      $func_info_obj->{"type"} = "component";
    }
  }
  else {
    my $ftype = $func_info_obj->{"type"};
    acl::Common::mydie("Unrecognize type ${ftype} for $func_name.\n");
  }

  return $func_info_obj;
}

# ==== Graph viewer specific  ==== #

=head2 parse_bottleneck_viewer($json_file_path)

Returns a list of bottlenecks in the data structure that can be used with public API's
Returns an empty array if fails to parse or file does not exist.

=cut

sub parse_bottleneck_viewer {
  my $bottleneck_json_file = shift;
  if (-e $bottleneck_json_file) {
    my %bottleneck_hash = try_to_parse($bottleneck_json_file);
    if (exists($bottleneck_hash{"bottlenecks"}) && defined($bottleneck_hash{"bottlenecks"})) {
      my @bottleneck_graphs;
      foreach my $bottleneck_graph (@{$bottleneck_hash{"bottlenecks"}}) {
        my $graph_obj = _parse_graph($bottleneck_graph);
        # parse the attributes of the bottleneck
        $graph_obj->{"type"} = $bottleneck_graph->{"type"};
        if ($graph_obj->{"type"} =~ /limiter/) {
          $graph_obj->{"concurrency"} = $bottleneck_graph->{"concurrency"};
        }
        $graph_obj->{"cause"} = $bottleneck_graph->{"brief"};  # data or mem
        push(@bottleneck_graphs, $graph_obj);
      }
      return @bottleneck_graphs;
    }
  }
  return [];
}

=head2 get_bottleneck_cause($bottleneck_graph)

Returns the caused of the bottleneck, i.e. data dependency or memory dependency

=cut

sub get_bottleneck_cause {
  my $graph_obj = shift;
  return $graph_obj->{"cause"};
}

=head2 get_bottleneck_concurrency($bottleneck_graph)

Returns the concurrency value for occupancy limiter type of bottleneck. Undef otherwise

=cut

sub get_bottleneck_concurrency {
  my $graph_obj = shift;
  return $graph_obj->{"concurrency"} if (defined($graph_obj->{"concurrency"}));
  return undef;
}

=head2 get_node_ids($bottleneck_graph)

Returns a list of ID of all nodes belong to that bottleneck graph.

=cut

sub get_node_ids {
  my $graph_obj = shift;
  my @id_list;
  foreach my $node (@{$graph_obj->{"nodes"}}) {
    push(@id_list, get_id($node));
  }
  return @id_list;
}

=head2 get_node($bottleneck_graph, $id)

Returns the node object in the bottleneck graph. Returns undef if the ID does not exist in the graph.

=cut

sub get_node {
  my $graph_obj = shift;
  my $id = shift;
  if (defined($graph_obj->{"node_dict"}->{$id})) {
    my $index = $graph_obj->{"node_dict"}->{$id};
    if ($index < scalar(@{$graph_obj->{"nodes"}})) {
      return $graph_obj->{"nodes"}[$index];
    }
  }
  return undef;
}

sub get_predecessor_ids {
  my $node = shift;
  return @{$node->{"pred"}} if (defined($node->{"pred"}));
  return undef;
}

sub get_successor_ids {
  my $node = shift;
  return @{$node->{"succ"}} if (defined($node->{"succ"}));
  return undef;
}

# ==== private helper functions ==== #

# _parse_graph expects the object to have "nodes" and "links" key
# Returns a dictionary for quick look up for id to index to node
# list, a dictionary for quick look up for details of edges, and
# a list of nodes in the same order in the original object.
# node_dict->{ id : index }
# edge_dict->{ link_id : details }  link_id = <from id>:<to id>
# nodes->[
#   {"name"}  = <name>
#   {"id"}    = <#>
#   {"type"}  = <#>  # see _parse_global_properties()
#   {"debug"} = (no change)
#   {"details"} = (no change)
#   {"parent"}  = <Block name>
#   {"pred"}  = [list of id's of predecessor]
#   {"succ"}  = [list of id's of successor]
# ]
sub _parse_graph {
  my $graph_data = shift;
  my $graph_obj;
  $graph_obj->{"node_dict"} = {};  # key = id, value = index
  $graph_obj->{"edge_dict"} = {};  # key = <from id>:<to id>, value = details
  my @nodes_list;
  my $node_count = 0;
  foreach my $node (@{$graph_data->{"nodes"}}) {
    my $id = get_id($node);
    $node->{"pred"} = [];
    $node->{"succ"} = [];
    push(@nodes_list, $node);
    $graph_obj->{"node_dict"}->{$id} = $node_count;
    $node_count++;
  }
  # details for edges are converted to predecessor and successor for ease of
  # graph traversal. The details of the edge is stored in edge_dict at the top
  # level for ease of access.
  foreach my $link (@{$graph_data->{"links"}}) {
    # Schema error checks
    next if (!defined($link->{"from"}) || !defined($link->{"to"}));

    my $src_id = 0;
    my $dst_id = 0;
    if (exists $link->{"reverse"}) {
      $src_id = $link->{"to"};
      $dst_id = $link->{"from"};
    } else {
      $src_id = $link->{"from"};
      $dst_id = $link->{"to"};
    }

    # Data error checks
    my $src_index = -1;
    my $dst_index = -1;
    $src_index = $graph_obj->{"node_dict"}->{$src_id} if (defined($graph_obj->{"node_dict"}->{$src_id}));
    $dst_index = $graph_obj->{"node_dict"}->{$dst_id} if (defined($graph_obj->{"node_dict"}->{$dst_id}));

    # Assign the successor to source node and predecessor to destination node and add to edge detail dictionary
    if ($src_index != -1 && $dst_index != -1) {
      my $src_node = $nodes_list[$src_index];
      push(@{$src_node->{"succ"}}, $dst_id);
      my $dst_node = $nodes_list[$dst_index];
      push(@{$dst_node->{"pred"}}, $src_id);

      $graph_obj->{"edge_dict"}->{_create_link_id($src_id, $dst_id)} = $link->{"details"} if (defined($link->{"details"}));
    }
  }
  $graph_obj->{"nodes"} = \@nodes_list;
  return $graph_obj;
}

sub _create_link_id {
  my $src_id = shift;
  my $dst_id = shift;
  return $src_id.":".$dst_id;
}

# ==== Simulator specific  ==== #

=head2 parse_simulation_raw($json_file_path)

Returns a dictionary of data structure that can be used with simulator
specific API's.
Returns an empty array if fails to parse or file does not exist.

=cut

sub parse_simulation_raw {
  my $simulation_raw_json_file = shift;
  if (-e $simulation_raw_json_file) {
    my %sim_obj = try_to_parse($simulation_raw_json_file);
    # simple check
    if ($sim_obj{"period"}) {
      return \%sim_obj;
    }
  }
  return {};
}

=head2 get_clock_period($simulation_object)

Returns the integer clock period in ps.

=cut

sub get_clock_period {
  my $sim_obj = shift;
  return $sim_obj->{"period"} if (defined($sim_obj->{"period"}));
  return undef;
}

=head2 get_simulation_time($simulation_object)

Returns the integer total simulation time in ps.

=cut

sub get_simulation_time {
  my $sim_obj = shift;
  return $sim_obj->{"time"} if (defined($sim_obj->{"time"}));
  return undef;
}

=head2 get_simulation_instruction_list($invocation_data_object)

Returns a list of instruction names which the simulator measured.

=cut

sub get_simulation_instruction_list {
  my $sim_obj = shift;
  my @inst_list;
  foreach my $attr (keys %{$sim_obj}) {
    next if ($attr == "period" || $attr == "time");
    push (@inst_list, $attr);
  }
  return @inst_list;
}

=head2 get_invocation_start_time($invocation_data_object)

Returns the integer invocation start time in ps.

=cut

sub get_invocation_start_time {
  my $invocation_obj = shift;
  return $invocation_obj->{"start"} if (defined($invocation_obj->{"start"}));
  return undef;
}

=head2 get_invocation_end_time($invocation_data_object)

Returns the integer invocation end time in ps.

=cut

sub get_invocation_end_time {
  my $invocation_obj = shift;
  return $invocation_obj->{"end"} if (defined($invocation_obj->{"end"}));
  return undef;
}

=head2 get_invocation_latency_time($invocation_data_object)

Returns the integer end minus start in ps.

=cut

sub get_invocation_latency_time {
  my $invocation_obj = shift;
  if (defined($invocation_obj->{"start"}) && defined($invocation_obj->{"end"})) {
    return $invocation_obj->{"end"} - $invocation_obj->{"start"};
  }
  return undef;
}

=head2 is_component_invocation_enqueued($invocation_data_object)

Returns 1 when invocation is enqueued. 0 when invocation is explicit.

=cut

sub is_component_invocation_enqueued {
  my $invocation_obj = shift;
  return $invocation_obj->{"enqueued"} if (defined($invocation_obj->{"enqueued"}));
  return undef;
}

=head2 get_invocation_data($invocation_object)

Returns a list of invocation data objects.

=cut

sub get_invocation_data {
  my $sim_obj = shift;
  my $inst_name = shift;
  if (defined($sim_obj->{$inst_name})) {
    return ${$sim_obj->{$inst_name}}{"data"};
  }
  return undef;
}

1;
