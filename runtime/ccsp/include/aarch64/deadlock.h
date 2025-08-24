/*
 *	Interface to the deadlock detection stuff (aarch64 version)
 *	Copyright (C) 2000 Fred Barnes
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


#ifndef AARCH64_DEADLOCK_H
#define AARCH64_DEADLOCK_H

#ifdef __DEADLOCK_C
/* architecture dependant stuff */
#define AARCH64_BRANCH_INS	0x14000000  /* B instruction encoding */

#define DEADLOCK_CODE_BLOCK(FN,PN,CP) \
	__asm__ __volatile__ ( "\n" \
		"\tblr %2\t\t\n" \
		: "=r" (FN), "=r" (PN) \
		: "r" (CP) \
		: "cc", "memory")

#endif	/* __DEADLOCK_C */

#endif	/* !AARCH64_DEADLOCK_H */