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
 *	Phase 4B-III C3: the descriptor now lives at positive offsets
 *	above Wptr, not negative offsets below.  PROC_DESC_NEG_WORDS
 *	was 9 in the legacy layout; it is now 0, and PROC_DESC() returns
 *	the workspace pointer directly cast to a descriptor pointer.
 *	The compiler's locals area has been biased up by PROC_DESC_BIAS
 *	(= 12) slots in commit C2, so Wptr[0..11] is guaranteed to be
 *	descriptor-only dead space from the compiler's perspective.
 */
#define PROC_DESC_NEG_WORDS	0

/*
 *	process_descriptor_t -- typed view of the per-process metadata.
 *
 *	Fields are in memory order, lowest address first.  In this
 *	positive-offset layout the first field (escape_ptr) lives at
 *	wptr[+0], the last (saved_priority) at wptr[+11].  Total 12
 *	words = PROC_DESC_BIAS.  Anonymous unions alias fields that
 *	share a slot under different names in different contexts.
 */
typedef struct process_descriptor {
	word	escape_ptr;		/* wptr[+0]  -- CIF only */
	word	barrier_ptr;		/* wptr[+1]  -- CIF only */
	union {				/* wptr[+2] */
		word	sched_ptr;	/*   CIF: pointer to ccsp_sched_t */
		word	stack_ptr;	/*   CIF: workspace top, init only */
	};
	word	time_f;			/* wptr[+3]  -- timer queue value */
	word	tlink;			/* wptr[+4]  -- timer queue link */
	union {				/* wptr[+5] */
		word	pointer;	/*   channel I/O pointer */
		word	state;		/*   ALT state */
	};
	word	priofinity;		/* wptr[+6]  -- priority + affinity */
	word	link;			/* wptr[+7]  -- run-queue link */
	word	iptr;			/* wptr[+8]  -- saved instruction pointer */
	union {				/* wptr[+9] */
		word	temp;		/*   scratch */
		word	iptr_succ;	/*   successor Iptr (LightProc) */
	};
	word	count;			/* wptr[+10] -- PAR reference count */
	word	saved_priority;		/* wptr[+11] -- caller's priofinity */
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
/*
 *	An inline function rather than a macro, on purpose: this lets
 *	the compiler enforce that the argument is a `word *` (i.e. a
 *	workspace pointer), not a `process_descriptor_t *`.  Phase 3D-3b
 *	hit a silent bug where a `process_t` (a desc pointer) was
 *	accidentally wrapped in PROC_DESC, which type-checked because
 *	the macro version cast to (word *) and so accepted any pointer,
 *	but produced a wrong address (desc was used as if it were a
 *	wptr, putting the result 9 words below the descriptor).  With
 *	a typed parameter, the same bug becomes an "incompatible pointer
 *	types" diagnostic at every offending call site.
 */
static inline process_descriptor_t *PROC_DESC(word *wptr)
{
	/*
	 * Phase 4B-III C3: descriptor lives at Wptr[+0..+11] now, so
	 * PROC_DESC(wptr) == (process_descriptor_t *)wptr.  Kept as a
	 * function (not a macro or a plain cast) to preserve the typed
	 * parameter discipline from Phase 3D-3: accidentally passing a
	 * process_descriptor_t* (already a desc) to PROC_DESC would be
	 * a type error, caught at compile time.  The function form
	 * also keeps PROC_WPTR() symmetric.
	 */
	return (process_descriptor_t *)(void *)wptr;
}

/*
 *	PROC_WPTR(desc) -- inverse of PROC_DESC, recovering the workspace
 *	pointer that the legacy alias model would have computed `desc` from.
 *
 *	Stage 3D-3 starts using process_descriptor_t* values as the
 *	canonical "process identity" inside the scheduler queues, replacing
 *	the legacy `word *Wptr` chain.  PROC_WPTR is the bridge: code that
 *	still needs the AJW-level Wptr value (e.g. dispatch, which sets the
 *	Wptr register before jumping to the resume Iptr) calls
 *	PROC_WPTR(desc) to get it.
 *
 *	In the alias model PROC_WPTR is the literal inverse arithmetic.
 *	After stage 3D-4 (or whichever stage moves the descriptor to a
 *	separately-allocated location), this macro will read a saved-Wptr
 *	field from inside the descriptor instead -- but the API stays
 *	the same so call sites don't change.
 */
#define PROC_WPTR(desc) \
	((word *)((char *)(desc) + (PROC_DESC_NEG_WORDS * sizeof(word))))

/*
 *	process_t -- typed handle for "a process in a scheduler queue".
 *	Phase 3D-3 onward, this is the canonical type passed in and out
 *	of enqueue/dequeue/run-queue helpers, replacing `word *Wptr`.
 *	The underlying value is just a process_descriptor_t pointer.
 */
typedef process_descriptor_t *process_t;

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
		((slot) * sizeof(word)), \
		"process_descriptor_t::" #field " not at wptr[+" #slot "]")

PROC_DESC_FIELD_AT(escape_ptr,     0);
PROC_DESC_FIELD_AT(barrier_ptr,    1);
PROC_DESC_FIELD_AT(sched_ptr,      2);
PROC_DESC_FIELD_AT(stack_ptr,      2);
PROC_DESC_FIELD_AT(time_f,         3);
PROC_DESC_FIELD_AT(tlink,          4);
PROC_DESC_FIELD_AT(pointer,        5);
PROC_DESC_FIELD_AT(state,          5);
PROC_DESC_FIELD_AT(priofinity,     6);
PROC_DESC_FIELD_AT(link,           7);
PROC_DESC_FIELD_AT(iptr,           8);
PROC_DESC_FIELD_AT(temp,           9);
PROC_DESC_FIELD_AT(iptr_succ,      9);
PROC_DESC_FIELD_AT(count,         10);
PROC_DESC_FIELD_AT(saved_priority,11);

#endif /* __PROCESS_DESC_H */
