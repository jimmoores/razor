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
