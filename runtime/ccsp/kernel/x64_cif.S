/*
 *	x64 CIF (C Interface) assembly support routines
 *	Based on aarch64_cif.S
 *
 *	Register convention (matching tranx86 archx64.c):
 *	  rsp = Wptr (Workspace Pointer) -- Phase 4D: unified with SP
 *	  r15 = Sched (Scheduler Pointer)
 *	  r13 = Fptr (Front Pointer)
 *	  r12 = Bptr (Back Pointer)
 *	  r14 = allocatable (freed by Phase 4D)
 *
 *	Kernel call convention (System V AMD64 ABI):
 *	  param0 -> rdi, sched -> rsi, Wptr -> rdx
 *	  return -> rax
 */

.text
.align 8

/*
 * void ccsp_cif_occam_call(void *sched, void *stack, word *ws, void *func, word top)
 *
 * Phase 2 / Phase 4D: CIF -> occam call helper used by the OccamCall
 * macro in ccsp_cif.h.  Sets up an occam call frame on the workspace
 * `ws`, jumps to `func` (occam-compiled code), and resumes when occam
 * returns via I_RET back to the saved Iptr.
 *
 * Phase 4D: occam code now uses rsp as Wptr.  We switch rsp to ws
 * before jumping to occam.  On return from occam, rsp has been
 * advanced by 32 bytes (4 words) by I_RET; we undo that, then
 * recover our saved C stack pointer from the save area.
 *
 * Arguments (System V x64 ABI):
 *   rdi = sched (unused -- present for API symmetry)
 *   rsi = stack (unused -- present for API symmetry)
 *   rdx = ws    (workspace pointer)
 *   rcx = func  (occam function to jump to)
 *   r8  = top   (number of args + 1; ws[top..top+1] is the save area)
 */
.global ccsp_cif_occam_call
.type ccsp_cif_occam_call, @function
ccsp_cif_occam_call:
	/* Save callee-saved registers + 1 dummy push for 16-byte alignment.
	 * Function entry has rsp == 8 (mod 16); after 7 pushes (56 bytes)
	 * rsp == 0 (mod 16) which is what occam expects. */
	pushq	%rbx
	pushq	%rbp
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	pushq	%rax			/* alignment padding */

	/* Compute save_area = ws + top*8 */
	shlq	$3, %r8			/* r8 = top * 8 */
	addq	%rdx, %r8		/* r8 = ws + top*8 (save_area) */

	movq	%rsp, (%r8)		/* save_area[0] = current C SP */
	movq	%r8, -80(%rdx)		/* ws[-10] = save_area address */

	/* Set up the occam return label so I_RET lands inside us */
	leaq	.Loccam_call_return(%rip), %r9
	movq	%r9, (%rdx)		/* ws[0] = return label (IptrSucc) */
	movq	%r9, -8(%rdx)		/* ws[-1] = return label (Iptr) */

	/* Phase 4D: switch rsp to ws (Wptr = rsp in occam code) */
	movq	%rdx, %rsp		/* rsp = ws (Wptr) */

	/* Set sched runtime register and jump to occam */
	movq	%rdi, %r15		/* r15 = sched */
	jmpq	*%rcx			/* jump to occam func */

.Loccam_call_return:
	/* Occam returned via I_RET, which advanced rsp by 32 bytes
	 * (4 words) for the I_CALL frame that OccamCall didn't push.
	 * Undo that to get back to ws. */
	subq	$32, %rsp

	/* Reload the save_area address from ws[-10] and restore C SP */
	movq	-80(%rsp), %r8
	movq	(%r8), %rsp

	/* Pop callee-saved regs (matching the prologue's order) */
	popq	%rax			/* discard alignment padding */
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
	ret
.size ccsp_cif_occam_call, . - ccsp_cif_occam_call

/*
 * void ccsp_cif_jump(word *wptr, void *addr) __attribute__((noreturn))
 *
 * Phase 2 / Phase 4D: CIF -> occam transition primitive.  Used by ProcEnd
 * to leave a "called from occam" CIF process by jumping back to the
 * parent's escape pointer.  Sets rsp = wptr (Phase 4D: Wptr = rsp)
 * and tail-jumps to addr.
 *
 * Arguments (System V x64 ABI):
 *   rdi = wptr
 *   rsi = addr
 */
.global ccsp_cif_jump
.type ccsp_cif_jump, @function
ccsp_cif_jump:
	movq	%rdi, %rsp		/* Phase 4D: rsp = Wptr */
	jmpq	*%rsi			/* tail-jump to occam code */
.size ccsp_cif_jump, . - ccsp_cif_jump

/*
 * word ccsp_cif_external_call(void *func, void *stack)
 *
 * Phase 2: external C call helper.  Used by ExternalCall0/1/N for
 * blocking syscalls that need to run on a separate stack so the
 * occam runtime registers (r12-r15) and stack are preserved.
 *
 * Saves the caller's rsp, switches to `stack`, calls func() (with
 * any args already placed on the stack by the caller), switches rsp
 * back, and returns whatever func returned in rax.
 *
 * Arguments:
 *   rdi = func   (function pointer to call)
 *   rsi = stack  (stack to switch to)
 * Returns:
 *   rax = func's return value
 *
 * Preserves the runtime registers (r12-r15, rbp) by saving them on
 * the C caller's stack before the switch.  func() may freely clobber
 * any caller-saved register.
 */
.global ccsp_cif_external_call
.type ccsp_cif_external_call, @function
ccsp_cif_external_call:
	pushq	%rbp			/* save callee-saved regs */
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	pushq	%rbx			/* 6 pushes -> 16-byte aligned */
	movq	%rsp, %rbx		/* save current sp in callee-saved rbx */
	movq	%rsi, %rsp		/* switch to provided stack */
	callq	*%rdi			/* call func() */
	movq	%rbx, %rsp		/* restore sp */
	popq	%rbx			/* restore callee-saved regs */
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	retq				/* result already in rax */
.size ccsp_cif_external_call, . - ccsp_cif_external_call

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

/*
 * Phase 4D note: dispatch sets rsp = Wptr (not r14).  These stubs
 * are entered with rsp = wptr.  We save wptr in a scratch register
 * (r14 is now allocatable and not guaranteed), then use the private
 * stack for the CIF C function call.
 */
.global ccsp_cif_proc_stub_label
.type ccsp_cif_proc_stub_label, @function
ccsp_cif_proc_stub_label:
	/* rsp = wptr on entry (set by dispatch).  Save it in r14 (scratch). */
	movq	%rsp, %r14		/* r14 = wptr (temporary) */
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
	/* rsp = wptr on entry (set by dispatch).  Save it in r14 (temporary). */
	movq	%rsp, %r14		/* r14 = wptr (temporary) */
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
	/* Phase 4D: rsp = Wptr on entry (set by dispatch).
	 * Load parent Wptr from wptr[-4] (byte -32) into rsp,
	 * then jump to parent's Iptr at wptr[-1] (byte -8). */
	movq	-32(%rsp), %rsp		/* rsp = parent Wptr */
	jmpq	*-8(%rsp)		/* jump to parent Wptr[Iptr] */
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
	/* Phase 4D: Wptr = rsp.  Set rsp = Wptr (user mode),
	 * r15 = sched, and jump to Wptr[Iptr]. */
	movq	%rdi, %r15		/* r15 = sched (runtime convention) */
	movq	%rsi, %rsp		/* rsp = Wptr (Phase 4D: Wptr IS rsp) */
	jmpq	*-8(%rsp)		/* jump to Wptr[Iptr] */
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
	 * Phase 4D: r14 is now allocatable (not Wptr).  We use it as a
	 * scratch register to hold wptr during the kernel call, since
	 * GCC preserves r14 (callee-saved) for non-rescheduling returns. */
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

	/* Set r14 = wptr for post-call addressing.
	 * Non-rescheduling: GCC preserves r14 (callee-saved).
	 * Rescheduling: dispatch sets rsp = Wptr; the BUMP shim below
	 * recovers r14 = wptr from rsp. */
	movq	%rdi, %r14		/* r14 = wptr */

	/* Set up kernel call args: param0=rdi, sched=rsi, Wptr=rdx */
	movq	%rdi, %r8		/* save wptr */
	movq	%rdx, %rdi		/* rdi = param0 */
	movq	%r8, %rdx		/* rdx = wptr (= Wptr for kernel) */
	/* rsi = sched (already correct) */

	/* Pre-load r15 = sched so the post-call store of r15 to
	 * wptr[SchedPtr] is correct in BOTH paths:
	 *   (a) Kernel reschedules via ccsp_dispatch_process: r15 is set
	 *       by dispatch to the (possibly migrated) current sched.
	 *   (b) Kernel returns normally: r15 is preserved across the call
	 *       as a callee-saved register. */
	movq	%rsi, %r15		/* r15 = sched */

	/* Call kernel function */
	callq	*%rcx

	/* Phase 4D: BUMP-compatible post-call sequence.
	 * K_CALL_HEADER stores return_address + CCSP_KCALL_RETURN_BUMP_BYTES
	 * (= 4) as the resume Iptr.  The next 4 bytes are for the
	 * non-reschedule path only; BUMP skips them for reschedule.
	 *
	 * Non-reschedule: r14 = wptr (preserved), rsp = kernel stack.
	 *   The jmp skips the movq shim and falls through to restore.
	 * Reschedule: dispatch sets rsp = Wptr = wptr, r14 = unknown.
	 *   BUMP lands at the movq which sets r14 = rsp = wptr. */
	jmp	.Lresched_common	/* 2 bytes: eb XX */
	.byte	0x90, 0x90		/* 2 bytes: nop padding to fill 4-byte slot */
	/* --- BUMP=4 resume point for reschedule --- */
	movq	%rsp, %r14		/* reschedule: r14 = wptr (rsp = Wptr) */
.Lresched_common:
	/* Both paths: r14 = wptr, r15 = current sched */
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
	/* Phase 4D: entered from kernel stack (caller did save/switch).
	 * Save ALL callee-saved registers to the workspace.
	 * The occam runtime uses r12 (Bptr), r13 (Fptr),
	 * r15 (sched), and rbp for its own purposes.  The C function
	 * expects these to be preserved per the System V ABI. */
	movq	%rbx, -192(%rdi)	/* wptr[-24] */
	movq	%rbp, -200(%rdi)	/* wptr[-25] */
	movq	%r12, -208(%rdi)	/* wptr[-26] */
	movq	%r13, -216(%rdi)	/* wptr[-27] */

	/* Pop return address into rbx temporarily, save to wptr[-23] (byte -184) */
	popq	%rbx
	movq	%rbx, -184(%rdi)

	/* Save current rsp (= kernel stack after pop) to wptr[-5] (byte -40) */
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
