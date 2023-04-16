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


// Memory with multiple read ports and one write (broadcast) port.

`default_nettype none

module acl_multireadport_mem
#(
  parameter LOG2DEPTH=10,
  parameter WIDTH=32, 
  parameter NUMPORTS=13,    // Number of ports desired
  parameter USE2XCLOCK=1, 
  parameter DEDICATED_BROADCAST_PORT=0,
  parameter FAMILY="Arria 10",
  parameter enable_ecc = "FALSE",                       // Enable error correction coding
  parameter ASYNC_RESET=1,                              // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
  parameter SYNCHRONIZE_RESET=0                         // set to '1' to pass the incoming reset signal through a synchronizer before use. Note that resets to registers on the 2x clock are always synchronized,
                                                        // as they require a false-path to meet timing   
)
(
  input  wire clk,
  input  wire clk2x,
  input  wire resetn,

  // Broadcast interface
  input  wire [LOG2DEPTH-1:0]   broadcast_addr,
  input  wire [WIDTH-1:0]      broadcast_writedata,
  input  wire                  broadcast_read,
  input  wire                  broadcast_write,
  output wire                  broadcast_waitrequest,
  output wire                  broadcast_readdatavalid,
  output wire [WIDTH-1:0]      broadcast_readdata,

  // Read port interfaces
  input wire [NUMPORTS*LOG2DEPTH-1:0]    rdport_addr,
  input wire [NUMPORTS-1:0]             rdport_read,
  output reg [NUMPORTS-1:0]             rdport_waitrequest,
  output reg [NUMPORTS-1:0]             rdport_readdatavalid,
  output reg [NUMPORTS*WIDTH-1:0]       rdport_readdata,
  output logic  [1:0] ecc_err_status  // ecc status signals
);

/******************
* LOCAL PARAMETERS
*******************/

  localparam DEPTH=2**LOG2DEPTH;

  localparam PORTSPERMEM = (USE2XCLOCK) ? 4 : 2;  //Dual-port + double-pumped block RAMs
  localparam MEMORYBLOCK_LATENCY = (USE2XCLOCK) ? 4 : 2;
  localparam KERNPORTSPERMEM=PORTSPERMEM-DEDICATED_BROADCAST_PORT;

  // Do ceiling manually: CEIL(NUMPORTS/KERNPORTSPERMEM)
  localparam NUMMEMORIES=(NUMPORTS/KERNPORTSPERMEM + 
    ((NUMPORTS%KERNPORTSPERMEM==0) ? 0 : 1));

/******************
* SIGNALS
*******************/

  // Ports interfaces - connect these to rdport_ interface
  reg  [NUMMEMORIES*PORTSPERMEM*LOG2DEPTH-1:0]    port_addr;
  reg  [NUMMEMORIES*PORTSPERMEM-1:0]             port_read;
  wire [NUMMEMORIES*PORTSPERMEM-1:0]             port_waitrequest;
  wire [NUMMEMORIES*PORTSPERMEM-1:0]             port_readdatavalid;
  wire [NUMMEMORIES*PORTSPERMEM*WIDTH-1:0]       port_readdata;

  reg  [NUMMEMORIES-1:0] done_write;
 
  reg  [NUMPORTS*WIDTH-1:0] rdport_readdata_direct;
  wire [NUMPORTS*WIDTH-1:0] rdport_readdata_bypass;
  wire [NUMPORTS-1:0]       broadcast_addr_comp;
  wire [NUMPORTS-1:0]       broadcast_addr_comp_delayed;
  wire [WIDTH-1:0]          broadcast_writedata_delayed;

/******************
* ARCHITECTURE
*******************/

   localparam                    NUM_RESET_COPIES = 1;
   localparam                    RESET_PIPE_DEPTH = 3;
   logic                         aclrn;
   logic [NUM_RESET_COPIES-1:0]  sclrn;
   logic                         resetn_synchronized;
   acl_reset_handler #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
      .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES)
   ) acl_reset_handler_inst (
      .clk                    (clk),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn),
      .o_sclrn                (sclrn),
      .o_resetn_synchronized  (resetn_synchronized)
   );


  // Connect kernel interfaces to ports.  Really these only exist to prevent
  // compile errors on trailing ports.  Eg. If you want only 1 port the code
  // will still instatiate a 4-port memory but the last 3 interfaces get
  // synthesized away
  // For prior-Stratix 10 devices the direct memory_block output is used (rdport_readdata_direct)
  // while the Stratix 10 implementation implements a potential bypass to emulate a 
  // READ_DURING_WRITE_MODE_MIXED_PORTS="NEW_DATA" mode of altsyncram, which only supports "DONT_CARE" for
  // Stratix 10
  integer k;
  integer j;
  always@*
  begin
    j=0;
    for (k=0; k<NUMPORTS; k=k+1)
    begin 
      port_addr[j*LOG2DEPTH +: LOG2DEPTH]=rdport_addr[k*LOG2DEPTH +: LOG2DEPTH];
      port_read[j]=rdport_read[k];
      rdport_waitrequest[k]=port_waitrequest[j];
      rdport_readdatavalid[k]=port_readdatavalid[j];
      rdport_readdata_direct[k*WIDTH +: WIDTH]=port_readdata[j*WIDTH +: WIDTH];
      rdport_readdata=((FAMILY=="Cyclone V") || (FAMILY=="Stratix V") || (FAMILY=="Arria 10")) ? rdport_readdata_direct : rdport_readdata_bypass;
      //If the 4th port is dedicated to the host, skip it
      j=(j%PORTSPERMEM==KERNPORTSPERMEM-1) ? j+1+DEDICATED_BROADCAST_PORT : j+1 ;
    end
  end


  // If the host and kernels share a port, sel arbitrates between them.
  // 1 selects the kernel, 0 selects the host
  // Note it will always take the kernel's request, so the host may starve
  reg  [NUMMEMORIES-1:0] sel;
  genvar p;
  generate
  if (DEDICATED_BROADCAST_PORT) begin
    assign sel = 0;
  end
  else begin
    for (p=0; p<NUMMEMORIES; p++) begin : GEN_RANDOM_BLOCK_NAME_R53
      assign sel[p] = port_read[p+PORTSPERMEM-1];
    end
  end
  endgenerate

  // Replicate number of memories to achieve desired number of ports
  generate
  logic [NUMMEMORIES-1:0] ecc_err_status_for_0;
  logic [NUMMEMORIES-1:0] ecc_err_status_for_1;
  assign ecc_err_status[0] = |ecc_err_status_for_0;
  assign ecc_err_status[1] = |ecc_err_status_for_1;
  for (p=0; p<NUMMEMORIES*PORTSPERMEM; p=p+PORTSPERMEM)
  begin : mem_gen

    if ( USE2XCLOCK )
    begin
      memory_block2x  #(.LOG2DEPTH(LOG2DEPTH), .WIDTH(WIDTH), .FAMILY(FAMILY), .enable_ecc(enable_ecc), .ASYNC_RESET(ASYNC_RESET), .SYNCHRONIZE_RESET(1)) mem(
        .clk(clk),
        .clk2x(clk2x),
        .resetn(resetn),

        .addr_1(port_addr[p*LOG2DEPTH +: LOG2DEPTH]),
        .read_1(port_read[p]),
        .data_out_1(port_readdata[p*WIDTH +: WIDTH]),
        .valid_1(port_readdatavalid[p]),

        .addr_2(port_addr[(p+1)*LOG2DEPTH +: LOG2DEPTH]),
        .read_2(port_read[p+1]),
        .data_out_2(port_readdata[(p+1)*WIDTH +: WIDTH]),
        .valid_2(port_readdatavalid[p+1]),

        .addr_3(port_addr[(p+2)*LOG2DEPTH +: LOG2DEPTH]),
        .read_3(port_read[p+2]),
        .data_out_3(port_readdata[(p+2)*WIDTH +: WIDTH]),
        .valid_3(port_readdatavalid[p+2]),

        .addr_4((sel) ? port_addr[(p+3)*LOG2DEPTH +: LOG2DEPTH] : broadcast_addr),
        .data_4( broadcast_writedata),
        .read_4((sel) ? port_read[p+3] : broadcast_read),
        .write_4((sel) ?  1'b0 : broadcast_write),
        .data_out_4( port_readdata[(p+3)*WIDTH +: WIDTH]),
        .valid_4( port_readdatavalid[p+3]),
        .ecc_err_status ({ecc_err_status_for_1[p/PORTSPERMEM], ecc_err_status_for_0[p/PORTSPERMEM]})
        );

        //Connect host read signals to 4th port
        assign broadcast_readdata=port_readdata[3*WIDTH +: WIDTH];
        assign broadcast_readdatavalid=port_readdatavalid[3];
    end
    else
    begin
      memory_block  #(.LOG2DEPTH(LOG2DEPTH), .WIDTH(WIDTH), .FAMILY(FAMILY), .enable_ecc(enable_ecc), .ASYNC_RESET(ASYNC_RESET), .SYNCHRONIZE_RESET(SYNCHRONIZE_RESET)) mem(
        .clk(clk),
        .clk2x(clk2x),
        .resetn(resetn),

        .addr_1(port_addr[p*LOG2DEPTH +: LOG2DEPTH]),
        .read_1(port_read[p]),
        .data_out_1(port_readdata[p*WIDTH +: WIDTH]),
        .valid_1(port_readdatavalid[p]),

        .addr_2((sel) ? port_addr[(p+1)*LOG2DEPTH +: LOG2DEPTH] : broadcast_addr),
        .data_2( broadcast_writedata),
        .read_2((sel) ? port_read[p+1] : broadcast_read),
        .write_2((sel) ?  1'b0 : broadcast_write),
        .data_out_2( port_readdata[(p+1)*WIDTH +: WIDTH]),
        .valid_2( port_readdatavalid[p+1]),
        .ecc_err_status ({ecc_err_status_for_1[p/PORTSPERMEM], ecc_err_status_for_0[p/PORTSPERMEM]})
        );

        //Connect host read signals to 4th port
        assign broadcast_readdata=port_readdata[1*WIDTH +: WIDTH];
        assign broadcast_readdatavalid=port_readdatavalid[1];
    end

    assign port_waitrequest[p +: PORTSPERMEM]={PORTSPERMEM{1'b0}};

  end
  endgenerate

  // If broadcast port is shared, track and make sure all memories have written
  always@(posedge clk or negedge aclrn) begin
    if (~aclrn) begin
      done_write<={NUMMEMORIES{1'b0}};
    end else begin
      if (&done_write)
        done_write<={NUMMEMORIES{1'b0}};
      else
        done_write<=done_write | ~sel;
      if (~sclrn[0]) done_write<={NUMMEMORIES{1'b0}};
    end
  end

  // Stall the host and write it again until all memories have written.
  // FIXME: What about host read?
  assign broadcast_waitrequest=(!DEDICATED_BROADCAST_PORT) & |(done_write | ~sel);

  // implement broadcast write data to rdport read data bypass
  // to emulate a READ_DURING_WRITE_MODE_MIXED_PORTS="NEW_DATA" for altsyncram in Stratix 10
  // while only supports "DONT_CARE" for this mode
  genvar x;
  generate
    if (!((FAMILY=="Cyclone V") || (FAMILY=="Stratix V") || (FAMILY=="Arria 10")))
    begin 
      for (x=0; x<NUMPORTS; x=x+1)
      begin : bypass_gen

        // create broadcast_addr comparators
        assign broadcast_addr_comp[x] = (broadcast_addr == rdport_addr[x*LOG2DEPTH +: LOG2DEPTH]) ? broadcast_write : 1'b0; 

        // create rdport_readdata muxes
        assign rdport_readdata_bypass[x*WIDTH +: WIDTH] = broadcast_addr_comp_delayed[x] ? broadcast_writedata_delayed : rdport_readdata_direct[x*WIDTH +: WIDTH]; 

      end

      // delay for broadcast_writedata
      shiftreg broadcast_writedata_delay(.D(broadcast_writedata), .clock(clk), .enable(1'b1), .Q(broadcast_writedata_delayed));
        defparam broadcast_writedata_delay.WIDTH = WIDTH;
        defparam broadcast_writedata_delay.DEPTH = MEMORYBLOCK_LATENCY;

      // delay broadcast_addr comparator outputs
      shiftreg broadcast_addr_comp_delay(.D(broadcast_addr_comp), .clock(clk), .enable(1'b1), .Q(broadcast_addr_comp_delayed));
        defparam broadcast_addr_comp_delay.WIDTH = NUMPORTS;
        defparam broadcast_addr_comp_delay.DEPTH = MEMORYBLOCK_LATENCY;

    end   
  endgenerate

endmodule


module memory_block
#(
  parameter LOG2DEPTH=14,
  parameter WIDTH=32,
  parameter FAMILY="Arria 10",
  parameter enable_ecc = "FALSE",    // Enable error correction coding
  parameter ASYNC_RESET=1,           // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
  parameter SYNCHRONIZE_RESET=0      // set to '1' to pass the incoming reset signal through a synchronizer before use)
)
(
    input  wire clk,
    input  wire clk2x,
    input  wire resetn,
    input  wire [WIDTH-1:0] data_1,
    input  wire [WIDTH-1:0] data_2,
    input  wire [LOG2DEPTH-1:0] addr_1,
    input  wire [LOG2DEPTH-1:0] addr_2,
    input  wire read_1,
    input  wire read_2,
    input  wire write_1,
    input  wire write_2,
    output wire [WIDTH-1:0] data_out_1,
    output wire [WIDTH-1:0] data_out_2,
    output wire valid_1,
    output wire valid_2,
    output wire [1:0] ecc_err_status  // ecc status signals
    );

  localparam DEPTH=2**LOG2DEPTH;
  localparam READ_DURING_WRITE_MODE = ((FAMILY=="Cyclone V") || (FAMILY=="Stratix V") || (FAMILY=="Arria 10")) ? "OLD_DATA" : "DONT_CARE";  // DONT_CARE for altsyncram in Stratix 10 and later families

   localparam                    NUM_RESET_COPIES = 1;
   localparam                    RESET_PIPE_DEPTH = 1;
   logic                         aclrn;
   logic [NUM_RESET_COPIES-1:0]  sclrn;
   logic                         resetn_synchronized;
   acl_reset_handler #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
      .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES)
   ) acl_reset_handler_inst (
      .clk                    (clk),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn),
      .o_sclrn                (sclrn),
      .o_resetn_synchronized  (resetn_synchronized)
   );
  
  shiftreg readatavalid_1(.D(read_1), .clock(clk), .enable(1'b1), .Q(valid_1));
    defparam readatavalid_1.WIDTH = 1;
    defparam readatavalid_1.DEPTH = 2;

  shiftreg readatavalid_2(.D(read_2), .clock(clk), .enable(1'b1), .Q(valid_2));
    defparam readatavalid_2.WIDTH = 1;
    defparam readatavalid_2.DEPTH = 2;

  acl_altera_syncram_wrapped  altsyncram_component (
    .clock0 (clk),
    .wren_a (write_1),
    .wren_b (write_2),
    .address_a (addr_1),
    .address_b (addr_2),
    .data_a (data_1),
    .data_b (data_2),
    .q_a (data_out_1),
    .q_b (data_out_2),
    .aclr0 (~aclrn),
    .aclr1 (1'b0),
    .sclr (~sclrn[0]),
    .addressstall_a (1'b0),
    .addressstall_b (1'b0),
    .byteena_a (1'b1),
    .byteena_b (1'b1),
    .clock1 (1'b1),
    .clocken0 (1'b1),
    .clocken1 (1'b1),
    .ecc_err_status (ecc_err_status),
    .rden_a (1'b1),
    .rden_b (1'b1));
  defparam
  altsyncram_component.address_reg_b = "CLOCK0",
    altsyncram_component.clock_enable_input_a = "BYPASS",
    altsyncram_component.clock_enable_input_b = "BYPASS",
    altsyncram_component.clock_enable_output_a = "BYPASS",
    altsyncram_component.clock_enable_output_b = "BYPASS",
    altsyncram_component.address_reg_b = "CLOCK0",
    altsyncram_component.rdcontrol_reg_b = "CLOCK0",
    altsyncram_component.byteena_reg_b = "CLOCK0",
    altsyncram_component.indata_reg_b = "CLOCK0",
    altsyncram_component.intended_device_family = "Stratix III",
    altsyncram_component.lpm_type = "altsyncram",
    altsyncram_component.ram_block_type = "M20K",
    altsyncram_component.numwords_a = DEPTH,
    altsyncram_component.numwords_b = DEPTH,
    altsyncram_component.operation_mode = "BIDIR_DUAL_PORT",
    altsyncram_component.outdata_aclr_a = "NONE",
    altsyncram_component.outdata_aclr_b = "NONE",
    altsyncram_component.outdata_reg_a = "CLOCK0",
    altsyncram_component.outdata_reg_b = "CLOCK0",
    altsyncram_component.power_up_uninitialized = "FALSE",
    altsyncram_component.read_during_write_mode_mixed_ports = READ_DURING_WRITE_MODE,
    altsyncram_component.read_during_write_mode_port_a = "DONT_CARE",
    altsyncram_component.read_during_write_mode_port_b = "DONT_CARE",
    altsyncram_component.widthad_a = LOG2DEPTH,
    altsyncram_component.widthad_b = LOG2DEPTH,
    altsyncram_component.width_a = WIDTH,
    altsyncram_component.width_b = WIDTH,
    altsyncram_component.width_byteena_a = 1,
    altsyncram_component.width_byteena_b = 1,
    //altsyncram_component.wrcontrol_wraddress_reg_b = "CLOCK0",
    altsyncram_component.connect_clr_to_ram = 0,
    altsyncram_component.enable_ecc = enable_ecc;

endmodule

module memory_block2x
#(
  parameter LOG2DEPTH=14,
  parameter WIDTH=32,
  parameter FAMILY="Arria 10",
  parameter enable_ecc = "FALSE",                       // Enable error correction coding
  parameter ASYNC_RESET=1,                              // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
  parameter SYNCHRONIZE_RESET=0                         // set to '1' to pass the incoming reset signal through a synchronizer before use
)
(
    input  wire clk,
    input  wire clk2x,
    input  wire resetn,
    input  wire [WIDTH-1:0] data_1,
    input  wire [WIDTH-1:0] data_2,
    input  wire [WIDTH-1:0] data_3,
    input  wire [WIDTH-1:0] data_4,
    input  wire [LOG2DEPTH-1:0] addr_1,
    input  wire [LOG2DEPTH-1:0] addr_2,
    input  wire [LOG2DEPTH-1:0] addr_3,
    input  wire [LOG2DEPTH-1:0] addr_4,
    input  wire read_1,
    input  wire read_2,
    input  wire read_3,
    input  wire read_4,
    input  wire write_1,
    input  wire write_2,
    input  wire write_3,
    input  wire write_4,
    output reg  [WIDTH-1:0] data_out_1,
    output reg  [WIDTH-1:0] data_out_2,
    output reg  [WIDTH-1:0] data_out_3,
    output reg  [WIDTH-1:0] data_out_4,
    output wire valid_1,
    output wire valid_2,
    output wire valid_3,
    output wire valid_4,
    output wire [1:0] ecc_err_status  // ecc status signals
    );

  localparam DEPTH=2**LOG2DEPTH;
  localparam READ_DURING_WRITE_MODE = ((FAMILY=="Cyclone V") || (FAMILY=="Stratix V") || (FAMILY=="Arria 10")) ? "OLD_DATA" : "DONT_CARE";  // DONT_CARE for altsyncram in Stratix 10 and later families

  reg [WIDTH-1:0] data_1_reg /* synthesis dont_merge */;
  reg [WIDTH-1:0] data_2_reg /* synthesis dont_merge */;
  reg [WIDTH-1:0] data_3_reg /* synthesis dont_merge */;
  reg [WIDTH-1:0] data_4_reg /* synthesis dont_merge */;
  reg [LOG2DEPTH-1:0] addr_1_reg /* synthesis dont_merge */;
  reg [LOG2DEPTH-1:0] addr_2_reg /* synthesis dont_merge */;
  reg [LOG2DEPTH-1:0] addr_3_reg /* synthesis dont_merge */;
  reg [LOG2DEPTH-1:0] addr_4_reg /* synthesis dont_merge */;
  reg write_1_reg, write_2_reg /* synthesis maxfan=32 */;
  reg write_3_reg, write_4_reg /* sytnthesis maxfan=32 */;
  wire [WIDTH-1:0] data_out_a_unreg;
  wire [WIDTH-1:0] data_out_b_unreg;
  reg  [WIDTH-1:0] data_out_a_reg;
  reg  [WIDTH-1:0] data_out_b_reg;
  reg  [WIDTH-1:0] data_out_a_reg2;
  reg  [WIDTH-1:0] data_out_b_reg2;

  reg [WIDTH-1:0] data_1_reg2x;
  reg [WIDTH-1:0] data_2_reg2x;
  reg [WIDTH-1:0] data_3_reg2x;
  reg [WIDTH-1:0] data_4_reg2x;
  reg [LOG2DEPTH-1:0] addr_1_reg2x;
  reg [LOG2DEPTH-1:0] addr_2_reg2x;
  reg [LOG2DEPTH-1:0] addr_3_reg2x;
  reg [LOG2DEPTH-1:0] addr_4_reg2x;
  reg write_1_reg2x, write_2_reg2x;
  reg write_3_reg2x, write_4_reg2x;

  reg clk_1x_toggle             /* synthesis dont_merge */;      // toggles on every rising edge of clk
  reg clk_1x_toggle_clk2x       /* synthesis dont_merge */;      // clk_1x_toggle clocked onto the clk2x clock
  reg clk_1x_toggle_clk2x_dly   /* synthesis dont_merge */;      // clk_1x_toggle_clk2x delyaed by one clk2x clock cycle
  reg sel2x                     /* sytnthesis maxfan=32 */;      // sel2x = ~clk, output synchronous with clk2x

   localparam                    NUM_RESET_COPIES = 1;
   localparam                    RESET_PIPE_DEPTH = 1;
   logic                         aclrn;
   logic [NUM_RESET_COPIES-1:0]  sclrn;
   logic                         resetn_synchronized;
   acl_reset_handler #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
      .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES)
   ) acl_reset_handler_inst (
      .clk                    (clk),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn),
      .o_sclrn                (sclrn),
      .o_resetn_synchronized  (resetn_synchronized)
   );

   logic                         aclrn_2x;
   logic [NUM_RESET_COPIES-1:0]  sclrn_2x;
   acl_reset_handler #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (1),
      .SYNCHRONIZE_ACLRN      (1),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES)
   ) acl_reset_handler_clk2x_inst (
      .clk                    (clk2x),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn_2x),
      .o_sclrn                (sclrn_2x),
      .o_resetn_synchronized  ()
   );

   
  
  always @(posedge clk or negedge aclrn) begin
    if (~aclrn) begin                    // reset only necessary for simulation, in real HW phase of this signal is not relevant
      clk_1x_toggle <= 1'b0;
    end else begin
      clk_1x_toggle <= ~clk_1x_toggle;    // toggle on every cycle of clk
      if (~sclrn[0]) clk_1x_toggle <= 1'b0;
    end
  end
  
  always @(posedge clk2x) begin
    clk_1x_toggle_clk2x <= clk_1x_toggle;                    // bring clk_1x_toggle into the clk2x clock domain
    clk_1x_toggle_clk2x_dly <= clk_1x_toggle_clk2x;          // delay by one clk2x clock cycle
    if (clk_1x_toggle_clk2x == clk_1x_toggle_clk2x_dly ) begin
      sel2x <= 1'b1;
    end else begin
      sel2x <= 1'b0;
    end
  end
  
  
  //Register before double pumping
  always@(posedge clk)
  begin
    addr_1_reg <= addr_1;
    addr_2_reg <= addr_2;
    addr_3_reg <= addr_3;
    addr_4_reg <= addr_4;
    data_1_reg <= data_1;
    data_2_reg <= data_2;
    data_3_reg <= data_3;
    data_4_reg <= data_4;
    write_1_reg <= write_1;
    write_2_reg <= write_2;
    write_3_reg <= write_3;
    write_4_reg <= write_4;
  end

  // Consider making only one port r/w and the rest read only
  always@(posedge clk2x)
  begin
    addr_1_reg2x <= (!sel2x) ? addr_1_reg : addr_3_reg2x;
    addr_2_reg2x <= (!sel2x) ? addr_2_reg : addr_4_reg2x;
    addr_3_reg2x <= addr_3_reg;
    addr_4_reg2x <= addr_4_reg;
    data_1_reg2x <= (!sel2x) ? data_1_reg : data_3_reg2x;
    data_2_reg2x <= (!sel2x) ? data_2_reg : data_4_reg2x;
    data_3_reg2x <= data_3_reg;
    data_4_reg2x <= data_4_reg;
    write_1_reg2x <= (!sel2x) ? write_1_reg : write_3_reg2x;
    write_2_reg2x <= (!sel2x) ? write_2_reg : write_4_reg2x;
    write_3_reg2x <= write_3_reg;
    write_4_reg2x <= write_4_reg;
  end

  shiftreg readatavalid_1(.D(read_1), .clock(clk), .enable(1'b1), .Q(valid_1));
    defparam readatavalid_1.WIDTH = 1;
    defparam readatavalid_1.DEPTH = 4;

  shiftreg readatavalid_2(.D(read_2), .clock(clk), .enable(1'b1), .Q(valid_2));
    defparam readatavalid_2.WIDTH = 1;
    defparam readatavalid_2.DEPTH = 4;

  shiftreg readatavalid_3(.D(read_3), .clock(clk), .enable(1'b1), .Q(valid_3));
    defparam readatavalid_3.WIDTH = 1;
    defparam readatavalid_3.DEPTH = 4;

  shiftreg readatavalid_4(.D(read_4), .clock(clk), .enable(1'b1), .Q(valid_4));
    defparam readatavalid_4.WIDTH = 1;
    defparam readatavalid_4.DEPTH = 4;


  acl_altera_syncram_wrapped altsyncram_component (
    .clock0 (clk2x),
    .wren_a (write_1_reg2x),
    .wren_b (write_2_reg2x),
    .address_a (addr_1_reg2x),
    .address_b (addr_2_reg2x),
    .data_a (data_1_reg2x),
    .data_b (data_2_reg2x),
    .q_a (data_out_a_unreg),
    .q_b (data_out_b_unreg),
    .aclr0 (~aclrn_2x),
    .aclr1 (1'b0),
    .sclr (~sclrn_2x[0]),
    .addressstall_a (1'b0),
    .addressstall_b (1'b0),
    .byteena_a (1'b1),
    .byteena_b (1'b1),
    .clock1 (1'b1),
    .clocken0 (1'b1),
    .clocken1 (1'b1),
    .ecc_err_status (ecc_err_status),
    .rden_a (1'b1),
    .rden_b (1'b1));
  defparam
  altsyncram_component.address_reg_b = "CLOCK0",
    altsyncram_component.clock_enable_input_a = "BYPASS",
    altsyncram_component.clock_enable_input_b = "BYPASS",
    altsyncram_component.clock_enable_output_a = "BYPASS",
    altsyncram_component.clock_enable_output_b = "BYPASS",
    altsyncram_component.address_reg_b = "CLOCK0",
    altsyncram_component.rdcontrol_reg_b = "CLOCK0",
    altsyncram_component.byteena_reg_b = "CLOCK0",
    altsyncram_component.indata_reg_b = "CLOCK0",
    altsyncram_component.intended_device_family = "Stratix III",
    altsyncram_component.lpm_type = "altsyncram",
    altsyncram_component.ram_block_type = "M20K",
    altsyncram_component.numwords_a = DEPTH,
    altsyncram_component.numwords_b = DEPTH,
    altsyncram_component.operation_mode = "BIDIR_DUAL_PORT",
    altsyncram_component.outdata_aclr_a = "NONE",
    altsyncram_component.outdata_aclr_b = "NONE",
    altsyncram_component.outdata_reg_a = "CLOCK0",
    altsyncram_component.outdata_reg_b = "CLOCK0",
    altsyncram_component.power_up_uninitialized = "FALSE",
    altsyncram_component.read_during_write_mode_mixed_ports = READ_DURING_WRITE_MODE,
    altsyncram_component.read_during_write_mode_port_a = "DONT_CARE",
    altsyncram_component.read_during_write_mode_port_b = "DONT_CARE",
    altsyncram_component.widthad_a = LOG2DEPTH,
    altsyncram_component.widthad_b = LOG2DEPTH,
    altsyncram_component.width_a = WIDTH,
    altsyncram_component.width_b = WIDTH,
    altsyncram_component.width_byteena_a = 1,
    altsyncram_component.width_byteena_b = 1,
    altsyncram_component.connect_clr_to_ram = 0,
    altsyncram_component.enable_ecc = enable_ecc;
    //altsyncram_component.wrcontrol_wraddress_reg_b = "CLOCK0";

  always@(posedge clk2x)
  begin
    data_out_a_reg<=data_out_a_unreg;
    data_out_b_reg<=data_out_b_unreg;
    data_out_a_reg2<=data_out_a_reg;
    data_out_b_reg2<=data_out_b_reg;
  end

  always@(posedge clk)
  begin
    data_out_1 <= data_out_a_reg2;
    data_out_2 <= data_out_b_reg2;
    data_out_3 <= data_out_a_reg;
    data_out_4 <= data_out_b_reg;
  end

endmodule


/*********************************************************************************
 * Support components
 *********************************************************************************/

module shiftreg(D, clock, enable, Q);
	parameter WIDTH = 32;
	parameter DEPTH = 1;
	input wire [WIDTH-1:0] D;
	input wire clock, enable;
	output wire [WIDTH-1:0] Q;
	
	reg [WIDTH-1:0] local_ffs [0:DEPTH-1];
	
	genvar i;
	generate
		for(i = 0; i<=DEPTH-1; i = i+1) 
		begin : local_register
			always @(posedge clock)
				if (enable)
					if (i==0)
						local_ffs[0] <= D;
					else
						local_ffs[i] <= local_ffs[i-1];
		end
	endgenerate
	assign Q = local_ffs[DEPTH-1];
endmodule

`default_nettype wire
