// --------------------------------------
// Synchronous Avalon-MM pipeline bridge
// Registers Avalon-MM command and response signals
//
// This block is originally taken from the Qsys Avalon-MM pipeline bridge and
// essentially shortened by removing optional features and limiting it
// to the feature set that is only used in our OpenCL board interface IP.
//
// It was modified in the following ways to meet S10 performance targets:
// - instantiation of acl_reset_handler for synchronizing the incoming ACLR
// - removal of optional command and response signal pipelining and making
//   this the default case (we only instantiate this IP for the added pipelining
//   purpose)
// - removal of the optional AVMM response signal (this feature was never used
//   in any of our board interface Qsys IPs)
// - change all ACLR reset signals to SCLR resets
// - change to not reset the data path, but control signals only to reduce reset fanout
//
// This block is instantiated automatically by changes to system integrator for the
// pipeline bridges in kernel_system.qsys but also can manually be used in board.qsys
// for additional pipeline stages (applied to the data path in the kernel clock domain
// to increase fmax between the kernel global memory output and the clock crosser into
// the DDR clock domain).
// --------------------------------------
`default_nettype none
module acl_avalon_mm_bridge_s10
#(
  parameter DATA_WIDTH        = 32, // bit width of the AVMM data interface
  parameter SYMBOL_WIDTH      = 8,  // bits per symbol to determine byte enable width
  parameter HDL_ADDR_WIDTH    = 10, // address width of the AVMM interface
  parameter BURSTCOUNT_WIDTH  = 1,  // burstcount width of the AVMM interface
  parameter SYNCHRONIZE_RESET = 1,  // force the incoming reset to be synchronized by the reset handler
  /*
    Normally this bridge will buffer incoming requests when waitrequested. This parameter disables this behaviour and instead
    implements a pure feedforward (unstallable) pipeline stage on all AVMM signals, including waitrequest.
    This parameter should be set to 1 when this pipeline bridge is used in an AVMM path with waitrequest-allowance enabled.
    Strictly speaking this bridge does *not* exactly support waitrequest-allowance since it does not have any internal
    buffer when used in this mode. Instead this bridge can be turned into a pure unstallable pipeline stage that is placed *between* a master and slave
    that each support waitrequest-allowance themselves.
    The _read and _write signals are still reset to 0.
  */
  parameter DISABLE_WAITREQUEST_BUFFERING = 0,
  parameter READDATA_PIPE_DEPTH = 1,  // Specifies how many stages of pipelining are placed on the readdata and readdatavalid. Min value 1.
  parameter CMD_PIPE_DEPTH = 1, // Specifies how many feed-forward (unstallable) pipe stages are placed on the command path. Min value 1.

  // --------------------------------------
  // Derived parameters
  // --------------------------------------
  parameter BYTEEN_WIDTH = DATA_WIDTH / SYMBOL_WIDTH
)
(
  input   wire                          clk,      // clock input
  input   wire                          reset,      // reset input

  // Avalon-MM slave interface (sink for previous stage in data path) 
  output  logic                         s0_waitrequest,
  output  logic [DATA_WIDTH-1:0]        s0_readdata,
  output  logic                         s0_readdatavalid,
  input   wire [BURSTCOUNT_WIDTH-1:0]   s0_burstcount,
  input   wire [DATA_WIDTH-1:0]         s0_writedata,
  input   wire [HDL_ADDR_WIDTH-1:0]     s0_address, 
  input   wire                          s0_write, 
  input   wire                          s0_read, 
  input   wire [BYTEEN_WIDTH-1:0]       s0_byteenable, 
  input   wire                          s0_debugaccess,

  // Avalon-MM master interface (source for next stage in data path)
  input   wire                          m0_waitrequest,
  input   wire [DATA_WIDTH-1:0]         m0_readdata,
  input   wire                          m0_readdatavalid,
  output  logic [BURSTCOUNT_WIDTH-1:0]  m0_burstcount,
  output  logic [DATA_WIDTH-1:0]        m0_writedata,
  output  logic [HDL_ADDR_WIDTH-1:0]    m0_address, 
  output  logic                         m0_write, 
  output  logic                         m0_read, 
  output  logic [BYTEEN_WIDTH-1:0]      m0_byteenable,
  output  logic                         m0_debugaccess
);

  // --------------------------------------
  // Registers & signals
  // --------------------------------------
  reg   [CMD_PIPE_DEPTH:1][BURSTCOUNT_WIDTH-1:0]    cmd_burstcount;
  reg   [CMD_PIPE_DEPTH:1][DATA_WIDTH-1:0]          cmd_writedata;
  reg   [CMD_PIPE_DEPTH:1][HDL_ADDR_WIDTH-1:0]      cmd_address; 
  reg   [CMD_PIPE_DEPTH:1]                          cmd_write;  
  reg   [CMD_PIPE_DEPTH:1]                          cmd_read;  
  reg   [CMD_PIPE_DEPTH:1][BYTEEN_WIDTH-1:0]        cmd_byteenable;
  wire  [CMD_PIPE_DEPTH:1]                          cmd_waitrequest;
  reg   [CMD_PIPE_DEPTH:1]                          cmd_debugaccess;

  reg [BURSTCOUNT_WIDTH-1:0]    wr_burstcount;
  reg [DATA_WIDTH-1:0]          wr_writedata;
  reg [HDL_ADDR_WIDTH-1:0]      wr_address; 
  reg                           wr_write;  
  reg                           wr_read;  
  reg [BYTEEN_WIDTH-1:0]        wr_byteenable;
  reg                           wr_debugaccess;

  reg [BURSTCOUNT_WIDTH-1:0]    wr_reg_burstcount;
  reg [DATA_WIDTH-1:0]          wr_reg_writedata;
  reg [HDL_ADDR_WIDTH-1:0]      wr_reg_address; 
  reg                           wr_reg_write;  
  reg                           wr_reg_read;  
  reg [BYTEEN_WIDTH-1:0]        wr_reg_byteenable;
  reg                           wr_reg_waitrequest;
  reg                           wr_reg_debugaccess;

  reg                           use_reg;
  wire                          wait_rise;

  reg [READDATA_PIPE_DEPTH:1][DATA_WIDTH-1:0] rsp_readdata;
  reg [READDATA_PIPE_DEPTH:1]                 rsp_readdatavalid;

  wire        sclrn;

  acl_reset_handler #(
    .ASYNC_RESET              (0),
    .USE_SYNCHRONIZER         (SYNCHRONIZE_RESET),
    .SYNCHRONIZE_ACLRN        (SYNCHRONIZE_RESET),
    .PIPE_DEPTH               (3),
    .NUM_COPIES               (1)
  ) acl_reset_handler_inst (
    .clk                      (clk),
    .i_resetn                 (!reset),
    .o_aclrn                  (),
    .o_resetn_synchronized    (),
    .o_sclrn                  (sclrn)
  );

  // --------------------------------------
  // Command pipeline
  //
  // Registers all command signals, including waitrequest
  // --------------------------------------

  // --------------------------------------
  // Waitrequest Pipeline Stage
  //
  // Output waitrequest is delayed by one cycle, which means
  // that a master will see waitrequest assertions one cycle 
  // too late.
  //
  // Solution: buffer the command when waitrequest transitions
  // from low->high. As an optimization, we can safely assume 
  // waitrequest is low by default because downstream logic
  // in the bridge ensures this.
  //
  // Note: this implementation buffers idle cycles should 
  // waitrequest transition on such cycles. This is a potential
  // cause for throughput loss, but ye olde pipeline bridge did
  // the same for years and no one complained. Not buffering idle
  // cycles costs logic on the waitrequest path.
  // --------------------------------------
  assign s0_waitrequest = wr_reg_waitrequest;
  assign wait_rise      = ~wr_reg_waitrequest & cmd_waitrequest;
     
  always @(posedge clk) begin
    wr_reg_waitrequest <= DISABLE_WAITREQUEST_BUFFERING? m0_waitrequest: cmd_waitrequest;

    if (wait_rise) begin
      wr_reg_writedata  <= s0_writedata;
      wr_reg_byteenable <= s0_byteenable;
      wr_reg_address    <= s0_address;
      wr_reg_write      <= s0_write;
      wr_reg_read       <= s0_read;
      wr_reg_burstcount <= s0_burstcount;
      wr_reg_debugaccess <= s0_debugaccess;
    end

    // stop using the buffer when waitrequest is low
    if (~cmd_waitrequest) begin 
      use_reg <= 1'b0;
    end
    else if (wait_rise) begin 
      use_reg <= 1'b1;
    end     

    if (!sclrn) begin
      wr_reg_waitrequest <= 1'b1;
      // --------------------------------------
      // Bit of trickiness here, deserving of a long comment.
      //
      // On the first cycle after reset, the pass-through
      // must not be used or downstream logic may sample
      // the same command twice because of the delay in
      // transmitting a falling waitrequest.
      //
      // Using the registered command works on the condition
      // that downstream logic deasserts waitrequest
      // immediately after reset, which is true of the 
      // next stage in this bridge.
      // --------------------------------------
      use_reg            <= 1'b1;
          
      wr_reg_write      <= 1'b0;
      wr_reg_read       <= 1'b0;
      wr_reg_debugaccess <= 1'b0;
    end

  end
     
  always @* begin
    wr_burstcount  =  s0_burstcount;
    wr_writedata   =  s0_writedata;
    wr_address     =  s0_address;
    wr_write       =  s0_write;
    wr_read        =  s0_read;
    wr_byteenable  =  s0_byteenable;
    wr_debugaccess =  s0_debugaccess;
    
    if (use_reg) begin
      wr_burstcount  =  wr_reg_burstcount;
      wr_writedata   =  wr_reg_writedata;
      wr_address     =  wr_reg_address;
      wr_write       =  wr_reg_write;
      wr_read        =  wr_reg_read;
      wr_byteenable  =  wr_reg_byteenable;
      wr_debugaccess =  wr_reg_debugaccess;
    end
  end
     
  // --------------------------------------
  // Master-Slave Signal Pipeline Stage 
  //
  // One notable detail is that cmd_waitrequest is deasserted
  // when this stage is idle. This allows us to make logic
  // optimizations in the waitrequest pipeline stage.
  // 
  // Also note that cmd_waitrequest is deasserted during reset,
  // which is not spec-compliant, but is ok for an internal
  // signal.
  // --------------------------------------
  wire no_command;
  assign no_command      = ~(cmd_read || cmd_write);
  assign cmd_waitrequest = m0_waitrequest & ~no_command;
     
  always @(posedge clk) begin
    
    if (DISABLE_WAITREQUEST_BUFFERING) begin // Pipeline the input once and feed it directly to the output.
      cmd_writedata  [1] <= s0_writedata;
      cmd_byteenable [1] <= s0_byteenable;
      cmd_address    [1] <= s0_address;
      cmd_write      [1] <= s0_write;
      cmd_read       [1] <= s0_read;
      cmd_burstcount [1] <= s0_burstcount;
      cmd_debugaccess[1] <= s0_debugaccess;
    end else if (~cmd_waitrequest) begin
      cmd_writedata  [1] <= wr_writedata;
      cmd_byteenable [1] <= wr_byteenable;
      cmd_address    [1] <= wr_address;
      cmd_write      [1] <= wr_write;
      cmd_read       [1] <= wr_read;
      cmd_burstcount [1] <= wr_burstcount;
      cmd_debugaccess[1]  <= wr_debugaccess;
    end

    for (int i=2; i<= CMD_PIPE_DEPTH; i++) begin
      cmd_writedata  [i] <= cmd_writedata  [i-1];  
      cmd_byteenable [i] <= cmd_byteenable [i-1];
      cmd_address    [i] <= cmd_address    [i-1];
      cmd_write      [i] <= cmd_write      [i-1];
      cmd_read       [i] <= cmd_read       [i-1];
      cmd_burstcount [i] <= cmd_burstcount [i-1];
      cmd_debugaccess[i] <= cmd_debugaccess[i-1];
    end    

    if (!sclrn) begin
      cmd_write      <= 1'b0;
      cmd_read       <= 1'b0;
      cmd_debugaccess <= 1'b0;
    end
  end

  assign m0_burstcount    = cmd_burstcount[CMD_PIPE_DEPTH];
  assign m0_writedata     = cmd_writedata[CMD_PIPE_DEPTH];
  assign m0_address       = cmd_address[CMD_PIPE_DEPTH];
  assign m0_write         = cmd_write[CMD_PIPE_DEPTH];
  assign m0_read          = cmd_read[CMD_PIPE_DEPTH];
  assign m0_byteenable    = cmd_byteenable[CMD_PIPE_DEPTH];
  assign m0_debugaccess   = cmd_debugaccess[CMD_PIPE_DEPTH];

  // --------------------------------------
  // Response pipeline
  //
  // Registers all response signals
  // --------------------------------------

  always @(posedge clk) begin
    rsp_readdatavalid[1] <= m0_readdatavalid;
    rsp_readdata[1]      <= m0_readdata;
    for (int i=2; i<= READDATA_PIPE_DEPTH; i++) begin
      rsp_readdatavalid[i] <= rsp_readdatavalid[i-1];
      rsp_readdata[i]      <= rsp_readdata[i-1];
    end

    if (!sclrn) begin
      rsp_readdatavalid <= '0;
    end
  end

  assign s0_readdatavalid = rsp_readdatavalid[READDATA_PIPE_DEPTH];
  assign s0_readdata      = rsp_readdata[READDATA_PIPE_DEPTH];

endmodule
`default_nettype wire
