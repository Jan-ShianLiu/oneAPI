package require -exact qsys 17.1

# module properties
set_module_property NAME         {aoc_sim_fake_pll}
set_module_property DISPLAY_NAME {OpenCL simulator fake PLL}

# default module properties
set_module_property VERSION      {17.0}
set_module_property GROUP        {OpenCL Simulation}
set_module_property DESCRIPTION  {default description}
set_module_property AUTHOR       {author}

set_module_property ELABORATION_CALLBACK         elaborate

# +-----------------------------------
# | parameters
# | 
add_parameter REF_CLK_RATE FLOAT 125.0
set_parameter_property REF_CLK_RATE DEFAULT_VALUE 125.0
set_parameter_property REF_CLK_RATE DISPLAY_NAME {REF_CLK_RATE}
set_parameter_property REF_CLK_RATE AFFECTS_ELABORATION false

set USE_CLOCK2X    "USE_CLOCK2X"
add_parameter $USE_CLOCK2X Integer 0 
set_parameter_property $USE_CLOCK2X DISPLAY_NAME "clock2x output"
set_parameter_property $USE_CLOCK2X AFFECTS_ELABORATION true
set_parameter_property $USE_CLOCK2X HDL_PARAMETER false
set_parameter_property $USE_CLOCK2X DESCRIPTION "Enable an extra 2x clock output"
set_parameter_property $USE_CLOCK2X DISPLAY_HINT boolean
# | 
# +-----------------------------------

# Only sim, no synth
add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL aoc_sim_fake_pll
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file aoc_sim_fake_pll.sv SYSTEM_VERILOG PATH aoc_sim_fake_pll.sv

#------------------------------------------------------------------------------
proc elaborate {} {
  global USE_CLOCK2X
  set ADD_CLOCK2X_PORT [ get_parameter $USE_CLOCK2X ]

  add_interface reset reset end
  set_interface_property reset associatedClock ""
  set_interface_property reset synchronousEdges NONE
  set_interface_property reset ENABLED true

  add_interface_port reset reset reset Input 1

  add_interface locked reset start
  set_interface_property locked associatedClock refclk
  set_interface_property locked associatedDirectReset ""
  set_interface_property locked associatedResetSinks reset
  set_interface_property locked synchronousEdges DEASSERT
  set_interface_property locked ENABLED true

  add_interface_port locked locked reset_n Output 1

  add_interface refclk clock sink
  add_interface_port refclk refclk clk Input 1

  add_interface outclk_0 clock source
  add_interface_port outclk_0 outclk_0 clk Output 1

  if {$ADD_CLOCK2X_PORT} {
    add_interface outclk_1 clock source
    add_interface_port outclk_1 outclk_1 clk Output 1
  }
}
