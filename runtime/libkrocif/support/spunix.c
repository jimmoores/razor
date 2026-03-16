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
 * $Source: /proj/ofa/inmoslibs/hostio/RCS/spunix.c,v $
 *
 * $Id: spunix.c,v 1.6 1997/10/23 17:10:22 djb1 Exp $
 *
 * (C) Copyright 1996 M.D. Poole <M.D.Poole@ukc.ac.uk>
 * University of Kent at Canterbury
 * Modified by Fred Barnes, 2000, 2001, 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>

#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
	/* just prototype them and hope they exist.. */
	extern char *getenv (const char *name);
	extern int system (const char *string);
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <errno.h>
#if defined(TIME_WITH_SYS_TIME)
#include <sys/time.h>
#include <time.h>
#elif defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#else
#include <time.h>
#endif


#include <assert.h>

#include <ccsp.h>
#include <spunixhdr.h>

extern char *long_cmdline, *short_cmdline;

#define SP_OK 0
#define SP_ERROR 128
#define SP_ACCESS_DENIED (-1)
#define SP_INVALID_HANDLE (-2)
#define SP_BUFFER_OVERFLOW (-3)

void
call_occam_exit (void)
{
	ccsp_exit (0, false);
}

void
C_fopen (word *handle, word p_name, word p_mode)
{
	FILE *Fd;
	Fd = fopen ((char *) p_name, (char *) p_mode);
	*handle = (word) Fd;
}

void
C_fflush (word *result, word handle)
{
	if (fflush ((FILE *) handle)) {
		if (errno == EBADF)
			*result = SP_INVALID_HANDLE;
		else
			*result = SP_ERROR;
	} else
		*result = SP_OK;
}

void
C_fclose (word *result, word handle)
{
	if (fclose ((FILE *) handle)) {
		if (errno == EBADF)
			*result = SP_INVALID_HANDLE;
		else
			*result = SP_ERROR;
	} else
		*result = SP_OK;
}

void
C_fread (word *result, word handle, word p_buffer, word SIZEbuffer, word *bytes_read)
{
	FILE *Fhandle = (handle == 0) ? stdin : (FILE *) handle;
	*bytes_read = (word) fread ((void *) p_buffer, 1, (size_t) SIZEbuffer, Fhandle);
	*result = SP_OK;
}

void
C_fgets (word *result, word handle, word p_buffer, word SIZEbuffer, word *bytes_read)
{
	char terminator;
	FILE *Fhandle = (handle == 0) ? stdin : (FILE *) handle;
	char *str = fgets ((void *) p_buffer, (int) SIZEbuffer, Fhandle);
	if (str == NULL) {
		*bytes_read = 0;
		*result = SP_ERROR;
		return;
	} else
		*bytes_read = (word) strlen (str);
	if (*bytes_read == 0) {
		*result = SP_ERROR;
		return;
	}
	terminator = ((char *) p_buffer)[*bytes_read - 1];
	if (terminator != '\n') {
		while (terminator != '\n')
			terminator = (char) fgetc (Fhandle);	/* read file to end of line */
		*result = SP_BUFFER_OVERFLOW;
	} else {
		while ((terminator == '\n') || (terminator == '\r'))	/* handle DOS text files better */
			terminator = ((char *) p_buffer)[--(*bytes_read)];
		((char *) p_buffer)[++(*bytes_read)] = '\0';
		*result = SP_OK;
	}
}
void
C_fwrite (word *result, word handle, word p_buffer, word SIZEbuffer, word *bytes_written)
{
	FILE *Fhandle = (handle == 1) ? stdout : (handle == 2) ? stderr : (FILE *) handle;
	*bytes_written = (word) fwrite ((void *) p_buffer, 1, (size_t) SIZEbuffer, Fhandle);
	if (handle == 1)
		fflush (Fhandle);
	*result = SP_OK;
}

void
C_fremove (word *result, word p_fname)
{
	if (remove ((char *) p_fname))
		*result = SP_ERROR;
	else
		*result = SP_OK;
}

void
C_frename (word *result, word p_oldname, word p_newname)
{
	if (rename ((char *) p_oldname, (char *) p_newname))
		*result = SP_ERROR;
	else
		*result = SP_OK;
}

void
C_fseek (word *result, word handle, word origin, word position)
{
	if (fseek ((FILE *) handle, (long) position, (int) origin))
		*result = SP_ERROR;
	else
		*result = SP_OK;
}

void
C_ftell (word *result, word handle, word *position)
{
	*position = (word) ftell ((FILE *) handle);
	if ((long)*position < 0)
		*result = SP_ERROR;
	else
		*result = SP_OK;
}

void
C_comdline (word *result, word all, word *len, word p_block, word SIZEblock)
{
	if (all)
		strcpy ((char *) p_block, long_cmdline);
	else
		strcpy ((char *) p_block, short_cmdline);
	*len = (word) strlen ((char *) p_block);
	assert (*len < (SIZEblock - 1));
	*result = SP_OK;
}

void
C_getenv (word *result, word p_envname, word *len, word p_block, word SIZEblock)
{
	char *Name;

	Name = (char *) getenv ((char *) p_envname);
	if (Name == NULL) {
		*result = SP_ERROR;
	} else {
		*len = (word) strlen (Name);
		assert (*len < (SIZEblock - 1));
		(void) strcpy ((char *) p_block, Name);
		*result = SP_OK;
	}
}
void
C_time (word *loctime, word *UTCtime)
{
	struct timeval tp;
	(void) gettimeofday (&tp, 0);
	*loctime = (word) tp.tv_sec;
	*UTCtime = 0;
}

void
C_system (word *result, word *rstatus, word p_block)
{
	*rstatus = (word) system ((char *) p_block);
	*result = SP_OK;
}

void
C_exit (word *result, word estatus)
{
	call_occam_exit ();
	exit ((int) estatus);		/* never returns */
}
