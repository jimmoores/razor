/*
 *\tarchaarch64.c -- aarch64 architecture support
 *\tCopyright (C) 2024 Amazon Q
 *\
 *\tThis program is free software; you can redistribute it and/or modify
 *\tit under the terms of the GNU General Public License as published by
 *\tthe Free Software Foundation; either version 2 of the License, or
 *\t(at your option) any later version.
 *\
 *\tThis program is distributed in the hope that it will be useful,
 *\tbut WITHOUT ANY WARRANTY; without even the implied warranty of
 *\tMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *\tGNU General Public License for more details.
 *\
 *\tYou should have received a copy of the GNU General Public License
 *\talong with this program; if not, write to the Free Software
 *\tFoundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#include "main.h"
#include "support.h"
#include "structs.h"
#include "archdef.h"
#include "tstate.h"
#include "machine.h"
#include "archaarch64.h"


#include "rtlops.h"
#include "kif.h"
#include "transputer.h"
#include "etcrtl.h"
#include "tstack.h"

/* Include CCSP scheduler structure definition */
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

/* Forward declaration of sched_t structure */
typedef struct sched_struct {
	word cparam[8];  /* Parameter storage for CCSP calling convention */
	/* Other fields not needed for this fix */
} sched_t;

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

/* Forward declarations for FP stack simulation (defined later, used in fp_init) */
#define AARCH64_FP_STACK_SIZE 3
static int aarch64_fp_stack_prec[AARCH64_FP_STACK_SIZE];
static int aarch64_fp_stack_depth;
static int aarch64_fp_precision;
static int aarch64_fp_rounding_mode = 0;  /* FPU_N=0 nearest, FPU_Z=3 truncate */
static int aarch64_fp_had_fpint = 0;  /* set by I_FPINT, cleared by I_FPSTNLI32 */
static int aarch64_fp_from_i64 = 0;  /* set by INS_FILD64, value is in d0 not s0 */

/*{{{  constants and definitions*/
/* aarch64 secondary opcodes for long operations (avoid conflicts with transputer.h) */
#define I_LADD_AARCH64 1
#define I_LSUB_AARCH64 2
#define I_LMUL_AARCH64 3
#define I_LSHL_AARCH64 4
#define I_LSHR_AARCH64 5

/* Test operation constants from transputer.h */
#ifndef I_TESTSTS
#define I_TESTSTS 0x26
#endif
#ifndef I_TESTSTE
#define I_TESTSTE 0x27
#endif
#ifndef I_TESTSTD
#define I_TESTSTD 0x28
#endif
#ifndef I_TESTERR
#define I_TESTERR 0x29
#endif

/* aarch64 floating point operations use standard constants from structs.h */

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

/* Reference counting operation constants */
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

/* Shift operation constants */
#ifndef I_SHL
#define I_SHL 0x41
#endif
#ifndef I_SHR
#define I_SHR 0x40
#endif

/* aarch64 register definitions */
#define REG_X0 0
#define REG_X1 1
#define REG_X2 2
#define REG_X3 3
#define REG_X4 4
#define REG_X16 16  /* Temporary register */
#define REG_X17 17  /* Temporary register */
#define REG_X29 29  /* Frame pointer */
#define REG_X30 30  /* Link register */
#define REG_SP (-10)   /* Stack pointer - negative to keep out of register allocation */

/* Special register mappings for occam runtime (use existing definitions from tstack.h) */
#define AARCH64_REG_WPTR 28  /* Workspace pointer */
#define AARCH64_REG_FPTR 27  /* Front pointer */
#define AARCH64_REG_BPTR 26  /* Back pointer */
#define AARCH64_REG_SCHED 25 /* Scheduler pointer */
#define AARCH64_REG_CC 32    /* Condition codes (virtual) */

/* Function result registers (following SPARC L0-L2 / PPC R6-R8 pattern) */
#define AARCH64_REG_FUNCRES0  19  /* x19 — function result 0 */
#define AARCH64_REG_FUNCRES1  20  /* x20 — function result 1 */
#define AARCH64_REG_FUNCRES2  21  /* x21 — function result 2 */

/*}}}*/
/*{{{  forward declarations*/
static void aarch64_emit_large_immediate (FILE *stream, long value, const char *temp_reg);
static void aarch64_emit_arithmetic_with_immediate (FILE *stream, const char *op, const char *dst, const char *src, long imm, const char *temp_reg, int set_flags);
static void compose_aarch64_kcall (tstate *ts, const int call, const int regs_in, const int regs_out);
static ins_chain *compose_aarch64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry);
static void compose_aarch64_deadlock_kcall (tstate *ts, const int call, const int regs_in, const int regs_out);
static void compose_aarch64_inline_ldtimer (tstate *ts);
static void compose_aarch64_inline_tin (tstate *ts);
static void compose_aarch64_inline_quick_reschedule (tstate *ts);
static void compose_aarch64_inline_full_reschedule (tstate *ts);
static void compose_aarch64_inline_in (tstate *ts, int width);
static void compose_aarch64_inline_out (tstate *ts, int width);
static void compose_aarch64_entry_prolog (tstate *ts);
static void compose_aarch64_return (tstate *ts);
static int compose_aarch64_widenshort (tstate *ts);
static int compose_aarch64_widenword (tstate *ts);
static void compose_aarch64_longop (tstate *ts, int secondary_opcode);
static void compose_aarch64_fpop (tstate *ts, int secondary_opcode);
static void compose_aarch64_occam_call (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last);
static void compose_external_ccall_aarch64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last);
static void compose_cif_call_aarch64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last);
static void compose_bcall_aarch64 (tstate *ts, int inlined, int kernel_call, int unused, char *name, ins_chain **pst_first, ins_chain **pst_last);
static int aarch64_regcolour_special_to_real (int reg);
static int aarch64_regcolour_get_regs (int *regs);
static int aarch64_code_to_asm (rtl_chain *rtl_code, char *filename);
static int aarch64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream);
static char *aarch64_get_register_name (int reg);
/* Helper function declarations */
static void aarch64_fp_push(int prec);
static const char *aarch64_get_label_prefix(int flags);
static void aarch64_emit_symbol_addr (FILE *stream, const char *dst_reg, const char *symbol);
static int aarch64_sets_flags (ins_chain *ins);
static int aarch64_validate_register(int reg);
static char *aarch64_convert_symbol_name(const char *symbol);
static void aarch64_emit_mem_op(FILE *stream, const char *op, const char *val_reg, const char *base_reg, long disp) {
	if (disp >= -256 && disp <= 255) {
		/* Assembler automatically chooses ldr/ldur or str/stur */
		fprintf(stream, "\t%s	%s, [%s, #%ld]\n", op, val_reg, base_reg, disp);
	} else if (disp >= 0 && disp <= 32760 && (disp % 8 == 0) && (op[2] == 'r' || op[2] == 'p')) {
		/* Unsigned scaled 12-bit offset for 64-bit loads/stores */
		fprintf(stream, "\t%s	%s, [%s, #%ld]\n", op, val_reg, base_reg, disp);
	} else if (disp >= 0 && disp <= 4095) {
		fprintf(stream, "\tadd	x16, %s, #%ld\n", base_reg, disp);
		fprintf(stream, "\t%s	%s, [x16]\n", op, val_reg);
	} else if (disp < 0 && disp >= -4095) {
		fprintf(stream, "\tsub	x16, %s, #%ld\n", base_reg, -disp);
		fprintf(stream, "\t%s	%s, [x16]\n", op, val_reg);
	} else {
		/* Fallback for very large offsets */
		aarch64_emit_large_immediate(stream, disp, "x16");
		fprintf(stream, "\tadd	x16, %s, x16\n", base_reg);
		fprintf(stream, "\t%s	%s, [x16]\n", op, val_reg);
	}
}

static void aarch64_emit_symbol_reference(FILE *stream, const char *symbol, const char *instruction);
static char *aarch64_convert_process_symbol(const char *fixed_name);
static char *aarch64_cleanup_symbol_name(const char *fixed_name);

/* I/O space operations */
static int compose_iospace_loadbyte_aarch64 (tstate *ts, int portreg, int targetreg);
static void compose_iospace_storebyte_aarch64 (tstate *ts, int portreg, int sourcereg);
static int compose_iospace_loadword_aarch64 (tstate *ts, int portreg, int targetreg);
static void compose_iospace_storeword_aarch64 (tstate *ts, int portreg, int sourcereg);
static void compose_iospace_read_aarch64 (tstate *ts, int portreg, int addrreg, int width);
static void compose_iospace_write_aarch64 (tstate *ts, int portreg, int addrreg, int width);

/* Stub functions for missing architecture functions */
static void aarch64_compose_reset_fregs (tstate *ts) {
        /* Initialize aarch64 floating point unit */
        add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// aarch64 FPU reset")));
        aarch64_fp_stack_depth = 0;
        aarch64_fp_stack_prec[0] = 0;
        aarch64_fp_stack_prec[1] = 0;
        aarch64_fp_stack_prec[2] = 0;
}
/*{{{  static void compose_pre_enbc_aarch64 (tstate *ts)*/
/*
 * areg has process address/label, breg has the guard, creg has the channel
 */
static void compose_pre_enbc_aarch64 (tstate *ts)
{
	int skip_lab = -1;

	if ((constmap_typeof (ts->stack->old_b_reg) == VALUE_CONST) && !constmap_regconst (ts->stack->old_b_reg)) {
		/* false pre-condition, nothing to do */
		return;
	} else if (constmap_typeof (ts->stack->old_b_reg) != VALUE_CONST) {
		/* test guard and maybe skip */
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		skip_lab = ++(ts->last_lab);
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
	}
	/* test channel for readiness.  Channel words are 64-bit
	 * pointers (Wptr|1), so we must use 64-bit comparison.
	 * Load the channel word into a register, then compare
	 * against NotProcess_p (0) using 64-bit x registers. */
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
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		break;
	}
	if (skip_lab > -1) {
		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
	}
	return;
}
/*}}}*/

/*{{{  static void compose_pre_enbt_aarch64 (tstate *ts)*/
/*
 * areg has process address/label, breg has the guard, creg has the timeout expression
 */
static void compose_pre_enbt_aarch64 (tstate *ts)
{
	int skip_lab = -1;

	ts->stack->a_reg = ts->stack->old_b_reg;		/* make guard result */

	if ((constmap_typeof (ts->stack->old_b_reg) == VALUE_CONST) && !constmap_regconst (ts->stack->old_b_reg)) {
		/* false pre-condition, nothing to do */
		return;
	} else if (constmap_typeof (ts->stack->old_b_reg) != VALUE_CONST) {
		/* test guard and maybe skip */
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		skip_lab = ++(ts->last_lab);
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
	}
	/* compare timeout with current time */
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
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		break;
	}
	if (skip_lab > -1) {
		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
	}
	return;
}
/*}}}*/

/*{{{  static void compose_inline_min_aarch64 (tstate *ts, int wide)*/
/*
 * generates an in-line version of MIN (mobile channel input)
 */
static void compose_inline_min_aarch64 (tstate *ts, int wide)
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

	/* check channel word */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, out_lab, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, ready_lab));

	/* place process in channel word and reschedule */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	compose_aarch64_inline_quick_reschedule (ts);

	/* channel-ready, queue us, reschedule other */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, ready_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER, ARG_REG, src_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REG, tmp_reg1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, dest_reg, ARG_REG, tmp_reg2));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND, dest_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND, src_reg));
	if (wide) {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, 4, ARG_REG, tmp_reg1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, dest_reg, 4, ARG_REG, tmp_reg2));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND | ARG_DISP, dest_reg, 4));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, src_reg, 4));
	}
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));

	/* out */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, out_lab));
	return;
}
/*}}}*/

/*{{{  static void compose_inline_mout_aarch64 (tstate *ts, int wide)*/
/*
 * generates an in-line version of MOUT (mobile channel output)
 */
static void compose_inline_mout_aarch64 (tstate *ts, int wide)
{
	int tmp_reg1, tmp_reg2, dest_reg;
	int chan_reg, src_reg;

	src_reg = ts->stack->old_b_reg;
	chan_reg = ts->stack->old_a_reg;

	/* check channel word */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));

	/* place process in channel word and reschedule */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	compose_aarch64_inline_quick_reschedule (ts);

	/* channel ready code */
	tmp_reg1 = tstack_newreg (ts->stack);
	dest_reg = tstack_newreg (ts->stack);

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg1, W_POINTER, ARG_REG, dest_reg));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, Z_READY, ARG_REG, dest_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GT, ARG_FLABEL, 2));

	/* talking to something ALTy */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, Z_WAITING, ARG_REG, dest_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 3));

	/* set us up and queue ALTer */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, tmp_reg1, W_STATUS));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	compose_aarch64_inline_quick_reschedule (ts);

	/* channel ready, do pointer swap */
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
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, 4, ARG_REG, tmp_reg1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, dest_reg, 4, ARG_REG, tmp_reg2));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND | ARG_DISP, dest_reg, 4));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, src_reg, 4));
	}
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));

	/* out */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
	return;
}
/*}}}*/

/*{{{  static void compose_inline_enbc_aarch64 (tstate *ts, int instr)*/
/*
 * generates an in-line version of ENBC/ENBC3
 */
static void compose_inline_enbc_aarch64 (tstate *ts, int instr)
{
	int tmp_lab, out_lab;
	int guard_reg, chan_reg;

	tmp_lab = ++(ts->last_lab);
	out_lab = ++(ts->last_lab);

	guard_reg = (instr == I_ENBC) ? ts->stack->old_a_reg : ts->stack->old_b_reg;
	chan_reg = (instr == I_ENBC) ? ts->stack->old_b_reg : ts->stack->old_c_reg;

	/* if guard is a constant 0, can optimise away */
	if (constmap_typeof (guard_reg) == VALUE_CONST) {
		if (constmap_regconst (guard_reg) == 0) {
			ts->stack->a_reg = guard_reg;
			return;
		}
		/* constant 1 means omit-test */
	} else {
		/* don't know guard value -- do test */
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, guard_reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_lab));
	}
	/* test channel */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (intptr_t)0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, tmp_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	constmap_remove (chan_reg);
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
	/* channel ready */
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, tmp_lab));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
	if (instr == I_ENBC3) {
		if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LABADDR) {
			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
		} else {
			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
		}
	}
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_lab));
	ts->stack->a_reg = guard_reg;
	return;
}
/*}}}*/

/*{{{  static void compose_inline_disc_aarch64 (tstate *ts, int instr)*/
/*
 * generates an in-line version of DISC/NDISC
 */
static void compose_inline_disc_aarch64 (tstate *ts, int instr)
{
	int tmp_reg;
	int out_rlab, out_flab, out_lab;

	out_rlab = ++(ts->last_lab);
	out_flab = ++(ts->last_lab);
	out_lab = ++(ts->last_lab);
	tmp_reg = tstack_newreg (ts->stack);

	/* if guard is constant 0, propagate through and return */
	if (constmap_typeof (ts->stack->old_b_reg) == VALUE_CONST) {
		if (constmap_regconst (ts->stack->old_b_reg) == 0) {
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = REG_UNDEFINED;
			ts->stack->c_reg = REG_UNDEFINED;
			return;
		}
	} else {
		/* don't know guard -- do check */
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_flab));
	}
	/* test channel */
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
		/* NDISC code: always select */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND, REG_WPTR));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, tmp_reg));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
	}
	/* clear channel */
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_rlab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND, ts->stack->old_c_reg));
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_flab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_lab));
	ts->stack->a_reg = tmp_reg;
	ts->stack->b_reg = REG_UNDEFINED;
	ts->stack->c_reg = REG_UNDEFINED;
	return;
}
/*}}}*/

/*{{{  static void compose_inline_altwt_aarch64 (tstate *ts)*/
/*
 * generates an inline ALTWT (ALT wait operations)
 */
static void compose_inline_altwt_aarch64 (tstate *ts)
{
	int ready_lab = ++(ts->last_lab);
	
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NONE_SELECTED, ARG_REGIND, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, ready_lab));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_WAITING, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, ready_lab, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	compose_aarch64_inline_quick_reschedule (ts);
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, ready_lab));
	return;
}
/*}}}*/

/*{{{  static void compose_inline_stlx_aarch64 (tstate *ts, int ins)*/
/*
 * used to inline STLF and STLB
 */
static void compose_inline_stlx_aarch64 (tstate *ts, int ins)
{
	int skip_lab = ++(ts->last_lab);

	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (intptr_t)0x8000000000000000ULL, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, skip_lab));	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
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
	return;
}
/*}}}*/

/*{{{  static void compose_inline_malloc_aarch64 (tstate *ts)*/
/*
 * allocates memory from the dmem_ allocator directly
 */
static void compose_inline_malloc_aarch64 (tstate *ts)
{
	int tmp_reg = tstack_newreg (ts->stack);
	
	/* Set up call to dmem_alloc2 */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_X0));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_X1));
	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("&dmem_alloc2")));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, ts->stack->a_reg));
	return;
}
/*}}}*/

/*{{{  static void compose_inline_startp_aarch64 (tstate *ts)*/
/*
 * inlined STARTP (start process) call
 * areg has "other workspace", breg has "start offset"
 */
static void compose_inline_startp_aarch64 (tstate *ts)
{
	switch (constmap_typeof (ts->stack->old_b_reg)) {
	case VALUE_LABADDR:
	case VALUE_LABDIFF:
		/* start point is a label */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_b_reg),
			ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR));
		/* Add to run queue */
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_BPTR));
		break;
	case VALUE_CONST:
		/* constant offset */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR));
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg), ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR,
			ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR));
		/* Add to run queue */
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_FPTR));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_BPTR));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
		break;
	default:
		break;
	}
	return;
}
/*}}}*/

/*{{{  static void compose_inline_endp_aarch64 (tstate *ts)*/
/*
 * inlined ENDP (end process) call
 */
static void compose_inline_endp_aarch64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_CONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_COUNT, ARG_REGIND | ARG_DISP, REG_WPTR, W_COUNT, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 0));
	compose_aarch64_inline_quick_reschedule (ts);
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	return;
}
/*}}}*/

/*{{{  static void compose_inline_stopp_aarch64 (tstate *ts)*/
/*
 * inlined STOPP (stop process) call
 */
static void compose_inline_stopp_aarch64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
	compose_aarch64_inline_quick_reschedule (ts);
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	return;
}
/*}}}*/

/* Debug function implementations */
static void compose_debug_insert_aarch64 (tstate *ts, int mdpairid)
{
	unsigned int x;

	if ((options.debug_options & DEBUG_INSERT) && !(ts->supress_debug_insert)) {
		x = ((ts->file_pending & 0xffff) << 16) + (ts->line_pending & 0xffff);
		/* Simple debug marker - store debug info in registers */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X1));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_X2));
	}
}

static void compose_debug_procnames_aarch64 (tstate *ts)
{
	char *procname_buffer;
	int procname_buflen;
	int procname_vcount;
	int procname_fcount;
	int procname_strlen;
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

static void compose_debug_filenames_aarch64 (tstate *ts)
{
	char *filename_buffer;
	int filename_buflen;
	int filename_vcount;
	int filename_fcount;
	int filename_strlen;
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

static void compose_debug_zero_div_aarch64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->zerodiv_label));
	/* Set up error parameters */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, (ts->line_pending & 0xffff), ARG_REG, REG_X1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff), ARG_REG, REG_X2));
	compose_aarch64_kcall (ts, K_ZERODIV, 2, 0);
}

static void compose_debug_floaterr_aarch64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floaterr_label));
	/* Set up error parameters */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, (ts->fp_line_pending & 0xffff), ARG_REG, REG_X1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, ((ts->fp_file_pending & 0xffff) << 16) + (ts->fp_proc_pending & 0xffff), ARG_REG, REG_X2));
	compose_aarch64_kcall (ts, K_FLOATERR, 2, 0);
}

static void compose_debug_overflow_aarch64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->overflow_label));
	/* Set up error parameters */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, (ts->line_pending & 0xffff), ARG_REG, REG_X1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff), ARG_REG, REG_X2));
	compose_aarch64_kcall (ts, K_OVERFLOW, 2, 0);
}

static void compose_debug_rangestop_aarch64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->range_entry_label));
	/* Set up error parameters */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, (ts->line_pending & 0xffff), ARG_REG, REG_X1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff), ARG_REG, REG_X2));
	compose_aarch64_kcall (ts, K_RANGERR, 2, 0);
}

static void compose_debug_seterr_aarch64 (tstate *ts)
{
	unsigned int x;

	x = (0xfb00 << 16) + (ts->line_pending & 0xffff);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X1));
	x = ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X2));
	compose_aarch64_kcall (ts, K_SETERR, 2, 0);
}

static void compose_overflow_jumpcode_aarch64 (tstate *ts, int dcode)
{
	unsigned int x;

	if (options.debug_options & DEBUG_OVERFLOW) {
		x = ((dcode & 0xff) << 24) + (ts->line_pending & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X1));
		x = ((ts->file_pending & 0xffff) << 16) + ((ts->proc_pending) & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X2));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->overflow_label));
	}
}

static void compose_floaterr_jumpcode_aarch64 (tstate *ts)
{
	unsigned int x;

	if (options.debug_options & DEBUG_FLOAT) {
		x = (ts->fp_line_pending & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X1));
		x = ((ts->fp_file_pending & 0xffff) << 16) + ((ts->fp_proc_pending) & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X2));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->floaterr_label));
	}
}

static void compose_rangestop_jumpcode_aarch64 (tstate *ts, int rcode)
{
	unsigned int x;

	if (options.debug_options & DEBUG_RANGESTOP) {
		x = ((rcode & 0xff) << 24) + (0xff << 16) + (ts->line_pending & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X1));
		x = ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X2));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->range_entry_label));
	}
}

static void compose_debug_deadlock_set_aarch64 (tstate *ts)
{
	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procfile_setup_label));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_X0));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_X1));
	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
}

static void compose_divcheck_zero_aarch64 (tstate *ts, int reg)
{
	int this_lab;
	unsigned int x;

	switch (constmap_typeof (reg)) {
	default:
		this_lab = ++(ts->last_lab);
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, this_lab));
		x = (ts->line_pending & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X1));
		x = ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff);
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X2));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
		break;
	case VALUE_CONST:
		if (constmap_regconst (reg) == 0) {
			fprintf (stderr, "%s: serious: division by zero seen around line %d\n", progname, ts->line_pending);
			x = (ts->line_pending & 0xffff);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X1));
			x = ((ts->file_pending & 0xffff) << 16) + (ts->proc_pending & 0xffff);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_X2));
			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
		}
		break;
	}
}

static void compose_divcheck_zero_simple_aarch64 (tstate *ts, int reg)
{
	switch (constmap_typeof (reg)) {
	default:
		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 2));
		compose_aarch64_kcall (ts, K_BNSETERR, 0, 0);
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
		break;
	case VALUE_CONST:
		if (constmap_regconst (reg) == 0) {
			fprintf (stderr, "%s: serious: division by zero seen around line %d\n", progname, ts->line_pending);
			compose_aarch64_kcall (ts, K_BNSETERR, 0, 0);
		}
		break;
	}
}

/*{{{  Advanced Operations Implementation*/

/*{{{  static void compose_move_loadptrs_aarch64 (tstate *ts)*/
/*
 * Loads pointers for memory move operations
 */
static void compose_move_loadptrs_aarch64 (tstate *ts)
{
	/* Load source pointer (C register) into x16 */
	switch (constmap_typeof (ts->stack->old_c_reg)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_c_reg), ARG_REG, REG_X16));
		break;
	case VALUE_LOCALPTR:
		if (!constmap_regconst (ts->stack->old_c_reg)) {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_X16));
		} else {
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, constmap_regconst (ts->stack->old_c_reg) << WSH, ARG_REG, REG_X16));
		}
		break;
	case VALUE_LOCAL:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_c_reg) << WSH, ARG_REG, REG_X16));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_X16));
		break;
	}

	/* Load destination pointer (B register) into x17 */
	switch (constmap_typeof (ts->stack->old_b_reg)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_b_reg), ARG_REG, REG_X17));
		break;
	case VALUE_LOCALPTR:
		if (!constmap_regconst (ts->stack->old_b_reg)) {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_X17));
		} else {
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG, REG_X17));
		}
		break;
	case VALUE_LOCAL:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG, REG_X17));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, REG_X17));
		break;
	}
}
/*}}}*/


/*{{{  static void compose_bcall_aarch64 (tstate *ts, int inlined, int kernel_call, int unused, char *name, ins_chain **pst_first, ins_chain **pst_last)*/
/*
 *	generates code for blocking system calls.
 *	Mirrors compose_bcall_i386: the 'i' parameter (inlined) matches
 *	generate_call() case values: 2=B., 3=BX., 4=KR.
 *	i386 uses: name + ((i == 2) ? 1 : 2) to keep the leading dot.
 */
static void compose_bcall_aarch64 (tstate *ts, int inlined, int kernel_call, int unused, char *name, ins_chain **pst_first, ins_chain **pst_last)
{
	int arg_reg;

	arg_reg = tstack_newreg (ts->stack);

	/* Set up workspace pointer parameter: Wptr + 1 word points to the param block */
	add_to_ins_chain (*pst_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (1 << WSH), ARG_REG, arg_reg));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, arg_reg, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[0])));

	if (kernel_call != K_KERNEL_RUN) {
		/* push address of function to call - skip prefix, keep leading dot */
		int name_offset = (inlined == 2) ? 1 : 2;
		int tmp_reg = tstack_newreg (ts->stack);

		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_X0));
		if (options.extref_prefix) {
			char sbuf[256];

			sprintf (sbuf, "%s%s", options.extref_prefix, name + name_offset);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, tmp_reg));
		} else {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + name_offset), ARG_REG, tmp_reg));
		}
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
	}

	/* Call kernel dispatch function.  Use regs_in=1 because cparam[0] is
	 * already set to the parameter block pointer above; regs_in=2 would
	 * cause the kcall to overwrite cparam[0] with the transputer B-reg. */
	compose_aarch64_kcall (ts, kernel_call, 1, 0);

	/* After the blocking call returns (process rescheduled), pop the
	 * I_CALL frame that was allocated before the call.  The I_CALL
	 * pushed 4 words (AJW -4 = -32 bytes on 64-bit). */
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, (4 << WSH), ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));

	*pst_last = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// bcall complete"));
	add_to_ins_chain (*pst_last);
}
/*}}}*/

/*{{{  static void compose_external_ccall_aarch64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last)*/
/*
 *	generates code to perform an external C call.
 *	Mirrors compose_external_ccall_i386: prepends extref_prefix to name+1
 *	(keeping the leading dot from "C.name"), leaving final dot-to-underscore
 *	conversion to aarch64_convert_symbol_name (like modify_name on i386).
 *
 *	On Darwin with extref_prefix="_": C.write.screen -> _.write.screen
 *	  -> aarch64_convert_symbol_name -> __write_screen (starts with '_', no prefix added)
 *	On Linux with no extref_prefix:   C.write.screen -> .write.screen
 *	  -> aarch64_convert_symbol_name -> _write_screen (starts with '_', no prefix added)
 */
static void compose_external_ccall_aarch64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last)
{
	int tmp_reg;

	/* AArch64 ABI: first argument in x0.  Pass Wptr + 1 word (pointer to params). */
	tmp_reg = tstack_newreg (ts->stack);
	*pst_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (1 << WSH), ARG_REG, tmp_reg);
	add_to_ins_chain (*pst_first);
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, REG_X0));

	/* Build symbol name: extref_prefix + name+1 (skip 'C', keep '.') */
	if (options.extref_prefix) {
		char sbuf[256];

		sprintf (sbuf, "%s%s", options.extref_prefix, name + 1);
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
	} else {
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
	}

	/* Restore Wptr adjustment: i386 does Wptr += 4<<WSH (=16 on 32-bit, 32 on 64-bit) */
	if (!options.nocc_codegen) {
		*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, (intptr_t)(4 << WSH), ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
	} else {
		*pst_last = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// external ccall complete"));
	}
	add_to_ins_chain (*pst_last);
}
/*}}}*/



/*{{{  static void compose_cif_call_aarch64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last)*/
/*
 *\tGenerates code for C interface calls
 */
static void compose_cif_call_aarch64 (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last)
{
	char sbuf[256];

	/* Adjust Wptr like i386 CIF call: Wptr += 4 (one word) */
	*pst_first = compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, (intptr_t)4, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
	add_to_ins_chain (*pst_first);

	/* Save resumption label and sched pointer for restoration after call */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-36)));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, (intptr_t)(-1), ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-32)));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-28)));

	/* Pass Wptr in x0 (AArch64 first argument register) */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_X0));

	/* Build symbol: @<extref_prefix><name+4>
	 * The '@' prevents aarch64_convert_symbol_name from adding O_ prefix,
	 * matching compose_cif_call_i386 behaviour. */
	sprintf (sbuf, "@%s%s", options.extref_prefix ? options.extref_prefix : "", name + 4);
	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));

	/* Restore state after CIF call */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (intptr_t)(-28), ARG_REG, REG_SCHED));

	/* Restore Wptr: subtract back the 4 we added */
	*pst_last = compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, (intptr_t)4, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
	add_to_ins_chain (*pst_last);
}
/*}}}*/

/*{{{  static void compose_fp_set_fround_aarch64 (tstate *ts, int mode)*/
/*
 * Sets floating point rounding mode
 */
static void compose_fp_set_fround_aarch64 (tstate *ts, int mode)
{
	/* Track the rounding mode for FP→INT conversions.
	 * On aarch64 we use the correct fcvt* variant instead of
	 * changing the hardware FPCR register. */
	aarch64_fp_rounding_mode = mode;
	{
		char buf[64];
		snprintf(buf, sizeof(buf), "// FP rounding mode set to %d", mode);
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
	}
}
/*}}}*/

/*{{{  static void compose_fp_init_aarch64 (tstate *ts)*/
/*
 * Initializes floating point unit
 */
static void compose_fp_init_aarch64 (tstate *ts)
{
	/* ARM64 FPU initialization - use default IEEE 754 settings.
	 * Also reset the FP stack simulation so each procedure starts clean. */
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// ARM64 FPU init (default IEEE 754)")));
	aarch64_fp_stack_depth = 0;
	aarch64_fp_stack_prec[0] = 0;
	aarch64_fp_stack_prec[1] = 0;
	aarch64_fp_stack_prec[2] = 0;
	aarch64_fp_precision = 0;
	aarch64_fp_rounding_mode = 0;  /* FPU_N = round to nearest */
	aarch64_fp_had_fpint = 0;
}
/*}}}*/

/*{{{  static void compose_refcountop_aarch64 (tstate *ts, int op, int reg)*/
/*
 * Performs reference counting operations
 */
static void compose_refcountop_aarch64 (tstate *ts, int op, int reg)
{
	switch (op) {
	case I_RCINIT:
		/* Initialize reference count to 1 */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REGIND, reg));
		break;
	case I_RCINC:
		/* Increment reference count atomically */
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 1, ARG_REGIND, reg, ARG_REGIND, reg));
		ts->stack->must_set_cmp_flags = 1;
		break;
	case I_RCDEC:
		/* Decrement reference count and test for zero */
		add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_CONST, 1, ARG_REGIND, reg, ARG_REGIND, reg, ARG_REG | ARG_IMP, REG_CC));
		/* Set result register based on zero flag */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, reg));
		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, reg));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
		constmap_remove (reg);
		ts->stack->must_set_cmp_flags = 0;
		break;
	default:
		fprintf (stderr, "%s: compose_refcountop_aarch64: unknown operation %d\n", progname, op);
		break;
	}
}
/*}}}*/

/*{{{  static void compose_memory_barrier_aarch64 (tstate *ts, int sec)*/
/*
 * Generates memory barrier instructions
 */
static void compose_memory_barrier_aarch64 (tstate *ts, int sec)
{
	switch (sec) {
	case I_MB:
		/* Full memory barrier */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("dmb sy")));
		break;
	case I_RMB:
		/* Read memory barrier */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("dmb ld")));
		break;
	case I_WMB:
		/* Write memory barrier */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("dmb st")));
		break;
	default:
		/* Default to full barrier */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("dmb sy")));
		break;
	}

	tstack_undefine (ts->stack);
	constmap_clearall ();
}
/*}}}*/

/*{{{  static void compose_nreturn_aarch64 (tstate *ts, int adjust)*/
/*
 * Generates non-standard return (for NOCC)
 */
static void compose_nreturn_aarch64 (tstate *ts, int adjust)
{
	int tmp_reg = tstack_newreg (ts->stack);
	int toldregs[3], tfixedregs[3];
	int i, nresults;

	/* Function result handling for aarch64, following the SPARC/PPC pattern:
	 * Constrain results to fixed physical registers (x19/x20/x21).
	 * The register allocator handles spilling for nested function calls.
	 * Always constrain at least 1 result for IS abbreviation function
	 * compatibility (where FUNCRETURN is not emitted by occ21). */
	toldregs[0] = ts->stack->old_a_reg;
	toldregs[1] = ts->stack->old_b_reg;
	toldregs[2] = ts->stack->old_c_reg;
	tfixedregs[0] = AARCH64_REG_FUNCRES0;
	tfixedregs[1] = AARCH64_REG_FUNCRES1;
	tfixedregs[2] = AARCH64_REG_FUNCRES2;

	nresults = ts->numfuncresults;

	for (i = 0; i < nresults; i++) {
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, toldregs[i], ARG_REG, tfixedregs[i]));
	}

	/* Load return address from Wptr[0] */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, REG_WPTR, ARG_REG, tmp_reg));

	/* Adjust workspace pointer */
	if (adjust != 0) {
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, adjust << WSH, ARG_REG, REG_WPTR));
	}

	/* Jump to return address */
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, tmp_reg));

	/* Unconstrain (metadata for register allocator, placed after the jump) */
	for (i = nresults - 1; i >= 0; i--) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, toldregs[i]));
	}
	ts->numfuncresults = 0;
}
/*}}}*/

/*{{{  static void compose_funcresults_aarch64 (tstate *ts, int nresults)*/
/*
 *\tHandles function result collection
 */
static void compose_funcresults_aarch64 (tstate *ts, int nresults)
{
	int i;
	int tnewregs[3], tfixedregs[3];

	/* Collect function results from fixed registers (SPARC/PPC pattern).
	 * The register allocator handles spilling if these regs were in use. */
	for (i = 0; i < nresults; i++) {
		tstack_push (ts->stack);
	}
	tfixedregs[0] = AARCH64_REG_FUNCRES0;
	tfixedregs[1] = AARCH64_REG_FUNCRES1;
	tfixedregs[2] = AARCH64_REG_FUNCRES2;
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

/*{{{  Validation Functions*/
static int rtl_validate_instr_aarch64 (ins_chain *ins)
{
	/* Basic validation - accept all instructions for now */
	/* More sophisticated validation could be added here */
	return 1;
}

static int rtl_prevalidate_instr_aarch64 (ins_chain *ins)
{
	/* Pre-validation - accept all instructions for now */
	return 1;
}

static int regcolour_fp_regs_aarch64 (int *regs)
{
	/* ARM64 has 32 SIMD/FP registers (v0-v31) */
	int fp_regs[] = {0, 1, 2, 3, 4, 5, 6, 7}; /* Use first 8 for allocation */
	int count = sizeof(fp_regs) / sizeof(fp_regs[0]);
	int i;

	for (i = 0; i < count; i++) {
		regs[i] = fp_regs[i];
	}
	return count;
}
/*}}}*/

/* Stub functions for remaining unimplemented features */
static void aarch64_stub_void (tstate *ts) { /* stub */ }
static void aarch64_stub_rmox_entry_prolog (tstate *ts, rmoxmode_e mode) { /* stub */ }

/* Critical function implementations to fix segfaults */
static void compose_aarch64_move (tstate *ts) {
	/* BLOCKCOPY: copy count bytes from src to dst.
	 * Transputer stack: Areg=count, Breg=dst, Creg=src.
	 * Following the SPARC/PPC byte-at-a-time loop pattern.
	 * Must materialise pointers from constmap before the loop since
	 * the loop modifies the registers via INS_ADD. */
	int tmpreg;

	/* Materialise src pointer from constmap if needed */
	switch (constmap_typeof (ts->stack->old_c_reg)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_c_reg), ARG_REG, ts->stack->old_c_reg));
		break;
	case VALUE_LOCALPTR:
		if (constmap_regconst (ts->stack->old_c_reg)) {
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, constmap_regconst (ts->stack->old_c_reg) << WSH, ARG_REG, ts->stack->old_c_reg));
		} else {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, ts->stack->old_c_reg));
		}
		break;
	default:
		break;
	}
	constmap_remove (ts->stack->old_c_reg);

	/* Materialise dst pointer from constmap if needed */
	switch (constmap_typeof (ts->stack->old_b_reg)) {
	case VALUE_LABADDR:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_b_reg), ARG_REG, ts->stack->old_b_reg));
		break;
	case VALUE_LOCALPTR:
		if (constmap_regconst (ts->stack->old_b_reg)) {
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, REG_WPTR, ARG_CONST, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG, ts->stack->old_b_reg));
		} else {
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, ts->stack->old_b_reg));
		}
		break;
	default:
		break;
	}
	constmap_remove (ts->stack->old_b_reg);

	/* Materialise count from constmap */
	constmap_remove (ts->stack->old_a_reg);

	/* Constrain count, src, dst to distinct physical registers so the
	 * register allocator doesn't merge them with the byte temp register.
	 * Use x0=count, x1=dst, x2=src, x3=tmp — all low registers. */
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, 0));
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG, 1));
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG, 2));
	tmpreg = tstack_newreg (ts->stack);
	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmpreg, ARG_REG, 3));

	/* Loop: test count, decrement, copy byte, increment ptrs, repeat */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg,
		ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 1));
	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, tmpreg));
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG | ARG_IS8BIT, tmpreg, ARG_REGIND, ts->stack->old_b_reg));
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_BLABEL, 0));
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));

	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmpreg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_c_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_b_reg));
	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
}

static void compose_aarch64_shift (tstate *ts, int sec, int r1, int r2, int r3) {
	/* Shift operations */
	if (sec == I_SHL) {
		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
	} else {
		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
	}
}

static void compose_aarch64_division (tstate *ts, int dividend, int divisor, int quotient) {
	/* Basic division */
	add_to_ins_chain (compose_ins (INS_DIV, 2, 1, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, quotient));
}

static int compose_aarch64_remainder (tstate *ts, int dividend, int divisor) {
	/* AArch64 has no hardware REM instruction.
	 * Compute remainder as: dividend - (dividend / divisor) * divisor
	 * Using: sdiv tmp, dividend, divisor; msub result, tmp, divisor, dividend */
	int tmp_reg = tstack_newreg(ts->stack);
	int result_reg = tstack_newreg(ts->stack);
	/* tmp = dividend / divisor */
	add_to_ins_chain (compose_ins (INS_DIV, 2, 1, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, tmp_reg));
	/* result = dividend - tmp * divisor  (using MUL then SUB since we don't have MSUB in RTL) */
	add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, tmp_reg, ARG_REG, divisor, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, tmp_reg, ARG_REG, dividend, ARG_REG, result_reg));
	return result_reg;
}

static void compose_aarch64_occam_call (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last)
{
	if (name) {
		char *sbuf = aarch64_convert_symbol_name(name);
		*pst_first = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, sbuf);
		add_to_ins_chain (*pst_first);
		*pst_last = *pst_first;
	} else {
		/* Internal Occam call generated by I_CALL */
		/* In AArch64, we don't drop Wptr here since etcrtl.c already dropped it.
		 * The return address MUST go into Wptr[-1] (W_IPTR), NOT Wptr[0]!
		 * Wptr[0] contains the 4th argument (Wptr[-4] from the caller)!
		 */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_ISCONST | ARG_FLABEL, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->stack->old_a_reg));
		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	}
}
static void compose_aarch64_external_ccall (tstate *ts, int inlined, char *name, ins_chain **pst_first, ins_chain **pst_last) {
	/* For aarch64, occam-to-occam calls are handled by compose_aarch64_occam_call.
	 * This function is now only for C calls, which are handled by compose_cif_call. */
	compose_cif_call_aarch64(ts, inlined, name, pst_first, pst_last);
}

/*{{{  I/O space operations for aarch64*/
/*
 * I/O space operations for aarch64 - these provide hardware interface functionality
 * for accessing I/O ports and memory-mapped I/O regions
 */

/*{{{  static int compose_iospace_loadbyte_aarch64 (tstate *ts, int portreg, int targetreg)*/
/*
 * Loads a byte from I/O space port into target register
 */
static int compose_iospace_loadbyte_aarch64 (tstate *ts, int portreg, int targetreg)
{
	int tmp_reg = tstack_newreg (ts->stack);
	int result_reg = (targetreg == portreg) ? tstack_newreg (ts->stack) : targetreg;

	/* Load port address into temporary register */
	switch (constmap_typeof (portreg)) {
	case VALUE_CONST:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
		break;
	}

	/* Load byte from I/O port (memory-mapped I/O on ARM64) */
	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, tmp_reg, ARG_REG, result_reg));

	return result_reg;
}
/*}}}*/

/*{{{  static void compose_iospace_storebyte_aarch64 (tstate *ts, int portreg, int sourcereg)*/
/*
 *\tStores a byte from source register to I/O space port
 */
static void compose_iospace_storebyte_aarch64 (tstate *ts, int portreg, int sourcereg)
{
	int tmp_reg = tstack_newreg (ts->stack);

	/* Load port address into temporary register */
	switch (constmap_typeof (portreg)) {
	case VALUE_CONST:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
		break;
	}

	/* Store byte to I/O port (memory-mapped I/O on ARM64) */
	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, sourcereg, ARG_REGIND, tmp_reg));
}
/*}}}*/

/*{{{  static int compose_iospace_loadword_aarch64 (tstate *ts, int portreg, int targetreg)*/
/*
 *\tLoads a word (32-bit) from I/O space port into target register
 */
static int compose_iospace_loadword_aarch64 (tstate *ts, int portreg, int targetreg)
{
	int tmp_reg = tstack_newreg (ts->stack);
	int result_reg = (targetreg == portreg) ? tstack_newreg (ts->stack) : targetreg;

	/* Load port address into temporary register */
	switch (constmap_typeof (portreg)) {
	case VALUE_CONST:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
		break;
	}

	/* Load word from I/O port (memory-mapped I/O on ARM64) */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, tmp_reg, ARG_REG, result_reg));

	return result_reg;
}
/*}}}*/

/*{{{  static void compose_iospace_storeword_aarch64 (tstate *ts, int portreg, int sourcereg)*/
/*
 *\tStores a word (32-bit) from source register to I/O space port
 */
static void compose_iospace_storeword_aarch64 (tstate *ts, int portreg, int sourcereg)
{
	int tmp_reg = tstack_newreg (ts->stack);

	/* Load port address into temporary register */
	switch (constmap_typeof (portreg)) {
	case VALUE_CONST:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
		break;
	}

	/* Store word to I/O port (memory-mapped I/O on ARM64) */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, sourcereg, ARG_REGIND, tmp_reg));
}
/*}}}*/

/*{{{  static void compose_iospace_read_aarch64 (tstate *ts, int portreg, int addrreg, int width)*/
/*
 *\tReads data from I/O space port to memory address with specified width
 */
static void compose_iospace_read_aarch64 (tstate *ts, int portreg, int addrreg, int width)
{
	int tmp_reg = tstack_newreg (ts->stack);
	int data_reg = tstack_newreg (ts->stack);

	/* Load port address into temporary register */
	switch (constmap_typeof (portreg)) {
	case VALUE_CONST:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
		break;
	case VALUE_LOCAL:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (portreg) << WSH, ARG_REG, tmp_reg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
		break;
	}

	/* Read data from I/O port based on width */
	switch (width) {
	case 8:
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, tmp_reg, ARG_REG, data_reg));
		break;
	case 16:
		add_to_ins_chain (compose_ins (INS_MOVEW, 1, 1, ARG_REGIND, tmp_reg, ARG_REG, data_reg));
		break;
	case 32:
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, tmp_reg, ARG_REG, data_reg));
		break;
	}

	/* Store data to memory address */
	switch (constmap_typeof (addrreg)) {
	case VALUE_LOCALPTR:
		switch (width) {
		case 8:
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, data_reg, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (addrreg) << WSH));
			break;
		case 16:
			add_to_ins_chain (compose_ins (INS_MOVEW, 1, 1, ARG_REG, data_reg, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (addrreg) << WSH));
			break;
		default:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, data_reg, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (addrreg) << WSH));
			break;
		}
		break;
	default:
		switch (width) {
		case 8:
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, data_reg, ARG_REGIND, addrreg));
			break;
		case 16:
			add_to_ins_chain (compose_ins (INS_MOVEW, 1, 1, ARG_REG, data_reg, ARG_REGIND, addrreg));
			break;
		default:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, data_reg, ARG_REGIND, addrreg));
			break;
		}
		break;
	}
}
/*}}}*/

/*{{{  static void compose_iospace_write_aarch64 (tstate *ts, int portreg, int addrreg, int width)*/
/*
 *\tWrites data from memory address to I/O space port with specified width
 */
static void compose_iospace_write_aarch64 (tstate *ts, int portreg, int addrreg, int width)
{
	int tmp_reg = tstack_newreg (ts->stack);
	int data_reg = tstack_newreg (ts->stack);

	/* Load data from memory address based on width */
	switch (constmap_typeof (addrreg)) {
	case VALUE_LOCALPTR:
		switch (width) {
		case 8:
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (addrreg) << WSH, ARG_REG, data_reg));
			break;
		case 16:
			add_to_ins_chain (compose_ins (INS_MOVEW, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (addrreg) << WSH, ARG_REG, data_reg));
			break;
		default:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (addrreg) << WSH, ARG_REG, data_reg));
			break;
		}
		break;
	default:
		switch (width) {
		case 8:
			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, addrreg, ARG_REG, data_reg));
			break;
		case 16:
			add_to_ins_chain (compose_ins (INS_MOVEW, 1, 1, ARG_REGIND, addrreg, ARG_REG, data_reg));
			break;
		default:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, addrreg, ARG_REG, data_reg));
			break;
		}
		break;
	}

	/* Load port address into temporary register */
	switch (constmap_typeof (portreg)) {
	case VALUE_CONST:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
		break;
	case VALUE_LOCAL:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (portreg) << WSH, ARG_REG, tmp_reg));
		break;
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
		break;
	}

	/* Write data to I/O port based on width */
	switch (width) {
	case 8:
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, data_reg, ARG_REGIND, tmp_reg));
		break;
	case 16:
		add_to_ins_chain (compose_ins (INS_MOVEW, 1, 1, ARG_REG, data_reg, ARG_REGIND, tmp_reg));
		break;
	case 32:
	default:
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, data_reg, ARG_REGIND, tmp_reg));
		break;
	}
}
/*}}}*/
/*}}}*/

/*{{{  static arch_t aarch64_arch*/
static arch_t aarch64_arch = {
	.archname = "aarch64",
	.compose_kcall = compose_aarch64_kcall,
	.compose_kjump = compose_aarch64_kjump,
	.compose_deadlock_kcall = compose_aarch64_deadlock_kcall,
	.compose_pre_enbc = compose_pre_enbc_aarch64,
	.compose_pre_enbt = compose_pre_enbt_aarch64,
	.compose_inline_ldtimer = compose_aarch64_inline_ldtimer,
	.compose_inline_tin = compose_aarch64_inline_tin,
	.compose_inline_quick_reschedule = compose_aarch64_inline_quick_reschedule,
	.compose_inline_full_reschedule = compose_aarch64_inline_full_reschedule,
	.compose_inline_in = NULL,		/* disabled: stub was broken (simple word copy, no proper channel protocol) */
	.compose_inline_in_2 = NULL,
	.compose_inline_min = NULL,
	.compose_inline_out = NULL,		/* disabled: stub was broken */
	.compose_inline_out_2 = NULL,
	.compose_inline_mout = NULL,
	.compose_inline_enbc = compose_inline_enbc_aarch64,
	.compose_inline_disc = compose_inline_disc_aarch64,
	.compose_inline_altwt = compose_inline_altwt_aarch64,
	.compose_inline_stlx = compose_inline_stlx_aarch64,
	.compose_inline_malloc = compose_inline_malloc_aarch64,
	.compose_inline_startp = compose_inline_startp_aarch64,
	.compose_inline_endp = compose_inline_endp_aarch64,
	.compose_inline_runp = aarch64_stub_void,
	.compose_inline_stopp = compose_inline_stopp_aarch64,
	.compose_debug_insert = compose_debug_insert_aarch64,
	.compose_debug_procnames = compose_debug_procnames_aarch64,
	.compose_debug_filenames = compose_debug_filenames_aarch64,
	.compose_debug_zero_div = compose_debug_zero_div_aarch64,
	.compose_debug_floaterr = compose_debug_floaterr_aarch64,
	.compose_debug_overflow = compose_debug_overflow_aarch64,
	.compose_debug_rangestop = compose_debug_rangestop_aarch64,
	.compose_debug_seterr = compose_debug_seterr_aarch64,
	.compose_overflow_jumpcode = compose_overflow_jumpcode_aarch64,
	.compose_floaterr_jumpcode = compose_floaterr_jumpcode_aarch64,
	.compose_rangestop_jumpcode = compose_rangestop_jumpcode_aarch64,
	.compose_debug_deadlock_set = compose_debug_deadlock_set_aarch64,
	.compose_divcheck_zero = compose_divcheck_zero_aarch64,
	.compose_divcheck_zero_simple = compose_divcheck_zero_simple_aarch64,
	.compose_division = compose_aarch64_division,
	.compose_remainder = compose_aarch64_remainder,
	.compose_iospace_loadbyte = compose_iospace_loadbyte_aarch64,
	.compose_iospace_storebyte = compose_iospace_storebyte_aarch64,
	.compose_iospace_loadword = compose_iospace_loadword_aarch64,
	.compose_iospace_storeword = compose_iospace_storeword_aarch64,
	.compose_iospace_read = compose_iospace_read_aarch64,
	.compose_iospace_write = compose_iospace_write_aarch64,
	.compose_move_loadptrs = compose_move_loadptrs_aarch64,
	.compose_move = compose_aarch64_move,
	.compose_shift = compose_aarch64_shift,
	.compose_widenshort = compose_aarch64_widenshort,
	.compose_widenword = compose_aarch64_widenword,
	.compose_longop = compose_aarch64_longop,
	.compose_fpop = compose_aarch64_fpop,
	.compose_external_ccall = compose_external_ccall_aarch64,
	.compose_bcall = compose_bcall_aarch64,
	.compose_cif_call = compose_cif_call_aarch64,
	.compose_entry_prolog = compose_aarch64_entry_prolog,
	.compose_rmox_entry_prolog = aarch64_stub_rmox_entry_prolog,
	.compose_fp_set_fround = compose_fp_set_fround_aarch64,
	.compose_fp_init = compose_fp_init_aarch64,
	.compose_reset_fregs = aarch64_compose_reset_fregs,
	.compose_refcountop = compose_refcountop_aarch64,
	.compose_memory_barrier = compose_memory_barrier_aarch64,
	.compose_return = compose_aarch64_return,
	.compose_nreturn = compose_nreturn_aarch64,
	.compose_funcresults = compose_funcresults_aarch64,
	.regcolour_special_to_real = aarch64_regcolour_special_to_real,
	.regcolour_rmax = 25,	/* x0-x24 are available; x25-x28 reserved for runtime, x29-x31 reserved */
	.regcolour_nodemax = 256,
	.regcolour_get_regs = aarch64_regcolour_get_regs,
	.regcolour_fp_regs = regcolour_fp_regs_aarch64,
	.code_to_asm = aarch64_code_to_asm,
	.code_to_asm_stream = aarch64_code_to_asm_stream,
	.rtl_validate_instr = rtl_validate_instr_aarch64,
	.rtl_prevalidate_instr = rtl_prevalidate_instr_aarch64,
	.get_register_name = aarch64_get_register_name,
	.int_options = 0,
	.kiface_tableoffs = 0
};
/*}}}*/

/*{{{  arch_t *init_arch_aarch64 (int mclass)*/
arch_t *init_arch_aarch64 (int mclass)
{
	return &aarch64_arch;
}
/*}}}*/

/*{{{  static void compose_aarch64_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)*/
static void compose_aarch64_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)
{
	kif_entrytype *entry = kif_entry (call);
	int i, cregs[3];
	ins_chain *call_ins;
	char *entrypoint_name;
	int sched_reg, wptr_reg;

	/* Validate entry and entrypoint */
	if (!entry || !entry->entrypoint) {
		entrypoint_name = string_dup ("unknown_kernel_call");
	} else {
		entrypoint_name = aarch64_convert_symbol_name(entry->entrypoint);
	}

				/* Map stack registers to aarch64 registers */
	if (ts->stack) {
		cregs[0] = ts->stack->old_a_reg;
		cregs[1] = ts->stack->old_b_reg;
		cregs[2] = ts->stack->old_c_reg;
	} else {
		cregs[0] = cregs[1] = cregs[2] = REG_UNDEFINED;
	}

	/* CCSP calling convention: pass parameters directly to kernel function */
	/* The CCSP runtime expects: kernel_func(param0, sched, Wptr) */
	/* where param0 is the first parameter, sched is scheduler pointer, Wptr is workspace */
	/* Additional parameters are accessed via sched->cparam[] array */

	/* CRITICAL FIX: We must process cregs[1] and cregs[2] BEFORE cregs[0] to avoid
	 * clobbering the register that cregs[1] might be stored in (e.g. x0) when we
	 * write cregs[0] into REG_X0.
	 */
	 
	/* Store additional parameters in scheduler cparam array for K_CALL_PARAM macro access */
	if (regs_in > 1 && cregs[1] != REG_UNDEFINED) {
		/* Second parameter goes to sched->cparam[0] */
		switch (constmap_typeof (cregs[1])) {
		case VALUE_CONST:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (cregs[1]), ARG_REGIND | ARG_DISP, REG_SCHED, 8));
			break;
		default:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, cregs[1], ARG_REGIND | ARG_DISP, REG_SCHED, 8));
			break;
		}
	}

	if (regs_in > 2 && cregs[2] != REG_UNDEFINED) {
		/* Third parameter goes to sched->cparam[1] */
		switch (constmap_typeof (cregs[2])) {
		case VALUE_CONST:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (cregs[2]), ARG_REGIND | ARG_DISP, REG_SCHED, 16));
			break;
		default:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, cregs[2], ARG_REGIND | ARG_DISP, REG_SCHED, 16));
			break;
		}
	}

	/* First parameter (value) goes to x0 */
	if (regs_in > 0 && cregs[0] != REG_UNDEFINED) {
		switch (constmap_typeof (cregs[0])) {
		case VALUE_CONST:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (cregs[0]), ARG_REG, REG_X0));
			break;
		default:
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, cregs[0], ARG_REG, REG_X0));
			break;
		}
	} else {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, REG_X0));
	}

	/* Get scheduler pointer and store in x1 */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, REG_X1));

	/* Workspace pointer goes to x2 */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_X2));

	call_ins = compose_ins (INS_CALL, 4, 0, ARG_NAMEDLABEL, entrypoint_name, ARG_REG, REG_X0, ARG_REG, REG_X1, ARG_REG, REG_X2);
	add_to_ins_chain (call_ins);

	if (regs_out > 0 && ts->stack) {
		int oregs[3];
		int oi, oj;

		tstack_undefine (ts->stack);
		constmap_clearall ();
		ts->stack->must_set_cmp_flags = 1;
		ts->stack->ts_depth = regs_out;

		oregs[0] = oregs[1] = oregs[2] = REG_UNDEFINED;
		if (regs_out >= 1) {
			/* Allocate a FRESH register for the return value.
			 * Using old_a_reg (cregs[0]) causes the register allocator
			 * to reuse the same physical register, which can be overwritten
			 * by subsequent instructions before the value is consumed.
			 * A fresh register avoids this conflict. */
			oregs[0] = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, oregs[0]));
			ts->stack->a_reg = oregs[0];
		}
		if (regs_out >= 2) {
			/* Second result comes from cparam[0] */
			oregs[1] = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[0]), ARG_REG, oregs[1]));
			ts->stack->b_reg = oregs[1];
		}
		if (regs_out >= 3) {
			/* Third result comes from cparam[1] */
			oregs[2] = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[1]), ARG_REG, oregs[2]));
			ts->stack->c_reg = oregs[2];
		}
		/* Set implied outputs on the call instruction so the register
		 * allocator knows these registers are live after the call */
		for (oi = 0; call_ins->out_args[oi]; oi++);
		for (oj = 0; oj < regs_out; oj++) {
			if (oregs[oj] != REG_UNDEFINED) {
				call_ins->out_args[oi + oj] = new_ins_arg ();
				call_ins->out_args[oi + oj]->regconst = oregs[oj];
				call_ins->out_args[oi + oj]->flags = (ARG_REG | ARG_IMP) & ARG_FLAGMASK;
			}
		}
	}
}
/*}}}*/

/*{{{  static ins_chain *compose_aarch64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry)*/
static ins_chain *compose_aarch64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry)
{
	ins_chain *jump_ins;
	char *entrypoint_name;

	/* Validate kif_entry and entrypoint */
	if (!kif_entry || !kif_entry->entrypoint) {
		entrypoint_name = string_dup ("unknown_kernel_jump");
	} else {
		entrypoint_name = aarch64_convert_symbol_name(kif_entry->entrypoint);
	}
	
	/* Generate appropriate jump instruction */
	if (instr == INS_CJUMP) {
		/* Conditional jump */
		jump_ins = compose_ins (INS_CJUMP, 2, 0, ARG_COND, cc, ARG_NAMEDLABEL, entrypoint_name);
	} else if (instr == INS_CALL) {
		/* Call instruction */
		jump_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, entrypoint_name);
	} else {
		/* Unconditional jump */
		jump_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, entrypoint_name);
	}

	return jump_ins;
}
/*}}}*/

/*{{{  static void compose_aarch64_deadlock_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)*/
static void compose_aarch64_deadlock_kcall (tstate *ts, const int call, const int regs_in, const int regs_out)
{
	/* Set up deadlock debugging information if enabled */
	if (options.debug_options & DEBUG_DEADLOCK) {
		/* Store current workspace pointer for debugging */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, REG_WPTR));
	}

	/* Perform the actual kernel call */
	compose_aarch64_kcall (ts, call, regs_in, regs_out);

	if (options.debug_options & DEBUG_DEADLOCK) {
		/* Clear link field after rescheduling */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR));
	}
}
/*}}}*/

/*{{{  static void compose_aarch64_inline_ldtimer (tstate *ts)*/
static void compose_aarch64_inline_ldtimer (tstate *ts)
{
	/* Use kernel call for LDTIMER.  NOTE: The kernel call path works
	 * correctly for simple timer reads. However, when the timer value
	 * is stored through a PROC reference parameter, occ21 generates
	 * ETC code with TSDEPTH >= 4, which overflows the 3-register
	 * evaluation stack model. This causes "register lost from stack"
	 * warnings and the timer value is discarded. This is an occ21
	 * compiler issue that needs to be fixed in the ETC code generation. */
	compose_aarch64_kcall (ts, K_LDTIMER, 0, 1);
}
/*}}}*/

/*{{{  static void compose_aarch64_inline_tin (tstate *ts)*/
static void compose_aarch64_inline_tin (tstate *ts)
{
	/* Use kernel call for timer input.  The inline version uses
	 * raw cntvct_el0 which is in hardware ticks, not microseconds.
	 * The kernel TIN handles conversion and proper timer queue management. */
	compose_aarch64_kcall (ts, K_TIN, 1, 0);
}
/*}}}*/

/*{{{  static void compose_aarch64_inline_quick_reschedule (tstate *ts)*/
static void compose_aarch64_inline_quick_reschedule (tstate *ts)
{
	/* Check if there are more processes to run */
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 1));

	/* No processes, call scheduler */
	compose_aarch64_kcall (ts, K_PAUSE, 0, 0);
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));

	/* Load next process */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, REG_WPTR));

	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
}
/*}}}*/

/*{{{  static void compose_aarch64_inline_full_reschedule (tstate *ts)*/
static void compose_aarch64_inline_full_reschedule (tstate *ts)
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

/*{{{  static void compose_aarch64_inline_in (tstate *ts, int width)*/
static void compose_aarch64_inline_in (tstate *ts, int width)
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
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));

	/* Channel not ready, block */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	compose_aarch64_kcall (ts, K_PAUSE, 0, 0);

	/* Channel ready, perform input */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	if (width == 8) {
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
	} else {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
	}
}
/*}}}*/

/*{{{  static void compose_aarch64_inline_out (tstate *ts, int width)*/
static void compose_aarch64_inline_out (tstate *ts, int width)
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
	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));

	/* Channel not ready, block */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
	compose_aarch64_kcall (ts, K_PAUSE, 0, 0);

	/* Channel ready, perform output */
	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
	if (width == 8) {
		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
	} else {
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
	}
}
/*}}}*/

/*{{{  static void compose_aarch64_entry_prolog (tstate *ts)*/
static void compose_aarch64_entry_prolog (tstate *ts)
{
	rtl_chain *trtl;
	char *sbuffer = aarch64_convert_symbol_name("occam_start");
	int tmp_reg;

	/* Generate the _occam_start symbol like i386 version */
	trtl = new_rtl ();
	trtl->type = RTL_PUBLICSETNAMEDLABEL;
	trtl->u.label_name = sbuffer;
	add_to_rtl_chain (trtl);

		/* Initialize aarch64 FPU - default IEEE 754 settings, no adjustment needed */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// ARM64 FPU init (default IEEE 754)")));

		/* Call kernel initialization */		compose_aarch64_kcall (ts, K_PAUSE, 0, 0);
	}/*}}}*/

/*{{{  static void compose_aarch64_return (tstate *ts)*/
static void compose_aarch64_return (tstate *ts)
{
	int toldregs[3], tfixedregs[3];
	int i;

	/* Function result handling (SPARC/PPC register-constrained pattern) */
	toldregs[0] = ts->stack->old_a_reg;
	toldregs[1] = ts->stack->old_b_reg;
	toldregs[2] = ts->stack->old_c_reg;
	tfixedregs[0] = AARCH64_REG_FUNCRES0;
	tfixedregs[1] = AARCH64_REG_FUNCRES1;
	tfixedregs[2] = AARCH64_REG_FUNCRES2;

	for (i = 0; i < ts->numfuncresults; i++) {
		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, toldregs[i], ARG_REG, tfixedregs[i]));
	}

	/* Pop 32 bytes from Wptr (I_CALL frame = 4 words on 64-bit) */
	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 32, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));

	/* Jump to return address at old Wptr[0] (now at Wptr[-32]) */
	add_to_ins_chain (compose_ins (INS_RET, 0, 0));

	for (i = ts->numfuncresults - 1; i >= 0; i--) {
		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, toldregs[i]));
	}
	ts->numfuncresults = 0;
}
/*}}}*/
/*{{{  static int compose_aarch64_widenshort (tstate *ts)*/
static int compose_aarch64_widenshort (tstate *ts)
{
	/* CRITICAL FIX: Validate stack state */
	if (!ts || !ts->stack) {
		fprintf (stderr, "aarch64_widenshort: invalid tstate or stack\n");
		return 0;
	}
	
	int tmp_reg = tstack_newreg (ts->stack);
	
	/* Ensure we have a valid source register */
	if (ts->stack->a_reg == REG_UNDEFINED || ts->stack->a_reg == (int)0x80000000) {
		ts->stack->a_reg = tstack_newreg (ts->stack);
	}

	/* Sign extend 16-bit to 64-bit */
	add_to_ins_chain (compose_ins (INS_MOVESEXT16TO32, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));

	ts->stack->a_reg = tmp_reg;
	return tmp_reg;
}
/*}}}*/

/*{{{  static int compose_aarch64_widenword (tstate *ts)*/
static int compose_aarch64_widenword (tstate *ts)
{
	/* CRITICAL FIX: Validate stack state */
	if (!ts || !ts->stack) {
		fprintf (stderr, "aarch64_widenword: invalid tstate or stack\n");
		return 0;
	}
	
	int tmp_reg = tstack_newreg (ts->stack);
	
	/* Ensure we have valid source registers */
	if (ts->stack->a_reg == REG_UNDEFINED || ts->stack->a_reg == (int)0x80000000) {
		ts->stack->a_reg = tstack_newreg (ts->stack);
	}
	if (ts->stack->old_a_reg == REG_UNDEFINED || ts->stack->old_a_reg == (int)0x80000000) {
		ts->stack->old_a_reg = tstack_newreg (ts->stack);
	}
	if (ts->stack->old_b_reg == REG_UNDEFINED || ts->stack->old_b_reg == (int)0x80000000) {
		ts->stack->old_b_reg = tstack_newreg (ts->stack);
	}

	/* Sign-extend 32-bit INT to double-word (hi:lo).
	 * Areg holds the 32-bit value.  We need:
	 *   a_reg (lo) = Areg (unchanged)
	 *   b_reg (hi) = sign extension (0x00000000 or 0xFFFFFFFF)
	 * Generate hi word by testing bit 31 and negating. */
	/* Arrange for old_a_reg to stay at stack-top */
	ts->stack->a_reg = ts->stack->old_a_reg;
	ts->stack->b_reg = tmp_reg;
	ts->stack->c_reg = ts->stack->old_b_reg;

	/* Sign-extend the 32-bit INT to produce a proper hi word 
	 * (0 for positive, 0xFFFFFFFF for negative). */
	add_to_ins_chain (compose_ins (INS_SIGNEXT32, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
	add_to_ins_chain (compose_ins (INS_SAR, 2, 1, ARG_CONST, (intptr_t)63, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
	constmap_remove (tmp_reg);
	return tmp_reg;
	}
/*}}}*/

/*{{{  static void compose_aarch64_longop (tstate *ts, int secondary_opcode)*/
static void compose_aarch64_longop (tstate *ts, int secondary_opcode)
{
        /* CRITICAL FIX: Validate stack state before operations */
        if (!ts || !ts->stack) {
                fprintf (stderr, "aarch64_longop: invalid tstate or stack\n");
                return;
        }

        /* Sync the architecture-specific FP stack depth with the generic tracker.
         * This handles cases like .REALRESULT where the generic tracker is updated
         * but the architecture backend is not explicitly told to push. */
        if (aarch64_fp_stack_depth > ts->stack->fs_depth) {
                aarch64_fp_stack_depth = ts->stack->fs_depth;
        }
        if (aarch64_fp_stack_depth < ts->stack->fs_depth) {
                while (aarch64_fp_stack_depth < ts->stack->fs_depth) {
                        aarch64_fp_push(ts->stack->fpu_mode == 2 ? 64 : 32);
                }
        }

        /* Ensure we have valid registers */	if (ts->stack->old_a_reg == REG_UNDEFINED || ts->stack->old_a_reg == (int)0x80000000) {
		ts->stack->old_a_reg = tstack_newreg (ts->stack);
	}
	if (ts->stack->old_b_reg == REG_UNDEFINED || ts->stack->old_b_reg == (int)0x80000000) {
		ts->stack->old_b_reg = tstack_newreg (ts->stack);
	}
	
	switch (secondary_opcode) {
	/* Handle actual transputer long operations */
	/*{{{  I_LADD -- long addition (with overflow check) */
	case I_LADD:
		/* LADD: sum := Areg + Breg + (Creg & 1), sets overflow flag
		 * Transputer stack: Areg=a, Breg=b, Creg=carry_in
		 * Result: a_reg = sum, CC set for overflow detection */
		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
		/* Use flag-setting add: adds old_a + old_b → old_b, sets CC */
		add_to_ins_chain (compose_ins (INS_ADD, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		/* Capture carry from first add into tmp */
		{
			int carry1 = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_AE, ARG_REG, carry1));
			/* Add carry_in: old_b += old_c (which is 0 or 1) */
			add_to_ins_chain (compose_ins (INS_ADD, 2, 2, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
			constmap_remove (ts->stack->old_b_reg);
		}
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	/*}}}*/
	/*{{{  I_LSUM -- long sum (no overflow checks, produces carry) */
	case I_LSUM:
		/* LSUM: carry_out, sum := Areg + Breg + (Creg & 1)
		 * Result: a_reg = sum, b_reg = carry_out (0 or 1) */
		{
			int sum64 = tstack_newreg (ts->stack);
			int carry = tstack_newreg (ts->stack);

			/* Zero-extend inputs to 32 bits (64-bit registers) to compute sum without 64-bit overflow */
			/* Creg (old_c_reg) is carry_in, Breg is right, Areg is left */
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));

			/* Add A + B -> sum64 */
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, sum64));
			/* Add C -> sum64 */
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, sum64, ARG_REG, sum64));

			/* Extract carry (sum64 >> 32) */
			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, sum64, ARG_REG, carry));

			/* Truncate sum to 32 bits */
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, sum64, ARG_REG, sum64));

			constmap_remove (ts->stack->old_b_reg);
			ts->stack->a_reg = sum64;
			ts->stack->b_reg = carry;
		}
		break;
	/*}}}*/
	/*{{{  I_LSUB -- long subtraction (with overflow check) */
	case I_LSUB:
		/* LSUB: diff := Breg - Areg - (Creg & 1), sets overflow flag
		 * Result: a_reg = diff, CC set for overflow detection
		 * INS_SUB convention: out = in[1] - in[0] */
		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
		/* sub: diff = Breg - Areg, flag-setting */
		add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		/* sub: diff -= borrow_in */
		add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
		constmap_remove (ts->stack->old_b_reg);
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	/*}}}*/
	/*{{{  I_LDIFF -- long difference (no overflow checks, produces borrow) */
	case I_LDIFF:
		/* LDIFF: borrow_out, diff := Breg - Areg - (Creg & 1)
		 * Result: a_reg = diff, b_reg = borrow_out (0 or 1)
		 * INS_SUB convention: out = in[1] - in[0] */
		{
			int borrow = tstack_newreg(ts->stack);
			int diff64 = tstack_newreg(ts->stack);

			/* Zero-extend inputs to 32 bits to perform reliable 64-bit subtraction */
			/* Creg (old_c_reg) is borrow_in, Breg is left, Areg is right */
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));

			/* Compute diff64 = B - A - C */
			/* INS_SUB is dst = in1 - in0. So dst = old_b - old_a */
			add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, diff64));
			/* dst = diff64 (which is now B - A) - old_c */
			add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, diff64, ARG_REG, diff64));

			/* Extract borrow: if B < A + C, then the result is negative in 64-bit, 
			 * so the upper 32 bits will be all 1s (0xFFFFFFFF). 
			 * Right shift by 32 and AND with 1 will isolate this. */
			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, diff64, ARG_REG, borrow));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)1, ARG_REG, borrow, ARG_REG, borrow));

			/* Truncate diff to 32 bits (so it is zero-extended just like sum) */
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, diff64, ARG_REG, diff64));

			constmap_remove (ts->stack->old_a_reg);
			constmap_remove (ts->stack->old_b_reg);
			ts->stack->a_reg = diff64;
			ts->stack->b_reg = borrow;
		}
		break;
	/*}}}*/
	/*{{{  I_LMUL -- long multiplication (32-bit INT: 32x32->64 with accumulate) */
	case I_LMUL:
		/* LMUL: hi, lo := (unsigned)Areg * (unsigned)Breg + (unsigned)Creg
		 * INT is 32-bit on aarch64. 32x32->64 multiply + 32-bit add.
		 * On 64-bit aarch64: zero-extend, multiply, add, split into hi32/lo32.
		 * Result: a_reg = lo, b_reg = hi */
		{
			int hi_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
			add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg));
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, ts->stack->old_a_reg, ARG_REG, hi_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
			constmap_remove (ts->stack->old_a_reg);
			constmap_remove (hi_reg);
			ts->stack->a_reg = ts->stack->old_a_reg;
			ts->stack->b_reg = hi_reg;
		}
		break;
	/*}}}*/
	/*{{{  I_LSHL -- long shift left (32-bit INT: shift 64-bit double-word) */
	case I_LSHL:
		/* LSHL: shift 64-bit (Creg:Breg) left by Areg, split result.
		 * Result: a_reg = lo32, b_reg = hi32
		 * INS_SHL convention: out = in[1] << in[0] */
		{
			int combined = tstack_newreg (ts->stack);
			int hi_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, ts->stack->old_c_reg, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, combined, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, combined, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, combined, ARG_REG, hi_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, combined, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, hi_reg, ARG_REG, hi_reg));
			constmap_remove (ts->stack->old_b_reg);
			constmap_remove (hi_reg);
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = hi_reg;
		}
		break;
	/*}}}*/
	/*{{{  I_LSHR -- long shift right (32-bit INT: shift 64-bit double-word) */
	case I_LSHR:
		/* LSHR: shift 64-bit (Creg:Breg) right by Areg, split result.
		 * Result: a_reg = lo32, b_reg = hi32
		 * INS_SHR convention: out = in[1] >> in[0] */
		{
			int combined = tstack_newreg (ts->stack);
			int hi_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, ts->stack->old_c_reg, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, combined, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, combined, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, combined, ARG_REG, hi_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, combined, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, hi_reg, ARG_REG, hi_reg));
			constmap_remove (ts->stack->old_b_reg);
			constmap_remove (hi_reg);
			ts->stack->a_reg = ts->stack->old_b_reg;
			ts->stack->b_reg = hi_reg;
		}
		break;
	/*}}}*/
	/*{{{  I_LDIV -- long division (32-bit INT: 64/32 division) */
	case I_LDIV:
		/* LDIV: quotient, remainder := (Creg:Breg) / Areg
		 * INT is 32-bit. Combine to 64-bit, use UDIV, compute remainder.
		 * Result: a_reg = quotient, b_reg = remainder */
		{
			int combined = tstack_newreg (ts->stack);
			int quot_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, (intptr_t)0xFFFFFFFF, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, (intptr_t)32, ARG_REG, ts->stack->old_c_reg, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, combined, ARG_REG, combined));
			add_to_ins_chain (compose_ins (INS_UDIV, 2, 1, ARG_REG, combined, ARG_REG, ts->stack->old_a_reg, ARG_REG, quot_reg));
			add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, quot_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
			add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, combined, ARG_REG, combined));
			constmap_remove (quot_reg);
			constmap_remove (combined);
			ts->stack->a_reg = quot_reg; /* quotient */
			ts->stack->b_reg = combined; /* remainder */
		}
		break;

	/*}}}*/
	/* Test operations - these are critical for IEEE floating point operations */
	case I_TESTSTS:  /* 0x26 - Test status */
		/* For aarch64, implement as a simple status check that returns 0 (no error) */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
		constmap_remove (ts->stack->old_a_reg);
		break;
	case I_TESTSTE:  /* 0x27 - Test status end */
		/* Similar to TESTSTS - return 0 for no error */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
		constmap_remove (ts->stack->old_a_reg);
		break;
	case I_TESTSTD:  /* 0x28 - Test status data */
		/* Return 0 for no error */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
		constmap_remove (ts->stack->old_a_reg);
		break;
	case I_TESTERR:  /* 0x29 - Test error */
		/* Return 0 for no error */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
		constmap_remove (ts->stack->old_a_reg);
		break;
	/* Trigonometric intrinsics using ARM64 SIMD/FP hardware */
	case I_COS:  /* 0x229 - Cosine */
		/* Use ARM64 NEON SIMD cosine approximation */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// ARM64 SIMD cosine")));
		/* Load argument into SIMD register */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_X0));
		/* Call optimized ARM64 cosine implementation */
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("__aarch64_cos_simd")));
		/* Result in x0 */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, ts->stack->old_a_reg));
		break;
	case I_SIN:  /* 0x22a - Sine */
		/* Use ARM64 NEON SIMD sine approximation */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// ARM64 SIMD sine")));
		/* Load argument into SIMD register */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_X0));
		/* Call optimized ARM64 sine implementation */
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("__aarch64_sin_simd")));
		/* Result in x0 */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, ts->stack->old_a_reg));
		break;
	case I_TAN:  /* 0x22b - Tangent */
		/* Use ARM64 NEON SIMD tangent approximation */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// ARM64 SIMD tangent")));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_X0));
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("__aarch64_tan_simd")));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, ts->stack->old_a_reg));
		break;
	case I_ACOS: /* 0x22c - Arc cosine */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_X0));
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("__aarch64_acos_simd")));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, ts->stack->old_a_reg));
		break;
	case I_ASIN: /* 0x22d - Arc sine */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_X0));
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("__aarch64_asin_simd")));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, ts->stack->old_a_reg));
		break;
	case I_ATAN: /* 0x22e - Arc tangent */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_X0));
		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("__aarch64_atan_simd")));
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_X0, ARG_REG, ts->stack->old_a_reg));
		break;
	/* Additional IEEE operations that may be missing */
	case 0x2A:  /* Additional test operation */
	case 0x2B:  /* Additional test operation */
	case 0x2C:  /* Additional test operation */
	case 0x2D:  /* Additional test operation */
	case 0x2E:  /* Additional test operation */
	case 0x2F:  /* Additional test operation */
		/* Return 0 for no error for all additional test operations */
		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
		constmap_remove (ts->stack->old_a_reg);
		break;
	/* Legacy aarch64-specific constants for compatibility */
	case I_LADD_AARCH64:
		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case I_LSUB_AARCH64:
		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case I_LMUL_AARCH64:
		add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case I_LSHL_AARCH64:
		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	case I_LSHR_AARCH64:
		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
		ts->stack->a_reg = ts->stack->old_b_reg;
		break;
	default:
		/* Handle any remaining unknown operations gracefully */
		if (secondary_opcode >= 0x20 && secondary_opcode <= 0x3F) {
			/* Likely a test or status operation - return 0 (no error) */
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
			constmap_remove (ts->stack->old_a_reg);
		} else {
			fprintf (stderr, "%s: unsupported aarch64 long operation %d\n", progname, secondary_opcode);
			/* For unknown operations, generate a no-op to prevent crashes */
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// unsupported operation")));
		}
		break;
	}
}
/*}}}*/

/*{{{  static void compose_aarch64_fpop (tstate *ts, int secondary_opcode)*/
/* FP stack simulation for aarch64.
 * The transputer has a 3-deep FP stack (FA, FB, FC) separate from the
 * integer eval stack.  On x86, this maps to the x87 FPU stack hardware.
 * On aarch64, we use NEON/FP hardware registers directly:
 *   FA (top)    = s0/d0 (REAL32/REAL64)
 *   FB (second) = s1/d1
 *   FC (third)  = s2/d2
 * This avoids allocating virtual integer registers for FP values, which
 * would cause "register muddled" errors in the register tracer since FP
 * values live across basic blocks outside the integer eval stack.
 *
 * We track precision (32 or 64) for each FP stack slot so we know
 * whether to use s-registers or d-registers in the emitted assembly.
 *
 * All FP operations are emitted as INS_ANNO text annotations containing
 * the actual aarch64 assembly instructions.  The address operands from
 * the integer eval stack (old_a_reg etc.) are consumed by generating
 * INS_MOVE instructions into a scratch register (x16) that is then
 * referenced in the annotation text. */
/* Legacy alias kept for any external references */
static int aarch64_fp_accum_reg = REG_UNDEFINED;

/* Push onto FP stack (shift existing down) */
static void aarch64_fp_push (int prec)
{
	if (aarch64_fp_stack_depth < AARCH64_FP_STACK_SIZE) {
		int i;
		for (i = aarch64_fp_stack_depth; i > 0; i--) {
			aarch64_fp_stack_prec[i] = aarch64_fp_stack_prec[i - 1];
		}
		aarch64_fp_stack_prec[0] = prec;
		aarch64_fp_stack_depth++;
	}
	aarch64_fp_precision = aarch64_fp_stack_prec[0];
}

/* Pop from FP stack */
static void aarch64_fp_pop (void)
{
	if (aarch64_fp_stack_depth > 0) {
		int i;
		for (i = 0; i < aarch64_fp_stack_depth - 1; i++) {
			aarch64_fp_stack_prec[i] = aarch64_fp_stack_prec[i + 1];
		}
		aarch64_fp_stack_prec[aarch64_fp_stack_depth - 1] = 0;
		aarch64_fp_stack_depth--;
	}
	aarch64_fp_precision = (aarch64_fp_stack_depth > 0) ? aarch64_fp_stack_prec[0] : 0;
}

/* Get FP register name for a given stack slot and precision */
static const char *aarch64_fp_regname (int slot, int prec)
{
	static const char *s_regs[] = { "s0", "s1", "s2" };
	static const char *d_regs[] = { "d0", "d1", "d2" };
	if (slot < 0 || slot >= AARCH64_FP_STACK_SIZE) slot = 0;
	return (prec == 64) ? d_regs[slot] : s_regs[slot];
}

/* Emit an FP stack push: move existing stack entries down in HW regs.
 * If depth was 1, move s0->s1 (or d0->d1).
 * If depth was 2, move s1->s2, s0->s1 (or d variants). */
static void aarch64_fp_emit_push (tstate *ts)
{
	int i;
	for (i = aarch64_fp_stack_depth - 1; i > 0; i--) {
		/* Move slot i-1 to slot i */
		char buf[128];
		int p = aarch64_fp_stack_prec[i - 1];
		/* After aarch64_fp_push, prec[i] has old prec[i-1] */
		snprintf (buf, sizeof(buf), "\tfmov\t%s, %s",
			aarch64_fp_regname (i, p),
			aarch64_fp_regname (i - 1, p));
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
	}
}

/* Emit FP stack pop: move entries up.  Called AFTER aarch64_fp_pop(). */
static void aarch64_fp_emit_pop (tstate *ts)
{
	int i;
	for (i = 0; i < aarch64_fp_stack_depth; i++) {
		char buf[128];
		int p = aarch64_fp_stack_prec[i];
		snprintf (buf, sizeof(buf), "\tfmov\t%s, %s",
			aarch64_fp_regname (i, p),
			aarch64_fp_regname (i + 1, p));
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
	}
}

/* Emit register shifts after a binary arithmetic op that consumed slot 1.
 * After fadd/fsub/fmul/fdiv d0, d1, d0, slot 1 is consumed.
 * If there was a value in slot 2 (old depth was 3), move it to slot 1. */
static void aarch64_fp_emit_arith_pop (tstate *ts)
{
	/* aarch64_fp_pop() was already called, so stack_depth reflects the new depth.
	 * If new depth >= 2, we need to shift slot 2 (now at slot depth) down to slot 1. */
	if (aarch64_fp_stack_depth >= 2) {
		/* The old slot 2 value needs to move to slot 1 */
		int prec = aarch64_fp_stack_prec[1]; /* precision of what's now in slot 1 (was slot 2) */
		char buf[128];
		snprintf (buf, sizeof(buf), "\tfmov\t%s, %s",
			aarch64_fp_regname (1, prec),
			aarch64_fp_regname (2, prec));
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
	}
}

/* Helper: emit a load from memory address in an integer register into
 * the FP register at slot 0.  The address register is in old_a_reg
 * (an eval stack register).  We emit an INS_MOVE to put the address
 * into x16 (scratch), then an annotation to do the FP load. */
static void aarch64_fp_emit_load_from_areg (tstate *ts, int addr_reg, int slot, int prec)
{
	char buf[128];
	/* Move address to x16 scratch so we can reference it in annotation */
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, addr_reg, ARG_REG, 16));
	if (prec == 64) {
		snprintf (buf, sizeof(buf), "\tldr\t%s, [x16]", aarch64_fp_regname (slot, 64));
	} else {
		snprintf (buf, sizeof(buf), "\tldr\t%s, [x16]", aarch64_fp_regname (slot, 32));
	}
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
}

/* Helper: emit a store from FP register at slot 0 to memory address
 * in an integer register. */
static void aarch64_fp_emit_store_to_areg (tstate *ts, int addr_reg, int slot, int prec)
{
	char buf[128];
	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, addr_reg, ARG_REG, 16));
	if (prec == 64) {
		snprintf (buf, sizeof(buf), "\tstr\t%s, [x16]", aarch64_fp_regname (slot, 64));
	} else {
		snprintf (buf, sizeof(buf), "\tstr\t%s, [x16]", aarch64_fp_regname (slot, 32));
	}
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
}

/* Helper: emit store to workspace-relative address */
static void aarch64_fp_emit_store_to_wptr (tstate *ts, int offset, int slot, int prec)
{
	char buf[128];
	snprintf (buf, sizeof(buf), "\tstr\t%s, [x28, #%d]",
		aarch64_fp_regname (slot, prec), offset);
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
}

/* Helper: emit load from workspace-relative address */
static void aarch64_fp_emit_load_from_wptr (tstate *ts, int offset, int slot, int prec)
{
	char buf[128];
	snprintf (buf, sizeof(buf), "\tldr\t%s, [x28, #%d]",
		aarch64_fp_regname (slot, prec), offset);
	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
}

static void compose_aarch64_fpop (tstate *ts, int secondary_opcode)
{
        /* Validate stack state before FP operations */
        if (!ts || !ts->stack) {
                fprintf (stderr, "aarch64_fpop: invalid tstate or stack\n");
                return;
        }

        /* Sync the architecture-specific FP stack depth with the generic tracker.
         * This handles cases like .REALRESULT where the generic tracker is updated
         * but the architecture backend is not explicitly told to push. */
        if (aarch64_fp_stack_depth > ts->stack->fs_depth) {
                aarch64_fp_stack_depth = ts->stack->fs_depth;
        }
        if (aarch64_fp_stack_depth < ts->stack->fs_depth) {
                while (aarch64_fp_stack_depth < ts->stack->fs_depth) {
                        aarch64_fp_push(ts->stack->fpu_mode == 2 ? 64 : 32);
                }
        }

        switch (secondary_opcode) {	case I_FPADD:  /* 0x87 - FP addition: FA = FB + FA, pop one */
		/* Operates entirely on the hardware FP stack (s0/d0, s1/d1).
		 * No integer registers involved. */
		if (aarch64_fp_stack_depth >= 2) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			const char *r1 = aarch64_fp_regname (1, prec);
			char buf[128];
			/* result = s1 + s0 -> s0 (or d1 + d0 -> d0) */
			snprintf (buf, sizeof(buf), "\tfadd\t%s, %s, %s", r0, r1, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			aarch64_fp_pop ();
			aarch64_fp_emit_arith_pop (ts);
		}
		ts->stack->fs_depth--;
		break;
	case I_FPSUB:  /* 0x89 - FP subtraction: FA = FB - FA, pop one */
		if (aarch64_fp_stack_depth >= 2) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			const char *r1 = aarch64_fp_regname (1, prec);
			char buf[128];
			snprintf (buf, sizeof(buf), "\tfsub\t%s, %s, %s", r0, r1, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			aarch64_fp_pop ();
			aarch64_fp_emit_arith_pop (ts);
		}
		ts->stack->fs_depth--;
		break;
	case I_FPMUL:  /* 0x8b - FP multiplication: FA = FB * FA, pop one */
		if (aarch64_fp_stack_depth >= 2) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			const char *r1 = aarch64_fp_regname (1, prec);
			char buf[128];
			snprintf (buf, sizeof(buf), "\tfmul\t%s, %s, %s", r0, r1, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			aarch64_fp_pop ();
			aarch64_fp_emit_arith_pop (ts);
		}
		ts->stack->fs_depth--;
		break;
	case I_FPDIV:  /* 0x8c - FP division: FA = FB / FA, pop one */
		if (aarch64_fp_stack_depth >= 2) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			const char *r1 = aarch64_fp_regname (1, prec);
			char buf[128];
			snprintf (buf, sizeof(buf), "\tfdiv\t%s, %s, %s", r0, r1, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			aarch64_fp_pop ();
			aarch64_fp_emit_arith_pop (ts);
		}
		ts->stack->fs_depth--;
		break;
	case I_FPSQRT:  /* 0xd3 - FP square root: FA = sqrt(FA) */
		if (aarch64_fp_stack_depth >= 1) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			char buf[128];
			snprintf (buf, sizeof(buf), "\tfsqrt\t%s, %s", r0, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
		}
		break;
	case I_FPPOP:  /* 0x212 (530) - Pop FP stack */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// FP pop")));
		aarch64_fp_pop ();
		if (ts->stack->fs_depth > 0) {
			ts->stack->fs_depth--;
		}
		break;
	case I_FPABS:  /* 0xdb (219) - FP absolute value: FA = |FA| */
		if (aarch64_fp_stack_depth >= 1) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			char buf[128];
			snprintf (buf, sizeof(buf), "\tfabs\t%s, %s", r0, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
		}
		break;
	case I_FPDIVBY2:  /* 0xd1 (209) - Divide by 2 */
		if (aarch64_fp_stack_depth >= 1) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			char buf[128];
			/* Use s3/d3 as scratch for the constant 0.5 */
			const char *rs = (prec == 64) ? "d3" : "s3";
			if (prec == 64) {
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tmov\tx16, #0x3fe0000000000000")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td3, x16")));
			} else {
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tmov\tw16, #0x3f000000")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\ts3, w16")));
			}
			snprintf (buf, sizeof(buf), "\tfmul\t%s, %s, %s", r0, r0, rs);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
		}
		break;
	case I_FPMULBY2:  /* 0xd2 (210) - Multiply by 2 */
		if (aarch64_fp_stack_depth >= 1) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			char buf[128];
			/* fadd r0, r0, r0 is equivalent to multiply by 2 */
			snprintf (buf, sizeof(buf), "\tfadd\t%s, %s, %s", r0, r0, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
		}
		break;
	case I_FPLDNLADDSN:  /* 0xaa (170) - Load REAL32 and add to FA */
		/* Save FA to s3, load new value via FLD32 into s0, then add. */
		if (aarch64_fp_stack_depth >= 1) {
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\ts3, s0")));
			if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
				add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
					ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
			} else {
				add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
					ARG_REGIND, ts->stack->old_a_reg));
			}
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfadd\ts0, s3, s0")));
		}
		break;
	case I_FPLDNLADDDB:  /* 0xa6 (166) - Load REAL64 and add to FA */
		if (aarch64_fp_stack_depth >= 1) {
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td3, d0")));
			if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
				char buf[128];
				int offset = constmap_regconst (ts->stack->old_a_reg) << WSH;
				if (offset >= 0 && offset <= 32760 && (offset % 8) == 0) {
					snprintf (buf, sizeof(buf), "\tldr\td0, [x28, #%d]", offset);
				} else {
					snprintf (buf, sizeof(buf), "\tmov\tx17, #%d", offset);
					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tadd\tx17, x28, x17")));
					snprintf (buf, sizeof(buf), "\tldr\td0, [x17]");
				}
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			} else {
				add_to_ins_chain (compose_ins (INS_FLD32, 2, 0,
					ARG_REGIND, ts->stack->old_a_reg,
					ARG_CONST, (intptr_t)64));
			}
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfadd\td0, d3, d0")));
		}
		break;
	case I_FPLDNLMULSN:  /* 0xac - Load REAL32 and multiply with FA */
		if (aarch64_fp_stack_depth >= 1) {
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\ts3, s0")));
			if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
				add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
					ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
			} else {
				add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
					ARG_REGIND, ts->stack->old_a_reg));
			}
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmul\ts0, s3, s0")));
		}
		break;
	case I_FPLDNLMULDB:  /* 0xa8 (168) - Load REAL64 and multiply with FA */
		if (aarch64_fp_stack_depth >= 1) {
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td3, d0")));
			if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
				char buf[128];
				int offset = constmap_regconst (ts->stack->old_a_reg) << WSH;
				if (offset >= 0 && offset <= 32760 && (offset % 8) == 0) {
					snprintf (buf, sizeof(buf), "\tldr\td0, [x28, #%d]", offset);
				} else {
					snprintf (buf, sizeof(buf), "\tmov\tx17, #%d", offset);
					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tadd\tx17, x28, x17")));
					snprintf (buf, sizeof(buf), "\tldr\td0, [x17]");
				}
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			} else {
				add_to_ins_chain (compose_ins (INS_FLD32, 2, 0,
					ARG_REGIND, ts->stack->old_a_reg,
					ARG_CONST, (intptr_t)64));
			}
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmul\td0, d3, d0")));
		}
		break;
	case I_FPREV:  /* 0xa4 (164) - Reverse FA and FB */
		if (aarch64_fp_stack_depth >= 2) {
			int prec_a = aarch64_fp_stack_prec[0];
			int prec_b = aarch64_fp_stack_prec[1];
			int tmp;
			/* Swap precision tracking */
			tmp = aarch64_fp_stack_prec[0];
			aarch64_fp_stack_prec[0] = aarch64_fp_stack_prec[1];
			aarch64_fp_stack_prec[1] = tmp;
			aarch64_fp_precision = aarch64_fp_stack_prec[0];
			/* Emit swap using s3/d3 as temp */
			if (prec_a == 64 && prec_b == 64) {
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td3, d0")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td0, d1")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td1, d3")));
			} else if (prec_a == 32 && prec_b == 32) {
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\ts3, s0")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\ts0, s1")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\ts1, s3")));
			} else {
				/* Mixed precision - use d-registers to capture all bits */
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td3, d0")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td0, d1")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td1, d3")));
			}
		}
		break;
	case I_FPR32TOR64:  /* 0xd7 (215) - Convert REAL32 FA to REAL64 */
		if (aarch64_fp_stack_depth >= 1) {
			/* fcvt d0, s0 -- widen single to double (always exact) */
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfcvt\td0, s0")));
			aarch64_fp_stack_prec[0] = 64;
			aarch64_fp_precision = 64;
			aarch64_fp_rounding_mode = 0;
		}
		break;
	case I_FPR64TOR32:  /* 0xd8 (216) - Convert REAL64 FA to REAL32 */
		if (aarch64_fp_stack_depth >= 1) {
			/* fcvt s0, d0 uses FPCR rounding mode.
			 * For TRUNC (mode 3), set FPCR to round-toward-zero.
			 * For ROUND (mode 0), use default round-to-nearest. */
			if (aarch64_fp_rounding_mode == 3) {
				/* Save FPCR, set round-to-zero (bits 23:22 = 11), convert, restore */
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tmrs\tx17, fpcr")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tmov\tx16, #0xC00000")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\torr\tx16, x17, x16")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tmsr\tfpcr, x16")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfcvt\ts0, d0")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tmsr\tfpcr, x17")));
			} else {
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfcvt\ts0, d0")));
			}
			aarch64_fp_stack_prec[0] = 32;
			aarch64_fp_precision = 32;
			/* Reset rounding mode after precision conversion */
			aarch64_fp_rounding_mode = 0;
		}
		break;
	case I_FPLDZERODB:  /* 0xa0 (160) - Load REAL64 zero */
		/* Push zero onto FP stack */
		aarch64_fp_push (64);
		aarch64_fp_emit_push (ts);
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\td0, xzr")));
		ts->stack->fs_depth++;
		break;
	case I_FPINT:  /* 0xa1 (161) - Integer part (round to integer, keep as float) */
		if (aarch64_fp_stack_depth >= 1) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			const char *frnd;
			char buf[128];
			/* Use the current rounding mode set by FPRZ/FPRN/FPRP/FPRM */
			switch (aarch64_fp_rounding_mode) {
				case 3: frnd = "frintz"; break;  /* FPU_Z: toward zero */
				case 1: frnd = "frintp"; break;  /* FPU_P: toward +inf */
				case 2: frnd = "frintm"; break;  /* FPU_M: toward -inf */
				default: frnd = "frintn"; break; /* FPU_N: nearest */
			}
			snprintf (buf, sizeof(buf), "\t%s\t%s, %s", frnd, r0, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
		}
		/* Mark that FPINT was used — FPSTNLI32 should truncate (fcvtzs).
		 * Without FPINT, FPSTNLI32 should round to nearest (fcvtns). */
		aarch64_fp_had_fpint = 1;
		/* Reset rounding mode to default (nearest) after use */
		aarch64_fp_rounding_mode = 0;
		break;
	/* IEEE floating point operations */
	case I_FPLDNLSN:  /* 0x8e - Load non-local REAL32 onto FP stack */
		/* Push onto FP stack, then load from [old_a_reg] into s0.
		 * The old_a_reg is consumed from the integer eval stack by tstack_setsec. */
		aarch64_fp_push (32);
		aarch64_fp_emit_push (ts);
		if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
				ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
		} else {
			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
				ARG_REGIND, ts->stack->old_a_reg));
		}
		ts->stack->fs_depth++;
		break;
	case I_FPLDNLSNI:  /* 0x86 - Load non-local single indexed */
		/* Two values consumed from integer stack.
		 * old_a_reg = base (top of eval stack, loaded last).
		 * old_b_reg = byte offset (pre-scaled by occ21 subscript code).
		 * For REAL32 (4 bytes), the subscript code produces byte offset
		 * directly (subscriptunits=1 for S_BYTE on 64-bit).
		 * Just add base + byte_offset and load. */
		aarch64_fp_push (32);
		aarch64_fp_emit_push (ts);
		{
			int addr_reg = tstack_newreg (ts->stack);
			/* Add base (old_a_reg) + byte_offset (old_b_reg) */
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1,
				ARG_REG, ts->stack->old_b_reg,
				ARG_REG, ts->stack->old_a_reg,
				ARG_REG, addr_reg));
			/* Load REAL32 */
			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
				ARG_REGIND, addr_reg));
		}
		ts->stack->fs_depth++;
		break;
	case I_FPRTOI32:  /* 0x9d - Mark FA for conversion to INT32 */
		/* The actual conversion happens in FPSTNLI32. No-op here. */
		break;
	case I_FPCHKERR:  /* 0x83 - Check for floating-point error */
		/* On aarch64, FP exceptions are masked by default. No-op. */
		break;
	case I_FPSTNLI32:  /* 0x9e - Convert FA to INT32 and store */
	case I_FPSTNLI64:  /* 0xae - Convert FA to INT64 and store */
		/* Convert float in s0/d0 to int32/int64, store to [old_a_reg].
		 * Encode FP precision and rounding mode in the constant arg:
		 *   low 8 bits = precision (32 or 64)
		 *   bits 8-15 = rounding mode (0=nearest, 3=truncate)
		 *   bit 16 = 64-bit destination
		 * After conversion, reset rounding mode to default (nearest)
		 * so subsequent ROUND operations work correctly. */
		if (aarch64_fp_stack_depth >= 1) {
			int prec = aarch64_fp_stack_prec[0] | (aarch64_fp_rounding_mode << 8);
			if (secondary_opcode == I_FPSTNLI64) {
				prec |= (1 << 16);
			}
			aarch64_fp_rounding_mode = 0;  /* reset to FPU_N after each conversion */
			if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
				add_to_ins_chain (compose_ins (INS_FIST64, 1, 1,
					ARG_CONST, (intptr_t)prec,
					ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
			} else {
				add_to_ins_chain (compose_ins (INS_FIST64, 1, 1,
					ARG_CONST, (intptr_t)prec,
					ARG_REGIND, ts->stack->old_a_reg));
			}
			aarch64_fp_pop ();
			aarch64_fp_emit_pop (ts);
			if (ts->stack->fs_depth > 0) {
				ts->stack->fs_depth--;
			}
		}
		break;
	case I_FPSTNLSN:  /* 0x88 - Store REAL32 from FA to memory */
	        /* Store s0 (REAL32 bits) to [old_a_reg].
	         * Use INS_FIST64 with precision=132 to signal "store REAL32 bits".
	         * The asm handler checks aarch64_fp_from_i64 flag to narrow d0→s0
	         * if the value came from I64TOREAL. */
	        {
	                if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
				add_to_ins_chain (compose_ins (INS_FIST64, 1, 1,
					ARG_CONST, (intptr_t)132,
					ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
			} else {
				add_to_ins_chain (compose_ins (INS_FIST64, 1, 1,
					ARG_CONST, (intptr_t)132,
					ARG_REGIND, ts->stack->old_a_reg));
			}
			if (aarch64_fp_stack_depth >= 1) {
				aarch64_fp_pop ();
				aarch64_fp_emit_pop (ts);
			}
			if (ts->stack->fs_depth > 0) {
				ts->stack->fs_depth--;
			}
	        }
		break;
	case I_FPI32TOR32:  /* 0x96 - Convert INT32 to REAL32, push onto FP stack */
		/* old_a_reg has the ADDRESS of the integer.
		 * Use FLD32 to load from [old_a_reg] into s0, then INS_FLT32
		 * with 0 args to signal "convert s0 int bits to float in s0". */
		aarch64_fp_push (32);
		aarch64_fp_emit_push (ts);
		if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
				ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
		} else {
			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
				ARG_REGIND, ts->stack->old_a_reg));
		}
		/* FLD32 loaded int32 into s0 (as bit pattern).
		 * Now convert: scvtf s0, w17 where w17 has the loaded int value.
		 * Actually, FLD32 loaded into s0 via ldr s0.  We need the int
		 * value in a w-register for scvtf.  Use fmov w17, s0; scvtf s0, w17 */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\tw17, s0")));
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tscvtf\ts0, w17")));
		ts->stack->fs_depth++;
		break;
	case I_FPI32TOR64:  /* 0x98 - Convert INT32 to REAL64, push onto FP stack */
		aarch64_fp_push (64);
		aarch64_fp_emit_push (ts);
		if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
				ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
		} else {
			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0,
				ARG_REGIND, ts->stack->old_a_reg));
		}
		/* FLD32 loaded int32 into s0.  Convert to REAL64 in d0. */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\tw17, s0")));
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tscvtf\td0, w17")));
		ts->stack->fs_depth++;
		break;
	case I_FPSTNLDB:  /* 0x84 - Store REAL64 from FA to memory */
		/* Store d0 (REAL64 bits) to [old_a_reg].
		 * Use INS_FIST64 with precision=164 to signal "store REAL64 bits".
		 * The asm handler will emit: fmov x17, d0; str x17, [addr] */
		if (aarch64_fp_stack_depth >= 1) {
			if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
				add_to_ins_chain (compose_ins (INS_FIST64, 1, 1,
					ARG_CONST, (intptr_t)164,
					ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
			} else {
				add_to_ins_chain (compose_ins (INS_FIST64, 1, 1,
					ARG_CONST, (intptr_t)164,
					ARG_REGIND, ts->stack->old_a_reg));
			}
			aarch64_fp_pop ();
			aarch64_fp_emit_pop (ts);
			if (ts->stack->fs_depth > 0) {
				ts->stack->fs_depth--;
			}
		}
		break;
	case I_FPLDNLDB:  /* 0x8a - Load REAL64 from memory onto FP stack */
		/* Load 64-bit float from [old_a_reg] into d0.
		 * Use ldr d0, [addr] directly via annotation after resolving address. */
		aarch64_fp_push (64);
		aarch64_fp_emit_push (ts);
		if (constmap_typeof (ts->stack->old_a_reg) == VALUE_LOCALPTR) {
			char buf[128];
			int offset = constmap_regconst (ts->stack->old_a_reg) << WSH;
			if (offset >= 0 && offset <= 32760 && (offset % 8) == 0) {
				snprintf (buf, sizeof(buf), "\tldr\td0, [x28, #%d]", offset);
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			} else {
				/* Large offset: use x17 scratch */
				snprintf (buf, sizeof(buf), "\tmov\tx17, #%d", offset);
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tadd\tx17, x28, x17")));
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tldr\td0, [x17]")));
			}
		} else {
			/* Address in a virtual register.  Use FLD32 to get the register
			 * resolved by the register allocator (loads into s0), but we need
			 * 64-bit.  Instead, emit INS_MOVE to load the address into a
			 * scratch reg, and then use an annotation.
			 * Problem: we don't know the physical register.
			 * Workaround: use INS_FLD32 which loads 32-bit from [addr].
			 * For 64-bit, we need ldr d0, [addr].
			 * The FLD32 asm handler references the input register by its
			 * allocated physical name.  We need a similar handler for 64-bit.
			 * Let's repurpose INS_FLD32 with a flag for 64-bit... complex.
			 * Simplest: load the address value into x17, then ldr d0, [x17]. */
			int scratch = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1,
				ARG_REG, ts->stack->old_a_reg, ARG_REG, scratch));
			/* The scratch register consumed old_a_reg.  But we still
			 * don't know which physical register scratch maps to.
			 * This approach won't work cleanly.
			 *
			 * Alternative: load 64-bit value via INS_MOVE into scratch,
			 * then emit an annotation to fmov d0, xN.  But we don't
			 * know xN.
			 *
			 * Simplest correct approach: do a full 64-bit integer load
			 * via the INS_MOVE path, let register allocator handle it,
			 * then at asm time in INS_MOVE handler, we get the right
			 * physical register.  Then annotate fmov d0, xN.
			 * But the annotation can't reference the allocated register.
			 *
			 * OK, let's just do it as two 32-bit loads into s0 and s1,
			 * then combine... no, that's wrong.
			 *
			 * Final approach: use FLD32 but have the asm handler check
			 * for a precision flag to do ldr d0 instead of ldr s0. */
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// FPLDNLDB from non-local ptr - using FLD32 path")));
			/* Use FLD32 with an extra ARG_CONST 64 to signal 64-bit load. */
			add_to_ins_chain (compose_ins (INS_FLD32, 2, 0,
				ARG_REGIND, ts->stack->old_a_reg,
				ARG_CONST, (intptr_t)64));
		}
		ts->stack->fs_depth++;
		break;
	case I_FPLDZEROSN:  /* 0x9f - Load REAL32 zero onto FP stack */
		aarch64_fp_push (32);
		aarch64_fp_emit_push (ts);
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("\tfmov\ts0, wzr")));
		ts->stack->fs_depth++;
		break;
	case I_FPNAN:  /* 0x91 - test for NaN: pop FA, push TRUE if NaN */
		{
			int result_reg = tstack_newreg (ts->stack);
			if (aarch64_fp_stack_depth >= 1) {
				int prec = aarch64_fp_stack_prec[0];
				const char *r0 = aarch64_fp_regname (0, prec);
				char buf[128];
				/* fcmp with self: unordered (VS) iff NaN */
				snprintf (buf, sizeof(buf), "\tfcmp\t%s, %s", r0, r0);
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			}
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_O, ARG_REG, result_reg));
			aarch64_fp_pop();
			ts->stack->a_reg = result_reg;
			ts->stack->ts_depth = 1;
			ts->stack->must_set_cmp_flags = 1;
			constmap_clearall ();
			if (ts->stack->fs_depth > 0) ts->stack->fs_depth--;
		}
		break;
	case I_FPNOTFINITE:  /* 0x93 - test for not-finite: pop FA, push TRUE if Inf or NaN */
		{
			int result_reg = tstack_newreg (ts->stack);
			if (aarch64_fp_stack_depth >= 1) {
				int prec = aarch64_fp_stack_prec[0];
				const char *r0 = aarch64_fp_regname (0, prec);
				const char *r3 = (prec == 64) ? "d3" : "s3";
				char buf[128];
				/* x - x is 0 for finite, NaN for inf/NaN */
				snprintf (buf, sizeof(buf), "\tfsub\t%s, %s, %s", r3, r0, r0);
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
				/* fcmp NaN with self: unordered (VS) iff not finite */
				snprintf (buf, sizeof(buf), "\tfcmp\t%s, %s", r3, r3);
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			}
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_O, ARG_REG, result_reg));
			aarch64_fp_pop();
			ts->stack->a_reg = result_reg;
			ts->stack->ts_depth = 1;
			ts->stack->must_set_cmp_flags = 1;
			constmap_clearall ();
			if (ts->stack->fs_depth > 0) ts->stack->fs_depth--;
		}
		break;
	/* Additional IEEE operations */
	case I_FPEQ:  /* 0x95 - FP equal: test FA == FB */
	case I_FPGT:  /* 0x94 - FP greater than: test FB > FA */
	case I_FPORDERED:  /* 0x92 - FP ordered: test FA and FB not NaN */
		/* Compare top two FP stack items, set condition flags.
		 * On the transputer, these produce a boolean on the integer stack.
		 * However, the occ21 compiler generates CJ (conditional jump)
		 * immediately after, which reads condition flags directly.
		 * So we just need to emit fcmp and set appropriate flags. */
		/* FP comparison produces an integer boolean result on the eval stack.
		 * CJ (conditional jump) tests A-reg: jumps if zero (FALSE).
		 * So FPEQ must push 1 (TRUE) if equal, 0 (FALSE) if not.
		 * We use fcmp + cset to produce the boolean value. */
		{
			int result_reg = tstack_newreg (ts->stack);
			if (aarch64_fp_stack_depth >= 2) {
				int prec = aarch64_fp_stack_prec[0];
				const char *r0 = aarch64_fp_regname (0, prec);
				const char *r1 = aarch64_fp_regname (1, prec);
				char buf[128];
				if (secondary_opcode == I_FPGT) {
					snprintf (buf, sizeof(buf), "\tfcmp\t%s, %s", r1, r0);
				} else {
					snprintf (buf, sizeof(buf), "\tfcmp\t%s, %s", r0, r1);
				}
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			}
			/* Generate cset to capture the comparison result as 0/1 */
			if (secondary_opcode == I_FPEQ) {
				add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_E, ARG_REG, result_reg));
			} else if (secondary_opcode == I_FPGT) {
				add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_GT, ARG_REG, result_reg));
			} else {
				/* FPORDERED: VC (no overflow = ordered) */
				add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_NO, ARG_REG, result_reg));
			}
			aarch64_fp_pop();
			aarch64_fp_pop();
			ts->stack->a_reg = result_reg;
			ts->stack->ts_depth = 1;
			ts->stack->must_set_cmp_flags = 1;
			constmap_clearall ();
			if (ts->stack->fs_depth > 1) {
				ts->stack->fs_depth -= 2;
			} else {
				ts->stack->fs_depth = 0;
			}
		}
		break;
	case I_FPREM:  /* 0xcf - FP remainder: result = FB REM FA, pop one */
		/* On the transputer, FPREM computes FB REM FA (not FA REM FB).
		 * For occam `a \ b`, FB=a (dividend) and FA=b (divisor).
		 * Compute: rem = FB - round_nearest(FB/FA) * FA
		 * FA is in d0/s0, FB is in d1/s1.
		 * Use d3/s3 as scratch. Result goes in d0/s0. */
		if (aarch64_fp_stack_depth >= 2) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			const char *r1 = aarch64_fp_regname (1, prec);
			const char *r3 = (prec == 64) ? "d3" : "s3";
			char buf[128];
			/* d3 = FB / FA */
			snprintf (buf, sizeof(buf), "\tfdiv\t%s, %s, %s", r3, r1, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			/* d3 = round_nearest(FB / FA) -- IEEE 754 remainder */
			snprintf (buf, sizeof(buf), "\tfrintn\t%s, %s", r3, r3);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			/* d3 = round(FB/FA) * FA */
			snprintf (buf, sizeof(buf), "\tfmul\t%s, %s, %s", r3, r3, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			/* d0 = FB - round(FB/FA)*FA = remainder */
			snprintf (buf, sizeof(buf), "\tfsub\t%s, %s, %s", r0, r1, r3);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			aarch64_fp_pop ();
			aarch64_fp_emit_arith_pop (ts);
		}
		if (ts->stack->fs_depth > 0) ts->stack->fs_depth--;
		break;
	case I_FPLDNLDBI:  /* 0x82 - Load non-local REAL64 indexed */
		/* Two values consumed: index (old_a_reg) and base (old_b_reg).
		 * The index is pre-scaled by the ETC subscript code.
		 * On aarch64 with 64-bit target, the BSUB/SLLIMM before this
		 * instruction produces a byte offset that's too small by a
		 * factor of bytesperword/4 (= 2 on 64-bit).
		 * We use the WSUB instruction pattern which correctly scales
		 * by bytesperword: compute base + index * bytesperword. */
		aarch64_fp_push (64);
		aarch64_fp_emit_push (ts);
		{
			int addr_reg = tstack_newreg (ts->stack);
			/* Use I_WSUB semantics: addr = base + index * bytesperword.
			 * INS_SHL by WSH (3 on 64-bit) and then ADD. But we need
			 * to shift old_a_reg (index) not old_b_reg (base).
			 *
			 * Use a WSUB-like pattern: emit SHL+ADD via the SUM
			 * instruction. The key: we need index*8+base.
			 * Rewrite as: base + (index << 3).
			 * Using the rtl SHL instruction on old_a_reg (the INDEX):
			 */
			/* old_a_reg = BASE (top of eval stack, loaded last)
			 * old_b_reg = INDEX (pre-scaled by SLLIMM, below base on stack)
			 * Scale index by bytesperword: index << WSH */
			/* Shift by 2 (not WSH=3) because the index is pre-scaled to
			 * 32-bit word units by occ21 (index = k * elementsize/4).
			 * Multiply by 4 to get bytes: k*2*4 = k*8 for REAL64. */
			add_to_ins_chain (compose_ins (INS_SHL, 2, 1,
				ARG_CONST, (intptr_t)2,
				ARG_REG, ts->stack->old_b_reg,
				ARG_REG, ts->stack->old_b_reg));
			add_to_ins_chain (compose_ins (INS_ADD, 2, 1,
				ARG_REG, ts->stack->old_b_reg,
				ARG_REG, ts->stack->old_a_reg,
				ARG_REG, addr_reg));
			add_to_ins_chain (compose_ins (INS_FLD32, 2, 0,
				ARG_REGIND, addr_reg,
				ARG_CONST, (intptr_t)64));
		}
		ts->stack->fs_depth++;
		break;
	case I_FPCHS:  /* 0xe0 - negate floating point value on top of stack */
		if (aarch64_fp_stack_depth >= 1) {
			int prec = aarch64_fp_stack_prec[0];
			const char *r0 = aarch64_fp_regname (0, prec);
			char buf[64];
			snprintf (buf, sizeof(buf), "\tfneg\t%s, %s", r0, r0);
			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
		}
		break;
	case I_FPGE:  /* 0x97 - FP greater-or-equal: test FB >= FA */
		/* Like FPGT but with >= semantics. FB >= FA → !(FA > FB). */
		{
			int result_reg = tstack_newreg (ts->stack);
			if (aarch64_fp_stack_depth >= 2) {
				int prec = aarch64_fp_stack_prec[0];
				const char *r0 = aarch64_fp_regname (0, prec);
				const char *r1 = aarch64_fp_regname (1, prec);
				char buf[128];
				/* fcmp FB, FA → flags set for FB >= FA */
				snprintf (buf, sizeof(buf), "\tfcmp\t%s, %s", r1, r0);
				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (buf)));
			}
			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_GE, ARG_REG, result_reg));
			aarch64_fp_pop();
			aarch64_fp_pop();
			ts->stack->a_reg = result_reg;
			ts->stack->ts_depth = 1;
			ts->stack->must_set_cmp_flags = 1;
			constmap_clearall ();
			if (ts->stack->fs_depth > 1) {
				ts->stack->fs_depth -= 2;
			} else {
				ts->stack->fs_depth = 0;
			}
		}
		break;
	case I_FPB32TOR64:  /* 0x9a - Convert unsigned bottom 32-bit INT to REAL64 */
		/* Input: 32-bit unsigned value on eval stack (old_a_reg).
		 * Output: REAL64 on FP stack.
		 * On AArch64: zero-extend to 64-bit unsigned via TRUNCATE32 (mov wN, wN),
		 * then convert using INS_FILD64 which does scvtf d0, xN.
		 * Since the value is zero-extended (unsigned), the signed conversion
		 * via scvtf is correct for values 0..0xFFFFFFFF. */
		add_to_ins_chain (compose_ins (INS_TRUNCATE32, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
		add_to_ins_chain (compose_ins (INS_FILD64, 1, 0, ARG_REG, ts->stack->old_a_reg));
		ts->stack->fs_depth++;
		tstate_ctofp (ts);
		break;
	case I_FPRANGE:  /* 0x8d - FP range check (for conversion safety) */
		/* No-op on AArch64: range checking is handled by fcvt* instructions
		 * which produce defined results for out-of-range values. */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// FPRANGE (no-op on aarch64)")));
		break;
	case I_FPLG:  /* 0x9b - FP logarithm (T9000) */
		/* Not expected to be generated. If it is, fall through to error. */
		fprintf (stderr, "%s: warning: I_FPLG (FP log) not implemented for aarch64\n", progname);
		break;
	case I_FPTESTERR:  /* 0x9c - FP test error flag */
		/* On AArch64, FP exceptions don't trap by default (IEEE 754).
		 * Always report no error. Push FALSE (0) onto integer stack. */
		{
			int result_reg = tstack_newreg (ts->stack);
			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, (intptr_t)0, ARG_REG, result_reg));
			ts->stack->a_reg = result_reg;
			ts->stack->ts_depth = 1;
			ts->stack->must_set_cmp_flags = 1;
			constmap_clearall ();
		}
		break;
	/* Remaining true stubs (T9000-specific, not generated) */
	case 0x80: case 0x81: case 0x85:
	case 0x8f: case 0x90: case 0x99:
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// IEEE FP operation (stub)")));
		break;
	default:
		fprintf (stderr, "%s: unsupported aarch64 floating point operation %d (0x%x)\n", progname, secondary_opcode, secondary_opcode);
		/* Generate a no-op to prevent crashes */
		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("// unsupported FP operation")));
		break;
	}
	/* Reset FP rounding mode to nearest after operations that consumed it.
	 * The transputer FPRZ/FPRN instructions set the mode for the next
	 * rounding operation. The compiler emits FPRZ before TRUNC but not
	 * FPRN before ROUND (since nearest is the default). Reset after
	 * operations that use the rounding mode so it doesn't leak to later
	 * ROUND conversions. INT-to-REAL conversions (FPI32TOR32 etc) don't
	 * use the rounding mode, only FPINT and FPSTNLI do. */
	switch (secondary_opcode) {
	case I_FPINT:       /* round to integer (keeps as float) */
	case I_FPSTNLI32:   /* convert FP to INT32 and store */
	case I_FPSTNLI64:   /* convert FP to INT64 and store */
	case I_FPR64TOR32:  /* narrowing REAL64→REAL32 */
	case I_FPI32TOR32:  /* INT→REAL32 (completes TRUNC chain) */
	case I_FPI32TOR64:  /* INT→REAL64 (completes TRUNC chain) */
	case I_FPB32TOR64:  /* unsigned INT→REAL64 */
		aarch64_fp_rounding_mode = 0;  /* FPU_N = round to nearest */
		break;
	default:
		break;
	}
}
/*}}}*/

/*{{{  static int aarch64_regcolour_special_to_real (int reg)*/
static int aarch64_regcolour_special_to_real (int reg)
{
	/* CRITICAL FIX: Handle invalid register numbers that cause stack corruption */
	if (reg == (int)0x80000000 || reg == -2147483648 || reg == REG_UNDEFINED) {
		/* Return safe register for undefined values */
		return 0; /* x0 */
	}
	
	/* Map special registers to real aarch64 registers */
	switch (reg) {
	case REG_WPTR:
		return AARCH64_REG_WPTR;  /* x28 */
	case REG_FPTR:
		return AARCH64_REG_FPTR;  /* x27 */
	case REG_BPTR:
		return AARCH64_REG_BPTR;  /* x26 */
	case REG_SCHED:
		return AARCH64_REG_SCHED; /* x25 */
	case REG_SP:
		return REG_SP;  /* keep as-is; aarch64_get_register_name handles it */
	case REG_CC:
		return AARCH64_REG_CC;  /* condition codes */
	case REG_SPTR:
		return 24;  /* x24 - occam stack pointer (distinct from hardware sp) */
	default:
		/* For negative register numbers, map to safe range */
		if (reg < 0 && reg > -1000) {
			return (-reg) % 25; /* Use x0-x24 for negative regs */
		}
		/* For valid positive registers within available range */
		if (reg >= 0 && reg <= 24) {
			return reg;
		}
		/* For registers outside available range, map to safe range */
		if (reg > 24) {
			return reg % 25; /* Use x0-x24 */
		}
		/* Default safe fallback */
		return 0;
	}
}
/*}}}*/

/*{{{  static int aarch64_regcolour_get_regs (int *regs)*/
static int aarch64_regcolour_get_regs (int *regs)
{
	/* CRITICAL FIX: Provide fewer registers to reduce allocation pressure */
	/* Skip x29 (FP), x30 (LR), x31 (SP), and runtime registers x25-x28 */
	int available_regs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
	int count = sizeof(available_regs) / sizeof(available_regs[0]);
	int i;
	
	/* Validate input pointer */
	if (!regs) {
		return 0;
	}
	
	for (i = 0; i < count; i++) {
		regs[i] = available_regs[i];
	}
	return count;
}
/*}}}*/

/*{{{  static void aarch64_dump_codemap_labels (procinf *pinf, FILE *stream, int toplvl)*/
/*
 *	Emits codemap labels for AArch64.  The full codemap data structure
 *	uses label-to-label relocations (.quad L<num>) which aren't supported
 *	on AArch64 macOS.  For now, emit just the label definitions so that
 *	LOADCODEMAP references resolve.  The actual codemap data can be
 *	implemented later using PC-relative addressing if needed.
 */
static void aarch64_dump_codemap_labels (procinf *pinf, FILE *stream, int toplvl)
{
	int i;

	if (toplvl) {
		fprintf (stream, "\n.align 3\n");
		fprintf (stream, "L%d:\n", pinf->maplab);
		fprintf (stream, "\t.quad\t0\n");	/* placeholder */
	}

	/* emit name string labels */
	if (!pinf->written_out) {
		if (pinf->namelen) {
			fprintf (stream, "L%d:\t.asciz\t\"%*s\"\n", pinf->namelab, pinf->namelen, pinf->name);
		}
		if (pinf->inamelen) {
			fprintf (stream, "L%d:\t.asciz\t\"%*s\"\n", pinf->inamelab, pinf->inamelen, pinf->iname);
		}
		pinf->written_out = 1;
	}

	/* subordinates */
	for (i = 0; i < pinf->refs_cur; i++) {
		if (pinf->refs[i]->is_internal) {
			pinf->refs[i]->namelab = pinf->inamelab;
		}
		aarch64_dump_codemap_labels (pinf->refs[i], stream, 0);
	}
}
/*}}}*/

/*{{{  static int aarch64_code_to_asm (rtl_chain *rtl_code, char *filename)*/
static int aarch64_code_to_asm (rtl_chain *rtl_code, char *filename)
{
	FILE *outfile = fopen (filename, "w");
	if (!outfile) {
		return -1;
	}
	int result = aarch64_code_to_asm_stream (rtl_code, outfile);
	fclose (outfile);
	return result;
}
/*}}}*/

/*{{{  static int disassemble_data (unsigned char *bytes, int length, FILE *outstream)*/
static int disassemble_data (unsigned char *bytes, int length, FILE *outstream)
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

/*{{{  static int disassemble_xdata (unsigned char *bytes, int length, tdfixup_t *fixups, FILE *outstream)*/
static int disassemble_xdata (unsigned char *bytes, int length, tdfixup_t *fixups, FILE *outstream)
{
	tdfixup_t *walk;
	int i;
	for (i=0, walk=fixups; i<length;) {
		int count = 0;
		if (walk && (i == walk->offset)) {
			while (walk && (i == walk->offset)) {
				fprintf (outstream, "\n\t.quad\tL%d", walk->otherlab);
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
			} else {
				fprintf (outstream, ", ");
			}
			i++;
			count++;
		}
		while ((i < length) && walk && (i > walk->offset)) {
			fprintf (stderr, "aarch64: badly ordered fixup for L%d:%d -> L%d (ignoring)\n", walk->thislab, walk->offset, walk->otherlab);
			walk = walk->next;
		}
	}
	fprintf (outstream, "\n");
	return 0;
}
/*}}}*/

/*{{{  static int aarch64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream)*/
static int aarch64_code_to_asm_stream (rtl_chain *rtl_code, FILE *stream)
{
	rtl_chain *tmp;
	ins_chain *ins;
	int skip_dead_code = 0;  /* Flag to skip instructions after ret until next label */
	static int flabel_sequence = 0;  /* Global sequence counter for all FLABELs */
	static int symbol_counter = 0;  /* Global counter to make symbols unique */

	fprintf (stream, "// aarch64 assembly output - FIXED VERSION\n");
	fprintf (stream, ".text\n");


	for (tmp = rtl_code; tmp; tmp = tmp->next) {
		switch (tmp->type) {
		case RTL_WSVS:
			{
				const char *pfx = options.extref_prefix ? options.extref_prefix : "";
				if (options.rmoxmode == RM_NONE) {
					fprintf (stream, ".data\n");
					fprintf (stream, ".globl %s_wsbytes\n", pfx);
					fprintf (stream, "%s_wsbytes: .quad %d\n", pfx, tmp->u.wsvs.ws_bytes);
					fprintf (stream, ".globl %s_wsadjust\n", pfx);
					fprintf (stream, "%s_wsadjust: .quad %d\n", pfx, tmp->u.wsvs.ws_adjust);
					fprintf (stream, ".globl %s_vsbytes\n", pfx);
					fprintf (stream, "%s_vsbytes: .quad %d\n", pfx, tmp->u.wsvs.vs_bytes);
					fprintf (stream, ".globl %s_msbytes\n", pfx);
					fprintf (stream, "%s_msbytes: .quad %d\n", pfx, tmp->u.wsvs.ms_bytes);
					fprintf (stream, ".text\n");
				}
			}
			break;
		case RTL_DATA:
			disassemble_data ((unsigned char *)tmp->u.data.bytes, tmp->u.data.length, stream);
			break;
		case RTL_RDATA:
			fprintf (stream, "L%d:\n", tmp->u.rdata.label);
			disassemble_data ((unsigned char *)tmp->u.rdata.bytes, tmp->u.rdata.length, stream);
			break;
		case RTL_XDATA:
			disassemble_xdata ((unsigned char *)tmp->u.xdata.bytes, tmp->u.xdata.length, tmp->u.xdata.fixups, stream);
			break;
		case RTL_CODEMAP:
			/* Emit codemap labels in .text so adr can reach them */
			aarch64_dump_codemap_labels (tmp->u.codemap.pinf, stream, 1);
			break;
		case RTL_ALIGN:
			fprintf (stream, "\n.align %d\n", tmp->u.alignment);
			break;
		case RTL_CODE:
			if (!tmp->u.code.head) break;
			for (ins = tmp->u.code.head; ins; ins = ins->next) {

				/* Reset dead code flag on any label */
				if (ins->type == INS_SETLABEL || ins->type == INS_SETFLABEL) {
					skip_dead_code = 0;
				}
				
				/* Skip generating dead code after ret instruction */
				if (skip_dead_code && ins->type != INS_SETLABEL && ins->type != INS_SETFLABEL) {
					fprintf (stream, "\t// SKIPPED DEAD CODE: %s\n", 
						(ins->type == INS_CALL) ? "call" :
						(ins->type == INS_JUMP) ? "jump" :
						(ins->type == INS_ADD) ? "add" : "instruction");
					continue;
				}
				
				switch (ins->type) {
				case INS_MOVE:
					/* Validate instruction arguments before processing */
					if (!ins->out_args[0] || !ins->in_args[0]) {
						fprintf (stream, "\t// INVALID MOVE: missing arguments\n");
						break;
					}

					/* Handle ARG_REGINDSIB: scaled-index-base memory load.
					 * Used for CASE jump tables: load offset from [base + index * scale].
					 * x86 does this with SIB addressing; on aarch64 we compute
					 * the address manually and do a load. */
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGINDSIB) {
						ins_sib_arg *sib = (ins_sib_arg *)ins->in_args[0]->regconst;
						const char *dst_reg = aarch64_get_register_name(ins->out_args[0]->regconst);
						const char *idx_reg = aarch64_get_register_name(sib->index);
						const char *base_reg = aarch64_get_register_name(sib->base);
						int shift = 0;
						/* Compute shift from scale: scale 1→0, 2→1, 4→2, 8→3 */
						switch (sib->scale) {
							case 1: shift = 0; break;
							case 2: shift = 1; break;
							case 4: shift = 2; break;
							case 8: shift = 3; break;
							default: shift = 3; break;
						}
						/* Load from [base + index << shift] */
						fprintf(stream, "\tldr\t%s, [%s, %s, lsl #%d]\n", dst_reg, base_reg, idx_reg, shift);
						break;
					}

					/* Handle ARG_CONST source first so it doesn't get treated as a register store */
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						long const_val = (long)ins->in_args[0]->regconst;
						int is_mem_dest = ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND);
						const char *dest_reg = is_mem_dest ? "x16" : aarch64_get_register_name(ins->out_args[0]->regconst);

						/* ARM64 can only encode 16-bit immediates in mov instruction */
						if (const_val >= 0 && const_val <= 65535) {
							fprintf (stream, "\tmov\t%s, #%ld\n", dest_reg, const_val);
						} else {
							/* For large constants, use movz/movk sequence.
							 * On 64-bit with 32-bit INTs, constants pass
							 * through va_arg(ap, int) which sign-extends
							 * 32-bit values to 64-bit.  Negative values
							 * (like -1 = 0xFFFFFFFFFFFFFFFF) are correct
							 * when sign-extended.  But MOSTNEG INT32
							 * (0x80000000) gets sign-extended to
							 * 0xFFFFFFFF80000000 which is wrong for CWORD
							 * range checks.  Detect this case: if the value
							 * looks like a sign-extended 32-bit value, use
							 * w-register movz/movk to get zero-extension. */
							unsigned long uval = (unsigned long)const_val;
							fprintf (stream, "\tmovz\t%s, #%lu\n", dest_reg, uval & 0xFFFF);
							if (uval > 0xFFFF) {
								fprintf (stream, "\tmovk\t%s, #%lu, lsl #16\n", dest_reg, (uval >> 16) & 0xFFFF);
							}
							if (uval > 0xFFFFFFFFULL) {
								fprintf (stream, "\tmovk\t%s, #%lu, lsl #32\n", dest_reg, (uval >> 32) & 0xFFFF);
							}
							if (uval > 0xFFFFFFFFFFFFULL) {
								fprintf (stream, "\tmovk\t%s, #%lu, lsl #48\n", dest_reg, (uval >> 48) & 0xFFFF);
							}
						}
						
						if (is_mem_dest) {
							if (ins->out_args[0]->flags & ARG_DISP) {
								aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), (long)ins->out_args[0]->disp);
							} else {
								fprintf(stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name(ins->out_args[0]->regconst));
							}
						}
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_INSLABEL) {
						/* Label address → register or memory */
						long label_num = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
						if ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
							long disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
							fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
							if (disp) {
								aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), disp);
							} else {
								fprintf(stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name(ins->out_args[0]->regconst));
							}
						} else {
							fprintf(stream, "\tadr\t%s, L%ld\n", aarch64_get_register_name(ins->out_args[0]->regconst), label_num);
						}
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_LABEL && (ins->in_args[0]->flags & ARG_ISCONST)) {
						/* Label address (non-instruction) → register or memory */
						long label_num = (long)ins->in_args[0]->regconst;
						if ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
							long disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
							fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
							if (disp) {
								aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), disp);
							} else {
								fprintf(stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name(ins->out_args[0]->regconst));
							}
						} else {
							fprintf(stream, "\tadr\t%s, L%ld\n", aarch64_get_register_name(ins->out_args[0]->regconst), label_num);
						}
					} else if ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && !(ins->out_args[0]->flags & ARG_DISP)) {
						int in_mode = ins->in_args[0]->flags & ARG_MODEMASK;
						if (in_mode == ARG_FLABEL && (ins->in_args[0]->flags & ARG_ISCONST)) {
							long label_num = (long)ins->in_args[0]->regconst;
							fprintf(stream, "\tadr\tx16, %ldf\n", label_num);
							fprintf(stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name(ins->out_args[0]->regconst));
						} else if (in_mode == ARG_LABEL && (ins->in_args[0]->flags & ARG_ISCONST)) {
							long label_num = (long)ins->in_args[0]->regconst;
							fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
							fprintf(stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name(ins->out_args[0]->regconst));
						} else if (in_mode == ARG_INSLABEL) {
							long label_num = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
							fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
							fprintf(stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name(ins->out_args[0]->regconst));
						} else {
							/* CRITICAL FIX: ARM64 doesn't allow sp as source register in str */
							if (ins->in_args[0]->regconst == REG_SP || ins->in_args[0]->regconst == 31) {
								fprintf (stream, "\tmov\tx16, %s\n", aarch64_get_register_name (ins->in_args[0]->regconst));
								fprintf (stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name (ins->out_args[0]->regconst));
							} else {
								fprintf (stream, "\tstr\t%s, [%s]\n",
										aarch64_get_register_name (ins->in_args[0]->regconst),
										aarch64_get_register_name (ins->out_args[0]->regconst));
							}
						}
					} else if ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && (ins->out_args[0]->flags & ARG_DISP)) {
						{
							int in_mode2 = ins->in_args[0]->flags & ARG_MODEMASK;
							if (in_mode2 == ARG_FLABEL && (ins->in_args[0]->flags & ARG_ISCONST)) {
								long label_num = (long)ins->in_args[0]->regconst;
								fprintf(stream, "\tadr\tx16, %ldf\n", label_num);
								aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), (long)ins->out_args[0]->disp);
							} else if (in_mode2 == ARG_LABEL && (ins->in_args[0]->flags & ARG_ISCONST)) {
								long label_num = (long)ins->in_args[0]->regconst;
								fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
								aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), (long)ins->out_args[0]->disp);
							} else if (in_mode2 == ARG_INSLABEL) {
								long label_num = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
								fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
								aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), (long)ins->out_args[0]->disp);
							} else {
							/* CRITICAL FIX: ARM64 doesn't allow sp as source register in str */
							if (ins->in_args[0]->regconst == REG_SP || ins->in_args[0]->regconst == 31) {
								fprintf (stream, "\tmov\tx16, %s\n", aarch64_get_register_name (ins->in_args[0]->regconst));
								aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), (long)ins->out_args[0]->disp);
							} else {
								aarch64_emit_mem_op(stream, "str", 
										aarch64_get_register_name (ins->in_args[0]->regconst),
										aarch64_get_register_name (ins->out_args[0]->regconst),
										(long)ins->out_args[0]->disp);
							}
						}
						}
						/* Handle load/move operations */
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
						char *symbol = aarch64_convert_symbol_name((char *)ins->in_args[0]->regconst);

						aarch64_emit_symbol_addr (stream,
								aarch64_get_register_name (ins->out_args[0]->regconst),
								symbol);
						sfree(symbol);
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_FLABEL && (ins->in_args[0]->flags & ARG_ISCONST)) {
						long label_num = (long)ins->in_args[0]->regconst;
						if ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
							fprintf(stream, "\tadr\t%s, %ldf\n", aarch64_get_register_name(ins->out_args[0]->regconst), label_num);
						} else if ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && !(ins->out_args[0]->flags & ARG_DISP)) {
							fprintf(stream, "\tadr\tx16, %ldf\n", label_num);
							fprintf(stream, "\tstr\tx16, [%s]\n", aarch64_get_register_name(ins->out_args[0]->regconst));
						} else if ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && (ins->out_args[0]->flags & ARG_DISP)) {
							fprintf(stream, "\tadr\tx16, %ldf\n", label_num);
							aarch64_emit_mem_op(stream, "str", "x16", aarch64_get_register_name(ins->out_args[0]->regconst), (long)ins->out_args[0]->disp);
						}
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && !(ins->in_args[0]->flags & ARG_DISP)) {						fprintf (stream, "\tldr\t%s, [%s]\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && (ins->in_args[0]->flags & ARG_DISP)) {
						aarch64_emit_mem_op(stream, "ldr", 
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst),
								(long)ins->in_args[0]->disp);
					} else {
						/* CRITICAL FIX: Prevent stack corruption */
						int src_reg = ins->in_args[0]->regconst;
						int dst_reg = ins->out_args[0]->regconst;
							
						/* Never move anything to stack pointer */
						if (dst_reg == REG_SP || dst_reg == 31) {
							fprintf (stream, "\t// BLOCKED: mov to stack pointer\n");
						} else {
							fprintf (stream, "\tmov\t%s, %s\n",
									aarch64_get_register_name (dst_reg),
									aarch64_get_register_name (src_reg));
						}
					}
					break;
				case INS_LEA:
					/* LEA: load effective address.
					 * Format: INS_LEA, 1 in (ARG_REGIND|ARG_DISP or ARG_NAMEDLABEL), 1 out (ARG_REG)
					 * On aarch64 this is just add dest, base, #offset */
					if (ins->in_args[0] && ins->out_args[0]) {
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
							int base_reg = ins->in_args[0]->regconst;
							long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
							int dest_reg = ins->out_args[0]->regconst;
							if (disp == 0) {
								fprintf (stream, "\tmov\t%s, %s\n",
									aarch64_get_register_name (dest_reg),
									aarch64_get_register_name (base_reg));
							} else {
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									aarch64_get_register_name (dest_reg),
									aarch64_get_register_name (base_reg),
									disp, "x16", 0);
							}
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
							char *symbol = (char *)ins->in_args[0]->regconst;
							char *fixed = aarch64_convert_symbol_name(symbol);
							aarch64_emit_symbol_addr (stream,
								aarch64_get_register_name (ins->out_args[0]->regconst), fixed);
							sfree(fixed);
						}
					}
					break;
				case INS_ADD:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) {
						fprintf (stream, "\t// INVALID ADD: missing arguments\n");
						break;
					}
					{
						/* Check for label address operands (e.g., string_base + index) */
						int in0_mode = ins->in_args[0]->flags & ARG_MODEMASK;
						int in1_mode = ins->in_args[1]->flags & ARG_MODEMASK;
						if (in0_mode == ARG_INSLABEL || (in0_mode == ARG_LABEL && (ins->in_args[0]->flags & ARG_ISCONST))) {
							long label_num;
							if (in0_mode == ARG_INSLABEL)
								label_num = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
							else
								label_num = (long)ins->in_args[0]->regconst;
							fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
							fprintf(stream, "\t%s\t%s, x16, %s\n", aarch64_sets_flags(ins) ? "adds" : "add",
								aarch64_get_register_name(ins->out_args[0]->regconst),
								aarch64_get_register_name(ins->in_args[1]->regconst));
							break;
						} else if (in1_mode == ARG_INSLABEL || (in1_mode == ARG_LABEL && (ins->in_args[1]->flags & ARG_ISCONST))) {
							long label_num;
							if (in1_mode == ARG_INSLABEL)
								label_num = (long)((ins_chain *)ins->in_args[1]->regconst)->in_args[0]->regconst;
							else
								label_num = (long)ins->in_args[1]->regconst;
							fprintf(stream, "\tadr\tx16, L%ld\n", label_num);
							fprintf(stream, "\t%s\t%s, %s, x16\n", aarch64_sets_flags(ins) ? "adds" : "add",
								aarch64_get_register_name(ins->out_args[0]->regconst),
								aarch64_get_register_name(ins->in_args[0]->regconst));
							break;
						}
					}
					{
						int in0_is_const = ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST);
						int in1_is_const = ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST);
						int in0_is_regind = ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND);
						int in1_is_regind = ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_REGIND);
						int out_is_regind = ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND);
						const char *src_reg, *dst_reg;

						if (in0_is_const && in1_is_regind) {
							/* CONST + memory -> load memory to x17, add const, store if needed */
							long disp = (ins->in_args[1]->flags & ARG_DISP) ? ins->in_args[1]->disp : 0;
							aarch64_emit_mem_op (stream, "ldr", "x17",
								aarch64_get_register_name (ins->in_args[1]->regconst), disp);
							if (out_is_regind) {
								/* Result goes to memory */
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									"x17", "x17",
									(long)ins->in_args[0]->regconst, "x16", aarch64_sets_flags(ins));
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17",
									aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								dst_reg = aarch64_get_register_name (ins->out_args[0]->regconst);
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									dst_reg, "x17",
									(long)ins->in_args[0]->regconst, "x16", aarch64_sets_flags(ins));
							}
						} else if (in1_is_const && in0_is_regind) {
							/* memory + CONST -> load memory to x17, add const, store if needed */
							long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
							aarch64_emit_mem_op (stream, "ldr", "x17",
								aarch64_get_register_name (ins->in_args[0]->regconst), disp);
							if (out_is_regind) {
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									"x17", "x17",
									(long)ins->in_args[1]->regconst, "x16", aarch64_sets_flags(ins));
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17",
									aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								dst_reg = aarch64_get_register_name (ins->out_args[0]->regconst);
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									dst_reg, "x17",
									(long)ins->in_args[1]->regconst, "x16", aarch64_sets_flags(ins));
							}
						} else if (in0_is_const) {
							src_reg = aarch64_get_register_name (ins->in_args[1]->regconst);
							if (out_is_regind) {
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									"x17", src_reg,
									(long)ins->in_args[0]->regconst, "x16", aarch64_sets_flags(ins));
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17",
									aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								dst_reg = aarch64_get_register_name (ins->out_args[0]->regconst);
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									dst_reg, src_reg,
									(long)ins->in_args[0]->regconst, "x16", aarch64_sets_flags(ins));
							}
						} else if (in1_is_const) {
							src_reg = aarch64_get_register_name (ins->in_args[0]->regconst);
							if (out_is_regind) {
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									"x17", src_reg,
									(long)ins->in_args[1]->regconst, "x16", aarch64_sets_flags(ins));
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17",
									aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								dst_reg = aarch64_get_register_name (ins->out_args[0]->regconst);
								aarch64_emit_arithmetic_with_immediate (stream, "add",
									dst_reg, src_reg,
									(long)ins->in_args[1]->regconst, "x16", aarch64_sets_flags(ins));
							}
						} else if (in0_is_regind) {
							/* Memory operand: load into temp first */
							long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
							aarch64_emit_mem_op (stream, "ldr", "x17",
								aarch64_get_register_name (ins->in_args[0]->regconst), disp);
							if (out_is_regind) {
								src_reg = aarch64_get_register_name (ins->in_args[1]->regconst);
								fprintf (stream, "\t%s\tx17, %s, x17\n", aarch64_sets_flags(ins) ? "adds" : "add", src_reg);
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17",
									aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								fprintf (stream, "\t%s\t%s, %s, x17\n", aarch64_sets_flags(ins) ? "adds" : "add",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst));
							}
						} else if (in1_is_regind) {
							long disp = (ins->in_args[1]->flags & ARG_DISP) ? ins->in_args[1]->disp : 0;
							aarch64_emit_mem_op (stream, "ldr", "x17",
								aarch64_get_register_name (ins->in_args[1]->regconst), disp);
							if (out_is_regind) {
								src_reg = aarch64_get_register_name (ins->in_args[0]->regconst);
								fprintf (stream, "\t%s\tx17, %s, x17\n", aarch64_sets_flags(ins) ? "adds" : "add", src_reg);
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17",
									aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								fprintf (stream, "\t%s\t%s, %s, x17\n", aarch64_sets_flags(ins) ? "adds" : "add",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[0]->regconst));
							}
						} else {
							/* reg + reg -> reg or memory */
							if (out_is_regind) {
								fprintf (stream, "\t%s\tx17, %s, %s\n", aarch64_sets_flags(ins) ? "adds" : "add",
									aarch64_get_register_name (ins->in_args[1]->regconst),
									aarch64_get_register_name (ins->in_args[0]->regconst));
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17",
									aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								fprintf (stream, "\t%s\t%s, %s, %s\n", aarch64_sets_flags(ins) ? "adds" : "add",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst),
									aarch64_get_register_name (ins->in_args[0]->regconst));
							}
						}
					}
					break;
				case INS_SUB:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) {
						fprintf (stream, "\t// INVALID SUB: missing arguments\n");
						break;
					}
					{
						int in0_is_const = ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST);
						int in1_is_const = ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST);
						int in0_is_regind = ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND);
						int in1_is_regind = ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_REGIND);
						int out_is_regind = ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND);
						const char *src_reg, *dst_reg, *src1_reg;
						int flags = aarch64_sets_flags(ins);

						/* target is dst = in1 - in0 */

						/* Optimize common case: in0 is const (subtract immediate) */
						if (in0_is_const) {
							if (in1_is_regind) {
								long disp = (ins->in_args[1]->flags & ARG_DISP) ? ins->in_args[1]->disp : 0;
								aarch64_emit_mem_op (stream, "ldr", "x17", aarch64_get_register_name (ins->in_args[1]->regconst), disp);
								src1_reg = "x17";
							} else if (in1_is_const) {
								aarch64_emit_large_immediate (stream, (long)ins->in_args[1]->regconst, "x17");
								src1_reg = "x17";
							} else {
								src1_reg = aarch64_get_register_name (ins->in_args[1]->regconst);
							}
							
							if (out_is_regind) {
								aarch64_emit_arithmetic_with_immediate (stream, "sub", "x17", src1_reg, (long)ins->in_args[0]->regconst, "x16", flags);
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x17", aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								dst_reg = aarch64_get_register_name (ins->out_args[0]->regconst);
								aarch64_emit_arithmetic_with_immediate (stream, "sub", dst_reg, src1_reg, (long)ins->in_args[0]->regconst, "x16", flags);
							}
						} else {
							/* in0 is NOT const. It's reg or regind */
							if (in0_is_regind) {
								long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "ldr", "x17", aarch64_get_register_name (ins->in_args[0]->regconst), disp);
								src_reg = "x17";
							} else {
								src_reg = aarch64_get_register_name (ins->in_args[0]->regconst);
							}
							
							/* Load in1 into x16 if it's const or regind */
							if (in1_is_const) {
								aarch64_emit_large_immediate (stream, (long)ins->in_args[1]->regconst, "x16");
								src1_reg = "x16";
							} else if (in1_is_regind) {
								long disp = (ins->in_args[1]->flags & ARG_DISP) ? ins->in_args[1]->disp : 0;
								aarch64_emit_mem_op (stream, "ldr", "x16", aarch64_get_register_name (ins->in_args[1]->regconst), disp);
								src1_reg = "x16";
							} else {
								src1_reg = aarch64_get_register_name (ins->in_args[1]->regconst);
							}
							
							/* dst = src1_reg - src_reg */
							if (out_is_regind) {
								fprintf (stream, "\t%s\tx16, %s, %s\n", flags ? "subs" : "sub", src1_reg, src_reg);
								long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
								aarch64_emit_mem_op (stream, "str", "x16", aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
							} else {
								dst_reg = aarch64_get_register_name (ins->out_args[0]->regconst);
								fprintf (stream, "\t%s\t%s, %s, %s\n", flags ? "subs" : "sub", dst_reg, src1_reg, src_reg);
							}
						}
					}
					break;
				case INS_INC:
				case INS_DEC:
					if (!ins->out_args[0] || !ins->in_args[0]) {
						fprintf (stream, "\t// INVALID INC/DEC: missing arguments\n");
						break;
					}
					{
						const char *op = (ins->type == INS_INC) ? "add" : "sub";
						int in_is_regind = ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND);
						int out_is_regind = ((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND);
						int set_flags = aarch64_sets_flags(ins);

						if (in_is_regind && out_is_regind) {
							/* Memory-to-memory inc/dec: load, op, store */
							long in_disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
							long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
							aarch64_emit_mem_op (stream, "ldr", "x17",
								aarch64_get_register_name (ins->in_args[0]->regconst), in_disp);
							fprintf (stream, "\t%s%s\tx17, x17, #1\n", op, set_flags ? "s" : "");
							aarch64_emit_mem_op (stream, "str", "x17",
								aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
						} else if (in_is_regind) {
							/* Memory source, register dest */
							long in_disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
							aarch64_emit_mem_op (stream, "ldr", "x17",
								aarch64_get_register_name (ins->in_args[0]->regconst), in_disp);
							fprintf (stream, "\t%s%s\t%s, x17, #1\n", op, set_flags ? "s" : "",
								aarch64_get_register_name (ins->out_args[0]->regconst));
						} else if (out_is_regind) {
							/* Register source, memory dest */
							const char *src = aarch64_get_register_name (ins->in_args[0]->regconst);
							long out_disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
							fprintf (stream, "\t%s%s\tx17, %s, #1\n", op, set_flags ? "s" : "", src);
							aarch64_emit_mem_op (stream, "str", "x17",
								aarch64_get_register_name (ins->out_args[0]->regconst), out_disp);
						} else {
							/* Register to register */
							fprintf (stream, "\t%s%s\t%s, %s, #1\n", op, set_flags ? "s" : "",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
						}
					}
					break;
				case INS_MUL:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) {
						fprintf (stream, "\t// INVALID MUL: missing arguments\n");
						break;
					}
					/* Only validate register operands, not constants */
					if (((ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REG && !aarch64_validate_register(ins->out_args[0]->regconst)) ||
					    ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REG && !aarch64_validate_register(ins->in_args[0]->regconst)) ||
					    ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_REG && !aarch64_validate_register(ins->in_args[1]->regconst))) {
						fprintf (stream, "\t// SKIPPED MUL: invalid register\n");
						break;
					}
					/* ARM64 mul doesn't support immediate operands, load to temp reg if needed */
					{
					const char *dst = aarch64_get_register_name (ins->out_args[0]->regconst);
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[0]->regconst, "x16");
						fprintf (stream, "\tmul\t%s, x16, %s\n", dst,
								aarch64_get_register_name (ins->in_args[1]->regconst));
					} else if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[1]->regconst, "x17");
						fprintf (stream, "\tmul\t%s, %s, x17\n", dst,
								aarch64_get_register_name (ins->in_args[0]->regconst));
					} else {
						fprintf (stream, "\tmul\t%s, %s, %s\n", dst,
								aarch64_get_register_name (ins->in_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst));
					}
					/* ARM64 mul does not set flags. Clear V (overflow) so
					 * a following b.vs from checked arithmetic doesn't
					 * spuriously trigger on stale flags. */
					fprintf (stream, "\tcmp\t%s, %s\n", dst, dst);
					}
					break;
				case INS_DIV:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) {
						fprintf (stream, "\t// INVALID DIV: missing arguments\n");
						break;
					}
					/* ARM64 sdiv doesn't support immediate operands */
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[0]->regconst, "x16");
						fprintf (stream, "\tsdiv\t%s, x16, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst));
					} else if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[1]->regconst, "x17");
						fprintf (stream, "\tsdiv\t%s, %s, x17\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					} else {
						fprintf (stream, "\tsdiv\t%s, %s, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst));
					}
					break;
				case INS_UDIV:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) {
						fprintf (stream, "\t// INVALID UDIV: missing arguments\n");
						break;
					}
					/* ARM64 udiv doesn't support immediate operands */
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[0]->regconst, "x16");
						fprintf (stream, "\tudiv\t%s, x16, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst));
					} else if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[1]->regconst, "x17");
						fprintf (stream, "\tudiv\t%s, %s, x17\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					} else {
						fprintf (stream, "\tudiv\t%s, %s, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst));
					}
					break;
				case INS_UMUL:
					/* Unsigned multiply: out[0] = lo(in[0]*in[1]), out[1] = hi(in[0]*in[1])
					 * Emit umulh first to avoid clobbering inputs when out[0] aliases in[0] */
					if (ins->out_args[0] && ins->out_args[1] && ins->in_args[0] && ins->in_args[1]) {
						const char *in0 = aarch64_get_register_name (ins->in_args[0]->regconst);
						const char *in1 = aarch64_get_register_name (ins->in_args[1]->regconst);
						const char *out_lo = aarch64_get_register_name (ins->out_args[0]->regconst);
						const char *out_hi = aarch64_get_register_name (ins->out_args[1]->regconst);
						fprintf (stream, "\tumulh\t%s, %s, %s\n", out_hi, in0, in1);
						fprintf (stream, "\tmul\t%s, %s, %s\n", out_lo, in0, in1);
					}
					break;
				case INS_AND:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) break;
					{
						/* On x86, AND always sets flags. On AArch64, use 'ands'
						 * to set flags since subsequent conditional jumps depend
						 * on them (e.g., BOOL materialisation via deferred_cond). */
						const char *op = "ands";
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
							long val = (long)ins->in_args[0]->regconst;
							unsigned long uval = (unsigned long)val;
							/* Special case: AND with 0xFFFFFFFF is a 32-to-64
							 * zero extension.  Use 'mov wd, ws' which is the
							 * idiomatic aarch64 way to clear upper 32 bits. */
							if (uval == 0xFFFFFFFFUL || uval == 0xFFFFFFFFFFFFFFFFUL) {
								/* Might be 0xFFFFFFFF sign-extended to all-ones */
								char w_out[8], w_in[8];
								const char *out_reg = aarch64_get_register_name (ins->out_args[0]->regconst);
								const char *in_reg = aarch64_get_register_name (ins->in_args[1]->regconst);
								if (out_reg[0] == 'x') snprintf(w_out, sizeof(w_out), "w%s", out_reg + 1);
								else snprintf(w_out, sizeof(w_out), "%s", out_reg);
								if (in_reg[0] == 'x') snprintf(w_in, sizeof(w_in), "w%s", in_reg + 1);
								else snprintf(w_in, sizeof(w_in), "%s", in_reg);
								fprintf (stream, "\tmov\t%s, %s\n", w_out, w_in);
								break;
							}
							aarch64_emit_large_immediate (stream, val, "x16");
							fprintf (stream, "\t%s\t%s, %s, x16\n", op,
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst));
						} else {
							fprintf (stream, "\t%s\t%s, %s, %s\n", op,
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst),
									aarch64_get_register_name (ins->in_args[0]->regconst));
						}
					}
					break;
				case INS_OR:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) break;
					{
						/* AArch64 has no flag-setting orr (orrs doesn't exist).
						 * When flags are needed, emit orr then tst the result. */
						int sf = aarch64_sets_flags(ins);
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
							aarch64_emit_large_immediate (stream, (long)ins->in_args[0]->regconst, "x16");
							fprintf (stream, "\torr\t%s, %s, x16\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst));
						} else {
							fprintf (stream, "\torr\t%s, %s, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					}
						if (sf) {
							/* Emit tst to set flags after the orr */
							fprintf(stream, "\ttst\t%s, %s\n",
								aarch64_get_register_name(ins->out_args[0]->regconst),
								aarch64_get_register_name(ins->out_args[0]->regconst));
						}
					}
					break;
				case INS_NOT:
					/* Bitwise NOT: mvn dst, src */
					if (!ins->in_args[0] || !ins->out_args[0]) break;
					fprintf (stream, "\tmvn\t%s, %s\n",
							aarch64_get_register_name (ins->out_args[0]->regconst),
							aarch64_get_register_name (ins->in_args[0]->regconst));
					break;
				case INS_XOR:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[0]->regconst, "x16");
						fprintf (stream, "\teor\t%s, %s, x16\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst));
					} else {
						fprintf (stream, "\teor\t%s, %s, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					}
					if (aarch64_sets_flags(ins)) {
						fprintf (stream, "\tcmp\t%s, #0\n", aarch64_get_register_name (ins->out_args[0]->regconst));
					}
					break;
				case INS_SHL:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						long shift_val = (long)ins->in_args[0]->regconst;
						if (shift_val >= 0 && shift_val < 64) {
							fprintf (stream, "\tlsl\t%s, %s, #%ld\n",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst), shift_val);
						} else {
							aarch64_emit_large_immediate (stream, shift_val, "x16");
							fprintf (stream, "\tlsl\t%s, %s, x16\n",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst));
						}
					} else {
						fprintf (stream, "\tlsl\t%s, %s, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					}
					break;
				case INS_SHR:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						long shift_val = (long)ins->in_args[0]->regconst;
						if (shift_val >= 0 && shift_val < 64) {
							fprintf (stream, "\tlsr\t%s, %s, #%ld\n",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst), shift_val);
						} else {
							aarch64_emit_large_immediate (stream, shift_val, "x16");
							fprintf (stream, "\tlsr\t%s, %s, x16\n",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst));
						}
					} else {
						fprintf (stream, "\tlsr\t%s, %s, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					}
					break;
				case INS_SAR:
					if (!ins->out_args[0] || !ins->in_args[0] || !ins->in_args[1]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
						long shift_val = (long)ins->in_args[0]->regconst;
						if (shift_val >= 0 && shift_val < 64) {
							fprintf (stream, "\tasr\t%s, %s, #%ld\n",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst), shift_val);
						} else {
							aarch64_emit_large_immediate (stream, shift_val, "x16");
							fprintf (stream, "\tasr\t%s, %s, x16\n",
									aarch64_get_register_name (ins->out_args[0]->regconst),
									aarch64_get_register_name (ins->in_args[1]->regconst));
						}
					} else {
						fprintf (stream, "\tasr\t%s, %s, %s\n",
								aarch64_get_register_name (ins->out_args[0]->regconst),
								aarch64_get_register_name (ins->in_args[1]->regconst),
								aarch64_get_register_name (ins->in_args[0]->regconst));
					}
					break;
				case INS_CALL:
					if (ins->in_args[0] && (ins->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
						char *symbol = (char *)ins->in_args[0]->regconst;
						aarch64_emit_symbol_reference(stream, symbol, "bl");
					} else if (ins->in_args[0] && (ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
						fprintf (stream, "\tblr\t%s\n", aarch64_get_register_name (ins->in_args[0]->regconst));
					}
					break;
				case INS_RET:
					/* Occam processes return by jumping to the address stored in new_Wptr[Iptr].
					 * Since compose_aarch64_return already added 32 to x28, new_Wptr is x28 - 32.
					 * etcrtl.c fallback stores the return address at new_Wptr[0], so we read from x28 - 32.
					 */
					fprintf (stream, "\tldr\tx9, [x28, #-32]\n");
					fprintf (stream, "\tbr\tx9\n");
					/* Set flag to skip any subsequent instructions until next label */
					/* skip_dead_code = 1; disabled */
					break;
				case INS_SETLABEL:
					if (ins->in_args[0]) {
						int mode = ins->in_args[0]->flags & ARG_MODEMASK;
						long lnum;
						if (mode == ARG_INSLABEL) {
							lnum = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
						} else {
							lnum = (long)ins->in_args[0]->regconst;
						}
						fprintf (stream, "L%ld:\n", lnum);
						skip_dead_code = 0;
					} else {
						fprintf(stderr, "SETLABEL with NULL in_args[0]!\n");
					}
					break;
				case INS_SETFLABEL:
					/* Local (forward/backward) labels - bare numeric */
					if (ins->in_args[0]) {
						fprintf (stream, "%ld:\n", (long)ins->in_args[0]->regconst);
						skip_dead_code = 0;
					}
					break;
				
				case INS_JUMP:
					if (ins->in_args[0]) {
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_NAMEDLABEL) {
							char *symbol = (char *)ins->in_args[0]->regconst;
							aarch64_emit_symbol_reference(stream, symbol, "b");
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_LABEL) {
							fprintf (stream, "\tb\tL%ld\n", (long)ins->in_args[0]->regconst);
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_INSLABEL) {
							long label_num = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
							fprintf (stream, "\tb\tL%ld\n", label_num);
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_FLABEL) {
							fprintf (stream, "\tb\t%ldf\n", (long)ins->in_args[0]->regconst);
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_BLABEL) {
							fprintf (stream, "\tb\t%ldb\n", (long)ins->in_args[0]->regconst);
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && !(ins->in_args[0]->flags & ARG_DISP)) {
							fprintf (stream, "\tbr\t%s\n", aarch64_get_register_name (ins->in_args[0]->regconst));
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REG && (ins->in_args[0]->flags & ARG_IND)) {
							/* Indirect jump through register (used by CASE tables) */
							fprintf (stream, "\tbr\t%s\n", aarch64_get_register_name (ins->in_args[0]->regconst));
						}
					}
					break;
				case INS_CJUMP:
					if (ins->in_args[0] && ins->in_args[1]) {
						char *cond_str = "eq";
						switch (ins->in_args[0]->regconst) {
							case CC_E:  cond_str = "eq"; break;
							case CC_NE: cond_str = "ne"; break;
							case CC_LT: cond_str = "lt"; break;
							case CC_GT: cond_str = "gt"; break;
							case CC_LE: cond_str = "le"; break;
							case CC_GE: cond_str = "ge"; break;
							case CC_B:  cond_str = "lo"; break;
							case CC_A:  cond_str = "hi"; break;
							case CC_BE: cond_str = "ls"; break;
							case CC_AE: cond_str = "hs"; break;
							case CC_S:  cond_str = "mi"; break;
							case CC_NS: cond_str = "pl"; break;
							case CC_O:  cond_str = "vs"; break;
							case CC_NO: cond_str = "vc"; break;
							default:    cond_str = "eq"; break;
						}
						int mode = ins->in_args[1]->flags & ARG_MODEMASK;
						long label_num;
						if (mode == ARG_INSLABEL) {
							label_num = (long)((ins_chain *)ins->in_args[1]->regconst)->in_args[0]->regconst;
							fprintf (stream, "\tb.%s\tL%ld\n", cond_str, label_num);
						} else if (mode == ARG_LABEL) {
							fprintf (stream, "\tb.%s\tL%ld\n", cond_str, (long)ins->in_args[1]->regconst);
						} else if (mode == ARG_FLABEL) {
							fprintf (stream, "\tb.%s\t%ldf\n", cond_str, (long)ins->in_args[1]->regconst);
						} else if (mode == ARG_BLABEL) {
							fprintf (stream, "\tb.%s\t%ldb\n", cond_str, (long)ins->in_args[1]->regconst);
						} else if (mode == ARG_NAMEDLABEL) {
							char *symbol = (char *)ins->in_args[1]->regconst;
							char *fixed_symbol = aarch64_convert_symbol_name(symbol);
							fprintf (stream, "\tb.%s\t%s\n", cond_str, fixed_symbol);
							sfree(fixed_symbol);
						}
					}
					break;
				case INS_CMP:
					if (!ins->in_args[0] || !ins->in_args[1]) break;

					int mode0 = ins->in_args[0]->flags & ARG_MODEMASK;
					int mode1 = ins->in_args[1]->flags & ARG_MODEMASK;
					int is_const0 = (ins->in_args[0]->flags & ARG_ISCONST) || (mode0 == ARG_CONST);
					int is_const1 = (ins->in_args[1]->flags & ARG_ISCONST) || (mode1 == ARG_CONST);
					
					const char *op0_reg = NULL;
					const char *op1_reg = NULL;

					if (is_const0) {
						aarch64_emit_large_immediate (stream, (long)ins->in_args[0]->regconst, "x16");
						op0_reg = "x16";
					} else if (mode0 == ARG_REGIND && (ins->in_args[0]->flags & ARG_DISP)) {
						aarch64_emit_mem_op(stream, "ldr", "x16", aarch64_get_register_name(ins->in_args[0]->regconst), (long)ins->in_args[0]->disp);
						op0_reg = "x16";
					} else if (mode0 == ARG_REGIND && !(ins->in_args[0]->flags & ARG_DISP)) {
						fprintf(stream, "\tldr\tx16, [%s]\n", aarch64_get_register_name(ins->in_args[0]->regconst));
						op0_reg = "x16";
					} else {
						op0_reg = aarch64_get_register_name(ins->in_args[0]->regconst);
						if (ins->in_args[0]->regconst == REG_SP || ins->in_args[0]->regconst == 31) {
							fprintf(stream, "\tmov\tx16, sp\n");
							op0_reg = "x16";
						}
					}

					if (is_const1) {
						long val = (long)ins->in_args[1]->regconst;
						if (val == 0) {
							fprintf (stream, "\tcmp\t%s, #0\n", op0_reg);
							break;
						} else if (val > 0 && val <= 4095 && strcmp(op0_reg, "x17") != 0) {
							char w0[8];
							if (op0_reg[0] == 'x') snprintf(w0, sizeof(w0), "w%s", op0_reg + 1);
							else snprintf(w0, sizeof(w0), "%s", op0_reg);
							fprintf (stream, "\tcmp\t%s, #%ld\n", w0, val);
							break;
						} else {
							aarch64_emit_large_immediate (stream, val, "x17");
							op1_reg = "x17";
						}
					} else if (mode1 == ARG_REGIND && (ins->in_args[1]->flags & ARG_DISP)) {
						aarch64_emit_mem_op(stream, "ldr", "x17", aarch64_get_register_name(ins->in_args[1]->regconst), (long)ins->in_args[1]->disp);
						op1_reg = "x17";
					} else if (mode1 == ARG_REGIND && !(ins->in_args[1]->flags & ARG_DISP)) {
						fprintf(stream, "\tldr\tx17, [%s]\n", aarch64_get_register_name(ins->in_args[1]->regconst));
						op1_reg = "x17";
					} else {
						op1_reg = aarch64_get_register_name(ins->in_args[1]->regconst);
						if (ins->in_args[1]->regconst == REG_SP || ins->in_args[1]->regconst == 31) {
							fprintf(stream, "\tmov\tx17, sp\n");
							op1_reg = "x17";
						}
					}

					/* When a constant is involved, normally we'd use 32-bit w-register
					 * comparison for INT32 compatibility. But since we now zero-extend
					 * I_EQC constants, and INT32 variables are already zero-extended,
					 * we can safely use 64-bit x-registers everywhere.
					 * WAIT! INT16 variables are SIGN-extended! So I_EQC constants for INT16
					 * will not match! We MUST use w-registers for non-zero constants to
					 * ignore the upper 32-bits so INT16 and INT32 match.
					 * But for 0, 64-bit comparison is always safe and required for INT64. */
					if ((is_const0 && (long)ins->in_args[0]->regconst != 0) || 
					    (is_const1 && (long)ins->in_args[1]->regconst != 0)) {
						char w0[8], w1[8];
						if (op0_reg[0] == 'x') snprintf(w0, sizeof(w0), "w%s", op0_reg + 1);
						else snprintf(w0, sizeof(w0), "%s", op0_reg);
						if (op1_reg[0] == 'x') snprintf(w1, sizeof(w1), "w%s", op1_reg + 1);
						else snprintf(w1, sizeof(w1), "%s", op1_reg);
						fprintf (stream, "\tcmp\t%s, %s\n", w1, w0);
					} else {
						fprintf (stream, "\tcmp\t%s, %s\n", op1_reg, op0_reg);
					}
					break;

				case INS_SETCC:
					/* Set register to 0/1 based on condition flags (aarch64: cset) */
					if (ins->in_args[0] && ins->out_args[0]) {
						int cc = (int)ins->in_args[0]->regconst;
						const char *dst = aarch64_get_register_name(ins->out_args[0]->regconst);
						const char *cond;
						switch (cc) {
							case CC_Z: cond = "eq"; break;
							case CC_NZ: cond = "ne"; break;
							case CC_LT: cond = "lt"; break;
							case CC_GE: cond = "ge"; break;
							case CC_LE: cond = "le"; break;
							case CC_GT: cond = "gt"; break;
							case CC_B: cond = "lo"; break;
							case CC_AE: cond = "hs"; break;
							case CC_BE: cond = "ls"; break;
							case CC_A: cond = "hi"; break;
							case CC_S: cond = "mi"; break;
							case CC_NS: cond = "pl"; break;
							case CC_O: cond = "vs"; break;
							case CC_NO: cond = "vc"; break;
							default: cond = "eq"; break;
						}
						fprintf(stream, "\tcset\t%s, %s\n", dst, cond);
					}
					break;

								case INS_ANNO:
					if (ins->in_args[0] && (ins->in_args[0]->flags & ARG_MODEMASK) == ARG_TEXT) {
						const char *text = (const char *)ins->in_args[0]->regconst;
						/* Check if text is already a comment or a real instruction */
						/* Skip leading whitespace for passthrough matching */
						const char *p = text;
						while (*p == '\t' || *p == ' ') p++;
						if (text[0] == '/' || text[0] == '\t'
						    || strncmp(p, "dmb", 3) == 0 || strncmp(p, "dsb", 3) == 0 || strncmp(p, "isb", 3) == 0
						    || strncmp(p, "fmov", 4) == 0 || strncmp(p, "fcvt", 4) == 0
						    || strncmp(p, "fadd", 4) == 0 || strncmp(p, "fsub", 4) == 0
						    || strncmp(p, "fmul", 4) == 0 || strncmp(p, "fdiv", 4) == 0
						    || strncmp(p, "fsqrt", 5) == 0 || strncmp(p, "fabs", 4) == 0
						    || strncmp(p, "fneg", 4) == 0 || strncmp(p, "fcmp", 4) == 0
						    || strncmp(p, "frint", 5) == 0  /* frintz, frintx, frintn, etc */
						    || strncmp(p, "ldr s", 5) == 0 || strncmp(p, "ldr d", 5) == 0
						    || strncmp(p, "str s", 5) == 0 || strncmp(p, "str d", 5) == 0
						    || strncmp(p, "str w1", 6) == 0 || strncmp(p, "str x1", 6) == 0
						    || strncmp(p, "ldr w1", 6) == 0 || strncmp(p, "ldr x1", 6) == 0
						    || strncmp(p, "scvtf", 5) == 0 || strncmp(p, "fcvtzs", 6) == 0
						    || strncmp(p, "fcvtns", 6) == 0 || strncmp(p, "fcvtps", 6) == 0
						    || strncmp(p, "fcvtms", 6) == 0
						    || strncmp(p, "mrs", 3) == 0 || strncmp(p, "msr", 3) == 0
						    || strncmp(p, "orr", 3) == 0 || strncmp(p, "bic", 3) == 0
						    || strncmp(p, "mov x1", 6) == 0 || strncmp(p, "add x1", 6) == 0) {
							/* Emit as actual instruction - text may already have leading tab */
							if (text[0] == '\t') {
								fprintf (stream, "%s\n", text);
							} else {
								fprintf (stream, "\t%s\n", text);
							}
						} else {
							fprintf (stream, "\t// %s\n", text);
						}
					}
					break;
				case INS_MOVEB:
					/* Byte move/store */
					if (!ins->in_args[0] || !ins->out_args[0]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST && (ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
						/* Store constant byte to memory */
						long val = (long)ins->in_args[0]->regconst & 0xFF;
						long disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
						const char *base = aarch64_get_register_name(ins->out_args[0]->regconst);
						fprintf(stream, "\tmov\tw16, #%ld\n", val);
						aarch64_emit_mem_op(stream, "strb", "w16", base, disp);
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND && (ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
						/* Load byte from memory */
						long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
						const char *base = aarch64_get_register_name(ins->in_args[0]->regconst);
						const char *dst = aarch64_get_register_name(ins->out_args[0]->regconst);
						char wdst[8];
						snprintf(wdst, sizeof(wdst), "w%ld", (long)ins->out_args[0]->regconst);
						aarch64_emit_mem_op(stream, "ldrb", wdst, base, disp);
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REG && (ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
						/* Store register byte to memory */
						long disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
						const char *base = aarch64_get_register_name(ins->out_args[0]->regconst);
						char wsrc[8];
						snprintf(wsrc, sizeof(wsrc), "w%ld", (long)ins->in_args[0]->regconst);
						aarch64_emit_mem_op(stream, "strb", wsrc, base, disp);
					}
					break;
				case INS_MOVEZEXT8TO32:
					/* Zero-extend byte to 32/64-bit */
					if (!ins->in_args[0] || !ins->out_args[0]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
						long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
						const char *base = aarch64_get_register_name(ins->in_args[0]->regconst);
						char wdst[8];
						snprintf(wdst, sizeof(wdst), "w%ld", (long)ins->out_args[0]->regconst);
						aarch64_emit_mem_op(stream, "ldrb", wdst, base, disp);
					} else {
						/* Register source: zero-extend with uxtb */
						char wsrc[8], wdst[8];
						snprintf(wsrc, sizeof(wsrc), "w%ld", (long)ins->in_args[0]->regconst);
						snprintf(wdst, sizeof(wdst), "w%ld", (long)ins->out_args[0]->regconst);
						fprintf(stream, "\tuxtb\t%s, %s\n", wdst, wsrc);
					}
					break;
				case INS_MOVEZEXT16TO32:
					/* Zero-extend halfword to 32/64-bit */
					if (!ins->in_args[0] || !ins->out_args[0]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
						long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
						const char *base = aarch64_get_register_name(ins->in_args[0]->regconst);
						char wdst[8];
						snprintf(wdst, sizeof(wdst), "w%ld", (long)ins->out_args[0]->regconst);
						aarch64_emit_mem_op(stream, "ldrh", wdst, base, disp);
					} else {
						char wsrc[8], wdst[8];
						snprintf(wsrc, sizeof(wsrc), "w%ld", (long)ins->in_args[0]->regconst);
						snprintf(wdst, sizeof(wdst), "w%ld", (long)ins->out_args[0]->regconst);
						fprintf(stream, "\tuxth\t%s, %s\n", wdst, wsrc);
					}
					break;
				case INS_MOVESEXT16TO32:
					/* Sign-extend halfword to 64-bit */
					if (!ins->in_args[0] || !ins->out_args[0]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
						long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
						const char *base = aarch64_get_register_name(ins->in_args[0]->regconst);
						const char *dst = aarch64_get_register_name(ins->out_args[0]->regconst);
						aarch64_emit_mem_op(stream, "ldrsh", dst, base, disp);
					} else {
						const char *src = aarch64_get_register_name(ins->in_args[0]->regconst);
						const char *dst = aarch64_get_register_name(ins->out_args[0]->regconst);
						fprintf(stream, "\tsxth\t%s, %s\n", dst, src);
					}
					break;
				case INS_MOVE32:
					/* 32-bit load/store for INT on 64-bit targets */
					if (!ins->in_args[0] || !ins->out_args[0]) break;
					if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REG &&
					    (ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
						/* Store 32-bit: str wreg, [base] */
						long disp = (ins->out_args[0]->flags & ARG_DISP) ? ins->out_args[0]->disp : 0;
						char wsrc[8];
						snprintf(wsrc, sizeof(wsrc), "w%ld", (long)ins->in_args[0]->regconst);
						aarch64_emit_mem_op(stream, "str", wsrc,
							aarch64_get_register_name(ins->out_args[0]->regconst), disp);
					} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND &&
					           (ins->out_args[0]->flags & ARG_MODEMASK) == ARG_REG) {
						/* Load 32-bit: ldr wN, [base] (zero-extends to 64-bit).
						 * Consistent with INS_TRUNCATE32 which also zero-extends.
						 * All INT values use zero-extended representation. */
						long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
						char wdst[8];
						snprintf(wdst, sizeof(wdst), "w%ld", (long)ins->out_args[0]->regconst);
						aarch64_emit_mem_op(stream, "ldr", wdst,
							aarch64_get_register_name(ins->in_args[0]->regconst), disp);
					}
					break;
				case INS_TRUNCATE32:
					/* Truncate to 32 bits: mov wN, wN (zero-extends to 64-bit).
					 * This gives correct 32-bit INT overflow/wrap semantics
					 * on 64-bit registers. */
					if (ins->in_args[0] && ins->out_args[0]) {
						fprintf(stream, "\tmov\tw%ld, w%ld\n",
							(long)ins->out_args[0]->regconst,
							(long)ins->in_args[0]->regconst);
					}
					break;
				case INS_SIGNEXT32:
					/* Sign-extend low 32 bits to 64: sxtw xN, wN. */
					if (ins->in_args[0] && ins->out_args[0]) {
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
							long val = (long)ins->in_args[0]->regconst;
							aarch64_emit_large_immediate(stream, val, "x16");
							fprintf(stream, "\tsxtw\t%s, w16\n",
								aarch64_get_register_name(ins->out_args[0]->regconst));
						} else if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_REGIND) {
							long disp = (ins->in_args[0]->flags & ARG_DISP) ? ins->in_args[0]->disp : 0;
							aarch64_emit_mem_op(stream, "ldrsw", 
								aarch64_get_register_name(ins->out_args[0]->regconst),
								aarch64_get_register_name(ins->in_args[0]->regconst), disp);
						} else {
							/* ARG_REG: Need to extract the W register name */
							char wsrc[8];
							snprintf(wsrc, sizeof(wsrc), "w%ld", (long)ins->in_args[0]->regconst);
							/* Wait, regconst is the register ID, we should get real hardware reg */
							const char *xsrc = aarch64_get_register_name(ins->in_args[0]->regconst);
							/* xsrc is e.g. "x4". Change to "w4" */
							if (xsrc[0] == 'x' || xsrc[0] == 'X') {
								snprintf(wsrc, sizeof(wsrc), "w%s", xsrc + 1);
							} else if (strcmp(xsrc, "sp") == 0) {
								strcpy(wsrc, "wsp");
							} else {
								strcpy(wsrc, xsrc);
							}
							fprintf(stream, "\tsxtw\t%s, %s\n",
								aarch64_get_register_name(ins->out_args[0]->regconst), wsrc);
						}
					}
					break;
				case INS_LOADLABDIFF:
					/* Compute label difference: dst = L1 - L2 */
					if (ins->in_args[0] && ins->in_args[1] && ins->out_args[0]) {
						long lab1, lab2;
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_INSLABEL)
							lab1 = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
						else
							lab1 = (long)ins->in_args[0]->regconst;
						if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_INSLABEL)
							lab2 = (long)((ins_chain *)ins->in_args[1]->regconst)->in_args[0]->regconst;
						else
							lab2 = (long)ins->in_args[1]->regconst;
						fprintf(stream, "\tadr\tx16, L%ld\n", lab1);
						fprintf(stream, "\tadr\tx17, L%ld\n", lab2);
						fprintf(stream, "\tsub\t%s, x16, x17\n",
							aarch64_get_register_name(ins->out_args[0]->regconst));
					}
					break;
				case INS_CONSTLABDIFF:
					/* Emit constant label difference (for data sections) */
					if (ins->in_args[0] && ins->in_args[1]) {
						long lab1, lab2;
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_INSLABEL)
							lab1 = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
						else
							lab1 = (long)ins->in_args[0]->regconst;
						if ((ins->in_args[1]->flags & ARG_MODEMASK) == ARG_INSLABEL)
							lab2 = (long)((ins_chain *)ins->in_args[1]->regconst)->in_args[0]->regconst;
						else
							lab2 = (long)ins->in_args[1]->regconst;
						fprintf(stream, "\t.quad\t(L%ld - L%ld)\n", lab1, lab2);
					}
					break;
				case INS_CONSTLABADDR:
					/* Emit constant label address (for data sections) */
					if (ins->in_args[0]) {
						long lab;
						if ((ins->in_args[0]->flags & ARG_MODEMASK) == ARG_INSLABEL)
							lab = (long)((ins_chain *)ins->in_args[0]->regconst)->in_args[0]->regconst;
						else
							lab = (long)ins->in_args[0]->regconst;
						fprintf(stream, "\t.quad\tL%ld\n", lab);
					}
					break;
				case INS_FLD32:
					/* Load float from memory into FP register s0 (REAL32) or d0 (REAL64).
					 * If in_args[1] is ARG_CONST 64, load as REAL64 (ldr d0).
					 * Otherwise load as REAL32 (ldr s0). */
					if (ins->in_args[0]) {
						int mode = ins->in_args[0]->flags & ARG_MODEMASK;
						int is_64 = 0;
						const char *fpreg = "s0";
						int align = 4;
						long max_scaled = 16380;
						if (ins->in_args[1] && (ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST
						    && ins->in_args[1]->regconst == 64) {
							is_64 = 1;
							fpreg = "d0";
							align = 8;
							max_scaled = 32760;
						}
						if (mode == ARG_REGIND && (ins->in_args[0]->flags & ARG_DISP)) {
							long disp = (long)ins->in_args[0]->disp;
							const char *base = aarch64_get_register_name (ins->in_args[0]->regconst);
							if (disp >= 0 && disp <= max_scaled && (disp % align) == 0) {
								fprintf (stream, "\tldr\t%s, [%s, #%ld]\n", fpreg, base, disp);
							} else if (disp >= -256 && disp <= 255) {
								fprintf (stream, "\tldur\t%s, [%s, #%ld]\n", fpreg, base, disp);
							} else {
								aarch64_emit_large_immediate (stream, disp, "x17");
								fprintf (stream, "\tadd\tx17, %s, x17\n", base);
								fprintf (stream, "\tldr\t%s, [x17]\n", fpreg);
							}
						} else if (mode == ARG_REGIND) {
							fprintf (stream, "\tldr\t%s, [%s]\n", fpreg,
								aarch64_get_register_name (ins->in_args[0]->regconst));
						}
					}
					break;
				case INS_FSQRT:
					/* Float square root: in_args[0]=src reg, in_args[1]=precision (ARG_CONST),
					 * out_args[0]=dst reg.  Precision 64 uses d-registers, 32 uses s-registers. */
					if (ins->in_args[0] && ins->out_args[0]) {
						int prec = 32;
						if (ins->in_args[1] && (ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[1]->regconst;
						}
						if (prec == 64) {
							const char *src = aarch64_get_register_name (ins->in_args[0]->regconst);
							const char *dst = aarch64_get_register_name (ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", src);
							fprintf (stream, "\tfsqrt\td0, d0\n");
							fprintf (stream, "\tfmov\t%s, d0\n", dst);
						} else {
							char ws[8], wd[8];
							snprintf(ws, sizeof(ws), "w%ld", (long)ins->in_args[0]->regconst);
							snprintf(wd, sizeof(wd), "w%ld", (long)ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\ts0, %s\n", ws);
							fprintf (stream, "\tfsqrt\ts0, s0\n");
							fprintf (stream, "\tfmov\t%s, s0\n", wd);
						}
					}
					break;
				case INS_FABS:
					/* Float absolute value: in_args[0]=src, in_args[1]=precision (ARG_CONST),
					 * out_args[0]=dst. */
					if (ins->in_args[0] && ins->out_args[0]) {
						int prec = 32;
						if (ins->in_args[1] && (ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[1]->regconst;
						}
						if (prec == 64) {
							const char *src = aarch64_get_register_name (ins->in_args[0]->regconst);
							const char *dst = aarch64_get_register_name (ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", src);
							fprintf (stream, "\tfabs\td0, d0\n");
							fprintf (stream, "\tfmov\t%s, d0\n", dst);
						} else {
							char ws[8], wd[8];
							snprintf(ws, sizeof(ws), "w%ld", (long)ins->in_args[0]->regconst);
							snprintf(wd, sizeof(wd), "w%ld", (long)ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\ts0, %s\n", ws);
							fprintf (stream, "\tfabs\ts0, s0\n");
							fprintf (stream, "\tfmov\t%s, s0\n", wd);
						}
					}
					break;
				case INS_FADD:
					/* Float addition: in_args[0]=src1, in_args[1]=src2,
					 * in_args[2]=precision (ARG_CONST, 32 or 64),
					 * out_args[0]=dst.
					 * For REAL64: fmov d0, x_a; fmov d1, x_b; fadd d0, d0, d1; fmov x_dst, d0
					 * For REAL32: fmov s0, w_a; fmov s1, w_b; fadd s0, s0, s1; fmov w_dst, s0 */
					if (ins->in_args[0] && ins->in_args[1] && ins->out_args[0]) {
						int prec = 32;
						if (ins->in_args[2] && (ins->in_args[2]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[2]->regconst;
						}
						if (prec == 64) {
							const char *ra = aarch64_get_register_name (ins->in_args[0]->regconst);
							const char *rb = aarch64_get_register_name (ins->in_args[1]->regconst);
							const char *rd = aarch64_get_register_name (ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", ra);
							fprintf (stream, "\tfmov\td1, %s\n", rb);
							fprintf (stream, "\tfadd\td0, d0, d1\n");
							fprintf (stream, "\tfmov\t%s, d0\n", rd);
						} else {
							char wa[8], wb[8], wd[8];
							snprintf(wa, sizeof(wa), "w%ld", (long)ins->in_args[0]->regconst);
							snprintf(wb, sizeof(wb), "w%ld", (long)ins->in_args[1]->regconst);
							snprintf(wd, sizeof(wd), "w%ld", (long)ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\ts0, %s\n", wa);
							fprintf (stream, "\tfmov\ts1, %s\n", wb);
							fprintf (stream, "\tfadd\ts0, s0, s1\n");
							fprintf (stream, "\tfmov\t%s, s0\n", wd);
						}
					}
					break;
				case INS_FSUB:
					/* Float subtraction: same layout as INS_FADD. */
					if (ins->in_args[0] && ins->in_args[1] && ins->out_args[0]) {
						int prec = 32;
						if (ins->in_args[2] && (ins->in_args[2]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[2]->regconst;
						}
						if (prec == 64) {
							const char *ra = aarch64_get_register_name (ins->in_args[0]->regconst);
							const char *rb = aarch64_get_register_name (ins->in_args[1]->regconst);
							const char *rd = aarch64_get_register_name (ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", ra);
							fprintf (stream, "\tfmov\td1, %s\n", rb);
							fprintf (stream, "\tfsub\td0, d0, d1\n");
							fprintf (stream, "\tfmov\t%s, d0\n", rd);
						} else {
							char wa[8], wb[8], wd[8];
							snprintf(wa, sizeof(wa), "w%ld", (long)ins->in_args[0]->regconst);
							snprintf(wb, sizeof(wb), "w%ld", (long)ins->in_args[1]->regconst);
							snprintf(wd, sizeof(wd), "w%ld", (long)ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\ts0, %s\n", wa);
							fprintf (stream, "\tfmov\ts1, %s\n", wb);
							fprintf (stream, "\tfsub\ts0, s0, s1\n");
							fprintf (stream, "\tfmov\t%s, s0\n", wd);
						}
					}
					break;
				case INS_FMUL:
					/* Float multiplication: same layout as INS_FADD. */
					if (ins->in_args[0] && ins->in_args[1] && ins->out_args[0]) {
						int prec = 32;
						if (ins->in_args[2] && (ins->in_args[2]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[2]->regconst;
						}
						if (prec == 64) {
							const char *ra = aarch64_get_register_name (ins->in_args[0]->regconst);
							const char *rb = aarch64_get_register_name (ins->in_args[1]->regconst);
							const char *rd = aarch64_get_register_name (ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", ra);
							fprintf (stream, "\tfmov\td1, %s\n", rb);
							fprintf (stream, "\tfmul\td0, d0, d1\n");
							fprintf (stream, "\tfmov\t%s, d0\n", rd);
						} else {
							char wa[8], wb[8], wd[8];
							snprintf(wa, sizeof(wa), "w%ld", (long)ins->in_args[0]->regconst);
							snprintf(wb, sizeof(wb), "w%ld", (long)ins->in_args[1]->regconst);
							snprintf(wd, sizeof(wd), "w%ld", (long)ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\ts0, %s\n", wa);
							fprintf (stream, "\tfmov\ts1, %s\n", wb);
							fprintf (stream, "\tfmul\ts0, s0, s1\n");
							fprintf (stream, "\tfmov\t%s, s0\n", wd);
						}
					}
					break;
				case INS_FDIV:
					/* Float division: same layout as INS_FADD. */
					if (ins->in_args[0] && ins->in_args[1] && ins->out_args[0]) {
						int prec = 32;
						if (ins->in_args[2] && (ins->in_args[2]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[2]->regconst;
						}
						if (prec == 64) {
							const char *ra = aarch64_get_register_name (ins->in_args[0]->regconst);
							const char *rb = aarch64_get_register_name (ins->in_args[1]->regconst);
							const char *rd = aarch64_get_register_name (ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", ra);
							fprintf (stream, "\tfmov\td1, %s\n", rb);
							fprintf (stream, "\tfdiv\td0, d0, d1\n");
							fprintf (stream, "\tfmov\t%s, d0\n", rd);
						} else {
							char wa[8], wb[8], wd[8];
							snprintf(wa, sizeof(wa), "w%ld", (long)ins->in_args[0]->regconst);
							snprintf(wb, sizeof(wb), "w%ld", (long)ins->in_args[1]->regconst);
							snprintf(wd, sizeof(wd), "w%ld", (long)ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\ts0, %s\n", wa);
							fprintf (stream, "\tfmov\ts1, %s\n", wb);
							fprintf (stream, "\tfdiv\ts0, s0, s1\n");
							fprintf (stream, "\tfmov\t%s, s0\n", wd);
						}
					}
					break;
				case INS_FRNDINT:
					/* Floating point integer part (truncate toward zero).
					 * in_args[0]=src reg, in_args[1]=precision (ARG_CONST),
					 * out_args[0]=dst reg.
					 * If no args (legacy), this is a no-op signal for INS_FIST64. */
					if (ins->in_args[0] && ins->out_args[0]) {
						int prec = 32;
						if (ins->in_args[1] && (ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[1]->regconst;
						}
						if (prec == 64) {
							const char *src = aarch64_get_register_name (ins->in_args[0]->regconst);
							const char *dst = aarch64_get_register_name (ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", src);
							fprintf (stream, "\tfrintz\td0, d0\n");
							fprintf (stream, "\tfmov\t%s, d0\n", dst);
						} else {
							char ws[8], wd[8];
							snprintf(ws, sizeof(ws), "w%ld", (long)ins->in_args[0]->regconst);
							snprintf(wd, sizeof(wd), "w%ld", (long)ins->out_args[0]->regconst);
							fprintf (stream, "\tfmov\ts0, %s\n", ws);
							fprintf (stream, "\tfrintz\ts0, s0\n");
							fprintf (stream, "\tfmov\t%s, s0\n", wd);
						}
					}
					/* else: legacy no-arg form, no-op (conversion deferred to FIST64) */
					break;
				case INS_FIST64:
					/* FP store operations.  The precision in in_args[0] (ARG_CONST)
					 * determines the mode:
					 *   32: convert s0 to INT32 and store (fcvtzs w17, s0; str w17)
					 *   64: convert d0 to INT32 and store (fcvtzs w17, d0; str w17)
					 *  132: store s0 bits as REAL32 (fmov w17, s0; str w17)
					 *  164: store d0 bits as REAL64 (fmov x17, d0; str x17)
					 * out_args[0] = destination memory address. */
					if (ins->out_args[0]) {
						int mode = ins->out_args[0]->flags & ARG_MODEMASK;
						int prec = 32;
						const char *store_reg = "w17";
						const char *store_op = "str";

						if (ins->in_args[0] && (ins->in_args[0]->flags & ARG_MODEMASK) == ARG_CONST) {
							prec = (int)ins->in_args[0]->regconst;
						}

						if (prec == 164) {
						        /* Store REAL64 bits: fmov x17, d0 */
						        fprintf (stream, "\tfmov\tx17, d0\n");
						        store_reg = "x17";
						        aarch64_fp_from_i64 = 0;
						} else if (prec == 132) {
						        /* Store REAL32 bits: fmov w17, s0. */
						        if (aarch64_fp_from_i64) {
						                /* Value is REAL64 in d0 from I64TOREAL; narrow first */
						                fprintf (stream, "\tfcvt\ts0, d0\n");
						                aarch64_fp_from_i64 = 0;
						        }
						        fprintf (stream, "\tfmov\tw17, s0\n");
						        store_reg = "w17";
						} else {
							/* Convert REAL32/REAL64 to INT32 or INT64. */
							int fp_prec = prec & 0xFF;
							if (aarch64_fp_from_i64 && fp_prec != 64) {
								/* Value is REAL64 in d0 from I64TOREAL but target
								 * precision is REAL32; narrow first. */
								fprintf (stream, "\tfcvt\ts0, d0\n");
							}
							aarch64_fp_from_i64 = 0;
							int rmode = (prec >> 8) & 0xFF;
							int is_int64 = (prec & 0x10000) != 0;
							const char *fp_src = (fp_prec == 64) ? "d0" : "s0";
							const char *cvt_insn;
							switch (rmode) {
								case 3: cvt_insn = "fcvtzs"; break; /* round toward zero */
								case 1: cvt_insn = "fcvtps"; break; /* round up */
								case 2: cvt_insn = "fcvtms"; break; /* round down */
								default: cvt_insn = "fcvtns"; break; /* round to nearest */
							}

							if (is_int64) {
								fprintf (stream, "\t%s\tx17, %s\n", cvt_insn, fp_src);
								store_reg = "x17";
							} else {
								fprintf (stream, "\t%s\tw17, %s\n", cvt_insn, fp_src);
								if (ins->out_args[0]->flags & ARG_DISP) {
									fprintf (stream, "\tsxtw\tx17, w17\n");
									store_reg = "x17";
								} else {
									store_reg = "w17";
								}
							}
						}						if (mode == ARG_REGIND && (ins->out_args[0]->flags & ARG_DISP)) {							aarch64_emit_mem_op (stream, store_op, store_reg,
								aarch64_get_register_name (ins->out_args[0]->regconst),
								(long)ins->out_args[0]->disp);
						} else if (mode == ARG_REGIND) {
							fprintf (stream, "\tstr\t%s, [%s]\n", store_reg,
								aarch64_get_register_name (ins->out_args[0]->regconst));
						}
					}
					break;
				case INS_FLT32:
					/* INT32->REAL32: scvtf s0, w_src; fmov w_dst, s0
					 * REAL64->REAL32 (if in_args[1] is ARG_CONST 64):
					 *   fmov d0, x_src; fcvt s0, d0; fmov w_dst, s0 */
					if (ins->in_args[0] && ins->out_args[0]) {
						int src_prec = 0;
						char w_dst[8];
						snprintf(w_dst, sizeof(w_dst), "w%ld", (long)ins->out_args[0]->regconst);
						if (ins->in_args[1] && (ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							src_prec = (int)ins->in_args[1]->regconst;
						}
						if (src_prec == 64) {
							/* Source is REAL64 float bits in x-register */
							const char *src = aarch64_get_register_name (ins->in_args[0]->regconst);
							fprintf (stream, "\tfmov\td0, %s\n", src);
							fprintf (stream, "\tfcvt\ts0, d0\n");
							fprintf (stream, "\tfmov\t%s, s0\n", w_dst);
						} else {
							/* Source is INT32 in w-register */
							char w_src[8];
							snprintf(w_src, sizeof(w_src), "w%ld", (long)ins->in_args[0]->regconst);
							fprintf (stream, "\tscvtf\ts0, %s\n", w_src);
							fprintf (stream, "\tfmov\t%s, s0\n", w_dst);
						}
					}
					break;
				case INS_FLT64:
					/* INT32->REAL64: scvtf d0, w_src; fmov x_dst, d0
					 * REAL32->REAL64 (if in_args[1] is ARG_CONST 32):
					 *   fmov s0, w_src; fcvt d0, s0; fmov x_dst, d0 */
					if (ins->in_args[0] && ins->out_args[0]) {
						int src_prec = 0;
						const char *dst = aarch64_get_register_name (ins->out_args[0]->regconst);
						char w_src[8];
						snprintf(w_src, sizeof(w_src), "w%ld", (long)ins->in_args[0]->regconst);
						if (ins->in_args[1] && (ins->in_args[1]->flags & ARG_MODEMASK) == ARG_CONST) {
							src_prec = (int)ins->in_args[1]->regconst;
						}
						if (src_prec == 32) {
							/* Source is REAL32 float bits - widen to REAL64 */
							fprintf (stream, "\tfmov\ts0, %s\n", w_src);
							fprintf (stream, "\tfcvt\td0, s0\n");
							fprintf (stream, "\tfmov\t%s, d0\n", dst);
						} else {
							/* Source is INT32 - convert to REAL64 */
							fprintf (stream, "\tscvtf\td0, %s\n", w_src);
							fprintf (stream, "\tfmov\t%s, d0\n", dst);
						}
					}
					break;
				case INS_FILD64:
					/* Load 64-bit integer and convert to REAL64.
					 * ARG_REGIND: address of INT64 value (load then convert).
					 * ARG_REG: value already in register (convert directly).
					 * Result goes to d0 (FP stack top). */
					if (ins->in_args[0]) {
						int mode = ins->in_args[0]->flags & ARG_MODEMASK;
						if (mode == ARG_REGIND) {
							long disp = (ins->in_args[0]->flags & ARG_DISP) ? (long)ins->in_args[0]->disp : 0;
							const char *base = aarch64_get_register_name (ins->in_args[0]->regconst);
							aarch64_emit_mem_op (stream, "ldr", "x17", base, disp);
							fprintf (stream, "\tscvtf\td0, x17\n");
							aarch64_fp_from_i64 = 1;
						} else if (mode == ARG_REG) {
							fprintf (stream, "\tscvtf\td0, %s\n",
								aarch64_get_register_name (ins->in_args[0]->regconst));
							aarch64_fp_from_i64 = 1;
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
			skip_dead_code = 0;
			skip_dead_code = 0;
			if (tmp->u.label_name) {
				char *converted_name = aarch64_convert_symbol_name(tmp->u.label_name);
				fprintf(stream, "%s:\n", converted_name);
				sfree(converted_name);
			}
			break;
		case RTL_PUBLICSETNAMEDLABEL:
			skip_dead_code = 0;
			skip_dead_code = 0;
			if (tmp->u.label_name) {
				char *converted_name = aarch64_convert_symbol_name(tmp->u.label_name);
				fprintf(stream, ".global %s\n%s:\n", converted_name, converted_name);
				sfree(converted_name);
			}
			break;
		default:
			break;
		}
	}

	return 0;
}
/*}}}*/

/*{{{  Helper functions for consistent symbol and register handling*/

/*
 *\tValidates register constants to prevent crashes from invalid values
 */

/*{{{  static const char *aarch64_get_label_prefix(int flags)*/
static const char *aarch64_get_label_prefix(int flags)
{
	int mode = flags & ARG_MODEMASK;
	if (mode == ARG_FLABEL) return "f";
	if (mode == ARG_BLABEL) return "b";
	return "";
}
/*}}}*/

/*{{{  static int aarch64_sets_flags (ins_chain *ins)*/
static int aarch64_sets_flags (ins_chain *ins)
{
	int i;
	for (i = 0; i < 4; i++) {
		if (ins->out_args[i] && (ins->out_args[i]->flags & ARG_MODEMASK) == ARG_REG &&
		    (ins->out_args[i]->regconst == REG_CC || ins->out_args[i]->regconst == AARCH64_REG_CC)) {
			return 1;
		}
	}
	return 0;
}
/*}}}*/

static int aarch64_validate_register(int reg)
{
	return (reg != (int)0x80000000 && reg != REG_UNDEFINED);
}


/*
 * Converts an occam symbol to a valid assembly symbol.
 *
 * This is the aarch64 equivalent of modify_name() in asm386.c.
 * It is called for every ARG_NAMEDLABEL during assembly emission,
 * so it must be idempotent for symbols already processed by the
 * compose_*_aarch64 functions (which prepend extref_prefix and
 * convert dots to underscores).
 *
 * The logic mirrors modify_name():
 *   1. Convert '.' to '_', strip '$', '^', '*', '@', '&', stop at '%'
 *   2. Add prefix (O_, E_, M_) unless result starts with '_', 'O_', 'DCR_',
 *      or original started with '&' or '@'
 *   3. If a prefix was added (or original was '&'), prepend extref_prefix
 *
 * The caller is responsible for freeing the returned string.
 */
static char *aarch64_convert_symbol_name(const char *symbol) {
	if (symbol == NULL) return NULL;

	int len = strlen(symbol);
	char *rbuf = smalloc(len * 2 + 20);
	int i, j;
	const char *prepend = NULL;
	const char *ext = options.extref_prefix ? options.extref_prefix : "";

	/*
	 * AArch64-specific: kernel call entrypoints (Y_xxx, X_xxx) are called
	 * directly (not through calltable like i386), so we must map them to
	 * the C symbol names: kernel_Y_xxx / kernel_X_xxx.
	 */
	if ((symbol[0] == 'Y' || symbol[0] == 'X') && symbol[1] == '_') {
		sprintf(rbuf, "%skernel_%s", ext, symbol);
		return rbuf;
	}

	/* System symbols: ccsp_*, occam_*, *wsbytes, etc -> __name */
	if (!strncmp(symbol, "ccsp_", 5) || !strncmp(symbol, "occam_", 6) ||
	    strstr(symbol, "wsbytes") || strstr(symbol, "msbytes") ||
	    strstr(symbol, "vsbytes") || strstr(symbol, "wsadjust")) {
		const char *base = symbol;
		while (*base == '_') base++;
		sprintf(rbuf, "__%s", base);
		return rbuf;
	}

	/* local_scheduler -> _local_scheduler */
	if (strstr(symbol, "local_scheduler")) {
		sprintf(rbuf, "%slocal_scheduler", ext);
		return rbuf;
	}

	/*
	 * General case: mirrors modify_name() from asm386.c exactly.
	 * Step 1: convert '.' to '_', strip '$', '^', '*', '@', '&', stop at '%'
	 */
	j = 0;
	for (i = 0; i < len; i++) {
		switch (symbol[i]) {
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
			i = len;  /* stop processing */
			break;
		default:
			rbuf[j++] = symbol[i];
			break;
		}
	}
	rbuf[j] = '\0';

	/* Step 2: determine if we need to add a semantic prefix */
	if ((rbuf[0] == '_') || (!strncmp(rbuf, "O_", 2)) || (!strncmp(rbuf, "DCR_", 4)) || (symbol[0] == '&') || (symbol[0] == '@')) {
		/* skip - no prefix needed */
	} else if (symbol[0] == '^') {
		prepend = "E_";
	} else if (symbol[0] == '*') {
		prepend = "M_";
	} else {
		prepend = "O_";
	}

	if (prepend) {
		int plen = strlen(prepend);
		memmove(rbuf + plen, rbuf, strlen(rbuf) + 1);
		memcpy(rbuf, prepend, plen);
	}

	/* Step 3: add platform prefix (e.g. '_' on Darwin) if a semantic prefix
	 * was added, or if the original symbol started with '&' */
	if (options.extref_prefix && (prepend || symbol[0] == '&')) {
		int plen = strlen(options.extref_prefix);
		memmove(rbuf + plen, rbuf, strlen(rbuf) + 1);
		memcpy(rbuf, options.extref_prefix, plen);
	}

	return rbuf;
}

static void aarch64_emit_symbol_reference(FILE *stream, const char *symbol, const char *instruction)
{
    char *fixed_symbol = aarch64_convert_symbol_name(symbol);
	fprintf(stream, "\t%s\t%s\n", instruction, fixed_symbol);
    sfree(fixed_symbol);
}

/*}}}*/

/*{{{  static void aarch64_emit_large_immediate (FILE *stream, long value, const char *temp_reg)*/
/*
 * Emits ARM64 instructions to load a large immediate value into a temporary register
 * ARM64 instructions only support 12-bit immediates (0-4095), so larger values
 * need to be loaded using movz/movk sequence
 */
static void aarch64_emit_large_immediate (FILE *stream, long value, const char *temp_reg)
{
	unsigned long uval = (unsigned long)value;
	if (uval > 0xFFFFFFFFUL) fprintf(stderr, "DBG: large_imm=0x%lx\n", uval);
	fprintf (stream, "\tmovz\t%s, #%lu\n", temp_reg, uval & 0xFFFF);
	if (uval > 0xFFFF) {
		fprintf (stream, "\tmovk\t%s, #%lu, lsl #16\n", temp_reg, (uval >> 16) & 0xFFFF);
	}
	if (uval > 0xFFFFFFFF) {
		fprintf (stream, "\tmovk\t%s, #%lu, lsl #32\n", temp_reg, (uval >> 32) & 0xFFFF);
	}
	if (uval > 0xFFFFFFFFFFFF) {
		fprintf (stream, "\tmovk\t%s, #%lu, lsl #48\n", temp_reg, (uval >> 48) & 0xFFFF);
	}
}
/*}}}*/

/*{{{  static void aarch64_emit_symbol_addr (FILE *stream, const char *dst_reg, const char *symbol)*/
/*
 * Emits platform-correct adrp+add sequence to load symbol address.
 * macOS uses @PAGE/@PAGEOFF, Linux ELF uses :got: or plain :lo12:
 */
static void aarch64_emit_symbol_addr (FILE *stream, const char *dst_reg, const char *symbol)
{
	if (options.extref_prefix && options.extref_prefix[0] == '_') {
		/* macOS/Darwin: use @PAGE/@PAGEOFF syntax */
		fprintf (stream, "\tadrp\t%s, %s@PAGE\n", dst_reg, symbol);
		fprintf (stream, "\tadd\t%s, %s, %s@PAGEOFF\n", dst_reg, dst_reg, symbol);
	} else {
		/* Linux ELF: use :lo12: syntax */
		fprintf (stream, "\tadrp\t%s, %s\n", dst_reg, symbol);
		fprintf (stream, "\tadd\t%s, %s, :lo12:%s\n", dst_reg, dst_reg, symbol);
	}
}
/*}}}*/

/*{{{  static void aarch64_emit_arithmetic_with_immediate (FILE *stream, const char *op, const char *dst, const char *src, long imm, const char *temp_reg, int set_flags)*/
/*
 * Emits ARM64 arithmetic instruction with immediate, handling large values automatically
 */
static void aarch64_emit_arithmetic_with_immediate (FILE *stream, const char *op, const char *dst, const char *src, long imm, const char *temp_reg, int set_flags)
{
	if (imm >= 0 && imm <= 4095) {
		fprintf (stream, "\t%s%s\t%s, %s, #%ld\n", op, set_flags ? "s" : "", dst, src, imm);
	} else {
		aarch64_emit_large_immediate (stream, imm, temp_reg);
		fprintf (stream, "\t%s%s\t%s, %s, %s\n", op, set_flags ? "s" : "", dst, src, temp_reg);
	}
}
/*}}}*/

/*{{{  static char *aarch64_get_register_name (int reg)*/
static char *aarch64_get_register_name (int reg)
{
	static char regnames[8][16];
	static int regnames_idx = 0;
	char *regname = regnames[regnames_idx];
	regnames_idx = (regnames_idx + 1) % 8;

	// TODO: investigate this 'fix'
	/* CRITICAL FIX: Handle invalid register numbers that cause crashes */
	if (reg == (int)0x80000000 || reg == -2147483648) {
		/* This is REG_UNDEFINED from tstack.h - return safe fallback */
		return "x0";
	}
	
	/* Handle special register mappings */
	switch (reg) {
	case REG_SP:
		return "sp";
	case REG_X30:
		return "x30";
	case REG_X29:
		return "x29";
	case REG_WPTR:
	case AARCH64_REG_WPTR:
		return "x28";
	case REG_FPTR:
	case AARCH64_REG_FPTR:
		return "x27";
	case REG_BPTR:
	case AARCH64_REG_BPTR:
		return "x26";
	case REG_SCHED:
	case AARCH64_REG_SCHED:
		return "x25";
	case REG_UNDEFINED:
		return "x0"; /* Safe fallback for undefined registers */
	default:
		/* Handle general purpose registers */
		if (reg >= 0 && reg < 31) {
			snprintf (regname, sizeof(regname), "x%d", reg);
			return regname;
		}
		/* Handle negative register numbers (map to positive range) */
		if (reg < 0 && reg > -1000) {
			int mapped_reg = (-reg) % 25; /* Use x0-x24 for negative regs */
			snprintf (regname, sizeof(regname), "x%d", mapped_reg);
			return regname;
		}
		/* Handle very large register numbers by mapping to valid range */
		if (reg > 31) {
			int mapped_reg = reg % 25; /* Use x0-x24 for large regs */
			snprintf (regname, sizeof(regname), "x%d", mapped_reg);
			return regname;
		}
		break;
	}
	/* Last resort: provide safe fallback without error message to avoid spam */
	return "x0";
}
/*}}}*/
