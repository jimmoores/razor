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
 * ccsp_cif_proc_stub_label  (Phase 2)
 * ccsp_cif_light_proc_stub_label
 *
 * Code-only labels whose addresses are returned by kernel_CIF_proc_stub
 * and kernel_CIF_light_proc_stub respectively.  Each is the entry point
 * for a CIF process: when the scheduler dispatches one, control lands
 * here with r14 = process Wptr (set by ccsp_dispatch_process), r15 =
 * sched.  The process workspace top (wptr[0]) holds [func, ws_arg].
 *
 * Sequence:
 *   1. Switch rsp to the process's private stack at wptr[0].
 *   2. Save sched into wptr[SchedPtr] (= -56) so the CIF C function
 *      can find it later via ccsp_cif_call_asm*.
 *   3. Pop func into rax, ws into rdi (System V 1st arg slot).
 *   4. Call func(ws) -- a normal C function on the private stack.
 *   5. After return, restore sched from wptr[SchedPtr] (in case the
 *      process migrated to a different scheduler thread).
 *   6. Set up the kernel ABI args (rdi=p0, rsi=sched, rdx=Wptr) for
 *      the follow-up kernel call.
 *   7. Switch rsp back to sched->stack so the kernel runs on the
 *      kernel stack.
 *   8. Tail-call kernel_Y_proc_end (proc_stub) or kernel_Y_endp
 *      (light_proc_stub).
 *
 * proc_stub passes p0 = wptr - CIF_PROCESS_WORDS*sizeof(word) = wptr-192
 * (the base of the workspace allocation, for mt_release_simple).
 * light_proc_stub passes p0 = *(wptr + BarrierPtr*sizeof(word))
 *   = *(wptr - 64) (the barrier pointer, dereferenced).
 *
 * The follow-up kernel call uses callq (not jmpq) so the rsp ≡ 8 (mod 16)
 * SysV alignment contract holds for the C-compiled kernel function;
 * the dead return address is harmless because the call never returns.
 */
.equ X64_CIF_PROCESS_BYTES, 192		/* CIF_PROCESS_WORDS * 8 */
.equ X64_BARRIER_PTR_BYTES, -64		/* BarrierPtr * 8 = -8*8 */
.equ X64_SCHED_PTR_BYTES, -56		/* SchedPtr * 8 = -7*8 */

.global ccsp_cif_proc_stub_label
.type ccsp_cif_proc_stub_label, @function
ccsp_cif_proc_stub_label:
	movq	(%r14), %rsp		/* rsp = wptr[0] (private stack top) */
	movq	%r15, X64_SCHED_PTR_BYTES(%r14)	/* wptr[SchedPtr] = sched */
	popq	%rax			/* rax = func address */
	popq	%rdi			/* rdi = ws (1st C arg) */
	callq	*%rax			/* call the CIF C function */
	movq	X64_SCHED_PTR_BYTES(%r14), %r15	/* restore sched (may have migrated) */
	movq	%r14, %rdi		/* rdi = wptr (will become p0) */
	movq	%r14, %rdx		/* rdx = wptr (Wptr arg) */
	movq	%r15, %rsi		/* rsi = sched (sched arg) */
	movq	(%r15), %rsp		/* rsp = sched->stack (16-byte aligned) */
	subq	$X64_CIF_PROCESS_BYTES, %rdi	/* p0 = wptr - CIF_PROCESS_WORDS*8 */
	callq	kernel_Y_proc_end	/* tail-call (alignment via callq+ud2) */
	ud2				/* unreachable */
.size ccsp_cif_proc_stub_label, . - ccsp_cif_proc_stub_label

.global ccsp_cif_light_proc_stub_label
.type ccsp_cif_light_proc_stub_label, @function
ccsp_cif_light_proc_stub_label:
	movq	(%r14), %rsp		/* rsp = wptr[0] (private stack top) */
	movq	%r15, X64_SCHED_PTR_BYTES(%r14)	/* wptr[SchedPtr] = sched */
	popq	%rax			/* rax = func address */
	popq	%rdi			/* rdi = ws (1st C arg) */
	callq	*%rax			/* call the CIF C function */
	movq	X64_SCHED_PTR_BYTES(%r14), %r15	/* restore sched */
	movq	%r14, %rdi		/* rdi = wptr */
	movq	%r14, %rdx		/* rdx = wptr (Wptr arg) */
	movq	%r15, %rsi		/* rsi = sched (sched arg) */
	movq	(%r15), %rsp		/* rsp = sched->stack */
	movq	X64_BARRIER_PTR_BYTES(%rdi), %rdi	/* p0 = *(wptr + BarrierPtr*8) */
	callq	kernel_Y_endp		/* tail-call */
	ud2				/* unreachable */
.size ccsp_cif_light_proc_stub_label, . - ccsp_cif_light_proc_stub_label

/*
 * ccsp_cif_endp_resume_label  (Phase 2)
 *
 * Code-only label whose address is returned by kernel_CIF_endp_resume_stub
 * and stored as the IptrSucc of every LightProc barrier.  When a child
 * process ends and the barrier count drops to zero, the parent resumes
 * here: load the parent's Wptr from wptr[-4] (= byte -32 from current
 * r14) and jump to its saved Iptr at wptr[-1] (= byte -8 from r14).
 *
 * This is NOT a callable function -- it has no prologue and is only
 * ever entered via Iptr-jump from the scheduler, with r14 already
 * set to the resuming process's Wptr by ccsp_dispatch_process.
 */
.global ccsp_cif_endp_resume_label
.type ccsp_cif_endp_resume_label, @function
ccsp_cif_endp_resume_label:
	movq	-32(%r14), %r14		/* r14 = parent Wptr */
	jmpq	*-8(%r14)		/* jump to parent Wptr[Iptr] */
.size ccsp_cif_endp_resume_label, . - ccsp_cif_endp_resume_label

/*
 * void ccsp_kernel_enter(void *init, void *stack, word *Wptr, word *Fptr) __attribute__((noreturn))
 *
 * Phase 2: kernel boot primitive.  Replaces the inline-asm K_ENTRY
 * macro called from ccsp_kernel_entry() in entry.c.  Switches the
 * stack pointer to the freshly-laid-out kernel stack and tail-calls
 * `init` (typically kernel_Y_rtthreadinit) with the legacy 3-arg
 * runtime convention:  init(stack, Fptr, Wptr) where stack→p0,
 * Fptr→sched, Wptr→Wptr.
 *
 * Arguments (System V x64 ABI):
 *   rdi = init   (function to call)
 *   rsi = stack  (kernel stack base; becomes p0 of init)
 *   rdx = Wptr   (initial process workspace)
 *   rcx = Fptr   (initial run-queue front pointer; becomes sched of init)
 */
.global ccsp_kernel_enter
.type ccsp_kernel_enter, @function
ccsp_kernel_enter:
	movq	%rdi, %r10		/* save init in scratch */
	movq	%rsi, %rsp		/* sp = stack (16-byte aligned by entry.c) */
	movq	%rsi, %rdi		/* rdi = stack (p0 of init) */
	movq	%rcx, %rsi		/* rsi = Fptr (sched of init) */
	/* rdx = Wptr (already correct) */
	/* Use callq (not jmpq) so the call instruction pushes a fake
	 * return address: x64 SysV ABI expects rsp ≡ 8 (mod 16) on
	 * entry to a function.  We're noreturn so the return address
	 * is dead, but the alignment must be correct or libc functions
	 * (notably pthread_create's internal alignment-sensitive code)
	 * will SIGSEGV. */
	callq	*%r10			/* call init (never returns) */
	ud2				/* unreachable */
.size ccsp_kernel_enter, . - ccsp_kernel_enter

/*
 * void ccsp_dispatch_process(sched_t *sched, word *Wptr) __attribute__((noreturn))
 *
 * Phase 2: kernel-to-user dispatch primitive.  Replaces the inline-asm
 * K_ZERO_OUT_JRET macro at all ~13 call sites in sched.c.  This function
 * never returns to its caller.
 *
 * Sets the runtime register convention for the dispatched process,
 * restores the kernel SP from sched->stack, and jumps to the saved
 * instruction pointer at Wptr[-1] (= byte -8 from Wptr).
 *
 * Arguments (System V x64 ABI):
 *   rdi = sched
 *   rsi = Wptr
 */
.global ccsp_dispatch_process
.type ccsp_dispatch_process, @function
ccsp_dispatch_process:
	movq	%rsi, %r14		/* r14 = Wptr (runtime convention) */
	movq	%rdi, %r15		/* r15 = sched (runtime convention) */
	movq	(%r15), %rsp		/* rsp = sched->stack */
	jmpq	*-8(%r14)		/* jump to Wptr[Iptr] */
.size ccsp_dispatch_process, . - ccsp_dispatch_process

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
	/* Save callee-saved regs to workspace at negative offsets from wptr (rdi).
	 * r14 (= Wptr from occam code) differs from rdi (= wptr = Wptr+8).
	 * After the kernel call, r14 could be either Wptr (non-rescheduling,
	 * preserved by kernel) or wptr (rescheduling, K_ZERO_OUT_JRET sets
	 * r14 = rdx = wptr).  To handle both cases, we save wptr at a fixed
	 * offset and use it to find the restore base. */
	movq	%rbp, -96(%rdi)		/* wptr[-12] */
	movq	%rbx, -88(%rdi)		/* wptr[-11] */
	movq	%r12, -104(%rdi)	/* wptr[-13] */
	movq	%r13, -112(%rdi)	/* wptr[-14] */
	movq	%r14, -120(%rdi)	/* wptr[-15] */
	movq	%r15, -128(%rdi)	/* wptr[-16] */
	movq	%rdi, -136(%rdi)	/* wptr[-17] = wptr itself */

	/* Save current SP (= private stack) to wptr[-10] (byte -80) */
	movq	%rsp, -80(%rdi)

	/* Switch to kernel stack */
	movq	(%rsi), %rsp		/* rsp = sched->stack */

	/* Set r14 = wptr (from rdi) so saves and restores use the same
	 * base.  The kernel preserves r14 (callee-saved) for non-
	 * rescheduling returns.  For rescheduling returns (K_ZERO_OUT_JRET),
	 * the kernel sets r14 = Wptr = wptr (from rdx, which we set below).
	 * Either way, r14 = wptr after the kernel call.
	 *
	 * The caller (CIF function) saved r14 before calling kcall_resched
	 * (push in prologue or callee-saved convention).  Setting r14 here
	 * is invisible to the caller since r14 is saved/restored by
	 * kcall_resched itself (at rdi[-15] offset). */
	movq	%rdi, %r14		/* r14 = wptr */

	/* Set up kernel call args: param0=rdi, sched=rsi, Wptr=rdx */
	movq	%rdi, %r8		/* save wptr */
	movq	%rdx, %rdi		/* rdi = param0 */
	movq	%r8, %rdx		/* rdx = wptr (= Wptr for kernel) */
	/* rsi = sched (already correct) */

	/* Pre-load r15 = sched so the post-call store of r15 to
	 * wptr[SchedPtr] is correct in BOTH paths:
	 *   (a) Kernel reschedules via K_ZERO_OUT_JRET: r15 is overwritten
	 *       with the (possibly migrated) current sched.
	 *   (b) Kernel returns normally (e.g. kernel_Y_mt_lock when the
	 *       lock is free): r15 is preserved across the call as a
	 *       callee-saved register, so it still holds sched.
	 * Without this, the caller's r15 (which GCC may use to cache an
	 * unrelated value such as a hoisted function pointer) would leak
	 * into wptr[SchedPtr] in path (b), corrupting the workspace. */
	movq	%rsi, %r15		/* r15 = sched */

	/* Call kernel function */
	callq	*%rcx
	/* After K_ZERO_OUT_JRET: r14=wptr, r15=current sched.
	 * If the process was migrated to a different scheduler during
	 * the rescheduling call, r15 now points to the NEW scheduler.
	 * Update wptr[SchedPtr] so the CIF C code picks up the correct
	 * scheduler for subsequent kernel calls. */
	movq	%r15, -56(%r14)		/* wptr[SchedPtr] = current sched */
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
	/* Save rbx (callee-saved) and use it to hold the original rsp.
	 * r8-r11 are caller-saved and may be clobbered by the kernel. */
	pushq	%rbx
	movq	%rsp, %rbx		/* save current rsp in callee-saved rbx */
	movq	(%rsi), %rsp		/* switch to kernel stack */
	movq	%rdi, %r9		/* save wptr */
	movq	%rdx, %rdi		/* rdi = param0 */
	movq	%r9, %rdx		/* rdx = wptr */
	/* rsi = sched (already correct) */
	callq	*%rcx
	movq	%rbx, %rsp		/* restore rsp from callee-saved rbx */
	popq	%rbx			/* restore rbx */
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
	 * Restore state from workspace.
	 * Load the CURRENT scheduler's stack via wptr[SchedPtr].
	 * If the CIF process migrated during a rescheduling kernel call,
	 * the original saved SP (wptr[-5]) points to the wrong scheduler.
	 * wptr[SchedPtr] (byte -56) is always updated after each resched. */
	movq	-56(%rbx), %rsp		/* rsp = wptr[SchedPtr] = current sched */
	movq	(%rsp), %rsp		/* rsp = sched->stack */
	movq	-184(%rbx), %rax	/* restore return address from wptr[-23] */
	pushq	%rax			/* put return address back on stack */
	movq	-216(%rbx), %r13	/* restore r13 from wptr[-27] */
	movq	-208(%rbx), %r12	/* restore r12 from wptr[-26] */
	movq	-200(%rbx), %rbp	/* restore rbp from wptr[-25] */
	movq	-192(%rbx), %rbx	/* restore rbx from wptr[-24] */
	retq				/* return to caller */
.size ccsp_cif_process_call, . - ccsp_cif_process_call

.section .note.GNU-stack,"",@progbits
