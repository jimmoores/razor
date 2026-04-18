.text
.align 2

// void ccsp_cif_occam_call(void *sched, void *stack, word *ws, void *func, word top)
//
// Phase 2: CIF -> occam call helper.  Sets up an occam call frame
// on the workspace `ws`, jumps to `func`, and resumes when occam
// returns via I_RET back to a label inside this function body.
//
// Arguments (AAPCS):
//   x0 = sched
//   x1 = stack (unused)
//   x2 = ws
//   x3 = func
//   x4 = top
//
// The save-area address (ws + top*8) is stashed at ws[-10] (byte
// -80 from ws) before the SP switch so it can be recovered after
// occam returns.  See x64_cif.S for the rationale.
#if defined(__APPLE__)
.global _ccsp_cif_occam_call
_ccsp_cif_occam_call:
#else
.global ccsp_cif_occam_call
ccsp_cif_occam_call:
#endif
    // Phase 4D: Wptr = sp.  Save callee-saved regs on C stack,
    // then switch sp to the occam workspace for the call.
    stp x29, x30, [sp, #-16]!
    stp x19, x20, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    stp x23, x24, [sp, #-16]!
    stp x25, x26, [sp, #-16]!
    stp x27, x28, [sp, #-16]!

    // x2 = ws (workspace), x4 = top (word count)
    lsl x4, x4, #3           // x4 = top * 8
    add x9, x2, x4           // x9 = ws + top*8 (save_area)

    mov x10, sp
    str x10, [x9]            // save_area[0] = C SP

    // Stash save_area address at ws[-10]
    stur x9, [x2, #-80]

    // Set up the occam return label at ws[0] and ws[-1]
    adr x10, .Locc_call_return
    str x10, [x2]            // ws[0] = return label
    stur x10, [x2, #-8]      // ws[-1] = return label

    // Phase 4D: switch sp to workspace (Wptr = sp)
    mov sp, x2               // sp = ws = Wptr

    // Set sched runtime register and jump to occam
    mov x25, x0              // x25 = sched
    br x3                    // jump to occam func

.Locc_call_return:
    // Occam returned via I_RET; sp = ws + 32 due to I_RET frame pop.
    // Phase 4D: sp IS Wptr, so sp is already ws + 32.
    // Use x28 as scratch to compute ws = sp - 32.
    mov x28, sp
    sub x28, x28, #32        // x28 = ws

    // Reload save_area address from ws[-10] and restore C SP
    ldur x9, [x28, #-80]
    ldr x10, [x9]
    mov sp, x10               // sp = C stack

    // Pop callee-saved regs
    ldp x27, x28, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16
    ldp x29, x30, [sp], #16
    ret

// void ccsp_cif_jump(word *wptr, void *addr) __attribute__((noreturn))
//
// Phase 4D: CIF -> occam transition primitive.  Used by ProcEnd to
// leave a "called from occam" CIF process by jumping back to the
// parent's escape pointer.  Sets sp = wptr (Phase 4D: Wptr = sp)
// and tail-jumps to addr.
//
// Arguments (AAPCS):
//   x0 = wptr
//   x1 = addr
#if defined(__APPLE__)
.global _ccsp_cif_jump
_ccsp_cif_jump:
#else
.global ccsp_cif_jump
ccsp_cif_jump:
#endif
    mov sp, x0           // sp = Wptr (Phase 4D)
    br x1                // tail-jump

// word ccsp_cif_external_call(void *func, void *stack)
//
// Phase 2: external C call helper.  Saves the caller's sp, switches
// to `stack`, calls func() (with any args already placed by caller),
// switches sp back, returns the result in x0.
//
// Arguments:
//   x0 = func
//   x1 = stack
// Returns:
//   x0 = func's return value
#if defined(__APPLE__)
.global _ccsp_cif_external_call
_ccsp_cif_external_call:
#else
.global ccsp_cif_external_call
ccsp_cif_external_call:
#endif
    stp x29, x30, [sp, #-16]!    // save fp, lr
    mov x29, sp
    stp x19, x20, [sp, #-16]!    // save callee-saved regs
    stp x21, x22, [sp, #-16]!
    stp x23, x24, [sp, #-16]!
    stp x25, x26, [sp, #-16]!
    stp x27, x28, [sp, #-16]!
    mov x19, sp                  // save sp in callee-saved x19
    mov x9, x0                   // x9 = func
    mov sp, x1                   // switch to provided stack
    blr x9                       // call func()
    mov sp, x19                  // restore sp
    ldp x27, x28, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16
    ldp x29, x30, [sp], #16
    ret                          // result already in x0

// ccsp_cif_proc_stub_label  (Phase 2)
// ccsp_cif_light_proc_stub_label
//
// CIF process trampoline labels.  Same idea as x64 versions: when the
// scheduler dispatches a CIF process, control lands here with sp =
// process Wptr (Phase 4D), x25 = sched.  The process workspace top (wptr[0])
// holds [func, ws_arg].  We:
//   1. Switch sp to wptr[0] (private stack)
//   2. Save sched into wptr[SchedPtr] (= -56)
//   3. Pop func into x30, ws into x0 (1st AAPCS arg)
//   4. Call func(ws)
//   5. Restore sched from wptr[SchedPtr]
//   6. Set up kernel ABI args (x0=p0, x1=sched, x2=Wptr)
//   7. Switch sp back to sched->stack
//   8. Tail-jump to kernel_Y_proc_end / kernel_Y_endp
//
// proc_stub:        p0 = wptr - CIF_PROCESS_WORDS*8 = wptr - 192
// light_proc_stub:  p0 = *(wptr + BarrierPtr*8) = *(wptr - 64)
//
// Uses `b` (tail call) since AAPCS sp is 16-byte aligned both before
// and after a function call -- no +8 bias to maintain.

#if defined(__APPLE__)
.global _ccsp_cif_proc_stub_label
_ccsp_cif_proc_stub_label:
#else
.global ccsp_cif_proc_stub_label
ccsp_cif_proc_stub_label:
#endif
    // Phase 4D: Wptr = sp.  Use x28 as scratch for the Wptr value.
    mov x28, sp                  // x28 = Wptr (save sp for workspace access)
    ldr x9, [x28, #0]            // x9 = wptr[0] (private stack top)
    stur x25, [x28, #-56]        // wptr[SchedPtr] = sched
    mov sp, x9                   // sp = private stack (C call)
    ldr x30, [sp], #16           // x30 = func; sp += 16
    ldur x0, [sp, #-8]           // x0 = ws (was at sp+8 before increment)
    blr x30                      // call func(ws)
    ldur x25, [x28, #-56]        // restore sched
    ldr x9, [x25, #0]            // x9 = sched->stack
    mov sp, x9                   // sp = kernel stack
    sub x0, x28, #192            // p0 = wptr - CIF_PROCESS_WORDS*8
    mov x1, x25                  // x1 = sched
    mov x2, x28                  // x2 = Wptr
#if defined(__APPLE__)
    b _kernel_Y_proc_end         // tail-call (Mach-O symbol prefix)
#else
    b kernel_Y_proc_end          // tail-call
#endif

#if defined(__APPLE__)
.global _ccsp_cif_light_proc_stub_label
_ccsp_cif_light_proc_stub_label:
#else
.global ccsp_cif_light_proc_stub_label
ccsp_cif_light_proc_stub_label:
#endif
    // Phase 4D: Wptr = sp.  Use x28 as scratch.
    mov x28, sp                  // x28 = Wptr (save sp)
    ldr x9, [x28, #0]            // x9 = wptr[0] (private stack top)
    stur x25, [x28, #-56]        // wptr[SchedPtr] = sched
    mov sp, x9                   // sp = private stack (C call)
    ldr x30, [sp], #16           // x30 = func; sp += 16
    ldur x0, [sp, #-8]           // x0 = ws
    blr x30                      // call func(ws)
    ldur x25, [x28, #-56]        // restore sched
    ldr x9, [x25, #0]            // x9 = sched->stack
    mov sp, x9                   // sp = kernel stack
    ldur x0, [x28, #-64]         // p0 = *(wptr + BarrierPtr*8)
    mov x1, x25                  // x1 = sched
    mov x2, x28                  // x2 = Wptr
#if defined(__APPLE__)
    b _kernel_Y_endp             // tail-call (Mach-O symbol prefix)
#else
    b kernel_Y_endp              // tail-call
#endif

// ccsp_cif_endp_resume_label  (Phase 2)
//
// Code-only label whose address is returned by kernel_CIF_endp_resume_stub
// and stored as the IptrSucc of every LightProc barrier.  When a child
// process ends and the barrier count drops to zero, the parent resumes
// here: load the parent's Wptr from wptr[-4] (= byte -32 from current
// x28) and jump to its saved Iptr at wptr[-1] (= byte -8 from x28).
//
// This is NOT a callable function -- it has no prologue and is only
// ever entered via Iptr-jump from the scheduler, with sp already
// set to the resuming process's Wptr by ccsp_dispatch_process (Phase 4D).
#if defined(__APPLE__)
.global _ccsp_cif_endp_resume_label
_ccsp_cif_endp_resume_label:
#else
.global ccsp_cif_endp_resume_label
ccsp_cif_endp_resume_label:
#endif
    // Phase 4D: Wptr = sp.  Use x28 as scratch.
    ldur x28, [sp, #-32]   // x28 = parent Wptr (from ws[Pointer])
    ldur x9, [x28, #-8]    // x9 = parent Wptr[Iptr]
    mov sp, x28            // sp = parent Wptr
    br x9                  // jump

// void ccsp_kernel_enter(void *init, void *stack, word *Wptr, word *Fptr) __attribute__((noreturn))
//
// Phase 2: kernel boot primitive.  Replaces the inline-asm K_ENTRY
// macro called from ccsp_kernel_entry() in entry.c.  Switches sp to
// the freshly-laid-out kernel stack and tail-calls `init` (typically
// kernel_Y_rtthreadinit) with the legacy 3-arg runtime convention:
//   init(stack, Fptr, Wptr) where stack->p0, Fptr->sched, Wptr->Wptr.
//
// Arguments (AAPCS):
//   x0 = init
//   x1 = stack  (becomes p0)
//   x2 = Wptr
//   x3 = Fptr   (becomes sched)
#if defined(__APPLE__)
.global _ccsp_kernel_enter
_ccsp_kernel_enter:
#else
.global ccsp_kernel_enter
ccsp_kernel_enter:
#endif
    mov x9, x0           // save init in scratch
    mov sp, x1           // sp = stack
    mov x0, x1           // x0 = stack (p0)
    mov x1, x3           // x1 = Fptr (sched)
    // x2 = Wptr (already correct)
    br x9                // tail-call init

// void ccsp_dispatch_process(sched_t *sched, word *Wptr) __attribute__((noreturn))
//
// Phase 2: kernel-to-user dispatch primitive.  Replaces the inline-asm
// K_ZERO_OUT_JRET macro at all ~13 call sites in sched.c.  This function
// never returns to its caller.
//
// Sets the runtime register convention for the dispatched process,
// restores the kernel SP from sched->stack, and jumps to the saved
// instruction pointer at Wptr[-1] (= byte -8 from Wptr).
//
// Arguments (AAPCS):
//   x0 = sched
//   x1 = Wptr
#if defined(__APPLE__)
.global _ccsp_dispatch_process
_ccsp_dispatch_process:
#else
.global ccsp_dispatch_process
ccsp_dispatch_process:
#endif
    mov x25, x0            // x25 = sched (runtime convention)
    mov sp, x1             // sp = Wptr (Phase 4D: Wptr = sp)
    // Use x1 (not sp) for the iptr load: CIF processes hand the kernel
    // a Wptr that is +1 word from the underlying 16-aligned base, i.e.
    // 8-aligned.  Loads/stores using SP as base on aarch64 EL0 trigger
    // SIGBUS unless SP is 16-aligned (SCTLR_EL1.SA0=1, the Linux
    // default).  Loads via a general register have no such restriction,
    // so reading the iptr through x1 is safe regardless of alignment.
    // For occam processes, sp here is 16-aligned and any subsequent
    // sp-relative access in the resume code remains safe; for CIF
    // processes the resume code (ccsp_cif_call_asm_resched) restores
    // sp from a saved value before any sp-relative access.
    ldur x9, [x1, #-8]     // x9 = Wptr[Iptr]
    br x9                  // jump

// word ccsp_cif_call_asm(void *wptr, void *sched, void *param0, void *func)
// Non-rescheduling variant: simple call, no context switch possible.
#if defined(__APPLE__)
.global _ccsp_cif_call_asm
_ccsp_cif_call_asm:
#else
.global ccsp_cif_call_asm
ccsp_cif_call_asm:
#endif
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    stp x19, x20, [sp, #-16]!

    mov x19, sp           // save sp

    // setup KRoC kernel arguments: x0=param0, x1=sched, x2=wptr
    mov x8, x0            // x8 = wptr
    mov x0, x2            // x0 = param0
    mov x2, x8            // x2 = wptr
    // x1 is already sched

    blr x3                // call func

    mov sp, x19           // restore sp

    ldp x19, x20, [sp], #16
    ldp x29, x30, [sp], #16
    ret

// word ccsp_cif_call_asm_resched(void *wptr, void *sched, void *param0, void *func)
//
// Rescheduling variant: the kernel may context-switch this process.
// Callee-saved registers are saved to the WORKSPACE (not the C stack)
// because sched->stack is shared by all processes.  When another process
// runs and calls C functions (fputc, write_screen, etc.), those calls
// use the same sched->stack and would overwrite any saved registers
// left on it.  The workspace is private to this process and survives
// context switches.
//
// Workspace layout for saves (byte offsets from wptr = x0 at entry):
//   wptr[-10]  = saved SP       (byte -80)
//   wptr[-11]  = saved x30/LR   (byte -88)
//   wptr[-12]  = saved x29/FP   (byte -96)
//   wptr[-13]  = saved x19      (byte -104)
//   wptr[-14]  = saved x20      (byte -112)
//   wptr[-15]  = saved x21      (byte -120)
//   wptr[-16]  = saved x22      (byte -128)
//   wptr[-17]  = saved x23      (byte -136)
//   wptr[-18]  = saved x24      (byte -144)
//   wptr[-19]  = saved x25      (byte -152)
//   wptr[-20]  = saved x26      (byte -160)
//   wptr[-21]  = saved x27      (byte -168)
//   wptr[-22]  = saved x28      (byte -176)
//
// compose_cif_call_aarch64 saves at wptr[-8..-6] (FLABEL, marker, sched).
// The CIF stub uses wptr[-10..-22] for callee-saved regs.
// These don't overlap.
//
#if defined(__APPLE__)
.global _ccsp_cif_call_asm_resched
_ccsp_cif_call_asm_resched:
#else
.global ccsp_cif_call_asm_resched
ccsp_cif_call_asm_resched:
#endif
    // Strip any PAC (Pointer Authentication Code) bits from LR before
    // saving it to the workspace.  libc functions (e.g. pthread_*) are
    // compiled with PAC and sign return addresses; if we save a signed
    // LR and later restore it for `br x30`, the PAC bits make the
    // address non-canonical and trigger SIGSEGV.  xpaclri clears the
    // PAC bits from x30 to produce a plain code pointer.
    hint #7        // xpaclri: strip PAC from x30 (NOP on non-PAC CPUs)

    // Save callee-saved registers to the WORKSPACE at negative offsets from wptr (x0).
    stur x29, [x0, #-96]
    stur x30, [x0, #-88]
    stur x19, [x0, #-104]
    stur x20, [x0, #-112]
    stur x21, [x0, #-120]
    stur x22, [x0, #-128]
    stur x23, [x0, #-136]
    stur x24, [x0, #-144]
    stur x25, [x0, #-152]
    stur x26, [x0, #-160]
    stur x27, [x0, #-168]
    stur x28, [x0, #-176]

    // Save current SP (the CIF private stack from ccsp_cif_process_call)
    mov x9, sp
    stur x9, [x0, #-80]   // wptr[-10] = saved private SP

    // Save sched at wptr[-7] (SchedPtr)
    str x1, [x0, #-56]

    mov x28, x0           // x28 = wptr

    // CRITICAL: switch to the scheduler's kernel stack BEFORE calling
    // the kernel function.  The CIF private stack is small (~248 bytes
    // minus the CIF C function's own frame) and would overflow when
    // kernel calls go deep (e.g. kernel_Y_outbyte -> kernel_chan_io ->
    // kernel_scheduler -> ccsp_safe_pause -> libc futex_wait).
    // The kernel stack at sched->stack has plenty of room.
    ldr x9, [x1]          // x9 = sched->stack
    mov sp, x9            // sp = kernel stack

    // setup KRoC kernel arguments: x0=param0, x1=sched, x2=wptr
    mov x8, x28           // x8 = wptr
    mov x0, x2            // x0 = param0
    mov x2, x8            // x2 = wptr
    // x1 is already sched

    // Pre-load x25 = sched so the post-call store of x25 to
    // wptr[SchedPtr] is correct in BOTH paths:
    //   (a) Kernel reschedules via K_ZERO_OUT_JRET: x25 is overwritten
    //       with the (possibly migrated) current sched.
    //   (b) Kernel returns normally (e.g. kernel_Y_mt_lock when the
    //       lock is free): x25 is preserved across the call as a
    //       callee-saved register, so it still holds sched.
    // Without this, the caller's x25 (which GCC may use to cache an
    // unrelated value such as a hoisted function pointer) would leak
    // into wptr[SchedPtr] in path (b), corrupting the workspace.
    mov x25, x1           // x25 = sched

    // Call KRoC function (now on the proper kernel stack)
    blr x3

    // Return-from-kernel handling.  There are two ways control can
    // arrive here, and they need slightly different bookkeeping:
    //
    //   (a) Natural return: blr returned normally with x28 still
    //       holding wptr (preserved by AAPCS through the C kernel
    //       call).  We just continue.
    //
    //   (b) Dispatched resume after reschedule: K_ZERO_OUT_JRET
    //       called ccsp_dispatch_process which did mov sp, wptr and
    //       branched to PROC_DESC->iptr.  iptr is captured by
    //       K_CALL_HEADER as LR + CCSP_KCALL_RETURN_BUMP_BYTES (= +8
    //       on aarch64), so the dispatch path lands two instructions
    //       past the natural-return point.  In this path x28 may
    //       hold anything (the previously-running process left
    //       something there), but sp == wptr (from dispatch's mov).
    //       We recover x28 from sp.
    //
    // To keep the natural path from clobbering its own x28, we put a
    // branch at the natural-return point that jumps over the
    // dispatch-only x28-recovery instruction.  CCSP_KCALL_RETURN_BUMP_BYTES
    // accounts for the two instructions between LR and the resume entry.
    b 1f                  // (a) natural: skip dispatch-only x28 recovery
    nop                   //     padding so iptr lands on the next insn
    mov x28, sp           // (b) dispatch entry: x28 = wptr (recovered from sp)
1:
    // If the process was migrated to a different scheduler during the
    // rescheduling call, x25 now points to the NEW scheduler.
    // Update wptr[SchedPtr] so the CIF C code picks up the correct
    // scheduler for subsequent kernel calls.
    stur x25, [x28, #-56]  // wptr[SchedPtr] = current sched (x25)

    // Restore callee-saved registers from the WORKSPACE.
    ldur x9, [x28, #-80]   // restore SP from wptr[-10]
    mov sp, x9

    ldur x29, [x28, #-96]
    ldur x30, [x28, #-88]
    ldur x19, [x28, #-104]
    ldur x20, [x28, #-112]
    ldur x21, [x28, #-120]
    ldur x22, [x28, #-128]
    ldur x23, [x28, #-136]
    ldur x24, [x28, #-144]
    ldur x25, [x28, #-152]
    ldur x26, [x28, #-160]
    ldur x27, [x28, #-168]
    // x28 restored LAST since we need it for addressing above
    ldur x28, [x28, #-176]

    br x30

// void ccsp_cif_process_call(void *wptr, void *func)
//
// Wrapper for calling a CIF C function on a PRIVATE stack in the workspace.
// The sched->stack is shared by all processes, so we can't let the CIF
// function's callee-saved register saves (especially LR/x30) live on it
// — other processes' C calls would overwrite them during context switches.
//
// This function:
//   1. Saves current SP (sched->stack) to wptr[-5] (byte -40)
//   2. Switches SP to a private stack at wptr - 248, 16-byte aligned
//   3. Calls func(wptr) — the CIF C function with wptr as x0
//   4. Restores SP from wptr[-5]
//   5. Returns to the occam code
//
#if defined(__APPLE__)
.global _ccsp_cif_process_call
_ccsp_cif_process_call:
#else
.global ccsp_cif_process_call
ccsp_cif_process_call:
#endif
    // x0 = wptr (Wptr+1, passed by compose_cif_call)
    // x1 = func address
    // Phase 4D: sp = kernel stack (from Phase 4D bracket in compose_cif_call)
    // x30 = LR (return address to compose_cif_call code)

    // Phase 4D: save wptr (x0) in x28 (callee-saved, survives blr x1).
    // In Phase 4D, x28 is a general-purpose register (Wptr = sp), so
    // we're free to use it as scratch/save here.
    mov x28, x0

    // Strip PAC bits from LR before saving - see explanation in
    // ccsp_cif_call_asm_resched above.  libc-compiled callers may
    // sign LR with paciasp; we save the bare code address instead.
    hint #7        // xpaclri: strip PAC from x30

    // Save LR to wptr[-23] (byte -184 from x0).
    // blr x1 will overwrite x30, and the CIF C function's channel
    // calls go through ccsp_cif_call_asm_resched which saves/restores
    // x30 from its own workspace slots.  After all kernel scheduling,
    // x30 will hold the asm_resched's LR, not our original LR.
    // We use offset -23 because:
    //   wptr[-1..-9]  = kernel workspace (Iptr, Link, Pointer, SchedPtr, etc.)
    //   wptr[-10..-22] = ccsp_cif_call_asm_resched callee-saved register saves
    //   wptr[-23]     = our LR save (safe, within CIF_PROCESS_WORDS=24)
    stur x30, [x0, #-184]

    // Save current SP (= sched->stack) to wptr[-5] (byte -40 from x0)
    mov x9, sp
    stur x9, [x0, #-40]

    // Set up private stack at wptr - 248, aligned to 16 bytes.
    // wptr = x0 = Wptr + 8.  So private stack = Wptr + 8 - 248 = Wptr - 240.
    // The CIF asm stub's workspace saves are at wptr[-10..-22] = bytes
    // -80 to -176 from wptr.  The private stack at -248 is below all saves.
    sub x9, x0, #248
    and x9, x9, #-16            // 16-byte align
    mov sp, x9                  // switch to private stack

    // Call the CIF C function: func(wptr)
    blr x1

    // CIF function returned.  Restore SP and LR from workspace.
    // x28 was set to wptr at entry and is callee-saved through blr x1.
    // Phase 4D: x28 holds wptr (not Wptr), wptr = Wptr + 8.
    mov x9, x28                 // x9 = wptr
    ldur x30, [x9, #-184]       // restore LR from wptr[-23]
    // Load the CURRENT scheduler's stack via wptr[SchedPtr].
    // wptr[SchedPtr] (byte -56) is always updated by
    // ccsp_cif_call_asm_resched after each rescheduling call.
    ldur x9, [x9, #-56]         // x9 = wptr[SchedPtr] = current sched
    ldr x9, [x9]                // x9 = sched->stack
    mov sp, x9                  // restore to current scheduler's stack

    ret
