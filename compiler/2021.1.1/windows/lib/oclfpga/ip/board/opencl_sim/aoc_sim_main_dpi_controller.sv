module aoc_sim_main_dpi_controller
#(
    parameter CMAIN_START_CYCLE                 = 10
    )
    (
        output  logic   trigger_reset,

        input           clock,
        input           resetn,
        input           clock2x,
        input           kernel_interrupt
    );

    export "DPI-C" task __ihc_hls_reset_sim_task;
    import "DPI-C" context function void __ihc_aoc_set_clock_period_ps(int clock_period_ps);
    import "DPI-C" context function void __ihc_aoc_set_sim_time_ps(longint unsigned sim_time_ps);
    import "DPI-C" context function void __ihc_aoc_interrupt();
    import "DPI-C" context function void __ihc_hls_dbgs(string msg);
    import "DPI-C" context function int __ihc_kernel_delay_cycles();

    time clk_period_ps;

    initial
    begin
        automatic time clk_period_start_time_ps, clk_period_end_time_ps;
        automatic string message;

        assert (CMAIN_START_CYCLE>=2) else $fatal(1, "[sim] Minimum allowed value for CMAIN_START_CYCLE is 2");

        $sformat(message, "[%7t][msim][main_dpi_ctrl] sim start", $time);
        __ihc_hls_dbgs(message);

        @(negedge resetn)
        $sformat(message, "[%7t][msim][main_dpi_ctrl] reset asserted", $time);
        __ihc_hls_dbgs(message);
        @(posedge resetn)
        $sformat(message, "[%7t][msim][main_dpi_ctrl] reset deasserted", $time);
        __ihc_hls_dbgs(message);


        @(posedge clock);
        clk_period_start_time_ps    = $time;
        @(posedge clock);
        clk_period_end_time_ps      = $time;
        clk_period_ps = clk_period_end_time_ps - clk_period_start_time_ps;
        repeat(CMAIN_START_CYCLE-2) @(posedge clock);
        __ihc_aoc_set_clock_period_ps(clk_period_ps);
        $sformat(message, "[%7t][msim][main_dpi_ctrl] Finished clock main start cycle", $time);
        __ihc_hls_dbgs(message);
    end

    initial begin
        trigger_reset = 0;
    end

    task automatic __ihc_hls_reset_sim_task();
        @(posedge clock);
        trigger_reset = 1;
        @(posedge clock);
        trigger_reset = 0;
        @(posedge clock);
    endtask

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Handle OpenCL kernel interrupts
    ////////////////////////////////////////////////////////////////////////////////////////////

    task automatic interrupt_handler;
      automatic string message;
      automatic int delay_cycles = __ihc_kernel_delay_cycles();
      while(1) begin
        @(posedge clock);
        if (kernel_interrupt) begin
          // Set a total time, we do not account for wait cycles that's artifact of simulator
          __ihc_aoc_set_sim_time_ps($time);
          // We can have a problem that some writes aren't done before the
          // kernel interrupts.  Wait a (relatively) long time before
          // signalling the interrupt.
          repeat(delay_cycles) @(posedge clock);
          $sformat(message, "[%7t][msim][main_dpi_ctrl] Raise kernel interrupt", $time);
          __ihc_hls_dbgs(message);
          __ihc_aoc_interrupt();
        end
      end
    endtask
    
    initial begin
      fork
        interrupt_handler;
      join_none
    end

endmodule

// vim:set filetype=verilog:

