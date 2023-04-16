// ----------------------------------------------------------------------------
// Avalon-MM DMA PR reordering buffer
// ----------------------------------------------------------------------------
// This IP is used to achieve data reordering between a AVMM slave and AVMM
// master by introducing a buffer. 
// It is mainly used for transferring the PR bitstream via DMA from the host
// workstation's memory to the PR IP. As the Stratix 10 AVMM DMA does not
// guaranteed data to be transferred in order from host to FPGA, we have to
// make sure to reorder the data so that our PR bitstream is written into the
// PR IP in order.
// The PR bitstream's length is a multiple of 4k bytes, therefore a 4k byte
// buffer is used and DMA transfers of 4k in length are set up and they enter
// this IP at the AVMM slave port. These 4k bytes
// are written into the 4k buffer which is implemented in M20Ks.
// After receiving 4k bytes the IP starts writing out the data in order to the
// AVMM master port.
`default_nettype none

module dma_pr_reordering_buffer
#(
  parameter DATA_WIDTH       = 32, // data width of the AVMM interface (both slave and master side)
  parameter ADDRESS_WIDTH    = 10, // address width of the AVMM interface (both slave and master side)
  parameter BURSTCOUNT_WIDTH = 4   // burstcount width of the AVMM interface (both slave and master side)
)
(
  input  wire                        clk,    // clock input
  input  wire                        resetn, // asynchronous reset input, this IP is reset by the PCIe reset to make sure it comes up from idle state before we PR

  // Avalon-MM slave interface (sink for previous stage in data path) 
  output logic                       s_waitrequest,
  input  wire [BURSTCOUNT_WIDTH-1:0] s_burstcount,
  input  wire [DATA_WIDTH-1:0]       s_writedata,
  input  wire [ADDRESS_WIDTH-1:0]    s_address, 
  input  wire                        s_write, 
  input  wire [(DATA_WIDTH/8)-1:0]   s_byteenable, 

  // Avalon-MM master interface (source for next stage in data path)
  input  wire                         m_waitrequest,
  output logic [BURSTCOUNT_WIDTH-1:0] m_burstcount,
  output logic [DATA_WIDTH-1:0]       m_writedata,
  output logic [ADDRESS_WIDTH-1:0]    m_address, 
  output logic                        m_write, 
  output logic [(DATA_WIDTH/8)-1:0]   m_byteenable
);

  localparam FLUSH_WIDTH = 3; // sets the width of the counter used for adding cycles between receive and send states to flush the pipelines

  logic [ADDRESS_WIDTH-1:0] receive_counter;  // counter for the received words stored into the reorder buffer
  logic [ADDRESS_WIDTH-1:0] send_counter;     // counter for the send words from the reorder buffer
  logic [ADDRESS_WIDTH-1:0] send_counter_reg; // counter for the send words from the reorder buffer
  logic [ADDRESS_WIDTH:0]   handled_counter;  // counter for the data handled (sent out of the AVMM master interface)
  logic [FLUSH_WIDTH-1:0]   flush_counter;    // counter for the flush stage between receive and send (32 cycles)

  logic buffer_full;
  logic buffer_full_reg; 

  logic send_valid;

  // AVMM slave side
  logic [BURSTCOUNT_WIDTH-1:0] s_burstcount_constant;  // registering the burstcount at the beginning of a new burst as s_burstcount might change during transaction
  logic [ADDRESS_WIDTH-1:0]    s_address_constant;     // registering the address at the beginning of a new burst used as base address for calculations
  logic [ADDRESS_WIDTH-1:0]    s_address_constant_reg; // one cycle delayed s_address_constant

  // registers AVMM slave signals
  // register stage 1
  logic                        s_write_reg;
  logic [ADDRESS_WIDTH-1:0]    s_address_reg;
  logic [BURSTCOUNT_WIDTH-1:0] s_burstcount_reg;
  logic [(DATA_WIDTH/8)-1:0]   s_byteenable_reg;
  logic [DATA_WIDTH-1:0]       s_writedata_reg;

  // register stage 2
  logic                        s_write_reg2;
  logic [DATA_WIDTH-1:0]       s_writedata_reg2;
  logic [(DATA_WIDTH/8)-1:0]   s_byteenable_reg2;

  // buffer write signals
  logic [ADDRESS_WIDTH-1:0]    writebuffer_address;
  logic [DATA_WIDTH-1:0]       writebuffer_writedata;
  logic [(DATA_WIDTH/8)-1:0]   writebuffer_byteenable;
  logic                        writebuffer_write;
  logic [BURSTCOUNT_WIDTH-1:0] burst_index_counter;
  logic [BURSTCOUNT_WIDTH-1:0] burst_index_counter_reg;

  // buffer read signals
  logic                        readbuffer_read;
  logic [DATA_WIDTH-1:0]       readbuffer_readdata;
  logic [ADDRESS_WIDTH-1:0]    readbuffer_address;

  // register buffer read signals
  // register stage 1
  logic                        readbuffer_read_reg;
  logic [ADDRESS_WIDTH-1:0]    readbuffer_address_reg;
  logic [DATA_WIDTH-1:0]       readbuffer_readdata_reg;

  // register stage 2
  logic                        readbuffer_read_reg2;
  logic [ADDRESS_WIDTH-1:0]    readbuffer_address_reg2;

  // state machine states
  enum {
    STATE_IDLE,
    STATE_RECEIVE,
    STATE_FLUSH,
    STATE_SEND
  } state;

  logic sclrn;
  
  // synchronize resetn input (which is the PCIe reset signal) 
  acl_reset_handler #(
    .ASYNC_RESET           (0),
    .USE_SYNCHRONIZER      (1),
    .SYNCHRONIZE_ACLRN     (0),
    .PIPE_DEPTH            (1),
    .NUM_COPIES            (1)
  ) acl_reset_handler_inst (
    .clk                   (clk),
    .i_resetn              (resetn),
    .o_aclrn               (),
    .o_sclrn               (sclrn),
    .o_resetn_synchronized ()
  );

  // instantiate buffer
  // This is the 4k bytes buffer used to reorder the data written to it from
  // the AVMM slave port
  acl_altera_syncram_wrapped #(
    .rdcontrol_reg_b                    ("CLOCK0"),
    .address_reg_b                      ("CLOCK0"),
    .indata_reg_b                       ("CLOCK0"),
    .byteena_reg_b                      ("CLOCK0"),
    .clock_enable_input_a               ("BYPASS"),
    .clock_enable_input_b               ("BYPASS"),
    .clock_enable_output_b              ("BYPASS"),
    .lpm_type                           ("altsyncram"),
    .numwords_a                         (2**ADDRESS_WIDTH),
    .numwords_b                         (2**ADDRESS_WIDTH),
    .operation_mode                     ("DUAL_PORT"),
    .outdata_aclr_b                     ("NONE"),
    .outdata_reg_b                      ("UNREGISTERED"),
    .power_up_uninitialized             ("FALSE"),
    .read_during_write_mode_mixed_ports ("DONT_CARE"),
    .read_during_write_mode_port_a      ("DONT_CARE"),
    .read_during_write_mode_port_b      ("DONT_CARE"),
    .widthad_a                          (ADDRESS_WIDTH),
    .widthad_b                          (ADDRESS_WIDTH),
    .width_a                            (DATA_WIDTH),
    .width_b                            (DATA_WIDTH),
    .width_byteena_a                    (DATA_WIDTH/8),
    .connect_clr_to_ram                 (0),
    .enable_ecc                         ("FALSE")
  )	 
  altsyncram_component (
    .clock0         (clk),
    .wren_a         (writebuffer_write),
    .address_a      (writebuffer_address),
    .data_a         (writebuffer_writedata),
    .wren_b         (1'b0),
    .address_b      (readbuffer_address),
    .data_b         (),
    .q_a            (),
    .q_b            (readbuffer_readdata),
    .aclr0          (),
    .aclr1          (),
    .sclr           (!sclrn),
    .addressstall_a (1'b0),
    .addressstall_b (1'b0),
    .byteena_a      (writebuffer_byteenable),
    .byteena_b      (),
    .clock1         (clk),
    .clocken0       (1'b1),
    .clocken1       (1'b1),
    .rden_a         (1'b0),
    .rden_b         (readbuffer_read),
    .ecc_err_status ()
  );
  
  // register buffer data 
  always @(posedge clk)
  begin
    // register stage 1 
    s_write_reg             <= s_write;
    s_address_reg           <= s_address;
    s_burstcount_reg        <= s_burstcount;
    s_writedata_reg         <= s_writedata;
    s_byteenable_reg        <= s_byteenable;

    s_address_constant_reg  <= s_address_constant;
    burst_index_counter_reg <= burst_index_counter;
    buffer_full_reg         <= buffer_full;

    // register stage 2 
    s_write_reg2            <= s_write_reg;
    s_writedata_reg2        <= s_writedata_reg;
    s_byteenable_reg2       <= s_byteenable_reg;

    // stall reading from the buffer when AVMM master is stalled by
    // m_waitrequest
    if(!m_waitrequest) 
    begin
      // register stage 1
      readbuffer_read_reg     <= readbuffer_read;
      readbuffer_address_reg  <= readbuffer_address;
      readbuffer_readdata_reg <= readbuffer_readdata;
      send_counter_reg        <= send_counter;

      // register stage 2
      readbuffer_read_reg2    <= readbuffer_read_reg;
      readbuffer_address_reg2 <= readbuffer_address_reg;
    end

    if(!sclrn)
    begin
      // register stage 1
      s_write_reg             <= '0;
      s_address_reg           <= '0;
      s_burstcount_reg        <= '0;
      s_writedata_reg         <= '0;
      s_byteenable_reg        <= '0;

      readbuffer_read_reg     <= '0;
      readbuffer_address_reg  <= '0;
      readbuffer_readdata_reg <= '0;

      s_address_constant_reg  <= '0;
      burst_index_counter_reg <= '0;
      send_counter_reg        <= '0;

      // register stage 2
      s_write_reg2            <= '0;
      s_writedata_reg2        <= '0;
      s_byteenable_reg2       <= '0;

      readbuffer_read_reg2    <= '0;
      readbuffer_address_reg2 <= '0;
    end
  end

  // implement state machine
  // the state machine cycles through states IDLE, RECEIVE, FLUSH and SEND
  // and going back to the IDLE state
  // Description of the states:
  // IDLE: IP waiting for first write to AVMM slave port
  // RECEIVE: data is being received out-of-order on the AVMM slave port
  // FLUSH: intermediate state to flush all data pipelines
  // SEND: data is being sent in-order out of the AVMM master port
  always @(posedge clk)
  begin	    
    case (state)

      // The DMA PR reordering buffer enters the 'idle' state when it is
      // reset by the resetn signal which is connected to the PCIe reset
      // signal
      STATE_IDLE:
      begin
        // We can only leave the 'idle' state when we receive the first
        // write transaction on the AVMM slave port indicated by s_write
        // going high. At this time the burstcount and address is also
        // registered so that we can ignore any changes on these signals
        // during the burst
	
	// stop backpressuring upstream
	s_waitrequest <= 1'b0;

	// reset internal counters
	receive_counter     <= '0;
	send_counter        <= '0;
	flush_counter       <= '0;
	handled_counter     <= '0;
        burst_index_counter <= '0;

        // Leave the 'idle' state at the first write transaction
        if(s_write)
        begin
          // capture start address and burstcount values
          s_burstcount_constant <= s_burstcount;
          s_address_constant <= s_address;

          state <= STATE_RECEIVE;
        end
      end

      // The 'receive' state allows the DMA controller to transfer the PR
      // bitstream into the IP's buffer memory until it is full
      // Then the DMA is being backpressured by the s_waitrequest signal
      STATE_RECEIVE:
      begin
        // capture the new burstcount and base address at the beginning of each
        // burst
        if(s_write)
        begin
          // begin new burst (old burst is complete)
          if(burst_index_counter == s_burstcount_constant-1)
          begin
            s_burstcount_constant <= s_burstcount;
            s_address_constant    <= s_address;
            burst_index_counter   <= '0;
          end
          else
          begin
            // increment the burst index counter to keep track of the
            // transaction within a burst
            burst_index_counter <= burst_index_counter + {{BURSTCOUNT_WIDTH-1{1'b0}},1'b1};
          end
	  
          // keep track of received transactions from upstream
          receive_counter <= receive_counter + {{ADDRESS_WIDTH-1{1'b0}},1'b1};
        end

        // start backpressuring the DMA (in case the buffer is full or we are pushing out
        // data to the PR IP downstream)
        if(receive_counter[ADDRESS_WIDTH-1:0] == {ADDRESS_WIDTH{1'b1}}-1)
        begin
          s_waitrequest <= 1'b1;
        end

        // go to next stage when whole frame of bitstream is received
        if(receive_counter[ADDRESS_WIDTH-1:0] == {ADDRESS_WIDTH{1'b1}})
        begin
          buffer_full <= 1'b1;
          state       <= STATE_FLUSH;
        end
      end

      // The 'flush' state makes sure that all pipelines are flushed
      // before beginning to send the bitstream to the PR IP.
      STATE_FLUSH:
      begin
        // count cycles that we spend in this state to flush all pipelines
        flush_counter <= flush_counter + {{FLUSH_WIDTH-1{1'b0}},1'b1};

        // go to next stage
        if(flush_counter[FLUSH_WIDTH-1:0] == {FLUSH_WIDTH{1'b1}})
        begin	 
          state           <= STATE_SEND;
          receive_counter <= '0;
          send_counter    <= '0;
          flush_counter   <= '0;
          handled_counter <= '0;
        end

      end

      // The 'send' state sends the reordered chunk of the PR bitstream to
      // the PR IP. It will be backpressured by it once the whole bitstream
      // has been transferred.  
      STATE_SEND:
      begin
        send_valid <= 1'b1;

        // go to next stage when the contents of the buffer were completely
        // sent out downstream, release the DMA backpressure
        if(handled_counter[ADDRESS_WIDTH:0] == {1'b1,{ADDRESS_WIDTH{1'b0}}})
        begin	 
          state         <= STATE_IDLE;
          s_waitrequest <= 1'b0;
          buffer_full   <= 1'b0;
          send_valid    <= 1'b0;

          // reset counters and registers
          receive_counter       <= '0;
          send_counter          <= '0;
          burst_index_counter   <= '0;
          s_burstcount_constant <= '0;
          s_address_constant    <= '0;
        end

        // handle waitrequest from downstream
        // only update the counter of handled transactions when they are
        // accepted downstream
        if(!m_waitrequest)
        begin
          // set send counter
          send_counter <= send_counter + {{ADDRESS_WIDTH-1{1'b0}},1'b1};
          if(m_write)
          begin
            handled_counter <= handled_counter + {{ADDRESS_WIDTH{1'b0}},1'b1};
          end
        end 

      end
    endcase
    
    // synchronously reset signals    
    if (!sclrn)
    begin
      state                 <= STATE_IDLE;

      // reset counters and registers
      s_waitrequest         <= 1'b0;

      s_burstcount_constant <= '0;
      s_address_constant    <= '0;

      receive_counter       <= '0;
      send_counter          <= '0;
      flush_counter         <= '0;
      handled_counter       <= '0;
      burst_index_counter   <= '0;

      buffer_full           <= 1'b0;
      send_valid            <= 1'b0;
    end
  end

  // push data from DMA into internal buffer from AVMM slave port
  // there is a 2 clock cycle delay between the data arriving at the AVMM
  // slave port and being written to the buffer (s_address_constant is 2 clock
  // cycles delayed compared to s_address, as is the burst_index_counter_reg)
  assign writebuffer_write      = s_write_reg2 & !buffer_full_reg;
  assign writebuffer_address    = s_address_constant_reg + burst_index_counter_reg;
  assign writebuffer_writedata  = s_writedata_reg2;
  assign writebuffer_byteenable = s_byteenable_reg2;

  // provide the buffer read side with read and address
  assign readbuffer_read    = (state==STATE_SEND) ? buffer_full & !m_waitrequest & send_valid : 1'b0;
  assign readbuffer_address = send_counter_reg;

  // push internal buffer data out of AVMM master port to the PR IP
  assign m_write      = (state==STATE_SEND) ? readbuffer_read_reg2 & !m_waitrequest & (handled_counter[ADDRESS_WIDTH:0] != {1'b1,{ADDRESS_WIDTH{1'b0}}}) : 1'b0;
  assign m_address    = (state==STATE_SEND) ? readbuffer_address_reg2 : '0;
  assign m_writedata  = (state==STATE_SEND) ? readbuffer_readdata_reg : '0;
  assign m_byteenable = '1;
  assign m_burstcount = {{BURSTCOUNT_WIDTH-1{1'b0}},1'b1};

endmodule
`default_nettype wire
