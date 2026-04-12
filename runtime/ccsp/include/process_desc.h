/*
 *	process_desc.h -- CCSP process descriptor abstraction
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
 *	Phase 3A of the calling-convention refactor introduces a typed
 *	view onto the per-process metadata that has historically lived at
 *	negative offsets from the workspace pointer (Wptr).  In the legacy
 *	Transputer model the workspace grew upward, so "below the workspace
 *	pointer" was the natural place to keep the process descriptor
 *	(saved Iptr, scheduler links, etc.).  On a modern descending stack
 *	this puts the descriptor *below* the stack pointer once Wptr and
 *	SP are unified (Phase 4), where signal handlers can clobber it.
 *
 *	Stage 3A only introduces the abstraction.  The macro PROC_DESC()
 *	maps a workspace pointer onto a process_descriptor_t* whose fields
 *	land at exactly the same memory locations the legacy Wptr[Iptr]
 *	style accessors would have produced -- so this header changes the
 *	*type system*, not the *layout*.  Later stages will:
 *
 *	  3D: relocate the descriptor so it no longer overlaps the user
 *	      workspace area, and reach it through a dedicated descriptor
 *	      pointer register rather than via Wptr arithmetic;
 *	  4 : unify Wptr with the hardware stack pointer.
 *
 *	The struct field order is dictated by the legacy negative-offset
 *	layout in ccsp_consts.h: the lowest-address field (escape_ptr at
 *	wptr[-9]) must come first.  Do not reorder these fields without
 *	also updating that layout and every consumer of it.
 */

#ifndef __PROCESS_DESC_H
#define __PROCESS_DESC_H

#include "ukcthreads_types.h"		/* word */

/*
 *	Number of words occupied by the descriptor strictly *below* the
 *	workspace pointer.  This must match the negative-offset constants
 *	in ccsp_consts.h (EscapePtr = -9 today).  PROC_DESC() subtracts
 *	this many words from Wptr to find the descriptor base.
 */
#define PROC_DESC_NEG_WORDS	9

/*
 *	process_descriptor_t -- typed view of the per-process metadata.
 *
 *	The fields are listed in *memory order*, lowest address first, so
 *	that &desc->iptr == &wptr[-1] when the descriptor pointer was
 *	obtained via PROC_DESC(wptr).  Anonymous unions provide aliases
 *	for fields that share a slot in the legacy layout but are used
 *	for different purposes at different times -- e.g. SchedPtr and
 *	StackPtr both live at wptr[-7].  Stage 3E will split those.
 */
typedef struct process_descriptor {
	word	escape_ptr;		/* wptr[-9]  -- CIF only */
	word	barrier_ptr;		/* wptr[-8]  -- CIF only */
	union {				/* wptr[-7] */
		word	sched_ptr;	/*   CIF: pointer to ccsp_sched_t */
		word	stack_ptr;	/*   CIF: workspace top, init only */
	};
	word	time_f;			/* wptr[-6]  -- timer queue value */
	word	tlink;			/* wptr[-5]  -- timer queue link */
	union {				/* wptr[-4] */
		word	pointer;	/*   channel I/O pointer */
		word	state;		/*   ALT state */
	};
	word	priofinity;		/* wptr[-3]  -- priority + affinity */
	word	link;			/* wptr[-2]  -- run-queue link */
	word	iptr;			/* wptr[-1]  -- saved instruction pointer */
	union {				/* wptr[ 0] */
		word	temp;		/*   scratch */
		word	iptr_succ;	/*   successor Iptr (LightProc) */
	};
	word	count;			/* wptr[+1]  -- PAR reference count */
	word	saved_priority;		/* wptr[+2]  -- caller's priofinity */
} process_descriptor_t;

/*
 *	PROC_DESC(wptr) -- map a workspace pointer to its descriptor.
 *
 *	In the Stage 3A layout the descriptor straddles the workspace
 *	pointer: nine words below it (escape_ptr .. iptr) and three at
 *	or above it (temp .. saved_priority).  Subtracting
 *	PROC_DESC_NEG_WORDS lands the struct base where the first
 *	descriptor field belongs.
 *
 *	The cast goes via (void *) to silence -Wcast-align on targets
 *	where alignof(process_descriptor_t) > alignof(word *) (it never
 *	is in practice -- both are alignof(word) -- but the diagnostic
 *	is annoyingly conservative).
 */
#define PROC_DESC(wptr) \
	((process_descriptor_t *)(void *)((word *)(wptr) - PROC_DESC_NEG_WORDS))

/*
 *	Compile-time assertions that the descriptor struct lays out
 *	exactly on top of the legacy negative-offset slots defined in
 *	ccsp_consts.h.  If anyone reorders the struct or fiddles with
 *	the constants in ccsp_consts.h without updating both, this
 *	header refuses to compile.
 *
 *	_Static_assert is available as a GCC extension under -std=gnu89
 *	and later (verified on the debian9 / gcc 6.3 toolchain that
 *	hosts our oldest 32-bit build), so we don't need any portability
 *	dance here.
 */
#include <stddef.h>			/* offsetof */
#include "ccsp_consts.h"		/* Iptr, Link, ... */

#define PROC_DESC_FIELD_AT(field, slot) \
	_Static_assert( \
		offsetof(process_descriptor_t, field) == \
		(((slot) + PROC_DESC_NEG_WORDS) * sizeof(word)), \
		"process_descriptor_t::" #field " not at wptr[" #slot "]")

PROC_DESC_FIELD_AT(escape_ptr,    -9);
PROC_DESC_FIELD_AT(barrier_ptr,   -8);
PROC_DESC_FIELD_AT(sched_ptr,     -7);
PROC_DESC_FIELD_AT(stack_ptr,     -7);
PROC_DESC_FIELD_AT(time_f,        -6);
PROC_DESC_FIELD_AT(tlink,         -5);
PROC_DESC_FIELD_AT(pointer,       -4);
PROC_DESC_FIELD_AT(state,         -4);
PROC_DESC_FIELD_AT(priofinity,    -3);
PROC_DESC_FIELD_AT(link,          -2);
PROC_DESC_FIELD_AT(iptr,          -1);
PROC_DESC_FIELD_AT(temp,           0);
PROC_DESC_FIELD_AT(iptr_succ,      0);
PROC_DESC_FIELD_AT(count,          1);
PROC_DESC_FIELD_AT(saved_priority, 2);

#endif /* __PROCESS_DESC_H */
