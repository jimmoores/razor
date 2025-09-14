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

/*{{{  architecture dependent kernel call declarations */
/* AArch64 doesn't have regparm, use standard calling convention */
#define _K_CALL_DEFINE(X) \
	void kernel_##X (word param0, sched_t *sched, word *Wptr)
#define _K_CALL_DEFINE_O(X) \
	word kernel_##X (word param0, sched_t *sched, word *Wptr)
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

#define K_CALL_HEADER \
	__attribute__ ((unused)) \
	unsigned long return_address = (unsigned long) __builtin_return_address (0);
#define K_CALL_PARAM(N) \
	((N) == 0 ? param0 : sched->cparam[(N) - 1])
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
		TRACE_RETURN (Wptr[Iptr]); \
		__asm__ __volatile__ ("\t\t\t\n" \
			"\tmov x29, %0\t\n" \
			"\tldr x9, [%1]\t\n" \
			"\tmov sp, x9\t\n" \
			"\tldr x9, [x29, #-8]\t\n" \
			"\tbr x9\t\n" \
			: /* no outputs */ \
			: "r" (Wptr), "r" (sched) \
			: "memory", "x9", "x29"); \
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
		"\tmov sp, %0\t\t\t\n" \
		"\tblr %3\t\t\t\n" \
		: /* no outputs */ \
		: "r" (stack), "r" (Wptr), "r" (Fptr), "r" (init) \
		: "memory")
/*}}}*/

/*{{{  CIF helpers */
#define K_CIF_BCALLN(func, argc, argv, ret) \
	do { \
		/* Simple function call for aarch64 */ \
		ret = ((word (*)(word *, int))(func))(argv, argc); \
	} while (0)

#define K_CIF_ENDP_RESUME(address) \
	do { \
		address = __builtin_return_address(0); \
		__asm__ __volatile__ ("\t\t\t\t\n" \
			"\tldr x29, [x29, #-16]\t\n" \
			"\tldr x9, [x29, #-8]\t\n" \
			"\tbr x9\t\t\t\n" \
			: /* no outputs */ \
			: /* no inputs */ \
			: "memory", "x9", "x29"); \
	} while (0)

#define K_CIF_PROC(address, call, offset) \
	__asm__ __volatile__ ("				\n" \
		"	bl	0f			\n" \
		"	/* Simplified aarch64 implementation */\n" \
		"0:	mov	%0, x30			\n" \
		: "=r" (address) \
		: /* no inputs */ \
		: "memory", "x30")

#define K_CIF_PROC_IND(address, call, offset) \
	K_CIF_PROC(address, call, offset)
/*}}}*/

#endif /* AARCH64_SCHED_ASM_INSERTS */