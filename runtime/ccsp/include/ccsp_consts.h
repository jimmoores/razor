/*
 *	CCSP kernel C macro scheduler definitions
 *	Copyright (C) 1995, 1996, 1997 D.J. Beckett, D.C. Wood
 *	Copyright (C) 1998 Jim Moores
 *	Modifications (C) 2002 Fred Barnes <frmb@kent.ac.uk>
 *	Modifications (C) 2007 Carl Ritson <cgr@kent.ac.uk>
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

#ifndef __CCSP_CONSTS_H
#define __CCSP_CONSTS_H

/* Constants used in transputer instructions */
#if defined(TARGET_64BIT) || defined(__x86_64__) || defined(__aarch64__)
#define MostNeg 		((word)0x8000000000000000ULL)
#else
#define MostNeg 		((word)0x80000000)
#endif
#define NotProcess_p 		0
#if 0
#define Enabling_p 		NotProcess_p+1
#define Waiting_p 		NotProcess_p+2
#define Ready_p 		NotProcess_p+3
#define Buffered_p 		NotProcess_p+4
#endif
#define TimeSet_p 		NotProcess_p+1
#define TimeNotSet_p 		NotProcess_p+2
#define NoneSelected_o 		MostNeg		/* must not be 0 or a plausible address */

/* ALT flag bits */
#define ALT_ENABLING_BIT	30
#define ALT_ENABLING		(1 << ALT_ENABLING_BIT)
#define ALT_WAITING_BIT 	29
#define ALT_WAITING 		(1 << ALT_WAITING_BIT)
#define ALT_NOT_READY_BIT	28
#define ALT_NOT_READY		(1 << ALT_NOT_READY_BIT)
#define ALT_GUARDS		0xffffff

/* Workspace offsets */
#define SavedPriority	2
#define Count 		1
#define IptrSucc 	0
#define Temp 		0
#define Iptr 		-1
#define Link 		-2
#define Priofinity	-3
#define Pointer		-4
#define State		-4
#define TLink		-5
#define Time_f		-6
#define SchedPtr	-7	/* for CIF */
#define StackPtr	-7	/* for CIF */
#define BarrierPtr	-8	/* for CIF */
#define EscapePtr	-9	/* for CIF */

/* buffered channel constants */
#ifdef BUFFERED_CHANNELS
	#define BCTemp 0
	#define BCWaitInput 1
	#define BCWaitOutput 2
	#define BCFreeSlots 3
	#define BCSizeMask 4
	#define BCBufHead 5
	#define BCBufTail 6
	#define BCMagic 7
	#define BCBufStart 8

	#define BCMagicConst 0xfeedbeef
#endif	/* BUFFERED_CHANNELS */

/* priority and affinity macros */
#define AFFINITY_MASK (0xffffffe0)
#define AFFINITY_SHIFT (5)
#define PRIORITY_MASK (0x0000001f)
#define PHasAffinity(X) ((X) & AFFINITY_MASK)
#define PAffinity(X) (((X) & AFFINITY_MASK) >> AFFINITY_SHIFT)
#define PPriority(X) ((X) & PRIORITY_MASK)
#define BuildPriofinity(A,P) \
	( \
		(((A) << AFFINITY_SHIFT) & AFFINITY_MASK) \
		| \
		((P) & PRIORITY_MASK) \
	)

/*
 *	Size in bytes of the post-call instruction(s) that tranx86 emits
 *	immediately after every kernel call.  When a process is
 *	descheduled inside a kernel call, K_CALL_HEADER bumps the stored
 *	resume iptr past this instruction, so that on wake-up the dispatch
 *	path skips the caller's post-call restore.
 *
 *	Phase 4D (x86-64): the post-call instruction is
 *	`mov 48(%r15), %rsp` (restore user sp from sched->saved_user_sp),
 *	encoded as `49 8b 67 30` = 4 bytes.
 *
 *	Phase 4D (AArch64): the post-call sequence is two 4-byte A64
 *	instructions: `ldr x9, [x25, #48]` + `mov sp, x9` = 8 bytes.
 *
 *	i386 has no post-call restore (Wptr stays in ebp), so the bump is 0.
 */
#if defined(__x86_64__)
#  define CCSP_KCALL_RETURN_BUMP_BYTES	4
#elif defined(__aarch64__)
#  define CCSP_KCALL_RETURN_BUMP_BYTES	8
#else
#  define CCSP_KCALL_RETURN_BUMP_BYTES	0
#endif

/* CIF constants */
#define CIF_STACK_ALIGN		4 /* words */
#if defined(__aarch64__) || defined(__x86_64__)
#define CIF_STACK_LINKAGE   2 /* words */
#else
#define CIF_STACK_LINKAGE   1 /* words */
#endif
/* CIF_PROCESS_WORDS: number of words reserved below the workspace pointer
 * for CIF runtime state.  On AArch64, the CIF assembly stub saves all
 * callee-saved registers (x19-x30, x29) plus SP to workspace negative
 * offsets, requiring slots -7 through -22 (22 words total). */
#if defined(__aarch64__) || defined(__x86_64__)
#define CIF_PROCESS_WORDS	24 /* words: enough for 22 saved regs + padding */
#else
#define CIF_PROCESS_WORDS	8 /* words */
#endif

#endif /* __CCSP_CONSTS_H */

