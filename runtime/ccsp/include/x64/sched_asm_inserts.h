/*
 *	x86-64 Architecture (GCC specific) Inserts
 *	Copyright (C) 2008  Carl Ritson <cgr@kent.ac.uk>
 *	Copyright (C) 2025 (x64 adaptation)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This file attempts to isolate as many as possible of the platform
 * specific features of this package as possible.  The macros have been
 * designed so that it should be easy to port this kernel to a more
 * register based machine fairly easily (eg Alpha, ARM etc).  Note that
 * there are numerous functions in this file labelled such that are no
 * longer used.
 *
 * x64 register convention (matching tranx86 archx64.c):
 *   WPTR  = r14  (workspace pointer, callee-saved)
 *   FPTR  = r13  (front pointer / run queue head, callee-saved)
 *   BPTR  = r12  (back pointer / run queue tail, callee-saved)
 *   SCHED = r15  (scheduler pointer, callee-saved)
 *   SP    = rsp  (hardware stack pointer)
 *   FP    = rbp  (frame pointer, reserved for C ABI)
 *
 * Kernel call convention (System V AMD64 ABI):
 *   param0 -> rdi  (1st argument)
 *   sched  -> rsi  (2nd argument)
 *   Wptr   -> rdx  (3rd argument)
 *   return -> rax
 */

#ifndef X64_SCHED_ASM_INSERTS_H
#define X64_SCHED_ASM_INSERTS_H

/*{{{  architecture dependent kernel call declarations */
/* x64 uses System V AMD64 ABI, no regparm needed.
 * noinline prevents the compiler from inlining kernel entry points,
 * which ensures __builtin_return_address(0) reads the correct return address. */
#define _K_CALL_DEFINE(X) \
	__attribute__((noinline)) void kernel_##X (word param0, sched_t *sched, word *Wptr)
#define _K_CALL_DEFINE_O(X) \
	__attribute__((noinline)) word kernel_##X (word param0, sched_t *sched, word *Wptr)
#define K_CALL_DEFINE_0_0(X) _K_CALL_DEFINE(X)
#define K_CALL_DEFINE_1_0(X) _K_CALL_DEFINE(X)
#define K_CALL_DEFINE_2_0(X) _K_CALL_DEFINE(X)
#define K_CALL_DEFINE_3_0(X) _K_CALL_DEFINE(X)
#define K_CALL_DEFINE_4_0(X) _K_CALL_DEFINE(X)
#define K_CALL_DEFINE_5_0(X) _K_CALL_DEFINE(X)
#define K_CALL_DEFINE_0_1(X) _K_CALL_DEFINE_O(X)
#define K_CALL_DEFINE_1_1(X) _K_CALL_DEFINE_O(X)
#define K_CALL_DEFINE_2_1(X) _K_CALL_DEFINE_O(X)
#define K_CALL_DEFINE_3_1(X) _K_CALL_DEFINE_O(X)
#define K_CALL_DEFINE_2_3(X) _K_CALL_DEFINE_O(X)
#define K_CALL_DEFINE_3_3(X) _K_CALL_DEFINE_O(X)

#define K_CALL_PTR(X) \
	((void *) (kernel_##X))

/* Capture the return address.  With noinline, __builtin_return_address(0)
 * reliably returns the caller's return address on x64. */
#define K_CALL_HEADER \
	__attribute__ ((unused)) \
	unsigned long return_address = (unsigned long) __builtin_return_address (0);
#define K_CALL_PARAM(N) \
	((N) == 0 ? param0 : sched->cparam[(N) - 1])
/*}}}*/

/*{{{  debugging support */
#define LOAD_ESP(X) \
	__asm__ __volatile__ ("		\n"	\
		"	movq %%rsp, %0	\n" 	\
		: "=g" (X)			\
	)
#define SAVE_EIP(X) \
	do { \
		X = (word) __builtin_return_address(0); \
	} while (0)
/*}}}*/

/*{{{  tracing support */
#ifdef CHECKING_MODE
#define TRACE_RETURN(addr)	\
	do { 						\
		sched->mdparam[8] = (word) Wptr;	\
		sched->mdparam[9] = (word) (addr);	\
		SAVE_EIP (sched->mdparam[11]);		\
	} while (0)
#else
#define	TRACE_RETURN(addr)	do { } while (0)
#endif /* CHECKING_MODE */
/*}}}*/

/*{{{  _K_SETGLABEL - internal global label define for inside asm blocks */
#define LABEL_ALIGN ".p2align 4	\n"
#ifndef NO_ASM_TYPE_DIRECTIVE
#define LABEL_TYPE(P,X) ".type "#P""#X", @function \n"
#else
#define LABEL_TYPE(P,X)
#endif

/* ELF x64 Linux: no leading underscore needed */
#define _K_SETGLABEL(X) \
	LABEL_ALIGN \
	LABEL_TYPE( ,X) \
	".globl "#X"	\n" \
	"	"#X":	\n"

#define _K_SETGGLABEL(X) \
	LABEL_ALIGN \
	LABEL_TYPE( ,X) \
	".globl "#X"	\n" \
	"	"#X":	\n"
/*}}}*/

/*{{{  outgoing entry-point macros*/
#define _SET_RETURN_ADDRESS(R) \
	((word *) sched->stack)[-1] = (word) (R)

#define K_ZERO_OUT() \
	return
#define K_ZERO_OUT_JUMP(R) \
	do { \
		_SET_RETURN_ADDRESS (R);\
		K_ZERO_OUT ();		\
	} while (0)
/* K_ZERO_OUT_JRET: kernel->user transition.
 * Load Wptr into r14, sched into r15, restore rsp from sched->stack (offset 0),
 * then jump to Wptr[Iptr] which is at Wptr[-1] = -8 bytes from Wptr (64-bit word). */
#define K_ZERO_OUT_JRET() \
	do { \
		TRACE_RETURN (Wptr[Iptr]); \
		__asm__ __volatile__ ("			\n" \
			"	movq	%0, %%r14	\n" \
			"	movq	%1, %%r15	\n" \
			"	movq	(%%r15), %%rsp	\n" \
			"	jmpq	*-8(%%r14)	\n" \
			: /* no outputs */ \
			: "r" (Wptr), "r" (sched) \
			: "memory", "r14", "r15"); \
	} while (0)

#define K_ONE_OUT(A) \
	return ((word) (A))
#define K_ONE_OUT_JUMP(R,A) \
	do { \
		_SET_RETURN_ADDRESS (R);\
		K_ONE_OUT (A);		\
	} while (0)

#define K_TWO_OUT(A,B) \
	do { \
		sched->cparam[0] = (word) (B);	\
		K_ONE_OUT (A);			\
	} while (0)

#define K_THREE_OUT(A,B,C) \
	do { \
		sched->cparam[0] = (word) (B); 	\
		sched->cparam[1] = (word) (C); 	\
		K_ONE_OUT (A);			\
	} while (0)
/*}}}*/

/*{{{  entry and label functions */
/* K_ENTRY: switches to kernel stack and calls init function.
 * Set rsp to kernel stack.  Pass stack as param0 (rdi), Fptr as sched (rsi),
 * Wptr as Wptr (rdx) per System V ABI.  Call init function. */
#define K_ENTRY(init,stack,Wptr,Fptr) \
	__asm__ __volatile__ ("				\n" \
		"	movq %0, %%rsp			\n" \
		"	movq %0, %%rdi			\n" \
		"	movq %2, %%rsi			\n" \
		"	movq %1, %%rdx			\n" \
		"	callq *%3			\n" \
		: /* no outputs */ \
		: "r" ((unsigned long)(stack)), "r" ((unsigned long)(Wptr)), \
		  "r" ((unsigned long)(Fptr)), "r" (init) \
		: "memory", "rdi", "rsi", "rdx", "rcx", "r8", "r9", \
		  "r10", "r11", "rax", "cc")
/*}}}*/

/*{{{  CIF helpers */
/* K_CIF_BCALLN: call a function with argc words of arguments copied from argv.
 * Save rbp, allocate stack space for args (argc << 3 bytes), align to 16,
 * copy args, call function, restore stack. */
#define K_CIF_BCALLN(func, argc, argv, ret) \
	__asm__ __volatile__ ("				\n" \
		"	pushq	%%rbp			\n" \
		"	movq	%%rsp, %%rbp		\n" \
		"	movq	%%rcx, %%rdx		\n" \
		"	shlq	$3, %%rdx		\n" \
		"	subq	%%rdx, %%rsp		\n" \
		"	andq	$-16, %%rsp		\n" \
		"	movq	%%rsp, %%rdi		\n" \
		"	cld				\n" \
		"	rep				\n" \
		"	movsb				\n" \
		"	callq	*%%rax			\n" \
		"	movq	%%rbp, %%rsp		\n" \
		"	popq	%%rbp			\n" \
		: "=a" (ret) \
		: "0" (func), "c" ((word)(argc) << 3), "S" (argv) \
		: "cc", "memory", "rdx", "rdi")

/* K_CIF_ENDP_RESUME: get resume address and do process end.
 * The call pushes the resume address (label 0:) onto the stack.
 * After the call falls through, we load Wptr from Wptr[-4] (saved parent Wptr)
 * and jump to Wptr[-1] (Iptr). */
#define K_CIF_ENDP_RESUME(address) \
	__asm__ __volatile__ ("				\n" \
		"	call	0f			\n" \
		"	movq	-32(%%r14), %%r14	\n" \
		"	jmpq	*-8(%%r14)		\n" \
		"0:	popq	%0			\n" \
		: "=g" (address) \
		: /* no inputs */ \
		: "memory")

/* _K_CIF_PROC: CIF process creation trampoline body.
 * Switches from kernel stack to process stack, pops and calls the C function,
 * then restores sched, sets up arguments for the kernel ENDP call. */
#define _K_CIF_PROC \
		"	movq	(%%r14), %%rsp		\n" \
		"	movq	%%r15, -56(%%r14)	\n" \
		"	popq	%%rax			\n" \
		"	callq	*%%rax			\n" \
		"	movq	-56(%%r14), %%r15	\n" \
		"	movq	%%r14, %%rdx		\n" \
		"	movq	%%r15, %%rsi		\n" \
		"	movq	(%%r15), %%rsp		\n" \
		"	addq	%2, %%rdi		\n" \
		"	addq	%1, %%rsi		\n"

#define K_CIF_PROC(address, call, offset) \
	__asm__ __volatile__ ("				\n" \
		"	call	0f			\n" \
		_K_CIF_PROC \
		"	callq	*(%%rsi)		\n" \
		"0:	popq	%0			\n" \
		: "=g" (address) \
		: "i" (offsetof(sched_t, calltable[call])), "i" (offset * sizeof(word)) \
		: "memory")
#define K_CIF_PROC_IND(address, call, offset) \
	__asm__ __volatile__ ("				\n" \
		"	call	0f			\n" \
		_K_CIF_PROC \
		"	movq	(%%rdi), %%rdi		\n" \
		"	callq	*(%%rsi)		\n" \
		"0:	popq	%0			\n" \
		: "=g" (address) \
		: "i" (offsetof(sched_t, calltable[call])), "i" (offset * sizeof(word)) \
		: "memory")
/*}}}*/

#endif /* X64_SCHED_ASM_INSERTS_H */
