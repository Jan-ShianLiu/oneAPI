//__DESIGN_FILE_COPYRIGHT__

//  AvalonMM Clock Crossing Bridge
//
//  Intended to be inserted between an AvalonMM master and an AvalonMM slave who operate on different clock domains.
//  This module is basically two clock crossing fifos (one in each direction) with some throttle logic to ensure that one does not request more read data than
//  one has space for, throttling is necessary if the read data goes from a faster clock to a slower clock since the response fifo cannot be drained fast enough.
//
//  Internally this module uses the new acl_dcfifo, which is a high fmax replacement to Altera's dcfifo. Reset has been very carefully thought out.
//  There is only 1 reset input which this module treats as asynchronous, internally it is synchronized to both clocks.
//  If one has a resetn from each clock domain then AND them together to drive the input port of this module.
//
//  AvalonMM waitrequest allowance is implemented. Basically this allows one to backpressure upstream with a pipelined almost full signal.
//
//  Write ack is supported, however it is intended only for use with AXI on HBM. Write ack is not part of the AvalonMM interface, and there is no ordering between
//  readdatavalid and write ack. Write ack is implemented with a zero width acl_dcfifo, which is completely independent from the acl_dcfifo used for read data.
//
//  Required files:
//  - acl_clock_crossing_bridge.sv
//  - acl_dcfifo.sv
//  - acl_reset_handler.sv
//  - acl_parameter_assert.svh
//  - acl_width_clip.svh

`default_nettype none
`include "acl_parameter_assert.svh"

module acl_clock_crossing_bridge #(
    //signal widths -- all must be at least 1
    parameter int unsigned ADDRESS_WIDTH,               //byte address must be word aligned, e.g. if BYTEENABLE_WIDTH = 4, then address must be 4-byte aligned, bottom 2 bits must be 0
    parameter int unsigned BURSTCOUNT_WIDTH,
    parameter int unsigned BYTEENABLE_WIDTH,            //must be a power of 2, specifies word size
    
    //clock crossing fifo depths -- depths will be increased internally if the ccb requires a more capacity than the minimum requested by the user, internal depths will always be at least 8
    parameter int unsigned CMD_DCFIFO_MIN_DEPTH = 0,    //minimum capacity requested by user for the dcfifo for sending commands from slave to master
    parameter int unsigned RSP_DCFIFO_MIN_DEPTH = 0,    //minimum capacity requested by user for the dcfifo for receiving responses from master to slave
    
    //clock crossing fifo ram block type, if "MLAB" or "M20K" you will get what you ask for, otherwise the fifo will decide
    parameter string       CMD_DCFIFO_RAM_TYPE = "FIFO_TO_CHOOSE",
    parameter string       RSP_DCFIFO_RAM_TYPE = "FIFO_TO_CHOOSE",
    
    //waitrequest allowance settings
    parameter int unsigned SLAVE_STALL_LATENCY = 0,     //if >= 1 then outgoing s_waitrequest is an almost full to upstream, incoming s_read and s_write cannot be backpressured
    parameter int unsigned MASTER_STALL_LATENCY = 0,    //if >= 1 then incoming m_waitrequest is an almost full from downstream, outgoing m_read and m_write are forced transactions
    
    //write acknowledge -- there is no ordering between readdatavalid and writeack, intended only for use with AXI on HBM, this is not avalon conformant
    parameter bit          USE_WRITE_ACK = 0,
    
    //response backpressure -- normally AvalonMM responses cannot be backpressured, but width-stitching needs this, just expose more signals on the read side of rsp_dcfifo
    parameter bit          RESPONSE_BACKPRESSURE = 0,   //if 1 then s_readack_stall can backpressure to rsp_dcfifo, and also s_writeack_stall can backpressure to writeack_rsp_dcfifo when USE_WRITE_ACK = 1
    parameter int unsigned RESPONSE_ALMOST_EMPTY = 0,   //sets the ALMOST_EMPTY_CUTOFF parameter for rsp_dcfifo (and writeack_rsp_dcfifo when USE_WRITE_ACK = 1)
    
    //derived parameters
    localparam int unsigned DATA_WIDTH = 8*BYTEENABLE_WIDTH,
    localparam int unsigned ADDRESS_BITS_PER_WORD = $clog2(BYTEENABLE_WIDTH),               //how many lower bits of the byte address are stuck at 0 to ensure it is word aligned
    localparam int unsigned WORD_ADDRESS_WIDTH = ADDRESS_WIDTH - ADDRESS_BITS_PER_WORD,     //convert byte address width to word address width
    localparam int unsigned MAX_BURST_SIZE = 2**(BURSTCOUNT_WIDTH-1),                       //per avalon spec
    
    //derived parameters -- determine size of cmd dcfifo
    localparam int unsigned CMD_DCFIFO_DEPTH_MIN_REQUIRED = 8,  //no advantage for a shallower fifo, internally acl_dcfifo is the same for depth up to 8, just the full counter is offset to backpressure earlier
    localparam int unsigned CMD_DCFIFO_DEPTH = (CMD_DCFIFO_MIN_DEPTH < CMD_DCFIFO_DEPTH_MIN_REQUIRED) ? CMD_DCFIFO_DEPTH_MIN_REQUIRED : CMD_DCFIFO_MIN_DEPTH,
    
    //derived parameters -- determine size of rsp dcfifo
    //throttle logic ensures there is space in the response fifo before sending a read request
    //there is some pipelining in the throttle logic so the counter is stale, compensate for this staleness by having some extra room in the response fifo
    localparam int unsigned RSP_DCFIFO_DEPTH_MIN_REQUIRED = 8*MAX_BURST_SIZE,   //will be at least 8 for same reason as cmd_dcfifo, and also since MAX_BURST_SIZE >= 1
    localparam int unsigned RSP_DCFIFO_DEPTH = (RSP_DCFIFO_MIN_DEPTH < RSP_DCFIFO_DEPTH_MIN_REQUIRED) ? RSP_DCFIFO_DEPTH_MIN_REQUIRED : RSP_DCFIFO_MIN_DEPTH,
    localparam int unsigned RSP_COUNT_WIDTH = $clog2(RSP_DCFIFO_DEPTH) + 1      //add 1 to width of counter for throttle logic so that we can use the MSB as the check for threshold reached
) (
    input  wire                         async_resetn,   //asynchronous reset, internally we synchronize it to both clocks, if you have a resetn from each clock domain then AND them together     
    
    //AvalonMM slave interface -- connects to an upstream master
    input  wire                         s_clock,
    input  wire                         s_read,
    input  wire                         s_write,
    input  wire     [ADDRESS_WIDTH-1:0] s_address,
    input  wire  [BURSTCOUNT_WIDTH-1:0] s_burstcount,
    input  wire  [BYTEENABLE_WIDTH-1:0] s_byteenable,
    input  wire        [DATA_WIDTH-1:0] s_writedata,
    output logic                        s_waitrequest,
    output logic                        s_readdatavalid,
    output logic       [DATA_WIDTH-1:0] s_readdata,
    output logic                        s_writeack,     //not part of AvalonMM interface, see comment about USE_WRITE_ACK
    
    //expose more signals from read side of response dcfifo to enable backpressure for width-stitching (wide ccb)
    output logic                        s_readack_empty,
    output logic                        s_readack_almost_empty,
    input  wire                         s_readack_stall,
    output logic                        s_writeack_empty,
    output logic                        s_writeack_almost_empty,
    input  wire                         s_writeack_stall,
    
    //AvalonMM master interface -- connects to a downstream slave
    input  wire                         m_clock,
    output logic                        m_read,
    output logic                        m_write,
    output logic    [ADDRESS_WIDTH-1:0] m_address,
    output logic [BURSTCOUNT_WIDTH-1:0] m_burstcount,
    output logic [BYTEENABLE_WIDTH-1:0] m_byteenable,
    output logic       [DATA_WIDTH-1:0] m_writedata,
    input  wire                         m_waitrequest,
    input  wire                         m_readdatavalid,
    input  wire        [DATA_WIDTH-1:0] m_readdata,
    input  wire                         m_writeack,     //not part of AvalonMM interface, see comment about USE_WRITE_ACK
    output logic                        m_empty         //to indicate cmd_dcfifo has data to send, m_read and m_write no longer convey this info when MASTER_STALL_LATENCY = 1 (forced transactions)
);
    
    //////////////////////////////////////
    //                                  //
    //  Sanity check on the parameters  //
    //                                  //
    //////////////////////////////////////
    
    generate
    `ACL_PARAMETER_ASSERT(ADDRESS_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BURSTCOUNT_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(BYTEENABLE_WIDTH == 2**ADDRESS_BITS_PER_WORD)
    `ACL_PARAMETER_ASSERT(SLAVE_STALL_LATENCY < CMD_DCFIFO_DEPTH)
    `ACL_PARAMETER_ASSERT(RESPONSE_ALMOST_EMPTY < RSP_DCFIFO_DEPTH)
    endgenerate
    
    
    
    ///////////////////////////
    //                       //
    //  Signal declarations  //
    //                       //
    ///////////////////////////
    
    // naming convention: all signal names begin with m_ or s_ and that indicates which clock domain it is on (master or slave, respectively)
    
    logic [WORD_ADDRESS_WIDTH-1:0] m_word_address, s_word_address;  //avalon spec requires byte address to be word aligned so the bottom ADDRESS_BITS_PER_WORD bits are 0, don't store in cmd_dcfifo
    
    logic m_empty_raw, m_is_write;      //read side of cmd_dcfifo
    logic m_write_raw, m_read_raw;      //without waitrequest allowance, this is what the output signals m_write and m_read would be
    logic s_rsp_empty;                  //read side of rsp_dcfifo
    logic m_sclrn;                      //reset for throttle logic
    logic m_throttle;                   //mask read and write requests from cmd_dcfifo because there is not enough space for the return data or write ack
    
    logic [BURSTCOUNT_WIDTH-1:0] m_pending_reads_increase;  //throttle logic - increase counter by burst count when read request sent
    logic [2:0] m_read_update, m_pending_reads_decrease;    //throttle logic - decrease counter by 1 when rsp_dcfifo is read on slave clock -- communicated to master clock as an update inside acl_dcfifo
    logic [RSP_COUNT_WIDTH-1:0] m_counter_update;           //combine the above increase and decrease into an update
    logic [RSP_COUNT_WIDTH-1:0] m_counter;                  //the throttle counter itself
    
    logic m_writeack_pending_reads_increase;                                //throttle logic - increase counter for writeack
    logic [2:0] m_writeack_read_update, m_writeack_pending_reads_decrease;  //throttle logic - decrease counter for writeack
    logic [RSP_COUNT_WIDTH-1:0] m_writeack_counter_update;                  //combine the above into update for writeack
    logic [RSP_COUNT_WIDTH-1:0] m_writeack_counter;                         //throttle counter itself for writeack
    logic s_writeack_rsp_empty;                                             //read side of writeack_dcfifo
    
    
    
    /////////////
    //         //
    //  Reset  //
    //         //
    /////////////
    
    // Each acl_dcfifo has its own reset management which is very complicated, the reset here is for the throttle logic.
    
    acl_reset_handler #(
        .ASYNC_RESET            (0),
        .USE_SYNCHRONIZER       (1),
        .PIPE_DEPTH             (2)
    )
    m_sclrn_inst
    (
        .clk                    (m_clock),
        .i_resetn               (async_resetn),
        .o_aclrn                (),
        .o_sclrn                (m_sclrn),
        .o_resetn_synchronized  ()
    );
    
    
    
    ///////////////////////////////////////////////////
    //                                               //
    //  Clock crossing fifo for read/write requests  //
    //                                               //
    ///////////////////////////////////////////////////
    
    // Avalon spec says the byte addresses must be word aligned. Normally the upstream interface should drive the lower ADDRESS_BITS_PER_WORD bits of the address to 0,
    // and the downstream interface should not consume the lower ADDRESS_BITS_PER_WORD bits of the address, then Quartus would prune away those bits from the cmd_dcfifo.
    // In practice, either the conditions are not being met or the Quartus pruning checks are too conservative, so we manually remove the bottom ADDRESS_BITS_PER_WORD bits.
    
    assign s_word_address = s_address[ADDRESS_WIDTH-1:ADDRESS_BITS_PER_WORD];   //convert byte address to word address
    
    acl_dcfifo #(
        .WIDTH                  (WORD_ADDRESS_WIDTH + BURSTCOUNT_WIDTH + BYTEENABLE_WIDTH + DATA_WIDTH + 1),
        .DEPTH                  (CMD_DCFIFO_DEPTH),
        .ALMOST_FULL_CUTOFF     (SLAVE_STALL_LATENCY),
        .NEVER_OVERFLOWS        ((SLAVE_STALL_LATENCY > 0) ? 1 : 0),
        .RAM_BLOCK_TYPE         (CMD_DCFIFO_RAM_TYPE)
    )
    cmd_dcfifo
    (
        .async_resetn           (async_resetn),
        .wr_clock               (s_clock),
        .wr_req                 ((s_read | s_write)),
        .wr_data                ({s_word_address, s_burstcount, s_byteenable, s_writedata, s_write}),
        .wr_full                (),
        .wr_almost_full         (s_waitrequest),
        .wr_read_update_for_ccb (),
        .rd_clock               (m_clock),
        .rd_ack                 (~m_throttle & ~m_waitrequest),
        .rd_empty               (m_empty_raw),
        .rd_almost_empty        (),
        .rd_data                ({m_word_address, m_burstcount, m_byteenable, m_writedata, m_is_write})
    );
    
    assign m_address = m_word_address << ADDRESS_BITS_PER_WORD;     //convert word address to byte address
    
    //factor in throttle before reporting empty status to outside world
    assign m_empty = m_throttle | m_empty_raw;
    
    //convert fifo signals to AvalonMM signals
    assign m_write_raw = ~m_empty &  m_is_write;
    assign m_read_raw  = ~m_empty & ~m_is_write;
    
    //if downstream provides an almost full, our outgoing valid means forced write, so we have to mask that in case waitrequest is asserted
    generate
    if (MASTER_STALL_LATENCY) begin     //force transaction to downstream
        assign m_write = m_write_raw & ~m_waitrequest;
        assign m_read = m_read_raw & ~m_waitrequest;
    end
    else begin                          //downstream may accept transaction
        assign m_write = m_write_raw;
        assign m_read = m_read_raw;
    end
    endgenerate
    
    
    
    //////////////////////////////////////////////////
    //                                              //
    //  Clock crossing fifo for read response data  //
    //                                              //
    //////////////////////////////////////////////////
    
    // RESPONSE_BACKPRESSURE = 1 is only used by the wide ccb. A normal clock crossing bridge (e.g. like the Qsys component) cannot backpressure read data.
    
    acl_dcfifo #(
        .WIDTH                  (DATA_WIDTH),
        .DEPTH                  (RSP_DCFIFO_DEPTH),
        .ALMOST_EMPTY_CUTOFF    (RESPONSE_ALMOST_EMPTY),
        .NEVER_OVERFLOWS        (1),
        .RAM_BLOCK_TYPE         (RSP_DCFIFO_RAM_TYPE)
    )
    rsp_dcfifo
    (
        .async_resetn           (async_resetn),
        .wr_clock               (m_clock),
        .wr_req                 (m_readdatavalid),
        .wr_data                (m_readdata),
        .wr_full                (),
        .wr_almost_full         (),
        .wr_read_update_for_ccb (m_read_update),
        .rd_clock               (s_clock),
        .rd_ack                 ((RESPONSE_BACKPRESSURE) ? ~s_readack_stall : 1'b1),
        .rd_empty               (s_readack_empty),
        .rd_almost_empty        (s_readack_almost_empty),
        .rd_data                (s_readdata)
    );
    assign s_readdatavalid = (RESPONSE_BACKPRESSURE) ? (~s_readack_empty & ~s_readack_stall) : ~s_readack_empty;
    
    // Same idea as above but for writeack.
    // Note that writeack and readdatavalid are handled completely separately. There is no point in enforcing order between writeacks and readdatavalids inside the CCB.
    // For OpenCL, kernel logic enforces memory dependencies by waiting for a writeack or readdatavalid to return before issuing the next request.
    // If one transaction depends on a previous one, those two transactions can never be outstanding at the same time by construction.
    // In other words, any transactions that are outstanding at the same time cannot have any dependencies between them.
    generate
    if (USE_WRITE_ACK == 0) begin : NO_WRITE_ACK_FIFO
        assign s_writeack = 1'b0;
        assign s_writeack_empty = 1'b1;
        assign s_writeack_almost_empty = 1'b1;
    end
    else begin : GEN_WRITE_ACK_FIFO
        //clock crosser
        acl_dcfifo #(
            .WIDTH                  (0),    //this is basically a dual clock occupancy tracker, no memory for data storage
            .DEPTH                  (RSP_DCFIFO_DEPTH),
            .ALMOST_EMPTY_CUTOFF    (RESPONSE_ALMOST_EMPTY),
            .NEVER_OVERFLOWS        (1)
        )
        writeack_rsp_dcfifo
        (
            .async_resetn           (async_resetn),
            .wr_clock               (m_clock),
            .wr_req                 (m_writeack),
            .wr_data                (),
            .wr_full                (),
            .wr_almost_full         (),
            .wr_read_update_for_ccb (m_writeack_read_update),
            .rd_clock               (s_clock),
            .rd_ack                 ((RESPONSE_BACKPRESSURE) ? ~s_writeack_stall : 1'b1),
            .rd_empty               (s_writeack_empty),
            .rd_almost_empty        (s_writeack_almost_empty),
            .rd_data                ()
        );
        assign s_writeack = (RESPONSE_BACKPRESSURE) ? (~s_writeack_empty & ~s_writeack_stall) : ~s_writeack_empty;
    end
    endgenerate
    
    
    
    //////////////////////
    //                  //
    //  Throttle logic  //
    //                  //
    //////////////////////
    
    // If read data goes from fast to slow clock, the response dcfifo cannot be drained as fast as it is filled.
    // Therefore to avoid overflow, never request more data than we have space for.
    
    // How lateness in the throttle logic affects the threshold:
    // - m_throttle is 3 clocks late (m_throttle is registerd, so one clock later than m_counter)
    // - due to lateness, we need to reserve 3*MAX_BURST_SIZE of full-side dead space in the response fifo
    // - we reserve an additional MAX_BURST_SIZE because by the time the MSB of m_counter asserts, we may have gone over by MAX_BURST_SIZE-1
    //   to illustrate this, think in base 10, the MSB asserts at a value of 100, MAX_BURST_SIZE is 10, and the counter is increasing like this: 79, 89, 99, 109
    // - total reserved space = 4*MAX_BURST_SIZE
    
    // Implementation:
    // - naive implementation would be to have a counter start at 0 and check if value is RSP_DCFIFO_DEPTH-4*MAX_BURST_SIZE or higher
    // - our implementation starts the counter at 4*MAX_BURST_SIZE and checks if the value is RSP_DCFIFO_DEPTH or higher
    // - since RSP_DCFIFO_DEPTH is a power of 2, just check if the MSB is 1
    
    localparam int unsigned THROTTLE_CUTOFF = 4*MAX_BURST_SIZE;     //based on reasoning in above comments, do not modify
    
    always_ff @(posedge m_clock) begin
        m_pending_reads_increase <= (m_read & ~m_waitrequest) ? m_burstcount : '0;  //on-time throttle -- read burst just went out this clock cycle, throttle as of the next clock cycle
        m_pending_reads_decrease <= m_read_update;
        m_counter_update <= m_pending_reads_increase - m_pending_reads_decrease;    //1 clock late
        m_counter <= m_counter + m_counter_update;                                  //2 clocks late
        if (~m_sclrn) m_counter <= THROTTLE_CUTOFF;    //offset so that the MSB detects almost full
    end
    
    // Same idea as above but for writeack
    generate
    if (USE_WRITE_ACK == 0) begin : NO_WRITE_ACK_THROTTLE
        assign m_writeack_counter = '0;
    end
    else begin : GEN_WRITE_ACK_THROTTLE
        //same throttle logic as above, but the increase is +1 instead of +burstcount
        //this assumes each word of write data has a write ack (same thing as each burst has a write ack if the burst length is always 1)
        always_ff @(posedge m_clock) begin
            m_writeack_pending_reads_increase <= (m_write & ~m_waitrequest) ? 1'b1 : 1'b0;
            m_writeack_pending_reads_decrease <= m_writeack_read_update;
            m_writeack_counter_update <= m_writeack_pending_reads_increase - m_writeack_pending_reads_decrease;
            m_writeack_counter <= m_writeack_counter + m_writeack_counter_update;
            if (~m_sclrn) m_writeack_counter <= 4*MAX_BURST_SIZE;
        end
    end
    endgenerate
    
    // If m_throttle is asserted, we cannot allow the transaction to go through:
    // - mask m_read and m_write (make it look like these are 0)
    // - ignore m_waitrequest (keep the read side of cmd_dcfifo stalled)
    
    // Properties
    // - m_throttle will not assert partway through a transaction (e.g. while waitrequest is stalling the current transaction) as that would result in cancelling the transaction which is illegal avalon behavior
    // - m_throttle can deassert at any time
    
    // Caveats:
    // - if USE_WRITE_ACK == 0 throttle logic should not affect writes, but the logic driving cmd_dcfifo|rd_stall is simpler if we mask reads and writes
    // - eventually rsp_dcfifo will drain since there is no backpressure, so it would only be a temporary starvation
    always_ff @(posedge m_clock) begin
        //too much oustanding stuff for some response dcfifo and just finished committing the current transaction
        if ((m_counter[RSP_COUNT_WIDTH-1] | m_writeack_counter[RSP_COUNT_WIDTH-1]) & ~m_waitrequest) m_throttle <= 1'b1;
        
        //if both response dcfifos will have space, allow transaction to be visible
        if (~m_counter[RSP_COUNT_WIDTH-1] & ~m_writeack_counter[RSP_COUNT_WIDTH-1]) m_throttle <= 1'b0;
        
        if (~m_sclrn) m_throttle <= 1'b0;
    end
    
    
endmodule

`default_nettype wire
