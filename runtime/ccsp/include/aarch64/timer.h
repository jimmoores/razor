/*
 *	Portable timer interface definitions (aarch64 version)
 *	Copyright (C) 1996-1999 Jim Moores
 *	Modifications (C) 2001-2005 Fred Barnes  <frmb@kent.ac.uk>
 *	Modifications (C) 2007 Carl Ritson <cgr@kent.ac.uk>
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

#ifndef AARCH64_TIMER_H
#define AARCH64_TIMER_H

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sched_types.h>
#include <aarch64/ccsp_types.h>

static inline Time Time_GetTime (sched_t *sched)
{
	#ifdef ENABLE_CPU_TIMERS
	unsigned long long counter;
	unsigned long long freq;
	
	/* Read the virtual counter */
	__asm__ __volatile__ ("mrs %0, cntvct_el0" : "=r" (counter));
	/* Read the counter frequency */
	__asm__ __volatile__ ("mrs %0, cntfrq_el0" : "=r" (freq));
	
	/* Convert to microseconds */
	return (Time)((counter * 1000000ULL) / freq);
	#else
	return ccsp_rtime ();
	#endif
}

#ifdef ENABLE_CPU_TIMERS
/*
 *	this checks to see whether the time stored in sched->timeout is <= the current time
 */
static inline int Time_PastTimeout (sched_t *sched)
{
	unsigned long long current_counter;
	unsigned long long timeout_counter = sched->timeout.counter;
	
	__asm__ __volatile__ ("mrs %0, cntvct_el0" : "=r" (current_counter));

	return current_counter >= timeout_counter;
}

/*
 *	this sets the value in sched->timeout (to something in the future only)
 */
static inline void Time_SetPastTimeout (sched_t *sched, Time delay)
{
	unsigned long long current_counter;
	unsigned long long freq;
	unsigned long long delay_ticks;
	
	/* Read current counter and frequency */
	__asm__ __volatile__ ("mrs %0, cntvct_el0" : "=r" (current_counter));
	__asm__ __volatile__ ("mrs %0, cntfrq_el0" : "=r" (freq));
	
	/* Convert delay from microseconds to counter ticks */
	delay_ticks = ((unsigned long long)delay * freq) / 1000000ULL;
	
	sched->timeout.counter = current_counter + delay_ticks;
}
#endif /* ENABLE_CPU_TIMERS */

#endif /* AARCH64_TIMER_H */