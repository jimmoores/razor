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
	 * Phase 4B-III C3: the per-call descriptor spans slot offsets
	 * Wptr[+0..+11] *from the post-AJW Wptr*, not from the
	 * pre-AJW value we store into scr_ws / kbd_ws / err_ws here.
	 *
	 * The compiler now biases every PROC's AJW by PROC_DESC_BIAS
	 * (=12) AND biases every LDL/STL/LDLP operand by the same
	 * amount, so the bias cancels across the frame: a user local
	 * or arg at slot N is still at physical address
	 * (pre-AJW Wptr) + N*sizeof(word), same as pre-Phase-4B.  We
	 * therefore write arg N at scr_ws[N], NOT at scr_ws[BIAS+N].
	 *
	 * What *does* change is that when the scheduler jumps to
	 * O_kroc_..._process, the PROC's entry prolog does
	 * AJW -(L+PROC_DESC_BIAS) rather than AJW -L.  That drops
	 * Wptr by an extra PROC_DESC_BIAS slots below scr_ws; the
	 * resulting space is used for the post-AJW descriptor
	 * (the metadata slots sched.c reads via PROC_DESC(Wptr)->X).
	 * Since L + PROC_DESC_BIAS is at most a couple dozen words
	 * even for the larger kbd/scr/err processes, the existing
	 * ..._WORKSPACE_WORDS allocation is still comfortably big
	 * enough.
	 *
	 * Iptr, Link, Priofinity, ... come from ccsp_consts.h and are
	 * the positive-offset values.  These writes go into the
	 * scheduler-visible slots *at* scr_ws / kbd_ws / err_ws,
	 * which is the pre-AJW Wptr the scheduler sees when it picks
	 * the process off the run-queue.
	 */
	kbd_ws = &(kbd_workspace_bottom[KBD_WORKSPACE_WORDS - 4]);
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
	 * Args at raw positive slots starting at scr_ws[+1] (the same
	 * physical offsets the pre-Phase-4B runtime used).  The PROC's
	 * own AJW -(L+PROC_DESC_BIAS) moves the runtime-visible Wptr
	 * down by L+12 slots, and the compiler biases its LDL operand
	 * for arg N up by the same amount, so (biased LDL) reads
	 * scr_ws + (N+1)*WSH physically.  These writes are below the
	 * scheduler-visible descriptor area (Wptr[+0] / EscapePtr at
	 * scr_ws[0] is the only slot collision, and it's zeroed anyway
	 * because scr_process is a plain-occam PROC with no CIF
	 * escape_ptr in use).
	 */
	kbd_ws[1] = (word) kbd_chan;
	kbd_ws[2] = (word) &kbd_termchan;
	kbd_termchan = NotProcess_p;

	scr_ws = &(scr_workspace_bottom[SCR_WORKSPACE_WORDS - 4]);
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
	scr_ws[1] = (word) scr_chan;

	err_ws = &(err_workspace_bottom[ERR_WORKSPACE_WORDS - 4]);
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
	err_ws[1] = (word) err_chan;

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
