/*
 *	ccsp.h -- CCSP header for use in external code
 *	Copyright (C) 2007 Carl Ritson <cgr@kent.ac.uk>
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

#ifndef __CCSP_H
#define __CCSP_H

#define MT_DEFINES 1

#if defined(__GNUC__) && !defined(__aarch64__) && !defined(__x86_64__)
#define _PACK_STRUCT __attribute__ ((packed))
#else
#warning "Unable to enforce alignment and packing on structures."
#define _PACK_STRUCT
#endif

#include "ccsp_config.h"
#include "ukcthreads_types.h"
#include "ccsp_consts.h"
#include "mobile_types.h"
#include "ccsp_pony.h"
#include "dmem_if.h"
#include "kiface.h"
#include "ccsp_if.h"
#include "ccsp_stats.h"

typedef struct _ccsp_sched_t {
#if defined(TARGET_CPU_AARCH64) || defined(TARGET_CPU_X64)
	word			stack;
#else
	unsigned int    stack;
#endif
	word		cparam[5];
#if defined(TARGET_CPU_AARCH64) || defined(TARGET_CPU_X64) || defined(__x86_64__) || defined(__aarch64__)
	/* Phase 4D: saved user-mode sp during kernel calls (x64).
	 * Must be at the same offset as sched_t.saved_user_sp (48).
	 * Only present on 64-bit to avoid shifting i386 struct layout. */
	word		*saved_user_sp;
#endif
#ifdef CCSP_HAS_CALLTABLE
	void		*calltable[K_MAX_SUPPORTED];
#endif
	word		mdparam[32];
	unsigned int	index;
	unsigned int	id;
	unsigned int	cpu_factor;
	unsigned int	cpu_khz;
	int		signal_in;
	int		signal_out;
	void		*allocator;
	unsigned int	spin;
	word		pad[8];
} _PACK_STRUCT ccsp_sched_t;

/* Byte offset of priofinity within the full sched_t structure.
 * ccsp_sched_t is a prefix subset of sched_t.  The priofinity field
 * sits after CACHELINE_ALIGN padding in sched_t which cannot be
 * replicated here without pulling in the full sched_types.h header.
 * This constant must match offsetof(sched_t, priofinity) and is
 * validated by a static assertion in the runtime (sched.c).
 *
 * 64-bit targets (aarch64, x86_64) have CCSP_HAS_CALLTABLE undefined
 * (Phase 1D Stage 1d) so the calltable[K_MAX_SUPPORTED] field is
 * omitted, shrinking sched_t by 122 * 8 = 976 bytes.  priofinity
 * therefore moves from offset 1416 to 1416 - 976 = 440. */
#if defined(__aarch64__) || defined(__x86_64__)
#define CCSP_SCHED_PRIOFINITY_OFFSET 440
#endif

#undef _PACK_STRUCT

#if defined(USE_TLS)
  extern __thread	ccsp_sched_t 	*_ccsp_scheduler;
  #define ccsp_scheduler		_ccsp_scheduler
#elif defined(ENABLE_MP)
  ccsp_sched_t 				*local_scheduler (void);
  #define ccsp_scheduler		(local_scheduler ())
#else
  extern ccsp_sched_t			*_ccsp_scheduler;
  #define ccsp_scheduler		_ccsp_scheduler
#endif

#ifdef CCSP_HAS_CALLTABLE
extern void			**_ccsp_calltable;
#endif

/* CIF process trampoline addresses, populated by ccsp_kernel_init.
 * These are code-label addresses returned by the kernel_CIF_*_stub
 * functions (kernel/sched.c) and are used by ccsp_cif.h to dispatch
 * CIF process start/end without going through the per-scheduler
 * calltable[] indirection. */
extern void			*_ccsp_cif_proc_stub;
extern void			*_ccsp_cif_light_proc_stub;
extern void			*_ccsp_cif_endp_resume_stub;

#endif	/* !__CCSP_H */

