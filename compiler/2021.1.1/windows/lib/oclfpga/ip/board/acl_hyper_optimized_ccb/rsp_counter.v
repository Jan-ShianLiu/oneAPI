`default_nettype none

//out takes 1 cycles
module rsp_counter #(
    parameter BITS = 10
)(
    input wire clk,
    input wire reset,
    
    input wire incr,
    input wire decr,

    input wire [BITS-1:0] incr_val,

    output reg [BITS-1:0] out
);

    reg [BITS-1:0] out_next;

    always @ (*) begin
        out_next = out;

        if(incr && decr) begin
            out_next = out + incr_val - 1'b1;
        end
        else if(incr) begin
            out_next = out + incr_val;
        end
        else if(decr) begin
            out_next = out - 1'b1;
        end
    end

    always @ (posedge clk) begin
        if(reset) begin
            out      <= 0;
        end
        else begin
            out      <= out_next;
        end
    end

endmodule
