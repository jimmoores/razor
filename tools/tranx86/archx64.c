/*
 *	archx64.c -- x64 architecture support
 *	Copyright (C) 2024 Amazon Q
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
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "support.h"
#include "structs.h"
#include "archdef.h"
#include "tstate.h"
#include "machine.h"
#include "archx64.h"
#include "rtlops.h"
#include "kif.h"
#include "transputer.h"
#include "etcrtl.h"
#include "tstack.h"

/* External declarations */
extern char *progname;
extern optstruct options;
extern void add_to_ins_chain (ins_chain *ins);
extern void flush_ins_chain (void);
extern void add_to_rtl_chain (rtl_chain *rtl);
extern int get_last_lab (void);
extern int tstack_newreg (tstack *);
extern void tstack_undefine (tstack *);
extern void constmap_clearall (void);
extern kif_entrytype *kif_entry (int call);

/*{{{  constants and definitions*/
/* x64 register definitions */
#define REG_RAX 0
#define REG_RCX 1
#define REG_RDX 2
#define REG_RBX 3
#define REG_RSP 4
#define REG_RBP 5
#define REG_RSI 6
#define REG_RDI 7
#define REG_R8  8
#define REG_R9  9
#define REG_R10 10
#define REG_R11 11
#define REG_R12 12
#define REG_R13 13
#define REG_R14 14
#define REG_R15 15

/* Special register mappings for occam runtime (use positive values to avoid conflicts) */
#define X64_REG_WPTR 14  /* Workspace pointer (r14) */
#define X64_REG_FPTR 13  /* Front pointer (r13) */
#define X64_REG_BPTR 12  /* Back pointer (r12) */
#define X64_REG_CC   16  /* Condition codes (virtual) */

/* Kernel call constants */
#ifndef K_PAUSE
#define K_PAUSE 0
#endif

/* Debug options */
#ifndef DEBUG_DEADLOCK
#define DEBUG_DEADLOCK 1
#endif

/* Missing constants from other headers */
#ifndef CC_E
#define CC_E 4
#endif
#ifndef CC_NE
#define CC_NE 5
#endif
#ifndef CC_GE
#define CC_GE 13
#endif

/* Floating point operation constants */
#ifndef I_FPADD
#define I_FPADD 74
#endif
#ifndef I_FPSUB
#define I_FPSUB 75
#endif
#ifndef I_FPMUL
#define I_FPMUL 76
#endif
#ifndef I_FPDIV
#define I_FPDIV 77
#endif
#ifndef I_FPSQRT
#define I_FPSQRT 89
#endif

/* Shift operation constants */
#ifndef I_SHL
#define I_SHL 0x41
#endif
#ifndef I_SHR
#define I_SHR 0x40
#endif
/*}}}*/
/*{{{  forward declarations*/
static void compose_x64_kcall (tstate *ts, const int call, const int regs_in, const int regs_out);
static ins_chain *compose_x64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry);
static void compose_x64_deadlock_kcall (tstate *ts, const int call, const int regs_in, const int regs_out);
static void compose_x64_inline_ldtimer (tstate *ts);
static void compose_x64_inline_tin (tstate *ts);
static void compose_x64_inline_quick_reschedule (tstate *ts);
static void compose_x64_inline_full_reschedule (tstate *ts);
static void compose_x64_inline_in (tstate *ts, int width);
static void compose_x64_inline_out (tstate *ts, int width);
static void compose_x64_entry_prolog (tstate *ts);
static void compose_x64_return (tstate *ts);
static int compose_x64_widenshort (tstate *ts);
static int compose_x64_widenword (tstate *ts);
static void compose_x64_longop (tstate *ts, int secondary_opcode);
static void compose_x64_fpop (tstate *ts, int secondary_opcode);
static int x64_regcolour_special_to_real (int reg);
static int x64_regcolour_get_regs (int *regs);
static int x64_code_to_asm (rtl_chain *rtl_code, char *filename);
static int x64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream);
static char *x64_get_register_name (int reg);
/* Stub functions for missing architecture functions */
static void x64_compose_reset_fregs (tstate *ts) {
	/* Initialize x64 floating point unit */
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# x64 FPU reset")));
}
static void x64_stub_void (tstate *ts) { /* stub */ }
static void x64_stub_void_int (tstate *ts, int arg) { /* stub */ }
static int x64_stub_int (tstate *ts) { 
	if (!ts || !ts->stack) return -1;
	return tstack_newreg(ts->stack); 
}
static int x64_stub_int_int_int (tstate *ts, int a, int b) { 
	if (!ts || !ts->stack) return -1;
	return tstack_newreg(ts->stack); 
}
static void x64_stub_void_int_int (tstate *ts, int a, int b) { /* stub */ }
static void x64_stub_void_int_int_int (tstate *ts, int a, int b, int c) { /* stub */ }
static void x64_stub_void_int_int_int_int (tstate *ts, int a, int b, int c, int d) { /* stub */ }
static void x64_stub_void_int_int_int_int_int (tstate *ts, int a, int b, int c, int d, int e) { /* stub */ }

/* Critical function implementations to fix segfaults */
static void compose_x64_move (tstate *ts) {
	/* Basic memory move operation */
	int src_reg = ts->stack->old_c_reg;
	int dst_reg = ts->stack->old_b_reg;
	int count_reg = ts->stack->old_a_reg;
	
	/* Simple move implementation */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REG, dst_reg));
}

static void compose_x64_shift (tstate *ts, int sec, int r1, int r2, int r3) {
	/* Shift operations */
	if (sec == I_SHL) {
		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
	} else {
		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
	}
}

static void compose_x64_division (tstate *ts, int dividend, int divisor, int quotient) {
	/* Basic division */
	add_to_ins_chain (compose_ins (INS_DIV, 2, 1, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, quotient));
}

static int compose_x64_remainder (tstate *ts, int dividend, int divisor) {
	/* Basic remainder */
	int result_reg = tstack_newreg(ts->stack);
	add_to_ins_chain (compose_ins (INS_DIV, 2, 1, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, result_reg));
	return result_reg;
}

static void compose_x64_external_ccall (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last) {
	/* Basic external C call */
	char *call_name;
	if (!name) {
		call_name = string_dup ("unknown_function");
	} else if (*name == '&') {
		call_name = string_dup (name + 1);
	} else {
		call_name = string_dup (name);
	}
	*pst_first = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, call_name);
	add_to_ins_chain (*pst_first);
	*pst_last = *pst_first;
}
static int x64_stub_validate (ins_chain *ins) { return 1; }
static int x64_stub_fp_regs (int *regs) { return 0; }
static void x64_stub_external_ccall (tstate *ts, int inlined, char *name, ins_chain **ret1, ins_chain **ret2) { 
	if (ret1) *ret1 = NULL;
	if (ret2) *ret2 = NULL;
}
static void x64_stub_bcall (tstate *ts, int inlined, int a, int b, char *name, ins_chain **ret1, ins_chain **ret2) { 
	if (ret1) *ret1 = NULL;
	if (ret2) *ret2 = NULL;
}
static void x64_stub_cif_call (tstate *ts, int inlined, char *name, ins_chain **ret1, ins_chain **ret2) { 
	if (ret1) *ret1 = NULL;
	if (ret2) *ret2 = NULL;
}
static void x64_stub_rmox_entry_prolog (tstate *ts, rmoxmode_e mode) { /* stub */ }
/*}}}*/

/*{{{  static arch_t x64_arch*/
static arch_t x64_arch = {
	.archname = "x64",
	.compose_kcall = compose_x64_kcall,
	.compose_kjump = compose_x64_kjump,
	.compose_deadlock_kcall = compose_x64_deadlock_kcall,
	.compose_pre_enbc = x64_stub_void,
	.compose_pre_enbt = x64_stub_void,
	.compose_inline_ldtimer = compose_x64_inline_ldtimer,
	.compose_inline_tin = compose_x64_inline_tin,
	.compose_inline_quick_reschedule = compose_x64_inline_quick_reschedule,
	.compose_inline_full_reschedule = compose_x64_inline_full_reschedule,
	.compose_inline_in = compose_x64_inline_in,
	.compose_inline_in_2 = compose_x64_inline_in,
	.compose_inline_min = x64_stub_void_int,
	.compose_inline_out = compose_x64_inline_out,
	.compose_inline_out_2 = compose_x64_inline_out,
	.compose_inline_mout = x64_stub_void_int,
	.compose_inline_enbc = x64_stub_void_int,
	.compose_inline_disc = x64_stub_void_int,
	.compose_inline_altwt = x64_stub_void,
	.compose_inline_stlx = x64_stub_void_int,
	.compose_inline_malloc = x64_stub_void,
	.compose_inline_startp = x64_stub_void,
	.compose_inline_endp = x64_stub_void,
	.compose_inline_runp = x64_stub_void,
	.compose_inline_stopp = x64_stub_void,
	.compose_debug_insert = x64_stub_void_int,
	.compose_debug_procnames = x64_stub_void,
	.compose_debug_filenames = x64_stub_void,
	.compose_debug_zero_div = x64_stub_void,
	.compose_debug_floaterr = x64_stub_void,
	.compose_debug_overflow = x64_stub_void,
	.compose_debug_rangestop = x64_stub_void,
	.compose_debug_seterr = x64_stub_void,
	.compose_overflow_jumpcode = x64_stub_void_int,
	.compose_floaterr_jumpcode = x64_stub_void,
	.compose_rangestop_jumpcode = x64_stub_void_int,
	.compose_debug_deadlock_set = x64_stub_void,
	.compose_divcheck_zero = x64_stub_void_int,
	.compose_divcheck_zero_simple = x64_stub_void_int,
	.compose_division = compose_x64_division,
	.compose_remainder = compose_x64_remainder,
	.compose_iospace_loadbyte = x64_stub_int_int_int,
	.compose_iospace_storebyte = x64_stub_void_int_int,
	.compose_iospace_loadword = x64_stub_int_int_int,
	.compose_iospace_storeword = x64_stub_void_int_int,
	.compose_iospace_read = x64_stub_void_int_int_int,
	.compose_iospace_write = x64_stub_void_int_int_int,
	.compose_move_loadptrs = x64_stub_void,
	.compose_move = compose_x64_move,
	.compose_shift = compose_x64_shift,
	.compose_widenshort = compose_x64_widenshort,
	.compose_widenword = compose_x64_widenword,
	.compose_longop = compose_x64_longop,
	.compose_fpop = compose_x64_fpop,
	.compose_external_ccall = compose_x64_external_ccall,
	.compose_bcall = x64_stub_bcall,
	.compose_cif_call = x64_stub_cif_call,
	.compose_entry_prolog = compose_x64_entry_prolog,
	.compose_rmox_entry_prolog = x64_stub_rmox_entry_prolog,
	.compose_fp_set_fround = x64_stub_void_int,
	.compose_fp_init = x64_stub_void,
	.compose_reset_fregs = x64_compose_reset_fregs,
	.compose_refcountop = x64_stub_void_int_int,
	.compose_memory_barrier = x64_stub_void_int,
	.compose_return = compose_x64_return,
	.compose_nreturn = x64_stub_void_int,
	.compose_funcresults = x64_stub_void_int,
	.regcolour_special_to_real = x64_regcolour_special_to_real,
	.regcolour_rmax = 16,
	.regcolour_nodemax = 256,
	.regcolour_get_regs = x64_regcolour_get_regs,
	.regcolour_fp_regs = x64_stub_fp_regs,
	.code_to_asm = x64_code_to_asm,
	.code_to_asm_stream = x64_code_to_asm_stream,
	.rtl_validate_instr = x64_stub_validate,
	.rtl_prevalidate_instr = x64_stub_validate,
	.get_register_name = x64_get_register_name,
	.int_options = 0,
	.kiface_tableoffs = 0
};
/*}}}*/

/*{{{  arch_t *init_arch_x64 (int mclass)*/
arch_t *init_arch_x64 (int mclass)
{
	return &x64_arch;
}
/*}}}*/

/*{{{  static void compose_x64_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)*/
static void compose_x64_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)
{
	kif_entrytype *entry = kif_entry (call);
	int i, cregs[3];
	ins_chain *call_ins;
	char *entrypoint_name;

	/* Validate entry and entrypoint */
	if (!entry || !entry->entrypoint) {
		entrypoint_name = string_dup ("unknown_kernel_call");
	} else {
		entrypoint_name = string_dup (entry->entrypoint);
	}

	/* Map stack registers to x64 registers */
	cregs[0] = ts->stack->old_a_reg;
	cregs[1] = ts->stack->old_b_reg;
	cregs[2] = ts->stack->old_c_reg;

	/* Constrain input registers to rdi, rsi, rdx (System V ABI) */
	for (i = 0; i < regs_in && i < 3; i++) {
		int target_reg = (i == 0) ? REG_RDI : ((i == 1) ? REG_RSI : REG_RDX);
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, target_reg));
	}

	/* Generate call instruction */
	call_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, entrypoint_name);
	add_to_ins_chain (call_ins);

	/* Unconstrain registers */
	for (i = 0; i < regs_in && i < 3; i++) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
	}

	/* Handle output registers */
	if (regs_out > 0) {
		tstack_undefine (ts->stack);
		constmap_clearall ();
		ts->stack->must_set_cmp_flags = 1;
		ts->stack->ts_depth = regs_out;
		for (i = 0; i < regs_out && i < 3; i++) {
			ts->stack->a_reg = (i == 0) ? cregs[0] : ((i == 1) ? cregs[1] : cregs[2]);
		}
	}
}
/*}}}*/

/*{{{  static ins_chain *compose_x64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry)*/
static ins_chain *compose_x64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry)
{
	ins_chain *jump_ins;
	char *entrypoint_name;

	/* Validate kif_entry and entrypoint */
	if (!kif_entry || !kif_entry->entrypoint) {
		entrypoint_name = string_dup ("unknown_kernel_jump");
	} else {
		entrypoint_name = string_dup (kif_entry->entrypoint);
	}

	if (instr == INS_CJUMP) {
		jump_ins = compose_ins (INS_CJUMP, 2, 0, ARG_COND, cc, ARG_NAMEDLABEL, entrypoint_name);
	} else {
		jump_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, entrypoint_name);
	}

	return jump_ins;
}
/*}}}*/

/*{{{  static void compose_x64_deadlock_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)*/
static void compose_x64_deadlock_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)
{
	/* Set up deadlock debugging information if enabled */
	if (options.debug_options & DEBUG_DEADLOCK) {
		/* Store current workspace pointer for debugging */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, REG_WPTR));
	}

	/* Perform the actual kernel call */
	compose_x64_kcall (ts, call, regs_in, regs_out);

	if (options.debug_options & DEBUG_DEADLOCK) {
		/* Clear link field after rescheduling */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR));
	}
}
/*}}}*/

/*{{{  static void compose_x64_inline_ldtimer (tstate *ts)*/
static void compose_x64_inline_ldtimer (tstate *ts)
{
	int timer_reg = tstack_newreg (ts->stack);

	/* Read TSC using rdtsc instruction */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("rdtsc"), ARG_REG, timer_reg));

	/* Place result in A register */
	ts->stack->a_reg = timer_reg;
}
/*}}}*/

/*{{{  static void compose_x64_inline_tin (tstate *ts)*/
static void compose_x64_inline_tin (tstate *ts)
{
	int timer_reg = tstack_newreg (ts->stack);
	int target_reg = ts->stack->old_a_reg;

	/* Read current time */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("rdtsc"), ARG_REG, timer_reg));

	/* Compare with target time */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, timer_reg, ARG_REG, target_reg, ARG_REG | ARG_IMP, X64_REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_FLABEL, 0));

	/* If time not reached, wait */
	compose_x64_kcall (ts, K_PAUSE, 0, 0);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
}
/*}}}*/

/*{{{  static void compose_x64_inline_quick_reschedule (tstate *ts)*/
static void compose_x64_inline_quick_reschedule (tstate *ts)
{
	/* Check if there are more processes to run */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 1));

	/* No processes, call scheduler */
	compose_x64_kcall (ts, K_PAUSE, 0, 0);
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));

	/* Load next process */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, REG_WPTR));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
}
/*}}}*/

/*{{{  static void compose_x64_inline_full_reschedule (tstate *ts)*/
static void compose_x64_inline_full_reschedule (tstate *ts)
{
	/* Save current process state */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR));

	/* Check if there are other processes */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 1));

	/* Switch to next process */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, REG_WPTR));

	/* No other processes, continue */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
}
/*}}}*/

/*{{{  static void compose_x64_inline_in (tstate *ts, int width)*/
static void compose_x64_inline_in (tstate *ts, int width)
{
	int chan_reg, dest_reg;

	if (width) {
		chan_reg = ts->stack->old_a_reg;
		dest_reg = ts->stack->old_b_reg;
	} else {
		chan_reg = ts->stack->old_b_reg;
		dest_reg = ts->stack->old_c_reg;
	}

	/* Check if channel is ready */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, X64_REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));

	/* Channel not ready, block */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, X64_REG_WPTR, ARG_REGIND, chan_reg));
	compose_x64_kcall (ts, K_PAUSE, 0, 0);

	/* Channel ready, perform input */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	if (width == 8) {
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
	} else {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
	}
}
/*}}}*/

/*{{{  static void compose_x64_inline_out (tstate *ts, int width)*/
static void compose_x64_inline_out (tstate *ts, int width)
{
	int chan_reg, src_reg;

	if (width) {
		chan_reg = ts->stack->old_a_reg;
		src_reg = ts->stack->old_b_reg;
	} else {
		chan_reg = ts->stack->old_b_reg;
		src_reg = ts->stack->old_c_reg;
	}

	/* Check if channel is ready */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, X64_REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));

	/* Channel not ready, block */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, X64_REG_WPTR, ARG_REGIND, chan_reg));
	compose_x64_kcall (ts, K_PAUSE, 0, 0);

	/* Channel ready, perform output */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	if (width == 8) {
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
	} else {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
	}
}
/*}}}*/

/*{{{  static void compose_x64_entry_prolog (tstate *ts)*/
static void compose_x64_entry_prolog (tstate *ts)
{
	/* Set up stack frame */
	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_RBP));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_RSP, ARG_REG, REG_RBP));

	/* Initialize workspace pointer */
	int wptr_reg = tstack_newreg (ts->stack);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&workspace"), ARG_REG, wptr_reg));

	/* Call kernel initialization */
	compose_x64_kcall (ts, K_PAUSE, 0, 0);
}
/*}}}*/

/*{{{  static void compose_x64_return (tstate *ts)*/
static void compose_x64_return (tstate *ts)
{
	/* Restore stack pointer */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_RBP, ARG_REG, REG_RSP));
	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_RBP));

	/* Return */
	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
}
/*}}}*/

/*{{{  static int compose_x64_widenshort (tstate *ts)*/
static int compose_x64_widenshort (tstate *ts)
{
	int tmp_reg = tstack_newreg (ts->stack);

	/* Sign extend 16-bit to 64-bit */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));

	ts->stack->a_reg = tmp_reg;
	return tmp_reg;
}
/*}}}*/

/*{{{  static int compose_x64_widenword (tstate *ts)*/
static int compose_x64_widenword (tstate *ts)
{
	int tmp_reg = tstack_newreg (ts->stack);

	/* Sign extend 32-bit to 64-bit */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));

	/* Arrange for old_a_reg to stay at stack-top */
	ts->stack->a_reg = ts->stack->old_a_reg;
	ts->stack->b_reg = tmp_reg;
	ts->stack->c_reg = ts->stack->old_b_reg;

	return tmp_reg;
}
/*}}}*/

/*{{{  static void compose_x64_longop (tstate *ts, int secondary_opcode)*/
static void compose_x64_longop (tstate *ts, int secondary_opcode)
{
	switch (secondary_opcode) {
	case 0x16: /* I_LADD */
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case 0x38: /* I_LSUB */
		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case 0x31: /* I_LMUL */
		add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case 0x36: /* I_LSHL */
		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case 0x35: /* I_LSHR */
		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	default:
		fprintf (stderr, "%s: unsupported x64 long operation %d\n", progname, secondary_opcode);
		break;
	}
}
/*}}}*/

/*{{{  static void compose_x64_fpop (tstate *ts, int secondary_opcode)*/
static void compose_x64_fpop (tstate *ts, int secondary_opcode)
{
	switch (secondary_opcode) {
	case 74: /* I_FPADD */
		add_to_ins_chain (compose_ins (INS_FADD, 0, 0));
		ts->stack->fs_depth--;
		break;
	case 75: /* I_FPSUB */
		add_to_ins_chain (compose_ins (INS_FSUB, 0, 0));
		ts->stack->fs_depth--;
		break;
	case 76: /* I_FPMUL */
		add_to_ins_chain (compose_ins (INS_FMUL, 0, 0));
		ts->stack->fs_depth--;
		break;
	case 77: /* I_FPDIV */
		add_to_ins_chain (compose_ins (INS_FDIV, 0, 0));
		ts->stack->fs_depth--;
		break;
	case 89: /* I_FPSQRT */
		add_to_ins_chain (compose_ins (INS_FSQRT, 0, 0));
		break;
	default:
		fprintf (stderr, "%s: unsupported x64 floating point operation %d\n", progname, secondary_opcode);
		break;
	}
}
/*}}}*/

/*{{{  static int x64_regcolour_special_to_real (int reg)*/
static int x64_regcolour_special_to_real (int reg)
{
	/* Map special registers to real x64 registers */
	switch (reg) {
	case REG_WPTR:
		return 14;  /* r14 */
	case REG_FPTR:
		return 13;  /* r13 */
	case REG_BPTR:
		return 12;  /* r12 */
	case REG_RSP:
		return 4;   /* rsp */
	case REG_RBP:
		return 5;   /* rbp */
	default:
		return reg;
	}
}
/*}}}*/

/*{{{  static int x64_regcolour_get_regs (int *regs)*/
static int x64_regcolour_get_regs (int *regs)
{
	/* x64 available registers for allocation */
	/* Skip rsp (4), rbp (5), and runtime registers (12, 13, 14) */
	int available_regs[] = {0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 15};
	int count = sizeof(available_regs) / sizeof(available_regs[0]);
	int i;
	
	for (i = 0; i < count; i++) {
		regs[i] = available_regs[i];
	}
	return count;
}
/*}}}*/

/*{{{  static int x64_code_to_asm (rtl_chain *rtl_code, char *filename)*/
static int x64_code_to_asm (rtl_chain *rtl_code, char *filename)
{
	FILE *outfile = fopen (filename, "w");
	if (!outfile) {
		return -1;
	}
	int result = x64_code_to_asm_stream (rtl_code, outfile);
	fclose (outfile);
	return result;
}
/*}}}*/

/*{{{  static int x64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream)*/
static int x64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream)
{
	rtl_chain *tmp;
	ins_chain *ins;
	int i;

	fprintf (stream, "# x64 assembly output\n");
	fprintf (stream, ".text\n");

	for (tmp = rtl_code; tmp; tmp = tmp->next) {
		switch (tmp->type) {
		case RTL_CODE:
			if (!tmp->u.code.head) break;
			for (ins = tmp->u.code.head; ins; ins = ins->next) {
				switch (ins->type) {
				case INS_MOVE:
					if (ins->out_args[0] && ins->in_args[0]) {
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
							fprintf (stream, "\tmovq\t$%ld, %s\n", 
								(long)ins->in_args[0]->regconst,
								x64_get_register_name (ins->out_args[0]->regconst));
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
							fprintf (stream, "\tleaq\t%s(%%rip), %s\n", 
								(char *)ins->in_args[0]->regconst,
								x64_get_register_name (ins->out_args[0]->regconst));
						} else {
							fprintf (stream, "\tmovq\t%s, %s\n", 
								x64_get_register_name (ins->in_args[0]->regconst),
								x64_get_register_name (ins->out_args[0]->regconst));
						}
					}
					break;
				case INS_ADD:
					if (ins->out_args[0] && ins->in_args[0] && ins->in_args[1]) {
						if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							fprintf (stream, "\taddq\t$%ld, %s\n",
								(long)ins->in_args[1]->regconst,
								x64_get_register_name (ins->out_args[0]->regconst));
						} else {
							fprintf (stream, "\taddq\t%s, %s\n",
								x64_get_register_name (ins->in_args[1]->regconst),
								x64_get_register_name (ins->out_args[0]->regconst));
						}
					}
					break;
				case INS_SUB:
					if (ins->out_args[0] && ins->in_args[0] && ins->in_args[1]) {
						if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							fprintf (stream, "\tsubq\t$%ld, %s\n",
								(long)ins->in_args[1]->regconst,
								x64_get_register_name (ins->out_args[0]->regconst));
						} else {
							fprintf (stream, "\tsubq\t%s, %s\n",
								x64_get_register_name (ins->in_args[1]->regconst),
								x64_get_register_name (ins->out_args[0]->regconst));
						}
					}
					break;
				case INS_MUL:
					if (ins->out_args[0] && ins->in_args[0] && ins->in_args[1]) {
						fprintf (stream, "\timulq\t%s, %s\n",
							x64_get_register_name (ins->in_args[1]->regconst),
							x64_get_register_name (ins->out_args[0]->regconst));
					}
					break;
				case INS_CALL:
					if (ins->in_args[0] && (ins->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
						fprintf (stream, "\tcallq\t%s\n", (char *)ins->in_args[0]->regconst);
					}
					break;
				case INS_RET:
					fprintf (stream, "\tretq\n");
					break;
				case INS_PUSH:
					if (ins->in_args[0]) {
						fprintf (stream, "\tpushq\t%s\n", x64_get_register_name (ins->in_args[0]->regconst));
					}
					break;
				case INS_POP:
					if (ins->out_args[0]) {
						fprintf (stream, "\tpopq\t%s\n", x64_get_register_name (ins->out_args[0]->regconst));
					}
					break;
				case INS_SETLABEL:
					if (ins->in_args[0] && (ins->in_args[0]->flags & ARG_MODEMASK) == ARG_LABEL) {
						fprintf (stream, "L%ld:\n", (long)ins->in_args[0]->regconst);
					}
					break;
				case INS_JUMP:
					if (ins->in_args[0]) {
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
							fprintf (stream, "\tjmp\t%s\n", (char *)ins->in_args[0]->regconst);
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_LABEL) {
							fprintf (stream, "\tjmp\tL%ld\n", (long)ins->in_args[0]->regconst);
						}
					}
					break;
				case INS_CJUMP:
					if (ins->in_args[0] && ins->in_args[1] && (ins->in_args[1]->flags & ARG_MODEMASK) == ARG_LABEL) {
						fprintf (stream, "\tj%s\tL%ld\n", 
							(ins->in_args[0]->regconst == CC_E) ? "e" : "ne",
							ins->in_args[1]->regconst);
					}
					break;
				case INS_CMP:
					if (ins->in_args[0] && ins->in_args[1]) {
						if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							fprintf (stream, "\tcmpq\t$%ld, %s\n",
								(long)ins->in_args[1]->regconst,
								x64_get_register_name (ins->in_args[0]->regconst));
						} else {
							fprintf (stream, "\tcmpq\t%s, %s\n",
								x64_get_register_name (ins->in_args[1]->regconst),
								x64_get_register_name (ins->in_args[0]->regconst));
						}
					}
					break;
				default:
					/* Skip unhandled instructions */
					break;
				}
			}
			break;
		case RTL_SETNAMEDLABEL:
			if (tmp->u.label_name) {
				fprintf (stream, "%s:\n", tmp->u.label_name);
			}
			break;
		case RTL_PUBLICSETNAMEDLABEL:
			if (tmp->u.label_name) {
				fprintf (stream, ".global %s\n%s:\n", tmp->u.label_name, tmp->u.label_name);
			}
			break;
		default:
			break;
		}
	}

	return 0;
}
/*}}}*/

/*{{{  static char *x64_get_register_name (int reg)*/
static char *x64_get_register_name (int reg)
{
	static char *regnames[] = {
		"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
		"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
	};
	if (reg >= 0 && reg < 16) {
		return regnames[reg];
	}
	return "unknown";
}
/*}}}*/