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


#ifndef __SPUNIXHDR_H
#define __SPUNIXHDR_H

#include <rts.h>

void call_occam_exit (void);
void C_fopen (word *handle, word p_name, word p_mode);
void C_fflush (word *result, word handle);
void C_fclose (word *result, word handle);
void C_fread (word *result, word handle, word p_buffer, word SIZEbuffer, word *bytes_read);
void C_fgets (word *result, word handle, word p_buffer, word SIZEbuffer, word *bytes_read);
void C_fwrite (word *result, word handle, word p_buffer, word SIZEbuffer, word *bytes_written);
void C_fremove (word *result, word p_fname);
void C_frename (word *result, word p_oldname, word p_newname);
void C_fseek (word *result, word handle, word origin, word position);
void C_ftell (word *result, word handle, word *position);
void C_comdline (word *result, word all, word *len, word p_block, word SIZEblock);
void C_getenv (word *result, word p_envname, word *len, word p_block, word SIZEblock);
void C_time (word *loctime, word *UTCtime);
void C_system (word *result, word *status, word p_block);
void C_exit (word *result, word status);
void C_getkey (word *keyval);
void C_pollkey (word *result, word *keyval);

#endif	/* !__SPUNIXHDR_H */
