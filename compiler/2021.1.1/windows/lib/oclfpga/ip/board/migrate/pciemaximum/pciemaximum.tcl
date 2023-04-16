
proc substFile {exp replacement inputFile outputFile} {
  set fin [open $inputFile r]
  set fout [open $outputFile w]
  while {[gets $fin linein] >= 0} {
    regsub $exp $linein $replacement lineout
    puts $fout $lineout
  }
  close $fin
  close $fout
}

set qsys_files [concat [glob -nocomplain *.qsys] [glob -nocomplain iface/*.qsys] ]

post_message "Auto migration processing the following files: $qsys_files"

foreach q $qsys_files {
  post_message "Attempting to apply fix to $q"
  substFile "name=\"rxbuffer_rxreq_hwtcl\" value=\"Maximum\"" "name=\"rxbuffer_rxreq_hwtcl\" value=\"Low\"" $q $q.am_tmp
  file copy -force $q.am_tmp $q
  file delete -force $q.am_tmp
}
