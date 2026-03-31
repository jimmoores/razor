.text
.align 2

// word ccsp_cif_call_asm(void *wptr, void *sched, void *param0, void *func)
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

    // Switch to kernel stack
    ldr x9, [x1, #0]
    mov sp, x9

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
#if defined(__APPLE__)
.global _ccsp_cif_call_asm_resched
_ccsp_cif_call_asm_resched:
#else
.global ccsp_cif_call_asm_resched
ccsp_cif_call_asm_resched:
#endif
    stp x29, x30, [sp, #-16]!
    mov x29, sp

    // Save ALL callee-saved registers so they survive a KRoC context switch!
    stp x19, x20, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    stp x23, x24, [sp, #-16]!
    stp x25, x26, [sp, #-16]!
    stp x27, x28, [sp, #-16]!

    str x1, [x0, #-56]    // wptr[-7] = sched (SchedPtr)

    mov x28, x0           // x28 = wptr

    // setup KRoC kernel arguments: x0=param0, x1=sched, x2=wptr
    mov x8, x0            // x8 = wptr (saved)
    mov x0, x2            // x0 = param0
    mov x2, x8            // x2 = wptr
    // x1 is already sched

    // Save current C stack pointer at Wptr[-10] (byte offset -80).
    mov x9, sp
    stur x9, [x28, #-80]  // wptr[-10] = sp

    // Switch to the kernel stack for the kernel call.
    // sched->stack points well below the CIF C stack frames, so the
    // kernel frames won't collide with our saved callee-saved registers.
    ldr x9, [x1, #0]      // x9 = sched->stack
    mov sp, x9

    // Call KRoC function
    blr x3

    // After return from kernel (possibly after context switch):
    // K_ZERO_OUT_JRET restores x28=Wptr.  The kernel received Wptr from
    // x2 which was set to x0 (= wptr = x28+8 at entry).  So x28 here
    // equals x28 at entry (both are wptr = original_x28+8).
    // The C SP was saved at [x28, #-80] above.  Restore from same offset:
    ldur x9, [x28, #-80]
    mov sp, x9

    // Reload sched.  The asm stub entry saved sched at [x0, #-56] where
    // x0 = wptr.  K_ZERO_OUT_JRET restores x28 = Wptr = wptr (same value).
    // So reload from [x28, #-56]:
    ldur x1, [x28, #-56]

    // Restore ALL callee-saved registers
    ldp x27, x28, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16

    ldp x29, x30, [sp], #16
    // Use br x30 instead of ret to bypass PAC verification
    br x30
