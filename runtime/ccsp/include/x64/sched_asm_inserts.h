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

/* CCSP_DIRECT_CALL: direct C calling convention for kernel functions.
 * Instead of passing all parameters through param0/cparam[], each kernel
 * function receives its parameters as normal C function arguments.
 * Enable when tranx86 kcall generation and CIF stubs are also updated
 * (Phase 1B/1C). Until then, leave disabled for ABI compatibility. */
/* #define CCSP_DIRECT_CALL */

/*{{{  architecture dependent kernel call declarations */
#ifdef CCSP_DIRECT_CALL
/*{{{  CCSP_DIRECT_CALL: kernel functions receive explicit C parameters */
/* Under CCSP_DIRECT_CALL, kernel functions receive inputs as normal C
 * parameters (p0, p1, ...) followed by sched and Wptr.  The _N_M naming
 * gives N inputs, M outputs.  Variants returning a value use word return. */

/* 0 inputs */
#define K_CALL_DEFINE_0_0(X) __attribute__((noinline)) void kernel_##X (sched_t *sched, word *Wptr)
#define K_CALL_DEFINE_0_1(X) __attribute__((noinline)) word kernel_##X (sched_t *sched, word *Wptr)

/* 1 input */
#define K_CALL_DEFINE_1_0(X) __attribute__((noinline)) void kernel_##X (word p0, sched_t *sched, word *Wptr)
#define K_CALL_DEFINE_1_1(X) __attribute__((noinline)) word kernel_##X (word p0, sched_t *sched, word *Wptr)

/* 2 inputs */
#define K_CALL_DEFINE_2_0(X) __attribute__((noinline)) void kernel_##X (word p0, word p1, sched_t *sched, word *Wptr)
#define K_CALL_DEFINE_2_1(X) __attribute__((noinline)) word kernel_##X (word p0, word p1, sched_t *sched, word *Wptr)
/* 2 inputs, 3 outputs: 1st output via word return, additional 2 via extra_out[0..1] */
#define K_CALL_DEFINE_2_3(X) __attribute__((noinline)) word kernel_##X (word p0, word p1, sched_t *sched, word *Wptr, word *extra_out)

/* 3 inputs */
#define K_CALL_DEFINE_3_0(X) __attribute__((noinline)) void kernel_##X (word p0, word p1, word p2, sched_t *sched, word *Wptr)
#define K_CALL_DEFINE_3_1(X) __attribute__((noinline)) word kernel_##X (word p0, word p1, word p2, sched_t *sched, word *Wptr)
/* 3 inputs, 3 outputs: 1st output via word return, additional 2 via extra_out[0..1] */
#define K_CALL_DEFINE_3_3(X) __attribute__((noinline)) word kernel_##X (word p0, word p1, word p2, sched_t *sched, word *Wptr, word *extra_out)

/* 4 inputs */
#define K_CALL_DEFINE_4_0(X) __attribute__((noinline)) void kernel_##X (word p0, word p1, word p2, word p3, sched_t *sched, word *Wptr)

/* 5 inputs */
#define K_CALL_DEFINE_5_0(X) __attribute__((noinline)) void kernel_##X (word p0, word p1, word p2, word p3, word p4, sched_t *sched, word *Wptr)

#define K_CALL_PTR(X) \
	((void *) (kernel_##X))

/* K_CALL_HEADER: still capture return address for error handlers.
 * With noinline, __builtin_return_address(0) reliably returns the
 * caller's return address on x64. */
#define K_CALL_HEADER \
	__attribute__ ((unused)) \
	unsigned long return_address = (unsigned long) __builtin_return_address (0);

/* K_CALL_PARAM: map parameter index to the corresponding C argument.
 * Token pasting is used since all call sites use literal indices 0-4. */
#define K_CALL_PARAM_0 p0
#define K_CALL_PARAM_1 p1
#define K_CALL_PARAM_2 p2
#define K_CALL_PARAM_3 p3
#define K_CALL_PARAM_4 p4
#define K_CALL_PARAM(N) K_CALL_PARAM_##N
/*}}}*/
#else /* !CCSP_DIRECT_CALL */
/*{{{  legacy: all parameters via param0/cparam[] */
/* x64 uses System V AMD64 ABI, no regparm needed.
 * noinline prevents the compiler from inlining kernel entry points,
 * which ensures __builtin_return_address(0) reads the correct return address. */
#define _K_CALL_DEFINE(X) \
	__attribute__((noinline)) void kernel_##X (word param0, sched_t *sched, word *Wptr)
#define _K_CALL_DEFINE_O(X) \
	__attribute__((noinline)) word kernel_##X (word param0, sched_t *sched, word *Wptr)
/* Multi-output kernel functions: 1st output via word return, extra 2
 * outputs written through caller-provided extra_out[0..1] */
#define _K_CALL_DEFINE_M3(X) \
	__attribute__((noinline)) word kernel_##X (word param0, sched_t *sched, word *Wptr, word *extra_out)
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
#define K_CALL_DEFINE_2_3(X) _K_CALL_DEFINE_M3(X)
#define K_CALL_DEFINE_3_3(X) _K_CALL_DEFINE_M3(X)

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
#endif /* CCSP_DIRECT_CALL */
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

/* K_TWO_OUT / K_THREE_OUT (Phase 1D Stage 2): the 1st output is
 * returned via the C return value (rax / x0); additional outputs go
 * through the caller-provided extra_out array instead of
 * sched->cparam[].  K_TWO_OUT has no users in the current kernel;
 * K_THREE_OUT is used by kernel_X_norm and kernel_X_trap, both of
 * which now take an extra `word *extra_out` parameter sized [2]. */
#define K_TWO_OUT(A,B) \
	do { \
		extra_out[0] = (word) (B); \
		return ((word) (A)); \
	} while (0)

#define K_THREE_OUT(A,B,C) \
	do { \
		extra_out[0] = (word) (B); \
		extra_out[1] = (word) (C); \
		return ((word) (A)); \
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
/* K_CIF_BCALLN: call a C function with N word-sized arguments from argv.
 * On x64, args go in registers (rdi, rsi, ...), not on the stack.
 * Unpack argv into registers via typed function pointer calls.
 * Zero al before call for x64 variadic function ABI compliance. */
#define K_CIF_BCALLN(func, argc, argv, ret) \
	do { \
		word (*_f0)(void) = (word (*)(void))(func); \
		word (*_f1)(word) = (word (*)(word))(func); \
		word (*_f2)(word,word) = (word (*)(word,word))(func); \
		word (*_f3)(word,word,word) = (word (*)(word,word,word))(func); \
		word (*_f4)(word,word,word,word) = (word (*)(word,word,word,word))(func); \
		__asm__ __volatile__ ("xorl %%eax, %%eax" ::: "rax"); \
		switch (argc) { \
		case 0: ret = _f0(); break; \
		case 1: ret = _f1((argv)[0]); break; \
		case 2: ret = _f2((argv)[0], (argv)[1]); break; \
		case 3: ret = _f3((argv)[0], (argv)[1], (argv)[2]); break; \
		default: ret = _f4((argv)[0], (argv)[1], (argv)[2], (argv)[3]); break; \
		} \
	} while (0)

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
 * then restores sched, sets up arguments for a follow-up kernel call.
 * After this fragment runs:
 *   rdi = wptr + (offset * sizeof(word))   (param0)
 *   rsi = sched                             (sched arg)
 *   rdx = wptr                              (Wptr arg)
 *   rsp = sched->stack                      (kernel stack restored) */
#define _K_CIF_PROC \
		"	movq	(%%r14), %%rsp		\n" \
		"	movq	%%r15, -56(%%r14)	\n" \
		"	popq	%%rax			\n" \
		"	popq	%%rdi			\n" \
		"	callq	*%%rax			\n" \
		"	movq	-56(%%r14), %%r15	\n" \
		"	movq	%%r14, %%rdi		\n" \
		"	movq	%%r14, %%rdx		\n" \
		"	movq	%%r15, %%rsi		\n" \
		"	movq	(%%r15), %%rsp		\n" \
		"	addq	%1, %%rdi		\n"

/* K_CIF_PROC / K_CIF_PROC_IND now dispatch directly to a named kernel
 * function symbol instead of indirecting through sched->calltable[call].
 * The `kernel_sym` parameter is a token (e.g. kernel_Y_proc_end) that
 * gets stringified into the asm.  This removes the only x64 dependency
 * on sched_t.calltable[]; the field is gated out under CCSP_HAS_CALLTABLE. */
#define K_CIF_PROC(address, kernel_sym, offset) \
	__asm__ __volatile__ ("				\n" \
		"	call	0f			\n" \
		_K_CIF_PROC \
		"	callq	" #kernel_sym "		\n" \
		"0:	popq	%0			\n" \
		: "=g" (address) \
		: "i" (offset * sizeof(word)) \
		: "memory")
#define K_CIF_PROC_IND(address, kernel_sym, offset) \
	__asm__ __volatile__ ("				\n" \
		"	call	0f			\n" \
		_K_CIF_PROC \
		"	movq	(%%rdi), %%rdi		\n" \
		"	callq	" #kernel_sym "		\n" \
		"0:	popq	%0			\n" \
		: "=g" (address) \
		: "i" (offset * sizeof(word)) \
		: "memory")
/*}}}*/

#endif /* X64_SCHED_ASM_INSERTS_H */
