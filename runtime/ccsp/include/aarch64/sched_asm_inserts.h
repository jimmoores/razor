/*
 *	AArch64 Architecture (GCC specific) Inserts
 *	Copyright (C) 2008  Carl Ritson <cgr@kent.ac.uk>
 *	Copyright (C) 2024 Amazon Q Developer (aarch64 adaptation)
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
 */

#ifndef AARCH64_SCHED_ASM_INSERTS_H
#define AARCH64_SCHED_ASM_INSERTS_H

/* CCSP_DIRECT_CALL: direct C calling convention for kernel functions.
 * Instead of passing all parameters through param0/cparam[], each kernel
 * function receives its parameters as normal C function arguments.
 * tranx86 (compose_aarch64_kcall) passes all inputs in x0..xN-1 followed
 * by sched and Wptr.  CIF stubs still use the calltable with adapter
 * wrappers that translate old ABI -> new ABI. */
#define CCSP_DIRECT_CALL

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
#define K_CALL_DEFINE_2_3(X) __attribute__((noinline)) word kernel_##X (word p0, word p1, sched_t *sched, word *Wptr)

/* 3 inputs */
#define K_CALL_DEFINE_3_0(X) __attribute__((noinline)) void kernel_##X (word p0, word p1, word p2, sched_t *sched, word *Wptr)
#define K_CALL_DEFINE_3_1(X) __attribute__((noinline)) word kernel_##X (word p0, word p1, word p2, sched_t *sched, word *Wptr)
#define K_CALL_DEFINE_3_3(X) __attribute__((noinline)) word kernel_##X (word p0, word p1, word p2, sched_t *sched, word *Wptr)

/* 4 inputs */
#define K_CALL_DEFINE_4_0(X) __attribute__((noinline)) void kernel_##X (word p0, word p1, word p2, word p3, sched_t *sched, word *Wptr)

/* 5 inputs */
#define K_CALL_DEFINE_5_0(X) __attribute__((noinline)) void kernel_##X (word p0, word p1, word p2, word p3, word p4, sched_t *sched, word *Wptr)

#define K_CALL_PTR(X) \
	((void *) (kernel_##X))

/* K_CALL_HEADER: still capture return address from LR for error handlers.
 * Direct x30 capture is used because __builtin_return_address(0) is
 * unreliable on AArch64 with -O2 when inlining occurs. */
#define K_CALL_HEADER \
	unsigned long return_address; \
	__asm__ volatile ("mov %0, x30" : "=r" (return_address));

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
/* AArch64 doesn't have regparm, use standard calling convention */
/* noinline prevents the compiler from inlining kernel entry points,
 * which would cause K_CALL_HEADER's x30 capture to read the wrong LR. */
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

/* Capture the return address (LR / x30) directly via inline asm.
 * __builtin_return_address(0) is unreliable on AArch64 with -O2 because
 * the compiler may inline kernel functions, causing it to return the
 * WRONG caller's return address (e.g., mt_release_simple instead of
 * the occam code's return point). Direct x30 capture is immune to
 * inlining because x30 always holds the link register at function entry. */
#define K_CALL_HEADER \
	unsigned long return_address; \
	__asm__ volatile ("mov %0, x30" : "=r" (return_address));
#define K_CALL_PARAM(N) \
	((N) == 0 ? param0 : sched->cparam[(N) - 1])
/*}}}*/
#endif /* CCSP_DIRECT_CALL */
/*}}}*/

/*{{{  debugging support */
#define LOAD_ESP(X) \
	__asm__ __volatile__ ("\t\t\n"\t\
		"\tmov %0, sp\t\n" \t\
		: "=r" (X)\t\t\t\
	)
#define SAVE_EIP(X) \
	do { \
		X = (word) __builtin_return_address(0); \
	} while (0)
/*}}}*/

/*{{{  tracing support */
#ifdef CHECKING_MODE
#define TRACE_RETURN(addr)\t\
	do { \t\t\t\t\t\t\
		sched->mdparam[8] = (word) Wptr;\t\
		sched->mdparam[9] = (word) (addr);\t\
		SAVE_EIP (sched->mdparam[11]);\t\t\
	} while (0)
#else
#define	TRACE_RETURN(addr)	do { } while (0)
#endif /* CHECKING_MODE */
/*}}}*/

/*{{{  _K_SETGLABEL - internal global label define for inside asm blocks */
#define LABEL_ALIGN ".p2align 4\t\n"
#ifndef NO_ASM_TYPE_DIRECTIVE
#define LABEL_TYPE(P,X) ".type "#P""#X", %function \n"
#else
#define LABEL_TYPE(P,X)
#endif

#if defined(TARGET_OS_DARWIN)
/* Darwin/Mach-O: use single leading underscore for global symbols */
#define _K_SETGLABEL(X) \
LABEL_ALIGN \
LABEL_TYPE(_,X) \
".globl _"#X"\t\n" \
"\t_"#X":\t\n"

#define _K_SETGGLABEL(X) \
LABEL_ALIGN \
LABEL_TYPE(_,X) \
".globl _"#X"\t\n" \
"\t_"#X":\t\n"
#else /* !defined(TARGET_OS_DARWIN) */
/* ELF: no leading underscore */
#define _K_SETGLABEL(X) \
LABEL_ALIGN \
LABEL_TYPE( ,X) \
".globl "#X"\t\n" \
"\t"#X":\t\n"

#define _K_SETGGLABEL(X) \
LABEL_ALIGN \
LABEL_TYPE( ,X) \
".globl "#X"\t\n" \
"\t"#X":\t\n"
#endif /* defined(TARGET_OS_DARWIN) */
/*}}}*/


/*{{{  outgoing entry-point macros*/
#define _SET_RETURN_ADDRESS(R) \
	((word *) sched->stack)[-1] = (word) (R)

#define K_ZERO_OUT() \
	return
#define K_ZERO_OUT_JUMP(R) \
	do { \
		_SET_RETURN_ADDRESS (R);\
		K_ZERO_OUT ();\
	} while (0)
#define K_ZERO_OUT_JRET() \
	do { \
		DT_LOG("JRET", Wptr, Wptr ? Wptr[Iptr] : 0, sched, 0); \
		TRACE_RETURN (Wptr[Iptr]); \
		__asm__ __volatile__ ("\t\t\t\n" \
			"\tmov x28, %0\t\n" \
			"\tmov x25, %1\t\n" \
			"\tldr x9, [x25]\t\n" \
			"\tmov sp, x9\t\n" \
			"\tldr x9, [x28, #-8]\t\n" \
			"\tbr x9\t\n" \
			: /* no outputs */ \
			: "r" (Wptr), "r" (sched) \
			: "memory", "x9", "x25", "x28"); \
	} while (0)
#define K_ONE_OUT(A) \
	return ((word) (A))
#define K_ONE_OUT_JUMP(R,A) \
	do { \
		_SET_RETURN_ADDRESS (R);\
		K_ONE_OUT (A);\
	} while (0)

#define K_TWO_OUT(A,B) \
	do { \
		sched->cparam[0] = (word) (B);\
		K_ONE_OUT (A);\
	} while (0)

#define K_THREE_OUT(A,B,C) \
	do { \
		sched->cparam[0] = (word) (B);\
		sched->cparam[1] = (word) (C);\
		K_ONE_OUT (A);\
	} while (0)
/*}}}*/

/*{{{  entry and label functions */
#define K_ENTRY(init,stack,Wptr,Fptr) \
	__asm__ __volatile__ ("\t\t\t\t\n" \
		"\tmov x0, %0\t\t\t\n" \
		"\tmov x1, %2\t\t\t\n" \
		"\tmov x2, %1\t\t\t\n" \
		"\tmov sp, x0\t\t\t\n" \
		"\tblr %3\t\t\t\n" \
		: /* no outputs */ \
		: "r" (stack), "r" (Wptr), "r" (Fptr), "r" (init) \
		: "x0", "x1", "x2", "memory", "x30", "cc")
/*}}}*/

/*{{{  CIF helpers */
/* K_CIF_BCALLN: call a C function with N word-sized arguments.
 * On i386, this copies argv onto the stack so the function receives
 * its arguments normally.  On AArch64, the first 8 arguments go in
 * registers x0-x7.  We unpack argv into the appropriate registers
 * using a switch on argc. */
#define K_CIF_BCALLN(func, argc, argv, ret) \
	do { \
		word (*_f0)(void) = (word (*)(void))(func); \
		word (*_f1)(word) = (word (*)(word))(func); \
		word (*_f2)(word,word) = (word (*)(word,word))(func); \
		word (*_f3)(word,word,word) = (word (*)(word,word,word))(func); \
		word (*_f4)(word,word,word,word) = (word (*)(word,word,word,word))(func); \
		switch (argc) { \
		case 0: ret = _f0(); break; \
		case 1: ret = _f1(argv[0]); break; \
		case 2: ret = _f2(argv[0], argv[1]); break; \
		case 3: ret = _f3(argv[0], argv[1], argv[2]); break; \
		default: ret = _f4(argv[0], argv[1], argv[2], argv[3]); break; \
		} \
	} while (0)

#define K_CIF_ENDP_RESUME(address) \
	__asm__ __volatile__ ("\t\t\t\t\n" \
		"\tadr %0, 0f\t\n" \
		"\tb 1f\t\t\n" \
		"0:\t\t\t\t\n" \
		"\tldr x28, [x28, #-32]\t\n" \
		"\tldr x9, [x28, #-8]\t\n" \
		"\tbr x9\t\t\t\n" \
		"1:\t\t\t\t\n" \
		: "=r" (address) \
		: /* no inputs */ \
		: "memory", "x9", "x28")
#define K_CIF_PROC(address, call, offset) \
	__asm__ __volatile__ ("\t\t\t\t\n" \
		"\tadr %0, 0f\t\n" \
		"\tb 1f\t\t\n" \
		"0:\t\t\t\t\n" \
		"\tldr x9, [x28, #0]\t\n" \
		"\tstr x25, [x28, #-56]\t\n" \
		"\tmov sp, x9\t\n" \
		"\tldr x30, [sp], #16\t\n" \
		"\tldur x0, [sp, #-8]\t\n" \
		"\tblr x30\t\t\n" \
		"\tldr x25, [x28, #-56]\t\n" \
		"\tldr x9, [x25, #0]\t\n" \
		"\tmov sp, x9\t\n" \
		"\tsub x0, x28, %2\t\n" \
		"\tmov x1, x25\t\n" \
		"\tmov x2, x28\t\n" \
		"\tldr x9, [x25, %1]\t\n" \
		"\tbr x9\t\t\n" \
		"1:\t\t\t\t\n" \
		: "=r" (address) \
		: "i" (offsetof(sched_t, calltable[call])), "i" (-(offset) * (int)sizeof(word)) \
		: "memory", "x0", "x9")
#define K_CIF_PROC_IND(address, call, offset) \
	__asm__ __volatile__ ("\t\t\t\t\n" \
		"\tadr %0, 0f\t\n" \
		"\tb 1f\t\t\n" \
		"0:\t\t\t\t\n" \
		"\tldr x9, [x28, #0]\t\n" \
		"\tstr x25, [x28, #-56]\t\n" \
		"\tmov sp, x9\t\n" \
		"\tldr x30, [sp], #16\t\n" \
		"\tldur x0, [sp, #-8]\t\n" \
		"\tblr x30\t\t\n" \
		"\tldr x25, [x28, #-56]\t\n" \
		"\tldr x9, [x25, #0]\t\n" \
		"\tmov sp, x9\t\n" \
		"\tldur x0, [x28, %2]\t\n" \
		"\tmov x1, x25\t\n" \
		"\tmov x2, x28\t\n" \
		"\tldr x9, [x25, %1]\t\n" \
		"\tbr x9\t\t\n" \
		"1:\t\t\t\t\n" \
		: "=r" (address) \
		: "i" (offsetof(sched_t, calltable[call])), "i" ((offset) * (int)sizeof(word)) \
		: "memory", "x0", "x9")
/*}}}*/

#endif /* AARCH64_SCHED_ASM_INSERTS */