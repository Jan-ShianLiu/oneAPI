//__DESIGN_FILE_COPYRIGHT__

//  AvalonMM Wide Clock Crossing Bridge
//
//  This is a wrapper around several instances of acl_clock_crossing_bridge. It is intended for user logic that operates on a wide data path to interact with
//  several physical memories that have been width-stitched into one logical interface. On the slave side (user logic), one gets one wide AvalonMM interface. 
//  On the master side (memory interfaces), one gets several narrow AvalonMM interfaces which are width-sliced, e.g. address is the same, data is sliced.
//
//  Required files:
//  - acl_wide_clock_crossing_bridge.sv
//  - acl_sync.sv
//  - acl_desync.sv
//  - acl_clock_crossing_bridge.sv
//  - acl_dcfifo.sv
//  - acl_reset_handler.sv
//  - acl_parameter_assert.svh
//  - acl_width_clip.svh

`default_nettype none
`include "acl_parameter_assert.svh"

module acl_wide_clock_crossing_bridge #(
    //number of master interfaces
    parameter int unsigned NUM_MASTERS,                 //must be a power of 2, for actual width-stitching this should be 2 or more, but 1 is still allowed (it is functional)
    
    //signal widths -- all must be at least 1
    parameter int unsigned MASTER_ADDRESS_WIDTH,        //byte address must be word aligned, e.g. if MASTER_BYTEENABLE_WIDTH = 4, then address must be 4-byte aligned, bottom 2 bits must be 0
    parameter int unsigned MASTER_BYTEENABLE_WIDTH,     //must be a power of 2, specifies word size, note that MASTER_DATA_WIDTH is 8*MASTER_BYTEENABLE_WIDTH
    parameter int unsigned BURSTCOUNT_WIDTH,            //common between slave and master
    //slave signals widths are derived:
    //slave address width = master address width + number of bits needed to determine which master to target, i.e. $clog2(num nasters)
    //slave byte enable width = master byte enable width * number of masters
    
    //clock crossing fifo depths -- depths will be increased internally if the ccb requires a more capacity than the minimum requested by the user, internal depths will always be at least 8
    parameter int unsigned CMD_DCFIFO_MIN_DEPTH = 0,    //minimum capacity requested by user for the dcfifo for sending commands from slave to master
    parameter int unsigned RSP_DCFIFO_MIN_DEPTH = 0,    //minimum capacity requested by user for the dcfifo for receiving responses from master to slave
    
    //clock crossing fifo ram block type, if "MLAB" or "M20K" you will get what you ask for, otherwise the fifo will decide
    parameter string       CMD_DCFIFO_RAM_TYPE = "FIFO_TO_CHOOSE",
    parameter string       RSP_DCFIFO_RAM_TYPE = "FIFO_TO_CHOOSE",
    
    //waitrequest allowance settings
    parameter int unsigned SLAVE_STALL_LATENCY = 0,     //if >= 1 then outgoing s_waitrequest is an almost full to upstream, incoming s_read and s_write cannot be backpressured
    parameter int unsigned MASTER_STALL_LATENCY = 0,    //if >= 1 then incoming m_waitrequest is an almost full from downstream, outgoing m_read and m_write are forced transactions
    
    //internal latency for width stitching
    parameter int unsigned ALMOST_FULL_LATENCY = 2,     //latency to collect almost full from each cmd dcfifo
    parameter int unsigned FORCED_WRITE_LATENCY = 2,    //latency from valid_in from outside world to each cmd dcfifo
    parameter int unsigned ALMOST_EMPTY_LATENCY = 3,    //latency to collect empty and almost empty from each rsp dcfifo
    parameter int unsigned FORCED_READ_LATENCY = 3,     //latency from sync decision to read acknolwedge of each rsp dcfifo
    
    //additional features
    parameter bit          SPLIT_WRITE_BURSTS = 1,      //arbitration between pcie and hbm becomes very simple if all transactions are contained within one word, read requests can still be for a burst
    parameter int unsigned BURST_BOUNDARY = 0,          //set to nonzero to specify what address size a burst will not cross, e.g. 12 means bursts cannot cross a 4K boundary
    
    //write acknowledge -- there is no ordering between readdatavalid and writeack, intended only for use with AXI on HBM, this is not avalon conformant
    parameter bit          USE_WRITE_ACK = 0,
    
    //derived parameters
    localparam int unsigned EXTRA_SLAVE_ADDRESS_BITS = $clog2(NUM_MASTERS),                         //how many more bits does SLAVE_ADDRESS_WIDTH have compared to MASTER_ADDRESS_WIDTH
    localparam int unsigned SLAVE_ADDRESS_WIDTH = MASTER_ADDRESS_WIDTH + EXTRA_SLAVE_ADDRESS_BITS,  //upper bits of slave are decoded to determine which master to target
    localparam int unsigned SLAVE_BYTEENABLE_WIDTH = MASTER_BYTEENABLE_WIDTH * NUM_MASTERS,         //data path is width stitched
    localparam int unsigned MASTER_DATA_WIDTH = 8 * MASTER_BYTEENABLE_WIDTH,
    localparam int unsigned SLAVE_DATA_WIDTH = 8 * SLAVE_BYTEENABLE_WIDTH,
    localparam int unsigned CMD_WIDTH = 2 + MASTER_ADDRESS_WIDTH + BURSTCOUNT_WIDTH + MASTER_BYTEENABLE_WIDTH + MASTER_DATA_WIDTH   //bundle of cmd interface = [write, read, burstcount, byteenable, writedata, address]
) (
    input  wire                                 async_resetn,       //asynchronous reset, internally we synchronize it to both clocks, if you have a resetn from each clock domain then AND them together     
    
    //AvalonMM slave interface -- connects to a single upstream master
    input  wire                                s_clock,
    input  wire                                s_read,
    input  wire                                s_write,
    input  wire      [SLAVE_ADDRESS_WIDTH-1:0] s_address,           //this is a byte address that must be word aligned, e.g. if SLAVE_DATA_WIDTH = 32, s_address must be 4-byte aligned, so lower 2 bits are zero
    input  wire         [BURSTCOUNT_WIDTH-1:0] s_burstcount,
    input  wire   [SLAVE_BYTEENABLE_WIDTH-1:0] s_byteenable,
    input  wire         [SLAVE_DATA_WIDTH-1:0] s_writedata,
    output logic                               s_waitrequest,
    output logic                               s_readdatavalid,
    output logic        [SLAVE_DATA_WIDTH-1:0] s_readdata,
    output logic                               s_writeack,          //not part of AvalonMM interface, see comment about USE_WRITE_ACK
    
    //AvalonMM master interface -- connects to multiple downstream slaves
    input  wire                                m_clock         [NUM_MASTERS-1:0],
    output logic                               m_read          [NUM_MASTERS-1:0],
    output logic                               m_write         [NUM_MASTERS-1:0],
    output logic    [MASTER_ADDRESS_WIDTH-1:0] m_address       [NUM_MASTERS-1:0],   //this is also a byte address that is word aligned to MASTER_DATA_WIDTH
    output logic        [BURSTCOUNT_WIDTH-1:0] m_burstcount    [NUM_MASTERS-1:0],
    output logic [MASTER_BYTEENABLE_WIDTH-1:0] m_byteenable    [NUM_MASTERS-1:0],
    output logic       [MASTER_DATA_WIDTH-1:0] m_writedata     [NUM_MASTERS-1:0],
    input  wire                                m_waitrequest   [NUM_MASTERS-1:0],
    input  wire                                m_readdatavalid [NUM_MASTERS-1:0],
    input  wire        [MASTER_DATA_WIDTH-1:0] m_readdata      [NUM_MASTERS-1:0],
    input  wire                                m_writeack      [NUM_MASTERS-1:0],   //not part of AvalonMM interface, see comment about USE_WRITE_ACK
    output logic                               m_empty         [NUM_MASTERS-1:0]    //to indicate cmd_dcfifo has data to send, m_read and m_write no longer convey this info when MASTER_STALL_LATENCY = 1 (forced transactions)
);
    
    //////////////////////////////////////
    //                                  //
    //  Sanity check on the parameters  //
    //                                  //
    //////////////////////////////////////
    
    generate
    `ACL_PARAMETER_ASSERT(NUM_MASTERS >= 1)
    `ACL_PARAMETER_ASSERT(NUM_MASTERS == 2**EXTRA_SLAVE_ADDRESS_BITS)
    `ACL_PARAMETER_ASSERT(MASTER_ADDRESS_WIDTH >= 1)
    `ACL_PARAMETER_ASSERT(MASTER_BYTEENABLE_WIDTH >= 1)
    //all of the other parameters like ***_WIDTH and ***_STALL_LATENCY are checked in the underlying ccb instances
    endgenerate
    
    
    
    ///////////////////////////
    //                       //
    //  Signal declarations  //
    //                       //
    ///////////////////////////
    
    // naming convention: all signal names begin with m_ or s_ and that indicates which clock domain it is on (master or slave, respectively)
    
    genvar g;
    logic [SLAVE_ADDRESS_WIDTH-1:0] s_noburst_address;                  //address after write bursts have been split (read bursts are unchanged)
    logic [BURSTCOUNT_WIDTH-1:0] s_noburst_burstcount;                  //burstcount after write bursts have been split (read bursts are unchanged) - 1 for writes, reads retain original value
    logic [NUM_MASTERS-1:0] s_waitrequest_raw;                          //slave waitrequest from each ccb
    logic [NUM_MASTERS*CMD_WIDTH-1:0] s_cmd_before, s_cmd_after;        //cmd bundles before and after acl_desync
    logic s_valid_before;                                               //this is stall latency valid = forced transaction, before acl_desync
    logic [NUM_MASTERS-1:0] s_valid_after;                              //same as above but after acl_desync, one copy for each ccb
    logic [NUM_MASTERS-1:0] s_readack_empty, s_readack_almost_empty;    //status signals from the read side of rsp_dcfifo for readdatavalid
    logic [NUM_MASTERS-1:0] s_readack_stall;                            //stall signal going to read side of rsp_dcfifo for readdatavalid
    logic [NUM_MASTERS-1:0] s_writeack_empty, s_writeack_almost_empty;  //likewise for writeack
    logic [NUM_MASTERS-1:0] s_writeack_stall;
    
    
    
    //////////////////////////
    //                      //
    //  Split write bursts  //
    //                      //
    //////////////////////////
    
    generate
    if (SPLIT_WRITE_BURSTS) begin : GEN_WRITE_BURST_SPLITTER
        acl_burst_splitter #(
            .ADDRESS_WIDTH      (SLAVE_ADDRESS_WIDTH),
            .BURSTCOUNT_WIDTH   (BURSTCOUNT_WIDTH),
            .BYTEENABLE_WIDTH   (SLAVE_BYTEENABLE_WIDTH),
            .DATA_WIDTH         (SLAVE_DATA_WIDTH),
            .SPLIT_WRITE_BURSTS (1),
            .SPLIT_READ_BURSTS  (0),
            .USE_STALL_LATENCY  ((SLAVE_STALL_LATENCY > 0) ? 1 : 0),
            .BURST_BOUNDARY     (BURST_BOUNDARY),
            .ASYNC_RESET        (0),
            .SYNCHRONIZE_RESET  (1),
            .BACKPRESSURE_DURING_RESET  (0)
        )
        acl_write_burst_splitter_inst
        (
            .clock              (s_clock),
            .resetn             (async_resetn),
            .up_waitrequest     (),                     //can ignore since no change in control flow
            .up_read            (s_read),
            .up_write           (s_write),
            .up_address         (s_address),
            .up_writedata       (),                     //data path passes through without change
            .up_byteenable      (),                     //data path passes through without change
            .up_burstcount      (s_burstcount),
            .down_waitrequest   (s_waitrequest),
            .down_read          (),                     //can ignore since no change in control flow
            .down_write         (),                     //can ignore since no change in control flow
            .down_address       (s_noburst_address),
            .down_writedata     (),                     //data path passes through without change
            .down_byteenable    (),                     //data path passes through without change
            .down_burstcount    (s_noburst_burstcount)
        );
    end
    else begin : NO_WRITE_BURST_SPLITTER
        assign s_noburst_address = s_address;
        assign s_noburst_burstcount = s_burstcount;
    end
    endgenerate
    
    
    
    //////////////////////////////////
    //                              //
    //  Write-side width-stitching  //
    //                              //
    //////////////////////////////////
    
    //make NUM_MASTERS bundles of [write, read, address, burstcount, byteenable, writedata] at the width of the master interface
    //common signals [write, read, burstcount] should be replicated per master interface for physical routability
    //data signals [writedata, byteenable] are width-sliced -- master interface i should grab bits [i*SIZE +: SIZE] from the slave interface
    //address is a bit of a special case - the bottom $clog2(NUM_MASTERS) bits need to be removed from the slave address to get the master address
    always_comb begin
        for (int unsigned i=0; i<NUM_MASTERS; i++) begin
            s_cmd_before[i*CMD_WIDTH +: CMD_WIDTH] =
                { s_write, s_read, s_noburst_burstcount,
                  s_byteenable[i*MASTER_BYTEENABLE_WIDTH +: MASTER_BYTEENABLE_WIDTH], s_writedata[i*MASTER_DATA_WIDTH +: MASTER_DATA_WIDTH],
                  s_noburst_address[SLAVE_ADDRESS_WIDTH-1:EXTRA_SLAVE_ADDRESS_BITS] };
        end
    end
    assign s_valid_before = (SLAVE_STALL_LATENCY) ? (s_write | s_read) : ((s_write | s_read) & ~s_waitrequest);    //this is stall latency valid = forced transaction
    
    //perform a desync on the cmd interface
    acl_desync_predicate_nonblocking #(
        .NUM_IFACES         (NUM_MASTERS),
        .DATA_WIDTH         (CMD_WIDTH),        //per downstream interface -- single upstream interface has width = NUM_MASTERS*CMD_WIDTH
        .ASYNC_RESET        (0),      
        .SYNCHRONIZE_RESET  (1),
        .RESET_EVERYTHING   (0),
        .STALL_LATENCY      (0),                                    //no latency from merge point of almost full until s_waitrequest, which is backpressure to upstream
        .DATA_LATENCY       ({NUM_MASTERS{FORCED_WRITE_LATENCY}}),  //latency from almost full from each fifo until merge point
        .FULL_LATENCY       ({NUM_MASTERS{ALMOST_FULL_LATENCY}}),   //latency from external facing Avalon slave (upstream) to write side of each underlying cmd_dcfifo (downstream)
        .STALL_IN_EARLINESS (0),
        .VALID_IN_EARLINESS (0),
        .NON_BLOCKING       (0)
    )
    acl_desync_predicate_nonblocking_inst
    (
        .clock              (s_clock),
        .resetn             (async_resetn),
        
        //upstream - single fifo read side interface -- this is the external facing Avalon slave, command part of the interface
        .i_data             (s_cmd_before),
        .i_valid            (s_valid_before),
        .i_predicate        (1'b0),
        .i_empty            (~s_valid_before),
        .o_stall            (s_waitrequest),
        
        //downstream - multiple fifo write side interfaces -- this is the write side of each underlying cmd_dcfifo
        .o_data             (s_cmd_after),
        .o_valid            (s_valid_after),
        .i_stall            (s_waitrequest_raw),
        
        //profiler -- unused
        .o_profiler_downstream_stall ()
    );
    
    
    
    ///////////////////////////////////
    //                               //
    //  Instantiate underlying CCBs  //
    //                               //
    ///////////////////////////////////
    
    generate
    for (g=0; g<NUM_MASTERS; g++) begin : GEN_CCB
        logic s_this_write, s_this_read;
        logic [MASTER_ADDRESS_WIDTH-1:0] s_this_address;
        logic [BURSTCOUNT_WIDTH-1:0] s_this_burstcount;
        logic [MASTER_BYTEENABLE_WIDTH-1:0] s_this_byteenable;
        logic [MASTER_DATA_WIDTH-1:0] s_this_writedata;
        
        //unpack the cmd bundle
        assign {s_this_write, s_this_read, s_this_burstcount, s_this_byteenable, s_this_writedata, s_this_address} = s_cmd_after[g*CMD_WIDTH +: CMD_WIDTH];
        
        acl_clock_crossing_bridge #(
            .ADDRESS_WIDTH              (MASTER_ADDRESS_WIDTH),
            .BURSTCOUNT_WIDTH           (BURSTCOUNT_WIDTH),
            .BYTEENABLE_WIDTH           (MASTER_BYTEENABLE_WIDTH),
            .CMD_DCFIFO_MIN_DEPTH       (CMD_DCFIFO_MIN_DEPTH),
            .RSP_DCFIFO_MIN_DEPTH       (RSP_DCFIFO_MIN_DEPTH),
            .CMD_DCFIFO_RAM_TYPE        (CMD_DCFIFO_RAM_TYPE),
            .RSP_DCFIFO_RAM_TYPE        (RSP_DCFIFO_RAM_TYPE),
            .SLAVE_STALL_LATENCY        (ALMOST_FULL_LATENCY + FORCED_WRITE_LATENCY + SLAVE_STALL_LATENCY), //this sets ALMOST_FULL_CUTOFF in cmd_dcfifo
            .MASTER_STALL_LATENCY       (MASTER_STALL_LATENCY),
            .USE_WRITE_ACK              (USE_WRITE_ACK),
            .RESPONSE_BACKPRESSURE      (1),
            .RESPONSE_ALMOST_EMPTY      (ALMOST_EMPTY_LATENCY + FORCED_READ_LATENCY)
        )
        acl_clock_crossing_bridge_inst
        (
            .async_resetn               (async_resetn),
            
            //slave interface -- cmd
            .s_clock                    (s_clock),
            .s_read                     (s_valid_after[g] & s_this_read),
            .s_write                    (s_valid_after[g] & s_this_write),
            .s_address                  (s_this_address),
            .s_burstcount               (s_this_burstcount),
            .s_byteenable               (s_this_byteenable),
            .s_writedata                (s_this_writedata),
            .s_waitrequest              (s_waitrequest_raw[g]),
            
            //slave interface -- rsp
            .s_readdatavalid            (),
            .s_readack_empty            (s_readack_empty[g]),
            .s_readack_almost_empty     (s_readack_almost_empty[g]),
            .s_readack_stall            (s_readack_stall[g]),
            .s_readdata                 (s_readdata[g*MASTER_DATA_WIDTH +: MASTER_DATA_WIDTH]),
            .s_writeack                 (),
            .s_writeack_empty           (s_writeack_empty[g]),
            .s_writeack_almost_empty    (s_writeack_almost_empty[g]),
            .s_writeack_stall           (s_writeack_stall[g]),
            
            //master interface
            .m_clock                    (m_clock[g]),
            .m_read                     (m_read[g]),
            .m_write                    (m_write[g]),
            .m_address                  (m_address[g]),
            .m_burstcount               (m_burstcount[g]),
            .m_byteenable               (m_byteenable[g]),
            .m_writedata                (m_writedata[g]),
            .m_waitrequest              (m_waitrequest[g]),
            .m_readdatavalid            (m_readdatavalid[g]),
            .m_readdata                 (m_readdata[g]),
            .m_writeack                 (m_writeack[g]),
            .m_empty                    (m_empty[g])
        );
    end
    endgenerate
    
    
    
    /////////////////////////////////
    //                             //
    //  Read-side width-stitching  //
    //                             //
    /////////////////////////////////
    
    generate
    if (NUM_MASTERS > 1) begin : GEN_RESPONSE_SYNC
        
        //synchronize readdatavalid
        acl_sync_predicate_nonblocking
        #(
            .NUM_IFACES                 (NUM_MASTERS),
            .DATA_WIDTH                 (0),    //data handled externally to acl_sync
            .ASYNC_RESET                (0),
            .SYNCHRONIZE_RESET          (1),
            .RESET_EVERYTHING           (0),
            .EMPTY_PLUS_STALL_LATENCY   (ALMOST_EMPTY_LATENCY + FORCED_READ_LATENCY),   //roundtrip latency used by centralized slow read/fast read state machine
            .EMPTY_LATENCY              ({NUM_MASTERS{ALMOST_EMPTY_LATENCY}}),  //latency from rsp_dcfifo empty/almost_empty to the centralized slow read/fast read state machine
            .STALL_LATENCY              ({NUM_MASTERS{FORCED_READ_LATENCY}}),   //latency from centralized slow read/fast read state machine to stall_in of each rsp_dcfifo
            .DATA_LATENCY               (0),    //no pipelining from read side of rsp_dcfifo to s_readdata output port
            .FULL_LATENCY               (0),    //there is no almost_full from downstream - Avalon reads cannot be backpressured
            .STALL_IN_EARLINESS         (0),
            .VALID_IN_EARLINESS         (0),
            .NON_BLOCKING               (0)
        )
        acl_sync_predicate_nonblocking_inst
        (
            .clock                      (s_clock),
            .resetn                     (async_resetn),
            
            //upstream - multiple fifo read interfaces
            .i_data                     (),         //not driven -- to be managed externally from acl_sync
            .i_almost_empty             (s_readack_almost_empty),
            .i_empty                    (s_readack_empty),
            .o_stall                    (s_readack_stall),
            
            //upstream predication
            .i_predicate_data           (1'b0),     //tie off
            .o_predicate_stall          (),         //ignored
            
            //downstream - single fifo write interface
            .o_valid                    (s_readdatavalid),
            .o_data                     (),         //ignored -- to be managed externally from acl_sync
            .i_stall                    (1'b0),     //Avalon reads cannot be backpressured
            
            //profiler -- unused
            .o_profiler_upstream_stall_fast_read (),
            .o_profiler_upstream_stall_slow_read ()
        );
        
        //this is pretty much the same as above but for writeack
        acl_sync_predicate_nonblocking
        #(
            .NUM_IFACES                 (NUM_MASTERS),
            .DATA_WIDTH                 (0),    //data handled externally to acl_sync
            .ASYNC_RESET                (0),
            .SYNCHRONIZE_RESET          (1),
            .RESET_EVERYTHING           (0),
            .EMPTY_PLUS_STALL_LATENCY   (ALMOST_EMPTY_LATENCY + FORCED_READ_LATENCY),   //roundtrip latency used by centralized slow read/fast read state machine
            .EMPTY_LATENCY              ({NUM_MASTERS{ALMOST_EMPTY_LATENCY}}),  //latency from rsp_dcfifo empty/almost_empty to the centralized slow read/fast read state machine
            .STALL_LATENCY              ({NUM_MASTERS{FORCED_READ_LATENCY}}),   //latency from centralized slow read/fast read state machine to stall_in of each rsp_dcfifo
            .DATA_LATENCY               (0),    //unlike readdatavalid, there is no data associated with writeack
            .FULL_LATENCY               (0),    //there is no almost_full from downstream - no backpressure
            .STALL_IN_EARLINESS         (0),
            .VALID_IN_EARLINESS         (0),
            .NON_BLOCKING               (0)
        )
        acl_sync_predicate_nonblocking_inst2
        (
            .clock                      (s_clock),
            .resetn                     (async_resetn),
            
            //upstream - multiple fifo read interfaces
            .i_data                     (),         //not driven -- to be managed externally from acl_sync
            .i_almost_empty             (s_writeack_almost_empty),
            .i_empty                    (s_writeack_empty),
            .o_stall                    (s_writeack_stall),
            
            //upstream predication
            .i_predicate_data           (1'b0),     //tie off
            .o_predicate_stall          (),         //ignored
            
            //downstream - single fifo write interface
            .o_valid                    (s_writeack),
            .o_data                     (),         //ignored -- to be managed externally from acl_sync
            .i_stall                    (1'b0),     //no backpressure
            
            //profiler -- unused
            .o_profiler_upstream_stall_fast_read (),
            .o_profiler_upstream_stall_slow_read ()
        );
    end
    else begin : NO_RESPONSE_SYNC   //NUM_MASTERS == 1
        //almost empty signals are not used, those occupancy trackers inside the response dcfifos will be pruned away
        assign s_readack_stall[0] = 1'b0;
        assign s_readdatavalid = ~s_readack_empty[0];
        
        assign s_writeack_stall[0] = 1'b0;
        assign s_writeack = ~s_writeack_empty[0];
    end
    endgenerate
    
endmodule

`default_nettype wire
