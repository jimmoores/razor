.text
.align 2

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

    // Save current C stack pointer
    mov x9, sp
    stur x9, [x0, #-80]   // wptr[-10] = sp

    // Save sched at wptr[-7] (SchedPtr)
    str x1, [x0, #-56]

    mov x28, x0           // x28 = wptr

    // setup KRoC kernel arguments: x0=param0, x1=sched, x2=wptr
    mov x8, x28           // x8 = wptr
    mov x0, x2            // x0 = param0
    mov x2, x8            // x2 = wptr
    // x1 is already sched

    // Call KRoC function (runs on current C stack = sched->stack)
    blr x3

    // After return from kernel (possibly after context switch):
    // K_ZERO_OUT_JRET restores x28=Wptr (= wptr from entry).
    // Restore callee-saved registers from the WORKSPACE.
    ldur x9, [x28, #-80]   // restore SP from wptr[-10]
    mov sp, x9

    ldur x1, [x28, #-56]   // reload sched from wptr[-7]

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
