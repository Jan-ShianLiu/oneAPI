module temperature_a10
#(
   parameter WIDTH = 32,
   parameter LOG2_RESET_CYCLES=11
)
(
   input clk, 
   input resetn,

   // Slave port
   input                 slave_address,                  // not used
   input [WIDTH-1:0]     slave_writedata,                // not used
   input                 slave_read,                     // reads the value from the last conversion (as triggered by a write)
   input                 slave_write,                    // not used
   input [WIDTH/8-1:0]   slave_byteenable,               // not used
   output reg            slave_waitrequest,              // only asserted during reset
   output [WIDTH-1:0]    slave_readdata,                 // result of temperature measurement
   output reg            slave_readdatavalid             // asserted on the next clock cycle after slave_read (except during reset condition)
);

   wire                          conv_done_in;           // conversion done signal from the temperature sensor
   reg   [1:0]                   conv_done;              // capture the conv_done_in signal onto the local clock
   wire  [9:0]                   tempout;                // temperature value from the temp sensor
   reg   [9:0]                   tempout_reg;            // locally registered version of temperature value
   
   // instantiate the temperature sensor (runs on an internal 1MHz oscillator clock)
   temp_sense_a10 temp(
      .corectl(1'b1),                  // always be making new measurements with the temp sensing diode
      .eoc(conv_done_in),              // output, asserted high for 1 1MHz clock cycle when the new temperature is ready
      .reset(!resetn),                 // reset input
      .tempout(tempout)                // temperature reading, valid on the falling edge of eoc
   );
   
   // logic to control capturing temperature sensor data from the temp core
   always @(posedge clk or negedge resetn) begin
   
      if ( resetn==1'b0 ) begin
         
         conv_done <= 2'b00;
         tempout_reg <= 10'b0000000000;
         slave_readdatavalid <= 1'b0;
         slave_waitrequest <= 1'b1;
         
      end else begin
      
         // capture the eoc signal onto the local clock
         conv_done[0] <= conv_done_in;
         conv_done[1] <= conv_done[0];
         
         // capture the temperature value on the falling edge of conv_done
         if ( conv_done[1]==1'b1 && conv_done[0]==1'b0 ) begin
            tempout_reg <= tempout;
         end
         
         // assert readdatavalid immediately when we receive a read request
         slave_readdatavalid <= slave_read;
         
         // never need to assert waitrequest (except during reset condition)
         slave_waitrequest <= 1'b0;
      
      end
      
   end

   assign slave_readdata = {{WIDTH-10{1'b0}},tempout_reg};     // set upper bits of readdata to 0, lower bits are the captured temperature reading

endmodule
