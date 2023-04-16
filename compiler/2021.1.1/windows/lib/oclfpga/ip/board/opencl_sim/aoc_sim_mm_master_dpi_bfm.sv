// BFM is responsible to transfering host MMD commands to kernel interface and memory bank divider
// Design constraints:
// - Kernel Interface Avalon transfers only supports 32-bit word, constraint code in msim DPI
// - Memory Bank Divider Avalon transfer can only transfer 1 burst of EMIF word width, i.e. 256 or 512.
//   The constraint is due to EMIF address translation can only do 1 Avalon address word at time during
//   bypass mode to allow ECC computation. Sending multiple burst means it would need to keep track of 
//   a counter which complicates the user API.
//BFM hierachy
`define KI_MSTR_BFM ki_bfm
`define MBD_MSTR_BFM mbd_bfm

module aoc_sim_mm_master_dpi_bfm(
                                  // clock, reset
                                  clock,
                                  reset_n,

                                  // Kernel Interface Avalon MM-master
                                  ki_avm_waitrequest,
                                  ki_avm_write,
                                  ki_avm_read,
                                  ki_avm_address,
                                  ki_avm_byteenable,
                                  ki_avm_burstcount,
                                  ki_avm_writedata,
                                  ki_avm_readdata,
                                  ki_avm_readdatavalid,

                                  // Memory bank divider Avalon MM-master
                                  mbd_avm_waitrequest,
                                  mbd_avm_write,
                                  mbd_avm_read,
                                  mbd_avm_address,
                                  mbd_avm_byteenable,
                                  mbd_avm_burstcount,
                                  mbd_avm_writedata,
                                  mbd_avm_readdata,
                                  mbd_avm_readdatavalid
                                  );

   //importing verbosity and avalon_mm packages
   import verbosity_pkg::*;
   import avalon_mm_pkg::*;

   parameter COMPONENT_NAME             = "dut";

   // Kernel Interface specific port parameters and properties
   parameter KI_AV_ADDRESS_W            = 16; // Address width
   parameter KI_AV_SYMBOL_W             = 8;  // Symbol width (default is byte)
   parameter KI_AV_NUMSYMBOLS           = 4;  // Number of symbols per word
   parameter KI_AV_BURSTCOUNT_W         = 1;  // kernel interface burst port - not supported by SimulatorInterface
   // port parameters
   parameter KI_USE_WAIT_REQUEST        = 1;  // Use waitrequest interface pin

   // Memory Bank Divider specific port parameters and properties
   parameter MBD_AV_ADDRESS_W           = 31; // Address width
   parameter MBD_AV_SYMBOL_W            = 8;  // Symbol width (default is byte)
   parameter MBD_AV_NUMSYMBOLS          = 4;  // Number of symbols per word
   parameter MBD_AV_BURSTSIZE           = 16;  // The maximum number of data it can send in one command, must power of 2 restricted by HW tcl

   // port parameters
   parameter MBD_USE_WAIT_REQUEST       = 1;  // Use waitrequest interface pin

   // Common parameters between Kernel Interface and Memory Bank Divider
   // Not supported parameters can only be set via instantiating the IP as a standalone and not through QSYS
   // port parameters
   parameter USE_READ                   = 1;  // Use read interface pin
   parameter USE_WRITE                  = 1;  // Use write interface pin
   parameter USE_ADDRESS                = 1;  // Use address interface pinsp
   parameter USE_BYTE_ENABLE            = 1;  // Use byte_enable interface pins
   parameter USE_READ_DATA              = 1;  // Use readdata interface pin
   parameter USE_READ_DATA_VALID        = 1;  // Use readdatavalid interface pin
   parameter USE_WRITE_DATA             = 1;  // Use writedata interface pin
   parameter USE_BEGIN_TRANSFER         = 0;  // Use begintransfer interface pin - not supported by SimulatorInterface
   parameter USE_BEGIN_BURST_TRANSFER   = 0;  // Use beginbursttransfer interface pin - not supported by SimulatorInterface

   parameter AV_FIX_READ_LATENCY        = 0;  // Fixed read latency (cycles)

   parameter AV_BURST_LINEWRAP          = 0;  // Line wrap bursts (y/n)
   parameter AV_BURST_BNDR_ONLY         = 0;  // Assert Addr alignment

   parameter AV_READ_WAIT_TIME          = 0;  // Fixed wait time cycles when
   parameter AV_WRITE_WAIT_TIME         = 0;  // USE_WAIT_REQUEST is 0
   parameter REGISTER_WAITREQUEST       = 0;  //
   parameter AV_REGISTERINCOMINGSIGNALS = 0; // Indicate that waitrequest is come from register

   parameter COMMAND_WAIT_CYCLES        = 1000; // How many cycles to delay when there is no command
                                                // from the host to execute.
   parameter COMMAND_INITIAL_WAIT_CYCLES = 5000; // How many cycles to delay at the start of the simulation.
   parameter COMMAND_READ_WRITE_CYCLES  = 2; // How many cycles to delay after a read/write.

   // TODO: case:520397: add interleaving and multiple banks for memory controller bypass and accurate memory
   // parameter NUM_MEMORY_BANKS         = 1; // Number of memory banks
   // parameter INTERLEAVING_CHUNK_SIZE  = 1; // size in kilobytes; default is 1. Ignore if NUM_MEMORY_BANKS == 1

   localparam KI_AV_DATA_W  = (KI_AV_SYMBOL_W * KI_AV_NUMSYMBOLS);
   localparam MBD_AV_DATA_W = (MBD_AV_SYMBOL_W * MBD_AV_NUMSYMBOLS);
   localparam MBD_AV_BURSTCOUNT_W = $clog2(MBD_AV_BURSTSIZE) + 1;

   // abstract PHY address width - temporary workaround to access the abstract PHY memory storage element
   localparam ABPHY_ADDR_W               = 44;  // s10gx: high_addr[5:0],temp_addr[37:0]

   // Commands from the host.
   typedef enum { KERNEL_READ, KERNEL_WRITE, TERMINATE, NO_WORK, MEMORY_READ, MEMORY_WRITE } Command;

   import "DPI-C" context function void __ihc_hls_dbgs(string msg);
   import "DPI-C" function Command __ihc_aoc_get_command(output int address, output int data,
                                                         output int size);
   import "DPI-C" function void __ihc_aoc_return_kernel_read_data(int data);
   import "DPI-C" context function int __ihc_aoc_mm_slave_read(input int id, input longint address,
                                                               output bit [MBD_AV_DATA_W-1:0] data, input int size);
   import "DPI-C" context function void __ihc_aoc_mm_slave_write(input int id, input longint address,
                                                                 input bit [MBD_AV_DATA_W-1:0] data,
                                                                 input bit [MBD_AV_NUMSYMBOLS-1:0] byte_enable,
                                                                 input int size);

   function int lindex;
      // returns the left index for a vector having a declared width
      // when width is 0, then the left index is set to 0 rather than -1
      input [31:0] width;
      lindex = (width > 0) ? (width-1) : 0;
   endfunction

   input                                            clock;
   input                                            reset_n;

   output                                                   ki_avm_waitrequest;
   output                                                   ki_avm_readdatavalid;
   output [lindex(KI_AV_SYMBOL_W * KI_AV_NUMSYMBOLS):0]     ki_avm_readdata;
   input                                                    ki_avm_write;
   input                                                    ki_avm_read;
   input  [lindex(KI_AV_ADDRESS_W):0]                       ki_avm_address;
   input  [lindex(KI_AV_NUMSYMBOLS):0]                      ki_avm_byteenable;
   input  [lindex(KI_AV_BURSTCOUNT_W):0]                    ki_avm_burstcount;
   input  [lindex(KI_AV_SYMBOL_W * KI_AV_NUMSYMBOLS):0]     ki_avm_writedata;

   output                                                   mbd_avm_waitrequest;
   output                                                   mbd_avm_readdatavalid;
   output [lindex(MBD_AV_SYMBOL_W * MBD_AV_NUMSYMBOLS):0]   mbd_avm_readdata;
   input                                                    mbd_avm_write;
   input                                                    mbd_avm_read;
   input  [lindex(MBD_AV_ADDRESS_W):0]                      mbd_avm_address;
   input  [lindex(MBD_AV_NUMSYMBOLS):0]                     mbd_avm_byteenable;
   input  [lindex(MBD_AV_BURSTCOUNT_W):0]                   mbd_avm_burstcount;
   input  [lindex(MBD_AV_SYMBOL_W * MBD_AV_NUMSYMBOLS):0]   mbd_avm_writedata;

  altera_avalon_mm_master_bfm #(
    .AV_ADDRESS_W               (KI_AV_ADDRESS_W         ),
    .AV_SYMBOL_W                (KI_AV_SYMBOL_W          ),
    .AV_NUMSYMBOLS              (KI_AV_NUMSYMBOLS        ),
    .AV_BURSTCOUNT_W            (KI_AV_BURSTCOUNT_W      ),
    .USE_READ                   (USE_READ                ),
    .USE_WRITE                  (USE_WRITE               ),
    .USE_ADDRESS                (USE_ADDRESS             ),
    .USE_BYTE_ENABLE            (USE_BYTE_ENABLE         ),
    .USE_BURSTCOUNT             (0),
    .USE_READ_DATA              (USE_READ_DATA           ),
    .USE_READ_DATA_VALID        (USE_READ_DATA_VALID     ),
    .USE_WRITE_DATA             (USE_WRITE_DATA          ),
    .USE_BEGIN_TRANSFER         (USE_BEGIN_TRANSFER      ),
    .USE_BEGIN_BURST_TRANSFER   (USE_BEGIN_BURST_TRANSFER),
    .USE_WAIT_REQUEST           (KI_USE_WAIT_REQUEST     ),
    .USE_TRANSACTIONID          (0),
    .USE_WRITERESPONSE          (0),
    .USE_READRESPONSE           (0),
    .USE_CLKEN                  (0),
    .AV_CONSTANT_BURST_BEHAVIOR (1),
    .AV_BURST_LINEWRAP          (0),
    .AV_BURST_BNDR_ONLY         (0),
    .AV_MAX_PENDING_READS       (0),
    .AV_MAX_PENDING_WRITES      (0),
    .AV_FIX_READ_LATENCY        (AV_FIX_READ_LATENCY),
    .AV_READ_WAIT_TIME          (AV_READ_WAIT_TIME         ),
    .AV_WRITE_WAIT_TIME         (AV_WRITE_WAIT_TIME        ),
    .REGISTER_WAITREQUEST       (REGISTER_WAITREQUEST      ),
    .AV_REGISTERINCOMINGSIGNALS (AV_REGISTERINCOMINGSIGNALS),
    .VHDL_ID                    (0)
  ) ki_bfm (
    .clk                    (clock),                     //       clk.clk
    .reset                  (~reset_n),                  // clk_reset.reset
    .avm_address            (ki_avm_address),               //        m0.address
    .avm_burstcount         (ki_avm_burstcount),            //          .burstcount
    .avm_readdata           (ki_avm_readdata),              //          .readdata
    .avm_writedata          (ki_avm_writedata),             //          .writedata
    .avm_waitrequest        (ki_avm_waitrequest),           //          .waitrequest
    .avm_write              (ki_avm_write),                 //          .write
    .avm_read               (ki_avm_read),                  //          .read
    .avm_byteenable         (ki_avm_byteenable),            //          .byteenable
    .avm_readdatavalid      (ki_avm_readdatavalid),         //          .readdatavalid
    .avm_begintransfer      (),                          // (terminated)
    .avm_beginbursttransfer (),                          // (terminated)
    .avm_arbiterlock        (),                          // (terminated)
    .avm_lock               (),                          // (terminated)
    .avm_debugaccess        (),                          // (terminated)
    .avm_transactionid      (),                          // (terminated)
    .avm_readid             (8'b00000000),               // (terminated)
    .avm_writeid            (8'b00000000),               // (terminated)
    .avm_clken              (),                          // (terminated)
    .avm_response           (2'b00),                     // (terminated)
    .avm_writeresponsevalid (1'b0),                      // (terminated)
    .avm_readresponse       (8'b00000000),               // (terminated)
    .avm_writeresponse      (8'b00000000)                // (terminated)
  );

  altera_avalon_mm_master_bfm #(
    .AV_ADDRESS_W               (MBD_AV_ADDRESS_W        ),
    .AV_SYMBOL_W                (MBD_AV_SYMBOL_W         ),
    .AV_NUMSYMBOLS              (MBD_AV_NUMSYMBOLS       ),
    .AV_BURSTCOUNT_W            (MBD_AV_BURSTCOUNT_W     ),
    .USE_READ                   (USE_READ                ),
    .USE_WRITE                  (USE_WRITE               ),
    .USE_ADDRESS                (USE_ADDRESS             ),
    .USE_BYTE_ENABLE            (USE_BYTE_ENABLE         ),
    .USE_BURSTCOUNT             (1),
    .USE_READ_DATA              (USE_READ_DATA           ),
    .USE_READ_DATA_VALID        (USE_READ_DATA_VALID     ),
    .USE_WRITE_DATA             (USE_WRITE_DATA          ),
    .USE_BEGIN_TRANSFER         (USE_BEGIN_TRANSFER      ),
    .USE_BEGIN_BURST_TRANSFER   (USE_BEGIN_BURST_TRANSFER),
    .USE_WAIT_REQUEST           (MBD_USE_WAIT_REQUEST    ),
    .USE_TRANSACTIONID          (0),
    .USE_WRITERESPONSE          (0),
    .USE_READRESPONSE           (0),
    .USE_CLKEN                  (0),
    .AV_CONSTANT_BURST_BEHAVIOR (1),
    .AV_BURST_LINEWRAP          (0),
    .AV_BURST_BNDR_ONLY         (0),
    .AV_MAX_PENDING_READS       (0),
    .AV_MAX_PENDING_WRITES      (0),
    .AV_FIX_READ_LATENCY        (AV_FIX_READ_LATENCY),
    .AV_READ_WAIT_TIME          (AV_READ_WAIT_TIME         ),
    .AV_WRITE_WAIT_TIME         (AV_WRITE_WAIT_TIME        ),
    .REGISTER_WAITREQUEST       (REGISTER_WAITREQUEST      ),
    .AV_REGISTERINCOMINGSIGNALS (AV_REGISTERINCOMINGSIGNALS),
    .VHDL_ID                    (0)
  ) mbd_bfm (
    .clk                    (clock),                     //       clk.clk
    .reset                  (~reset_n),                  // clk_reset.reset
    .avm_address            (mbd_avm_address),           //        m1.address
    .avm_burstcount         (mbd_avm_burstcount),        //          .burstcount
    .avm_readdata           (mbd_avm_readdata),          //          .readdata
    .avm_writedata          (mbd_avm_writedata),         //          .writedata
    .avm_waitrequest        (mbd_avm_waitrequest),       //          .waitrequest
    .avm_write              (mbd_avm_write),             //          .write
    .avm_read               (mbd_avm_read),              //          .read
    .avm_byteenable         (mbd_avm_byteenable),        //          .byteenable
    .avm_readdatavalid      (mbd_avm_readdatavalid),     //          .readdatavalid
    .avm_begintransfer      (),                          // (terminated)
    .avm_beginbursttransfer (),                          // (terminated)
    .avm_arbiterlock        (),                          // (terminated)
    .avm_lock               (),                          // (terminated)
    .avm_debugaccess        (),                          // (terminated)
    .avm_transactionid      (),                          // (terminated)
    .avm_readid             (8'b00000000),               // (terminated)
    .avm_writeid            (8'b00000000),               // (terminated)
    .avm_clken              (),                          // (terminated)
    .avm_response           (2'b00),                     // (terminated)
    .avm_writeresponsevalid (1'b0),                      // (terminated)
    .avm_readresponse       (8'b00000000),               // (terminated)
    .avm_writeresponse      (8'b00000000)                // (terminated)
  );

  ////////////////////////////////////////////////////////////////////////////////////////////
  // Control the MM Master BFM
  ////////////////////////////////////////////////////////////////////////////////////////////

  //initialize the Master BFM
  initial
  begin
    `KI_MSTR_BFM.init();
    `MBD_MSTR_BFM.init();
  end  

  function automatic void write_to_slave(input bit [63:0] addr, input bit [63:0] data, input bit [7:0] byteenable);
    `KI_MSTR_BFM.set_command_address(addr);
    `KI_MSTR_BFM.set_command_data(data, 0);
    `KI_MSTR_BFM.set_command_byte_enable(byteenable, 0);
    `KI_MSTR_BFM.set_command_burst_count(1);
    `KI_MSTR_BFM.set_command_burst_size(1);
    `KI_MSTR_BFM.set_command_request(REQ_WRITE);
    `KI_MSTR_BFM.push_command();
  endfunction 

  task read_from_slave(input bit [63:0] addr);
    `KI_MSTR_BFM.set_command_address(addr);
    `KI_MSTR_BFM.set_command_byte_enable({{KI_AV_NUMSYMBOLS}{1'b1}}, 0);  // set to index 0 to make sure we can read something
    `KI_MSTR_BFM.set_command_burst_count(1);
    `KI_MSTR_BFM.set_command_burst_size(1);
    `KI_MSTR_BFM.set_command_request(REQ_READ);
    `KI_MSTR_BFM.push_command();
  endtask

  function automatic void write_to_mbd_slave(input bit [63:0] addr, input bit [lindex(MBD_AV_DATA_W):0] data);
    // always the send a full data to avoid read-modify-write with accurate memory model
    `MBD_MSTR_BFM.set_command_address(addr);
    `MBD_MSTR_BFM.set_command_data(data, 0);
    `MBD_MSTR_BFM.set_command_byte_enable({{MBD_AV_NUMSYMBOLS}{1'b1}}, 0);
    `MBD_MSTR_BFM.set_command_burst_count(1);
    `MBD_MSTR_BFM.set_command_burst_size(1);
    `MBD_MSTR_BFM.set_command_request(REQ_WRITE);
    `MBD_MSTR_BFM.push_command();
  endfunction 

  task read_from_mbd_slave(input bit [63:0] addr);
    `MBD_MSTR_BFM.set_command_address(addr);
    `MBD_MSTR_BFM.set_command_burst_count(1);
    `MBD_MSTR_BFM.set_command_burst_size(1);
    `MBD_MSTR_BFM.set_command_request(REQ_READ);
    `MBD_MSTR_BFM.push_command();
  endtask

  reg [MBD_AV_DATA_W-1:0] dummy_mem;

  // Include file generated from AOC compile
  // File defines parameters related to host-device memory transfer when using -sim-acc-mem flag set at AOC compile
  `include "aoc_sim_mcf_options.svh"

  // There were 3 transfer mechansims consider during the design phase:
  // 1) Transfer all memory through msim DPI. This is long because it's only capable of transfering 4 bytes at a time
  // 2) memcpy then read the maximum burstcount supported. This is the fastest if the transfer is done through memory bank
  //    divider, but it cannot support memory controller bypass mode as mention in the note on top. Also HBM does not 
  //    support burst larger than 1.
  // 3) memcpy then read 1 burscount. This is still fast and can still support memory controller bypass option. That's why 
  //    we only write MBD_AV_NUMSYMBOLS at a time.
  // *Note, writing less than one EMIF address is bad too because the QSys interconnect would convert the command to use
  // Byteenable which would result the EMIF to do RMW operation.

  // Speed up real EMIF simulation by abstracting host memory read transter as verilog task
  // read the data from EMIF and memcpy back to Host, returns first 32-bit to acknowledge completion to avoid race condition
  // base_addr : address request from the host, this is the base address to be copy back to host
  // size      : size of data request from host in bytes
  task automatic read_from_memory_ip_storage(input bit [63:0] base_addr, input bit [31:0] size);
    reg [MBD_AV_NUMSYMBOLS-1:0] mem_byteenable;  // size of memory each it is read in bytes
    reg [MBD_AV_DATA_W-1:0] mem_data;       // EMIF data for a single PHY mem address for burstsize of 1
    reg [ABPHY_ADDR_W-1:0] abphy_addr;  // abstract PHY mem address, not the same as Avalon or DDR
    reg [63:0] next_addr;  // keeps track of next avalon address to write to EMIF, w.r.t. base_addr
    int byte_count_remaining;

    // initialize addr to be read, remaining byte count,
    next_addr = base_addr;
    byte_count_remaining = size;

    if (size > MBD_AV_NUMSYMBOLS) begin
      // the loop only handles copying more than emif width
      while (byte_count_remaining >= 0) begin

        if (MODEL_HOST_MEMORY_READ) begin
          // Read a single DDR address has MBD_AV_NUMSYMBOLS bytes
          read_from_mbd_slave(next_addr);

          // Wait until things are done.
          wait (`MBD_MSTR_BFM.signal_read_response_complete.triggered) @(posedge clock)

          // And grab the data.
          `MBD_MSTR_BFM.pop_response();
          @(posedge clock);
          mem_data = `MBD_MSTR_BFM.get_response_data(0);

          $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] read_from_memory_ip_storage addr 0x%1x result = 0x%1x", $time, next_addr, mem_data);
          __ihc_hls_dbgs(message);
        end
        else begin
          // Note that EMIF bypass only supports burst of 1
          // TODO: determine which bank and the exact avalon address
          // since we have one bank, emif_addr == next_addr

          // backdoor access read from emif
          abphy_addr = get_emif_abphy_addr(next_addr);
          mem_data = `EMIF_BANK0_MEM[abphy_addr];

          $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] read_from_memory_ip_storage addr 0x%1x --> mem_abphy 0x%1x; data = 0x%1x", $time, next_addr, abphy_addr, mem_data);
          __ihc_hls_dbgs(message);
        end

        // Write back to share memory
        __ihc_aoc_mm_slave_write(0, next_addr, mem_data, {{MBD_AV_NUMSYMBOLS}{1'b1}}, MBD_AV_NUMSYMBOLS);

        byte_count_remaining = byte_count_remaining - MBD_AV_NUMSYMBOLS;
        next_addr = next_addr + MBD_AV_NUMSYMBOLS;

        if (byte_count_remaining < MBD_AV_NUMSYMBOLS) begin
          break;
        end
      end
    end

    // send last remaining data that's not fully aligned with emif memory width
    if (byte_count_remaining > 0) begin

      if (MODEL_HOST_MEMORY_READ) begin

          read_from_mbd_slave(next_addr);

          // Wait until things are done.
          wait (`MBD_MSTR_BFM.signal_read_response_complete.triggered) @(posedge clock)

          // And grab the data.
          `MBD_MSTR_BFM.pop_response();
          @(posedge clock);
          mem_data = `MBD_MSTR_BFM.get_response_data(0);

          $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] Memory Read last %0d addr 0x%1x result = 0x%1x", $time, byte_count_remaining, next_addr, mem_data);
          __ihc_hls_dbgs(message);
      end
      else begin
        // Note that EMIF bypass only supports burst of 1
        // TODO: determine which bank
        // backdoor access read from emif
        abphy_addr = get_emif_abphy_addr(next_addr);
        mem_data = `EMIF_BANK0_MEM[abphy_addr];

        $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] read_from_memory_ip_storage last %0d addr 0x%1x --> mem_abphy 0x%1x; data = 0x%1x", $time, byte_count_remaining, next_addr, abphy_addr, mem_data);
        __ihc_hls_dbgs(message);
      end

      // change the byteenable to copy less data
      mem_byteenable = (1 << byte_count_remaining) - 1;
      __ihc_aoc_mm_slave_write(0, next_addr, mem_data, mem_byteenable, MBD_AV_NUMSYMBOLS);
    end

    // Return some acknowledge to the MMD layer that memcpy is done to avoid race condition
    __ihc_aoc_return_kernel_read_data(32'b0000_0001);
    repeat(COMMAND_READ_WRITE_CYCLES) @(posedge clock);
  endtask

  // Memcpy back from Host and write the data to EMIF
  // When MODEL_HOST_MEMORY_WRITE = 0, it speed up real host memory write transter with bypass memory bank divider and
  // memory controller
  // base_addr : address request from the host. This is the base address to be copy from host
  // size      : size of data request from host in bytes
  task automatic write_to_memory_ip_storage(input bit [63:0] base_addr, input bit [31:0] size);
    reg [MBD_AV_DATA_W-1:0] mem_data;   // EMIF data for 1 burst, increase the with burstcount width to support multiple
    reg [ABPHY_ADDR_W-1:0] abphy_addr;  // abstract PHY mem address, not the same as Avalon or DDR
    reg [63:0] next_addr;  // keeps track of next avalon address to write to EMIF, w.r.t. base_addr
    int byte_count_remaining;
    int data_byte_i;

    // initialize
    next_addr = base_addr;
    byte_count_remaining = size;

    if (size > MBD_AV_NUMSYMBOLS) begin
      // the loop only handles copying more than emif width
      while (byte_count_remaining >= 0) begin

        __ihc_aoc_mm_slave_read(0, next_addr, mem_data, MBD_AV_NUMSYMBOLS);

        if (MODEL_HOST_MEMORY_WRITE) begin
          $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] write_to_memory_ip_storage %d bytes addr=0x%1x; data = 0x%1x", $time, byte_count_remaining, next_addr, mem_data);
          __ihc_hls_dbgs(message);

          write_to_mbd_slave(next_addr, mem_data);

          // Wait until the response is complete.
          wait (`MBD_MSTR_BFM.signal_write_response_complete.triggered) @(posedge clock)

          // And remove the response.
          `MBD_MSTR_BFM.pop_response(); 
          repeat(COMMAND_READ_WRITE_CYCLES) @(posedge clock);
        end
        else begin
          // Note that EMIF bypass only supports burst of 1
          // TODO: determine the bank

          // EMIF write with HMC bypass
          abphy_addr = get_emif_abphy_addr(next_addr);
          `EMIF_BANK0_MEM[abphy_addr] = mem_data;

          $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] write_to_memory_ip_storage addr=0x%1x --> mem_abphy=0x%1x; data = 0x%1x", $time, next_addr, abphy_addr, mem_data);
          __ihc_hls_dbgs(message);
        end

        byte_count_remaining = byte_count_remaining - MBD_AV_NUMSYMBOLS;
        next_addr = next_addr + MBD_AV_NUMSYMBOLS;

        // break out when less than 1 EMIF address of data
        if (byte_count_remaining < MBD_AV_NUMSYMBOLS) begin
          break;
        end

      end
    end

    // finish the last data that doesn't fill up one emif memory
    if (byte_count_remaining > 0) begin

      __ihc_aoc_mm_slave_read(0, next_addr, mem_data, byte_count_remaining);
      // Set unused bits as 0 instead of using byteenable to avoid read-modify-write
      for (data_byte_i = byte_count_remaining ; data_byte_i < MBD_AV_NUMSYMBOLS ; data_byte_i = data_byte_i + 1) begin
        mem_data[data_byte_i*8 +: 8] = 8'h00;
      end

      if (MODEL_HOST_MEMORY_WRITE) begin
        $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] write_to_memory_ip_storage last %d bytes addr=0x%1x; data = 0x%1x", $time, byte_count_remaining, next_addr, mem_data);
        __ihc_hls_dbgs(message);

        write_to_mbd_slave(next_addr, mem_data);

        // Wait until the response is complete.
        wait (`MBD_MSTR_BFM.signal_write_response_complete.triggered) @(posedge clock)

        // And remove the response.
        `MBD_MSTR_BFM.pop_response(); 
        repeat(COMMAND_READ_WRITE_CYCLES) @(posedge clock);
      end
      else begin
        // Note that EMIF bypass only supports burst of 1
        // TODO: determine the bank

        abphy_addr = get_emif_abphy_addr(next_addr);
        `EMIF_BANK0_MEM[abphy_addr] = mem_data;

        $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] write_to_memory_ip_storage last %d bytes addr=0x%1x --> mem_abphy 0x%1x; data = 0x%1x", $time, byte_count_remaining, next_addr, abphy_addr, mem_data);
        __ihc_hls_dbgs(message);

        @(posedge clock);
      end
    end

    // send an write acknowledge command back
    // Return 1 back to MMD to say it's done writing to avoid race condition
    __ihc_aoc_return_kernel_read_data(32'h0000_0001);
    repeat(COMMAND_READ_WRITE_CYCLES) @(posedge clock);
  endtask

  // Check to see if there is any kernel read/write work to be done.
  task automatic read_commands;
    string message;
    int address, data, size;
    Command cmd;

    // Start reading commands
    while (1) begin
      // Don't do anything if reset is active or the MASTER BFM is telling us to wait.
      if (~reset_n || `KI_MSTR_BFM.avm_waitrequest || `MBD_MSTR_BFM.avm_waitrequest) begin
        @(posedge clock);
        continue;
      end
      cmd = __ihc_aoc_get_command(address, data, size);
      case (cmd)
        KERNEL_READ:
          begin
            $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] Kernel Read %2d bits from address 0x%1x", $time, KI_AV_DATA_W, address);
            __ihc_hls_dbgs(message);
            read_from_slave(address);

            // Wait until things are done.
            wait (`KI_MSTR_BFM.signal_read_response_complete.triggered) @(posedge clock)

            // And grab the data.
            `KI_MSTR_BFM.pop_response();
            @(posedge clock);
            data = `KI_MSTR_BFM.get_response_data(0);
            $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] Kernel Read result = %d", $time, data);
            __ihc_hls_dbgs(message);

            // Return it to the MMD layer.
            __ihc_aoc_return_kernel_read_data(data);
            repeat(COMMAND_READ_WRITE_CYCLES) @(posedge clock);
          end
        KERNEL_WRITE:
          begin
            int byte_enable = (1 << size) - 1;
            // Send write commands to the BFM on negative clock edges to avoid race conditions
            // due to blocking assignments in the BFM.
            @(negedge clock);
            $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] Kernel Write %2d bits to address 0x%1x (size %1d - enable 0x%1x)", $time, KI_AV_DATA_W, address, size, byte_enable);
            __ihc_hls_dbgs(message);
            write_to_slave(address, data, byte_enable);

            // Wait until the response is complete.
            wait (`KI_MSTR_BFM.signal_write_response_complete.triggered) @(posedge clock)

            // And remove the response.
            `KI_MSTR_BFM.pop_response(); 
            repeat(COMMAND_READ_WRITE_CYCLES) @(posedge clock);
          end
        MEMORY_READ:
          begin
            $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] Memory read address 0x%1x (size %1d)", $time, address, size);
            __ihc_hls_dbgs(message);

            read_from_memory_ip_storage(address, size);
          end
        MEMORY_WRITE:
          begin
            // logic [32:0] bit_enable = (1 << (size<<3)) - 1;
            // @(negedge clock);
            $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] Memory write address 0x%1x (size %1d), data = 0x%1x", $time, address, size, data);
            __ihc_hls_dbgs(message);

            write_to_memory_ip_storage(address, size);
          end
        TERMINATE:
          begin
            // We are finished the simulation.
            $sformat(message, "[%7t][msim][aoc_sim_mm_master_dpi_bfm] Finished simulation", $time);
            __ihc_hls_dbgs(message);
            $finish;
          end
        NO_WORK:
          begin
            // There are no commands to be run.  Don't hold up the simulation.
            repeat(COMMAND_WAIT_CYCLES) @(posedge clock);
          end
      endcase
    end
  endtask

  // Run the read_commands loop forever after initialization
  initial begin

    // Wait some time for things to settle down.
    repeat(COMMAND_INITIAL_WAIT_CYCLES) @(posedge clock);

    fork
      read_commands;
    join_none
  end

endmodule

// vim:set filetype=verilog:
