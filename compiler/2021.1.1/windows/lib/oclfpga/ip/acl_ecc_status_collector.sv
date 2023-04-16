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


`default_nettype none

module acl_ecc_status_collector (
    input  wire clock,
    //no reset, if system integrator complains then we could add the input signal which won't get used
    input  wire [1:0] ecc_err_status    //this is assumed to already be OR-ed from all sources -- easy to break this into separate 1-bit signals here if it makes system integrator easier to deal with
    //no output yet, hook up signaltap for now
);

    //this code is borrowed from acl_reset_handler.sv with the reset removed
    localparam SYNCHRONIZER_DEFAULT_DEPTH = 3;  // number of register stages used in sychronizer, default is 3 for S10 devices, must always be 2 or larger for all devices
    
    (* altera_attribute = {"-name ADV_NETLIST_OPT_ALLOWED NEVER_ALLOW; -name SYNCHRONIZER_IDENTIFICATION FORCED; -name DONT_MERGE_REGISTER ON; -name PRESERVE_REGISTER ON  "} *) reg synchronizer_head_0;
    (* altera_attribute = {"-name ADV_NETLIST_OPT_ALLOWED NEVER_ALLOW; -name SYNCHRONIZER_IDENTIFICATION FORCED; -name DONT_MERGE_REGISTER ON; -name PRESERVE_REGISTER ON  "} *) reg synchronizer_head_1;
    (* altera_attribute = {"-name ADV_NETLIST_OPT_ALLOWED NEVER_ALLOW; -name DONT_MERGE_REGISTER ON; -name PRESERVE_REGISTER ON"} *) reg [SYNCHRONIZER_DEFAULT_DEPTH-2:0] synchronizer_body_0;
    (* altera_attribute = {"-name ADV_NETLIST_OPT_ALLOWED NEVER_ALLOW; -name DONT_MERGE_REGISTER ON; -name PRESERVE_REGISTER ON"} *) reg [SYNCHRONIZER_DEFAULT_DEPTH-2:0] synchronizer_body_1;
    
    //first register in the synchronizer chain
    always_ff @(posedge clock) begin
        synchronizer_head_0 <= ecc_err_status[0];
        synchronizer_head_1 <= ecc_err_status[1];
    end
    
    //remaining registers in the synchronizer chain
    generate
    if (SYNCHRONIZER_DEFAULT_DEPTH < 3) begin : GEN_SHALLOW_SYNCHRONIZER
        always_ff @(posedge clock) begin
            synchronizer_body_0 <= synchronizer_head_0;
            synchronizer_body_1 <= synchronizer_head_1;
        end
    end
    else begin : GEN_DEEP_SYNCHRONIZER
        always_ff @(posedge clock) begin
            synchronizer_body_0 <= {synchronizer_body_0[SYNCHRONIZER_DEFAULT_DEPTH-3:0], synchronizer_head_0};
            synchronizer_body_1 <= {synchronizer_body_1[SYNCHRONIZER_DEFAULT_DEPTH-3:0], synchronizer_head_1};
        end
    end
    endgenerate
    
    //TODO: need to export this signal to the BSP
    //for now, using a noprune register which one can hook up signaltap to, noprune tells quartus to keep the register around even though it has no fanout
    //using a verbose name to make it easy to find in the signaltap node finder
    logic acl_ecc_status_collector_signaltap_double_bit_error_non_correctable /* synthesis noprune */;
    logic acl_ecc_status_collector_signaltap_single_bit_error_corrected /* synthesis noprune */;
    
    //TODO add reset here if we think we can get spurious ecc errors
    //the opencl reset is held for a long time (something like 1000 clocks) so the reset should propagate from each ecc decoder status to here before reset is released
    always_ff @(posedge clock) begin
        acl_ecc_status_collector_signaltap_double_bit_error_non_correctable <= synchronizer_body_0; //bit 0 is for fatal error -- this mapping must match acl_altera_syncram_wrapper.sv
        acl_ecc_status_collector_signaltap_single_bit_error_corrected <= synchronizer_body_1;   //bit 1 is for correctable error
    end

endmodule

`default_nettype wire
