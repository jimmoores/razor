/*
 *	Interface to the deadlock detection stuff (x64 version)
 *	Copyright (C) 2000 Fred Barnes
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


#ifndef X64_DEADLOCK_H
#define X64_DEADLOCK_H

#ifdef __DEADLOCK_C
/* architecture dependant stuff */
#define X64_JUMP_INS	0xe9

#define DEADLOCK_CODE_BLOCK(FN,PN,CP) \
	__asm__ __volatile__ ( "\n" \
		"	callq *%%rax		\n" \
		: "=a" (FN), "=c" (PN) \
		: "a" (CP) \
		: "cc", "memory")

#endif	/* __DEADLOCK_C */

#endif	/* !X64_DEADLOCK_H */
