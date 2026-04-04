/*
 *	Architecture dependent types (x64 version)
 *	Copyright (C) 2007-2008 Carl Ritson <cgr@kent.ac.uk>
 *	Copyright (C) 2024 Amazon Q Developer (x64 adaptation)
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

#ifndef X64_CCSP_TYPES_H
#define X64_CCSP_TYPES_H

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

/* x64 doesn't use regparm calling convention */
#define REGPARM

#if defined(__GNUC__) && !defined(__aarch64__) && !defined(__x86_64__)
#define _PACK_STRUCT __attribute__ ((packed))
#else
#define _PACK_STRUCT
#endif

typedef int Time;
typedef struct _cputime_t { 
	unsigned long long counter; 
} _PACK_STRUCT cputime_t;

#undef _PACK_STRUCT

#endif /* !X64_CCSP_TYPES_H */

