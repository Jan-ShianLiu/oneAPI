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


/**********************
* Snoop data path
*   1) First buffer in dc fifo since snoops are on another clock domain
*   2) Filter out requests beyond the currently cached address range
*   3) Break up bursts
*   4) Check if there's a tagmatch
*   5) If so, issue invalidation to cache
**********************/

`default_nettype none

module acl_snoop
#(
  parameter LOG2SIZE=10,       //in bytes
  parameter LOG2WIDTH=5,       //in bits
  parameter AWIDTH=32,         //Word address width
  parameter WIDTH=2**LOG2WIDTH,
  parameter BURSTWIDTH=6,
  parameter enable_ecc = "FALSE",                       // Enable error correction coding
  parameter ASYNC_RESET=1,                              // set to '1' to consume the incoming reset signal asynchronously (use ACLR port on registers), '0' to use synchronous reset (SCLR port on registers)
  parameter SYNCHRONIZE_RESET=0                         // set to '1' to pass the incoming reset signal through a synchronizer before use
)
(
  input  wire clk,
  input  wire resetn,
  input  wire flush,

  // Cache fill port interface - must keep tag filter synced w/ cache
  input  wire [AWIDTH-1:0]       fill_readdata_addr,  //word-address
  input  wire                    fill_readdatavalid,

  // Snoop port interface - these are the addresses being modified
  input  wire [AWIDTH-1:0]           snoop_addr,  //word-address
  input  wire [BURSTWIDTH-1:0]       snoop_burst,
  input  wire                        snoop_valid,
  input  wire                        snoop_overflow, // Only used without CLOCKCROSSFIFO
  output wire                        snoop_stall,

  // Invalidation interface - these cache lines should be invalidated
  output reg [AWIDTH-1:0]        invalidate_addr,  //word-address
  output reg                     invalidate_en,
  input  wire                    invalidate_waitrequest,

  output reg                     overflow,
  output logic  [1:0] ecc_err_status // ecc status signals
);

  /******************
  * LOCAL PARAMETERS
  *******************/

  localparam LOG2NUMCACHEENTRIES=LOG2SIZE-(LOG2WIDTH-3);

  // Address is byte address
  //localparam TAGSIZE=AWIDTH-LOG2SIZE;
  //`define TAGRANGE AWIDTH-1:LOG2SIZE
  //`define OFFSETRANGE LOG2SIZE-1:LOG2WIDTH-3

  // Address is word address (word size defined by WIDTH)
  localparam TAGSIZE=AWIDTH-(LOG2SIZE-LOG2WIDTH+3);
  `define TAGRANGE AWIDTH-1:LOG2SIZE-LOG2WIDTH+3
  `define OFFSETRANGE LOG2SIZE-(LOG2WIDTH-3)-1:0

  localparam DEVICE_BLOCKRAM_BITS = 8192; //Stratix IV M9Ks

  localparam FIFO_SIZE = DEVICE_BLOCKRAM_BITS*2/(AWIDTH+BURSTWIDTH);
  localparam LOG2_FIFO_SIZE =$clog2(FIFO_SIZE);

  /******************
  * SIGNALS
  *******************/


  // Snoop signals 
  wire [AWIDTH-1:0] snoop_fifo_addr;
  wire [BURSTWIDTH-1:0] snoop_fifo_burst;
  wire snoop_fifo_valid;
  wire snoop_fifo_empty;
  reg               fill_readdatavalid_r;
  reg  [AWIDTH-1:0] fill_readdata_addr_r;
  reg  [AWIDTH-1:0] max_fill_readdata_addr;
  reg  [AWIDTH-1:0] min_fill_readdata_addr;

  // Snoop request after filtering
  reg   [AWIDTH-1:0]           filtered_addr;  //word-address
  reg   [BURSTWIDTH-1:0]       filtered_burst; 
  reg                          filtered_en;
  wire                         filtered_waitrequest;

  // Break up bursts
  reg   [AWIDTH-1:0]           breakup_addr;  //word-address
  reg   [BURSTWIDTH-1:0]       breakup_burst; 
  reg                          breakup_en;
  wire                         breakup_waitrequest;
  wire  [TAGSIZE-1:0]          tag_out;  //word-address

  // Cache check 
  reg   [AWIDTH-1:0]           cache_addr;  //word-address
  reg                          cache_en;
  wire                         cache_waitrequest;

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

  // Capture max and min addresses that have ever been cached.  We can use
  // these to filter out snoop requests that will for sure miss in the cache
  always@(posedge clk)
    fill_readdata_addr_r <= fill_readdata_addr;
  always@(posedge clk)
    fill_readdatavalid_r <= fill_readdatavalid;

  always@(posedge clk or negedge aclrn) begin
    if (~aclrn) begin
      max_fill_readdata_addr<='0;
      min_fill_readdata_addr<='1;
    end else begin
      if (flush)
      begin
        max_fill_readdata_addr<='0;
        min_fill_readdata_addr<='1;
      end
      else if (fill_readdatavalid_r)
      begin
        if ( fill_readdata_addr_r > max_fill_readdata_addr )
          max_fill_readdata_addr<= fill_readdata_addr_r;
        if ( fill_readdata_addr_r < min_fill_readdata_addr )
          min_fill_readdata_addr<= fill_readdata_addr_r;
      end
      if (~sclrn[0]) begin
        max_fill_readdata_addr<='0;
        min_fill_readdata_addr<='1;
      end
    end
  end

  // Register snoop data from iface
  reg  [AWIDTH-1:0]     snoop_addr_r;  //word-address
  reg  [BURSTWIDTH-1:0] snoop_burst_r;
  reg                   snoop_valid_r;
  always@(posedge clk or negedge aclrn) begin
    if (~aclrn)
    begin
      snoop_addr_r<={AWIDTH{1'b0}};
      snoop_burst_r<={BURSTWIDTH{1'b0}};
      snoop_valid_r<=1'b0;
    end else begin
      if (!snoop_stall)
      begin
        snoop_addr_r<=snoop_addr;
        snoop_burst_r<=snoop_burst;
        snoop_valid_r<=snoop_valid;
      end
      if (~sclrn[0]) begin
        //snoop_addr_r<={AWIDTH{1'b0}};
        //snoop_burst_r<={BURSTWIDTH{1'b0}};
        snoop_valid_r<=1'b0;
      end
    end
  end

  assign snoop_fifo_addr = snoop_addr_r;
  assign snoop_fifo_burst = snoop_burst_r;
  assign snoop_fifo_valid = snoop_valid_r;
  assign snoop_stall = filtered_waitrequest && !flush;  // Don't stall if flushing
  always@(posedge clk) 
    overflow = snoop_overflow;

  // 2) Filter out snoops beyond cached address range
  assign filtered_waitrequest=filtered_en && breakup_waitrequest;
  always@(posedge clk or negedge aclrn) 
  begin
    if (~aclrn)
    begin
      filtered_addr <= {AWIDTH{1'b0}};
      filtered_burst <= {BURSTWIDTH{1'b0}};
      filtered_en <= 1'b0;
    end else begin
      if ((~filtered_en) || ~breakup_waitrequest)
      begin
        filtered_addr <= snoop_fifo_addr;  //word-address
        filtered_burst <= snoop_fifo_burst;
        filtered_en <= snoop_fifo_valid &&
          !((snoop_fifo_addr + snoop_fifo_burst <= min_fill_readdata_addr) ||
            (snoop_fifo_addr > max_fill_readdata_addr));
      end
      if (~sclrn[0]) begin
        //filtered_addr <= {AWIDTH{1'b0}};
        //filtered_burst <= {BURSTWIDTH{1'b0}};
        filtered_en <= 1'b0;
      end
    end
  end

  // 3) Break up bursts into single cache line accesses (assume cache line size
  // matches snooped data bus width)
  assign breakup_waitrequest=breakup_en && !(breakup_burst==1 && ~cache_waitrequest);
  always@(posedge clk or negedge aclrn) 
  begin
    if (~aclrn)
    begin
      breakup_addr <= {AWIDTH{1'b0}};
      breakup_burst <= {BURSTWIDTH{1'b0}};
      breakup_en <= 1'b0;
    end else begin
      if ((~breakup_en) || breakup_burst==1 && ~cache_waitrequest)
      begin
        breakup_addr <= filtered_addr;
        breakup_burst <= filtered_burst;
        breakup_en <= filtered_en;
      end
      else if (breakup_en && !invalidate_waitrequest)
      begin
        breakup_addr <= breakup_addr+1;
        breakup_burst <= breakup_burst-1;
      end
      if (~sclrn[0]) begin
        //breakup_addr <= {AWIDTH{1'b0}};
        //breakup_burst <= {BURSTWIDTH{1'b0}};
        breakup_en <= 1'b0;
      end
    end
  end

  // 4) Check if tags are in cache - must keep this synchronized with cache's tags
  assign cache_waitrequest=cache_en && invalidate_waitrequest;
  always@(posedge clk or negedge aclrn) 
  begin
    if (~aclrn)
    begin
      cache_addr <= {AWIDTH{1'b0}};  //word-address
      cache_en <= 1'b0;
    end
    else 
    begin
      cache_addr <= breakup_addr;  //word-address
      cache_en <= breakup_en && breakup_burst>=1;
      if (~sclrn[0]) begin
        //cache_addr <= {AWIDTH{1'b0}};  //word-address
        cache_en <= 1'b0;
      end
    end
  end

  acl_altera_syncram_wrapped  tags (
    .clock0 (clk),
    .wren_a (fill_readdatavalid),
    .address_a (fill_readdata_addr[`OFFSETRANGE]),
    .data_a (fill_readdata_addr[`TAGRANGE]),
    .address_b (breakup_addr[`OFFSETRANGE]),
    .q_b (tag_out),
    .addressstall_b (invalidate_waitrequest),
    .q_a (),
    .data_b (),
    .wren_b (),
    .aclr0 (~aclrn),
    .aclr1 (1'b0),
    .sclr (~sclrn[0]),
    .addressstall_a (1'b0),
    .byteena_a (1'b1),
    .byteena_b (1'b1),
    .clock1 (1'b1),
    .clocken0 (1'b1),
    .clocken1 (1'b1),
    .ecc_err_status (ecc_err_status),
    .rden_a (1'b1),
    .rden_b (1'b1));
  defparam tags.address_reg_b = "CLOCK0",
    tags.clock_enable_input_a = "BYPASS",
    tags.clock_enable_input_b = "BYPASS",
    tags.clock_enable_output_a = "BYPASS",
    tags.clock_enable_output_b = "BYPASS",
    tags.address_reg_b = "CLOCK0",
    tags.rdcontrol_reg_b = "CLOCK0",
    tags.byteena_reg_b = "CLOCK0",
    tags.indata_reg_b = "CLOCK0",
    tags.intended_device_family = "Stratix IV",
    tags.lpm_type = "altsyncram",
    tags.operation_mode = "DUAL_PORT",
    tags.outdata_aclr_a = "NONE",
    tags.outdata_aclr_b = "NONE",
    tags.outdata_reg_a = "UNREGISTERED",
    tags.outdata_reg_b = "UNREGISTERED",
    tags.power_up_uninitialized = "FALSE",
    tags.read_during_write_mode_mixed_ports = "DONT_CARE",
    tags.read_during_write_mode_port_a = "DONT_CARE",
    tags.read_during_write_mode_port_b = "DONT_CARE",
    tags.widthad_a = LOG2NUMCACHEENTRIES,
    tags.widthad_b = LOG2NUMCACHEENTRIES,
    tags.width_a = TAGSIZE,
    tags.width_b = TAGSIZE,
    tags.width_byteena_a = 1,
    tags.width_byteena_b = 1,
    //tags.wrcontrol_wraddress_reg_b = "CLOCK0",
    tags.numwords_a = (2**LOG2NUMCACHEENTRIES),
    tags.numwords_b = (2**LOG2NUMCACHEENTRIES),
    tags.connect_clr_to_ram = 0,
    tags.enable_ecc = enable_ecc;

  // 5) If the tag matches, invalidate this entry
  always@(posedge clk or negedge aclrn) 
  begin
    if (~aclrn)
    begin
      invalidate_addr <= {AWIDTH{1'b0}};  
      invalidate_en <= 1'b0;
    end
    else 
    begin
      invalidate_addr <= cache_addr;  
      invalidate_en <= cache_en && (tag_out==cache_addr[`TAGRANGE]);
      if (~sclrn[0]) begin
        //invalidate_addr <= {AWIDTH{1'b0}};  
        invalidate_en <= 1'b0;
      end
    end
  end


endmodule

`default_nettype wire