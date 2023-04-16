`default_nettype none
module ccb
#(
    parameter DATA_W           = 512,
              ADDRESS_W        = 31,
              SYMBOL_W         = 8,
              BYTEENABLE_W     = DATA_W / SYMBOL_W,
              MAX_BURST_SIZE   = 16,
              BURSTCOUNT_W     = $clog2(MAX_BURST_SIZE) + 1,

              CMD_SCFIFO_DEPTH = 512, // hw_tcl was used to enforce a minimum command and response scfifo depth
              RSP_SCFIFO_DEPTH = 512,
        
              USE_ERROR_PREVENTION = 1,
              FAST_TO_SLOW         = 0,
              // A value of 0 means waitrequest allowance is disabled. A value > 0 specifies how many words the slave port can accept once slave_waitrequest asserts.
              SLAVE_PORT_WAITREQUEST_ALLOWANCE = 0,
              /* The AvalonMM spec defines a waitrequest allowance parameter for master ports (in addition to slave ports). The usage of the phrase "waitrequest allowance" is a bit misleading for master ports because 
                 the master port sends requests rather than receives them. For master ports this parameter effectively specifies the internal latency.
                 The spec says that a value of 0 means waitrequest allowance is disabled. A value > 0 specifies the amount of latency from master_waitrequest asserting to master_read/write de-asserting. 
		 If waitrequest allowance is desired, the minimum value is 1 to be spec-compliant. This module technically can support an internal latency of 0 on this interface, but that's
                 not spec-compliant if WRA is used. So we force an internal latency to exist, which is implemented using the FIFO stall-in-earliness feature (see below).
              */
              MASTER_PORT_WAITREQUEST_ALLOWANCE = 0     
)
(
    input wire                              slave_clk,

    input wire                              master_clk,
    input wire                              master_reset,

    // slave interface
    input wire      [ADDRESS_W-1    : 0]    slave_address,
    input wire      [BYTEENABLE_W-1 : 0]    slave_byteenable,
    input wire                              slave_read,
    input wire                              slave_write,
    input wire      [DATA_W-1       : 0]    slave_writedata,
    input wire      [BURSTCOUNT_W-1 : 0]    slave_burstcount,
    output logic                            slave_waitrequest,
    output logic    [DATA_W-1       : 0]    slave_readdata,
    output logic                            slave_readdatavalid,
    
    // master interface
    output logic    [ADDRESS_W-1    : 0]    master_address,
    output logic    [BYTEENABLE_W-1 : 0]    master_byteenable,
    output logic                            master_read,
    output logic                            master_write,
    output logic    [DATA_W-1       : 0]    master_writedata,
    output logic    [BURSTCOUNT_W-1 : 0]    master_burstcount,
    input wire                              master_waitrequest,
    input wire      [DATA_W-1       : 0]    master_readdata,
    input wire                              master_readdatavalid
);

    // The Avalon-MM spec says that if waitrequest-allowance=0 then the feature is disabled and the interface uses standard handshaking (ie. immediate backpressure and avm_read/write must remain asserted
    // when waitrequest is asserted).
    localparam ENABLE_SLAVE_PORT_WRA = SLAVE_PORT_WAITREQUEST_ALLOWANCE > 0; // WRA = WaitRequest Allowance
    localparam ENABLE_MASTER_PORT_WRA = MASTER_PORT_WAITREQUEST_ALLOWANCE > 0;

    /*
     CMD_DCFIFO_ALMOST_FULL_THRESH = cmd_dcfifo_depth - 2 - WAITREQUEST_ALLOWANCE
     cmd_dcfifo_depth = 31
     The -2 is to account for cmd_dcfifo's latency from its internal write-side occupancy updating to din_ready toggling.
     SLAVE_PORT_WAITREQUEST_ALLOWANCE should be set to the maximum upstream round-trip latency (ie. latency from slave_waitrequest asserting to slave_read/write de-asserting)
     If ENABLE_WAITREQUEST_ALLOWANCE == 0, we cap the almost-full threshold at 23 to artificially limit the capacity of the CCB as a band-aid solution for preventing thrashing between reads and writes
     in the global memory interconnect (Case:495449). Setting it smaller results in lower throughput because the cmd_dcfifo can drain (ie. when de-asserting slave_waitrequest, it takes time for new data to arrive at the CCB and be readable).
     It's quite possible we don't need this special case but we'd need to test it.
    */
    localparam CMD_DCFIFO_ALMOST_FULL_THRESH = ENABLE_SLAVE_PORT_WRA? (31 - 2 - SLAVE_PORT_WAITREQUEST_ALLOWANCE) : 23;
    
    localparam DCFIFO_INTERNAL_READ_LATENCY = 2;        // When read-req toggles, output valid toggles 2 cycles later.
    
    /*
    RSP_DCFIFO_ALMOST_FULL_THRESH = rsp_dcfifo_depth - 2 - UPSTREAM_ROUNDTRIP = 31 - 2 - 3 = 26
    The -2 is to account for rsp_dcfifo's internal latency from din_req to din_ready.
    The -3 is the roundtrip latency upstream of rsp_dcfifo, from din_ready de-asssertion to deassertion of wrreq to rsp_dcfifo.
    */
    localparam RSP_DCFIFO_ALMOST_FULL_THRESH = 26;

    // Max width size for the RSP_SCFIFO_DEPTH
    // 10 bits for a range from 0 - 512 
    localparam MAX_RSP_SCFIFO_DEPTH_WIDTH        = $clog2(512) + 1;

    localparam PAD_INCR_VAL                      = (BURSTCOUNT_W <= MAX_RSP_SCFIFO_DEPTH_WIDTH) ?
                                                   MAX_RSP_SCFIFO_DEPTH_WIDTH - BURSTCOUNT_W : 0;
    // -----------------------------------------------
    // Resets
    //
    // To be safe, we want to reset the read-side of 
    // all async FIFOs first.
    // -----------------------------------------------
    reg [2:0] slave_rst_r /* synthesis preserve */;
    reg slave_rst_d0, slave_rst_d1, slave_rst_d2; // pipeline reset for retiming
    reg master_rst;
    reg master_rst_d;

    // command side reset configuration
    always @(posedge master_clk) begin
        master_rst    <= master_reset;
        master_rst_d  <= master_rst;
    end

    always @(posedge slave_clk) begin
        slave_rst_r[0] <= master_rst_d;
        slave_rst_r[1] <= slave_rst_r[0];
        slave_rst_r[2] <= slave_rst_r[1];

        slave_rst_d0    <= slave_rst_r[2];
        slave_rst_d1    <= slave_rst_d0;
        slave_rst_d2    <= slave_rst_d1;
    end

    // -----------------------------------------------
    // Command path
    //
    // Wire all signals through a synchronous FIFO for depth,
    // then a shallower async FIFO for clock crossing.
    // -----------------------------------------------
    localparam TRANS_W = 2; // read, write
    localparam CMD_FIFO_W = TRANS_W + ADDRESS_W + DATA_W + BURSTCOUNT_W + BYTEENABLE_W;

    wire [CMD_FIFO_W-1:0] cmd_data_in;

    wire                  cmd_scfifo_valid_in;
    wire                  cmd_scfifo_almost_full;

    wire [CMD_FIFO_W-1:0] cmd_scfifo_data_out;
    wire                  cmd_scfifo_valid_out;
    wire                  cmd_scfifo_stall_in;

    // TODO: optimize which bits are co-located
    assign cmd_data_in = { slave_read, 
                           slave_write,
                           slave_address, 
                           slave_byteenable, 
                           slave_burstcount, 
                           slave_writedata };

    // If ENABLE_SLAVE_PORT_WRA==1 then ignore slave_waitrequest. slave_read/slave_write force the CCB to accept the request
    assign cmd_scfifo_valid_in = (slave_read | slave_write) & (~slave_waitrequest || ENABLE_SLAVE_PORT_WRA);
    


    wire [CMD_FIFO_W-1:0] cmd_dcfifo_data_out;
    wire                  cmd_dcfifo_valid_out;
    wire                  cmd_dcfifo_rreq;
    wire                  cmd_dcfifo_data_out_rd_trans;
    wire                  cmd_dcfifo_data_out_wr_trans;
    wire                  cmd_dcfifo_din_ready;
    wire                  cmd_throttle;

    wire    [ADDRESS_W-1    : 0] cmd_dcfifo_data_out_address;
    wire    [DATA_W-1       : 0] cmd_dcfifo_data_out_writedata;
    wire    [BURSTCOUNT_W-1 : 0] cmd_dcfifo_data_out_burstcount;
    wire    [BYTEENABLE_W-1 : 0] cmd_dcfifo_data_out_byteenable;
    wire                         cmd_dcfifo_data_out_read;

    wire                  ready_d0;
    reg                   ready_d1, ready_d2, ready_d3;
    
    wire                  zero_latency_handshake_almost_full;
    wire                  zero_latency_handshake_data_out_rd_trans;
    wire                  zero_latency_handshake_data_out_wr_trans;
    wire                  zero_latency_handshake_valid_out;
    wire [CMD_FIFO_W-1:0] zero_latency_handshake_data_out;
    logic cmd_dcfifo_qual_rreq;
    
    //bypass the front fifo
    assign slave_waitrequest = ~cmd_dcfifo_din_ready;
    
    // this fifo has a fixed capacity of 31 because it requires an MLAB
    // 3 clocks of latency from din_wreq to din_ready, READY_THRESHOLD is the almost full threshold and the maximum usable capacity is 2 above this
    // cmd_dcfifo_din_ready de-asserts (ie. "almost-full" asserts) when the FIFO's occupancy is CMD_DCFIFO_ALMOST_FULL_THRESH+2. This is because of the internal latency from din_req to din_ready.
    alt_st_mlab_dcfifo 
    #(
        .DATA_WIDTH        (CMD_FIFO_W),
        .PREVENT_UNDERFLOW (1),
        .READY_THRESHOLD   (CMD_DCFIFO_ALMOST_FULL_THRESH),
        .SIM_EMULATE       (1'b0)
    ) cmd_dcfifo (
        .din_clk     (slave_clk),
        .din_sclr    (slave_rst_d2),
        .din_wreq    (cmd_scfifo_valid_in),     //this means forced write
        .din         (cmd_data_in),
        .din_ready   (cmd_dcfifo_din_ready),

        .dout_clk    (master_clk),
        .dout_sclr   (master_rst_d),
        .dout_rreq   (cmd_dcfifo_rreq),
        .dout_valid  (cmd_dcfifo_valid_out),    //this means forced read, the backpressure is merged in early by cmd_dcfifo_rreq
        .dout        (cmd_dcfifo_data_out),
        .dout_qual_rreq (cmd_dcfifo_qual_rreq)  //did we read, if so we will get cmd_dcfifo_valid_out in 2 clocks from now
    );
    

    
    assign { cmd_dcfifo_data_out_rd_trans,
             cmd_dcfifo_data_out_wr_trans,
             cmd_dcfifo_data_out_address,
             cmd_dcfifo_data_out_byteenable,
             cmd_dcfifo_data_out_burstcount,
             cmd_dcfifo_data_out_writedata } = cmd_dcfifo_data_out;
   
    //used by throttle logic, so anything inside zero_latency_handshake_scfifo is counted as an outstanding transaction
    assign cmd_dcfifo_data_out_read = cmd_dcfifo_data_out_rd_trans & cmd_dcfifo_valid_out;
    assign cmd_dcfifo_rreq = USE_ERROR_PREVENTION && !FAST_TO_SLOW ? ~cmd_throttle: ~zero_latency_handshake_almost_full;
    
    //add a fifo to get same clock cycle handshaking to outside world
    //this can be removed once all qsys components support waitrequest latency
    hld_fifo #(
        .WIDTH (CMD_FIFO_W),
        .DEPTH (4),                     //absolute minimum
        .ALMOST_EMPTY_CUTOFF (0),
        .ALMOST_FULL_CUTOFF (0),
        .INITIAL_OCCUPANCY (0),
        .ASYNC_RESET (0),
        .SYNCHRONIZE_RESET (0),
        .RESET_EVERYTHING (0),
        .RESET_EXTERNALLY_HELD (1),
        .STALL_IN_EARLINESS (ENABLE_MASTER_PORT_WRA? MASTER_PORT_WAITREQUEST_ALLOWANCE : 0), // Use the FIFO's stall-in-earliness feature to implement the specified master port WRA (which is simply the internal latency on the master port).
        .VALID_IN_EARLINESS (0),
        .REGISTERED_DATA_OUT_COUNT (CMD_FIFO_W),//register the output data so that the pipeline bridge that follows does not need to be pulled close to here
        .NEVER_OVERFLOWS (0),
        .HOLD_DATA_OUT_WHEN_EMPTY (0),
        .WRITE_AND_READ_DURING_FULL (0),
        .USE_STALL_LATENCY_UPSTREAM (0),
        .USE_STALL_LATENCY_DOWNSTREAM (ENABLE_MASTER_PORT_WRA),
        .STYLE ("ll")
    )
    zero_latency_handshake_scfifo
    (
        .clock          (master_clk),
        .resetn         (~master_rst_d),
    
        .i_data         (cmd_dcfifo_data_out),
        .i_valid        (cmd_dcfifo_valid_out),
        .o_stall        (),
        .o_almost_full  (),
    
        .o_data         (zero_latency_handshake_data_out),
        .o_valid        (zero_latency_handshake_valid_out),
        .i_stall        (master_waitrequest),
        .o_almost_empty (),
        .o_empty        ()
    );
    
    // value of occ     | actual occupancy
    // -----------------+-----------------
    // 4'b0000          | 0
    // 4'b0001          | 1
    // 4'b0011          | 2
    // 4'b0111          | 3
    // 4'b1111          | 4
    
    logic [3:0] occ;    //use the same occupancy tracking scheme as inside acl_low_latency_fifo
    logic occ_incr;
    logic occ_decr;
    
    assign occ_incr = cmd_dcfifo_qual_rreq; //did the dcfifo decide to proceed with a read, if so we will get data out in 2 clocks from now and will write into zero_latency_handshake_scfifo in 2 clocks from now
    assign occ_decr = zero_latency_handshake_valid_out & (~master_waitrequest || ENABLE_MASTER_PORT_WRA);   //did we read from zero_latency_handshake_scfifo. Valid is forced if ENABLE_MASTER_PORT_WRA==1.
    
    always_ff @(posedge master_clk) begin
        if (master_rst_d) begin
            occ <= 4'h0;
        end
        else begin
            occ[0] <= (occ_incr & ~occ_decr) ?   1'b1 : (~occ_incr & occ_decr) ? occ[1] : occ[0];
            occ[1] <= (occ_incr & ~occ_decr) ? occ[0] : (~occ_incr & occ_decr) ? occ[2] : occ[1];
            occ[2] <= (occ_incr & ~occ_decr) ? occ[1] : (~occ_incr & occ_decr) ? occ[3] : occ[2];
            occ[3] <= (occ_incr & ~occ_decr) ? occ[2] : (~occ_incr & occ_decr) ?   1'b0 : occ[3];
        end
    end
    assign zero_latency_handshake_almost_full = occ[3]; //this only turns on when occupancy is 4 -- this means we have reserved space for 4 items but don't necessarily have this many items yet
    
    
    assign { zero_latency_handshake_data_out_rd_trans,
             zero_latency_handshake_data_out_wr_trans,
             master_address,
             master_byteenable,
             master_burstcount,
             master_writedata } = zero_latency_handshake_data_out;
    
    assign master_read = zero_latency_handshake_valid_out & zero_latency_handshake_data_out_rd_trans;
    assign master_write = zero_latency_handshake_valid_out & zero_latency_handshake_data_out_wr_trans;
    
    
    // -----------------------------------------------
    // Response path
    //
    // fast to slow:
    // We'll plonk down a shallow async FIFO because we know
    // this is a fast-slow link, and therefore responses can
    // always be drained faster than the ingress rate. 
    //
    // slow to fast:
    // A scfifo would be added to queue up responses from the faster master
    // port. And a shallow dcfifo would be used to clock cross commands from the
    // master port clock domain to the slave port clock domain
    //
    // -----------------------------------------------
    localparam RSP_FIFO_W                        = DATA_W; 

    // command throttling from expected number of responses exceeding a threshold is delayed by 1 cycle
    localparam THROTTLE_LATENCY                  = 1;

    // MIN_RSP_SCFIFO_DEPTH is the minimum required SCFIFO depth. It must accommodate the data from all outstanding read requests to memory.
    // +DCFIFO_INTERNAL_READ_LATENCY since it takes this many cycles for the cmd_dcfifo to respond to de-assertion of read-req (assertion of throttle = de-assertion of read-req).
    // +THROTTLE_LATENCY for latency through throttle logic.
    // +1 more so throttling logic would not throttle initially. (Allows up to one more read command at max burstcount.)
    localparam MIN_RSP_SCFIFO_DEPTH = (DCFIFO_INTERNAL_READ_LATENCY + THROTTLE_LATENCY + 1) * MAX_BURST_SIZE;

    // TODO: make the minimum response sc fifo depth a requirement in the hw_tcl
    // Adjust response scfifo depth to be at least the minumum response scfifo depth required
    //
    // The hw_tcl will always set RSP_SCFIFO_DEPTHX = RSP_SCFIFO_DEPTH
    localparam RSP_SCFIFO_DEPTHX = (RSP_SCFIFO_DEPTH >= MIN_RSP_SCFIFO_DEPTH) ? 
                                        RSP_SCFIFO_DEPTH : 
                                        MIN_RSP_SCFIFO_DEPTH;

    // Response SC FIFO almost full threshold. This is used as the throttle threshold.
    localparam RSP_SCFIFO_ALMOST_FULL_THRESHOLD  = RSP_SCFIFO_DEPTHX - 
                                                   ((DCFIFO_INTERNAL_READ_LATENCY + THROTTLE_LATENCY) * MAX_BURST_SIZE);  
    
    wire [DATA_W - 1 : 0] rsp_scfifo_data_out;
    wire rsp_scfifo_valid_out;
    wire rsp_scfifo_stall_in;  // equivalent to !rreq or !out_ready
    wire rsp_scfifo_stall_out; // equivalent to !in_ready
    wire rsp_dcfifo_din_ready;   
    wire rsp_dcfifo_din_wreq;
    wire overflow_error = 0; // used to check for response overflow in simulation
    wire overflow_error_comb;

    wire rsp_dcfifo_din_ready_d0;
    reg rsp_dcfifo_din_ready_d1, rsp_dcfifo_din_ready_d2, rsp_dcfifo_din_ready_d3;
    reg rsp_master_reset_0, rsp_master_reset_1, rsp_master_rst; /* synthesis preserve */
    reg rsp_master_rst_d0, rsp_master_rst_d1, rsp_master_rst_d2; // pipeline reset for retiming

    assign overflow_error_comb = !master_reset && (FAST_TO_SLOW ? ~rsp_dcfifo_din_ready & master_readdatavalid: 
                                                    rsp_scfifo_stall_out & master_readdatavalid);

    assign overflow_error = overflow_error_comb;
    
    assert property ( @(posedge master_clk) overflow_error == 0) 
    else begin
        $error("The Response FIFO has overflowed. Please consider sending less commands at a time, or enable error prevention");
    end

    // response side reset configuration
    always@(posedge master_clk) begin
        rsp_master_reset_0 <= slave_rst_d2;
        rsp_master_reset_1 <= rsp_master_reset_0;
        rsp_master_rst     <= rsp_master_reset_1;
        
        rsp_master_rst_d0  <= rsp_master_rst;
        rsp_master_rst_d1  <= rsp_master_rst_d0;
        rsp_master_rst_d2  <= rsp_master_rst_d1;
    end
    
generate
    if(!FAST_TO_SLOW) begin : slow_to_fast_rsp_path
        assign rsp_scfifo_stall_in  = ~rsp_dcfifo_din_ready_d0;
    
        // rsp_dcfifo stalled din_ready
        assign rsp_dcfifo_din_ready_d0 = rsp_dcfifo_din_ready;
        always@(posedge master_clk) begin
            rsp_dcfifo_din_ready_d1 <= rsp_dcfifo_din_ready_d0;
            rsp_dcfifo_din_ready_d2 <= rsp_dcfifo_din_ready_d1;
            rsp_dcfifo_din_ready_d3 <= rsp_dcfifo_din_ready_d2;
        end
        
        hld_fifo #(
            .WIDTH (RSP_FIFO_W),
            .DEPTH (RSP_SCFIFO_DEPTHX),
            .ALMOST_EMPTY_CUTOFF (0),
            .ALMOST_FULL_CUTOFF (0),
            .INITIAL_OCCUPANCY (0),
            .ASYNC_RESET (0),
            .SYNCHRONIZE_RESET (1),
            .RESET_EVERYTHING (0),
            .RESET_EXTERNALLY_HELD (1),
            .STALL_IN_EARLINESS (3),
            .VALID_IN_EARLINESS (0),
            .REGISTERED_DATA_OUT_COUNT (0),
            .NEVER_OVERFLOWS (0),
            .HOLD_DATA_OUT_WHEN_EMPTY (0),
            .WRITE_AND_READ_DURING_FULL (0),
            .USE_STALL_LATENCY_UPSTREAM (0),
            .USE_STALL_LATENCY_DOWNSTREAM (0),
            .STYLE ("hs")
        )
        rsp_scfifo
        (
            .clock          (master_clk),
            .resetn         (~rsp_master_rst_d2),
        
            .i_data         (master_readdata),
            .i_valid        (master_readdatavalid),
            .o_stall        (rsp_scfifo_stall_out),
            .o_almost_full  (),
        
            .o_data         (rsp_scfifo_data_out),
            .o_valid        (rsp_scfifo_valid_out),
            .i_stall        (rsp_scfifo_stall_in),
            .o_almost_empty (),
            .o_empty        ()
        );
    
        assign rsp_dcfifo_din_wreq = rsp_scfifo_valid_out & rsp_dcfifo_din_ready_d3;    //mask with ~rsp_scfifo_stall_in delayed by 3 clocks for stall in earliness
    
        alt_st_mlab_dcfifo 
        #(
            .DATA_WIDTH        (RSP_FIFO_W),
            .PREVENT_UNDERFLOW (1),
            .READY_THRESHOLD   (RSP_DCFIFO_ALMOST_FULL_THRESH),
            .SIM_EMULATE       (1'b0)
        ) rsp_dcfifo (
            .din_clk     (master_clk),
            .din_sclr    (rsp_master_rst_d2),
            .din_wreq    (rsp_dcfifo_din_wreq),
            .din         (rsp_scfifo_data_out),
            .din_ready   (rsp_dcfifo_din_ready), 
    
            .dout_clk    (slave_clk),
            .dout_sclr   (slave_rst_d2),
            .dout_rreq   (1'b1),
            .dout_valid  (slave_readdatavalid),
            .dout        (slave_readdata)
        );
     end

    if(USE_ERROR_PREVENTION && !FAST_TO_SLOW) begin : use_error_protection  

            wire [MAX_RSP_SCFIFO_DEPTH_WIDTH-1 : 0] bridge_incr_val;    
            assign bridge_incr_val = {{PAD_INCR_VAL{1'b0}}, cmd_dcfifo_data_out_burstcount};
 
            bridge_throttle #(
                .THRESHOLD  ( RSP_SCFIFO_ALMOST_FULL_THRESHOLD ),
                .BITS       ( MAX_RSP_SCFIFO_DEPTH_WIDTH)
            ) bridge_throttle_inst (
                .clk         (master_clk),
                .reset       (rsp_master_rst_d2),
        
                .incr        (cmd_dcfifo_data_out_read),            //did a read transaction come out of cmd_dcfifo
                .decr        (rsp_dcfifo_din_wreq),                 //did we receive the response data
                .incr_val    (bridge_incr_val),                     //how many response data for this read transaction will we get
                .waitrequest (zero_latency_handshake_almost_full),  //this what backpressures cmd_dcfifo
                
                .throttle    (cmd_throttle)
            );
    end
    
    if (FAST_TO_SLOW) begin : fast_to_slow_rsp_path
        // Fast to slow:
        alt_st_mlab_dcfifo 
        #(
            .DATA_WIDTH        (RSP_FIFO_W),
            .PREVENT_UNDERFLOW (1),
            .READY_THRESHOLD   (RSP_DCFIFO_ALMOST_FULL_THRESH),
            .SIM_EMULATE       (1'b0)
        ) rsp_dcfifo (
            .din_clk     (master_clk),
            .din_sclr    (rsp_master_rst_d2),
            .din_wreq    (master_readdatavalid),
            .din         (master_readdata),
            .din_ready   (rsp_dcfifo_din_ready),

            .dout_clk    (slave_clk),
            .dout_sclr   (slave_rst_d2),
            .dout_rreq   (1'b1),
            .dout_valid  (slave_readdatavalid),
            .dout        (slave_readdata)
        );
        end
    endgenerate

endmodule
`default_nettype wire