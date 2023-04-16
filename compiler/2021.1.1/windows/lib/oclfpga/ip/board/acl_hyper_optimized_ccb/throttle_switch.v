`default_nettype none

module throttle_switch #(
    parameter THRESHOLD   = 512, 
              BITS          = 10
)(
    input   wire            waitrequest,
    input   wire [BITS-1:0] count,

    output  reg             throttle
);

    always@ (*) begin
        if(waitrequest || (count > THRESHOLD) ) begin
            throttle = 1;
        end
        else begin
            throttle = 0;
        end
    end

endmodule
