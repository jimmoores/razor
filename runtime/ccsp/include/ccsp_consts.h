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

/*
 * Phase 4B (option III): bias every PROC's workspace frame by
 * PROC_DESC_BIAS slots, reserving the bottom N slots for the per-call
 * kernel descriptor.  The compiler's own local area starts at slot
 * PROC_DESC_BIAS (instead of slot 0) and every LDL/STL/LDLP operand
 * is biased accordingly, including the wssize reported to libkrocif
 * so the workspace is large enough.
 *
 * The target end state is: descriptor fields live at positive offsets
 * Wptr[0..PROC_DESC_BIAS-1] instead of Wptr[-1..-9]; the compiler's
 * locals live at Wptr[PROC_DESC_BIAS..PROC_DESC_BIAS+L-1]; and below
 * Wptr becomes dead space that C function prologues and signal frames
 * can freely clobber.  That unlocks Phase 4B-part-2 where tranx86 can
 * map REG_WPTR directly onto the hardware stack pointer.
 *
 * Size chosen as 12 slots (96 bytes = 16-byte aligned) to cover all
 * of the legacy descriptor fields in one contiguous area:
 *   - 9 currently-negative fields (iptr..escape_ptr)
 *   - 3 currently-positive fields (temp/iptr_succ, count, saved_priority)
 *
 * PROC_DESC_BIAS must stay in sync with:
 *   tools/occ21/include/genhdr.h       -- PROC_DESC_BIAS
 *   tools/tranx86/transputer.h         -- PROC_DESC_BIAS
 *   tools/tranx86/proc_desc.h          -- struct size
 * If these drift, the compiler and runtime disagree about where
 * each slot lives.  A _Static_assert is dangerous here because the
 * three files see different compile-time environments; rely on the
 * comments and the fact that nobody should be editing this number
 * lightly.
 */
#define PROC_DESC_BIAS		12

/* Workspace offsets.  These are the legacy NEGATIVE-offset constants
 * that currently address the descriptor area below Wptr.  They stay
 * valid until commit C3 flips the struct to positive offsets; commit
 * C2 merely biases the compiler's frame so the slots below (and at)
 * the new cutoff become dead space, ready for the descriptor to
 * move into. */
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

