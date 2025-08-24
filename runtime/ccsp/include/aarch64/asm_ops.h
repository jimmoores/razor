/*
 *	AArch64 assembler inserts
 *	Copyright (C) 1999-2002 Fred Barnes  <frmb2@ukc.ac.uk>
 *	          (C) 2007      Carl Ritson  <cgr@kent.ac.uk>
 *	          (C) 2024      Amazon Q Developer (aarch64 adaptation)
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

#ifndef AARCH64_ASM_OPS_H
#define AARCH64_ASM_OPS_H

/*{{{  barriers */
#define compiler_barrier()	__asm__ __volatile__ ( ""       : : : "memory" )
#define strong_memory_barrier()	__asm__ __volatile__ ( "dmb sy"	: : : "memory" )
#define strong_read_barrier()  	__asm__ __volatile__ ( "dmb ld"	: : : "memory" )
#define strong_write_barrier()  __asm__ __volatile__ ( "dmb st"	: : : "memory" )

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

/*{{{  static INLINE unsigned int bsf (unsigned int v)*/
/* bit-scan forward - find first set bit */
static INLINE unsigned int bsf (unsigned int v)
{
	if (v == 0) return 32;
	return __builtin_ctz(v);
}
/*}}}*/

/*{{{  static INLINE unsigned int bsr (unsigned int v)*/
/* bit-scan reverse - find last set bit */
static INLINE unsigned int bsr (unsigned int v)
{
	if (v == 0) return 32;
	return 31 - __builtin_clz(v);
}
/*}}}*/

/*{{{  static INLINE void cli (void)*/
/* clear interrupts enabled (for RMOX build) - no-op on aarch64 */
static INLINE void cli (void) {
	/* No direct equivalent on aarch64 in user space */
}
/*}}}*/

/*{{{  static INLINE void idle_cpu (void)*/
static INLINE void idle_cpu (void) {
	__asm__ __volatile__ ("yield");
}
/*}}}*/

/*{{{  static INLINE unsigned int one_if_nz (unsigned int value, unsigned int mask)*/
static INLINE unsigned int one_if_nz (unsigned int value, unsigned int mask) {
	return (value & mask) ? 1 : 0;
}
/*}}}*/

/*{{{  static INLINE unsigned int one_if_z (unsigned int value, unsigned int mask)*/
static INLINE unsigned int one_if_z (unsigned int value, unsigned int mask) {
	return (value & mask) ? 0 : 1;
}
/*}}}*/

/*{{{  static WARM unsigned int pick_random_bit (unsigned int mask)*/
static WARM unsigned int pick_random_bit (unsigned int mask) {
#ifdef ENABLE_CPU_TIMERS
	unsigned int l, r;

	l = bsf (mask);
	r = bsr (mask);

	if (l != r) {
		unsigned int m, shift;
		unsigned long long counter;

		m = 0xffffffff >> (32 - bsr ((r - l) << 1));

		/* generate a shift from the cycle counter */
		__asm__ __volatile__ ("mrs %0, cntvct_el0" : "=r" (counter));
		
		shift 	= ((unsigned int)counter & m) + l;
		shift	&= 0x1f;
		m 	= (mask >> shift) | (mask << (32 - shift));

		return (bsf (m) + shift) & 0x1f;
	} else {
		return l & 0x1f;
	}
#else
	return bsf (mask);
#endif
}
/*}}}*/

/*{{{  static INLINE void serialise (void)*/
/* serialises the instruction stream - the strongest form of barrier */
static INLINE void serialise (void) {
	__asm__ __volatile__ ("dsb sy; isb" : : : "memory");
}
/*}}}*/

/*{{{  static INLINE void sti (void)*/
/* set interrupts enabled (for RMOX build) - no-op on aarch64 */
static INLINE void sti (void) {
	/* No direct equivalent on aarch64 in user space */
}
/*}}}*/

/*{{{  static INLINE void xmemcpy (void *src, void *dst, unsigned int count)*/
static INLINE void xmemcpy (void *src, void *dst, unsigned int count) {
	/* Use standard memcpy for aarch64 */
	__builtin_memcpy(dst, src, count);
}
/*}}}*/

#endif /* !AARCH64_ASM_OPS_H */