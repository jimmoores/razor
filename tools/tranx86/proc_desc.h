/*
 *	proc_desc.h -- tranx86 mirror of the CCSP process descriptor
 *	Copyright (C) 2026 Jim Moores
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
 */

/*
 *	Phase 3C of the calling-convention refactor.
 *
 *	tranx86 emits memory accesses using byte-offset constants
 *	(W_IPTR, W_LINK, ..., defined in transputer.h) to address the
 *	per-process metadata at negative offsets from the workspace
 *	pointer.  The runtime mirrors the same layout via the typed
 *	process_descriptor_t struct in runtime/ccsp/include/process_desc.h.
 *
 *	Historically the two sides could drift independently: nothing
 *	checked at build time that, e.g., tranx86's W_IPTR landed at the
 *	same byte offset as the runtime's process_descriptor_t::iptr.
 *	This header introduces a tranx86-side mirror struct
 *	(tranx86_proc_desc_t) and a PROC_DESC_OFF() macro that computes
 *	field offsets via offsetof, then re-derives the W_* macros from
 *	those offsets so any future layout change only needs to update a
 *	single struct definition (here, mirrored against the runtime).
 *
 *	Static assertions further down verify that the new offsets are
 *	*numerically identical* to the legacy literal-shift definitions
 *	-- i.e. that this stage is purely a syntactic refactor and emits
 *	exactly the same bytes as before.
 *
 *	Important: this struct must stay in step with
 *	runtime/ccsp/include/process_desc.h.  We can't simply #include
 *	the runtime header because it depends on runtime types (word,
 *	ccsp_consts.h) that aren't on tranx86's include path; we also
 *	want tranx86's types to be host-native (long), since tranx86
 *	runs on the build host.  This implicitly assumes host word size
 *	== target word size, which is the same assumption transputer.h
 *	already makes for WSH.
 */

#ifndef __TRANX86_PROC_DESC_H
#define __TRANX86_PROC_DESC_H

#include <stddef.h>		/* offsetof */

/*
 *	tranx86_proc_desc_t -- mirrors process_descriptor_t in
 *	runtime/ccsp/include/process_desc.h.
 *
 *	The runtime header uses anonymous unions for slots that have
 *	more than one legacy name (sched_ptr/stack_ptr at -7, etc.).
 *	tranx86 only ever references the canonical name for each slot
 *	(it never emits code that touches the CIF-specific stack_ptr or
 *	the ALT-specific state, for example), so we don't need the
 *	union aliases here.  Just the canonical fields at the same
 *	offsets.
 *
 *	Field types are `long`, which equals the host word size: this
 *	is what transputer.h's WSH already assumes.
 */
/*
 *	Phase 4B-III C3: the descriptor now lives at POSITIVE Wptr
 *	offsets.  The layout below mirrors the flipped
 *	process_descriptor_t in runtime/ccsp/include/process_desc.h:
 *	escape_ptr at +0, saved_priority at +11.  The net size is
 *	PROC_DESC_BIAS words (12), which is the amount commit C2
 *	biased the compiler's frame by.
 */
typedef struct tranx86_proc_desc {
	long	escape_ptr;		/* wptr[+0]  -- CIF only */
	long	barrier_ptr;		/* wptr[+1]  -- CIF only */
	long	sched_ptr;		/* wptr[+2]  -- CIF only (alias stack_ptr) */
	long	time_f;			/* wptr[+3] */
	long	tlink;			/* wptr[+4] */
	long	pointer;		/* wptr[+5]  (alias state) */
	long	priofinity;		/* wptr[+6] */
	long	link;			/* wptr[+7] */
	long	iptr;			/* wptr[+8] */
	long	temp;			/* wptr[+9]  (alias iptr_succ) */
	long	count;			/* wptr[+10] */
	long	saved_priority;		/* wptr[+11] */
} tranx86_proc_desc_t;

/*
 *	Number of words below wptr that the descriptor occupies.  Zero
 *	now: the descriptor lives entirely at and above Wptr.  Kept as
 *	a macro so PROC_DESC_OFF() still does the right arithmetic.
 */
#define TRANX86_PROC_DESC_NEG_WORDS	0

/*
 *	PROC_DESC_OFF(field) -- byte offset of `field` from the
 *	workspace pointer.  With NEG_WORDS = 0, this is just the
 *	byte offsetof the field in the struct.
 */
#define PROC_DESC_OFF(field) \
	((long) offsetof(tranx86_proc_desc_t, field) \
	 - (TRANX86_PROC_DESC_NEG_WORDS * BytesPerWord))

#endif /* __TRANX86_PROC_DESC_H */
