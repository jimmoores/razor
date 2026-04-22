/*
 *	gen13.c -- workspace size/offset routines
 *	Copyright (C) 2003 Fred Barnes <frmb2@ukc.ac.uk>
 *	With bits from genhdr.h (C) Inmos
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

/*{{{  includes*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "includes.h"
#include "generror.h"
#include "instruct.h"
#include "genhdr.h"
/*}}}*/

/*{{{  int genhdr_ds_min (void)*/
/*
 *	all processes require at least this many slots
 */
int genhdr_ds_min (void)
{
	int slots;

#ifdef PROCESS_PRIORITY
	slots = 3;
#else
	slots = 2;
#endif
	slots += L_process;
	return slots;
}
/*}}}*/
/*{{{  int genhdr_ds_io (void)*/
/*
 *	for processes performing I/O
 *
 *	On 64-bit targets workspace slots are WORD-sized (8 bytes) but PAR
 *	per-process allocation uses INT-sized (4 byte) strides.  When a
 *	process blocks on a channel the CCSP kernel writes WORD-sized fields
 *	below Wptr: Iptr(-1), Link(-2), Priofinity(-3), Pointer(-4).  For the
 *	Pointer field not to corrupt a local variable in the adjacent lower
 *	PAR process (at INT slot DS_IO from that process's Wptr), DS_IO must
 *	satisfy:  4*DS_IO + 5 > 8*4 = 32,  i.e. DS_IO >= 7.
 */
int genhdr_ds_io (void)
{
	int slots;

	slots = genhdr_ds_min () + 1;
	if (bytesperword > 4) {
		if (slots < 7) {
			slots = 7;
		}
	}
	return slots;
}
/*}}}*/
/*{{{  int genhdr_ds_wait (void)*/
/*
 *	for processes that wait on the timer queue
 *
 *	On 64-bit targets the TLink field at Wptr[-5] requires DS_WAIT >= 9
 *	to avoid corrupting local variables in adjacent PAR processes.
 *	Condition: 4*DS_WAIT + 5 > 8*5 = 40, so DS_WAIT >= 9.
 */
int genhdr_ds_wait (void)
{
	int slots;

	slots = genhdr_ds_min () + 3;
	if (bytesperword > 4) {
		if (slots < 9) {
			slots = 9;
		}
	}
	return slots;
}
/*}}}*/
/*{{{  int genhdr_w_time_slot (void)*/
/*
 *	workspace slot used by ALT pre-enabling for timers
 */
int genhdr_w_time_slot (void)
{
	int slot;

#ifdef PROCESS_PRIORITY
	slot = -6;
#else
	slot = -5;
#endif

	return slot;
}
/*}}}*/

