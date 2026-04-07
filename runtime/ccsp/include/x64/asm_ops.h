/*
 *	x86-64 assembler inserts
 *	Copyright (C) 1999-2002 Fred Barnes  <frmb2@ukc.ac.uk>
 *	          (C) 2007      Carl Ritson  <cgr@kent.ac.uk>
 *	          (C) 2025      (x64 adaptation)
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

#include <inlining.h>

#ifndef X64_ASM_OPS_H
#define X64_ASM_OPS_H

/*{{{  barriers */
#define compiler_barrier()	__asm__ __volatile__ ( ""       : : : "memory" )
/* SSE2 is always available on x86-64 */
#define strong_memory_barrier()	__asm__ __volatile__ ( "mfence"	: : : "memory" )
#define strong_read_barrier()  	__asm__ __volatile__ ( "lfence"	: : : "memory" )
#define strong_write_barrier()  __asm__ __volatile__ ( "sfence"	: : : "memory" )

#if MAX_RUNTIME_THREADS > 1
#define weak_memory_barrier() 	strong_memory_barrier()
#define weak_read_barrier() 	strong_read_barrier()
#define weak_write_barrier() 	compiler_barrier()
#else
#define weak_memory_barrier() 	compiler_barrier()
#define weak_read_barrier() 	compiler_barrier()
#define weak_write_barrier() 	compiler_barrier()
#endif /* MAX_RUNTIME_THREADS */
/*}}}*/

/*{{{  static INLINE unsigned long bsf (unsigned long v)*/
/* bit-scan forward */
static INLINE unsigned long bsf (unsigned long v)
{
	unsigned long res = 64;
	asm ("bsfq %1,%0\n" : "=r" (res) : "r" (v) : "cc");
	return res;
}
/*}}}*/

/*{{{  static INLINE unsigned long bsr (unsigned long v)*/
/* bit-scan reverse */
static INLINE unsigned long bsr (unsigned long v)
{
	unsigned long res = 64;
	asm ("bsrq %1,%0\n" : "=r" (res) : "r" (v) : "cc");
	return res;
}
/*}}}*/

/*{{{  static INLINE void cli (void)*/
/* clear interrupts enabled (for RMOX build) */
static INLINE void cli (void) {
	__asm__ __volatile__ ("cli\n");
}
/*}}}*/

/*{{{  static INLINE void idle_cpu (void)*/
static INLINE void idle_cpu (void) {
	__asm__ __volatile__ ("	\n"
		"	pause	\n"
		"	pause	\n"
		"	pause	\n"
		"	pause	\n"
	);
}
/*}}}*/

/*{{{  static INLINE unsigned long one_if_nz (unsigned long value, unsigned long mask)*/
static INLINE unsigned long one_if_nz (unsigned long value, unsigned long mask) {
	unsigned char result;
	asm (	"			\n"
		"	testq %1,%2	\n"
		"	setnz %0	\n"
		: "=q" (result)
		: "ir" (mask), "r" (value)
		: "cc"
	);
	return (unsigned long) result;
}
/*}}}*/

/*{{{  static INLINE unsigned long one_if_z (unsigned long value, unsigned long mask)*/
static INLINE unsigned long one_if_z (unsigned long value, unsigned long mask) {
	unsigned char result;
	asm (	"			\n"
		"	testq %1,%2	\n"
		"	setz %0		\n"
		: "=q" (result)
		: "ir" (mask), "r" (value)
		: "cc"
	);
	return (unsigned long) result;
}
/*}}}*/

/*{{{  static WARM unsigned long pick_random_bit (unsigned long mask)*/
static WARM unsigned long pick_random_bit (unsigned long mask) {
#ifdef ENABLE_CPU_TIMERS
	unsigned long l, r;

	l = bsf (mask);
	r = bsr (mask);

	if (l != r) {
		unsigned long m, shift;
		unsigned int lo;

		m = 0xffffffffffffffffUL >> (64 - bsr ((r - l) << 1));

		/* generate a shift from the tsc */
		__asm__ __volatile__ (	"		\n"
			"	rdtsc			\n"
			: "=a" (lo)
			: /* no inputs */
			: "rdx"
		);

		shift 	= ((unsigned long)lo & m) + l;
		shift	&= 0x3f;
		m 	= (mask >> shift) | (mask << (64 - shift));

		return (bsf (m) + shift) & 0x3f;
	} else {
		return l & 0x3f;
	}
#else
	return bsf (mask);
#endif
}
/*}}}*/

/*{{{  static INLINE void serialise (void)*/
/* serialises the instruction stream - the strongest form of barrier */
static INLINE void serialise (void) {
	__asm__ __volatile__ ("		\n"
		"	xorl %%eax, %%eax	\n"
		"	cpuid		\n"
		: /* no outputs */
		: /* no inputs */
		: "cc", "memory", "rax", "rbx", "rcx", "rdx"
	);
}
/*}}}*/

/*{{{  static INLINE void sti (void)*/
/* set interrupts enabled (for RMOX build) */
static INLINE void sti (void) {
	__asm__ __volatile__ ("sti\n");
}
/*}}}*/

/*{{{  static INLINE void xmemcpy (void *src, void *dst, size_t count)*/
static INLINE void xmemcpy (void *src, void *dst, size_t count) {
	asm (	"		\n"
    		"	cld	\n"
		"	rep	\n"
    		"	movsb	\n"
		: /* no outputs */
		: "S" (src), "D" (dst), "c" (count)
	);
}
/*}}}*/

#endif /* !X64_ASM_OPS_H */
