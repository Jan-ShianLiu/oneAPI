`default_nettype none

module bridge_throttle 
#(
    parameter   THRESHOLD = 512,
                BITS      = 10
)(
    input wire            clk,
    input wire            reset,
    
    input wire            incr,
    input wire            decr,
    input wire [BITS-1:0] incr_val,

    input wire            waitrequest,

    output wire           throttle
);

    wire [BITS-1:0] count;

    rsp_counter 
    #(  
        .BITS       (BITS)
     ) 
    rsp_counter_inst 
    (
        .clk      (clk),
        .reset    (reset),
        .incr     (incr),
        .decr     (decr),
        .incr_val (incr_val),
        
        .out      (count)
    );

    throttle_switch 
    #(
        .THRESHOLD  (THRESHOLD),
        .BITS       (BITS)
     )
    throttle_switch_inst
    (
        .waitrequest (waitrequest),
        .count       (count),

        .throttle    (throttle)
    );

endmodule


