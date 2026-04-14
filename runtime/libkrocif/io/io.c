/*
 *	krocif.c -- kroc interface (separated out from CCSP run-time)
 *	Copyright (C) 2006 Fred Barnes <frmb@kent.ac.uk>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

/* link with CCSP */
#include <ccsp.h>

/* local headers */
#include <tlpcodes.h>
#include <kroc_io.h>

/* constants */
#define KBD_WORKSPACE_WORDS 64
#define SCR_WORKSPACE_WORDS 64
#define ERR_WORKSPACE_WORDS 64

static int stdin_is_tty;

extern int using_keyboard;
extern FILE *kroc_out, *kroc_err;

/*{{{  workspaces/channels/etc. for keyboard, screen and error processes*/
static word *kbd_workspace_bottom;
static word *scr_workspace_bottom;
static word *err_workspace_bottom;

/* pointers to the right offsets into each workspace for convenience */
static word *kbd_ws;
static word *scr_ws;
static word *err_ws;

/* actual locations holding channel-words -- large if SHARED anon-ct */
static word **kbd_chan;
static word **scr_chan;
static word **err_chan;

static word *kbd_termchan;

/* defined in generate code */
extern void O_kroc_screen_process (void);
extern void O_kroc_error_process (void);
extern void O_kroc_keyboard_process (void);
/*}}}*/

/*{{{  static word **setup_chan (word init_state) */
static word **setup_chan (word init_state)
{
	mt_cb_t *cb = ccsp_mt_alloc (
		MT_SIMPLE | MT_MAKE_TYPE (MT_CB) | MT_CB_SHARED | MT_CB_STATE_SPACE, 
		1
	);

	/* cb->channels[0] = NotProcess_p; */
	mt_cb_get_pony_state (cb)->state = init_state;

	return (word **) cb;
}
/*}}}*/

/*{{{  void init_occam_io (int tlpiface)*/
/*
 *	initialises the occam IO workspaces
 */
int init_occam_io (int tlpiface)
{
	int i;

	#if defined(DM_DEBUG) && (DM_DEBUG == 1)
		extadd_ord_mem (_kbd_workspace_bottom, 13 * sizeof(word), MODE_READ | MODE_WRITE);
		extadd_ord_mem (_scr_workspace_bottom, 128 * sizeof(word), MODE_READ | MODE_WRITE);
		extadd_ord_mem (_err_workspace_bottom, 12 * sizeof(word), MODE_READ | MODE_WRITE);
		extadd_ord_mem (kbd_chan, 8 * sizeof(word), MODE_READ | MODE_WRITE);
		extadd_ord_mem (scr_chan, 8 * sizeof(word), MODE_READ | MODE_WRITE);
		extadd_ord_mem (err_chan, 8 * sizeof(word), MODE_READ | MODE_WRITE);
	#endif

	kbd_workspace_bottom = (word *) dmem_alloc (KBD_WORKSPACE_WORDS * sizeof(word));
	for (i = 0; i < KBD_WORKSPACE_WORDS; ++i) {
		kbd_workspace_bottom[i] = 0;
	}

	scr_workspace_bottom = (word *) dmem_alloc (SCR_WORKSPACE_WORDS * sizeof(word));
	for (i = 0; i < SCR_WORKSPACE_WORDS; ++i) {
		scr_workspace_bottom[i] = 0;
	}

	err_workspace_bottom = (word *) dmem_alloc (ERR_WORKSPACE_WORDS * sizeof(word));
	for (i = 0; i < ERR_WORKSPACE_WORDS; ++i) {
		err_workspace_bottom[i] = 0;
	}
	
	kbd_chan = setup_chan (0x00020001); /* server shared not claimed, client unshared */
	scr_chan = setup_chan (0x00010002); /* client shared not claimed, server unshared */
	err_chan = setup_chan (0x00010002); /* client shared not claimed, server unshared */

	/*
	 * Phase 4B-III C3: the per-call descriptor has moved from
	 * Wptr[-1..-9] to Wptr[+0..+11].  Wptr needs to be positioned
	 * so that there are at least PROC_DESC_BIAS (= 12) slots above
	 * it (for the descriptor area).  Previously Wptr was placed 4
	 * slots from the top (plenty of space below for deeply-nested
	 * call chains, only 4 slots above for the legacy positive
	 * slots temp/count/saved_priority).  Now we need 12 slots
	 * above, so bump the Wptr position down by that amount
	 * (equivalently: leave more space above the Wptr).
	 *
	 * Iptr, Link, Priofinity etc. come from ccsp_consts.h, which
	 * has been updated to the positive-offset values.
	 */
	kbd_ws = &(kbd_workspace_bottom[KBD_WORKSPACE_WORDS - 12]);
	kbd_ws[Priofinity] = 0;
	kbd_ws[Link] = (word) NotProcess_p;
	/* Get the address of the occam-generated symbol, bypassing C name-mangling. */
#if defined(__aarch64__) && defined(__APPLE__)
	asm ("adrp %0, _O_kroc_keyboard_process@PAGE\n\t"
		 "add  %0, %0, _O_kroc_keyboard_process@PAGEOFF" : "=r" (kbd_ws[Iptr]));
#elif defined(__aarch64__)
	asm ("adrp %0, :got:O_kroc_keyboard_process\n\t"
		 "ldr  %0, [%0, :got_lo12:O_kroc_keyboard_process]" : "=r" (kbd_ws[Iptr]));
#elif defined(__x86_64__)
	asm ("leaq O_kroc_keyboard_process(%%rip), %0" : "=r" (kbd_ws[Iptr]));
#elif defined(__i386__)
	asm ("movl $O_kroc_keyboard_process, %0" : "=r" (kbd_ws[Iptr]));
#endif
	/* Zero the descriptor area (skipping Iptr which we just set). */
	for (i = 0; i < 12; i++) {
		if (i != Iptr) kbd_ws[i] = 0;
	}
	kbd_ws[Link] = (word) NotProcess_p;
	/*
	 * The following slots carry "arguments" to the keyboard PROC.
	 * In the biased layout the compiler reads these via LDL with
	 * biased operands, but the bias cancels across AJW so the
	 * physical addresses are the same: arg N lives at Wptr[+N]
	 * below the AJW and at (biased Wptr) + byte ((N+BIAS)*WSH)
	 * after AJW -(L+BIAS).  We still write at the raw positive
	 * slot index because that's the physical offset below the
	 * pre-AJW Wptr.  PROC_DESC_BIAS slots are in use by the
	 * descriptor so the first arg slot is now at position
	 * PROC_DESC_BIAS (not 1).
	 */
	kbd_ws[PROC_DESC_BIAS + 0] = (word) kbd_chan;
	kbd_ws[PROC_DESC_BIAS + 1] = (word) &kbd_termchan;
	kbd_termchan = NotProcess_p;

	scr_ws = &(scr_workspace_bottom[SCR_WORKSPACE_WORDS - 12]);
	for (i = 0; i < 12; i++) {
		scr_ws[i] = 0;
	}
	scr_ws[Link] = (word) NotProcess_p;
	/* Get the address of the occam-generated symbol, bypassing C name-mangling. */
#if defined(__aarch64__) && defined(__APPLE__)
	asm ("adrp %0, _O_kroc_screen_process@PAGE\n\t"
	     "add  %0, %0, _O_kroc_screen_process@PAGEOFF" : "=r" (scr_ws[Iptr]));
#elif defined(__aarch64__)
	asm ("adrp %0, :got:O_kroc_screen_process\n\t"
	     "ldr  %0, [%0, :got_lo12:O_kroc_screen_process]" : "=r" (scr_ws[Iptr]));
#elif defined(__x86_64__)
	asm ("leaq O_kroc_screen_process(%%rip), %0" : "=r" (scr_ws[Iptr]));
#elif defined(__i386__)
	asm ("movl $O_kroc_screen_process, %0" : "=r" (scr_ws[Iptr]));
#endif
	scr_ws[PROC_DESC_BIAS + 0] = (word) scr_chan;

	err_ws = &(err_workspace_bottom[ERR_WORKSPACE_WORDS - 12]);
	for (i = 0; i < 12; i++) {
		err_ws[i] = 0;
	}
	err_ws[Link] = (word) NotProcess_p;
	/* Get the address of the occam-generated symbol, bypassing C name-mangling. */
#if defined(__aarch64__) && defined(__APPLE__)
	asm ("adrp %0, _O_kroc_error_process@PAGE\n\t"
	     "add  %0, %0, _O_kroc_error_process@PAGEOFF" : "=r" (err_ws[Iptr]));
#elif defined(__aarch64__)
	asm ("adrp %0, :got:O_kroc_error_process\n\t"
	     "ldr  %0, [%0, :got_lo12:O_kroc_error_process]" : "=r" (err_ws[Iptr]));
#elif defined(__x86_64__)
	asm ("leaq O_kroc_error_process(%%rip), %0" : "=r" (err_ws[Iptr]));
#elif defined(__i386__)
	asm ("movl $O_kroc_error_process, %0" : "=r" (err_ws[Iptr]));
#endif
	err_ws[PROC_DESC_BIAS + 0] = (word) err_chan;

	return 0;
}
/*}}}*/
/*{{{  bool kbd_ready (void)*/
/*
 *	returns true if the keyboard handler is blocked on the keyboard
 *	channel.
 */
bool kbd_ready (void)
{
	if ((kbd_chan[0] != NotProcess_p) && (kbd_chan[0] >= kbd_workspace_bottom) && (kbd_chan[0] <= kbd_ws)) {
		return true;
	}
	return false;
}
/*}}}*/
/*{{{  bool process_blocked_on_kbd (void)*/
/*
 *	returns true if a process other than the keyboard handler is
 *	blocked on the keyboard channel.
 */
bool process_blocked_on_kbd (void)
{
	if ((kbd_chan[0] != NotProcess_p) && ((kbd_chan[0] < kbd_workspace_bottom) || (kbd_chan[0] > kbd_ws))) {
		return true;
	}
	return false;
}
/*}}}*/


/*{{{  word *kbd_chan_addr (void)*/
word *kbd_chan_addr (void)
{
	return (word *) &(kbd_chan[0]);
}
/*}}}*/
/*{{{  word *scr_chan_addr (void)*/
word *scr_chan_addr (void)
{
	return (word *) &(scr_chan[0]);
}
/*}}}*/
/*{{{  word *err_chan_addr (void)*/
word *err_chan_addr (void)
{
	return (word *) &(err_chan[0]);
}
/*}}}*/

/*{{{  word *kbd_workspace (void)*/
word *kbd_workspace (void)
{
	return (word *) kbd_ws;
}
/*}}}*/
/*{{{  word *scr_workspace (void)*/
word *scr_workspace (void)
{
	return (word *) scr_ws;
}
/*}}}*/
/*{{{  word *err_workspace (void)*/
word *err_workspace (void)
{
	return (word *) err_ws;
}
/*}}}*/


/*{{{  void init_kbdio (int is_a_tty)*/
/*
 *	initialises keyboard (or input) stuff
 */
void init_kbdio (int is_a_tty)
{
	stdin_is_tty = is_a_tty;
	return;
}
/*}}}*/
#if 0
/*{{{  int kill_kbdio (void)*/
/*
 *	kills the "keyboard process" (which might be blocked in terminal read)
 */
int kill_kbdio (void)
{
	word ws_arry[2];
	int status;

	if (using_keyboard) {
		ws_arry[0] = (word) &kbd_termchan;
		ws_arry[1] = (word) &status;
		_killcall (ws_arry);
	} else {
		status = 0;
	}
	return status;
}
/*}}}*/
#endif
/*{{{  void read_keyboard (word *wsptr)*/
/*
 *	reads the keyboard and places a byte at (char *)wsptr[0]
 */
void read_keyboard (word *wsptr)
{
	word *ch = (word *)(wsptr[0]);
	int c_read, n;
	unsigned char tty_char;

	if (!stdin_is_tty) {
		c_read = getc (stdin);
		n = (c_read == EOF) ? 0 : 1;
		tty_char = (unsigned char)c_read;
	} else {
		n = read (0, &tty_char, 1);
	}
	if (n < 1) {
		*ch = -1;
	} else {
		*ch = (int)((word) tty_char);
	}
	return;
}
/*}}}*/

/*  output handling*/
#define KROC_OUTPUT_BUFFER_SIZE 256

/*{{{  void write_screen (word *wsptr)*/
/*
 *	krocif.s calls this to print something on the screen channel
 *	PROC C.write.screen (VAL []BYTE buffer)
 */
void write_screen (word *wsptr)
{
	const char *buffer = (char *)(wsptr[0]);

	fputs (buffer, kroc_out);
	fflush (kroc_out);
	return;
}
/*}}}*/
/*{{{  void write_error (word *wsptr)*/
/*
 *	krocif.s calls this to print something on the error channel
 *	PROC C.write.error (VAL BYTE ch)
 */
void write_error (word *wsptr)
{
	fputc ((int)(wsptr[0]), kroc_err);
	fflush (kroc_err);
	return;
}
/*}}}*/
/*{{{  void out_stderr (word *wsptr)*/
/*
 *	this is available for debugging
 *	PROC C.out.stderr (VAL []BYTE str)
 */
void out_stderr (word *wsptr)
{
	const char *buf = (char*)(wsptr[0]);
	int slen = (int)(wsptr[1]);

	fflush (kroc_err);
	write (fileno (kroc_err), buf, slen);
	return;
}
/*}}}*/
/*{{{  void out_stderr_int (word *wsptr)*/
/*
 *	this is available for debugging
 *	PROC C.out.stderr.int (VAL INT n)
 */
void out_stderr_int (word *wsptr)
{
	const int n = (int)(wsptr[0]);

	fprintf (kroc_err, "%d", n);
	fflush (kroc_err);
	return;
}
/*}}}*/

/* Create aliases for external linkage from occam code.
 * The occam toolchain expects these exact symbol names.
 * On macOS, C symbols get a leading underscore automatically, so
 * _read_keyboard becomes __read_keyboard in the object file.
 * On Linux, there is no automatic prefix, so we need _read_keyboard. */
#if defined(__APPLE__)
void _read_keyboard (word *wsptr) asm("__read_keyboard");
void _write_screen (word *wsptr) asm("__write_screen");
void _write_error (word *wsptr) asm("__write_error");
void _out_stderr (word *wsptr) asm("__out_stderr");
void _out_stderr_int (word *wsptr) asm("__out_stderr_int");
#else
void _read_keyboard (word *wsptr) asm("_read_keyboard");
void _write_screen (word *wsptr) asm("_write_screen");
void _write_error (word *wsptr) asm("_write_error");
void _out_stderr (word *wsptr) asm("_out_stderr");
void _out_stderr_int (word *wsptr) asm("_out_stderr_int");
#endif
void _read_keyboard (word *wsptr) { read_keyboard(wsptr); }
void _write_screen (word *wsptr) { write_screen(wsptr); }
void _write_error (word *wsptr) { write_error(wsptr); }
void _out_stderr (word *wsptr) { out_stderr(wsptr); }
void _out_stderr_int (word *wsptr) { out_stderr_int(wsptr); }
