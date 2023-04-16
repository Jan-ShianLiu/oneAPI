set DELIM "\n"
array set parsed_cached_panels {}
array set parsed_clocks {}
# splits $str by $spl, trimming the resulting elements and
# returning non-empty elements
proc extract_from_string {str spl} {
  set res [split $str $spl]
  set ret {}
  foreach r $res {
    set tmp [string trim $r]
    lappend ret $tmp
  }
  set ret [lreplace $ret 0 0]
  set ret [lreplace $ret end end]
  return $ret
}


proc get_row_by_column {panel column value} {
  global DELIM
  # assume column is in first entry
  set columns [split [lindex $panel 0] $DELIM]
  set count 0

  # find index of column
  for {set i 0} {$i < [llength $columns]} {incr i} {
    if {[lindex $columns $i] eq $column} {
      break
    }
    incr count
  }
  if {$count == [llength $columns]} {
    error "Could not find the following column in the panel: $column. Panel: $panel"
  }

  # search for value - linear search based on the column index
  foreach l $panel {
    set vals  [split $l $DELIM]
    if {[string match "$value" [lindex $vals $count]]} {
      # found it, return value
      return $vals
    }
  }

  # value not found
  return [list]
}

proc get_panel_by_name {report panel} {
  global parsed_cached_panels
  global DELIM
  if {[info exists parsed_cached_panels($panel)]} {
    return $parsed_cached_panels($panel)
  }
  set fh [open $report]
  set lines [split [read $fh] "\n"]
  close $fh

  # find the panel - fetch everything
  # starts with ^+[+|-]*+$
  # Then ;\s*NAME\s*;
  # then ^\+[\+|\-]*\+$
  # then optional column headers: ;\s*(\w+)\s*; - repeated groups
  # then ^\+[\+|\-]*\+$
  # then values similar to column headers: ;\s*(\w+)\s*; - repeated groups
  # then ^\+[\+|\-]*\+$
  set myEntries [list]
  set column_divider {^\+[\+|\-]*\+$}
  set panel_name_extract {^;\s+(.*?)\s+;$}
  set val_extract ";"
  for {set i 0} {$i < [llength $lines]} {incr i} {
    set l [lindex $lines $i]
    if {[regexp $column_divider $l ->]} {
      # start of a panel - begin extraction
      incr i
      set l [lindex $lines $i]
      regexp $panel_name_extract $l -> panel_name
      if {$panel_name eq $panel} {
        incr i 2
        set l [lindex $lines $i]
        # extract column names
        lappend myEntries [join [extract_from_string $l $val_extract] $DELIM]
        incr i
        set l [lindex $lines $i]
        if {[regexp $column_divider $l ->]} {
          incr i
          set l [lindex $lines $i]
        }

        # extract keys and values
        set vals [extract_from_string $l $val_extract]
        while {[llength $vals] > 0} {
          # store for later
          lappend myEntries [join $vals $DELIM]

          # post
          incr i
          set l [lindex $lines $i]
          set vals [extract_from_string $l $val_extract]
        }
        set parsed_cached_panels($panel) $myEntries
        return $myEntries
      }
    }
  }

}


# fetch_from_report $report -panel $panel_name -column $column_to_search -match $row_to_search [-all $boolean
# -column specifies the column name in the report to search against
# -match specifies the value to match against in the specified column
# -all specifies to grab each column and return a list instead of individual TCL array
proc fetch_from_report {report args} {
  array set "" {-panel "" -column "" -all 0 -match ""}
  foreach {k v} $args {
    if {![info exists ($k)]} { error "Bad option: $k" }
    set ($k) $v
  }
  # assert correct information is specified
  # cannot use -all and -column/match at the same time
  if {$(-panel) eq ""} {
    error "Must specify a panel!"
  }
  if {$(-all) && [expr {$(-column) ne "" || $(-match) ne ""}]} {
    error "Cannot specify -all and -column or -match simultaneously"
  }

  if {[expr {$(-column) ne "" && $(-match) eq ""}] ||
      [expr {$(-column) eq "" && $(-match) ne ""}] } {
    error "Cannot specify -column with -match and vice versa"
  }

  # search for this panel and parse the results
  # fetch all rows, as tcl list of arrays
  set panel [get_panel_by_name $report $(-panel)]
  if {$(-all)} {
    return $panel
  } else {
    # search by column value. Return rows that regexp match against it
    return [get_row_by_column $panel $(-column) $(-match)]
  }
  return [list]
}

# searches report for the "panel" which is a precursor line, returning the columns in indices
# report: file containing panel
# panel: string before the contents of the panel
# indices: list indicated which columns to return, and the order
# length: max number of columns for this pseudo panel
# Returns flattened list of columns in the order that indices specified
proc fetch_pseudo_panel {report panel indices length} {
  # open report and fetch contents
  set fh [open $report]
  set lines [split [read $fh] "\n"]
  close $fh

  set returnList {}
  # go line by line until timing line is found
  for {set i 0} {$i <  [llength $lines]} {incr i} {
    set l [lindex $lines $i]
    if {[regexp ".*$panel.*" $l]} {
      incr i 3
      set l [lindex $lines $i]
      # matched the line, now to parse the results
      # split based on space
      set vals [regexp -all -inline {\S+} $l]
      while {[llength $vals] == [expr {$length + 2}]} {
        foreach in $indices {
          set realI [expr {$in + 2}]
          lappend returnList [lindex $vals $realI]
        }
        incr i
        set l [lindex $lines $i]
        set vals [regexp -all -inline {\S+} $l]
      }
      return $returnList
    }
  }
  return $returnList
}

# searches report for list of clocks and their respective periods
# report: file containing the clocks
# Returns clock,period pairs
proc fetch_clock_periods {report} {
  global parsed_clocks
  if {[info exists parsed_clocks($report)]} {
    return $parsed_clocks($report)
  }
  return [fetch_pseudo_panel $report "Found \[0-9\]* clocks" {1 0} 2]
}

proc fetch_clock {report clk} {
  array set all_clks [fetch_clock_periods $report]
  # linear search for a match
  foreach {clk_name period} [array get all_clks] {
    # if the clk string matches the clk name in regexp
    if {[string match $clk $clk_name]} {
      return [list $clk_name $period]
    }
  }
  return [list -1 -1]
}

# searches report for "Worst-cast $slack slack is $val", and parses that table
# report: file containing the timing reports
# slack: type of slack (setup, hold, recovery, etc)
# clk: hpath to the clock corresponding to the value requested
# Returns clock,slack pairs in a list
proc fetch_timing_all_clocks {report slack clk} {
  # open report and fetch contents
  array set all_clks [fetch_pseudo_panel $report "Worst-case $slack slack is" {2 0} 3]
  if {[info exists all_clks($clk)]} {
    return [array get all_clks]
  }
  # Starting in Quartus 20.1, there are new fields to denote the timing corner
  array set all_clks [fetch_pseudo_panel $report "Worst-case $slack slack is" {2 0} 7]
  return [array get all_clks]
}

# fetch the slack of the clock from the report corresponding to the requested slack
# report: file containing the entries
# slack: Type of slack (setup, hold, recovery, etc)
# clk: hpath to the clock corresponding to the value requested
# Returns slack value
proc fetch_timing {report slack clk {required 1}} {
  array set all_clks [fetch_timing_all_clocks $report $slack $clk]
  if {[info exists all_clks($clk)]} {
    return $all_clks($clk)
  } else {
    if {!$required} {
      return "N/A"
    }
    error "Could not find clock! Make sure it is correctly specified"
  }
}
