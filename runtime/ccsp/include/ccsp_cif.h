/*
 *	ccsp_cif.h -- CCSP interface for C currency
 *	Copyright (C) 2007 Carl Ritson <cgr@kent.ac.uk>
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

#ifndef CCSP_CIF_H
#define CCSP_CIF_H

#define CCSP_CIF

#include <stdarg.h>
#include <stdlib.h>
#include <ccsp.h>
#include <ccsp_timer.h>
#include <process_desc.h>

/*{{{  void ccsp_cif_noreturn(void) */
#define ccsp_cif_noreturn() { exit(1); }
/*}}}*/

/* Phase 2: CIF helpers extracted from inline asm to real functions
 * in the per-arch *_cif.S file.  Declared here so the static inline
 * helpers below (and any user CIF code) can call them. */
extern word ccsp_cif_external_call (void *func, void *stack);
extern void ccsp_cif_jump (void *wptr, void *addr) __attribute__((noreturn));
extern void ccsp_cif_occam_call (void *sched, void *stack, word *ws, void *func, word top);

#include <ccsp_cif_stubs.h>

typedef word Channel;
typedef word *Workspace;
typedef void (*Process)(Workspace);
typedef struct _light_proc_bar_t LightProcBarrier;

/*{{{ Workspace and Stack Constants */
#define WORKSPACE_SIZE(args, stack) \
	((args) + (stack) + CIF_PROCESS_WORDS + 2 + CIF_STACK_LINKAGE + (CIF_STACK_ALIGN - 1))
/*}}}*/

/*{{{ LightProcBarrier */
struct _light_proc_bar_t {
	word data[CIF_PROCESS_WORDS + 4];
};
/*}}}*/

/*{{{ ALTing */
/*{{{  void Alt (Workspace wptr) */
static inline void Alt (Workspace wptr)
{
	ccsp_cif_X_alt (wptr);
}
/*}}}*/
/*{{{  void TimerAlt (Workspace wptr) */
static inline void TimerAlt (Workspace wptr)
{
	ccsp_cif_X_talt (wptr);
}
/*}}}*/
/*{{{  bool AltEnableChannel (Workspace wptr, int id, Channel *c) */
static inline bool AltEnableChannel (Workspace wptr, int id, Channel *c)
{
	bool fired;
	ccsp_cif_X_cenbc (wptr, id, c, fired);
	return fired;
}
/*}}}*/
/*{{{  bool AltEnableTimer (Workspace wptr, int id, Time timeout) */
static inline bool AltEnableTimer (Workspace wptr, int id, Time timeout)
{
	bool fired;
	ccsp_cif_X_cenbt (wptr, id, timeout, fired);
	return fired;
}
/*}}}*/
/*{{{  bool AltEnableSkip (Workspace wptr, int id) */
static inline bool AltEnableSkip (Workspace wptr, int id)
{
	bool fired;
	ccsp_cif_X_cenbs (wptr, id, fired);
	return fired;
}
/*}}}*/
/*{{{  bool AltDisableChannel (Workspace wptr, int id, Channel *c) */
static inline bool AltDisableChannel (Workspace wptr, int id, Channel *c)
{
	bool fired;
	ccsp_cif_X_cdisc (wptr, id, c, fired);
	return fired;
}
/*}}}*/
/*{{{  bool AltDisableTimer (Workspace wptr, int id, Time timeout) */
static inline bool AltDisableTimer (Workspace wptr, int id, Time timeout)
{
	bool fired;
	ccsp_cif_X_cdist (wptr, id, timeout, fired);
	return fired;
}
/*}}}*/
/*{{{  bool AltDisableSkip (Workspace wptr, int id) */
static inline bool AltDisableSkip (Workspace wptr, int id)
{
	bool fired;
	ccsp_cif_X_cdiss (wptr, id, fired);
	return fired;
}
/*}}}*/
/*{{{  void AltWait (Workspace wptr) */
static inline void AltWait (Workspace wptr)
{
	ccsp_cif_Y_altwt (wptr);
}
/*}}}*/
/*{{{  void TimerAltWait (Workspace wptr) */
static inline void TimerAltWait (Workspace wptr)
{
	ccsp_cif_Y_taltwt (wptr);
}
/*}}}*/
/*{{{  int AltEnd (Workspace wptr) */
static inline int AltEnd (Workspace wptr)
{
	ccsp_cif_Y_caltend (wptr);
	return (int) PROC_DESC(wptr)->temp;
}
/*}}}*/
/*}}}*/

/*{{{ Channels */
/*{{{  void ChanInit (Workspace wptr, Channel *c) */
static inline void ChanInit (Workspace wptr, Channel *c)
{
	*c = NotProcess_p;
}
/*}}}*/
/*{{{  void ChanIn (Workspace wptr, Channel *c, void *buffer, word length) */
static inline void ChanIn (Workspace wptr, Channel *c, void *buffer, word length)
{
	ccsp_cif_Y_in (wptr, length, c, buffer);
}
/*}}}*/
/*{{{  void ChanOut (Workspace wptr, Channel *c, const void *buffer, word length) */
static inline void ChanOut (Workspace wptr, Channel *c, const void *buffer, word length)
{
	ccsp_cif_Y_out (wptr, length, c, buffer);
}
/*}}}*/
/*{{{  void ChanInByte (Workspace wptr, Channel *c, byte *buffer) */
static inline void ChanInByte (Workspace wptr, Channel *c, byte *buffer)
{
	ccsp_cif_Y_in8 (wptr, c, buffer);
}
/*}}}*/
/*{{{  void ChanOutByte (Workspace wptr, Channel *c, byte b) */
static inline void ChanOutByte (Workspace wptr, Channel *c, byte b)
{
	ccsp_cif_Y_outbyte (wptr, b, c);
}
/*}}}*/
/*{{{  void ChanInWord (Workspace wptr, Channel *c, word *buffer) */
static inline void ChanInWord (Workspace wptr, Channel *c, word *buffer)
{
	ChanIn (wptr, c, buffer, sizeof (word));
}
/*}}}*/
/*{{{  void ChanOutWord (Workspace wptr, Channel *c, word w) */
static inline void ChanOutWord (Workspace wptr, Channel *c, word w)
{
	ccsp_cif_Y_outword (wptr, w, c);
}
/*}}}*/
/*{{{  void ChanXAble (Workspace wptr, Channel *c) */
static inline void ChanXAble (Workspace wptr, Channel *c)
{
	ccsp_cif_Y_xable (wptr, c);
}
/*}}}*/
/*{{{  void ChanXEnd (Workspace wptr, Channel *c)*/
static inline void ChanXEnd (Workspace wptr, Channel *c)
{
	ccsp_cif_X_xend (wptr, c);
}
/*}}}*/
/*{{{  void ChanXIn (Workspace wptr, Channel *c, void *buffer, word length) */
static inline void ChanXIn (Workspace wptr, Channel *c, void *buffer, word length)
{
	ccsp_cif_Y_xin (wptr, length, c, buffer);
}
/*}}}*/
/*}}}*/

/*{{{ Errors */
/*{{{  void _SetErrW(Workspace wptr, char *file, word line) */
static inline void _SetErrW(Workspace wptr, char *file, word line)
{
	line |= 0xfe000000;

	ccsp_cif_Y_Seterr (wptr, "(external)", file, 0, line);
}
/*}}}*/
/*{{{  void SetErrW(Workspace wptr) */
#define SetErrW(wptr) _SetErrW ((wptr), __FILE__, __LINE__)
/*}}}*/
/*{{{  void SetErr(void) */
#define SetErr() \
	do { word ws[CIF_PROCESS_WORDS + 1]; SetErrW (&(ws[CIF_PROCESS_WORDS])); } while (0)
/*}}}*/
/*}}}*/
#if !defined(RMOX_BUILD) && defined(BLOCKING_SYSCALLS)
/*{{{  External Calls */
/*{{{  word BlockingCall0 (Workspace wptr, void *func)*/
static inline word BlockingCall0 (Workspace wptr, void *func)
{
	word arg = (word) func;
	word *tmp = &arg;

	ccsp_cif_Y_b_dispatch (wptr, ccsp_cif_bcall0_stub, tmp);

	return arg;
}
/*}}}*/
/*{{{  word BlockingCall1 (Workspace wptr, void *func, word arg)*/
static inline word BlockingCall1 (Workspace wptr, void *func, word arg)
{
	word args[2];
	word *tmp = &(args[0]);
	
	args[0] = (word) func;
	args[1] = arg;

	ccsp_cif_Y_b_dispatch (wptr, ccsp_cif_bcall1_stub, tmp);

	return args[0];
}
/*}}}*/
/*{{{  word BlockingCallN (Workspace wptr, void *func, word argc, ...)*/
#if defined(__GNUC__)
__attribute__ ((unused)) /* make GCC ignore when unused */
#endif
static word BlockingCallN (Workspace wptr, void *func, word argc, ...)
{
	word args[3];
	word va_args[8]; /* max 8 variadic arguments */
	word *tmp = &(args[0]);
	va_list ap;
	word i;

	va_start (ap, argc);
	for (i = 0; i < argc && i < 8; i++) {
		va_args[i] = va_arg (ap, word);
	}
	va_end (ap);

	args[0] = (word) func;
	args[1] = argc;
	args[2] = (word) va_args;

	ccsp_cif_Y_b_dispatch (wptr, ccsp_cif_bcalln_stub, tmp);

	return args[0];
}
/*}}}*/
#endif
/*{{{  word ExternalCall0 (void *func) */
static inline word ExternalCall0 (void *func)
{
	ccsp_sched_t *sched = ccsp_scheduler;
	word *stack = (word *) sched->stack;

	return ccsp_cif_external_call (func, stack);
}
/*}}}*/
/*{{{  word ExternalCall1 (void *func, word arg) */
static inline word ExternalCall1 (void *func, word arg)
{
	ccsp_sched_t *sched = ccsp_scheduler;
	word *stack = (word *) sched->stack;

	*(stack--) = arg;

	return ccsp_cif_external_call (func, stack);
}
/*}}}*/
/*{{{  word ExternalCallN (void *func, word argc, ...) */
#if defined(__GNUC__)
__attribute__ ((unused)) /* make GCC ignore when unused */
#endif
static word ExternalCallN (void *func, word argc, ...)
{
#if defined(__aarch64__) || defined(__x86_64__)
	va_list ap;
	word result = 0;
	word args[8] = {0};
	int i;

	va_start (ap, argc);
	for (i = 0; i < argc && i < 8; ++i) {
		args[i] = va_arg (ap, word);
	}
	va_end (ap);

	/* Cast to typed function pointers so the compiler generates correct
	 * calling convention code.  On x64, variadic functions like printf
	 * require al=0 (no XMM register args).  Use __asm__ to zero al before
	 * each call since the compiler won't do it for non-variadic types. */
	typedef word (*fn0_t)(void);
	typedef word (*fn1_t)(word);
	typedef word (*fn2_t)(word, word);
	typedef word (*fn3_t)(word, word, word);
	typedef word (*fn4_t)(word, word, word, word);
	typedef word (*fn5_t)(word, word, word, word, word);
	typedef word (*fn6_t)(word, word, word, word, word, word);
	typedef word (*fn7_t)(word, word, word, word, word, word, word);
	typedef word (*fn8_t)(word, word, word, word, word, word, word, word);
#if defined(__x86_64__)
#define _ZERO_AL __asm__ __volatile__ ("xorl %%eax, %%eax" ::: "rax");
#else
#define _ZERO_AL
#endif
	switch (argc) {
		case 0: _ZERO_AL result = ((fn0_t)func) (); break;
		case 1: _ZERO_AL result = ((fn1_t)func) (args[0]); break;
		case 2: _ZERO_AL result = ((fn2_t)func) (args[0], args[1]); break;
		case 3: _ZERO_AL result = ((fn3_t)func) (args[0], args[1], args[2]); break;
		case 4: _ZERO_AL result = ((fn4_t)func) (args[0], args[1], args[2], args[3]); break;
		case 5: _ZERO_AL result = ((fn5_t)func) (args[0], args[1], args[2], args[3], args[4]); break;
		case 6: _ZERO_AL result = ((fn6_t)func) (args[0], args[1], args[2], args[3], args[4], args[5]); break;
		case 7: _ZERO_AL result = ((fn7_t)func) (args[0], args[1], args[2], args[3], args[4], args[5], args[6]); break;
		case 8: _ZERO_AL result = ((fn8_t)func) (args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]); break;
	}
#undef _ZERO_AL
	return result;
#else
	ccsp_sched_t *sched = ccsp_scheduler;
	word *stack = (word *) sched->stack;
	va_list ap;
	int i;

	va_start (ap, argc);

	stack -= (argc & (~(CIF_STACK_ALIGN - 1))) + CIF_STACK_ALIGN;

	for (i = 0; i < argc; ++i) {
		stack[i] = va_arg (ap, word);
	}

	va_end (ap);

	return ccsp_cif_external_call (func, stack);
#endif
}
/*}}}*/
#if !defined(RMOX_BUILD) && defined(BLOCKING_SYSCALLS)
/*{{{  word KillableBlockingCallN (Workspace wptr, void *func, word argc, ...)*/
#if defined(__GNUC__)
__attribute__ ((unused)) /* make GCC ignore when unused */
#endif
static word KillableBlockingCallN (Workspace wptr, void *func, Channel *killchan, word argc, ...)
{
	word args[4];
	word va_args[8]; /* max 8 variadic arguments */
	word *tmp = &(args[0]);
	va_list ap;
	word i;

	va_start (ap, argc);
	for (i = 0; i < argc && i < 8; i++) {
		va_args[i] = va_arg (ap, word);
	}
	va_end (ap);

	args[0] = (word) killchan;
	args[1] = (word) func;
	args[2] = argc;
	args[3] = (word) va_args;

	ccsp_cif_Y_bx_dispatch (wptr, ccsp_cif_bcalln_stub, tmp);

	va_end (ap);

	return args[0];
}
/*}}}*/
/*{{{ static int KillBlockingCall (Workspace wptr, Channel *killchan) */
#if defined(__GNUC__)
__attribute__ ((unused)) /* make GCC ignore when unused */
#endif
static int KillBlockingCall (Workspace wptr, Channel *killchan)
{
	int result;

	ccsp_cif_X_bx_kill (wptr, killchan, result);

	return result;
}
/*}}}*/
#endif
/*{{{  void OccamCall (void *func, word stack, word argc, ...) */
#if defined(__GNUC__)
__attribute__ ((unused)) /* make GCC ignore when unused */
#endif
static void build_occam_call_ws (word *ws, int argc, ...)
{
	va_list ap;
	int i;

	va_start (ap, argc);

	for (i = 0; i < argc; ++i) {
		ws[i + 1] = va_arg (ap, word);
	}

	va_end (ap);
}
#define OccamCall(FUNC, SS, ARGC, ARGV...)			\
	do {							\
		ccsp_sched_t *sched	= ccsp_scheduler;	\
		const int top		= (ARGC) + 1;		\
		word ws[(SS) + CIF_PROCESS_WORDS + top + 3];	\
		word *p = &(ws[(SS) + CIF_PROCESS_WORDS]);	\
		build_occam_call_ws (p, ARGC, ## ARGV);		\
		ccsp_cif_occam_call ((void *)sched, (void *)sched->stack, p, (void *)FUNC, top); \
	} while (0)
/*}}}*/
/*}}}*/

/*{{{ Memory Allocation*/
/*{{{  void *MAlloc (Workspace wptr, word size) */
static inline void *MAlloc (Workspace wptr, word size)
{
	void *ptr;
	ccsp_cif_X_malloc (wptr, size, ptr);
	return ptr;
}
/*}}}*/
/*{{{  void MRelease (Workspace wptr, void *ptr) */
static inline void MRelease (Workspace wptr, void *ptr)
{
	ccsp_cif_X_mrelease (wptr, ptr);
}
/*}}}*/
/*}}}*/

/*{{{ Mobile Types */
/*{{{  void *MTAlloc (Workspace wptr, word type, word size) */

/**
 * Allocates a mobile.  See mobile_types.h for a explanation of the latter two
 * parameters, which describe the structure of the mobile type.  This is
 * necessary in case you have nested mobiles, and allows the CCSP kernel to
 * handle moving the inner mobiles.
 *
 * @return The pointer to the mobile data (such as an mt_array_t).
 */
static inline void *MTAlloc (Workspace wptr, word type, word size)
{
	void *ptr;
	ccsp_cif_X_mt_alloc (wptr, type, size, ptr);
	return ptr;
}
/*}}}*/
/*{{{  void *MTClone (Workspace wptr, void *ptr) */

/**
 * Clones the given mobile data.  You only need pass a pointer to the data,
 * not a pointer-to-a-pointer as in some other functions.  For arrays, what
 * you pass should be of type mt_array_t*.
 *
 * @return A pointer to the cloned data.
 */
static inline void *MTClone (Workspace wptr, void *ptr)
{
	void *clone;
	ccsp_cif_X_mt_clone (wptr, ptr, clone);
	return clone;
}
/*}}}*/
/*{{{  void MTRelease (Workspace wptr, void *ptr) */

/**
 * Releases (frees) the given mobile data.  You only need pass a pointer to
 * the data, not a pointer-to-a-pointer as in some other functions.  Therefore
 * it's up to you to NULL your pointer afterwards.
 *
 * Do not pass a NULL pointer to this function (check yourself before calling).
 */
static inline void MTRelease (Workspace wptr, void *ptr)
{
	ccsp_cif_X_mt_release (wptr, ptr);
}
/*}}}*/
/*{{{  void *MTResize (Workspace wptr, word type, void *ptr, word param) */
static inline void *MTResize (Workspace wptr, word type, void *ptr, word param)
{
	void *result;
	ccsp_cif_X_mt_resize (wptr, type, ptr, param, result);
	return result;
}
/*}}}*/
/*{{{  void MTChanIn (Workspace wptr, Channel *c, void **pptr) */

/**
 * Reads from the given channel into the given mobile.  You must pass in a
 * pointer to your pointer-to-mobile-data, so that it can be modified.  It is
 * up to you to make sure that any previous mobile referred to by that pointer
 * has already been released before calling (using MTRelease), as the contents
 * of your pointer-to-mobile-data will be blindly overwritten.  For arrays,
 * the parameter should be of type mt_array_t**.
 */
static inline void MTChanIn (Workspace wptr, Channel *c, void **pptr)
{
	ccsp_cif_Y_mt_in (wptr, c, pptr);
}
/*}}}*/
/*{{{  void MTChanOut (Workspace wptr, Channel *c, void **pptr) */

/**
 * Writes to the given channel from the given mobile.  You must pass in a
 * pointer to your pointer-to-mobile-data, so that it can be modified (it will
 * be set to NULL once the output has completed). For arrays, the parameter
 * should be of type mt_array_t**.
 */
static inline void MTChanOut (Workspace wptr, Channel *c, void **pptr)
{
	ccsp_cif_Y_mt_out (wptr, c, pptr);
}
/*}}}*/
/*{{{  void MTChanXChg (Workspace wptr, Channel *c, void **pptr) */
static inline void MTChanXChg (Workspace wptr, Channel *c, void **pptr)
{
	ccsp_cif_Y_mt_xchg (wptr, c, pptr);
}
/*}}}*/
/*{{{  void MTChanXIn (Workspace wptr, Channel *c, void **pptr) */
static inline void MTChanXIn (Workspace wptr, Channel *c, void **pptr)
{
	ccsp_cif_Y_mt_xin (wptr, c, pptr);
}
/*}}}*/
/*{{{  void MTChanXXChg (Workspace wptr, Channel *c, void **pptr) */
static inline void MTChanXXChg (Workspace wptr, Channel *c, void **pptr)
{
	ccsp_cif_Y_mt_xxchg (wptr, c, pptr);
}
/*}}}*/
/*{{{  void MTLock (Workspace wptr, void *ptr, word lock) */
static inline void MTLock (Workspace wptr, void *ptr, word lock)
{
	ccsp_cif_Y_mt_lock (wptr, lock, ptr);
}
/*}}}*/
/*{{{  void MTUnlock (Workspace wptr, void *ptr, word lock) */
static inline void MTUnlock (Workspace wptr, void *ptr, word lock)
{
	ccsp_cif_X_mt_unlock (wptr, lock, ptr);
}
/*}}}*/
/*{{{  void MTSync (Workspace wptr, void *ptr) */
static inline void MTSync (Workspace wptr, void *ptr)
{
	ccsp_cif_Y_mt_sync (wptr, ptr);
}
/*}}}*/
/*{{{  void MTEnroll (Workspace wptr, void *ptr, word count) */
static inline void MTEnroll (Workspace wptr, void *ptr, word count)
{
	ccsp_cif_X_mt_enroll (wptr, count, ptr);
}
/*}}}*/
/*{{{  void MTResign (Workspace wptr, void *ptr, word count) */
static inline void MTResign (Workspace wptr, void *ptr, word count)
{
	ccsp_cif_X_mt_resign (wptr, count, ptr);
}
/*}}}*/
/*}}}*/

/*{{{ Priority and Affinity */
/*{{{  word GetPriority (Workspace wptr) */
static inline word GetPriority (Workspace wptr)
{
	word priority;
	ccsp_cif_X_getpri (wptr, priority);
	return priority;
}
/*}}}*/
/*{{{  void SetPriority (Workspace wptr, word priority) */
static inline void SetPriority (Workspace wptr, word priority)
{
	ccsp_cif_Y_setpri (wptr, priority);
}
/*}}}*/
/*{{{  word GetAffinity (Workspace wptr) */
static inline word GetAffinity (Workspace wptr)
{
	word affinity;
	ccsp_cif_X_getaff (wptr, affinity);
	return affinity;
}
/*}}}*/
/*{{{  void SetAffinity (Workspace wptr, word affinity) */
static inline void SetAffinity (Workspace wptr, word affinity)
{
	ccsp_cif_Y_setaff (wptr, affinity);
}
/*}}}*/
/*}}}*/

/*{{{ Processes */
/*{{{  Workspace ProcAlloc (Workspace wptr, word args, word stack) */
static inline Workspace ProcAlloc (Workspace wptr, word args, word stack)
{
	Workspace ws;
	word words = WORKSPACE_SIZE (args, stack);

	ccsp_cif_X_proc_alloc (wptr, 0, words, ws);

	ws += CIF_PROCESS_WORDS;
	PROC_DESC(ws)->barrier_ptr = (word) NULL;
	PROC_DESC(ws)->stack_ptr = words - CIF_PROCESS_WORDS;
	
	return ws;
}
/*}}}*/
/*{{{  void ProcParam (Workspace wptr, Workspace ws, word n, type param) */
#define ProcParam(wptr, ws, n, param) \
	do { (ws)[(n) + CIF_WPTR_BIAS_WORDS] = (word) (param); } while (0)
/*}}}*/
/*{{{  void ProcMTCopy (Workspace wptr, Workspace ws, word n, void *ptr) */

/**
 * Copies the mobile data from the workspace wptr (the current workspace) into
 * the workspace ws (the new process's workspace), in slot n.  You should pass
 * a pointer-to-mobile-data, not a pointer-to-a-pointer as in some other
 * methods.  The process should then use ProcGetParam (with the exact same
 * type as you pass to this function) to retrieve the parameter.  For arrays,
 * the type of ptr should be mt_array_t*.
 */
static inline void ProcMTCopy (Workspace wptr, Workspace ws, word n, void *ptr)
{
	ws -= CIF_PROCESS_WORDS;
	n += CIF_PROCESS_WORDS + CIF_WPTR_BIAS_WORDS;
	ccsp_cif_X_proc_mt_copy (wptr, n, ws, ptr);
}
/*}}}*/
/*{{{  void ProcMTMove (Workspace wptr, Workspace ws, word n, void *pptr) */

/**
 * Moves the mobile data from the workspace wptr (the current workspace) into
 * the workspace ws (the new process's workspace), in slot n.  You should pass
 * a pointer-to-a-pointer-to-mobile-data; the pointed-to-pointer will be set
 * to NULL after this call.  The process should then use ProcGetParam (with
 * the type of pointer-to-mobile-data -- one less indirection level) to
 * retrieve the parameter.  For arrays, the type of pptr should be mt_array_t**.
 */
static inline void ProcMTMove (Workspace wptr, Workspace ws, word n, void *pptr)
{
	ws -= CIF_PROCESS_WORDS;
	n += CIF_PROCESS_WORDS + CIF_WPTR_BIAS_WORDS;
	ccsp_cif_X_proc_mt_move (wptr, n, ws, pptr);
}
/*}}}*/
/*{{{  type ProcGetParam (Workspace wptr, word n, type) */
#define ProcGetParam(wptr, n, type) \
	((type) ((wptr)[(n) + CIF_WPTR_BIAS_WORDS]))
/*}}}*/
/*{{{  void ProcStart (Workspace wptr, Workspace ws, Process func) */
static inline void ProcStart (Workspace wptr, Workspace ws, Process func)
{
	ccsp_sched_t *sched = (ccsp_sched_t *) PROC_DESC(wptr)->sched_ptr;
	Workspace top = ws + PROC_DESC(ws)->stack_ptr;
	
	top -= 1; 			/* one parameter (ws) */
	top = (Workspace) (((word) top) & (~((sizeof(word) * CIF_STACK_ALIGN) - 1)));
	top -= CIF_STACK_LINKAGE;	/* return pointer */

	ws[0]	= (word) top;
	top[0]	= (word) func;
	top[1]	= (word) ws;
	/* Set SchedPtr AFTER using PROC_DESC(ws)->stack_ptr for top calculation above,
	 * since StackPtr and SchedPtr share offset -7. */
	PROC_DESC(ws)->sched_ptr = (word) sched;

	ws -= CIF_PROCESS_WORDS;

	ccsp_cif_Y_proc_start (wptr, CIF_PROCESS_WORDS, ws, _ccsp_cif_proc_stub);
}
/*}}}*/
/*{{{  void ProcEnd (Workspace wptr) */
static inline void ProcEnd (Workspace wptr)
{
	switch (PROC_DESC(wptr)->barrier_ptr) {
		case ((word) NULL):
			{
				/* started with ProcStart */
				Workspace ws = wptr - CIF_PROCESS_WORDS;
				ccsp_cif_Y_proc_end (wptr, ws);
			}
			break;
		case ((word) -1):
			{
				/* called from occam */
				ccsp_cif_jump ((void *) wptr, (void *) PROC_DESC(wptr)->escape_ptr);
			}
			break;
		default:
			{
				/* start with LightProcStart */
				word bar = PROC_DESC(wptr)->barrier_ptr;
				ccsp_cif_Y_endp (wptr, bar);
			}
			break;
	}
}
/*}}}*/
/*{{{  Workspace ProcAllocInitial (word args, word stack) */
static inline Workspace ProcAllocInitial (word args, word stack)
{
	Workspace ws;
	word words = WORKSPACE_SIZE (args, stack);

	ws = ccsp_proc_alloc (0, words);

	ws += CIF_PROCESS_WORDS;
	PROC_DESC(ws)->barrier_ptr = (word) NULL;
	PROC_DESC(ws)->stack_ptr = words - CIF_PROCESS_WORDS;
	
	return ws;
}
/*}}}*/
/*{{{  void ProcStartInitial (Workspace ws, Process func) */
static inline void ProcStartInitial (Workspace ws, Process func)
{
	Workspace top = ws + PROC_DESC(ws)->stack_ptr;
	
	top -= 1; 			/* one parameter (ws) */
	top = (Workspace) (((word) top) & (~((sizeof(word) * CIF_STACK_ALIGN) - 1)));
	top -= CIF_STACK_LINKAGE;	/* return pointer */

	PROC_DESC(ws)->iptr	= (word) _ccsp_cif_proc_stub;
	ws[0]		= (word) top;
	top[0]		= (word) func;
	top[1]		= (word) ws;

	ccsp_kernel_entry (ws, NULL);
}
/*}}}*/
/*{{{  void LightProcBarrierInit (Workspace wptr, LightProcBarrier *bar, word count) */
static inline void LightProcBarrierInit (Workspace wptr, LightProcBarrier *bar, word count)
{
	word *wbar = bar->data + CIF_PROCESS_WORDS;
	word pas;

	ccsp_cif_X_getpas (wptr, pas);

	PROC_DESC(wbar)->iptr_succ		= (word) _ccsp_cif_endp_resume_stub;
	PROC_DESC(wbar)->count 		= count + 1;
	PROC_DESC(wbar)->saved_priority	= pas;
}
/*}}}*/
/*{{{  void LightProcBarrierEnroll (Workspace wptr, LightProcBarrier *bar, word count) */
static inline void LightProcBarrierEnroll (Workspace wptr, LightProcBarrier *bar, word count)
{
	word *wbar = bar->data + CIF_PROCESS_WORDS;

	ccsp_cif_X_par_enroll (wptr, wbar, count);
}
/*}}}*/
/*{{{  void LightProcBarrierWait (Workspace wptr, LightProcBarrier *bar) */
static inline void LightProcBarrierWait (Workspace wptr, LightProcBarrier *bar)
{
	word *wbar = bar->data + CIF_PROCESS_WORDS;

	PROC_DESC(wbar)->pointer = (word) wptr;

	ccsp_cif_Y_endp (wptr, wbar);
}
/*}}}*/
/*{{{  Workspace LightProcInit (Workspace wptr, word *base, word args, word stack) */
static inline Workspace LightProcInit (Workspace wptr, word *base, word args, word stack)
{
	/* Round base up to a 16-byte boundary.  C declares the underlying
	 * storage (typically `word stack_pN[WORKSPACE_SIZE(...)]` on the
	 * function's frame) with alignof(word) = 8, but on aarch64 EL0 SP
	 * accesses require 16-byte alignment.  Phase 4D unifies sp with
	 * Wptr, so the workspace pointer must itself be 16-aligned for any
	 * sp-relative accesses (in CIF resume code or in occam-derived
	 * inserts) to succeed.  CIF_PROCESS_WORDS=24 (192 bytes) is itself
	 * 16-aligned, so once base is, the resulting ws is too.  The
	 * trailing slack already in WORKSPACE_SIZE (CIF_STACK_ALIGN-1=3
	 * words) absorbs the up-to-1-word loss from this rounding. */
	base = (word *)(((word)base + (2 * sizeof(word) - 1)) & ~(2 * sizeof(word) - 1));
	Workspace ws = base + CIF_PROCESS_WORDS;
	word words = WORKSPACE_SIZE (args, stack);

	PROC_DESC(ws)->stack_ptr = words - CIF_PROCESS_WORDS;

	return ws;
}
/*}}}*/
/*{{{  void LightProcStart (Workspace wptr, LightProcBarrier *bar, Workspace ws, Process func) */
static inline void LightProcStart (Workspace wptr, LightProcBarrier *bar, Workspace ws, Process func)
{
	ccsp_sched_t *sched = (ccsp_sched_t *) PROC_DESC(wptr)->sched_ptr;
	Workspace top = ws + PROC_DESC(ws)->stack_ptr;
	
	top -= 1; 			/* one parameter (ws) */
	top = (Workspace) (((word) top) & (~((sizeof(word) * CIF_STACK_ALIGN) - 1)));
	top -= CIF_STACK_LINKAGE;	/* return pointer */

	PROC_DESC(ws)->barrier_ptr	= (word) (bar->data + CIF_PROCESS_WORDS);
	PROC_DESC(ws)->priofinity	= PROC_DESC(bar->data + CIF_PROCESS_WORDS)->saved_priority;
	PROC_DESC(ws)->iptr	= (word) _ccsp_cif_light_proc_stub;
	ws[0]		= (word) top;
	/* Set SchedPtr AFTER using PROC_DESC(ws)->stack_ptr for top calculation above,
	 * since StackPtr and SchedPtr share offset -7. */
	PROC_DESC(ws)->sched_ptr	= (word) sched;
	top[0]		= (word) func;
	top[1]		= (word) ws;


	ccsp_cif_X_runp (wptr, ws);
}
/*}}}*/
/*}}}*/

/*{{{ Scheduling */
/*{{{  void Reschedule (Workspace wptr) */
static inline void Reschedule (Workspace wptr)
{
	ccsp_cif_Y_pause (wptr);
}
/*}}}*/
/*{{{  void Shutdown (Workspace wptr)*/
static inline void Shutdown (Workspace wptr)
{
	ccsp_cif_Y_shutdown (wptr);
}
/*}}}*/
/*}}}*/

/*{{{ Timers */
/*{{{  Time TimerRead (Workspace wptr)*/
static inline Time TimerRead (Workspace wptr)
{
	Time now;
	ccsp_cif_X_ldtimer (wptr, now);
	return now;
}
/*}}}*/
/*{{{  Time TimerWait (Workspace wptr, Time timeout) */
static inline Time TimerWait (Workspace wptr, Time timeout)
{
	ccsp_cif_Y_tin (wptr, timeout);
	return GetTimeField (wptr);
}
/*}}}*/
/*}}}*/

#endif /* CCSP_CIF_H */

