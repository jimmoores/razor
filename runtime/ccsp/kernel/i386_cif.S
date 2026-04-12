/*
 *	i386 CCSP runtime assembly support routines
 *
 *	Currently contains only the ccsp_dispatch_process kernel->user
 *	dispatch primitive (Phase 2).  The CIF process call path on i386
 *	is still inline-asm in sched_asm_inserts.h; only K_ZERO_OUT_JRET
 *	has been moved out so all 13 sched.c call sites share a single
 *	function definition.
 *
 *	Register convention (matches arch386.c / sched_asm_inserts.h):
 *	  ebp = Wptr (workspace pointer)
 *	  esi = sched (scheduler pointer)
 *	  esp = kernel stack pointer (= sched->stack while in kernel C)
 */

.text
.align 4

/*
 * ccsp_cif_proc_stub_label  (Phase 2)
 * ccsp_cif_light_proc_stub_label
 *
 * CIF process trampoline labels.  Same idea as the x64 / aarch64
 * versions: when the scheduler dispatches a CIF process, control
 * lands here with ebp = process Wptr, esi = sched.  The process
 * workspace top (wptr[0]) holds [func, ws_arg].  We:
 *   1. Switch esp to wptr[0] (private stack)
 *   2. Save sched into wptr[SchedPtr] (= -28 on 32-bit)
 *   3. Pop func into eax (ws is left at 0(%esp) for the cdecl call)
 *   4. Call func -- cdecl reads ws from 4(%esp) after the call pushes
 *      its 4-byte return address
 *   5. Restore sched from wptr[SchedPtr]
 *   6. Set up the regparm(3) kernel ABI args (eax=p0, edx=sched, ecx=Wptr)
 *   7. Switch esp back to sched->stack
 *   8. Tail-call kernel_Y_proc_end / kernel_Y_endp
 *
 * proc_stub:        p0 = wptr - CIF_PROCESS_WORDS*4 = wptr - 32
 * light_proc_stub:  p0 = *(wptr + BarrierPtr*4) = *(wptr - 32)
 *
 * Stack-alignment: i386 cdecl wants esp ≡ 12 (mod 16) on entry to a
 * function.  After `popl %eax` (rsp += 4), esp ≡ -4 (= 12 mod 16) ≡
 * 12 (mod 16), so a `call` to the C function maintains alignment.
 * For the kernel call, after switching to sched->stack (16-byte
 * aligned, ≡ 0 mod 16), use `call` (not `jmp`) so esp inside the
 * kernel function is ≡ 12 (mod 16).
 */
.equ I386_CIF_PROCESS_BYTES, 32		/* CIF_PROCESS_WORDS * 4 = 8*4 */
.equ I386_BARRIER_PTR_BYTES, -32	/* BarrierPtr * 4 = -8*4 */
.equ I386_SCHED_PTR_BYTES, -28		/* SchedPtr * 4 = -7*4 */

.globl ccsp_cif_proc_stub_label
.type ccsp_cif_proc_stub_label, @function
ccsp_cif_proc_stub_label:
	movl	(%ebp), %esp		/* esp = wptr[0] (private stack top) */
	movl	%esi, I386_SCHED_PTR_BYTES(%ebp)	/* wptr[SchedPtr] = sched */
	popl	%eax			/* eax = func; esp now points at ws */
	call	*%eax			/* call func(ws) -- ws read from 4(%esp) */
	movl	I386_SCHED_PTR_BYTES(%ebp), %esi	/* restore sched */
	movl	(%esi), %esp		/* esp = sched->stack (16-byte aligned) */
	movl	%ebp, %eax		/* eax = wptr (will become p0) */
	subl	$I386_CIF_PROCESS_BYTES, %eax	/* p0 = wptr - CIF_PROCESS_WORDS*4 */
	movl	%esi, %edx		/* edx = sched (regparm(3) arg 2) */
	movl	%ebp, %ecx		/* ecx = Wptr (regparm(3) arg 3) */
	call	kernel_Y_proc_end	/* tail-call (alignment via call+ud2) */
	ud2				/* unreachable */
.size ccsp_cif_proc_stub_label, . - ccsp_cif_proc_stub_label

.globl ccsp_cif_light_proc_stub_label
.type ccsp_cif_light_proc_stub_label, @function
ccsp_cif_light_proc_stub_label:
	movl	(%ebp), %esp		/* esp = wptr[0] (private stack top) */
	movl	%esi, I386_SCHED_PTR_BYTES(%ebp)	/* wptr[SchedPtr] = sched */
	popl	%eax			/* eax = func */
	call	*%eax			/* call func(ws) */
	movl	I386_SCHED_PTR_BYTES(%ebp), %esi	/* restore sched */
	movl	(%esi), %esp		/* esp = sched->stack */
	movl	I386_BARRIER_PTR_BYTES(%ebp), %eax	/* p0 = *(wptr + BarrierPtr*4) */
	movl	%esi, %edx		/* edx = sched */
	movl	%ebp, %ecx		/* ecx = Wptr */
	call	kernel_Y_endp		/* tail-call */
	ud2				/* unreachable */
.size ccsp_cif_light_proc_stub_label, . - ccsp_cif_light_proc_stub_label

/*
 * ccsp_cif_endp_resume_label  (Phase 2)
 *
 * Code-only label whose address is returned by kernel_CIF_endp_resume_stub
 * and stored as the IptrSucc of every LightProc barrier.  When a child
 * process ends and the barrier count drops to zero, the parent resumes
 * here: load the parent's Wptr from wptr[-4] (= byte -16 from current
 * ebp on 32-bit) and jump to its saved Iptr at wptr[-1] (= byte -4
 * from ebp).
 *
 * This is NOT a callable function -- it has no prologue and is only
 * ever entered via Iptr-jump from the scheduler, with ebp already
 * set to the resuming process's Wptr by ccsp_dispatch_process.
 */
.globl ccsp_cif_endp_resume_label
.type ccsp_cif_endp_resume_label, @function
ccsp_cif_endp_resume_label:
	movl	-16(%ebp), %ebp		/* ebp = parent Wptr */
	jmp	*-4(%ebp)		/* jump to parent Wptr[Iptr] */
.size ccsp_cif_endp_resume_label, . - ccsp_cif_endp_resume_label

/*
 * void ccsp_kernel_enter(void *init, void *stack, word *Wptr, word *Fptr) __attribute__((noreturn))
 *
 * Phase 2: kernel boot primitive.  Replaces the inline-asm K_ENTRY
 * macro called from ccsp_kernel_entry() in entry.c.  Switches esp to
 * the freshly-laid-out kernel stack and tail-jumps to `init`
 * (typically kernel_Y_rtthreadinit) with the regparm(3) runtime
 * convention: init(stack, Fptr, Wptr) where eax=stack=p0,
 * edx=Fptr=sched, ecx=Wptr.
 *
 * cdecl args (incoming):
 *   4(%esp) = init
 *   8(%esp) = stack
 *  12(%esp) = Wptr
 *  16(%esp) = Fptr
 *
 * ebx is callee-saved per cdecl, but we're noreturn so the caller can
 * never observe it.  Use it as a scratch to hold init across the SP
 * switch (the original args on the old stack become inaccessible).
 */
.globl ccsp_kernel_enter
.type ccsp_kernel_enter, @function
ccsp_kernel_enter:
	movl	4(%esp), %ebx		/* ebx = init (scratch) */
	movl	8(%esp), %eax		/* eax = stack (will be p0) */
	movl	12(%esp), %ecx		/* ecx = Wptr */
	movl	16(%esp), %edx		/* edx = Fptr (will be sched) */
	movl	%eax, %esp		/* esp = stack (16-byte aligned by entry.c) */
	/* Use call (not jmp) so the call pushes a fake return address:
	 * i386 cdecl/SysV ABI expects esp ≡ 12 (mod 16) on entry to a
	 * function.  We're noreturn so the return address is dead, but
	 * the alignment must be correct. */
	call	*%ebx			/* call init (never returns) */
	ud2				/* unreachable */
.size ccsp_kernel_enter, . - ccsp_kernel_enter

/*
 * void ccsp_dispatch_process(sched_t *sched, word *Wptr) __attribute__((noreturn))
 *
 * Phase 2: kernel-to-user dispatch primitive.  Replaces the inline-asm
 * K_ZERO_OUT_JRET macro at all 13 call sites in sched.c.  This function
 * never returns to its caller.
 *
 * Sets the runtime register convention for the dispatched process,
 * restores the kernel SP from sched->stack, and jumps to the saved
 * instruction pointer at Wptr[-1] (= byte -4 from Wptr on 32-bit).
 *
 * Calling convention: cdecl, args on the stack.
 *   4(%esp) = sched
 *   8(%esp) = Wptr
 */
.globl ccsp_dispatch_process
.type ccsp_dispatch_process, @function
ccsp_dispatch_process:
	movl	4(%esp), %esi		/* esi = sched */
	movl	8(%esp), %ebp		/* ebp = Wptr */
	movl	(%esi), %esp		/* esp = sched->stack */
	jmp	*-4(%ebp)		/* jump to Wptr[Iptr] */
.size ccsp_dispatch_process, . - ccsp_dispatch_process

.section .note.GNU-stack,"",@progbits
