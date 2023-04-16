//// (c) 1992-2020 Intel Corporation.                            
// Intel, the Intel logo, Intel, MegaCore, NIOS II, Quartus and TalkBack words    
// and logos are trademarks of Intel Corporation or its subsidiaries in the U.S.  
// and/or other countries. Other marks and brands may be claimed as the property  
// of others. See Trademarks on intel.com for full list of Intel trademarks or    
// the Trademarks & Brands Names Database (if Intel) or See www.Intel.com/legal (if Altera) 
// Your use of Intel Corporation's design tools, logic functions and other        
// software and tools, and its AMPP partner logic functions, and any output       
// files any of the foregoing (including device programming or simulation         
// files), and any associated documentation or information are expressly subject  
// to the terms and conditions of the Altera Program License Subscription         
// Agreement, Intel MegaCore Function License Agreement, or other applicable      
// license agreement, including, without limitation, that your use is for the     
// sole purpose of programming logic devices manufactured by Intel and sold by    
// Intel or its authorized distributors.  Please refer to the applicable          
// agreement for further details.                                                 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Objective:
//  The width of a data path signal should be parameter of type "int unsigned", however this causes some complications for width = 0. 
//  When the width parameter is of type "int", the bit indexing is actually [-1:0] which is confusing but legal.
//  However for "int unsigned", verilog spec say mixing signed and unsigned results in unsigned, so -1 unsigned is interpreted as a large number.
//  We provide a macro to clip the width when 0.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

`ifndef ACL_WIDTH_CLIP_SVH
`define ACL_WIDTH_CLIP_SVH

`define ACL_WIDTH_CLIP(WIDTH) (((WIDTH)==0)?0:((WIDTH)-1)):0

`endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Example usage:
//
//  `include "acl_width_clip.svh"
//  module my_module #(
//      parameter int unsigned MY_WIDTH
//  ) (
//      output logic [`ACL_WIDTH_CLIP(MY_WIDTH)] my_signal
//  );
//  localparam int unsigned BYTEENABLE_WIDTH = MY_WIDTH/8;
//  logic [`ACL_WIDTH_CLIP(BYTEENABLE_WIDTH)] my_byteenable;
//
//  Notes:
//  - if MY_WIDTH == 0, my_signal is declared as [0:0]
//  - if MY_WIDTH >= 1, my_signal is declared as [WIDTH-1:0]
//  - do not include the :0 to select the lower bit range, the macro already includes it
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
