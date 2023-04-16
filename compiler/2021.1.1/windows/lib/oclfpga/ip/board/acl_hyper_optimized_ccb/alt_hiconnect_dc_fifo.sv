//----------------------------------------------------------------------
// Description: Dual clocked FIFO 
//
// In showahead mode the read is an acknowledge signal. In non-showahead
// mode the read is a request, and the read to data latency is 3 cycles.
//            __
// read   ___|  |____________
//                    __
// data   ___________|  |____
//
// For proper resets, out_reset must be asserted for at least two clock 
// cycles of the longest clock period.
// ---------------------------------------------------------------------

`timescale 1 ns / 100 ps

module alt_hiconnect_dc_fifo (
    
    in_clk,

    out_clk,
    out_reset,

    // sink
    in_data,
    in_valid,
    in_ready,
    in_startofpacket,
    in_endofpacket,
    in_channel,

    // source
    out_data,
    out_valid,
    out_ready,
    out_startofpacket,
    out_endofpacket,
    out_channel

);

    // ---------------------------------------------------------------------
    // Parameters
    // ---------------------------------------------------------------------
    parameter DATA_WIDTH        = 32;
    parameter FIFO_DEPTH        = 16;
    parameter CHANNEL_WIDTH     = 0;
    parameter USE_PACKETS       = 0;
    parameter PREVENT_UNDERFLOW = 0;
    parameter SHOWAHEAD         = 0;

    localparam DEPTH = FIFO_DEPTH;
    localparam PACKET_SIGNALS_WIDTH = 2;
    localparam PAYLOAD_WIDTH = (USE_PACKETS == 1) ?
                                  2 + DATA_WIDTH + CHANNEL_WIDTH :
                                  DATA_WIDTH + CHANNEL_WIDTH;

    // ---------------------------------------------------------------------
    // Input/Output Signals
    // ---------------------------------------------------------------------
    input in_clk;

    input out_clk;
    input out_reset;

    input [DATA_WIDTH - 1 : 0] in_data;
    input in_valid;
    input in_startofpacket;
    input in_endofpacket;
    input [((CHANNEL_WIDTH > 0) ? CHANNEL_WIDTH - 1 : 0) : 0] in_channel;
    output in_ready;

    output [DATA_WIDTH - 1 : 0] out_data;
    output reg out_valid;
    output out_startofpacket;
    output out_endofpacket;
    output [((CHANNEL_WIDTH > 0) ? CHANNEL_WIDTH - 1 : 0) : 0] out_channel;
    input out_ready;

    wire [PACKET_SIGNALS_WIDTH - 1 : 0] in_packet_signals;
    wire [PACKET_SIGNALS_WIDTH - 1 : 0] out_packet_signals;

    wire [PAYLOAD_WIDTH - 1 : 0] in_payload;
    wire [PAYLOAD_WIDTH - 1 : 0] out_payload;

    wire in_reset;

    // --------------------------------------------------
    // Define Payload
    //
    // Icky part where we decide which signals form the
    // payload to the FIFO.
    // --------------------------------------------------
    assign in_packet_signals = {in_startofpacket, in_endofpacket};
    assign {out_startofpacket, out_endofpacket} = out_packet_signals;

    generate
        if (USE_PACKETS) begin
            if (CHANNEL_WIDTH > 0) begin : pkt_ch
                assign in_payload = {in_packet_signals, in_data, in_channel};
                assign {out_packet_signals, out_data, out_channel} = out_payload;
            end
            else begin : pkt_no_ch
                assign in_payload = {in_packet_signals, in_data};
                assign {out_packet_signals, out_data} = out_payload;
            end
        end
        else begin
            if (CHANNEL_WIDTH > 0) begin : ch
                assign in_payload = {in_data, in_channel};
                assign {out_data, out_channel} = out_payload;
            end
            else begin : no_ch
                assign in_payload = in_data;
                assign out_data = out_payload;
            end
            assign out_packet_signals = 2'b0;
        end
    endgenerate

    // --------------------------------------------------
    // Resets
    //
    // Derive in_reset from out_reset. The idea is to ensure
    // the read side is always reset first.
    //--------------------------------------------------
    
    reg out_resetx_1;
    reg out_resetx_2;

    reg in_resetx_1 /* synthesis preserve */;
    reg in_resetx_2 /* synthesis preserve */;
    reg in_resetx_3 /* synthesis preserve */;

    always @(posedge out_clk) begin
        out_resetx_1 <= out_reset;
        out_resetx_2 <= out_resetx_1;
    end

    always @(posedge in_clk) begin
        in_resetx_1 <= out_resetx_2;
        in_resetx_2 <= in_resetx_1;
        in_resetx_3 <= in_resetx_2;
    end

    assign in_reset = in_resetx_3;

    generate if (SHOWAHEAD) begin : ack

        alt_st_mlab_dcfifo_ack
        #(
            .DATA_WIDTH        (PAYLOAD_WIDTH),
            .READY_THRESHOLD   (18),
            .SIM_EMULATE       (1'b0)
        )
        mlab_fifo
        (
            .din_clk    (in_clk),
            .din_sclr   (in_reset),
            .din_wreq   (in_valid),
            .din        (in_payload),
            .din_ready  (in_ready),

            .dout_clk   (out_clk),
            .dout_sclr  (out_reset),
            .dout_rreq  (out_ready),
            .dout_valid (out_valid),
            .dout       (out_payload)
        );

    end else begin : req

        alt_st_mlab_dcfifo 
        #(
            .DATA_WIDTH        (PAYLOAD_WIDTH),
            .PREVENT_UNDERFLOW (PREVENT_UNDERFLOW),
            .READY_THRESHOLD   (23),
            .SIM_EMULATE       (1'b0)
        )
        mlab_fifo 
        (
            .din_clk    (in_clk),
            .din_sclr   (in_reset),
            .din_wreq   (in_valid),
            .din        (in_payload),
            .din_ready  (in_ready),

            .dout_clk   (out_clk),
            .dout_sclr  (out_reset),
            .dout_rreq  (out_ready),
            .dout_valid (out_valid),
            .dout       (out_payload)
        );

    end endgenerate

endmodule
