/*
 *	archx64.c -- x64 (AMD64/Intel 64) architecture support
 *	Copyright (C) 2024-2026
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <ctype.h>

#include "main.h"
#include "support.h"
#include "structs.h"
#include "transputer.h"
#include "trancomm.h"
#include "tstack.h"
#include "postmortem.h"
#include "tstate.h"
#include "archdef.h"
#include "archx64.h"
#include "rtlops.h"
#include "kif.h"
#include "machine.h"
#include "etcrtl.h"

#ifndef offsetof
#define offsetof(t,f) ((int) (&((((t *)(0))->f))))
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/*{{{  constants and definitions*/
/* x64 physical register numbers (match encoding order) */
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

/* Special register mappings for occam runtime */
#define X64_REG_WPTR  14	/* r14 = Workspace Pointer */
#define X64_REG_FPTR  13	/* r13 = Front Pointer (run queue head) */
#define X64_REG_BPTR  12	/* r12 = Back Pointer (run queue tail) */
#define X64_REG_SCHED 15	/* r15 = Scheduler Pointer */

/* Function result registers (matching SPARC/PPC convention) */
#define X64_REG_FUNCRES0 REG_RBX	/* 3 */
#define X64_REG_FUNCRES1 REG_RSI	/* 6 */
#define X64_REG_FUNCRES2 REG_RDI	/* 7 */

#define RMAX_X64 10
#define NODEMAX_X64 256

/* Reference counting operation constants (may not be in transputer.h) */
#ifndef I_RCINIT
#define I_RCINIT 200
#endif
#ifndef I_RCINC
#define I_RCINC 201
#endif
#ifndef I_RCDEC
#define I_RCDEC 202
#endif

/* Memory barrier operation constants */
#ifndef I_MB
#define I_MB 300
#endif
#ifndef I_RMB
#define I_RMB 301
#endif
#ifndef I_WMB
#define I_WMB 302
#endif

#if defined(CAN_DO_DOTLABELS)
	#define LBLPFX ".L"
#else
	#define LBLPFX "L"
#endif
/*}}}*/

/*{{{  external declarations*/
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
/*}}}*/

/*{{{  register name tables*/
static char *x64_regs[] = {
	"%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi",
	"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
};
static char *x64_regs32[] = {
	"%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi",
	"%r8d", "%r9d", "%r10d", "%r11d", "%r12d", "%r13d", "%r14d", "%r15d"
};
static char *x64_regs16[] = {
	"%ax", "%cx", "%dx", "%bx", "%sp", "%bp", "%si", "%di",
	"%r8w", "%r9w", "%r10w", "%r11w", "%r12w", "%r13w", "%r14w", "%r15w"
};
static char *x64_regs8[] = {
	"%al", "%cl", "%dl", "%bl", "%spl", "%bpl", "%sil", "%dil",
	"%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"
};
static char *x64_fregs[] = {
	"%st(0)", "%st(1)", "%st(2)", "%st(3)", "%st(4)", "%st(5)", "%st(6)", "%st(7)"
};

/* System V ABI: first arg in rdi for kernel calls */
static const int xregs[3] = { REG_RDI, REG_RSI, REG_RDX };
/*}}}*/

/*{{{  forward declarations*/
static void set_implied_inputs (ins_chain *instr, int n_inputs, int *i_regs);
static void set_implied_outputs (ins_chain *instr, int n_outputs, int *o_regs);
static void compose_x64_kcall (tstate *ts, const int call, const int regs_in, const int regs_out);
static ins_chain *compose_x64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *entry);
static void compose_x64_deadlock_kcall (tstate *ts, const int call, const int regs_in, const int regs_out);
static void compose_x64_inline_ldtimer (tstate *ts);
static void compose_x64_inline_tin (tstate *ts);
static void compose_x64_inline_quick_reschedule (tstate *ts);
static void compose_x64_inline_full_reschedule (tstate *ts);
static void compose_x64_inline_in (tstate *ts, int width);
static void compose_x64_inline_in_2 (tstate *ts, int width);
static void compose_x64_inline_out (tstate *ts, int width);
static void compose_x64_inline_out_2 (tstate *ts, int width);
static void compose_x64_inline_min (tstate *ts, int wide);
static void compose_x64_inline_mout (tstate *ts, int wide);
static void compose_x64_inline_enbc (tstate *ts, int instr);
static void compose_x64_inline_disc (tstate *ts, int instr);
static void compose_x64_inline_altwt (tstate *ts);
static void compose_x64_inline_stlx (tstate *ts, int ins);
static void compose_x64_inline_malloc (tstate *ts);
static void compose_x64_inline_enqueue (tstate *ts, int preg, int nflab);
static void compose_x64_inline_startp (tstate *ts);
static void compose_x64_inline_endp (tstate *ts);
static void compose_x64_inline_runp (tstate *ts);
static void compose_x64_inline_stopp (tstate *ts);
static void compose_x64_entry_prolog (tstate *ts);
static void compose_x64_return (tstate *ts);
static void compose_x64_nreturn (tstate *ts, int adjust);
static void compose_x64_funcresults (tstate *ts, int nresults);
static int compose_x64_widenshort (tstate *ts);
static int compose_x64_widenword (tstate *ts);
static void compose_x64_longop (tstate *ts, int sec);
static void compose_x64_fpop (tstate *ts, int sec);
static void compose_external_ccall_x64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last);
static void compose_x64_bcall (tstate *ts, int inlined, int kernel_call, int unused, char *name, ins_chain **pst_first, ins_chain **pst_last);
static void compose_x64_cif_call (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last);
static void compose_x64_move (tstate *ts);
static void compose_x64_move_loadptrs (tstate *ts);
static void compose_x64_shift (tstate *ts, int sec, int r1, int r2, int r3);
static void compose_x64_division (tstate *ts, int dividend, int divisor, int quotient);
static int compose_x64_remainder (tstate *ts, int dividend, int divisor);
static void compose_pre_enbc_x64 (tstate *ts);
static void compose_pre_enbt_x64 (tstate *ts);
static void compose_debug_insert_x64 (tstate *ts, int mdpairid);
static void compose_debug_procnames_x64 (tstate *ts);
static void compose_debug_filenames_x64 (tstate *ts);
static void compose_debug_zero_div_x64 (tstate *ts);
static void compose_debug_floaterr_x64 (tstate *ts);
static void compose_debug_overflow_x64 (tstate *ts);
static void compose_debug_rangestop_x64 (tstate *ts);
static void compose_debug_seterr_x64 (tstate *ts);
static void compose_overflow_jumpcode_x64 (tstate *ts, int dcode);
static void compose_floaterr_jumpcode_x64 (tstate *ts);
static void compose_rangestop_jumpcode_x64 (tstate *ts, int rcode);
static void compose_debug_deadlock_set_x64 (tstate *ts);
static void compose_divcheck_zero_x64 (tstate *ts, int reg);
static void compose_divcheck_zero_simple_x64 (tstate *ts, int reg);
static int compose_iospace_loadbyte_x64 (tstate *ts, int portreg, int targetreg);
static void compose_iospace_storebyte_x64 (tstate *ts, int portreg, int sourcereg);
static int compose_iospace_loadword_x64 (tstate *ts, int portreg, int targetreg);
static void compose_iospace_storeword_x64 (tstate *ts, int portreg, int sourcereg);
static void compose_iospace_read_x64 (tstate *ts, int portreg, int addrreg, int width);
static void compose_iospace_write_x64 (tstate *ts, int portreg, int addrreg, int width);
static void compose_fp_set_fround_x64 (tstate *ts, int mode);
static void compose_fp_init_x64 (tstate *ts);
static void compose_reset_fregs_x64 (tstate *ts);
static void compose_refcountop_x64 (tstate *ts, int op, int reg);
static void compose_memory_barrier_x64 (tstate *ts, int sec);
static int x64_regcolour_special_to_real (int reg);
static int x64_regcolour_get_regs (int *regs);
static int x64_regcolour_fp_regs (int *regs);
static int x64_code_to_asm (rtl_chain *rtl_code, char *filename);
static int x64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream);
static char *x64_get_register_name (int reg);
static int x64_rtl_validate_instr (ins_chain *ins);
static int x64_rtl_prevalidate_instr (ins_chain *ins);
static void x64_stub_rmox_entry_prolog (tstate *ts, rmoxmode_e mode);
/*}}}*/

/*{{{  static void set_implied_inputs / set_implied_outputs */
static void set_implied_inputs (ins_chain *instr, int n_inputs, int *i_regs)
{
	int i, j;

	for (i=0; instr->in_args[i]; i++);
	for (j=0; j<n_inputs; j++) {
		instr->in_args[i+j] = new_ins_arg ();
		instr->in_args[i+j]->regconst = i_regs[j];
		instr->in_args[i+j]->flags = (ARG_REG | ARG_IMP) & ARG_FLAGMASK;
	}
}

static void set_implied_outputs (ins_chain *instr, int n_outputs, int *o_regs)
{
	int i, j;

	for (i=0; instr->out_args[i]; i++);
	for (j=0; j<n_outputs; j++) {
		instr->out_args[i+j] = new_ins_arg ();
		instr->out_args[i+j]->regconst = o_regs[j];
		instr->out_args[i+j]->flags = (ARG_REG | ARG_IMP) & ARG_FLAGMASK;
	}
}
/*}}}*/

/*{{{  helper: x64_get_register_name */
static char *x64_get_register_name (int reg)
{
	if (reg >= 0 && reg < 16) {
		return x64_regs[reg];
	}
	return "??";
}
/*}}}*/

/*{{{  static ins_chain *compose_x64_kjump */
/*
 *	composes a jump or call instruction via the calltable
 */
static ins_chain *compose_x64_kjump (tstate *ts, const int type, const int cond, const kif_entrytype *entry)
{
	return compose_ins (INS_CALL, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SCHED, offsetof(ccsp_sched_t, calltable[entry->call_offset]));
}
/*}}}*/

/*{{{  static void compose_x64_kcall */
/*
 *	creates a kernel-call (constraining registers appropriately)
 *	System V ABI: param0 in rdi(7), sched in rsi(6), Wptr in rdx(2)
 *	Call via calltable: call *offset(%r15)
 *	Return in rax(0), extra results via cparam[]
 */
static void compose_x64_kcall (tstate *ts, int call, int regs_in, int regs_out)
{
	kif_entrytype *entry = kif_entry (call);
	int to_preserve, r_in, r_out;
	int i, cregs[3], oregs[3];
	ins_chain *call_ins;

	/*{{{  check registers in/out*/
	if (regs_out > 0) {
		to_preserve = ts->stack->old_ts_depth - regs_in;
	} else {
		to_preserve = 0;
	}
	r_in = regs_in + to_preserve;
	if (regs_out < 0) {
		r_out = 0;
	} else {
		r_out = regs_out + to_preserve;
	}
	if (to_preserve < 0) {
		fprintf (stderr, "%s: warning: %d registers into kernel call %d, but only %d registers on stack\n", progname, regs_in, call, ts->stack->old_ts_depth);
		regs_out += to_preserve;
		to_preserve = 0;
	}
	if (to_preserve) {
		fprintf (stderr, "%s: warning: this doesn\'t work properly yet... (preserving regs across kernel calls)\n", progname);
		fprintf (stderr, "%s: warning: call = %d (%s), regs_in = %d, regs_out = %d, to_preserve = %d\n", progname, call, entry->entrypoint, regs_in, regs_out, to_preserve);
	}
	if (r_out > 3) {
		fprintf (stderr, "%s: warning: %d registers out from kernel call\n", progname, r_out);
	}
	tstack_checkdepth_ge (ts->stack, regs_in);
	/*}}}*/

	/*{{{  constrain operands to registers (or push into cparam)*/
	cregs[0] = ts->stack->old_a_reg;
	cregs[1] = ts->stack->old_b_reg;
	cregs[2] = ts->stack->old_c_reg;
	for (i = 0; i < r_in; i++) {
		if (options.kernel_interface & (KRNLIFACE_NEWCCSP | KRNLIFACE_RMOX)) {
			if (i > 0) {
				/* Extra args go to sched->cparam[i-1] */
				switch (constmap_typeof (cregs[i])) {
				default:
					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, cregs[i], ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)])));
					break;
				case VALUE_CONST:
					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (cregs[i]), ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)])));
					break;
				case VALUE_LABADDR:
					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (cregs[i]), ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)])));
					break;
				}
			} else {
				/* First arg constrained to rdi */
				add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
			}
		}
	}
	/*}}}*/

	if (r_in < r_out) {
		switch (r_out - r_in) {
			case 3: cregs[r_in + 2] = ts->stack->c_reg;
			case 2: cregs[r_in + 1] = ts->stack->b_reg;
			case 1: cregs[r_in + 0] = ts->stack->a_reg;
		}
		for (i = r_in; i < 1; i++) {
			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
		}
	}

	/* generate call */
	call_ins = NULL;
	if (options.kernel_interface & (KRNLIFACE_NEWCCSP | KRNLIFACE_RMOX)) {
		if (options.annotate_output) {
			char sbuf[128];
			sprintf (sbuf, "CCSP [%s]", entry->entrypoint);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
		}
		/* Move sched (r15) to rsi, Wptr (r14) to rdx before call */
		call_ins = compose_x64_kjump (ts, INS_CALL, 0, entry);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, xregs[1]));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, xregs[2]));
		add_to_ins_chain (call_ins);
		if (call_ins) {
			set_implied_inputs (call_ins, r_in > r_out ? r_in : r_out, cregs);
		}
	}
	ts->stack_drift = 0;

	/*{{{  unconstrain registers*/
	if (options.kernel_interface & (KRNLIFACE_NEWCCSP | KRNLIFACE_RMOX)) {
		for (i = 0; i < (r_in >= r_out ? r_in : 1); i++) {
			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
		}
	}
	/*}}}*/

	/*{{{  clean up and collect results */
	if (regs_out > 0) {
		tstack_undefine (ts->stack);
		constmap_clearall ();
		ts->stack->must_set_cmp_flags = 1;
		ts->stack->ts_depth = r_out;
		for (i = 0; i < r_out; i++) {
			oregs[i] = cregs[i];
			if (i == 0) {
				/* Primary result: C ABI returns in rax, move to result register.
				 * On x64, xregs[0]=rdi is the first argument register, but
				 * the C return value is in rax.  We need to move it. */
				int rax_reg = tstack_newreg (ts->stack);
				add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, rax_reg, ARG_REG, REG_RAX));
				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, rax_reg, ARG_REG, oregs[0]));
				add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, rax_reg));
			} else {
				/* Additional results from cparam[] */
				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)]), ARG_REG, oregs[i]));
			}
		}
		for (i = ts->stack->ts_depth; i < 3; i++) {
			oregs[i] = REG_UNDEFINED;
		}
		ts->stack->a_reg = oregs[0];
		ts->stack->b_reg = oregs[1];
		ts->stack->c_reg = oregs[2];
		if (call_ins) {
			set_implied_outputs (call_ins, r_out, oregs);
		}
	}
	/*}}}*/
}
/*}}}*/

/*{{{  static void compose_x64_deadlock_kcall */
static void compose_x64_deadlock_kcall (tstate *ts, int call, int regs_in, int regs_out)
{
	int this_lab;
	unsigned int x;

	if (options.debug_options & DEBUG_DEADLOCK) {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
	}
	compose_x64_kcall (ts, call, regs_in, regs_out);
	if (options.debug_options & DEBUG_DEADLOCK) {
		this_lab = ++(ts->last_lab);
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab));
		switch (call) {
		case K_IN:
		case K_IN8:
		case K_IN32:
		#if K_MIN != K_UNSUPPORTED
		case K_MIN:
		#endif
		#if K_MINN != K_UNSUPPORTED
		case K_MINN:
		#endif
		#ifdef USER_DEFINED_CHANNELS
		case K_EXTIN:
		#endif
		#ifdef K_MT_IN
		case K_MT_IN:
		#endif
			x = DLOP_IN;
			break;
		case K_OUT:
		case K_OUT8:
		case K_OUT32:
		#if K_MOUT != K_UNSUPPORTED
		case K_MOUT:
		#endif
		#if K_MOUTN != K_UNSUPPORTED
		case K_MOUTN:
		#endif
		#ifdef USER_DEFINED_CHANNELS
		case K_EXTOUT:
		#endif
		#ifdef K_MT_OUT
		case K_MT_OUT:
		#endif
			x = DLOP_OUT;
			break;
		case K_OUTBYTE:
			x = DLOP_OUTBYTE;
			break;
		case K_OUTWORD:
			x = DLOP_OUTWORD;
			break;
		case K_ALTWT:
		case K_MWALTWT:
			x = DLOP_ALTWT;
			break;
		case K_TALTWT:
			x = DLOP_TALTWT;
			break;
		case K_XABLE:
			x = DLOP_XABLE;
			break;
		#if K_MWS_SYNC != K_UNSUPPORTED
		case K_MWS_SYNC:
			x = DLOP_SYNC;
			break;
		#endif
		default:
			x = DLOP_INVALID;
			break;
		}
		x = (x << 24) + ((call & 0xff) << 16) + (ts->line_pending & 0xffff);
		declare_data_bytes (mem_ndup ((char *)&x, 4), 4);
		x = ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff);
		declare_data_bytes (mem_ndup ((char *)&x, 4), 4);
		declare_data_bytes (mem_ndup ("\xde\xad\xbe\xef", 4), 4);
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->procfile_setup_label));
		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
	}
}
/*}}}*/

/*{{{  inline scheduler operations */
/*{{{  compose_x64_inline_quick_reschedule */
static void compose_x64_inline_quick_reschedule (tstate *ts)
{
	if (options.kernel_interface & (KRNLIFACE_NEWCCSP | KRNLIFACE_RMOX)) {
		/* Fptr == NotProcess_p (0) ? */
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 2));
		/* No runnable processes: call kernel scheduler via calltable */
		compose_x64_kcall (ts, K_PAUSE, 0, 0);

		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
		/* Dequeue next process from FPTR */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
	} else {
		fprintf (stderr, "%s: warning: compose_x64_inline_quick_reschedule() does not support selected kernel interface\n", progname);
	}
}
/*}}}*/

/*{{{  compose_x64_inline_full_reschedule */
static void compose_x64_inline_full_reschedule (tstate *ts)
{
	int tmp_reg = tstack_newreg (ts->stack);
	int tmp_reg2 = tstack_newreg (ts->stack);

	/* Check sync-flags */
	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_NAMEDLABEL, string_dup ("&sf"), ARG_REG, tmp_reg2));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, tmp_reg2, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REGIND | ARG_DISP, tmp_reg2, 8, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 3));

	/* Need to reschedule due to sync activity */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));

	#ifdef PROCESS_PRIORITY
	{
		int tmp_reg3 = tstack_newreg (ts->stack);
		/* Save sched->priofinity into Wptr's Priofinity slot before re-enqueue */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg3));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg3, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
	}
	#endif	/* PROCESS_PRIORITY */

	/* Add this process to the back of the run-queue */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_FPTR));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_BPTR));

	/* Call scheduler */
	compose_x64_kcall (ts, K_PAUSE, 0, 0);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
	/* Sync flags are clear; check FPTR */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 0));

	/* Inline reschedule */
	#ifdef PROCESS_PRIORITY
	{
		int tmp_reg3 = tstack_newreg (ts->stack);
		/* Save sched->priofinity into Wptr's Priofinity slot before inline reschedule */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg3));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg3, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
	}
	#endif	/* PROCESS_PRIORITY */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_FPTR, ARG_REG, REG_BPTR, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 4));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_BPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 4));
	add_to_ins_chain (compose_ins (INS_SWAP, 2, 0, ARG_REG, REG_WPTR, ARG_REG, REG_FPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_BPTR));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
}
/*}}}*/

/*{{{  compose_x64_inline_enqueue */
static void compose_x64_inline_enqueue (tstate *ts, int preg, int nflab)
{
	if (options.kernel_interface & (KRNLIFACE_NEWCCSP | KRNLIFACE_RMOX)) {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND | ARG_DISP, preg, W_LINK));
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, nflab));

		/* empty queue */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, nflab + 1));

		/* non-empty queue */
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, nflab));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));

		/* out */
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, nflab + 1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REG, REG_BPTR));
	}
}
/*}}}*/

/*{{{  compose_x64_inline_startp */
static void compose_x64_inline_startp (tstate *ts)
{
	switch (constmap_typeof (ts->stack->old_b_reg)) {
	case VALUE_LABADDR:
	case VALUE_LABDIFF:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_b_reg),
			ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR));
		#ifdef PROCESS_PRIORITY
		{
			int tmp_reg = tstack_newreg (ts->stack);
			/* Copy sched->priofinity into new workspace's Priofinity slot */
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_PRIORITY));
		}
		#endif	/* PROCESS_PRIORITY */
		compose_x64_inline_enqueue (ts, ts->stack->old_a_reg, 0);
		break;
	case VALUE_CONST:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR));
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg), ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR,
			ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR));
		#ifdef PROCESS_PRIORITY
		{
			int tmp_reg = tstack_newreg (ts->stack);
			/* Copy sched->priofinity into new workspace's Priofinity slot */
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_PRIORITY));
		}
		#endif	/* PROCESS_PRIORITY */
		compose_x64_inline_enqueue (ts, ts->stack->old_a_reg, 1);
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
		break;
	default:
		break;
	}
}
/*}}}*/

/*{{{  compose_x64_inline_endp */
static void compose_x64_inline_endp (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_DEC, 1, 2, ARG_REGIND | ARG_DISP, REG_WPTR, W_COUNT, ARG_REGIND | ARG_DISP, REG_WPTR, W_COUNT, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 0));
	compose_x64_inline_quick_reschedule (ts);
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
}
/*}}}*/

/*{{{  compose_x64_inline_stopp */
static void compose_x64_inline_stopp (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	#ifdef PROCESS_PRIORITY
	{
		int tmp_reg = tstack_newreg (ts->stack);
		/* Save sched->priofinity into Wptr's Priofinity slot before descheduling */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
	}
	#endif	/* PROCESS_PRIORITY */
	compose_x64_inline_quick_reschedule (ts);
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
}
/*}}}*/

/*{{{  compose_x64_inline_runp */
static void compose_x64_inline_runp (tstate *ts)
{
	compose_x64_inline_enqueue (ts, ts->stack->old_a_reg, 0);
}
/*}}}*/
/*}}}*/

/*{{{  timer operations */
/*{{{  compose_x64_inline_ldtimer */
static void compose_x64_inline_ldtimer (tstate *ts)
{
	int eax_reg, edx_reg;
	int tmp_reg;

	eax_reg = tstack_newreg (ts->stack);
	edx_reg = tstack_newreg (ts->stack);
	tmp_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_RAX));
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_RDX));
	add_to_ins_chain (compose_ins (INS_RDTSC, 0, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg));

	/* rdtsc puts result in edx:eax. On x64 we combine into 64-bit rax */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
		ARG_REG | ARG_IMP, edx_reg, ARG_REG | ARG_IMP, eax_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
		ARG_REG | ARG_IMP, edx_reg, ARG_REG | ARG_IMP, eax_reg));
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg, ARG_REG, eax_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));

	ts->stack->a_reg = eax_reg;
}
/*}}}*/

/*{{{  compose_x64_inline_tin */
static void compose_x64_inline_tin (tstate *ts)
{
	int eax_reg, edx_reg;
	int tmp_reg;

	eax_reg = tstack_newreg (ts->stack);
	edx_reg = tstack_newreg (ts->stack);
	tmp_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_RAX));
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_RDX));
	add_to_ins_chain (compose_ins (INS_RDTSC, 0, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg));

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
		ARG_REG | ARG_IMP, edx_reg, ARG_REG | ARG_IMP, eax_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
		ARG_REG | ARG_IMP, edx_reg, ARG_REG | ARG_IMP, eax_reg));
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg, ARG_REG, eax_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));

	/* Time now is in eax_reg. If (Areg AFTER now), don't reschedule */
	add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, eax_reg, ARG_REG, eax_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NS, ARG_FLABEL, 0));

	/* Need to wait: store time, Iptr, status and call kernel */
	add_to_ins_chain (compose_ins (INS_INC, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, Z_WAITING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
	compose_x64_kcall (ts, K_FASTTIN, 1, 0);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
}
/*}}}*/
/*}}}*/

/*{{{  channel I/O */
/*{{{  compose_x64_inline_in */
static void compose_x64_inline_in (tstate *ts, int width)
{
	int i, lim, cregs[3], count_reg, chan_reg, dest_reg;

	cregs[0] = ts->stack->old_a_reg;
	cregs[1] = ts->stack->old_b_reg;
	cregs[2] = ts->stack->old_c_reg;
	if (width) {
		lim = 2;
		count_reg = -1;
		chan_reg = cregs[0];
		dest_reg = cregs[1];
	} else {
		lim = 3;
		count_reg = cregs[0];
		chan_reg = cregs[1];
		dest_reg = cregs[2];
	}
	for (i=0; i<lim; i++) {
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
	}

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	#ifdef PROCESS_PRIORITY
	{
		int tmp_reg = tstack_newreg (ts->stack);
		/* Save sched->priofinity into Wptr's Priofinity slot */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
	}
	#endif	/* PROCESS_PRIORITY */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 1));

	/* Channel ready: call kernel FASTIN */
	switch (width) {
	case 8:
		compose_x64_kcall (ts, K_FASTIN8, 2, 0);
		break;
	case 32:
		compose_x64_kcall (ts, K_FASTIN32, 2, 0);
		break;
	default:
		compose_x64_kcall (ts, K_FASTIN, 3, 0);
		break;
	}
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 0));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
	for (i=(lim-1); i>=0; i--) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
	}

	/* Channel not ready: block */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));

	if (options.inline_options & INLINE_SCHEDULER) {
		compose_x64_inline_quick_reschedule (ts);
	} else {
		compose_x64_kcall (ts, K_PAUSE, 0, 0);
	}

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
}
/*}}}*/

/*{{{  compose_x64_inline_in_2 */
static void compose_x64_inline_in_2 (tstate *ts, int width)
{
	int count_reg, chan_reg, dest_reg;
	int tmp_reg;
	int known_size;

	if (width) {
		count_reg = -1;
		chan_reg = ts->stack->old_a_reg;
		dest_reg = ts->stack->old_b_reg;
	} else {
		count_reg = ts->stack->old_a_reg;
		chan_reg = ts->stack->old_b_reg;
		dest_reg = ts->stack->old_c_reg;
	}

	/* Test channel word */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));

	/* Channel not ready: block */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));

	if (options.inline_options & INLINE_SCHEDULER) {
		compose_x64_inline_quick_reschedule (ts);
	} else {
		compose_x64_kcall (ts, K_PAUSE, 0, 0);
	}

	/* Channel ready: copy data inline */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	tmp_reg = tstack_newreg (ts->stack);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg));

	/* Enqueue blocked process */
	if (options.inline_options & INLINE_SCHEDULER) {
		compose_x64_inline_enqueue (ts, tmp_reg, 2);
	}

	/* Clear channel */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REGIND, chan_reg));

	if (width) {
		known_size = (width >> 3);
	} else if (constmap_typeof (count_reg) == VALUE_CONST) {
		known_size = constmap_regconst (count_reg);
	} else {
		known_size = 0;
	}

	if (known_size && (known_size <= 16)) {
		int tmp_reg2 = tstack_newreg (ts->stack);
		int tmp_reg3 = tstack_newreg (ts->stack);
		int offset = 0;

		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_POINTER, ARG_REG, tmp_reg2));
		while (known_size > 7) {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg2, offset, ARG_REG, tmp_reg3));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg3, ARG_REGIND | ARG_DISP, dest_reg, offset));
			offset += 8;
			known_size -= 8;
		}
		while (known_size > 3) {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg2, offset, ARG_REG, tmp_reg3));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg3, ARG_REGIND | ARG_DISP, dest_reg, offset));
			offset += 4;
			known_size -= 4;
		}
		while (known_size) {
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg2, offset, ARG_REG, tmp_reg3));
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, tmp_reg3, ARG_REGIND | ARG_DISP, dest_reg, offset));
			offset++;
			known_size--;
		}
	} else {
		/* Use rep movsb for general case */
		int tmp_reg2 = tstack_newreg (ts->stack);
		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_BPTR));
		ts->stack_drift += 2;
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_POINTER, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REG, REG_BPTR));
		if (known_size) {
			tmp_reg2 = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, known_size, ARG_REG, tmp_reg2));
		} else {
			tmp_reg2 = count_reg;
		}
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_RCX));
		add_to_ins_chain (compose_ins (INS_REPMOVEB, 2, 1, ARG_REG | ARG_IMP, tmp_reg2, ARG_REG | ARG_IMP, REG_FPTR, ARG_REG | ARG_IMP, REG_BPTR));
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_BPTR));
		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_FPTR));
		ts->stack_drift -= 2;
	}
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
}
/*}}}*/

/*{{{  compose_x64_inline_out */
static void compose_x64_inline_out (tstate *ts, int width)
{
	int i, lim, cregs[3], count_reg, chan_reg, src_reg;

	cregs[0] = ts->stack->old_a_reg;
	cregs[1] = ts->stack->old_b_reg;
	cregs[2] = ts->stack->old_c_reg;
	if (width) {
		lim = 2;
		count_reg = -1;
		chan_reg = cregs[0];
		src_reg = cregs[1];
	} else {
		lim = 3;
		count_reg = cregs[0];
		chan_reg = cregs[1];
		src_reg = cregs[2];
	}
	for (i=0; i<lim; i++) {
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
	}

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	#ifdef PROCESS_PRIORITY
	{
		int tmp_reg = tstack_newreg (ts->stack);
		/* Save sched->priofinity into Wptr's Priofinity slot */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
	}
	#endif	/* PROCESS_PRIORITY */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 1));

	/* Channel ready: call kernel FASTOUT */
	switch (width) {
	case 8:
		compose_x64_kcall (ts, K_FASTOUT8, 2, 0);
		break;
	case 32:
		compose_x64_kcall (ts, K_FASTOUT32, 2, 0);
		break;
	default:
		compose_x64_kcall (ts, K_FASTOUT, 3, 0);
		break;
	}
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 0));

	for (i=(lim-1); i>=0; i--) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
	}

	/* Channel not ready: block */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	if (options.inline_options & INLINE_SCHEDULER) {
		compose_x64_inline_quick_reschedule (ts);
	} else {
		compose_x64_kcall (ts, K_PAUSE, 0, 0);
	}

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
}
/*}}}*/

/*{{{  compose_x64_inline_out_2 */
static void compose_x64_inline_out_2 (tstate *ts, int width)
{
	int count_reg, chan_reg, src_reg;
	int tmp_reg, tmp_reg2;
	int known_size;

	if (width) {
		count_reg = -1;
		chan_reg = ts->stack->old_a_reg;
		src_reg = ts->stack->old_b_reg;
	} else {
		count_reg = ts->stack->old_a_reg;
		chan_reg = ts->stack->old_b_reg;
		src_reg = ts->stack->old_c_reg;
	}

	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));

	/* Channel not ready: block */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 2, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));

	if (options.inline_options & INLINE_SCHEDULER) {
		compose_x64_inline_quick_reschedule (ts);
	} else {
		compose_x64_kcall (ts, K_PAUSE, 0, 0);
	}

	/* Channel ready: copy data and enqueue */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	tmp_reg = tstack_newreg (ts->stack);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg));
	tmp_reg2 = tstack_newreg (ts->stack);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_STATUS, ARG_REG, tmp_reg2));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_READY, ARG_REG, tmp_reg2, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GT, ARG_FLABEL, 1));

	/* ALTing: check for WAITING */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_WAITING, ARG_REG, tmp_reg2, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 3));
	compose_x64_inline_enqueue (ts, tmp_reg, 4);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, tmp_reg, W_STATUS));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 2, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	if (options.inline_options & INLINE_SCHEDULER) {
		compose_x64_inline_quick_reschedule (ts);
	} else {
		compose_x64_kcall (ts, K_PAUSE, 0, 0);
	}

	/* Channel ready: enqueue process, copy data */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));

	if (width) {
		known_size = (width >> 3);
	} else if (constmap_typeof (count_reg) == VALUE_CONST) {
		known_size = constmap_regconst (count_reg);
	} else {
		known_size = 0;
	}

	if (known_size && (known_size <= 16)) {
		int tmp_reg3 = tstack_newreg (ts->stack);
		int tmp_reg4 = tstack_newreg (ts->stack);
		int offset = 0;

		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_POINTER, ARG_REG, tmp_reg3));
		while (known_size > 7) {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, offset, ARG_REG, tmp_reg4));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg4, ARG_REGIND | ARG_DISP, tmp_reg3, offset));
			offset += 8;
			known_size -= 8;
		}
		while (known_size > 3) {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, offset, ARG_REG, tmp_reg4));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg4, ARG_REGIND | ARG_DISP, tmp_reg3, offset));
			offset += 4;
			known_size -= 4;
		}
		while (known_size) {
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND | ARG_DISP, src_reg, offset, ARG_REG, tmp_reg4));
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, tmp_reg4, ARG_REGIND | ARG_DISP, tmp_reg3, offset));
			offset++;
			known_size--;
		}
	} else {
		/* Use rep movsb for general case */
		int count_r = tstack_newreg (ts->stack);
		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_BPTR));
		ts->stack_drift += 2;
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_POINTER, ARG_REG, REG_BPTR));
		if (known_size) {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, known_size, ARG_REG, count_r));
		} else {
			count_r = count_reg;
		}
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, count_r, ARG_REG, REG_RCX));
		add_to_ins_chain (compose_ins (INS_REPMOVEB, 2, 1, ARG_REG | ARG_IMP, count_r, ARG_REG | ARG_IMP, REG_FPTR, ARG_REG | ARG_IMP, REG_BPTR));
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, count_r));
		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_BPTR));
		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_FPTR));
		ts->stack_drift -= 2;
	}

	/* Enqueue blocked process and clear channel */
	compose_x64_inline_enqueue (ts, tmp_reg, 6);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REGIND, chan_reg));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
}
/*}}}*/

/*{{{  compose_x64_inline_min */
static void compose_x64_inline_min (tstate *ts, int wide)
{
	int tmp_reg1, tmp_reg2, src_reg;
	int chan_reg, dest_reg;
	int ready_lab, out_lab;

	tmp_reg1 = tstack_newreg (ts->stack);
	tmp_reg2 = tstack_newreg (ts->stack);
	src_reg = tstack_newreg (ts->stack);
	dest_reg = ts->stack->old_b_reg;
	chan_reg = ts->stack->old_a_reg;
	ready_lab = ++(ts->last_lab);
	out_lab = ++(ts->last_lab);

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, out_lab, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, ready_lab));

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	compose_x64_inline_quick_reschedule (ts);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, ready_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER, ARG_REG, src_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REG, tmp_reg1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, dest_reg, ARG_REG, tmp_reg2));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND, dest_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND, src_reg));
	if (wide) {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, 8, ARG_REG, tmp_reg1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, dest_reg, 8, ARG_REG, tmp_reg2));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND | ARG_DISP, dest_reg, 8));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, src_reg, 8));
	}
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, out_lab));
}
/*}}}*/

/*{{{  compose_x64_inline_mout */
static void compose_x64_inline_mout (tstate *ts, int wide)
{
	int tmp_reg1, tmp_reg2, dest_reg;
	int chan_reg, src_reg;

	src_reg = ts->stack->old_b_reg;
	chan_reg = ts->stack->old_a_reg;

	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	compose_x64_inline_quick_reschedule (ts);

	tmp_reg1 = tstack_newreg (ts->stack);
	dest_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg1, W_POINTER, ARG_REG, dest_reg));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, Z_READY, ARG_REG, dest_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GT, ARG_FLABEL, 2));

	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, Z_WAITING, ARG_REG, dest_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 3));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, tmp_reg1, W_STATUS));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	compose_x64_inline_quick_reschedule (ts);

	tmp_reg1 = tstack_newreg (ts->stack);
	tmp_reg2 = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg));

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REG, tmp_reg1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, dest_reg, ARG_REG, tmp_reg2));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND, dest_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND, src_reg));
	if (wide) {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, 8, ARG_REG, tmp_reg1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, dest_reg, 8, ARG_REG, tmp_reg2));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND | ARG_DISP, dest_reg, 8));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, src_reg, 8));
	}
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
}
/*}}}*/
/*}}}*/

/*{{{  ALT operations */
/*{{{  compose_pre_enbc_x64 */
static void compose_pre_enbc_x64 (tstate *ts)
{
	int skip_lab = -1;

	if ((constmap_typeof (ts->stack->old_b_reg) == VALUE_CONST) && !constmap_regconst (ts->stack->old_b_reg)) {
		return;
	} else if (constmap_typeof (ts->stack->old_b_reg) != VALUE_CONST) {
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		skip_lab = ++(ts->last_lab);
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
	}
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (intptr_t)0, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
	switch (constmap_typeof (ts->stack->old_a_reg)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
		break;
	default:
		if (skip_lab < 0) {
			skip_lab = ++(ts->last_lab);
		}
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND, ts->stack->old_a_reg));
		break;
	}
	if (skip_lab > -1) {
		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
	}
}
/*}}}*/

/*{{{  compose_pre_enbt_x64 */
static void compose_pre_enbt_x64 (tstate *ts)
{
	int skip_lab = -1;

	ts->stack->a_reg = ts->stack->old_b_reg;

	if ((constmap_typeof (ts->stack->old_b_reg) == VALUE_CONST) && !constmap_regconst (ts->stack->old_b_reg)) {
		return;
	} else if (constmap_typeof (ts->stack->old_b_reg) != VALUE_CONST) {
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		skip_lab = ++(ts->last_lab);
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
	}
	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
	switch (constmap_typeof (ts->stack->old_a_reg)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_LT, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
		break;
	default:
		if (skip_lab < 0) {
			skip_lab = ++(ts->last_lab);
		}
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_LABEL, skip_lab));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND, ts->stack->old_a_reg));
		break;
	}
	if (skip_lab > -1) {
		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
	}
}
/*}}}*/

/*{{{  compose_x64_inline_enbc */
static void compose_x64_inline_enbc (tstate *ts, int instr)
{
	int tmp_lab, out_lab;
	int guard_reg, chan_reg;

	tmp_lab = ++(ts->last_lab);
	out_lab = ++(ts->last_lab);

	guard_reg = (instr == I_ENBC) ? ts->stack->old_a_reg : ts->stack->old_b_reg;
	chan_reg = (instr == I_ENBC) ? ts->stack->old_b_reg : ts->stack->old_c_reg;

	if (constmap_typeof (guard_reg) == VALUE_CONST) {
		if (constmap_regconst (guard_reg) == 0) {
			ts->stack->a_reg = guard_reg;
			return;
		}
	} else {
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, guard_reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_lab));
	}
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (intptr_t)0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, tmp_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	constmap_remove (chan_reg);
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, tmp_lab));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
	if (instr == I_ENBC3) {
		if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LABADDR) {
			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
		} else {
			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND, ts->stack->old_a_reg));
		}
	}
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_lab));
	ts->stack->a_reg = guard_reg;
}
/*}}}*/

/*{{{  compose_x64_inline_disc */
static void compose_x64_inline_disc (tstate *ts, int instr)
{
	int tmp_reg;
	int out_rlab, out_flab, out_lab;

	out_rlab = ++(ts->last_lab);
	out_flab = ++(ts->last_lab);
	out_lab = ++(ts->last_lab);
	tmp_reg = tstack_newreg (ts->stack);

	if (constmap_typeof (ts->stack->old_b_reg) == VALUE_CONST) {
		if (constmap_regconst (ts->stack->old_b_reg) == 0) {
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = REG_UNDEFINED;
			ts->stack->c_reg = REG_UNDEFINED;
			return;
		}
	} else {
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_flab));
	}
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_WPTR, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_rlab));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (intptr_t)0, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_flab));
	if (instr == I_DISC) {
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NONE_SELECTED, ARG_REGIND, REG_WPTR, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, out_flab));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND, REG_WPTR));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
	} else {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND, REG_WPTR));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
	}
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_rlab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND, ts->stack->old_c_reg));
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_flab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_lab));
	ts->stack->a_reg = tmp_reg;
	ts->stack->b_reg = REG_UNDEFINED;
	ts->stack->c_reg = REG_UNDEFINED;
}
/*}}}*/

/*{{{  compose_x64_inline_altwt */
static void compose_x64_inline_altwt (tstate *ts)
{
	int ready_lab = ++(ts->last_lab);

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NONE_SELECTED, ARG_REGIND, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, ready_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_WAITING, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	#ifdef PROCESS_PRIORITY
	{
		int tmp_reg = tstack_newreg (ts->stack);
		/* Save sched->priofinity into Wptr's Priofinity slot before ALT wait */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, (intptr_t)CCSP_SCHED_PRIOFINITY_OFFSET, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
	}
	#endif	/* PROCESS_PRIORITY */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, ready_lab, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	compose_x64_inline_quick_reschedule (ts);
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, ready_lab));
}
/*}}}*/

/*{{{  compose_x64_inline_stlx */
static void compose_x64_inline_stlx (tstate *ts, int ins)
{
	int skip_lab = ++(ts->last_lab);

	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (intptr_t)0x8000000000000000ULL, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, skip_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
	constmap_remove (ts->stack->old_a_reg);
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, skip_lab));
	switch (ins) {
	case I_STLB:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_BPTR));
		break;
	case I_STLF:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_FPTR));
		break;
	}
}
/*}}}*/

/*{{{  compose_x64_inline_malloc */
static void compose_x64_inline_malloc (tstate *ts)
{
	/* Call dmem_alloc2 via kernel */
	compose_x64_kcall (ts, K_MALLOC, 1, 1);
}
/*}}}*/
/*}}}*/

/*{{{  memory operations */
/*{{{  compose_x64_move_loadptrs */
static void compose_x64_move_loadptrs (tstate *ts)
{
	/* This is a no-op on x64; the MOVE instruction handles it */
}
/*}}}*/

/*{{{  compose_x64_move */
static void compose_x64_move (tstate *ts)
{
	/* BLOCKCOPY: Areg=count, Breg=dst, Creg=src. Use rep movsb. */
	int count_r, src_r, dst_r;

	count_r = ts->stack->old_a_reg;
	dst_r = ts->stack->old_b_reg;
	src_r = ts->stack->old_c_reg;

	/* Materialise pointers from constmap if needed */
	switch (constmap_typeof (src_r)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (src_r), ARG_REG, src_r));
		break;
	case VALUE_LOCALPTR:
		if (constmap_regconst (src_r)) {
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, constmap_regconst (src_r) << WSH, ARG_REG, src_r));
		} else {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, src_r));
		}
		break;
	default:
		break;
	}
	constmap_remove (src_r);

	switch (constmap_typeof (dst_r)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (dst_r), ARG_REG, dst_r));
		break;
	case VALUE_LOCALPTR:
		if (constmap_regconst (dst_r)) {
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, constmap_regconst (dst_r) << WSH, ARG_REG, dst_r));
		} else {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, dst_r));
		}
		break;
	default:
		break;
	}
	constmap_remove (dst_r);
	constmap_remove (count_r);

	/* Save and use RSI/RDI for rep movsb */
	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_FPTR));
	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_BPTR));
	ts->stack_drift += 2;

	/* rep movsb: rsi=src, rdi=dst, rcx=count */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_r, ARG_REG, REG_RSI));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dst_r, ARG_REG, REG_RDI));
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, count_r, ARG_REG, REG_RCX));
	add_to_ins_chain (compose_ins (INS_REPMOVEB, 2, 1, ARG_REG | ARG_IMP, count_r, ARG_REG | ARG_IMP, REG_RSI, ARG_REG | ARG_IMP, REG_RDI));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, count_r));

	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_BPTR));
	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_FPTR));
	ts->stack_drift -= 2;
}
/*}}}*/

/*{{{  compose_x64_shift */
static void compose_x64_shift (tstate *ts, int sec, int r1, int r2, int r3)
{
	/* x64 variable shifts must use cl register */
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, r1, ARG_REG, REG_RCX));
	if (sec == I_SHL) {
		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
	} else if (sec == I_SHR) {
		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
	} else {
		/* Arithmetic right shift */
		add_to_ins_chain (compose_ins (INS_SAR, 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
	}
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, r1));
}
/*}}}*/

/*{{{  compose_x64_division */
static void compose_x64_division (tstate *ts, int dividend, int divisor, int quotient)
{
	/* x64 division: idiv divides rdx:rax by operand. Quotient in rax, remainder in rdx. */
	int eax_reg = tstack_newreg (ts->stack);
	int edx_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_RAX));
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_RDX));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dividend, ARG_REG, eax_reg));
	add_to_ins_chain (compose_ins (INS_CDQ, 0, 0));
	add_to_ins_chain (compose_ins (INS_DIV, 3, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg, ARG_REG, divisor,
		ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, eax_reg, ARG_REG, quotient));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));
}
/*}}}*/

/*{{{  compose_x64_remainder */
static int compose_x64_remainder (tstate *ts, int dividend, int divisor)
{
	int eax_reg = tstack_newreg (ts->stack);
	int edx_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_RAX));
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_RDX));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dividend, ARG_REG, eax_reg));
	add_to_ins_chain (compose_ins (INS_CDQ, 0, 0));
	add_to_ins_chain (compose_ins (INS_DIV, 3, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg, ARG_REG, divisor,
		ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg));
	/* Remainder is in rdx */
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
	return edx_reg;
}
/*}}}*/
/*}}}*/

/*{{{  widen operations */
/*{{{  compose_x64_widenshort */
static int compose_x64_widenshort (tstate *ts)
{
	int tmp_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_MOVESEXT16TO32, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
	ts->stack->a_reg = tmp_reg;
	return tmp_reg;
}
/*}}}*/

/*{{{  compose_x64_widenword */
static int compose_x64_widenword (tstate *ts)
{
	int tmp_reg = tstack_newreg (ts->stack);

	/* Sign-extend 32-bit INT to double-word (hi:lo).
	 * Areg holds the 32-bit value. We need:
	 *   a_reg (lo) = Areg (unchanged)
	 *   b_reg (hi) = sign extension (0 or 0xFFFFFFFF) */
	ts->stack->a_reg = ts->stack->old_a_reg;
	ts->stack->b_reg = tmp_reg;
	ts->stack->c_reg = ts->stack->old_b_reg;

	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_SAR, 2, 1, ARG_CONST, (intptr_t)63, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
	constmap_remove (tmp_reg);
	return tmp_reg;
}
/*}}}*/
/*}}}*/

/*{{{  external call functions */
/*{{{  compose_external_ccall_x64 */
static void compose_external_ccall_x64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last)
{
	int tmp_reg;

	/* Pass Wptr + 1 word as first arg (rdi) */
	tmp_reg = tstack_newreg (ts->stack);
	*pst_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (1 << WSH), ARG_REG, tmp_reg);
	add_to_ins_chain (*pst_first);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, REG_RDI));

	/* Build symbol name: extref_prefix + name+1 (skip 'C', keep '.') */
	if (options.extref_prefix) {
		char sbuf[256];
		sprintf (sbuf, "%s%s", options.extref_prefix, name + 1);
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
	} else {
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
	}

	/* Restore Wptr adjustment */
	if (!options.nocc_codegen) {
		*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, (intptr_t)(4 << WSH), ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
	} else {
		*pst_last = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# external ccall complete"));
	}
	add_to_ins_chain (*pst_last);
	constmap_clearall ();
}
/*}}}*/

/*{{{  compose_x64_bcall */
static void compose_x64_bcall (tstate *ts, int inlined, int kernel_call, int unused, char *name, ins_chain **pst_first, ins_chain **pst_last)
{
	int arg_reg;

	arg_reg = tstack_newreg (ts->stack);

	/* Set up workspace pointer parameter */
	add_to_ins_chain (*pst_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (1 << WSH), ARG_REG, arg_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, arg_reg, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[0])));

	if (kernel_call != K_KERNEL_RUN) {
		int name_offset = (inlined == 2) ? 1 : 2;
		int tmp_reg = tstack_newreg (ts->stack);

		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_RDI));
		if (options.extref_prefix) {
			char sbuf[256];
			sprintf (sbuf, "%s%s", options.extref_prefix, name + name_offset);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, tmp_reg));
		} else {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + name_offset), ARG_REG, tmp_reg));
		}
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
	}

	compose_x64_kcall (ts, kernel_call, 1, 0);
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, (4 << WSH), ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));

	*pst_last = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# bcall complete"));
	add_to_ins_chain (*pst_last);
	constmap_clearall ();
}
/*}}}*/

/*{{{  compose_x64_cif_call */
static void compose_x64_cif_call (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last)
{
	char sbuf[256];

	/* Save resumption label and sched pointer */
	*pst_first = compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-8 << WSH));
	add_to_ins_chain (*pst_first);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, (intptr_t)(-1), ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-7 << WSH)));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-6 << WSH)));

	/* Pass Wptr+1 word in rdi */
	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(1 << WSH), ARG_REG, REG_RDI));

	/* Build CIF function name and call via ccsp_cif_process_call */
	sprintf (sbuf, "@%s%s", options.extref_prefix ? options.extref_prefix : "", name + 4);

	/* Load function address into rsi (second arg) */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, REG_RSI));

	{
		char cif_call_buf[64];
		snprintf (cif_call_buf, sizeof(cif_call_buf), "@%sccsp_cif_process_call",
			options.extref_prefix ? options.extref_prefix : "");
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (cif_call_buf)));
	}

	/* Restore state after CIF call */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-6 << WSH), ARG_REG, REG_SCHED));

	/* Advance Wptr by 4 words */
	*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, (intptr_t)(4 << WSH), ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
	add_to_ins_chain (*pst_last);
	constmap_clearall ();
}
/*}}}*/
/*}}}*/

/*{{{  code generation functions */
/*{{{  compose_x64_entry_prolog */
static void compose_x64_entry_prolog (tstate *ts)
{
	rtl_chain *trtl;

	/* Generate the _occam_start entry point symbol.
	 * The runtime (occam_entry.c) expects 'extern void _occam_start(void)'.
	 * On Linux this is '_occam_start', on Darwin '__occam_start'.
	 * We bypass x64_modify_name here because '^occam_start' would produce
	 * 'E_occam_start' which doesn't match. Match aarch64 behavior. */
	{
		char sbuf[64];
		const char *ext = options.extref_prefix ? options.extref_prefix : "";
		sprintf (sbuf, "%s_occam_start", ext);
		trtl = new_rtl ();
		trtl->type = RTL_PUBLICSETNAMEDLABEL;
		trtl->u.label_name = string_dup (sbuf);
		add_to_rtl_chain (trtl);
	}

	/* x64 FPU init: use default settings */
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# x64 FPU init")));
}
/*}}}*/

/*{{{  compose_x64_return */
static void compose_x64_return (tstate *ts)
{
	int toldregs[3], tfixedregs[3];
	int i;

	toldregs[0] = ts->stack->old_a_reg;
	toldregs[1] = ts->stack->old_b_reg;
	toldregs[2] = ts->stack->old_c_reg;
	tfixedregs[0] = X64_REG_FUNCRES0;
	tfixedregs[1] = X64_REG_FUNCRES1;
	tfixedregs[2] = X64_REG_FUNCRES2;

	for (i = 0; i < ts->numfuncresults; i++) {
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, toldregs[i], ARG_REG, tfixedregs[i]));
	}

	/* Pop I_CALL frame: 4 words = 32 bytes on 64-bit.
	 * The return address was stored at Wptr[0] (by the I_CALL stub or JENTRY)
	 * BEFORE the frame pop.  After adding 4<<WSH, read it back at -(4<<WSH)
	 * from the new Wptr -- matching the i386 pattern (PUSH Wptr[-16]; RET). */
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, (intptr_t)(4 << WSH), ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, -(4 << WSH)));

	for (i = ts->numfuncresults - 1; i >= 0; i--) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, toldregs[i]));
	}
	ts->numfuncresults = 0;
}
/*}}}*/

/*{{{  compose_x64_nreturn */
static void compose_x64_nreturn (tstate *ts, int adjust)
{
	int tmp_reg = tstack_newreg (ts->stack);
	int toldregs[3], tfixedregs[3];
	int i, nresults;

	toldregs[0] = ts->stack->old_a_reg;
	toldregs[1] = ts->stack->old_b_reg;
	toldregs[2] = ts->stack->old_c_reg;
	tfixedregs[0] = X64_REG_FUNCRES0;
	tfixedregs[1] = X64_REG_FUNCRES1;
	tfixedregs[2] = X64_REG_FUNCRES2;

	nresults = ts->numfuncresults;

	for (i = 0; i < nresults; i++) {
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, toldregs[i], ARG_REG, tfixedregs[i]));
	}

	/* Load return address from Wptr[0] */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, REG_WPTR, ARG_REG, tmp_reg));

	if (adjust != 0) {
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, adjust << WSH, ARG_REG, REG_WPTR));
	}

	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND, tmp_reg));

	for (i = nresults - 1; i >= 0; i--) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, toldregs[i]));
	}
	ts->numfuncresults = 0;
}
/*}}}*/

/*{{{  compose_x64_funcresults */
static void compose_x64_funcresults (tstate *ts, int nresults)
{
	int i;
	int tnewregs[3], tfixedregs[3];

	for (i = 0; i < nresults; i++) {
		tstack_push (ts->stack);
	}
	tfixedregs[0] = X64_REG_FUNCRES0;
	tfixedregs[1] = X64_REG_FUNCRES1;
	tfixedregs[2] = X64_REG_FUNCRES2;
	tnewregs[0] = ts->stack->a_reg;
	tnewregs[1] = ts->stack->b_reg;
	tnewregs[2] = ts->stack->c_reg;

	for (i = 0; i < nresults; i++) {
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tnewregs[i], ARG_REG, tfixedregs[i]));
	}
	for (i = 0; i < nresults; i++) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tnewregs[i]));
	}
}
/*}}}*/
/*}}}*/

/*{{{  long operations */
static void compose_x64_longop (tstate *ts, int sec)
{
	switch (sec) {
	case I_LADD:
		add_to_ins_chain (compose_ins (INS_ADD, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg,
			ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case I_LSUB:
		add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg,
			ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case I_LSUM:
		/* LSUM: carry_out, sum := Areg + Breg */
		{
			int carry = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_ADD, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg,
				ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, carry));
			add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_CONST, 0, ARG_REG, carry, ARG_REG, carry));
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = carry;
		}
		break;
	case I_LDIFF:
		/* LDIFF: borrow_out, diff := Breg - Areg */
		{
			int borrow = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg,
				ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, borrow));
			add_to_ins_chain (compose_ins (INS_SBB, 2, 1, ARG_CONST, 0, ARG_REG, borrow, ARG_REG, borrow));
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = borrow;
		}
		break;
	case I_LMUL:
		/* LMUL: hi, lo := (unsigned)Areg * (unsigned)Breg + (unsigned)Creg */
		{
			int eax_reg = tstack_newreg (ts->stack);
			int edx_reg = tstack_newreg (ts->stack);

			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_RAX));
			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_RDX));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, eax_reg));
			add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG, ts->stack->old_b_reg,
				ARG_REG | ARG_IMP, edx_reg, ARG_REG | ARG_IMP, eax_reg));
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, eax_reg, ARG_REG, eax_reg));
			add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_CONST, 0, ARG_REG, edx_reg, ARG_REG, edx_reg));
			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));
			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
			ts->stack->a_reg = eax_reg;
			ts->stack->b_reg = edx_reg;
		}
		break;
	case I_LDIV:
		/* LDIV: quotient, remainder := (Creg:Breg) / Areg */
		{
			int eax_reg = tstack_newreg (ts->stack);
			int edx_reg = tstack_newreg (ts->stack);

			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_RAX));
			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_RDX));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, eax_reg));
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, edx_reg));
			add_to_ins_chain (compose_ins (INS_UDIV, 3, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg, ARG_REG, ts->stack->old_a_reg,
				ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg));
			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));
			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
			ts->stack->a_reg = eax_reg;
			ts->stack->b_reg = edx_reg;
		}
		break;
	case I_LSHL:
		{
			int tmp_reg = tstack_newreg (ts->stack);

			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_RCX));
			add_to_ins_chain (compose_ins (INS_SHLD, 3, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg,
				ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = ts->stack->old_c_reg;
		}
		break;
	case I_LSHR:
		{
			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_RCX));
			add_to_ins_chain (compose_ins (INS_SHRD, 3, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_c_reg,
				ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = ts->stack->old_c_reg;
		}
		break;
	case I_TESTSTS:
	case I_TESTSTE:
	case I_TESTSTD:
	case I_TESTERR:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
		constmap_remove (ts->stack->old_a_reg);
		break;
	default:
		fprintf (stderr, "%s: unsupported x64 long operation %d (0x%x)\n", progname, sec, sec);
		break;
	}
}
/*}}}*/

/*{{{  floating point operations -- uses x87 FPU stack */
static void compose_x64_fpop (tstate *ts, int sec)
{
	switch (sec) {
	case I_FPADD:
		add_to_ins_chain (compose_ins (INS_FADD, 0, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPSUB:
		add_to_ins_chain (compose_ins (INS_FSUB, 0, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPMUL:
		add_to_ins_chain (compose_ins (INS_FMUL, 0, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPDIV:
		add_to_ins_chain (compose_ins (INS_FDIV, 0, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPSQRT:
		add_to_ins_chain (compose_ins (INS_FSQRT, 0, 0));
		break;
	case I_FPABS:
		add_to_ins_chain (compose_ins (INS_FABS, 0, 0));
		break;
	case I_FPREV:
		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
		break;
	case I_FPDUP:
		add_to_ins_chain (compose_ins (INS_FLD, 1, 0, ARG_FREG, 0));
		ts->stack->fs_depth++;
		break;
	case I_FPRN:
		compose_fp_set_fround_x64 (ts, 0);
		break;
	case I_FPRZ:
		compose_fp_set_fround_x64 (ts, 3);
		break;
	case I_FPRP:
		compose_fp_set_fround_x64 (ts, 2);
		break;
	case I_FPRM:
		compose_fp_set_fround_x64 (ts, 1);
		break;
	case I_FPREM:
		add_to_ins_chain (compose_ins (INS_FPREM1, 0, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPEQ:
		{
			int tmp_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_FUCOM, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG | ARG_IMP, REG_RAX));
			add_to_ins_chain (compose_ins (INS_SAHF, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_E, ARG_REG | ARG_IS8BIT, tmp_reg));
			ts->stack->a_reg = tmp_reg;
		}
		break;
	case I_FPGT:
		{
			int tmp_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_FUCOM, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG | ARG_IMP, REG_RAX));
			add_to_ins_chain (compose_ins (INS_SAHF, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_A, ARG_REG | ARG_IS8BIT, tmp_reg));
			ts->stack->a_reg = tmp_reg;
		}
		break;
	case I_FPORDERED:
		{
			int tmp_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_FUCOM, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG | ARG_IMP, REG_RAX));
			add_to_ins_chain (compose_ins (INS_SAHF, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_FPORD, ARG_REG | ARG_IS8BIT, tmp_reg));
			ts->stack->a_reg = tmp_reg;
		}
		break;
	case I_FPNAN:
		{
			int tmp_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_FTST, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG | ARG_IMP, REG_RAX));
			add_to_ins_chain (compose_ins (INS_SAHF, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_FPNAN, ARG_REG | ARG_IS8BIT, tmp_reg));
			ts->stack->a_reg = tmp_reg;
		}
		break;
	case I_FPNOTFINITE:
		{
			int tmp_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_FXAM, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG | ARG_IMP, REG_RAX));
			add_to_ins_chain (compose_ins (INS_SAHF, 0, 0));
			add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
			ts->stack->fs_depth--;
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_FPINFNAN, ARG_REG | ARG_IS8BIT, tmp_reg));
			ts->stack->a_reg = tmp_reg;
		}
		break;
	case I_FPCHKERR:
		/* Do nothing for now */
		break;
	case I_FPSTNLI32:
		add_to_ins_chain (compose_ins (INS_FIST32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPLDNLSN:
		add_to_ins_chain (compose_ins (INS_FLD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		ts->stack->fs_depth++;
		break;
	case I_FPLDNLDB:
		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		ts->stack->fs_depth++;
		break;
	case I_FPSTNLSN:
		add_to_ins_chain (compose_ins (INS_FST32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPSTNLDB:
		add_to_ins_chain (compose_ins (INS_FST64, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		add_to_ins_chain (compose_ins (INS_FSTP, 1, 0, ARG_FREG, 0));
		ts->stack->fs_depth--;
		break;
	case I_FPINT:
		add_to_ins_chain (compose_ins (INS_FRNDINT, 0, 0));
		break;
	case I_FPR32TOR64:
		/* Widen: already on x87 stack, no-op */
		break;
	case I_FPR64TOR32:
		/* Narrow: store as 32-bit and reload */
		/* The x87 FPU handles precision internally */
		break;
	case I_FPRANGE:
		/* Range check -- no-op for now */
		break;
	default:
		fprintf (stderr, "%s: unsupported x64 fp operation %d (0x%x)\n", progname, sec, sec);
		break;
	}
}
/*}}}*/

/*{{{  FP control */
static void compose_fp_set_fround_x64 (tstate *ts, int mode)
{
	/* x87 rounding mode: change FPCW bits 10-11 */
	int tmp_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_FSTCW, 0, 1, ARG_REGIND | ARG_DISP, REG_WPTR, -16));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, -16, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xF3FF, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_CONST, (intptr_t)(mode << 10), ARG_REG, tmp_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, -16));
	add_to_ins_chain (compose_ins (INS_FLDCW, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, -16));
}

static void compose_fp_init_x64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# x64 FPU init")));
}

static void compose_reset_fregs_x64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# x64 FPU reset")));
}
/*}}}*/

/*{{{  debugging & error handling */
static void compose_debug_insert_x64 (tstate *ts, int mdpairid)
{
	unsigned int x;

	if ((options.debug_options & DEBUG_INSERT) && !(ts->supress_debug_insert)) {
		x = ((ts->file_pending & 0xffff) << 16) + (ts->line_pending & 0xffff);
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# debug insert")));
	}
}

static void compose_debug_procnames_x64 (tstate *ts)
{
	char *procname_buffer;
	int procname_buflen, procname_fcount, procname_vcount, procname_strlen;
	static char *procname_nulls = "\\0\\0\\0\\0";
	rtl_chain *trtl;

	procname_buflen = 64000;
	procname_buffer = (char *)smalloc (procname_buflen);
	procname_fcount = ((ts->proc_cur + 1) * sizeof(int));
	*(int *)procname_buffer = ts->proc_cur;
	for (procname_vcount=0; procname_vcount < ts->proc_cur; procname_vcount++) {
		((int *)procname_buffer)[procname_vcount+1] = procname_fcount;
		procname_strlen = strlen (ts->proc_list[procname_vcount]);
		memcpy (procname_buffer + procname_fcount, ts->proc_list[procname_vcount], procname_strlen);
		procname_fcount += procname_strlen;
		memcpy (procname_buffer + procname_fcount, procname_nulls, 4 - (procname_fcount % 4));
		procname_fcount += (4 - (procname_fcount % 4));
	}
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procedure_label));
	flush_ins_chain ();
	trtl = new_rtl ();
	trtl->type = RTL_DATA;
	trtl->u.data.bytes = (char *)smalloc (procname_fcount);
	trtl->u.data.length = procname_fcount;
	memcpy (trtl->u.data.bytes, procname_buffer, procname_fcount);
	add_to_rtl_chain (trtl);
	sfree (procname_buffer);
}

static void compose_debug_filenames_x64 (tstate *ts)
{
	char *filename_buffer;
	int filename_buflen, filename_fcount, filename_vcount, filename_strlen;
	static char *filename_nulls = "\\0\\0\\0\\0";
	rtl_chain *trtl;

	filename_buflen = 10000;
	filename_buffer = (char *)smalloc (filename_buflen);
	filename_fcount = ((ts->file_cur + 1) * sizeof(int));
	*(int *)filename_buffer = ts->file_cur;
	for (filename_vcount = 0; filename_vcount < ts->file_cur; filename_vcount++) {
		((int *)filename_buffer)[filename_vcount+1] = filename_fcount;
		filename_strlen = strlen (ts->file_list[filename_vcount]);
		memcpy (filename_buffer + filename_fcount, ts->file_list[filename_vcount], filename_strlen);
		filename_fcount += filename_strlen;
		memcpy (filename_buffer + filename_fcount, filename_nulls, 4 - (filename_fcount % 4));
		filename_fcount += (4 - (filename_fcount % 4));
	}
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->filename_label));
	flush_ins_chain ();
	trtl = new_rtl ();
	trtl->type = RTL_DATA;
	trtl->u.data.bytes = (char *)smalloc (filename_fcount);
	trtl->u.data.length = filename_fcount;
	memcpy (trtl->u.data.bytes, filename_buffer, filename_fcount);
	add_to_rtl_chain (trtl);
	sfree (filename_buffer);
}

static void compose_debug_zero_div_x64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->zerodiv_label));
	compose_x64_kcall (ts, K_ZERODIV, 0, 0);
}

static void compose_debug_floaterr_x64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floaterr_label));
	compose_x64_kcall (ts, K_FLOATERR, 0, 0);
}

static void compose_debug_overflow_x64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->overflow_label));
	compose_x64_kcall (ts, K_OVERFLOW, 0, 0);
}

static void compose_debug_rangestop_x64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->range_entry_label));
	compose_x64_kcall (ts, K_RANGERR, 0, 0);
}

static void compose_debug_seterr_x64 (tstate *ts)
{
	unsigned int x;

	x = (0xfb00 << 16) + (ts->line_pending & 0xffff);
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("# seterr")));
	compose_x64_kcall (ts, K_SETERR, 0, 0);
}

static void compose_overflow_jumpcode_x64 (tstate *ts, int dcode)
{
	if (options.debug_options & DEBUG_OVERFLOW) {
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->overflow_label));
	}
}

static void compose_floaterr_jumpcode_x64 (tstate *ts)
{
	if (options.debug_options & DEBUG_FLOAT) {
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->floaterr_label));
	}
}

static void compose_rangestop_jumpcode_x64 (tstate *ts, int rcode)
{
	if (options.debug_options & DEBUG_RANGESTOP) {
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->range_entry_label));
	}
}

static void compose_debug_deadlock_set_x64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procfile_setup_label));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_RAX));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_RDX));
	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
}

static void compose_divcheck_zero_x64 (tstate *ts, int reg)
{
	int this_lab;
	unsigned int x;

	switch (constmap_typeof (reg)) {
	default:
		this_lab = ++(ts->last_lab);
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, this_lab));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
		break;
	case VALUE_CONST:
		if (constmap_regconst (reg) == 0) {
			fprintf (stderr, "%s: serious: division by zero seen around line %d\n", progname, ts->line_pending);
			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
		}
		break;
	}
}

static void compose_divcheck_zero_simple_x64 (tstate *ts, int reg)
{
	switch (constmap_typeof (reg)) {
	default:
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 2));
		compose_x64_kcall (ts, K_BNSETERR, 0, 0);
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
		break;
	case VALUE_CONST:
		if (constmap_regconst (reg) == 0) {
			fprintf (stderr, "%s: serious: division by zero seen around line %d\n", progname, ts->line_pending);
			compose_x64_kcall (ts, K_BNSETERR, 0, 0);
		}
		break;
	}
}
/*}}}*/

/*{{{  I/O space operations */
static int compose_iospace_loadbyte_x64 (tstate *ts, int portreg, int targetreg)
{
	int tmp_reg = tstack_newreg (ts->stack);
	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, portreg, ARG_REG, tmp_reg));
	return tmp_reg;
}

static void compose_iospace_storebyte_x64 (tstate *ts, int portreg, int sourcereg)
{
	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, sourcereg, ARG_REGIND, portreg));
}

static int compose_iospace_loadword_x64 (tstate *ts, int portreg, int targetreg)
{
	int tmp_reg = tstack_newreg (ts->stack);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, portreg, ARG_REG, tmp_reg));
	return tmp_reg;
}

static void compose_iospace_storeword_x64 (tstate *ts, int portreg, int sourcereg)
{
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, sourcereg, ARG_REGIND, portreg));
}

static void compose_iospace_read_x64 (tstate *ts, int portreg, int addrreg, int width)
{
	int tmp_reg = tstack_newreg (ts->stack);

	switch (width) {
	case 8:
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, portreg, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, addrreg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, portreg, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, addrreg));
		break;
	}
}

static void compose_iospace_write_x64 (tstate *ts, int portreg, int addrreg, int width)
{
	int tmp_reg = tstack_newreg (ts->stack);

	switch (width) {
	case 8:
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, addrreg, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, portreg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, addrreg, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, portreg));
		break;
	}
}
/*}}}*/

/*{{{  miscellaneous */
static void compose_refcountop_x64 (tstate *ts, int op, int reg)
{
	switch (op) {
	case I_RCINIT:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REGIND, reg));
		break;
	case I_RCINC:
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 1, ARG_REGIND, reg, ARG_REGIND, reg));
		ts->stack->must_set_cmp_flags = 1;
		break;
	case I_RCDEC:
		add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_CONST, 1, ARG_REGIND, reg, ARG_REGIND, reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, reg));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, reg));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
		constmap_remove (reg);
		ts->stack->must_set_cmp_flags = 0;
		break;
	default:
		fprintf (stderr, "%s: compose_refcountop_x64: unknown operation %d\n", progname, op);
		break;
	}
}

static void compose_memory_barrier_x64 (tstate *ts, int sec)
{
	switch (sec) {
	case I_MB:
		add_to_ins_chain (compose_ins (INS_MB, 0, 0));
		break;
	case I_RMB:
		add_to_ins_chain (compose_ins (INS_RMB, 0, 0));
		break;
	case I_WMB:
		add_to_ins_chain (compose_ins (INS_WMB, 0, 0));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MB, 0, 0));
		break;
	}
	tstack_undefine (ts->stack);
	constmap_clearall ();
}

static void x64_stub_rmox_entry_prolog (tstate *ts, rmoxmode_e mode)
{
	/* RMoX not supported on x64 */
}
/*}}}*/

/*{{{  register allocation */
static int x64_regcolour_special_to_real (int reg)
{
	switch (reg) {
	case REG_WPTR:  return X64_REG_WPTR;	/* r14 */
	case REG_FPTR:  return X64_REG_FPTR;	/* r13 */
	case REG_BPTR:  return X64_REG_BPTR;	/* r12 */
	case REG_SCHED: return X64_REG_SCHED;	/* r15 */
	case REG_SPTR:  return REG_RSP;		/* rsp (4) */
	}
	return reg;
}

static int x64_regcolour_get_regs (int *regs)
{
	/* 10 allocatable registers: rax,rcx,rdx,rbx,rsi,rdi,r8,r9,r10,r11 */
	regs[0] = REG_RAX;
	regs[1] = REG_RCX;
	regs[2] = REG_RDX;
	regs[3] = REG_RBX;
	regs[4] = REG_RSI;
	regs[5] = REG_RDI;
	regs[6] = REG_R8;
	regs[7] = REG_R9;
	regs[8] = REG_R10;
	regs[9] = REG_R11;
	return 10;
}

static int x64_regcolour_fp_regs (int *regs)
{
	int i;
	for (i = 0; i < 8; i++) {
		regs[i] = i;
	}
	return 8;
}
/*}}}*/

/*{{{  RTL validation */
static int x64_rtl_validate_instr (ins_chain *ins)
{
	return 1;
}

static int x64_rtl_prevalidate_instr (ins_chain *ins)
{
	return 1;
}
/*}}}*/

/*{{{  assembly output engine */

/*{{{  static char *x64_modify_name (char *name) */
static char *x64_modify_name (char *name)
{
#define MAXNAME 128
	static char rbuf[MAXNAME];
	int i, j, len;

	j = 0;
	len = strlen (name);
	for (i=0; i<len; i++) {
		switch (name[i]) {
		case '.':
			rbuf[j++] = '_';
			break;
		case '$':
		case '^':
		case '*':
		case '@':
		case '&':
			break;
		case '%':
			i = len;
			break;
		default:
			rbuf[j++] = name[i];
			break;
		}
	}
	rbuf[j] = '\0';

	const char *prepend = NULL;
	if ((rbuf[0] == '_') || (!strncmp (rbuf, "O_", 2)) || (!strncmp (rbuf, "DCR_", 4)) || (name[0] == '&') || (name[0] == '@')) {
		/* skip */
	} else if (name[0] == '^') {
		prepend = "E_";
	} else if (name[0] == '*') {
		prepend = "M_";
	} else {
		prepend = "O_";
	}
	if (prepend) {
		int plen = strlen(prepend);
		memmove (rbuf + plen, rbuf, strlen(rbuf) + 1);
		memcpy (rbuf, prepend, plen);
	}

	if (options.extref_prefix && (prepend || name[0] == '&')) {
		int plen = strlen(options.extref_prefix);
		memmove (rbuf + plen, rbuf, strlen(rbuf) + 1);
		memcpy (rbuf, options.extref_prefix, plen);
	}

	return rbuf;
}
/*}}}*/

/*{{{  static int x64_drop_arg (ins_arg *arg, FILE *outstream) */
static int x64_drop_arg (ins_arg *arg, FILE *outstream)
{
	ins_sib_arg *t_sib;
	char *tptr;
	char **regset;

	if (arg->flags & ARG_IND) {
		fprintf (outstream, "*");
	}
	if (arg->flags & ARG_DISP) {
		fprintf (outstream, "%d", arg->disp);
	}
	if (arg->flags & ARG_ISCONST) {
		fprintf (outstream, "$");
	}
	if (arg->flags & ARG_IS8BIT) {
		regset = x64_regs8;
	} else if (arg->flags & ARG_IS16BIT) {
		regset = x64_regs16;
	} else {
		regset = x64_regs;
	}
	switch (arg->flags & ARG_MODEMASK) {
	case ARG_REG:
		{
			int r = (int)(long)arg->regconst;
			if (r < 0) r = x64_regcolour_special_to_real (r);
			fprintf (outstream, "%s", (r < 0 || r >= 16) ? "??" : regset[r]);
		}
		break;
	case ARG_FREG:
		fprintf (outstream, "%s", ((long)arg->regconst < 0) ? "??" : x64_fregs[(long)arg->regconst]);
		break;
	case ARG_REGIND:
		{
			int r = (int)(long)arg->regconst;
			if (r < 0) r = x64_regcolour_special_to_real (r);
			fprintf (outstream, "(%s)", (r < 0 || r >= 16) ? "??" : x64_regs[r]);
		}
		break;
	case ARG_CONST:
		fprintf (outstream, "%ld", (long)arg->regconst);
		break;
	case ARG_LABEL:
		fprintf (outstream, LBLPFX "%ld", (long)arg->regconst);
		break;
	case ARG_INSLABEL:
		fprintf (outstream, LBLPFX "%ld", (long)((ins_chain *)(long)arg->regconst)->in_args[0]->regconst);
		break;
	case ARG_FLABEL:
		fprintf (outstream, "%ldf", (long)arg->regconst);
		break;
	case ARG_BLABEL:
		fprintf (outstream, "%ldb", (long)arg->regconst);
		break;
	case ARG_NAMEDLABEL:
		fprintf (outstream, "%s", x64_modify_name ((char *)(long)arg->regconst));
		break;
	case ARG_TEXT:
		for (tptr = (char *)(long)arg->regconst; (*tptr == ' ') || (*tptr == '\t'); tptr++);
		fprintf (outstream, "%s", tptr);
		break;
	case ARG_REGINDSIB:
		t_sib = (ins_sib_arg *)(long)arg->regconst;
		{
			int rb = t_sib->base, ri = t_sib->index;
			if (rb < 0) rb = x64_regcolour_special_to_real (rb);
			if (ri < 0) ri = x64_regcolour_special_to_real (ri);
			fprintf (outstream, "(%s,%s,%d)", x64_regs[rb], x64_regs[ri], t_sib->scale);
		}
		break;
	}
	return 0;
}
/*}}}*/

/*{{{  static int x64_disassemble_code (ins_chain *ins, FILE *outstream, int regtrace) */
static int x64_disassemble_code (ins_chain *ins, FILE *outstream, int regtrace)
{
	static char *codes[] = {"..", "movq", "nop", "leaq", "set", "cmpq", "addq", "andq", "orq", "into", \
		"..", "..", "pushq", "popq", "retq", "callq", "..", "jmp", "xchgq", "decq", \
		"incq", "subq", "cqto", "xorq", "imulq", "rdtsc", "..", "cqto", "idivq", "shlq", \
		"shrq", "movb", "notq", "..", "..", "movzbq", "rcrq", "rclq", "rorq", "rolq", \
		"adcq", "sbbq", "mulq", "divq", "shldq", "shrdq", "fnstcw", "fldcw", "fwait", "fstp", \
		"#", "fildl", "fxch", "fld", "fildll", "flds", "fldl", "fldt", "fadds", "faddl", \
		"fmuls", "fmull", "fstps", "fstpl", "fstpt", "fistps", "fistpl", "fld1", "fldl2t", "fldl2e", \
		"fldpi", "fldlg2", "fldln2", "fldz", "faddp", "fsubrp", "fmulp", "fdivrp", "sahf", "ftst", \
		"fxam", "fnstsw", "fucom", "fucomp", "fucompp", "fcom", "fcomp", "fcompp", "frndint", "fsqrt", \
		"fabs", "fchs", "fscale", "fprem1", "..", "movswq", "lahf", "..", "..", "..", \
		"inb", "outb", "callq", "movzwq", "movw", "..", "..", "..", "..", "fsin", \
		"fcos", "inw", "outw", "inl", "outl", "lock", "fptan", "mfence", "lfence", "sfence", \
		"..", "movl", "movl", "movslq", "..", "..", "sarq"};
	static char *setcc_tailcodes[] = {"o", "no", "b", "ae", "e", "nz", "be", "a", "s", "ns", "pe", "po", "l", "ge", "le", "g", "..", ".."};
	ins_chain *tmp;
	ins_arg *arg;
	int i, tlab1, tlab2;
	char *tptr;

	for (tmp=ins; tmp; tmp=tmp->next) {
		if ((tmp->type > INS_LAST) || (tmp->type < 0)) {
			continue;
		}
		switch (tmp->type) {
		case INS_SETLABEL:
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, ":\n");
			break;
		case INS_SETFLABEL:
			fprintf (outstream, "%ld:\n", (long)tmp->in_args[0]->regconst);
			break;
		case INS_LOADLABDIFF:
		case INS_CONSTLABDIFF:
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_INSLABEL) {
				tlab1 = ((ins_chain *)tmp->in_args[0]->regconst)->in_args[0]->regconst;
			} else {
				tlab1 = tmp->in_args[0]->regconst;
			}
			if ((tmp->in_args[1]->flags & ARG_MODEMASK) == ARG_INSLABEL) {
				tlab2 = ((ins_chain *)tmp->in_args[1]->regconst)->in_args[0]->regconst;
			} else {
				tlab2 = tmp->in_args[1]->regconst;
			}
			if (tmp->type == INS_LOADLABDIFF) {
				fprintf (outstream, "\tmovq\t$(" LBLPFX "%d - " LBLPFX "%d), %s\n", tlab1, tlab2, x64_regs[tmp->out_args[0]->regconst]);
			} else {
				fprintf (outstream, ".quad (" LBLPFX "%d - " LBLPFX "%d)\n", tlab1, tlab2);
			}
			break;
		case INS_CONSTLABADDR:
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_INSLABEL) {
				tlab1 = ((ins_chain *)tmp->in_args[0]->regconst)->in_args[0]->regconst;
			} else {
				tlab1 = tmp->in_args[0]->regconst;
			}
			fprintf (outstream, ".quad " LBLPFX "%d\n", tlab1);
			break;
		case INS_CJUMP:
			fprintf (outstream, "\tj%s\t", setcc_tailcodes[tmp->in_args[0]->regconst]);
			x64_drop_arg (tmp->in_args[1], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_CMOVE:
			fprintf (outstream, "\tcmov%s\t", setcc_tailcodes[tmp->in_args[0]->regconst]);
			x64_drop_arg (tmp->in_args[1], outstream);
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->out_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_SETCC:
			fprintf (outstream, "\tset%s\t", setcc_tailcodes[tmp->in_args[0]->regconst]);
			if ((tmp->out_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				fprintf (outstream, "%s", x64_regs8[tmp->out_args[0]->regconst]);
			} else {
				x64_drop_arg (tmp->out_args[0], outstream);
			}
			fprintf (outstream, "\n");
			break;
		case INS_ADD:
		case INS_SUB:
		case INS_OR:
		case INS_AND:
		case INS_XOR:
		case INS_ADC:
		case INS_SBB:
			{
				int in_mode = tmp->in_args[0]->flags & ARG_MODEMASK;
				int is_const = tmp->in_args[0]->flags & ARG_ISCONST;
				/* Handle label addresses as operands (PIC-incompatible otherwise) */
				if ((in_mode == ARG_LABEL || in_mode == ARG_FLABEL || in_mode == ARG_INSLABEL || in_mode == ARG_NAMEDLABEL) && is_const) {
					/* Load label address via RIP-relative into r11, then operate */
					if (in_mode == ARG_NAMEDLABEL) {
						fprintf (outstream, "\tleaq\t%s(%%rip), %%r11\n", x64_modify_name ((char *)(long)tmp->in_args[0]->regconst));
					} else if (in_mode == ARG_LABEL) {
						fprintf (outstream, "\tleaq\t" LBLPFX "%ld(%%rip), %%r11\n", (long)tmp->in_args[0]->regconst);
					} else if (in_mode == ARG_FLABEL) {
						fprintf (outstream, "\tleaq\t%ldf(%%rip), %%r11\n", (long)tmp->in_args[0]->regconst);
					} else {
						fprintf (outstream, "\tleaq\t" LBLPFX "%ld(%%rip), %%r11\n", (long)((ins_chain *)(long)tmp->in_args[0]->regconst)->in_args[0]->regconst);
					}
					fprintf (outstream, "\t%s\t%%r11, ", codes[tmp->type]);
					x64_drop_arg (tmp->out_args[0], outstream);
					fprintf (outstream, "\n");
					break;
				}
				/* Check if constant operand exceeds 32-bit signed range */
				if (in_mode == ARG_CONST && is_const) {
					long val = (long)tmp->in_args[0]->regconst;
					if (val > 0x7fffffffL || val < -0x80000000L) {
						/* Load large constant into r11 (scratch), then operate */
						fprintf (outstream, "\tmovabsq\t$%ld, %%r11\n", val);
						fprintf (outstream, "\t%s\t%%r11, ", codes[tmp->type]);
						x64_drop_arg (tmp->out_args[0], outstream);
						fprintf (outstream, "\n");
						break;
					}
				}
			}
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->out_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_CMP:
			/* Check if constant operand exceeds 32-bit signed range */
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST &&
			    (tmp->in_args[0]->flags & ARG_ISCONST)) {
				long val = (long)tmp->in_args[0]->regconst;
				if (val > 0x7fffffffL || val < -0x80000000L) {
					fprintf (outstream, "\tmovabsq\t$%ld, %%r11\n", val);
					fprintf (outstream, "\tcmpq\t%%r11, ");
					x64_drop_arg (tmp->in_args[1], outstream);
					fprintf (outstream, "\n");
					break;
				}
			}
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->in_args[1], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_MUL:
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			x64_drop_arg (tmp->in_args[0], outstream);
			if (tmp->in_args[1] && !(tmp->in_args[1]->flags & ARG_IMP)) {
				fprintf (outstream, ", ");
				x64_drop_arg (tmp->in_args[1], outstream);
				if (tmp->out_args[0] && !(tmp->out_args[0]->flags & ARG_IMP)) {
					if (tmp->in_args[1]->regconst != tmp->out_args[0]->regconst) {
						fprintf (outstream, ", ");
						x64_drop_arg (tmp->out_args[0], outstream);
					}
				}
			}
			fprintf (outstream, "\n");
			break;
		case INS_SHLD:
		case INS_SHRD:
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				fprintf (outstream, "%s", x64_regs8[tmp->in_args[0]->regconst]);
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->in_args[1], outstream);
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->in_args[2], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_SHR:
		case INS_SHL:
		case INS_SAR:
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				fprintf (outstream, "%s", x64_regs8[tmp->in_args[0]->regconst]);
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->in_args[1], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_INC:
		case INS_DEC:
		case INS_NOT:
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_DIV:
		case INS_UDIV:
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			x64_drop_arg (tmp->in_args[2], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_UMUL:
			fprintf (outstream, "\tmulq\t");
			x64_drop_arg (tmp->in_args[1], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_FSUB:
			fprintf (outstream, "\tfsubrp\t%%st, %%st(1)\n");
			break;
		case INS_MOVEB:
			fprintf (outstream, "\tmovb\t");
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				fprintf (outstream, "%s", x64_regs8[tmp->in_args[0]->regconst]);
				fprintf (outstream, ", ");
				x64_drop_arg (tmp->out_args[0], outstream);
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
				fprintf (outstream, ", ");
				if ((tmp->out_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
					fprintf (outstream, "%s", x64_regs8[tmp->out_args[0]->regconst]);
				} else {
					x64_drop_arg (tmp->out_args[0], outstream);
				}
			}
			fprintf (outstream, "\n");
			break;
		case INS_REPMOVEB:
			fprintf (outstream, "\trep\n\tmovsb\n");
			break;
		case INS_REPMOVEL:
			fprintf (outstream, "\trep\n\tmovsq\n");
			break;
		case INS_ANNO:
			fprintf (outstream, "# ");
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_SWAP:
			fprintf (outstream, "\txchgq\t");
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->in_args[1], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_FSTSW:
			fprintf (outstream, "\tfnstsw\t%%ax\n");
			break;
		case INS_JUMP:
		case INS_CALL:
		case INS_CCALL:
			fprintf (outstream, "\t%s\t", codes[tmp->type]);
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL &&
			    !(tmp->in_args[0]->flags & ARG_IND) &&
			    (tmp->type == INS_CALL || tmp->type == INS_CCALL)) {
				/* Use @PLT for PIC-compatible external function calls */
				fprintf (outstream, "%s@PLT", x64_modify_name ((char *)(long)tmp->in_args[0]->regconst));
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, "\n");
			break;
		case INS_PJUMP:
			fprintf (outstream, "# generating PJUMP code (x64)\n");
			fprintf (outstream, "\tleaq\tsf(%%rip), %%rbx\n");
			fprintf (outstream, "\tmovq\t0(%%rbx), %%rax\n");
			fprintf (outstream, "\torq\t8(%%rbx), %%rax\n");
			fprintf (outstream, "\tjz\t");
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, "\n");
			fprintf (outstream, "# reschedule code\n");
			fprintf (outstream, "\tleaq\t");
			x64_drop_arg (tmp->in_args[0], outstream);
			fprintf (outstream, "(%%rip), %%rax\n");
			fprintf (outstream, "\tmovq\t%%rax, %d(%%r14)\n", W_IPTR);
			/* Call kernel scheduler via calltable */
			fprintf (outstream, "\tcallq\t*%d(%%r15)\n", (int)offsetof(ccsp_sched_t, calltable[K_PAUSE]));
			break;
		case INS_LOCK:
			fprintf (outstream, "\tlock;");
			break;
		case INS_MB:
			fprintf (outstream, "\tmfence\n");
			break;
		case INS_RMB:
			fprintf (outstream, "\tlfence\n");
			break;
		case INS_WMB:
			fprintf (outstream, "\tsfence\n");
			break;
		case INS_MOVE32:
		case INS_TRUNCATE32:
			/* 32-bit move / truncate (for INT on 64-bit targets) - use 32-bit register names */
			fprintf (outstream, "\tmovl\t");
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				int r = (int)(long)tmp->in_args[0]->regconst;
				if (r < 0) r = x64_regcolour_special_to_real (r);
				fprintf (outstream, "%s", (r >= 0 && r < 16) ? x64_regs32[r] : "??");
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, ", ");
			if ((tmp->out_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				int r = (int)(long)tmp->out_args[0]->regconst;
				if (r < 0) r = x64_regcolour_special_to_real (r);
				fprintf (outstream, "%s", (r >= 0 && r < 16) ? x64_regs32[r] : "??");
			} else {
				x64_drop_arg (tmp->out_args[0], outstream);
			}
			fprintf (outstream, "\n");
			break;
		case INS_SIGNEXT32:
			fprintf (outstream, "\tmovslq\t");
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				int r = (int)(long)tmp->in_args[0]->regconst;
				if (r < 0) r = x64_regcolour_special_to_real (r);
				fprintf (outstream, "%s", (r >= 0 && r < 16) ? x64_regs32[r] : "??");
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->out_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_MOVESEXT16TO32:
			/* movswq: sign-extend 16-bit to 64-bit, source must be 16-bit reg */
			fprintf (outstream, "\tmovswq\t");
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				int r = (int)(long)tmp->in_args[0]->regconst;
				if (r < 0) r = x64_regcolour_special_to_real (r);
				fprintf (outstream, "%s", (r >= 0 && r < 16) ? x64_regs16[r] : "??");
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->out_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_MOVEZEXT16TO32:
			/* movzwq: zero-extend 16-bit to 64-bit */
			fprintf (outstream, "\tmovzwq\t");
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				int r = (int)(long)tmp->in_args[0]->regconst;
				if (r < 0) r = x64_regcolour_special_to_real (r);
				fprintf (outstream, "%s", (r >= 0 && r < 16) ? x64_regs16[r] : "??");
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->out_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		case INS_MOVEZEXT8TO32:
			/* movzbq: zero-extend 8-bit to 64-bit */
			fprintf (outstream, "\tmovzbq\t");
			if ((tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
				int r = (int)(long)tmp->in_args[0]->regconst;
				if (r < 0) r = x64_regcolour_special_to_real (r);
				fprintf (outstream, "%s", (r >= 0 && r < 16) ? x64_regs8[r] : "??");
			} else {
				x64_drop_arg (tmp->in_args[0], outstream);
			}
			fprintf (outstream, ", ");
			x64_drop_arg (tmp->out_args[0], outstream);
			fprintf (outstream, "\n");
			break;
		/* INS_MOVESEXT8TO32 doesn't exist in this codebase */
		case INS_MOVE:
			/* x64 MOV: handle address-type sources with RIP-relative addressing for PIC */
			if (tmp->in_args[0] && tmp->out_args[0]) {
				int in_mode = tmp->in_args[0]->flags & ARG_MODEMASK;
				int out_mode = tmp->out_args[0]->flags & ARG_MODEMASK;
				int is_const = tmp->in_args[0]->flags & ARG_ISCONST;
				if (in_mode == ARG_NAMEDLABEL && is_const && out_mode == ARG_REG) {
					/* Load symbol address: use @GOTPCREL for external, leaq for local */
					char *sym = x64_modify_name ((char *)(long)tmp->in_args[0]->regconst);
					/* Use movq GOT for external symbols (starting with _ or no O_/E_/M_ prefix) */
					if (sym[0] == '_' || (sym[0] != 'O' && sym[0] != 'E' && sym[0] != 'M')) {
						fprintf (outstream, "\tmovq\t%s@GOTPCREL(%%rip), ", sym);
					} else {
						fprintf (outstream, "\tleaq\t%s(%%rip), ", sym);
					}
					x64_drop_arg (tmp->out_args[0], outstream);
					fprintf (outstream, "\n");
				} else if (in_mode == ARG_NAMEDLABEL && !is_const) {
					/* Load from named label: movq symbol(%rip), dest */
					fprintf (outstream, "\tmovq\t%s(%%rip), ", x64_modify_name ((char *)(long)tmp->in_args[0]->regconst));
					x64_drop_arg (tmp->out_args[0], outstream);
					fprintf (outstream, "\n");
				} else if ((in_mode == ARG_LABEL || in_mode == ARG_FLABEL || in_mode == ARG_INSLABEL) && is_const) {
					/* Label address as constant: leaq .Lnn(%rip), %reg or store */
					/* Must emit label WITHOUT $ prefix for leaq */
					char lbuf[64];
					if (in_mode == ARG_LABEL) {
						sprintf (lbuf, LBLPFX "%ld", (long)tmp->in_args[0]->regconst);
					} else if (in_mode == ARG_FLABEL) {
						sprintf (lbuf, "%ldf", (long)tmp->in_args[0]->regconst);
					} else {
						sprintf (lbuf, LBLPFX "%ld", (long)((ins_chain *)(long)tmp->in_args[0]->regconst)->in_args[0]->regconst);
					}
					if (out_mode == ARG_REG) {
						fprintf (outstream, "\tleaq\t%s(%%rip), ", lbuf);
						x64_drop_arg (tmp->out_args[0], outstream);
						fprintf (outstream, "\n");
					} else {
						/* Storing label to memory: need temp register */
						fprintf (outstream, "\tleaq\t%s(%%rip), %%r11\n", lbuf);
						fprintf (outstream, "\tmovq\t%%r11, ");
						x64_drop_arg (tmp->out_args[0], outstream);
						fprintf (outstream, "\n");
					}
				} else {
					/* Default MOV handling */
					fprintf (outstream, "\tmovq\t");
					x64_drop_arg (tmp->in_args[0], outstream);
					fprintf (outstream, ", ");
					x64_drop_arg (tmp->out_args[0], outstream);
					fprintf (outstream, "\n");
				}
			}
			break;
		case INS_LEA:
			/* LEA with RIP-relative for named labels */
			if (tmp->in_args[0] && (tmp->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
				fprintf (outstream, "\tleaq\t%s(%%rip), ", x64_modify_name ((char *)(long)tmp->in_args[0]->regconst));
				x64_drop_arg (tmp->out_args[0], outstream);
				fprintf (outstream, "\n");
			} else {
				fprintf (outstream, "\tleaq\t");
				x64_drop_arg (tmp->in_args[0], outstream);
				fprintf (outstream, ", ");
				x64_drop_arg (tmp->out_args[0], outstream);
				fprintf (outstream, "\n");
			}
			break;
		default:
			/* Default handler for remaining instructions */
			if (tmp->type >= INS_FIRST && tmp->type <= INS_LAST) {
				if (tmp->type < (int)(sizeof(codes)/sizeof(codes[0])) && strcmp (codes[tmp->type], "..")) {
					fprintf (outstream, "\t%s\t", codes[tmp->type]);
				} else {
					fprintf (outstream, "\t..%d..\t", tmp->type);
				}
				for (i=0; tmp->in_args[i]; i++) {
					arg = tmp->in_args[i];
					if (arg->flags & ARG_IMP) continue;
					x64_drop_arg (arg, outstream);
					if (tmp->in_args[i+1] || tmp->out_args[0]) {
						fprintf (outstream, ", ");
					}
				}
				for (i=0; tmp->out_args[i]; i++) {
					arg = tmp->out_args[i];
					if (arg->flags & ARG_IMP) continue;
					x64_drop_arg (arg, outstream);
					if (tmp->out_args[i+1]) {
						fprintf (outstream, ", ");
					}
				}
				fprintf (outstream, "\n");
			}
			break;
		}
	}
	return 0;
}
/*}}}*/

/*{{{  static int x64_disassemble_data (unsigned char *bytes, int length, FILE *outstream) */
static int x64_disassemble_data (unsigned char *bytes, int length, FILE *outstream)
{
	int i;

	for (i=0; i<length; i++) {
		if (!(i % 8)) {
			if (i > 0) fprintf (outstream, "\n");
			fprintf (outstream, "\t.byte\t");
		}
		fprintf (outstream, "0x%2.2x%s", bytes[i], (((i % 8) == 7) || (i==(length-1))) ? " " : ", ");
	}
	fprintf (outstream, "\n");
	return 0;
}
/*}}}*/

/*{{{  static int x64_disassemble_xdata */
static int x64_disassemble_xdata (unsigned char *bytes, int length, tdfixup_t *fixups, FILE *outstream)
{
	tdfixup_t *walk;
	int i;

	for (i=0, walk=fixups; i<length;) {
		int count = 0;

		if (walk && (i == walk->offset)) {
			while (walk && (i == walk->offset)) {
				fprintf (outstream, "\n\t.quad\t" LBLPFX "%d", walk->otherlab);
				walk = walk->next;
				i += 8;
			}
			if (i >= length) continue;
		}
		fprintf (outstream, "\n\t.byte\t");
		while ((i < length) && (!walk || (i < walk->offset))) {
			if (count == 8) {
				fprintf (outstream, "\n\t.byte\t");
				count = 0;
			}
			fprintf (outstream, "0x%2.2x", bytes[i]);
			if (((count & 0x07) == 0x07) || (i == (length -1)) || (walk && (walk->offset == (i+1)))) {
				/* nothing */
			} else {
				fprintf (outstream, ", ");
			}
			i++;
			count++;
		}
		while ((i < length) && walk && (i > walk->offset)) {
			fprintf (stderr, "%s: badly ordered fixup for L%d:%d -> L%d (ignoring)\n", progname, walk->thislab, walk->offset, walk->otherlab);
			walk = walk->next;
		}
	}
	fprintf (outstream, "\n");
	return 0;
}
/*}}}*/

/*{{{  static void x64_dump_codemap_stream */
static void x64_dump_codemap_stream (procinf *piinf, FILE *stream, int *seen_data, int toplvl)
{
	char namebuf[MAXNAME];
	char *ntmp = NULL;
	int i;

	if (!*seen_data) {
		fprintf (stream, ".data\n");
		*seen_data = 1;
	}
	if (toplvl) {
		fprintf (stream, "\n.align 8\n");
		fprintf (stream, LBLPFX "%d:\n", piinf->maplab);
	}
	if (toplvl) {
		snprintf (namebuf, sizeof namebuf, "*%s", piinf->name);
		ntmp = x64_modify_name (namebuf);
		fprintf (stream, ".globl %s\n", ntmp);
		if (!options.disable_symbol_ops) {
			fprintf (stream, ".type %s, @object\n", ntmp);
		}
		fprintf (stream, "%s:\n", ntmp);
	}

	if (piinf->is_internal) {
		fprintf (stream, "\t.quad\t" LBLPFX "%d\n", piinf->eplab);
	} else {
		ntmp = x64_modify_name (piinf->name);
		fprintf (stream, "\t.quad\t%s\n", ntmp);
	}

	if (toplvl) {
		snprintf (namebuf, sizeof namebuf, "^%s", piinf->name);
		ntmp = x64_modify_name (namebuf);
		fprintf (stream, "\t.quad\t%s", ntmp);
		fprintf (stream, " - %s\n", x64_modify_name (piinf->name));
	} else if (piinf->is_internal) {
		fprintf (stream, "\t.quad\t-1\n");
	} else {
		snprintf (namebuf, sizeof namebuf, "*%s", piinf->name);
		ntmp = x64_modify_name (namebuf);
		fprintf (stream, "\t.quad\t%s\n", ntmp);
	}

	fprintf (stream, "\t.quad\t" LBLPFX "%d\n", piinf->namelab);

	if (toplvl) {
		fprintf (stream, "\t.long\t%d\n", piinf->refs_cur);
	} else {
		fprintf (stream, "\t.long\t0\n");
	}

	for (i=0; i<piinf->refs_cur; i++) {
		if (piinf->refs[i]->is_internal) {
			piinf->refs[i]->namelab = piinf->inamelab;
		}
		x64_dump_codemap_stream (piinf->refs[i], stream, seen_data, 0);
	}
}

static void x64_dump_codemap_namelabels_stream (procinf *piinf, FILE *stream)
{
	int i;

	if (!piinf->written_out) {
		if (piinf->namelen) {
			fprintf (stream, LBLPFX "%d:\t.asciz\t\"%*s\"\n", piinf->namelab, piinf->namelen, piinf->name);
		}
		if (piinf->inamelen) {
			fprintf (stream, LBLPFX "%d:\t.asciz\t\"%*s\"\n", piinf->inamelab, piinf->inamelen, piinf->iname);
		}
		piinf->written_out = 1;
	}
	for (i=0; i<piinf->refs_cur; i++) {
		x64_dump_codemap_namelabels_stream (piinf->refs[i], stream);
	}
}
/*}}}*/

/*{{{  int x64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream) */
static int x64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream)
{
	rtl_chain *tmp;
	int seen_data;
	int errored;

	seen_data = 0;
	errored = 0;
	for (tmp=rtl_code; tmp && !errored; tmp=tmp->next) {
		switch (tmp->type) {
		case RTL_SOURCEFILE:
			fprintf (stream, "\t# sourcefile \"%s\"\n", tmp->u.sourcefile);
			break;
		case RTL_CODE:
			errored = x64_disassemble_code (tmp->u.code.head, stream, (options.debug_options & DEBUG_MEMCHK));
			break;
		case RTL_DATA:
			errored = x64_disassemble_data ((unsigned char *)tmp->u.data.bytes, tmp->u.data.length, stream);
			break;
		case RTL_XDATA:
			if (tmp->u.xdata.label >= 0) {
				if (!seen_data) {
					fprintf (stream, ".data\n");
					seen_data = 1;
				}
				fprintf (stream, LBLPFX "%d:\n", tmp->u.xdata.label);
			}
			errored = x64_disassemble_xdata ((unsigned char *)tmp->u.xdata.bytes, tmp->u.xdata.length, tmp->u.xdata.fixups, stream);
			break;
		case RTL_RDATA:
			if (!seen_data) {
				fprintf (stream, ".data\n");
				seen_data = 1;
			}
			fprintf (stream, LBLPFX "%d:\n", tmp->u.rdata.label);
			errored = x64_disassemble_data ((unsigned char *)tmp->u.rdata.bytes, tmp->u.rdata.length, stream);
			break;
		case RTL_SETNAMEDLABEL:
			fprintf (stream, "%s:\n", x64_modify_name (tmp->u.label_name));
			break;
		case RTL_STUBIMPORT:
			fprintf (stream, "\tjmp\t%s\n", x64_modify_name (tmp->u.label_name));
			break;
		case RTL_PUBLICSETNAMEDLABEL:
			{
				const char *label = x64_modify_name (tmp->u.label_name);

				fprintf (stream, ".globl %s\n", label);
				if (!options.disable_symbol_ops) {
					if (strstr(label, "_occam_tlp_iface") || strstr(label, "_occam_errormode")) {
						fprintf (stream, ".type %s, @object\n", label);
					} else {
						fprintf (stream, ".type %s, @function\n", label);
					}
				}
				fprintf (stream, "%s:\n", label);
			}
			break;
		case RTL_DYNCODEENTRY:
			{
				const char *label = x64_modify_name (tmp->u.dyncode.label_name);
				const char *flabel;

				if (!seen_data) {
					fprintf (stream, ".data\n");
					seen_data = 1;
				}

				fprintf (stream, ".globl %s\n", label);
				fprintf (stream, ".align 8\n");
				fprintf (stream, "%s:\n", label);

				flabel = x64_modify_name (tmp->u.dyncode.fcn_name);
				fprintf (stream, "\t.quad %s\n\t.long %d, %d, 0x%x\n", flabel,
						tmp->u.dyncode.ws_slots, tmp->u.dyncode.vs_slots, tmp->u.dyncode.typehash);
			}
			break;
		case RTL_PUBLICENDNAMEDLABEL:
			if (!options.disable_symbol_ops) {
				const char *label = x64_modify_name (tmp->u.label_name);
				fprintf (stream, ".size %s, . - %s\n", label, label);
			}
			break;
		case RTL_ALIGN:
			fprintf (stream, "\n.align %d\n", (1 << tmp->u.alignment));
			break;
		case RTL_WSVS:
			if (!seen_data) {
				fprintf (stream, ".data\n");
				seen_data = 1;
			}
			{
				const char *pfx = options.extref_prefix ? options.extref_prefix : "";

				if (options.rmoxmode == RM_NONE) {
					/* Use .quad for 64-bit: these are 'word' (8 bytes) in C */
					fprintf (stream, ".globl %s_wsbytes\n", pfx);
					fprintf (stream, "%s_wsbytes: .quad %d\n", pfx, tmp->u.wsvs.ws_bytes);
					fprintf (stream, ".globl %s_wsadjust\n", pfx);
					fprintf (stream, "%s_wsadjust: .quad %d\n", pfx, tmp->u.wsvs.ws_adjust);
					fprintf (stream, ".globl %s_vsbytes\n", pfx);
					fprintf (stream, "%s_vsbytes: .quad %d\n", pfx, tmp->u.wsvs.vs_bytes);
					fprintf (stream, ".globl %s_msbytes\n", pfx);
					fprintf (stream, "%s_msbytes: .quad %d\n", pfx, tmp->u.wsvs.ms_bytes);
				}
			}
			break;
		case RTL_UNDEFINED:
		case RTL_CODELINE:
		case RTL_COMMENT:
		case RTL_MESSAGE:
			break;
		case RTL_CODEMAP:
			x64_dump_codemap_stream (tmp->u.codemap.pinf, stream, &seen_data, 1);
			x64_dump_codemap_namelabels_stream (tmp->u.codemap.pinf, stream);
			break;
		}
	}
#ifdef HOST_OS_IS_LINUX
	fprintf (stream, ".section .note.GNU-stack,\"\",%%progbits\n");
#endif
	return errored;
}
/*}}}*/

/*{{{  int x64_code_to_asm (rtl_chain *rtl_code, char *filename) */
static int x64_code_to_asm (rtl_chain *rtl_code, char *filename)
{
	FILE *outstream;
	int result;

	outstream = fopen (filename, "w");
	if (!outstream) {
		fprintf (stderr, "%s: error: unable to open %s for writing\n", progname, filename);
		return -1;
	}
	result = x64_code_to_asm_stream (rtl_code, outstream);
	fclose (outstream);
	return result;
}
/*}}}*/
/*}}}*/

/*{{{  arch_t structure and init */
static arch_t x64_arch = {
	.archname = "x64",
	.compose_kcall = compose_x64_kcall,
	.compose_kjump = compose_x64_kjump,
	.compose_deadlock_kcall = compose_x64_deadlock_kcall,
	.compose_pre_enbc = compose_pre_enbc_x64,
	.compose_pre_enbt = compose_pre_enbt_x64,
	.compose_inline_ldtimer = compose_x64_inline_ldtimer,
	.compose_inline_tin = compose_x64_inline_tin,
	.compose_inline_quick_reschedule = compose_x64_inline_quick_reschedule,
	.compose_inline_full_reschedule = compose_x64_inline_full_reschedule,
	.compose_inline_in = compose_x64_inline_in,
	.compose_inline_in_2 = compose_x64_inline_in_2,
	.compose_inline_min = compose_x64_inline_min,
	.compose_inline_out = compose_x64_inline_out,
	.compose_inline_out_2 = compose_x64_inline_out_2,
	.compose_inline_mout = compose_x64_inline_mout,
	.compose_inline_enbc = compose_x64_inline_enbc,
	.compose_inline_disc = compose_x64_inline_disc,
	.compose_inline_altwt = compose_x64_inline_altwt,
	.compose_inline_stlx = compose_x64_inline_stlx,
	.compose_inline_malloc = compose_x64_inline_malloc,
	.compose_inline_startp = compose_x64_inline_startp,
	.compose_inline_endp = compose_x64_inline_endp,
	.compose_inline_runp = compose_x64_inline_runp,
	.compose_inline_stopp = compose_x64_inline_stopp,
	.compose_debug_insert = compose_debug_insert_x64,
	.compose_debug_procnames = compose_debug_procnames_x64,
	.compose_debug_filenames = compose_debug_filenames_x64,
	.compose_debug_zero_div = compose_debug_zero_div_x64,
	.compose_debug_floaterr = compose_debug_floaterr_x64,
	.compose_debug_overflow = compose_debug_overflow_x64,
	.compose_debug_rangestop = compose_debug_rangestop_x64,
	.compose_debug_seterr = compose_debug_seterr_x64,
	.compose_overflow_jumpcode = compose_overflow_jumpcode_x64,
	.compose_floaterr_jumpcode = compose_floaterr_jumpcode_x64,
	.compose_rangestop_jumpcode = compose_rangestop_jumpcode_x64,
	.compose_debug_deadlock_set = compose_debug_deadlock_set_x64,
	.compose_divcheck_zero = compose_divcheck_zero_x64,
	.compose_divcheck_zero_simple = compose_divcheck_zero_simple_x64,
	.compose_division = compose_x64_division,
	.compose_remainder = compose_x64_remainder,
	.compose_iospace_loadbyte = compose_iospace_loadbyte_x64,
	.compose_iospace_storebyte = compose_iospace_storebyte_x64,
	.compose_iospace_loadword = compose_iospace_loadword_x64,
	.compose_iospace_storeword = compose_iospace_storeword_x64,
	.compose_iospace_read = compose_iospace_read_x64,
	.compose_iospace_write = compose_iospace_write_x64,
	.compose_move_loadptrs = compose_x64_move_loadptrs,
	.compose_move = compose_x64_move,
	.compose_shift = compose_x64_shift,
	.compose_widenshort = compose_x64_widenshort,
	.compose_widenword = compose_x64_widenword,
	.compose_longop = compose_x64_longop,
	.compose_fpop = compose_x64_fpop,
	.compose_external_ccall = compose_external_ccall_x64,
	.compose_bcall = compose_x64_bcall,
	.compose_cif_call = compose_x64_cif_call,
	.compose_entry_prolog = compose_x64_entry_prolog,
	.compose_rmox_entry_prolog = x64_stub_rmox_entry_prolog,
	.compose_fp_set_fround = compose_fp_set_fround_x64,
	.compose_fp_init = compose_fp_init_x64,
	.compose_reset_fregs = compose_reset_fregs_x64,
	.compose_refcountop = compose_refcountop_x64,
	.compose_memory_barrier = compose_memory_barrier_x64,
	.compose_return = compose_x64_return,
	.compose_nreturn = compose_x64_nreturn,
	.compose_funcresults = compose_x64_funcresults,
	.regcolour_special_to_real = x64_regcolour_special_to_real,
	.regcolour_rmax = RMAX_X64,
	.regcolour_nodemax = NODEMAX_X64,
	.regcolour_get_regs = x64_regcolour_get_regs,
	.regcolour_fp_regs = x64_regcolour_fp_regs,
	.code_to_asm = x64_code_to_asm,
	.code_to_asm_stream = x64_code_to_asm_stream,
	.rtl_validate_instr = x64_rtl_validate_instr,
	.rtl_prevalidate_instr = x64_rtl_prevalidate_instr,
	.get_register_name = x64_get_register_name,
	.int_options = 0,
	.kiface_tableoffs = 0
};

arch_t *init_arch_x64 (int mclass)
{
	return &x64_arch;
}
/*}}}*/
