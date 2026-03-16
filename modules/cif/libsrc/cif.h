/*
	cif.h: C support for C interface to the scheduler
	Copyright (C) 2007  University of Kent

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation, either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library.  If not, see
	<http://www.gnu.org/licenses/>.
 */

#ifndef CIF_H
#define CIF_H

#include <stdarg.h>

/*{{{ pull in CIF basic operations */
/* In the future we might have multiple implementations of CIF. For now, we
 * just include the CCSP CIF header for the basic operations.
 */

#include "ccsp_cif.h"
/*}}}*/

/*{{{ CIF convenience functions */
/*{{{ void ChanInInt (Workspace wptr, Channel *c, int *i) */
static inline void ChanInInt (Workspace wptr, Channel *c, int *i)
{
	ChanIn (wptr, c, i, sizeof(int));
}
/*}}}*/
/*{{{ void ChanOutInt (Workspace wptr, Channel *c, int i) */
static inline void ChanOutInt (Workspace wptr, Channel *c, int i)
{
	ChanOut (wptr, c, &i, sizeof(int));
}
/*}}}*/
/*{{{ void ChanInChar (Workspace wptr, Channel *c, char *b) */
static inline void ChanInChar (Workspace wptr, Channel *c, char *b)
{
	ChanInByte (wptr, c, (byte *) b);
}
/*}}}*/
/*{{{ void ChanOutChar (Workspace wptr, Channel *c, char b) */
static inline void ChanOutChar (Workspace wptr, Channel *c, char b)
{
	ChanOutByte (wptr, c, (byte) b);
}
/*}}}*/
/*{{{ void TimerDelay (Workspace wptr, Time delay) */
static inline void TimerDelay (Workspace wptr, Time delay)
{
	Time t = TimerRead (wptr);
	TimerWait (wptr, Time_PLUS (t, delay));
}
/*}}}*/
/*{{{ void ProcPar (Workspace wptr, int numprocs, ...) */
extern void ProcPar (Workspace wptr, int numprocs, ...);
/*}}}*/
/*{{{ int ProcAlt (Workspace wptr, ...) */
extern int ProcAlt (Workspace wptr, ...);
/*}}}*/
/*{{{ mt_array_t *MTAllocArray (Workspace wptr, word element_type, int dimensions, ...) */
extern mt_array_t *MTAllocArray (Workspace wptr, word element_type, int dimensions, ...);
/*}}}*/
/*{{{ mt_array_t *MTAllocDataArray (Workspace wptr, word element_type, int dimensions, ...) */
extern mt_array_t *MTAllocDataArray (Workspace wptr, int element_size, int dimensions, ...);
/*}}}*/
/*{{{ mt_cb_t *MTAllocChanType (Workspace wptr, int channels, bool shared) */
static inline mt_cb_t *MTAllocChanType (Workspace wptr, int channels, bool shared)
{
	word type = MT_SIMPLE | MT_MAKE_TYPE (MT_CB);

	if (shared)
		type |= MT_CB_SHARED;

	return MTAlloc (wptr, type, channels);
}
/*}}}*/
/*{{{ mt_array_t *MTResize1D (Workspace wptr, mt_array_t *array, int new_size) */

/**
 * Resizes a one-dimensional mobile array, either in-place or by allocating a
 * new array and copying across all the data that will fit inside the new
 * array.  You should use the return pointer instead of the one you passed in
 * (since the array might have been re-allocated in a new location).  The
 * final parameter is the new length (in terms of array elements).
 */
static inline mt_array_t *MTResize1D (Workspace wptr, mt_array_t *array, int new_size)
{
	mt_array_t *ma = (mt_array_t *) MTResize (wptr, MT_RESIZE_DATA, array, new_size);

	if (ma != NULL)
		ma->dimensions[0] = new_size;

	return ma;
}
/*}}}*/
/*}}}*/

#endif
