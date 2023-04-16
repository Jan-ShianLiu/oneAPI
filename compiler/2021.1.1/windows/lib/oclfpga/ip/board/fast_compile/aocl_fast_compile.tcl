namespace eval ::aocl_fast_compile {
  variable package_root "$::env(INTELFPGAOCLSDKROOT)/ip/board/fast_compile"
  variable fast_compile
}

proc ::aocl_fast_compile::is_fast_compile {} {
  variable fast_compile

  if {[catch {set fast_compile $::env(AOCL_FAST_COMPILE)} result]} {
    set fast_compile 0
  } 

  if {$fast_compile != 1} {
    set fast_compile 0
  }

  return $fast_compile;
}

# Check and warn if logic utilization is not within limit on fast compile
proc ::aocl_fast_compile::check_logic_utilization {logic_limit} {
  set acl_rpt [open "acl_quartus_report.txt" r]
  set contents [read -nonewline $acl_rpt]
  close $acl_rpt
  set lines [split $contents "\n"]
  foreach li $lines {
    if {[regexp {^Logic utilization:.*\( (.*) %} $li -> utilization ]} {
      if { $utilization > $logic_limit } {
        post_message -type critical_warning "BSP_MSG: This design is large and was compiled with --fast-compile, which is known to introduce higher power requirements. Please ensure this design is within the power limits of your board."
      }
    }
  }
}

