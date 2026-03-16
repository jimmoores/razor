/*
 *	KRoC interface to SP library
 *	Copyright (C) 1996 Michael Poole
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

/*
 * $Source: /proj/ofa/inmoslibs/hostio/RCS/spinterf.c,v $
 *
 * $Id: spinterf.c,v 1.5 1997/10/23 17:11:08 djb1 Exp $
 *
 * (C) Copyright 1996 M. D. Poole <M.D.Poole@ukc.ac.uk>
 * University of Kent at Canterbury
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <spunixhdr.h>
#include <rts.h>

void _fopen (word w[])
{	C_fopen ((word *)w[0], w[1], w[2]);
}

void _fflush (word w[])
{	C_fflush ((word *)w[0], w[1]);
}

void _fclose (word w[])
{	C_fclose ((word *)w[0], w[1]);
}

void _fread (word w[])
{	C_fread ((word *)w[0], w[1], w[2], w[3], (word *)w[4]);
}

void _fgets (word w[])
{	C_fgets ((word *)w[0], w[1], w[2], w[3], (word *)w[4]);
}

void _fwrite (word w[])
{	C_fwrite ((word *)w[0], w[1], w[2], w[3], (word *)w[4]);
}

void _fremove (word w[])
{	C_fremove ((word *)w[0], w[1]);
}

void _frename (word w[])
{	C_frename ((word *)w[0], w[1], w[2]);
}

void _fseek (word w[])
{	C_fseek ((word *)w[0], w[1], w[2], w[3]);
}

void _ftell (word w[])
{	C_ftell ((word *)w[0], w[1], (word *)w[2]);
}

void _comdline (word w[])
{	C_comdline ((word *)w[0], w[1], (word *)w[2], w[3], w[4]);
}

void _getenvval (word w[])
{	C_getenv ((word *)w[0], w[1], (word *)w[2], w[3], w[4]);
}

void _timenow (word w[])
{	C_time ((word *)w[0], (word *)w[1]);
}

void _system (word w[])
{	C_system ((word *)w[0], (word *)w[1], w[2]);
}

void _exitoccam (word w[])      /* MUST not be _exit as there is a Unix function with this name */
{	C_exit ((word *)w[0], w[1]);
}
