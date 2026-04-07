/*
 *	Portable timer interface definitions (x64 version)
 *	Copyright (C) 1996-1999 Jim Moores
 *	Modifications (C) 2001-2005 Fred Barnes  <frmb@kent.ac.uk>
 *	Modifications (C) 2007 Carl Ritson <cgr@kent.ac.uk>
 *	Copyright (C) 2025 (x64 adaptation)
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

#ifndef X64_TIMER_H
#define X64_TIMER_H

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sched_types.h>
#include <x64/ccsp_types.h>

static inline Time Time_GetTime (sched_t *sched)
{
	#ifdef ENABLE_CPU_TIMERS
	unsigned int lo, hi;
	unsigned long long tsc;
	Time time_in_us;

	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	tsc = ((unsigned long long)hi << 32) | lo;

	/* cpu_factor converts TSC ticks to microseconds */
	time_in_us = (Time)(tsc * (unsigned long long)sched->cpu_factor >> 32);

	return time_in_us;
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
	unsigned int *timeout = sched->timeout.tsc;
	unsigned int lo, hi;

	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));

	return (hi > timeout[1]) || (hi == timeout[1] && lo > timeout[0]);
}

/*
 *	this sets the value in sched->timeout (to something in the future only)
 */
static inline void Time_SetPastTimeout (sched_t *sched, Time delay)
{
	unsigned int *timeout = sched->timeout.tsc;

	__asm__ __volatile__ ("				\n"
		"	mull	%2			\n" /* result = delay * cpu_khz */
		"	movl	%%edx, %%ecx		\n" /* result /= 1024           */
		"	shr	$10, %%ecx		\n" /*  "                       */
		"	movl	%%eax, %%ebx		\n" /*  "                       */
		"	shr	$10, %%ebx		\n" /*  "                       */
		"	andl	$0x003fffff, %%ebx	\n" /*  "                       */
		"	shl	$22, %%edx		\n" /*  "                       */
		"	orl	%%edx, %%ebx		\n" /*  " (side-effect CF = 0)  */
		"	rdtsc				\n" /* rdtsc                    */
		"	addl	%%ebx, %%eax		\n" /* result += rdtsc          */
		"	adcl	%%ecx, %%edx		\n" /*  "                       */
		: "=a" (timeout[0]), "=d" (timeout[1])
		: "g" (sched->cpu_khz), "a" (delay)
		: "ebx", "ecx"
	);
}
#endif /* ENABLE_CPU_TIMERS */

#endif /* X64_TIMER_H */
