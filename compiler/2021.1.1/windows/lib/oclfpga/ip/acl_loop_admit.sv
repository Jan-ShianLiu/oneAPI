// (c) 1992-2020 Intel Corporation.                            
// Intel, the Intel logo, Intel, MegaCore, NIOS II, Quartus and TalkBack words    
// and logos are trademarks of Intel Corporation or its subsidiaries in the U.S.  
// and/or other countries. Other marks and brands may be claimed as the property  
// of others. See Trademarks on intel.com for full list of Intel trademarks or    
// the Trademarks & Brands Names Database (if Intel) or See www.Intel.com/legal (if Altera) 
// Your use of Intel Corporation's design tools, logic functions and other        
// software and tools, and its AMPP partner logic functions, and any output       
// files any of the foregoing (including device programming or simulation         
// files), and any associated documentation or information are expressly subject  
// to the terms and conditions of the Altera Program License Subscription         
// Agreement, Intel MegaCore Function License Agreement, or other applicable      
// license agreement, including, without limitation, that your use is for the     
// sole purpose of programming logic devices manufactured by Intel and sold by    
// Intel or its authorized distributors.  Please refer to the applicable          
// agreement for further details.                                                 


/*
  *** Behavioural Description ***
  This module is used in loop orchestration. Its function is to admit an upstream thread into a downstream loop and subsequently generate loop iterations (with the specified II) until the loop exits, 
  at which time the process repeats. It supports separate latencies on the data (the stall-valid round trip), empty, and exit.
  This module supports both zero and non-zero latencies on its stall-valid interfaces (ie. both the stall-valid and stall-latency protocols)

  A bit of background. In a non-interleaving scenario (interleaving is explained below), the push-pop backedge (in the kernel datapath created by the compiler) is populated with DD pieces of garbage data (where DD = dependence distance of that push-pop). When a new thread is admitted (AKA 'forked'), the
  garbage is popped and discarded. The purpose of this scheme is to somewhat simplify the backedge read logic by allowing the backedge FIFO to always be read when the pop receives a valid from upstream rather than conditioning
  the read based on the fork signal. The fork signal controls a mux that selects either the new initial data from upstream or the backedge data. When forking, we select the former, meaning the backedge garbage data is discarded.

  acl_loop_admit supports interleaving, meaning that multiple threads can be admitted to the loop simultaneously (up to ii threads). The concept of 'dummy' threads is used to simplify the handling of initial garbage backedge data when multiple
  threads are being admitted. The arrival time of threads from upstream can be variable at runtime. This means that we can have one admitted thread (T0) that has made some progress (ie. iterated a bunch of times) when
  a second thread (T1) appears from upstream. At that time, the push-pop backedges will be populated with DD pieces of real data from T0 and we must ensure that T1 does not pop or clobber T0's data. The way this is handled is by fully populating
  the loop with ii threads at all times and introducing a signal to distinguish between real and not-real (dummy) threads. After reset, the backedges are populated with ii*DD pieces of garbage data. acl_loop_admit begins issuing valids as though
  the loop was fully populated with ii-threads. When those valids reach the push-pops, they push and pop as normal. When those valids reach the exit node, the exit condition is computed as normal. However, when those valids reach the latch block of 
  the loop (ie. the final basic-block in the loop pipeline), 
  the dummy signal is consumed to determine if the thread (and therefore the value of the 'exit' and 'exited' signals) is real. Only a valid from the final iteration of a *real* thread is allowed to leave the latch block.
  Furthermore, the dummy signal is used to predicate operations in the loop that have side-effects.

  We can think of the interleaved threads in the loop as occupying a specific slot number [ii-1:0]. The slots are constantly circulating, as observed at acl_loop_admit's kernel downstream port.
  When a real thread is to be admitted, acl_loop_admit inserts it into the next vacant slot that it sees and it displaces the dummy thread currently in that slot.
  This means that threads may not be admitted in slot-order (as in, slot-0, followed by slot-1, followed by slot-2). They can, and likely will, be admitted out-of-slot-order. However, if the thread tripcounts are all the same, the threads will exit
  in the admit-order.
  An admitted real thread will displace that slot's corresponding garbage data in the backedge FIFOs. In this scheme, we don't
  have to worry about a new thread consuming/clobbering the data from some other thread because the spacing between threads, in the backedge FIFOs, is always maintained at a constant amount by the circulating garbage data from dummy threads.

  When the exit condition asserts, the corresponding real thread is displaced by a dummy thread.
  acl_loop_admit requires that i_exit_valid asserts for all dummy threads. It internally tracks which slots contain real vs. dummy threads and therefore knows when to pay attention to i_exit_valid. But this depends on i_exit_valid
  asserting for all slots.

  Since there may be some latency between the final true iteration being launched and the assertion of exit, acl_loop_admit will generate iterations that are marked as belonging to a real (non-dummy) thread, during this period. 
  This is what we call 'speculative iterations'. 
  To be clear, these iterations will push/pop backedge data like any other iteration, but even though the thread is marked as real (ie. dummy=0), it's actually/technically dummy (because the thread exited). All of this is fine because the loop 
  orchestration logic knows to ignore 
  iterations that occur after exit asserts. acl_loop_admit will eventually mark this thread as dummy. A subsequent real thread admitted to the same slot will cause the garbage data, that was generated during the previous speculation period in that slot, 
  to be popped and discarded.

  The latency from i_stall to o_valid (ie. the internal stall-latency) is variable, but bounded. (This refers to the latency from when i_stall toggles to when o_valid correspondingly toggles). When iterating on admitted threads, the latency is one value.
  But when we are admitting a new real thread, the latency is a different value, because we have to go upstream to fetch the thread. This upstream latency + acl_loop_admit's internal control logic latency
  make the observed stall-latency vary during execution. The overall internal stall-latency of acl_loop_admit should be considered to be the larger of the two latencies. See the latency calculations at the end of the file.

  It should be noted that this module, on its own, is not considered to be a node in a stall-valid network, which is why it does not have a complete stall-latency interface (for example it is
  missing o_empty/o_almost_empty ports).
  Rather it combines with other pieces to form a node that, overall, conforms to the S/V protocol and interface.
  For more details on this and to understand this module's role in overall loop orchestration, please see the corresponding FD: //depot/docs/hld/compiler/S10_Loop_Orchestration_FD.docx

    acl_loop_admit has a parameterizable feature called FAST_ADMIT. This minimizes the latency of admitting a new thread. Fast-admit makes it possible to admit threads on back-to-back cycles. However much of the pipelining in the control
  logic has been removed in order to lower this latency and therefore the deeper combinational logic could cause an FMAX penalty. A typical scenario where fast-admit is useful is in nested loops where the inner loop has a small trip-count. 
  With a large trip-count, a longer admit latency is tolerable because the admit latency is small relative to the runtime of the loop, so the overall throughput penalty is small. But for small trip-counts the opposite can be true. 
  The design may experience an overall performance gain by enabling fast-admit, despite the potential FMAX loss. The admit latency of course depends on LATENCY_UPSTREAM_EMPTY because we must wait at least this many cycles
  between admits. Its minimum value is 1, which is the value required if back-to-back admits are desired. With FAST_ADMIT=0, even if LATENCY_UPSTREAM_EMPTY=1, the admit latency can be approximately ii cycles (acl_loop_admit waits for the next
  vacant slot to come around, which can take up to ii cycles).

  How to parameterize acl_loop_admit for the desired stall-valid/stall-latency configurations on the upstream and downstream ports:

  Upstream      Downstream      Required Params                                       Notes
  ------------------------------------------------------------------------------------------
  S/V           S/V             FAST_ADMIT=1                                          
                                LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP = 0           S/V on upstream requires zero upstream latency.

  S/V           S-L             FAST_ADMIT=0
                                LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP = 0           S/V on upstream requires zero upstream latency.
  
  S-L           S/V             Not supported.                                        acl_loop_admit does not have capacity. Therefore if downstream releases backpressure, it must guarantee it can receive the data from upstream N cycles later. This behaviour contradicts the S/V protocol.
  
  S-L           S-L             FAST_ADMIT=0
                                LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP > 0


  How to parameterizate for various internal target latencies:

  L_a = Latency from i_stall to o_valid when admitting a new thread. Can be 0 or >0.
  L_f = Latency from i_exit to o_valid. Can be 0, 1, or >1.

  Latency        Desired Value  Required Params                                       Notes
  ------------------------------------------------------------------------------------------
  L_a             0             FAST_ADMIT = 1  
                                LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP = 0

  L_a             >0            FAST_ADMIT = 0                                        Technically could set FAST_ADMIT = 1, but if you're already able to wait for upstream latency, might as well wait a bit longer for acl_loop_admit's internal admit latency.
                                LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP > 0
  
  L_f             0             FAST_ADMIT = 1                                        Internally implemented as 0 cycles from exit to admit decision, 0 cycles from admit decision to o_valid.
                                FAST_EXIT = 1

  L_f             1             FAST_ADMIT = 1                                        Internally implemented as 1 cycle from exit to admit decision, 0 cycles from admit decision to o_valid.
                                FAST_EXIT = 0
  
  L_f             >1            FAST_ADMIT = 0                                        Internally implemented as multiple cycles from exit to admit decision, multiple cycles from admit decision to o_valid.
                                FAST_EXIT = 0

  Typical use cases
  - Desired behaviour = Absolute minimum latency between admitted threads (ie. admit threads on back-to-back cycles). Useful for inner loops with low trip-counts.
      - L_a = 0
      - L_f = 0 if latency=1 on the kernel datapath from o_valid to i_exit_valid
      - L_f = 1 if latency=0 on the kernel datapath from o_valid to i_exit_valid
  - Desired behaviour = Some latency is OK between admitted threads (loops have long trip counts)
      - L_a > 0
      - L_f > 0

  *** Implementation Details ***
    
    The real vs. dummy status of the ii threads are tracked in a thread table, which is simply a bit-vector of length ii.

    * Output Valid Generation *
    The o_valids for a given thread must be spaced by at least ii and o_dummy must assert according to the real vs. dummy status of the thread. This is implemented by reading the thread statuses from the thread table in a cyclic fashion using 
    a read pointer that increments over all the slot positions. When i_stall asserts, after some latency, the read-pointer stalls and output valids are gated. Since we always iterate over the entire table, and the table length is ii, 
    it's guaranteed that valids for a given slot are spaced by at least ii. This is how we implement the requirement that valids for a given thread are spaced by at least ii and that valids between loop invocations in a given slot are spaced by at least ii.
    The "table read" is simply a mux on the table output whose select signal is the read pointer. As a potential future enhancement, this logic can be pipelined arbitrarily (and acl_loop_admit's stall-latency will 
    increase accordingly), which can be useful for large ii (since the LUT depth of the mux will grow).

    * Thread Table Update *
    When a real thread is admitted or exits, the thread table must be updated. Both admits and exits can occur in any slot on a given cycle.
    The next vacant slot is found by using an "ahead" pointer, which stays ahead of the main table read pointer by a fixed amount. This pointer is used to look ahead to find the next
    vacant slot so that the admit process can begin early and synchronize exactly to when the target slot actually comes around.
    As a potential future enhancement, this table update logic can be pipelined arbitrarily which can be useful for large ii since the LUT depth of the slot number comparison can increase.

    We decide to admit when downstream can accept data (ie. not back pressuring), upstream has a thread available, and we've found a vacant slot in the table.
    We decide to exit when exit asserts.

    When admitting a thread, it is critical to synchronize the output valid and forked signals with the arrival of data from upstream. In order to facilitate this, the implementations of the output valid generation and thread table update have fixed latencies, that
    are the same for all slot positions, and are known at compile time. By knowing the fixed latencies from the admit decision to the output valid+fork assertion as well as the upstream data latency, we can synchronize both by elongating the latency of the shorter
    path to match the longer path. In other words, if the upstream data latency is longer, the output valid is delayed accordingly. Or if the table update latency is longer, the read from upstream (o_stall) is delayed accordingly.
  
    Furthermore, upstream_stall-to-empty latency must elapse before we pay attention to i_empty from upstream. Each time we de-assert o_stall (ie. read from upstream), we must allow this time for i_empty to update.
  
    The decision to exit is a bit tricky because there can be multiple exit conditions from multiple real threads and there is a fixed window of time in which it is valid to observe the exit (it must be ignored otherwise).
    Once exit asserts on a real thread, acl_loop_admit must ignore subsequent assertions of exit. Exit can continue to falsely assert during speculation because the data circulating during speculation is indeterminate and could
    theoretically take on values that trigger exit. Secondly, acl_loop_admit must ignore assertions of exit for some time after asserting fork. This is to allow time for the speculative
    iterations from the previous thread in that slot to flush out (because they could, again, falsely trigger exit during the flush process).
    To simplify the determination of the start of the window, the i_exit_forked signal is used, which is simply the o_forked signal pipelined through the exit node in the kernel datapath and looped back to this module. i_exit_forked asserts on 
    the first iteration, so this marks the start of the window. The window ends after exit asserts for the *first* time.
    When interleaving, acl_loop_admit must track the valid observability window for multiple real threads that are admitted (and therefore will exit) at different times.
    The valid observability window of each thread is tracked in an exit table, which is simply a bit-vector of length ii. A value of 1 means we are in the window and it's safe to observe exit. 

    It's critical that i_exit_valid asserts for every thread (dummy or real). Said another way, i_exit_valid must assert for every slot. The compiler must not mask i_exit_valid (using o_dummy or something else). The reason is that
    acl_loop_admit correlates i_exit to slot positions using i_exit_valid. It expects to see (ii-1) i_exit_valids between exits for a given slot. Said another way, it simply uses i_exit_valid to count the slot numbers.
    Furthermore, it uses i_exit_dummy to know if a given exit condition is for a real or dummy thread. Technically acl_loop_admit could do a thread-table lookup to determine if a given slot contains a real or dummy thread
    but we can avoid this if the compiler feeds us back the dummy signal, synchronized to the exit. This is helpful because the exit-table update is very latency sensitive (see the explanation above) so skipping
    the thread-table lookup helps to meet the exit-table latency requirements.

    Non-interleaving is implemented by simply limiting the number of admitted threads to 1 and freezing the admit and exit pointers to slot-0.
*/

`timescale 1ps/1ps
`default_nettype none

module acl_loop_admit #(
    parameter int II = 1,                                       // Initiation-interval of the loop
    parameter int ENABLE_INTERLEAVING = 0,                      // 1 = Allow up to II threads to be interleaved. 0 = Allow only one thread to be admitted at a time.
    parameter int LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP = 1,  // Latency from de-assertion of o_stall to valid data arriving to downstream cluster body. Must be >= 0. A value of 0 means the kernel-upstream interface uses stall-valid instead of stall-latency.
    parameter int LATENCY_UPSTREAM_EMPTY = 1,                   // Latency from o_stall to i_empty. Must be >= 1.
    parameter int EXIT_USES_STALL_VALID = 0,                    // Indicates that the downstream interface of the exit node uses stall-valid and therefore we must only consume i_exit* when i_exit_stall=0. This is useful if an orchestration is *not* used and therefore the exit node is implemented outside of the stall-free cluster that contains acl_loop_admit.
    parameter int FAST_ADMIT = 0,                               // Minimize the latency of admitting a new thread. This implements several paths with combinational logic and can result in an FMAX penalty. See above for comments about when to use this feature.
    parameter int FAST_EXIT = 0,                                // If FAST_EXIT=1, latency from o_valid to i_exit_valid *must* be >=1. Similarly implements some paths with combinational logic and can result in an FMAX penalty.
    parameter int ASYNC_RESET = 1,                              // 1 = Registers are reset asynchronously. 0 = Registers are reset synchronously -- the reset signal is pipelined before consumption. In both cases, some registers are not reset at all.
    parameter int SYNCHRONIZE_RESET = 0,                        // 1 = resetn is synchronized before consumption. The consumption itself is either asynchronous or synchronous, as specified by ASYNC_RESET.
    parameter int LATENCY_INTERNAL_STALL_TO_VALID_FROM_COMPILER = 0 // The stall-latency calculation for this module is somewhat complex. The compiler must implement the formulas at the end of this file. This parameter is used as a sanity check that the latency calculated by the compiler matches the latency used by this IP.
)
(
    input  wire     clock,
    input  wire     resetn,

    /*  
      Stall-valid interface to upstream. 
      Note: this module does not generate o_valid by passing the valid from upstream through. Instead it generates o_valid, synchronized
      to the arrival of data to downstream by looking ahead to when that data will arrive. This is so o_valid can be registered when LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP > 0.
      This module uses the following to determine when data will arrive downstream:
      LATENCY_UPSTREAM_EMPTY specifies when i_empty will be updated and is safe to consume, i_empty indicates if upstream has data, and LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP specifies when
      valid data will arrive at the downstream node.
    */
    output  logic   o_stall,
    input   wire    i_empty,
    // i_almost_empty is consumed only in stall-latency (i.e. when LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP > 0). 
    // 0 = upstream has multiple threads to admit and it's safe to set o_stall=0 for multiple cycles in a row. 
    // 1 = upstream's occupancy is unknown (i.e. don't know how many threads it has), therefore we must observe i_empty, taking into account that it's stale for LATENCY_UPSTREAM_EMPTY cycles after o_stall=0.
    input   wire    i_almost_empty, 

    // Kernel downstream interface (can behave as stall-valid or stall-latency)
    output  logic   o_valid,
    input   wire    i_stall,

    // Loop-specific interface signals with downstream
    output  logic   o_forked,       // Asserts on the first o_valid of an admitted thread (ie. asserts on the first iteration of a loop invocation)
    output  logic   o_dummy,        // Indicates if the current iteration (ie. when o_valid=1) corresponds to a real or dummy thread. 1=dummy, 0=real.
    input   wire    i_exit_valid,   // Validates i_exit. This should be a delayed version of o_valid that is looped back through the exit node.
    input   wire    i_exit,         // Exit condition from the exit node.
    input   wire    i_exit_dummy,   // Indicates if i_exit_valid corresponds to a real or dummy thread. This should be a delayed version of o_dummy, looped back through the exit node, synchronized with i_exit_valid.
    input   wire    i_exit_forked,  // Indicates if i_exit_valid is the first valid in a loop. This should be a delayed version of o_forked, looped back through the exit node, synchronized with i_exit_valid.
    input   wire    i_exit_stall    // Ignored if EXIT_USES_STALL_VALID=0. Otherwise i_exit* are only consumed when i_exit_stall=0. This input must be fed with a copy of the the exit node downstream interface's i_stall signal that is synchronized with i_exit_valid.
);

  function integer my_max;
    input integer a;
    input integer b;
    begin
      my_max = (a > b) ? a : b;
    end
  endfunction  

  localparam TABLE_PTR_WIDTH      = ($clog2(II) == 0)? 1 : $clog2(II); // II must be > 0.

  /*  Latency calculations for upstream data synchronization. This is somewhat complex and may require reading the following comments a few times.

      When admitting a thread, the o_valid, o_forked, o_dummy signals (let's call these the "output" signals in this context) must assert synchronous with the arrival of upstream data (the upstream latency is LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP)
      Furthermore we must ensure that the thread from upstream is inserted into the correct slot. Therefore careful synchronization is required.
      Recall that o_dummy asserts by reading from the thread-table (the thread-table indicates if a given slot contains a real or dummy thread). In non-interleaving, o_valid also asserts by reading from the thread-table.
      Therefore in order to assert the output signals when admitting a new thread, the thread-table must first be updated, then it is read, then the signals assert. All of these latencies must be accounted for in order to correctly synchronize
      the output signals with the data arrival from upstream.
      Currently the table update latency is 1, and the table read latency is 1. As a future enhancement, it's possible to elongate both if more pipelining is needed for FMAX.
      If the overall latency to the output signals asserting is less than the upstream data latency, the output signals must be delayed.
      The admit decision (and therefore commencement of the table update process) is made exactly TT_UPDATE_LATENCY cycles before the target slot comes around. When the target slot is read from the thread-table, it would have been updated
      just in time and therefore the output signals will be generated correctly.
      
      In fast-admit mode, the table-update latency is forced to be 1 cycle. This cannot be elongated (this is partly why it's called "fast" admit). For example, if ii=1 and tripcount=1
      the exit condition can assert coincident with the very first o_valid. In fast-admit we want the ability to admit a new thread on the very next cycle, therefore the table must update in 1 cycle in the worst case.

      Ultimately all of the formulas below are for calculating the latency from the admit decision to the output signals asserting, so that we can decide if this latency should be elongated or not
      to match the upstream data latency. The admit decision (the 'admit_thread' signal) is considered to occur in cycle-0 for the purpose of these calculations.
  */
  localparam TT_UPDATE_LATENCY = FAST_ADMIT? 1 : 1; // Latency from admit_thread/thread_is_exiting to thread_table being updated. Yes this statement is redundant and its value is always 1 -- just coding it this way as a reminder that we can pipeline this path when FAST_ADMIT=0 (and would need to update this localparam accordingly).

  // In FAST_ADMIT mode, we do not wait for the thread table to be updated. The output signals assert on the same cycle as the admit decision.
  localparam TT_LATENCY_ADMIT_TO_FIRST_READ = FAST_ADMIT? 0 : TT_UPDATE_LATENCY;
  localparam TT_READ_LATENCY = FAST_ADMIT? 0 : 1; // Latency from thread_table to out_dummy (ie. latency of the output mux). In fast-admit, the thread-table combinationally feeds the output signals, therefore the latency is 0.

  // When we make the admit decision, it takes TT_UPDATE_LATENCY cycles to update the thread-table, then another TT_READ_LATENCY cycles to read from the table and generate the output signals.

  localparam LATENCY_ADMIT_DECISION_TO_OUT_STALL = FAST_ADMIT? 0 : 1; // Latency from admit_thread to out_stall.
  // In fast-admit, the overall latency from admit-decision to output signals is 0. Therefore the output latency elongate amount is completely determined by the upstream data latency (which could itself be 0);
  // The latency from admit-decision to data arriving from upstream is nominally (LATENCY_ADMIT_DECISION_TO_OUT_STALL + LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP).
  // The latency from admit-decision to o_valid is nominally (TT_LATENCY_ADMIT_TO_FIRST_READ + TT_READ_LATENCY)
  // The difference between the two is how much the shorter path must be elongated.
  // Negative number means the o_stall path is elongated, positive number means the o_valid path is elongated.
  localparam OUTPUT_LATENCY_ELONGATE_AMOUNT = (LATENCY_ADMIT_DECISION_TO_OUT_STALL + LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP) - (TT_LATENCY_ADMIT_TO_FIRST_READ + TT_READ_LATENCY);

  // If interleaving is disabled, set the max # of threads to admit to 1
  localparam MAX_THREADS = ENABLE_INTERLEAVING? II : 1;

  logic [II-1:0] thread_table;            // Table index corresponds to thread slot #. Value of 1 = dummy, 0 = real. Basically match the polarity of o_dummy.
  // Pointers for accessing the thread table (tt)
  logic [TABLE_PTR_WIDTH-1:0] tt_ptr_read, tt_ptr_exit, tt_ptr_admit_ahead;

  localparam NUM_TT_EMPTY_PIPE_STAGES = 3;
  logic [NUM_TT_EMPTY_PIPE_STAGES:1] tt_empty;
  logic kill_valid;

  logic [II-1:0] exit_table;                // Value of 1 = we are in the valid observability window and it is safe to consume i_exit/i_exit_valid. 0 = outside the window and must ignore exit.
  // Pointers for accessing the exit table ("et")
  logic [TABLE_PTR_WIDTH-1:0] et_ptr_read;  // Iterates over all slots. For each real exit condition, we read from this location to determine if we're in the window or not. 
  
  logic thread_is_exiting;  // Indicates that a real thread is exiting. This flag asserts for 1 cycle. It's generated by the exit-table logic (which determines when a thread exits) and is consumed by the thread-table logic (which must remove the exiting thread from the thread-table).

  // Outputs from the respective latency counters
  logic latency_elapsed_upstream_empty;
  logic latency_elapsed_upstream_data;

  // Asserts when we decide to admit a thread. This signal is the "admit decision", which is a phrased used throughput the comments in this module.. 
  logic admit_thread;
  logic admit_thread_no_downstream_stall; // Same as admit_thread, but does not depend on i_stall. Needed for stall/valid handshaking. Explanation provided below.
  logic safe_to_read_from_upstream; // Flag to indicate that it's guaranteed that upstream has threads and it is therefore safe to read from upstream.

  // Output signals *before* any latency matching.
  logic out_valid, out_forked, out_dummy, out_stall;

  localparam                    NUM_RESET_COPIES = 1;
  localparam                    RESET_PIPE_DEPTH = 3;
  logic                         aclrn;
  logic [NUM_RESET_COPIES-1:0]  sclrn;
  logic                         resetn_synchronized;

  /*
    If SYNCHRONIZE_RESET==1, the synchronized version of resetn is distributed down the hierarchy to submodules, and submodules will not synchronize again.
    Currently resetn_synchronized is not consumed (there are no submodules) but leaving it here as a reminder that it should be used in the future if submodules are added.
  */
  acl_reset_handler
  #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
      .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES) 
  )
  acl_reset_handler_inst
  (
      .clk                    (clock),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn),
      .o_resetn_synchronized  (resetn_synchronized),  
      .o_sclrn                (sclrn)
  );

  // Output Valid Generation (ie. thread table read).
  // In fast-admit, generate out_valid combinationally. Otherwise register it.
  // Generate valids whenever there is no backpressure
  // But if we're reading from upstream and the upstream data latency has not yet elapsed, ignore the stall and continue to generate valids. This implies that stall-latency is being used.  
  generate
    if (FAST_ADMIT) begin // fast-admit is only used in stall-valid mode.

      /*  Intended behaviour is that we assert valid whenever we have a valid to send, irrespective of i_stall. But when i_stall=0, this constitutes transmission of the valid.
          But nobody in this block consumes out-valid therefore nothing relies on knowing when a valid is actually transmitted.
          Waiting for upstream latency is not relevant here because there must be zero upstream latency when using stall-valid.
          When interleaving, we generally continually output valids to force activity on all slots, even if the slots are not occupied. As a side note, this block does not know if the valids are predicated since the predicate
          signal is generated outside the IP. However, when the loop is completely empty, we de-assert valid (using kill_valid) to handle the case where overall kernel execution has completed but the incessant 
          generation of predicated valids can cause a downstream LSU to signal that it's still active, which prevents the kernel-finish signal from asserting, causing a hang.
      */
      always_comb begin
        out_dummy = !(thread_table[tt_ptr_read] || admit_thread);
        out_valid = ENABLE_INTERLEAVING? !kill_valid : (thread_table[tt_ptr_read] || admit_thread_no_downstream_stall); 
      end

    end else begin

      always_ff @(posedge clock or negedge aclrn) begin
        if (!aclrn) begin
          out_dummy                       <= 1'b0;
          out_valid                       <= 1'b0;
        end else begin
          // If we introduce more than 1 cycle from i_stall to out_valid, need to update INTERNAL_STALL_LATENCY_ITERATE_ON_ADMITTED_THREAD.
          if ((!i_stall && !kill_valid) || !latency_elapsed_upstream_data) begin // See comments above regarding kill_valid.
            out_dummy <= !thread_table[tt_ptr_read]; // TABLE_PTR_WIDTH:1 mux.
            out_valid <= ENABLE_INTERLEAVING? 1'b1 : thread_table[tt_ptr_read]; // If interleaving then we issue valids on every unstalled cycle. If not interleaving, only issue valids for a real thread.
          end else begin
            out_valid <= 1'b0;
          end
          if (!sclrn[0]) begin 
            out_dummy                     <= 1'b0;
            out_valid                     <= 1'b0;
          end
        end
      end
      
    end
  endgenerate

  // Thread table read pointer update.
  always_ff @(posedge clock or negedge aclrn) begin
    if (!aclrn) begin
      // If fast_admit=0, tt_ptr_read must always have a value of (tt_ptr_admit_ahead - TT_UPDATE_LATENCY). So initialize to 0-TT_UPDATE_LATENCY, but modulo II so it falls into the range [II-1:0]. This calculation is the equivalent of (0 - TT_UPDATE_LATENCY) % II.
      // For example, if II=7, and TT_UPDATE_LATENCY=1, then tt_ptr_read will reset to 6 (because -1 % 7 = 6)    
      // If fast_admit=1, we do not try to find the next vacant slot in a lookahead fashion, so jsut reset both pointers to 0.
      tt_ptr_read                   <= FAST_ADMIT? '0 : (II - (TT_UPDATE_LATENCY % II)) % II; 
      tt_ptr_admit_ahead            <= '0;
    end else begin
      /* Advance the pointer when:
        - i_stall=0 (downstream is not backpressuring)
        - In non-interleaving mode, only slot-0 is used, so if downstream is backpressuring, we can continue to advance the pointer as long as we stop on slot-0. This is important because if i_stall=1, we need to assert o_valid in order for downstream to set i_stall=0. If we don't advance the pointer, we'll never get back to slot-0 and o_valid will never assert.
        - Upstream data latency hasn't elapsed (we've asked for a thread from upstream and we're waiting for the data to arrive)
      */
      if ((!i_stall && !kill_valid) || (!ENABLE_INTERLEAVING && i_stall && tt_ptr_read != 0) || !latency_elapsed_upstream_data) begin
        if (tt_ptr_read == II-1) begin
          tt_ptr_read <= '0;
        end else begin
          tt_ptr_read <= tt_ptr_read + 1;
        end
        if (tt_ptr_admit_ahead == II-1) begin
          tt_ptr_admit_ahead <= '0;
        end else begin
          tt_ptr_admit_ahead <= tt_ptr_admit_ahead + 1;
        end        
      end
      if (!sclrn[0]) begin 
        // See comments above
        tt_ptr_read                   <= FAST_ADMIT? '0 : (II - (TT_UPDATE_LATENCY % II)) % II;
        tt_ptr_admit_ahead            <= '0;
      end
    end
  end  

  /*  *** Thread Table ***

    The logic below decides when to update the thread-table (ie. decides when to admit and exit threads from the table), and also performs the table update.

  */

  /*
    ** Admit decision **
    The behaviour of latency_elapsed_upstream_empty requires some explanation. When we ask for a thread from upstream (o_stall=0), i_empty becomes stale for LATENCY_UPSTREAM_EMPTY cycles.
    We must wait for these cycles to elapse before i_empty is safe to sample again when making another admit decision. This latency is counted using a latency-counter and latency_elapsed_upstream_empty tells us when the latency has elapsed.

    safe_to_read_from_upstream is used to indicate if upstream is guaranteed to have a thread we can read. If stall-latency is used (LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP>0), then i_almost_empty=0 guarantees that it's safe to read. If i_almost_empty=1 or if stall-valid is used, then we
    must observe i_empty, taking into account its staleness. This behaviour is the same as the "slow-read/fast-read" modes in the stall-latency protocol.

    If FAST_ADMIT=0, there is 1 cycle from admit_thread to out_stall. So on the very next cycle after admit_thread=1, i_empty is stale and we should not sample it. This is handled by latency_elapsed_upstream_empty de-asserting (=0) on this cycle using the ASSERT_IMMEDIATE feature of latency_counter.

    If FAST_ADMIT=1, then are 0 cycles of latency from admit_thread to out_stall. Therefore on the very next cycle after admit_thread=1, if the upstream latencies (on empty and data) are 1, it is actually safe to sample i_empty and make a
    decision again. This is the spirit of FAST_ADMIT mode (ie. make admit decisions as fast as possible). However, we turn off ASSERT_IMMEDIATE here because it otherwises causes a combinational loop from admit_thread -> out_stall -> 
    -> latency_elapsed_upstream_empty -> admit_thread. On the cycle after admit_thread=1, latency_elapsed_upstream_empty's value will be correct and will gate another admit_decision if necessary (which is necessary if LATENCY_UPSTREAM_EMPTY > 1)

    If FAST_EXIT=1, allow an admit decision to be made on the same cycle as thread_is_exiting. This assumes that latency from o_valid to i_exit_valid is >= 1 (otherwise we can have a combinational loop
    from admit_thread -> out_valid -> o_valid -> i_exit_valid -> thread_is_exiting -> admit_thread).

    The LUT depth of admit_thread depends on ii but is likely to be depth 2 in most cases. If this becomes an FMAX bottleneck, a future enhancement can be to pipeline admit_thread. This must be accounted for in the latency matching 
    formulas above as well as in latency_elapsed_upstream_empty (i.e. if admit_thread is pipelined, we should not erroneously make multiple admit decisions in a row due to staleness of i_empty).

    In the stall/valid protocol stall is allowed to remain asserted until valid asserts. This means that even if downstream has capacity, it is allowed to assert stall until it seeds valid=1 from upstream.
    Said another way, stall can depend on valid. 
    admit_thread_no_downstream_stall indicates if acl_loop_admit is ready to admit a new thread, whereas admit_thread indicates if the thread is actually being admitted (asserts when downstream is
    able to accept the new thread). A key point is that admit_thread_no_downstream_stall is used to drive out_valid to indicate that a thread is available in FAST_ADMIT mode.

  */
  assign safe_to_read_from_upstream = ((LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP > 0) && !i_almost_empty) || (!i_empty && latency_elapsed_upstream_empty); // See above for description.
  // We make the admit decision in advance of the target slot coming around to allow enough time for the thread-table update to complete. We determine when to make the admit decision
  // (and therefore when to commence the thread-table update) by waiting until we reach slot # tt_ptr_admit_ahead. The tt_ptr_admit_ahead pointer is always ahead of tt_ptr_read by the proper amount.
  // In interleaving, a key point is that we are searching for a vacancy in a round-robin fashion, meaning that we admit into the next slot that becomes available. This means that we may not admit in slot-order (i.e. slot-0 first, then slot-1, then slot-2 etc.).
  // This can occur if interleaved threads have different trip counts, meaning they exit at different times. 
  // We also only allow admission into the first MAX_THREAD slots, and MAX_THREADS=1 for non-interleaving, meaning that only one thread is admitted at a time.
  // If FAST_EXIT==1, then check if the target slot for admitting is being evacuated by an exit on the same cycle, if so, we can admit.
    
  assign admit_thread_no_downstream_stall = (tt_ptr_admit_ahead < MAX_THREADS) && (!thread_table[tt_ptr_admit_ahead] || (FAST_EXIT && thread_is_exiting && (tt_ptr_exit == tt_ptr_admit_ahead))) && safe_to_read_from_upstream;
  assign admit_thread = admit_thread_no_downstream_stall && !i_stall && !kill_valid;

  generate
    // In fast-admit, these output signals are generated combinationally, otherwise they're registered.
    if (FAST_ADMIT) begin
      always_comb begin
        out_stall = !admit_thread;
        out_forked = admit_thread_no_downstream_stall; // Assert fork when we can, irrespective of i_stall. This is because acl_pop's out_stall has a dependency on 'dir' (which is basically driven by out_forked). So if fork is not asserted, the pop can backpressure indefinitely.
      end
    end else begin
      always_ff @(posedge clock or negedge aclrn) begin
        if (!aclrn) begin
          out_stall     <= 1'b1;
          out_forked    <= 1'b0;
        end else begin
          if (admit_thread) begin
            out_stall   <= 1'b0;
            out_forked  <= 1'b1;
          end else begin
            out_stall   <= 1'b1;
            out_forked  <= 1'b0;      
          end
          if (!sclrn[0]) begin 
            out_stall   <= 1'b1;
            out_forked  <= 1'b0;
          end
        end
      end
    end
  endgenerate

  // Actual thread table
  always_ff @(posedge clock or negedge aclrn) begin
    if (!aclrn) begin
      thread_table  <= '0;
      tt_empty      <= '1;
      kill_valid    <= ENABLE_INTERLEAVING? 1'b1 : 1'b0;
    end else begin
      
      // Actual thread-table update
      for (int i=0;i<II;i++) begin : GEN_RANDOM_BLOCK_NAME_R26
        // Admits and Exits for different slots can be commited to the table on the same cycle.
        // The behaviour of FAST_EXIT can be confusing. 
        // If we see simultaneous admit and exit for a given slot,
          // and FAST_EXIT=0, the net result is an exit, and the table should be set to 0. This can occur if a thread's tripcount is 1 and stall-valid is used everywhere, meaning that the thread exits on the same cycle as it forks. This means that when a simultaneous admit and exit it seen on a given slot, they are for the same thread.
          // and FAST_EXIT=1, the net result is an admit, and the table should be set to 1. When FAST_EXIT=1, there is a requirement that latency from o_valid to i_exit_valid >= 1. Said another way, it is required to have at least
            // 1 cycle of latency, outside this IP, from fork to exit. This ensures that when a simultaneous admit and exit is seen on a given slot, they are for *different* threads.

        // If a slot is being exited and admitted in the same cycle, the admit takes priority.
        if (admit_thread && (tt_ptr_admit_ahead == i)) begin
          // Check if a thread is exiting on the same cycle from the same slot.
          if (!FAST_EXIT && thread_is_exiting && (tt_ptr_exit == i)) begin // If FAST-EXIT is disabled, then simply never occupy the thread-table.
            thread_table[i] <= 1'b0;
          end else begin // Either we're not exiting from slot-i or we are and FAST_EXIT==1. FAST_EXIT can only be used if there's at least 1 cycle of latency from o_valid to i_exit_valid.
            thread_table[i] <= 1'b1;
          end
        end else if (thread_is_exiting && (tt_ptr_exit == i)) begin
          thread_table[i] <= 1'b0;
        end

      end

      // Determine when to stop asserting output valid. We do this when interleaving AND the loop is empty AND there are no available threads from upstream.
      // The completion of kernel execution (often called kernel-done or kernel-finish) is determined by looking at the o_active signal from LSUs. So if the loop contains an LSU, the dummy/predicated valids will cause o_active=1, making it seem
      // like the kernel is still active even though it's finished, leading to a hang. So the solution for now is to cease outputting valids when the loop is empty.
      tt_empty[1] <= !(|thread_table); // TT is empty when all bits are 0.
      for (int i=2;i<=NUM_TT_EMPTY_PIPE_STAGES;i++) begin : GEN_RANDOM_BLOCK_NAME_R27
        tt_empty[i] <= tt_empty[i-1]; // Pipeline it and let the retimer balance the combinational logic. Latency insensitive, can pipeline arbitrarily.
      end

      kill_valid <= ENABLE_INTERLEAVING && tt_empty[NUM_TT_EMPTY_PIPE_STAGES] && i_empty;

      if (!sclrn[0]) begin 
        thread_table        <= '0;
        tt_empty            <= '1;
        kill_valid          <= ENABLE_INTERLEAVING? 1'b1 : 1'b0;
       end
    end
  end

  /*
    Latency matching for synchronization with upstream data arrival
  */

  // (potentially) delay o_valid/o_dummy to synchronize with the arrival of data from upstream.
  localparam O_VALID_LATENCY_ELONGATE_AMOUNT = my_max(OUTPUT_LATENCY_ELONGATE_AMOUNT,0);  // If OUTPUT_LATENCY_ELONGATE_AMOUNT is negative, set STAGES to 0. The kernel upstream latency will instead be elongated.
  acl_shift_register_no_reset_dont_merge
  #(
      .WIDTH  (2),
      .STAGES (O_VALID_LATENCY_ELONGATE_AMOUNT)  
  )
  kernel_downstream_latency_match
  (
      .clock  (clock),
      .D      ({out_valid, out_dummy}),
      .Q      ({o_valid, o_dummy})
  );

  // (potentially) delay o_stall to delay data from upstream so it can be synchronized with the generation of o_valid/o_dummy
  localparam O_STALL_LATENCY_ELONGATE_AMOUNT = my_max(-1 * OUTPUT_LATENCY_ELONGATE_AMOUNT,0); // If OUTPUT_LATENCY_ELONGATE_AMOUNT is positive, set STAGES to 0 since kernel downstream latency will be elongated.
  acl_shift_register_no_reset_dont_merge
  #(
      .WIDTH  (1),
      .STAGES (O_STALL_LATENCY_ELONGATE_AMOUNT)  
  )
  kernel_upstream_latency_match
  (
      .clock  (clock),
      .D      (out_stall), 
      .Q      (o_stall)
  );

  // out_forked asserts on the same cycle as out_stall. o_forked must be synchronized to the arrival of data to kernel downstream. It must also be synchronized with o_valid/o_dummy, which will happen
  // because o_valid/o_dummy are also synchronized to the data (above).
  localparam O_FORKED_LATENCY_ELONGATE_AMOUNT = O_STALL_LATENCY_ELONGATE_AMOUNT + LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP;
  acl_shift_register_no_reset_dont_merge
  #(
      .WIDTH  (1),
      .STAGES (O_FORKED_LATENCY_ELONGATE_AMOUNT)
  )
  fork_latency_match
  (
      .clock  (clock),
      .D      (out_forked), 
      .Q      (o_forked)
  );

  // When generating out_valid, we need to mask/ignore i_stall when reading from upstream because the upstream latency-match calculations assume that tt_ptr_read advances on every cycle.
  // We must mask long enough to generate the out_valid corresponding to the first iteration (and forked signal). This occurs TT_LATENCY_ADMIT_TO_FIRST_READ cycles after the admit decision.
  // For up to and including that cycle, we must ignore i_stall. 'latency_elapsed_upstream_data' gets asserted 1 cycle after that, which unmasks i_stall.
  // In fast-admit, out_valid is asserted immediately and we do not rely on tt_ptr_read advancing on every cycle of upstream data latency. In this scenario, TT_LATENCY_ADMIT_TO_FIRST_READ=0, so we'll end up masking i_stall for just 1 cycle, which
  // is the desired behaviour.
  localparam INPUT_STALL_MASK_DURATION = TT_LATENCY_ADMIT_TO_FIRST_READ+1;
  latency_counter
  #(
      .ASYNC_RESET            (ASYNC_RESET),
      .SYNCHRONIZE_RESET      (0), 
      .LATENCY                (INPUT_STALL_MASK_DURATION), 
      .ASSERT_AFTER_RESET     (1), // If the latency has *not* elapsed, we ignore i_stall. This is only relevant when reading from upstream, so assert o_elapsed after reset to allow the first read at the beginning of time to take place.
      .ASSERT_IMMEDIATE       (0)  // latency_elapsed_upstream_data does not need to be consumed immediately.
  )
  latency_counter_upstream_data_latency
  (
      .clock                  (clock),
      .resetn                 (resetn),
      .i_start                (admit_thread), 
      .o_elapsed              (latency_elapsed_upstream_data)
  );

  // This counts upstream latency on i_empty. After sampling i_empty to make an admit decision, i_empty becomes stale. This counter tells us when its safe to sample again.
  localparam UPSTREAM_EMPTY_LATENCY_COUNT = O_STALL_LATENCY_ELONGATE_AMOUNT+LATENCY_UPSTREAM_EMPTY;
  latency_counter
  #(
      .ASYNC_RESET            (ASYNC_RESET),
      .SYNCHRONIZE_RESET      (0), 
      .LATENCY                (UPSTREAM_EMPTY_LATENCY_COUNT), 
      .ASSERT_AFTER_RESET     (1),            // After reset it's safe to consume i_empty immediately.
      .ASSERT_IMMEDIATE       (!FAST_ADMIT)   // When out_stall=0, the upstream empty count must start immediately. Otherwise admit_thread will assert and potentionally underflow upstream.
  )
  latency_counter_upstream_empty
  (
      .clock                  (clock),
      .resetn                 (resetn),
      .i_start                (!out_stall), // Start counting the empty latency when we read from upstream
      .o_elapsed              (latency_elapsed_upstream_empty)
  );

  /*  *** Exit Table ***

    This logic decides when to update the exit-table and communicates the exit observation back to the thread table.

  */

  // Detect the exit condition, on real (!dummy) threads, from the data path.
  logic real_exit_occurring;
  assign real_exit_occurring = i_exit && (i_exit_valid && (EXIT_USES_STALL_VALID? !i_exit_stall : 1'b1)) && !i_exit_dummy;
  
  generate
    if (FAST_ADMIT) begin
      // In fast-admit, do not pipeline the observation of the exit condition. This is done to minimize the latency from exit to the next admit decision.
      assign thread_is_exiting = real_exit_occurring && (exit_table[et_ptr_read] || i_exit_forked); // Observe i_exit_forked to catch the special case where the loop tripcount=1 (loop exits immediately on the first iteration)
      assign tt_ptr_exit = et_ptr_read;

    end else begin
      always_ff @(posedge clock or negedge aclrn) begin
        if (!aclrn) begin
          thread_is_exiting             <= 1'b0;
          tt_ptr_exit                   <= '0;
        end else begin
          thread_is_exiting             <= 1'b0;
          // Check if we get an exit condition on a real thread.
          if (real_exit_occurring) begin
            if (exit_table[et_ptr_read] || i_exit_forked) begin
              thread_is_exiting         <= 1'b1; // Tell the thread table that a real thread is exiting. This can be further pipelined if necessary.
              tt_ptr_exit               <= et_ptr_read;
            end
          end
          if (!sclrn[0]) begin 
            thread_is_exiting           <= 1'b0;
            tt_ptr_exit                 <= '0;
          end
        end
      end
    end
  endgenerate

  // Update the exit table
  always_ff @(posedge clock or negedge aclrn) begin
    if (!aclrn) begin
      exit_table  <= '0;
      et_ptr_read <= ENABLE_INTERLEAVING? (FAST_ADMIT? '0 : (II - (TT_UPDATE_LATENCY % II)) % II) : 0; // Reset value matches tt_ptr_read. The first i_exit_valid we see must correspond to the slot # for which the first o_valid was generated. This slot # is simply the reset value of tt_ptr_read.
    end else begin

      // We expect one i_exit_valid for every slot, so use it to iterate over all the slots
      if (i_exit_valid && (EXIT_USES_STALL_VALID? !i_exit_stall : 1'b1)) begin
        et_ptr_read <= (et_ptr_read == MAX_THREADS-1)? '0 : et_ptr_read + 1;
      end
      
      /*  The exit_table tracks the valid window of observability for the exit condition.
          The window is entered when i_exit_forked=1 (first real iteration), and we exit the window when the loop exits.
          The exit-table update latency must be 1 cycle. This is to handle the case of ii=1 and tripcount=2, in which it's possible for exit
          to assert 1 cycle after i_exit_forked. On this cycle, the table must be up-to-date.
          If ii=1, the table is only 1 bit and the et_ptr_read comparison is also 1 bit, so the logic depth should be alright. If ii>1, the comparison widens, but if this becomes a problem,
          we can pipeline the table update decision because we have time to do this after all (since ii>1).
      */

      // Logic for exiting the window. Check if we get an exit condition on a real thread.
      if (real_exit_occurring) begin
        // Read the exit-table to determine if we're in the window.
        // In the special case of exiting on the first thread, the exit-table may not have had time to update.
        if (exit_table[et_ptr_read] || i_exit_forked) begin
          // Update the table to say we're outside the window
          exit_table[et_ptr_read] <= 1'b0;
        end
      end

      // Logic for entering the window. (takes priority over the above assignment)
      for (int i=0;i<II;i++) begin : GEN_RANDOM_BLOCK_NAME_R28
        // Enter the window when i_exit_forked asserts on this slot. But not if we're simultaneously exiting (ie. this is special handling when tripcount=1)
        if ((i_exit_valid && (EXIT_USES_STALL_VALID? !i_exit_stall : 1'b1)) && i_exit_forked && !i_exit_dummy && !i_exit && et_ptr_read==i) begin
          exit_table[i] <= 1'b1;
        end
      end

      if (!sclrn[0]) begin 
        exit_table  <= '0;
        et_ptr_read <= ENABLE_INTERLEAVING? (FAST_ADMIT? '0 : (II - (TT_UPDATE_LATENCY % II)) % II) : 0; // See comments above.
      end
    end
  end

  // Stall-Latency calculations. These values are used by the testbench but also the compiler needs to perform the same calculation to ensure this module's stall-latency is modeled correctly.
  /* LATENCY_INTERNAL_STALL_TO_VALID is Latency from i_stall to o_valid. This latency can't be set here, rather, this localparam must report what the latency is and must be kept up to date.
     This localparam is read by the testbench.
  */
  
  // The latency from i_stall to o_valid can have 2 possible values, so we take the maximum here. 
  // It has one value when launching iterations on an admitted thread. And has another value when admitting a new thread (because we have to fetch the data from upstream)

  // Cycles from i_stall to out_valid = 0 in fast-admit, 1 otherwise.
  // There are additionally O_VALID_LATENCY_ELONGATE_AMOUNT cycles from out_valid to o_valid.
  localparam INTERNAL_STALL_LATENCY_ITERATE_ON_ADMITTED_THREAD = (FAST_ADMIT? 0 : 1) + O_VALID_LATENCY_ELONGATE_AMOUNT; 

  // Cycles from i_stall to out_stall = 0 in fast-admit, 1 otherwise.
  // + O_STALL_LATENCY_ELONGATE_AMOUNT cycles from out_stall to o_stall
  // + LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP from o_stall to o_valid.
  localparam INTERNAL_STALL_LATENCY_ADMIT_NEW_THREAD = (FAST_ADMIT? 0 : 1) + O_STALL_LATENCY_ELONGATE_AMOUNT + LATENCY_UPSTREAM_STALL_VALID_ROUND_TRIP; 
  // Take the max
  localparam LATENCY_INTERNAL_STALL_TO_VALID = my_max(INTERNAL_STALL_LATENCY_ITERATE_ON_ADMITTED_THREAD,INTERNAL_STALL_LATENCY_ADMIT_NEW_THREAD);

  // Latency from i_almost_empty to o_stall. This value is needed by the TB and compiler when computing upstream's almost-empty threshold.
  // Cycles from i_almost_empty to out_stall = 0 in fast-admit, 1 otherwise.
  // + O_STALL_LATENCY_ELONGATE_AMOUNT cycles from out_stall to o_stall
  localparam LATENCY_INTERNAL_ALMOST_EMPTY_TO_O_STALL = (FAST_ADMIT? 0 : 1) + O_STALL_LATENCY_ELONGATE_AMOUNT;

  // This param is used by the TB.
  // In !fast-admit, 1 cycle from exit to thread_is_exiting, TT_UPDATE_LATENCY cycles to update thread table. Future enhancement: include having to wait up to II-1 cycles when interleaving. The TB accounts for this, but it's better if it's accounted for here.
  // This would provide a reference formula for the compiler if it wanted to accurately calculate LATENCY_EXIT_TO_ADMIT_DECISION.
  // In fast-admit, 0 cycles from exit to thread_is_exiting, 1 cycle to thread_table.
  localparam LATENCY_EXIT_TO_ADMIT_DECISION = 1 + TT_UPDATE_LATENCY;


  `ifdef ALTERA_RESERVED_QHD
  `else
  //synthesis translate_off
  `endif
  generate
  // TODO: enable this once it's hooked up in the compiler
  //if (LATENCY_INTERNAL_STALL_TO_VALID_FROM_COMPILER != LATENCY_INTERNAL_STALL_TO_VALID) begin
  //    $fatal(1, "acl_loop_admit: internal stall-latency value from compiler does not match (%d) does not match the IP's internal calculation of this value(%d).\n", LATENCY_INTERNAL_STALL_TO_VALID_FROM_COMPILER, LATENCY_INTERNAL_STALL_TO_VALID);
  //end
  endgenerate
  `ifdef ALTERA_RESERVED_QHD
  `else
  //synthesis translate_on
  `endif

  `ifdef SIM_ONLY
  initial begin
    $display("--- acl_loop_admit localparams ---");
    $display("TT_UPDATE_LATENCY = %d", TT_UPDATE_LATENCY);
    $display("TT_LATENCY_ADMIT_TO_FIRST_READ = %d", TT_LATENCY_ADMIT_TO_FIRST_READ);
    $display("TT_READ_LATENCY = %d", TT_READ_LATENCY);
    $display("OUTPUT_LATENCY_ELONGATE_AMOUNT = %d", OUTPUT_LATENCY_ELONGATE_AMOUNT);
    $display("O_VALID_LATENCY_ELONGATE_AMOUNT = %d", O_VALID_LATENCY_ELONGATE_AMOUNT);
    $display("O_STALL_LATENCY_ELONGATE_AMOUNT = %d", O_STALL_LATENCY_ELONGATE_AMOUNT);
    $display("O_FORKED_LATENCY_ELONGATE_AMOUNT = %d", O_FORKED_LATENCY_ELONGATE_AMOUNT);
    $display("INPUT_STALL_MASK_DURATION = %d", INPUT_STALL_MASK_DURATION);
    $display("UPSTREAM_EMPTY_LATENCY_COUNT = %d", UPSTREAM_EMPTY_LATENCY_COUNT);
    $display("LATENCY_INTERNAL_STALL_TO_VALID = %d", LATENCY_INTERNAL_STALL_TO_VALID);
    $display("INTERNAL_STALL_LATENCY_ITERATE_ON_ADMITTED_THREAD = %d", INTERNAL_STALL_LATENCY_ITERATE_ON_ADMITTED_THREAD);
    $display("INTERNAL_STALL_LATENCY_ADMIT_NEW_THREAD = %d", INTERNAL_STALL_LATENCY_ADMIT_NEW_THREAD);
    $display("LATENCY_INTERNAL_ALMOST_EMPTY_TO_O_STALL = %d", LATENCY_INTERNAL_ALMOST_EMPTY_TO_O_STALL);
    $display("LATENCY_EXIT_TO_ADMIT_DECISION = %d", LATENCY_EXIT_TO_ADMIT_DECISION);

  end
  `endif

endmodule

/*
  Simple shift-register latency counter. There are a few instances in which acl_loop_admit needs to track that some particular latency has elapsed.
  This module asserts and latches o_elapsed LATENCY cycles after i_start asserts. When i_start asserts, o_elapsed de-asserts on the next cycle (not applicable if LATENCY=0 or 1).
  If ASSERT_IMMEDIATE=0, it assumes that the earliest o_elapsed is observed is on the cycle after i_start asserts. In other words, i_start does not combinationally clear o_elapsed (except when LATENCY=0, in which case o_elapsed=i_start).
  If ASSERT_IMMEDIATE=1, o_elapsed will de-assert immediately on the same cycle that i_start asserts, allowing o_elapsed to be observed on the same cycle. Therefore o_elapsed is combinational with i_start. 
    If i_start has a lot of combinational logic already, this setting can lead to a larger LUT-depth.
  It assumes that reset is externally held long enough to flush the shift-register.
*/

module latency_counter #(
    parameter int LATENCY = 1,            // Must be >= 0.
    parameter int ASSERT_AFTER_RESET = 0, // Assert o_elapsed after reset. This is useful if the instantiator doesn't need to wait for the latency to elapse specifically after reset.
    parameter int ASSERT_IMMEDIATE = 0,   // 1 = Assert o_elapsed immediately with i_start. Otherwise assert on the next cycle
    parameter int ASYNC_RESET = 1,        // 1 = Registers are reset asynchronously. 0 = Registers are reset synchronously -- the reset signal is pipelined before consumption. In both cases, some registers are not reset at all.
    parameter int SYNCHRONIZE_RESET = 0   // 1 = resetn is synchronized before consumption. The consumption itself is either asynchronous or synchronous, as specified by ASYNC_RESET.
)
(
    input  wire     clock,
    input  wire     resetn,
    input  wire     i_start,
    output logic    o_elapsed   // o_elapsed asserts LATENCY cycles after i_start asserts.
);

  `ifdef ALTERA_RESERVED_QHD
  `else
  //synthesis translate_off
  `endif
  generate
  if (LATENCY < 0) begin
      $fatal(1, "acl_loop_admit --> latency_counter: illegal value of LATENCY= %d, minimum allowed is 0\n", LATENCY);
  end
  endgenerate
  `ifdef ALTERA_RESERVED_QHD
  `else
  //synthesis translate_on
  `endif

  localparam NUM_RESET_COPIES = 1;
  localparam RESET_PIPE_DEPTH = 0;

  logic                         aclrn;
  logic [NUM_RESET_COPIES-1:0]  sclrn;

  acl_reset_handler
  #(
      .ASYNC_RESET            (ASYNC_RESET),
      .USE_SYNCHRONIZER       (SYNCHRONIZE_RESET),
      .SYNCHRONIZE_ACLRN      (SYNCHRONIZE_RESET),
      .PIPE_DEPTH             (RESET_PIPE_DEPTH),
      .NUM_COPIES             (NUM_RESET_COPIES) 
  )
  acl_reset_handler_inst
  (
      .clk                    (clock),
      .i_resetn               (resetn),
      .o_aclrn                (aclrn),
      .o_resetn_synchronized  (),  
      .o_sclrn                (sclrn)
  );

  logic [LATENCY:1] pipe;

  generate
    if (LATENCY == 0) begin
    
      assign o_elapsed = i_start;
    
    end else if (LATENCY == 1) begin

      always_ff @(posedge clock or negedge aclrn) begin
        if (!aclrn) begin
          pipe[1] <= ASSERT_AFTER_RESET;
        end else begin
          if (i_start) begin
            pipe[1] <= 1'b1;
          end
          if (!sclrn[0]) begin 
            pipe[1] <= ASSERT_AFTER_RESET;
          end
        end
      end    

      assign o_elapsed = (ASSERT_IMMEDIATE && i_start)? 1'b0 : pipe[1];

    end else begin // LATENCY >= 2

      always_ff @(posedge clock or negedge aclrn) begin
        if (!aclrn) begin
          pipe[LATENCY] <= ASSERT_AFTER_RESET;
        end else begin
          
          pipe[1] <= i_start; // Push into the beginning of the shift-reg

          // The end of the pipe is handled separately from the other stages to facilitate ASSERT_AFTER_RESET.
          if (i_start) begin
            pipe[LATENCY] <= 1'b0; // Clear the end of the pipe when starting the count
          end else if (pipe[LATENCY-1]) begin // Implement the end of the shift-reg
            pipe[LATENCY] <= 1'b1;
          end
          
          if (i_start) begin
            // Clear the pipe when starting the count.
            for (int i=2;i<=LATENCY-1;i++) begin // Implement the middle of the shift-reg
              pipe[i] <= 1'b0;
            end            
          end else begin
            for (int i=2;i<=LATENCY-1;i++) begin // Implement the middle of the shift-reg
              pipe[i] <= pipe[i-1];
            end
          end

          if (!sclrn[0]) begin 
            pipe[LATENCY] <= ASSERT_AFTER_RESET;
          end
        end
      end
      assign o_elapsed = (ASSERT_IMMEDIATE && i_start)? 1'b0 : pipe[LATENCY]; // Grab the end of the shift-reg
    end
  endgenerate

endmodule

`default_nettype wire
