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

/* K_CALL_HEADER: still capture return address from LR for error handlers.
 * Direct x30 capture is used because __builtin_return_address(0) is
 * unreliable on AArch64 with -O2 when inlining occurs. */
/* Capture x30 (LR) and strip any PAC bits.  GCC may compile kernel
 * functions with paciasp (sign return address) which leaves x30 with
 * Pointer Authentication Code in bits 48-54.  save_return stores this
 * as Wptr[Iptr], and K_ZERO_OUT_JRET later branches to it via `br x9`.
 * Branching to a PAC-signed address (non-canonical) causes SIGSEGV.
 *
 * Strip PAC bits with a bitmask (0x0000FFFFFFFFFFFF = 48-bit user VA).
 * We use the captured value ONLY for save_return; the real x30 (which
 * may still be signed) is left intact so the function's autiasp/retaa
 * in its epilogue works correctly.
 */
#define K_CALL_HEADER \
	unsigned long return_address; \
	__asm__ volatile ("mov %0, x30" : "=r" (return_address)); \
	return_address &= 0x0000FFFFFFFFFFFFUL;

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

/* Capture the return address (LR / x30) directly via inline asm.
 * __builtin_return_address(0) is unreliable on AArch64 with -O2 because
 * the compiler may inline kernel functions, causing it to return the
 * WRONG caller's return address (e.g., mt_release_simple instead of
 * the occam code's return point). Direct x30 capture is immune to
 * inlining because x30 always holds the link register at function entry. */
/* Capture x30 (LR) and strip any PAC bits.  GCC may compile kernel
 * functions with paciasp (sign return address) which leaves x30 with
 * Pointer Authentication Code in bits 48-54.  save_return stores this
 * as Wptr[Iptr], and K_ZERO_OUT_JRET later branches to it via `br x9`.
 * Branching to a PAC-signed address (non-canonical) causes SIGSEGV.
 *
 * Strip PAC bits with a bitmask (0x0000FFFFFFFFFFFF = 48-bit user VA).
 * We use the captured value ONLY for save_return; the real x30 (which
 * may still be signed) is left intact so the function's autiasp/retaa
 * in its epilogue works correctly.
 */
#define K_CALL_HEADER \
	unsigned long return_address; \
	__asm__ volatile ("mov %0, x30" : "=r" (return_address)); \
	return_address &= 0x0000FFFFFFFFFFFFUL;
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
/* K_ZERO_OUT_JRET: kernel->user transition (Phase 2).
 * Calls the noreturn ccsp_dispatch_process(sched, Wptr) helper in
 * aarch64_cif.S, which sets x28=Wptr, x25=sched, restores sp from
 * sched->stack and jumps to Wptr[Iptr]. */
extern void ccsp_dispatch_process (sched_t *sched, word *Wptr) __attribute__((noreturn));
#define K_ZERO_OUT_JRET() \
	do { \
		DT_LOG("JRET", Wptr, Wptr ? PROC_DESC(Wptr)->iptr : 0, sched, 0); \
		TRACE_RETURN (PROC_DESC(Wptr)->iptr); \
		ccsp_dispatch_process (sched, Wptr); \
	} while (0)
#define K_ONE_OUT(A) \
	return ((word) (A))
#define K_ONE_OUT_JUMP(R,A) \
	do { \
		_SET_RETURN_ADDRESS (R);\
		K_ONE_OUT (A);\
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
/* K_ENTRY: kernel boot dispatch (Phase 2).
 * Calls the noreturn ccsp_kernel_enter helper in aarch64_cif.S, which
 * switches sp to the kernel stack and tail-calls `init` with the
 * legacy 3-arg runtime convention init(stack, Fptr, Wptr). */
extern void ccsp_kernel_enter (void *init, void *stack, word *Wptr, word *Fptr) __attribute__((noreturn));
#define K_ENTRY(init,stack,Wptr,Fptr) \
	ccsp_kernel_enter ((void *)(init), (void *)(stack), (word *)(Wptr), (word *)(Fptr))
/*}}}*/

/*{{{  CIF helpers */
/* K_CIF_BCALLN removed in Phase 2: ccsp_cif_bcalln_stub in entry.c
 * now does the dispatch in C using a switch on argc with typed
 * function pointers. */

/* K_CIF_ENDP_RESUME removed in Phase 2: kernel_CIF_endp_resume_stub
 * now returns &ccsp_cif_endp_resume_label directly (defined in
 * aarch64_cif.S).  No inline-asm dance needed. */
/* K_CIF_PROC / K_CIF_PROC_IND removed in Phase 2.
 * kernel_CIF_proc_stub and kernel_CIF_light_proc_stub now return the
 * addresses of ccsp_cif_proc_stub_label / ccsp_cif_light_proc_stub_label
 * directly (defined in aarch64_cif.S).  No inline-asm dance needed. */
/*}}}*/

#endif /* AARCH64_SCHED_ASM_INSERTS */