/*
 *	Kernel entry points
 *	Copyright (C) 1996-1999 Jim Moores
 *	Based on the KRoC/sparc kernel Copyright (C) 1994-2000 D.C. Wood and P.H. Welch
 *	Modifications Copyright (C) 1999-2005 Fred Barnes  <frmb@kent.ac.uk>
 *	Modifications Copyright (C) 2007 Carl Ritson <cgr@kent.ac.uk>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*{{{  includes*/
#ifdef RMOX_BUILD
	#include <rmox_if.h>
#else	/* !RMOX_BUILD */
	#include <stdlib.h>
	#include <stdio.h>
#endif	/* !RMOX_BUILD */

#include <kernel.h>
#include <arch/sched_asm_inserts.h>
#include <deadlock.h>
#include <kiface.h>
#include <process_desc.h>
/*}}}*/

/*{{{  void ccsp_kernel_entry (word *wptr, word *fptr)*/
void ccsp_kernel_entry (word *wptr, word *fptr)
{
	word top = (word) (((byte *)&fptr) + sizeof(word));
	word sched_base, kernel_stack_top;

	/* Layout on the C stack (growing downward):
	 *
	 *   [high]  ... CIF C function frames ...
	 *           ccsp_kernel_entry frame  (top = &fptr + sizeof(word))
	 *           64KB gap for CIF call frames and IO process workspaces
	 *           sched_t structure        (sizeof(sched_t) bytes)
	 *   [low]   kernel stack grows DOWN from sched_base
	 *
	 * sched->stack = sched_base.  The kernel SP starts at sched_base and
	 * grows downward into the thread stack below (which has plenty of room).
	 * The 64KB gap above sched ensures the CIF asm stub's saved callee-saved
	 * registers (on the C stack above) are well above sched_base, so the
	 * kernel stack frames (which start at sched_base and grow down) cannot
	 * collide with them.
	 */
	/* Leave a gap between the C stack frames above and the sched_t below.
	 * This protects the sched_t from being overlapped by deep C call
	 * chains (e.g., CIF process → _write_error → fputc → libc internals)
	 * that run on the C stack above sched_base after K_ZERO_OUT_JRET
	 * restores SP to sched_base. */
	#define SCHED_STACK_GAP (64 * 1024)
	sched_base = (top - SCHED_STACK_GAP - sizeof(sched_t)) & ~((word)0x3F);

	/* Direct dispatch to kernel_Y_rtthreadinit (Phase 1D Stage 1c).
	 * Previously this looked up the function via ccsp_calltable[K_RTTHREADINIT]
	 * but now references the symbol directly so build_calltable and the
	 * global calltable can be removed.  Y_rtthreadinit takes one input
	 * (the stack base) so its ABI is identical under both legacy and
	 * CCSP_DIRECT_CALL: void f(word p0, sched_t *sched, word *Wptr).
	 *
	 * Phase 4B-IV: K_CALL_HEADER normalises its Wptr parameter to
	 * user-mode by adding CCSP_KCALL_SHIFT_BYTES (because the usual
	 * caller is a tranx86-emitted kcall bracket that shifts Wptr
	 * down before the call).  K_ENTRY is a different, non-bracketed
	 * path, so we pre-subtract KSHIFT here to compensate for the
	 * header's addition and leave the in-kernel Wptr in the correct
	 * (user-mode) form.  Y_rtthreadinit actually ignores its Wptr
	 * parameter entirely, but keeping this correct avoids a
	 * latent bug the first time something uses it. */
	{
		extern void kernel_Y_rtthreadinit (word, void *, word *);
		word *wptr_shifted = (word *)((char *)wptr - CCSP_KCALL_SHIFT_BYTES);
		K_ENTRY ((void *) kernel_Y_rtthreadinit, sched_base, wptr_shifted, fptr);
	}
}
/*}}}*/

/*{{{  void ccsp_occam_entry (word *wptr, word *fptr)*/
/* Entry point to CCSP for occam programs. */
void ccsp_occam_entry (void *ws, unsigned int ws_bytes, word iptr, word *wptr, word *fptr)
{
	PROC_DESC(wptr)->iptr = iptr;
	PROC_DESC(wptr)->priofinity = 0;
	ccsp_give_ws_code ((char *) ws, (int) ws_bytes, (unsigned char *) iptr);
	ccsp_kernel_entry (wptr, fptr);
}
/*}}}*/


/*{{{  void ccsp_cif_bcall0_stub (word *arg)*/
void ccsp_cif_bcall0_stub (word *arg)
{
	typedef word (*_func_ptr)(void);
	_func_ptr func = (_func_ptr) arg[0];
	arg[0] = func();
}
/*}}}*/

/*{{{  void ccsp_cif_bcall1_stub (word *arg)*/
void ccsp_cif_bcall1_stub (word *arg)
{
	typedef word (*_func_ptr)(word);
	_func_ptr func = (_func_ptr) arg[0];
	arg[0] = func(arg[1]);
}
/*}}}*/

/*{{{  void ccsp_cif_bcalln_stub (word *arg)*/
/*
 * Phase 2: K_CIF_BCALLN macro replaced by an in-function switch on
 * argc using typed function pointers.  Same pattern as ExternalCallN
 * in ccsp_cif.h: cap at 8 args, dispatch via the appropriate fn type
 * so the C compiler emits the correct calling convention (arg
 * registers on x64/aarch64, stack pushes on i386 cdecl).
 *
 * On x64, variadic functions like printf require al=0 to indicate
 * that no XMM registers are used for variadic args.  GCC zeroes al
 * automatically for declared variadic types but not for our typed
 * pointer casts -- so we emit `xorl %eax, %eax` ourselves.
 */
void ccsp_cif_bcalln_stub (word *arg)
{
	void *func	= (void *) arg[0];
	word argc	= arg[1];
	word *argv	= (word *) arg[2];
	word ret;

	typedef word (*fn0_t)(void);
	typedef word (*fn1_t)(word);
	typedef word (*fn2_t)(word, word);
	typedef word (*fn3_t)(word, word, word);
	typedef word (*fn4_t)(word, word, word, word);
	typedef word (*fn5_t)(word, word, word, word, word);
	typedef word (*fn6_t)(word, word, word, word, word, word);
	typedef word (*fn7_t)(word, word, word, word, word, word, word);
	typedef word (*fn8_t)(word, word, word, word, word, word, word, word);

#if defined(__x86_64__)
#define _BCALLN_ZERO_AL __asm__ __volatile__ ("xorl %%eax, %%eax" ::: "rax")
#else
#define _BCALLN_ZERO_AL (void)0
#endif

	switch (argc) {
	case 0:
		_BCALLN_ZERO_AL;
		ret = ((fn0_t)func)();
		break;
	case 1:
		_BCALLN_ZERO_AL;
		ret = ((fn1_t)func)(argv[0]);
		break;
	case 2:
		_BCALLN_ZERO_AL;
		ret = ((fn2_t)func)(argv[0], argv[1]);
		break;
	case 3:
		_BCALLN_ZERO_AL;
		ret = ((fn3_t)func)(argv[0], argv[1], argv[2]);
		break;
	case 4:
		_BCALLN_ZERO_AL;
		ret = ((fn4_t)func)(argv[0], argv[1], argv[2], argv[3]);
		break;
	case 5:
		_BCALLN_ZERO_AL;
		ret = ((fn5_t)func)(argv[0], argv[1], argv[2], argv[3], argv[4]);
		break;
	case 6:
		_BCALLN_ZERO_AL;
		ret = ((fn6_t)func)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
		break;
	case 7:
		_BCALLN_ZERO_AL;
		ret = ((fn7_t)func)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
		break;
	default:
		_BCALLN_ZERO_AL;
		ret = ((fn8_t)func)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
		break;
	}
#undef _BCALLN_ZERO_AL

	arg[0] = ret;
}
/*}}}*/

