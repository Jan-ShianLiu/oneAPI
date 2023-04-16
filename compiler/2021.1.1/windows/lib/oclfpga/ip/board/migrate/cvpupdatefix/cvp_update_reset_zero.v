module cvp_update_reset # (
   parameter PIPE32_SIM_ONLY = 0
   ) (
   input pipe8_sim_only,
   input npor,
   input perst,
   output cvp_tx_analogreset
   );




//   assign serdes_tx_analogreset      = ((PIPE32_SIM_ONLY==1)||(pipe8_sim_only==1'b1))?1'b0           :(low_str(hip_hard_reset)=="disable")?~npor_int:1'b0;
//
   assign cvp_tx_analogreset      = 1'b0;



endmodule
