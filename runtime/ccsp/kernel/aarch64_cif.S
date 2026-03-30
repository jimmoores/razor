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

    mov sp, x0            // switch to wptr

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

    str x1, [x0, #-56]    // wptr[-7] = sched

    mov x28, x0           // x28 = wptr

    // setup KRoC kernel arguments: x0=param0, x1=sched, x2=wptr
    mov x8, x0            // x8 = wptr
    mov x0, x2            // x0 = param0
    mov x2, x8            // x2 = wptr
    // x1 is already sched

    // Save current C stack pointer into wptr[0]
    mov x9, sp
    str x9, [x28, #0]     // wptr[0] = sp

    // Switch stack to KRoC kernel stack (sched)
    mov sp, x1

    // Call KRoC function
    blr x3

    // Restore C stack pointer from wptr[0].
    // If we are rescheduled, KRoC sets x28 to wptr before jumping here!
    ldr x9, [x28, #0]
    mov sp, x9

    // Reload sched from wptr[-7] (which might have changed)
    ldr x1, [x28, #-56]

    // Restore ALL callee-saved registers
    ldp x27, x28, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16

    ldp x29, x30, [sp], #16
    ret
