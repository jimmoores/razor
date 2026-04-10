/*
 *	x64 CIF (C Interface) assembly support routines
 *	Based on aarch64_cif.S
 *
 *	Register convention (matching tranx86 archx64.c):
 *	  r14 = Wptr (Workspace Pointer)
 *	  r15 = Sched (Scheduler Pointer)
 *	  r13 = Fptr (Front Pointer)
 *	  r12 = Bptr (Back Pointer)
 *
 *	Kernel call convention (System V AMD64 ABI):
 *	  param0 -> rdi, sched -> rsi, Wptr -> rdx
 *	  return -> rax
 */

.text
.align 8

/*
 * void ccsp_cif_process_call(void *wptr, void *func)
 *
 * Wrapper for calling a CIF C function on a private stack in the workspace.
 * The sched->stack is shared by all processes, so we cannot let the CIF
 * function's C stack frame live on it -- other processes' C calls would
 * overwrite it during context switches.
 *
 * This function:
 *   1. Saves current rsp (= sched->stack) to wptr[-5] (byte -40)
 *   2. Saves return address to wptr[-23] (byte -184)
 *   3. Saves rbx to wptr[-24] (byte -192)
 *   4. Switches rsp to a private stack at wptr - 256, 16-byte aligned
 *   5. Calls func(wptr) -- the CIF C function with wptr as rdi
 *   6. Restores rbx, rsp, and returns
 *
 * Arguments (System V ABI):
 *   rdi = wptr (workspace pointer + 1 word)
 *   rsi = func (function address to call)
 */
.global ccsp_cif_process_call
.type ccsp_cif_process_call, @function
/*
 * word ccsp_cif_x64_kcall_resched(void *wptr, void *sched, word param0, void *func)
 *
 * Rescheduling kernel call from CIF C code.  Saves all callee-saved
 * registers to the workspace (private to this process), switches to the
 * kernel stack, calls the kernel function, then restores after reschedule.
 *
 * Arguments (System V ABI):
 *   rdi = wptr (CIF workspace pointer)
 *   rsi = sched (scheduler pointer)
 *   rdx = param0 (first kernel call argument)
 *   rcx = func (calltable function pointer)
 */
.global ccsp_cif_x64_kcall_resched
.type ccsp_cif_x64_kcall_resched, @function
ccsp_cif_x64_kcall_resched:
	/* Save callee-saved regs to workspace at negative offsets from wptr (rdi) */
	movq	%rbp, -96(%rdi)		/* wptr[-12] */
	movq	%rbx, -88(%rdi)		/* wptr[-11] */
	movq	%r12, -104(%rdi)	/* wptr[-13] */
	movq	%r13, -112(%rdi)	/* wptr[-14] */
	movq	%r14, -120(%rdi)	/* wptr[-15] */
	movq	%r15, -128(%rdi)	/* wptr[-16] */

	/* Save current SP (= private stack) to wptr[-10] (byte -80) */
	movq	%rsp, -80(%rdi)

	/* Switch to kernel stack */
	movq	(%rsi), %rsp		/* rsp = sched->stack */

	/* Set up kernel call args: param0=rdi, sched=rsi, Wptr=rdx */
	/* param0 is already in rdx, but we need to shuffle:
	 *   rdi(param0) -> rdi  (stays)
	 *   rsi(sched)  -> rsi  (stays)
	 *   rdx(param0) -> rdi, rsi(sched) -> rsi, rdi(wptr) -> rdx */
	movq	%rdi, %r8		/* save wptr */
	movq	%rdx, %rdi		/* rdi = param0 */
	movq	%r8, %rdx		/* rdx = wptr (= Wptr for kernel) */
	/* rsi = sched (already correct) */

	/* Call kernel function */
	callq	*%rcx

	/* After return (possibly after context switch via K_ZERO_OUT_JRET):
	 * r14 = Wptr (set by K_ZERO_OUT_JRET).  For CIF calls, the
	 * kernel's Wptr IS the CIF wptr we passed in. */

	/* Use r14 (= wptr, set by K_ZERO_OUT_JRET) to restore everything */
	movq	-80(%r14), %rsp		/* restore SP from wptr[-10] */
	movq	-96(%r14), %rbp
	movq	-128(%r14), %r15
	movq	-112(%r14), %r13
	movq	-104(%r14), %r12
	movq	-88(%r14), %rbx
	movq	-120(%r14), %r14	/* restore r14 LAST (used for addressing) */
	ret
.size ccsp_cif_x64_kcall_resched, . - ccsp_cif_x64_kcall_resched

/* Non-rescheduling variant: simple call, no context switch possible */
.global ccsp_cif_x64_kcall
.type ccsp_cif_x64_kcall, @function
ccsp_cif_x64_kcall:
	/* Simple: switch to kernel stack, call, switch back */
	movq	%rsp, %r8		/* save current rsp */
	movq	(%rsi), %rsp		/* switch to kernel stack */
	movq	%rdi, %r9		/* save wptr */
	movq	%rdx, %rdi		/* rdi = param0 */
	movq	%r9, %rdx		/* rdx = wptr */
	/* rsi = sched (already correct) */
	callq	*%rcx
	movq	%r8, %rsp		/* restore rsp */
	ret
.size ccsp_cif_x64_kcall, . - ccsp_cif_x64_kcall

ccsp_cif_process_call:
	/* Save ALL callee-saved registers to the workspace.
	 * The occam runtime uses r12 (Bptr), r13 (Fptr), r14 (Wptr),
	 * r15 (sched), and rbp for its own purposes.  The C function
	 * expects these to be preserved per the System V ABI. */
	movq	%rbx, -192(%rdi)	/* wptr[-24] */
	movq	%rbp, -200(%rdi)	/* wptr[-25] */
	movq	%r12, -208(%rdi)	/* wptr[-26] */
	movq	%r13, -216(%rdi)	/* wptr[-27] */

	/* Pop return address into rbx temporarily, save to wptr[-23] (byte -184) */
	popq	%rbx
	movq	%rbx, -184(%rdi)

	/* Save current rsp (= sched->stack) to wptr[-5] (byte -40) */
	movq	%rsp, -40(%rdi)

	/* Save wptr in rbx (callee-saved, survives the call) */
	movq	%rdi, %rbx

	/* Switch to private stack at wptr - 280, 16-byte aligned.
	 * Must be below all workspace saves (wptr[-27] = -216). */
	leaq	-280(%rdi), %rsp
	andq	$-16, %rsp

	/* Call the CIF C function: func(wptr)
	 * rdi already has wptr, rsi has func address */
	callq	*%rsi

	/* CIF function returned.  rbx = wptr (callee-saved).
	 * Restore state from workspace. */
	movq	-40(%rbx), %rsp		/* restore rsp from wptr[-5] */
	movq	-184(%rbx), %rax	/* restore return address from wptr[-23] */
	pushq	%rax			/* put return address back on stack */
	movq	-216(%rbx), %r13	/* restore r13 from wptr[-27] */
	movq	-208(%rbx), %r12	/* restore r12 from wptr[-26] */
	movq	-200(%rbx), %rbp	/* restore rbp from wptr[-25] */
	movq	-192(%rbx), %rbx	/* restore rbx from wptr[-24] */
	retq				/* return to caller */
.size ccsp_cif_process_call, . - ccsp_cif_process_call

.section .note.GNU-stack,"",@progbits
