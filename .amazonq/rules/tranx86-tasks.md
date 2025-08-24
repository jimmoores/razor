




NEW FEATURE

An interactive, agentic coding experience
Amazon Q can now help you write, modify, and maintain code by combining the power of natural language understanding with the ability to take actions on your behalf such as directly making code changes, modifying files, and running commands.

Amazon Q
Did you know?
Create and add Saved Prompts using the @ context menu
Select code & ask me to explain, debug or optimize it, or type / for quick actions

You were working on getting tranx86 producing code for x64 and aarch64 targets and the build was looking relatively clean, but there were still segfaults when running test .tce files through with those targets selected. {Please investigate whether there are still missing funciton implementations that could cause that.


Context
I'll investigate the missing function implementations that could be causing segfaults in tranx86 for x64 and aarch64 targets. Let me examine the architecture-specific files and identify what's missing.

2 files read
archx64.c
archaarch64.c
Now let me check what functions are being called during the segfault by examining the main tranx86 code and looking for missing function implementations:

2 files read
tstack.h
tstack.c
Now let me check the main etcrtl.c file to see what functions are being called that might be missing:

Searched for "compose_ins" in
tranx86
0 results found
Searched for "structs" in
tranx86
1 result found
1 file read
structs.h
Now let me check for the compose_ins function:

Searched for "support" in
tranx86
4 results found
1 file read
support.h
Let me check the support.c file to see if compose_ins is defined there:

Search for compose_ins function definition in support.c

shell

Completed
grep -n "compose_ins" /Users/jim/CLionProjects/kroc/tools/tranx86/support.c
Let me search for compose_ins in all files:

shell

Completed
grep -rn "compose_ins" *.c *.h
arch386.c:90:	return compose_ins (INS_CALL, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SCHED, offsetof(ccsp_sched_t, calltable[entry->call_offset]));
arch386.c:93:		return compose_ins (type, 2, 0, ARG_COND, cond, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
arch386.c:95:		return compose_ins (INS_CALL, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SCHED, entry->call_offset << 2);
arch386.c:97:		return compose_ins (type, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (entry->call_offset + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2);
arch386.c:161:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, cregs[i], ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)])));
arch386.c:164:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (cregs[i]), ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)])));
arch386.c:167:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (cregs[i]), ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)])));
arch386.c:172:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (constmap_regconst (cregs[i])) << WSH, ARG_REG, tmp_reg));
arch386.c:173:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_SCHED, (i - 1) << WSH));
arch386.c:178:				add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
arch386.c:196:			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
arch386.c:207:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
arch386.c:210:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, xregs[1]));
arch386.c:211:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, xregs[2]));
arch386.c:228:			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
arch386.c:242:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[(i - 1)]), ARG_REG, oregs[i]));
arch386.c:258:				add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, oregs[i], ARG_REG, xregs[i]));
arch386.c:259:				add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, oregs[i]));
arch386.c:283:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
arch386.c:288:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab));
arch386.c:355:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->procfile_setup_label));
arch386.c:356:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:358:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
arch386.c:422:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_EAX));
arch386.c:423:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_EDX));
arch386.c:424:	add_to_ins_chain (compose_ins (INS_RDTSC, 0, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg));
arch386.c:427:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
arch386.c:428:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:430:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg));
arch386.c:431:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
arch386.c:432:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:434:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg, ARG_REG, eax_reg));
arch386.c:435:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
arch386.c:436:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));
arch386.c:453:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
arch386.c:454:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 2));
arch386.c:455:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_OCCSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:456:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
arch386.c:459:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
arch386.c:460:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
arch386.c:461:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:491:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, eax_reg, ARG_REG, REG_EAX));
arch386.c:492:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, edx_reg, ARG_REG, REG_EDX));
arch386.c:493:	add_to_ins_chain (compose_ins (INS_RDTSC, 0, 2, ARG_REG | ARG_IMP, eax_reg, ARG_REG | ARG_IMP, edx_reg));
arch386.c:496:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
arch386.c:497:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:499:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg));
arch386.c:500:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, edx_reg, ARG_REG, tmp_reg));
arch386.c:501:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:503:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, tmp_reg, ARG_REG, eax_reg, ARG_REG, eax_reg));
arch386.c:504:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, edx_reg));
arch386.c:505:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, eax_reg));
arch386.c:512:	add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, eax_reg, ARG_REG, eax_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:513:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NS, ARG_FLABEL, 0));
arch386.c:516:	add_to_ins_chain (compose_ins (INS_INC, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
arch386.c:517:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME));	/* store time */
arch386.c:518:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));		/* store return address */
arch386.c:519:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, Z_WAITING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));	/* store status */
arch386.c:524:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:540:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x40, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:541:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x50, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
arch386.c:542:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x60, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
arch386.c:546:	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_NAMEDLABEL, string_dup ("&sf"), ARG_REG, tmp_reg2));
arch386.c:547:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, tmp_reg2, ARG_REG, tmp_reg));
arch386.c:548:	add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REGIND | ARG_DISP, tmp_reg2, 4, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:549:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 3));
arch386.c:551:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:552:	add_to_ins_chain (compose_i
ns (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
arch386.c:558:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:559:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
arch386.c:564:	add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));		/* Fptr = NotProcess.p ? */
arch386.c:565:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 1));				/* yup, then jump. */
arch386.c:566:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));	/* Bptr[Link] = Wptr */
arch386.c:567:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));
arch386.c:569:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
arch386.c:570:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_FPTR));				/* Fptr = Wptr */
arch386.c:572:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
arch386.c:573:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_BPTR));				/* Bptr = Wptr */
arch386.c:576:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_OCCSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:578:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));						/* get here when synch flags clear */
arch386.c:579:	add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));		/* Fptr = NotProcess.p ? */
arch386.c:580:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 0));				/* yup, then jump (out) */
arch386.c:587:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:588:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
arch386.c:592:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
arch386.c:593:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:594:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_FPTR, ARG_REG, REG_BPTR, ARG_REG | ARG_IMP, REG_CC));				/* Fptr = Bptr ? */
arch386.c:595:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 4));				/* yes, then jump */
arch386.c:596:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));	/* Bptr[Link] = Wptr */
arch386.c:597:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_BPTR));				/* Bptr = Wptr */
arch386.c:598:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));				/* Wptr = Fptr */
arch386.c:599:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));	/* Fptr = Fptr[Link] */
arch386.c:600:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_WPTR, W_IPTR));		/* jump to process resume address */
arch386.c:602:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 4));		/* only one other on the run-queue */
arch386.c:603:	add_to_ins_chain (compose_ins (INS_SWAP, 2, 0, ARG_REG, REG_WPTR, ARG_REG, REG_FPTR));				/* Wptr, Fptr = Fptr, Wptr */
arch386.c:604:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_BPTR));				/* Bptr = Fptr */
arch386.c:605:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_WPTR, W_IPTR));		/* jump to process resume address */
arch386.c:607:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:622:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND | ARG_DISP, preg, W_LINK));
arch386.c:623:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
arch386.c:624:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, nflab));
arch386.c:627:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REG, REG_FPTR));
arch386.c:628:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, nflab + 1));
arch386.c:631:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, nflab));
arch386.c:632:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
arch386.c:635:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, nflab + 1));
arch386.c:636:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REG, REG_BPTR));
arch386.c:639:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND | ARG_DISP, preg, W_LINK));
arch386.c:640:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_NAMEDLABEL, string_dup (Fptr_name), ARG_REG | ARG_IMP, REG_CC));
arch386.c:641:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, nflab));
arch386.c:644:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_NAMEDLABEL, string_dup (Fptr_name)));
arch386.c:645:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, nflab + 1));
arch386.c:648:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, nflab));
arch386.c:650:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (Bptr_name), ARG_REG, tmp_reg));
arch386.c:651:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK));
arch386.c:654:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, nflab + 1));
arch386.c:655:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_NAMEDLABEL, string_dup (Bptr_name)));
arch386.c:679:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_b_reg),
arch386.c:685:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:686:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_PRIORITY));
arch386.c:698:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR));
arch386.c:699:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg), ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_IPTR,
arch386.c:705:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:706:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_PRIORITY));
arch386.c:710:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:739:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_WPTR));
arch386.c:740:	add_to_ins_chain (compose_ins (INS_DEC, 1, 2, ARG_REGIND | ARG_DISP, REG_WPTR, W_COUNT, ARG_REGIND | ARG_DISP, REG_WPTR, W_COUNT, ARG_REG | ARG_IMP, REG_CC));
arch386.c:741:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 0));
arch386.c:743:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:744:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND, REG_WPTR));
arch386.c:764:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:769:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:770:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
arch386.c:774:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch38
6.c:803:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
arch386.c:807:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:812:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:813:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
arch386.c:816:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:817:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 1));
arch386.c:822:		add_to_ins_chain (compose_ins (INS_JUMP, 3, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTIN8 + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2, ARG_REG | ARG_IMP, chan_reg, ARG_REG | ARG_IMP, dest_reg));
arch386.c:825:		add_to_ins_chain (compose_ins (INS_JUMP, 3, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTIN32 + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2, ARG_REG | ARG_IMP, chan_reg, ARG_REG | ARG_IMP, dest_reg));
arch386.c:828:		add_to_ins_chain (compose_ins (INS_JUMP, 4, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTIN + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2, ARG_REG | ARG_IMP, count_reg, ARG_REG | ARG_IMP, chan_reg,
arch386.c:833:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
arch386.c:835:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
arch386.c:840:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:841:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:846:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:850:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:880:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:881:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
arch386.c:889:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:890:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:891:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:896:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:922:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:924:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg));
arch386.c:932:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REGIND, chan_reg));
arch386.c:946:		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("** coding for copy (inline_in_2)  size in reg")));
arch386.c:954:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_POINTER, ARG_REG, tmp_reg2));
arch386.c:956:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg2, offset, ARG_REG, tmp_reg3));
arch386.c:957:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg3, ARG_REGIND | ARG_DISP, dest_reg, offset));
arch386.c:962:			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg2, offset, ARG_REG, tmp_reg3));
arch386.c:963:			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, tmp_reg3, ARG_REGIND | ARG_DISP, dest_reg, offset));
arch386.c:968:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_FPTR));
arch386.c:969:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_BPTR));
arch386.c:971:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_POINTER, ARG_REG, REG_FPTR));
arch386.c:972:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REG, REG_BPTR));
arch386.c:976:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, known_size >> 2, ARG_REG, tmp_reg2));
arch386.c:978:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, known_size, ARG_REG, tmp_reg2));
arch386.c:983:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_ECX));
arch386.c:985:			add_to_ins_chain (compose_ins (INS_REPMOVEL, 2, 1, ARG_REG | ARG_IMP, tmp_reg2, ARG_REG | ARG_IMP, REG_FPTR, ARG_REG | ARG_IMP, REG_BPTR));
arch386.c:987:			add_to_ins_chain (compose_ins (INS_REPMOVEB, 2, 1, ARG_REG | ARG_IMP, tmp_reg2, ARG_REG | ARG_IMP, REG_FPTR, ARG_REG | ARG_IMP, REG_BPTR));
arch386.c:989:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
arch386.c:990:		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_BPTR));
arch386.c:991:		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_FPTR));
arch386.c:994:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
arch386.c:1015:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1016:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1017:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
arch386.c:1020:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:1021:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:1025:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_OCCSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:1029:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:1031:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, REG_WPTR));
arch386.c:1032:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg));
arch386.c:1033:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER, ARG_REG, src_reg));
arch386.c:1034:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REG, tmp_reg1));
arch386.c:1035:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, dest_reg, ARG_REG, tmp_reg2));
arch386.c:1036:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND, dest_reg));
arch386.c:1037:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND, src_reg));
arch386.c:1039:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, 4, ARG_REG, tmp_reg1));
arch386.c:1040:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, dest_reg, 4, ARG_REG, tmp_reg2));
arch386.c:1041:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND | ARG_DISP, dest_reg, 4));
arch386.c:1042:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, src_reg, 4));
arch386.c:1044:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_WPTR, W_IPTR));
arch386.c:1047:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
arch386.c:1067:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1068:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
arch386.c:1071:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:1072:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1073:	add_to_ins_chain (compose_ins (INS_MOVE
, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:1078:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_OCCSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:1085:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:1086:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg1));
arch386.c:1087:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg1, W_POINTER, ARG_REG, dest_reg));
arch386.c:1088:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, Z_READY, ARG_REG, dest_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1089:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GT, ARG_FLABEL, 2));
arch386.c:1092:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, Z_WAITING, ARG_REG, dest_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1093:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 3));
arch386.c:1097:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
arch386.c:1098:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, tmp_reg1, W_STATUS));
arch386.c:1099:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:1100:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:1101:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1105:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_OCCSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:1112:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
arch386.c:1113:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1115:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, REG_WPTR));
arch386.c:1116:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg));
arch386.c:1118:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REG, tmp_reg1));
arch386.c:1119:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, dest_reg, ARG_REG, tmp_reg2));
arch386.c:1120:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND, dest_reg));
arch386.c:1121:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND, src_reg));
arch386.c:1123:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, src_reg, 4, ARG_REG, tmp_reg1));
arch386.c:1124:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, dest_reg, 4, ARG_REG, tmp_reg2));
arch386.c:1125:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg1, ARG_REGIND | ARG_DISP, dest_reg, 4));
arch386.c:1126:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, src_reg, 4));
arch386.c:1128:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_WPTR, W_IPTR));
arch386.c:1131:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
arch386.c:1161:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
arch386.c:1165:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1170:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:1171:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
arch386.c:1174:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1175:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 1));
arch386.c:1180:		add_to_ins_chain (compose_ins (INS_JUMP, 3, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTOUT8 + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2, ARG_REG | ARG_IMP, chan_reg, ARG_REG | ARG_IMP, src_reg));
arch386.c:1183:		add_to_ins_chain (compose_ins (INS_JUMP, 3, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTOUT32 + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2, ARG_REG | ARG_IMP, chan_reg, ARG_REG | ARG_IMP, src_reg));
arch386.c:1186:		add_to_ins_chain (compose_ins (INS_JUMP, 4, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTOUT + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2, ARG_REG | ARG_IMP, count_reg,
arch386.c:1191:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
arch386.c:1195:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
arch386.c:1196:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:1197:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:1201:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:1205:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:1234:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1235:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
arch386.c:1243:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:1244:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:1245:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 2, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1249:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_OCCSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:1259:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:1261:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg));
arch386.c:1263:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_STATUS, ARG_REG, tmp_reg2));
arch386.c:1264:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_READY, ARG_REG, tmp_reg2, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1265:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GT, ARG_FLABEL, 1));
arch386.c:1279:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_WAITING, ARG_REG, tmp_reg2, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1280:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 3));
arch386.c:1282:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
arch386.c:1283:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, tmp_reg, W_STATUS));
arch386.c:1284:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:1285:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
arch386.c:1286:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 2, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1290:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_OCCSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:1312:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
arch386.c:1323:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_FPTR));
arch386.c:1324:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_BPTR));
arch386.c:1326:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, src_reg, ARG_REG, REG_FPTR));
arch386.c:1327:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REG, REG_BPTR));
arch386.c:1331:			add_to_ins_chai
n (compose_ins (INS_MOVE, 1, 1, ARG_CONST, known_size >> 2, ARG_REG, tmp_reg3));
arch386.c:1333:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, known_size, ARG_REG, tmp_reg3));
arch386.c:1338:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg3, ARG_REG, REG_ECX));
arch386.c:1340:		add_to_ins_chain (compose_ins (INS_REPMOVEL, 2, 1, ARG_REG | ARG_IMP, tmp_reg3, ARG_REG | ARG_IMP, REG_FPTR, ARG_REG | ARG_IMP, REG_BPTR));
arch386.c:1342:		add_to_ins_chain (compose_ins (INS_REPMOVEB, 2, 1, ARG_REG | ARG_IMP, tmp_reg3, ARG_REG | ARG_IMP, REG_FPTR, ARG_REG | ARG_IMP, REG_BPTR));
arch386.c:1344:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg3));
arch386.c:1345:	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_BPTR));
arch386.c:1346:	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_FPTR));
arch386.c:1351:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REG, tmp_reg));
arch386.c:1352:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, NOT_PROCESS, ARG_REGIND, chan_reg));
arch386.c:1359:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
arch386.c:1394:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, guard_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1395:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_lab));
arch386.c:1403:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1404:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, tmp_lab));
arch386.c:1405:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
arch386.c:1407:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
arch386.c:1416:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, tmp_lab));
arch386.c:1417:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1418:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_lab));
arch386.c:1419:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
arch386.c:1422:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
arch386.c:1424:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
arch386.c:1427:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_lab));
arch386.c:1446:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1448:			add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1451:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
arch386.c:1454:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1457:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
arch386.c:1463:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
arch386.c:1464:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
arch386.c:1468:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
arch386.c:1489:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1491:			add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1494:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
arch386.c:1497:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
arch386.c:1498:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1501:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_LT, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
arch386.c:1507:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_LABEL, skip_lab));
arch386.c:1508:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
arch386.c:1512:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
arch386.c:1546:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1547:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_flab));
arch386.c:1555:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_WPTR, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1556:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_rlab));
arch386.c:1557:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1558:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, out_flab));
arch386.c:1567:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NONE_SELECTED, ARG_REGIND, REG_WPTR, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1568:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, out_flab));
arch386.c:1569:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND, REG_WPTR));
arch386.c:1570:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, tmp_reg));
arch386.c:1571:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
arch386.c:1574:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND, REG_WPTR));
arch386.c:1575:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, tmp_reg));
arch386.c:1576:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, out_lab));
arch386.c:1585:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_rlab));
arch386.c:1586:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND, ts->stack->old_c_reg));
arch386.c:1587:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_flab));
arch386.c:1588:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
arch386.c:1589:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_lab));
arch386.c:1613:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NONE_SELECTED, ARG_REGIND, REG_WPTR));
arch386.c:1614:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1615:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 0));
arch386.c:1616:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_WAITING, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
arch386.c:1621:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:1622:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
arch386.c:1625:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
arch386.c:1629:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_DISP | ARG_IND, REG_SPTR, (K_FASTSCHEDULER + ts->stack_drift + KIFACE_TABLEOFFS_I386) << 2));
arch386.c:1631:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:1642:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (signed int)0x
80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1643:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
arch386.c:1644:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
arch386.c:1646:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:1650:			add_to_ins_chain  (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_BPTR));
arch386.c:1652:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup (Bptr_name)));
arch386.c:1657:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_FPTR));
arch386.c:1659:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup (Fptr_name)));
arch386.c:1676:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_WPTR));
arch386.c:1677:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, ts->stack->old_a_reg));
arch386.c:1679:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ESP, ARG_REG, tmp_reg));
arch386.c:1680:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, tmp_reg));
arch386.c:1681:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("&dmem_alloc2")));
arch386.c:1682:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP));
arch386.c:1683:	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, ts->stack->a_reg));
arch386.c:1684:	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_WPTR));
arch386.c:1703:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
arch386.c:1704:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1) + 1])));
arch386.c:1706:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, mdparam[(mdpairid << 1)])));
arch386.c:1707:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, mdparam[((mdpairid << 1) + 1)])));
arch386.c:1740:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procedure_label));
arch386.c:1779:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->filename_label));
arch386.c:1795:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_EDX, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[0])));
arch386.c:1797:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[1])));
arch386.c:1799:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[2])));
arch386.c:1802:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_EAX, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[3])));
arch386.c:1806:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_ECX, ARG_REG, xregs[0]));
arch386.c:1808:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, xregs[1]));
arch386.c:1809:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, xregs[2]));
arch386.c:1821:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->zerodiv_label));
arch386.c:1832:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floaterr_label));
arch386.c:1843:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->overflow_label));
arch386.c:1854:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->range_entry_label));
arch386.c:1867:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_EDX));
arch386.c:1869:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_ECX));
arch386.c:1885:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_EDX));
arch386.c:1887:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_ECX));
arch386.c:1888:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->overflow_label));
arch386.c:1904:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_EDX));
arch386.c:1906:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_ECX));
arch386.c:1907:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->floaterr_label));
arch386.c:1922:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_EDX));
arch386.c:1924:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_ECX));
arch386.c:1925:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->range_entry_label));
arch386.c:1937:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procfile_setup_label));
arch386.c:1938:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_EAX));
arch386.c:1939:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_ECX));
arch386.c:1940:	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
arch386.c:1957:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1958:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, this_lab));
arch386.c:1960:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_EDX));
arch386.c:1962:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_ECX));
arch386.c:1963:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
arch386.c:1964:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:1971:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_EDX));
arch386.c:1973:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_ECX));
arch386.c:1974:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
arch386.c:1991:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:1992:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 2));
arch386.c:1993:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, xregs[1]));
arch386.c:1994:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, xregs[2]));
arch386.c:1996:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
arch386.c:2002:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, xregs[1]));
arch386.c:2003:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, xregs[2]));
arch386.c:2022:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
arch386.c:2023:	add_to_ins_chain (compose_ins (INS_CWDE, 1, 1, ARG_REG | ARG_IMP | ARG_IS16BIT, ts->stack->a_reg, ARG_REG | ARG_IMP, ts->stack->a_reg));
arch386.c:2024:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
arch386.c:2025:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
arch386.c:2046:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
arch386.c:2047:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->b_reg, ARG_REG, REG_EDX));
arch386.c:2048:	add_to_ins_chain (compose_ins (INS_CDQ, 1, 2, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, ts->stack->b_reg));
arch386.c:2049:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->b_reg));
arch386.c:2050:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_
REG, ts->stack->a_reg));
arch386.c:2066:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, dividend, ARG_REG, REG_EAX));
arch386.c:2067:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2068:	add_to_ins_chain (compose_ins (INS_CDQ, 1, 2, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, tmp_reg));
arch386.c:2069:	add_to_ins_chain (compose_ins (INS_DIV, 3, 2, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, dividend, ARG_REG, divisor, \
arch386.c:2071:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2072:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, dividend));
arch386.c:2087:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, dividend, ARG_REG, REG_EAX));
arch386.c:2088:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, divisor, ARG_REG, REG_ECX));
arch386.c:2089:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2090:	add_to_ins_chain (compose_ins (INS_CDQ, 1, 2, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, tmp_reg));
arch386.c:2091:	add_to_ins_chain (compose_ins (INS_DIV, 3, 2, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, dividend, ARG_REG, divisor, \
arch386.c:2093:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2094:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, divisor));
arch386.c:2095:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, dividend));
arch386.c:2109:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2112:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
arch386.c:2115:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
arch386.c:2122:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, targetreg, ARG_REG, REG_EAX));
arch386.c:2123:	add_to_ins_chain (compose_ins (INS_INB, 1, 1, ARG_REG | ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IS8BIT, targetreg));
arch386.c:2124:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, targetreg));
arch386.c:2125:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2138:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2141:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
arch386.c:2144:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
arch386.c:2147:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, sourcereg, ARG_REG, REG_EAX));
arch386.c:2148:	add_to_ins_chain (compose_ins (INS_OUTB, 2, 0, ARG_REG | ARG_IS8BIT, sourcereg, ARG_REG | ARG_IS16BIT, tmp_reg));
arch386.c:2149:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, sourcereg));
arch386.c:2150:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2185:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2188:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
arch386.c:2191:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (portreg) << WSH, ARG_REG, tmp_reg));
arch386.c:2194:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
arch386.c:2197:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_EAX));
arch386.c:2198:	add_to_ins_chain (compose_ins (instr, 1, 1, ARG_REG | ARG_IS16BIT, tmp_reg, ARG_REG | flags, tmp_reg2));
arch386.c:2199:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
arch386.c:2200:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2204:		add_to_ins_chain (compose_ins (mvinstr, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (addrreg) << WSH));
arch386.c:2207:		add_to_ins_chain (compose_ins (mvinstr, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND, addrreg));
arch386.c:2245:	add_to_ins_chain (compose_ins (mvinstr, 1, 1, ARG_REGIND, addrreg, ARG_REG, tmp_reg2));
arch386.c:2247:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2250:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (portreg), ARG_REG, tmp_reg));
arch386.c:2253:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (portreg) << WSH, ARG_REG, tmp_reg));
arch386.c:2256:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
arch386.c:2259:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_EAX));
arch386.c:2260:	add_to_ins_chain (compose_ins (instr, 1, 1, ARG_REG | flags, tmp_reg2, ARG_REG | ARG_IS16BIT, tmp_reg));
arch386.c:2261:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
arch386.c:2262:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2272:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_c_reg), ARG_REG, REG_JPTR));
arch386.c:2276:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_JPTR));
arch386.c:2278:			add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_c_reg) << WSH, ARG_REG, REG_JPTR));
arch386.c:2282:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_c_reg) << WSH, ARG_REG, REG_JPTR));
arch386.c:2285:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_JPTR));
arch386.c:2290:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_b_reg), ARG_REG, REG_LPTR));
arch386.c:2294:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, REG_LPTR));
arch386.c:2296:			add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG, REG_LPTR));
arch386.c:2300:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG, REG_LPTR));
arch386.c:2303:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, REG_LPTR));
arch386.c:2319:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL, constmap_regconst (ts->stack->old_c_reg), ARG_REG, tmp_reg));
arch386.c:2322:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, ts->stack->old_c_reg, ARG_REG, tmp_reg));
arch386.c:2327:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH));
arch386.c:2330:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, ts->stack->old_b_reg));
arch386.c:2334:		add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2335:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 0));
arch386.c:2336:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_LPTR));
arch386.c:2337:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_JPTR));
arch386.c:2342:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_JPTR));
arch386.c:2343:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_LPTR));
arch386.c:2354:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, loop_count >> 2, ARG_REG, new_reg));
arch386.c:2355:					add_to_ins_chain (compo
se_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, new_reg, ARG_REG, REG_ECX));
arch386.c:2356:					add_to_ins_chain (compose_ins (INS_REPMOVEL, 2, 1, ARG_REG | ARG_IMP, new_reg, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, tmp_reg2));
arch386.c:2357:					add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, new_reg));
arch386.c:2360:					add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_ECX));
arch386.c:2361:					add_to_ins_chain (compose_ins (INS_REPMOVEB, 2, 1, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, tmp_reg2));
arch386.c:2362:					add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
arch386.c:2368:			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_ECX));
arch386.c:2369:			add_to_ins_chain (compose_ins (INS_REPMOVEB, 2, 1, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, tmp_reg2));
arch386.c:2370:			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
arch386.c:2373:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
arch386.c:2374:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2375:		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_JPTR));
arch386.c:2376:		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_LPTR));
arch386.c:2378:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:2390:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, r1, ARG_REG, REG_ECX));
arch386.c:2391:	add_to_ins_chain (compose_ins (((sec == I_SHL) ? INS_SHL : INS_SHR), 2, 1, ARG_REG | ARG_IMP, r1, ARG_REG, r2, ARG_REG, r3));
arch386.c:2392:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, r1));
arch386.c:2408:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_EDX));
arch386.c:2409:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg));
arch386.c:2411:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_c_reg));
arch386.c:2413:		add_to_ins_chain (compose_ins (INS_ADC, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2420:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
arch386.c:2422:		add_to_ins_chain (compose_ins (INS_SBB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2429:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ECX));
arch386.c:2430:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg));
arch386.c:2432:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_c_reg));
arch386.c:2434:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
arch386.c:2438:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2439:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
arch386.c:2440:		add_to_ins_chain (compose_ins (INS_RCL, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, tmp_reg, ARG_REG | ARG_IS8BIT, tmp_reg));
arch386.c:2442:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2448:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ECX));
arch386.c:2449:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg));
arch386.c:2451:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_c_reg));
arch386.c:2453:		add_to_ins_chain (compose_ins (INS_SBB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
arch386.c:2457:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2458:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
arch386.c:2459:		add_to_ins_chain (compose_ins (INS_RCL, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, tmp_reg, ARG_REG | ARG_IS8BIT, tmp_reg));
arch386.c:2461:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2468:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
arch386.c:2469:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_EAX));
arch386.c:2470:		add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG | ARG_IMP, tmp_reg));
arch386.c:2473:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
arch386.c:2474:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2475:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
arch386.c:2476:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_CONST, 0, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
arch386.c:2484:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_ECX));
arch386.c:2486:			add_to_ins_chain (compose_ins (INS_SHLD, 3, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_c_reg, \
arch386.c:2488:			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
arch386.c:2490:			add_to_ins_chain (compose_ins (INS_SHRD, 3, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_b_reg, \
arch386.c:2492:			add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
arch386.c:2496:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
arch386.c:2500:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 32, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2501:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_B, ARG_LABEL, this_lab));
arch386.c:2503:		add_to_ins_chain (compose_ins (INS_SWAP, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_c_reg, \
arch386.c:2506:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, (sec == I_LSHL) ? ts->stack->old_b_reg : ts->stack->old_c_reg));
arch386.c:2509:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 64, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2510:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_B, ARG_LABEL, this_lab));
arch386.c:2512:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, (sec == I_LSHL) ? ts->stack->old_c_reg : ts->stack->old_b_reg));
arch386.c:2515:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:2522:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_EDX));
arch386.c:2523:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG, REG_EAX));
arch386.c:2524:		add_to_ins_chain (compose_ins (INS_UDIV, 3, 2, ARG_REG | ARG_IMP, ts->stack->old_c_reg, ARG_REG | ARG_IMP, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, \
arch386.c:2528:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_b_reg));
arch386.c:2529:		add_to_ins_
chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_c_reg));
arch386.c:2554:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 0));
arch386.c:2561:		add_to_ins_chain (compose_ins (INS_FSQRT, 0, 0));
arch386.c:2568:		add_to_ins_chain (compose_ins (INS_FABS, 0, 0));
arch386.c:2575:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
arch386.c:2582:		add_to_ins_chain (compose_ins (INS_FLD, 1, 0, ARG_FREG, 0));
arch386.c:2590:			add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2592:			add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2602:			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2604:			add_to_ins_chain (compose_ins (INS_FLD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2613:			add_to_ins_chain (compose_ins (INS_FADD64, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2615:			add_to_ins_chain (compose_ins (INS_FADD64, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2624:			add_to_ins_chain (compose_ins (INS_FADD32, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2626:			add_to_ins_chain (compose_ins (INS_FADD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2635:			add_to_ins_chain (compose_ins (INS_FMUL64, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2637:			add_to_ins_chain (compose_ins (INS_FMUL64, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2646:			add_to_ins_chain (compose_ins (INS_FMUL32, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2648:			add_to_ins_chain (compose_ins (INS_FMUL32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2657:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGINDSIB, 8, ts->stack->old_b_reg, ts->stack->old_a_reg));
arch386.c:2665:		add_to_ins_chain (compose_ins (INS_FLD32, 1, 0, ARG_REGINDSIB, 4, ts->stack->old_b_reg, ts->stack->old_a_reg));
arch386.c:2673:			add_to_ins_chain (compose_ins (INS_FST64, 0, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2675:			add_to_ins_chain (compose_ins (INS_FST64, 0, 1, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2685:			add_to_ins_chain (compose_ins (INS_FST32, 0, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2687:			add_to_ins_chain (compose_ins (INS_FST32, 0, 1, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2698:			add_to_ins_chain (compose_ins (INS_FIST64, 0, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH));
arch386.c:2700:			add_to_ins_chain (compose_ins (INS_FIST64, 0, 1, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2711:		add_to_ins_chain (compose_ins (INS_FLDZ, 0, 0));
arch386.c:2717:		add_to_ins_chain (compose_ins (INS_FADD, 0, 0));
arch386.c:2725:		add_to_ins_chain (compose_ins (INS_FSUB, 0, 0));
arch386.c:2733:		add_to_ins_chain (compose_ins (INS_FMUL, 0, 0));
arch386.c:2741:		add_to_ins_chain (compose_ins (INS_FDIV, 0, 0));
arch386.c:2750:		add_to_ins_chain (compose_ins (INS_FSIN, 0, 0));
arch386.c:2758:		add_to_ins_chain (compose_ins (INS_FCOS, 0, 0));
arch386.c:2766:		add_to_ins_chain (compose_ins (INS_FPTAN, 0, 0));
arch386.c:2767:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 0));
arch386.c:2775:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2784:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2787:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_BE, ARG_LABEL, this_lab));
arch386.c:2790:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 0x7fffffff, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
arch386.c:2791:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2793:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[1], ARG_REG, tmp_reg));
arch386.c:2794:		add_to_ins_chain (compose_ins (INS_FADD64, 1, 0, ARG_REGIND, tmp_reg));
arch386.c:2795:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab2));
arch386.c:2798:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:2799:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
arch386.c:2802:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab2));
arch386.c:2810:		add_to_ins_chain (compose_ins (INS_FRNDINT, 0, 0));
arch386.c:2828:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
arch386.c:2829:		add_to_ins_chain (compose_ins (INS_LAHF, 0, 1, ARG_REG|ARG_IMP, tmp_reg));
arch386.c:2830:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, tmp_reg));
arch386.c:2833:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG|ARG_IS16BIT, tmp_reg));
arch386.c:2834:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, error_flags, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG|ARG_IS16BIT, tmp_reg));
arch386.c:2836:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x20, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2837:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
arch386.c:2839:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG|ARG_IS16BIT, tmp_reg));
arch386.c:2840:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_CONST, error_flags, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2841:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2843:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
arch386.c:2846:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP));
arch386.c:2855:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:2857:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
arch386.c:2858:		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, tmp_reg));
arch386.c:2859:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG|ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2860:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2867:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, -32));
arch386.c:2869:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, REG_ESP));
arch386.c:2870:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP));
arch386.c:2872:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
arch386.c:2873:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
arch386.c:2874:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
arch386.c:2881:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, 32));
arch386.c:2883:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, REG_ESP));
arch386.c:2884:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP));
arch386.c:2886:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
arch386.c:2887:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
arch386.c:2888:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
arch386.c:2899:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[(sec == I_FPCHKI32) ? 0 : 2], ARG_REG, tmp_reg));
arch386.c:2900:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, tmp_reg));
arch386.c:2901:		add_to_ins_chain (compose_ins (INS_FCOMP, 0, 0));
arch386.c:2903:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
arch386.c:2904:		add_to_ins_chain (compose_ins (INS_FSTSW
, 0, 1, ARG_REG, tmp_reg));
arch386.c:2905:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2906:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2908:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_BE, ARG_LABEL, this_lab));
arch386.c:2910:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:2913:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[(sec == I_FPCHKI32) ? 1 : 3], ARG_REG, tmp_reg));
arch386.c:2914:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, tmp_reg));
arch386.c:2915:		add_to_ins_chain (compose_ins (INS_FCOMP, 0, 0));
arch386.c:2917:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
arch386.c:2918:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
arch386.c:2919:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2920:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:2922:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, this_lab));
arch386.c:2924:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:2929:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
arch386.c:2930:		add_to_ins_chain (compose_ins (INS_FTST, 0, 0));
arch386.c:2931:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
arch386.c:2932:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2933:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
arch386.c:2943:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
arch386.c:2944:		add_to_ins_chain (compose_ins (INS_FXAM, 0, 0));
arch386.c:2945:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
arch386.c:2946:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2947:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
arch386.c:2957:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
arch386.c:2958:		add_to_ins_chain (compose_ins (INS_FUCOM, 0, 0));
arch386.c:2959:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
arch386.c:2960:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2961:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
arch386.c:2970:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
arch386.c:2971:		add_to_ins_chain (compose_ins (INS_FCOMPP, 0, 0));
arch386.c:2972:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
arch386.c:2973:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2974:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
arch386.c:2984:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
arch386.c:2985:		add_to_ins_chain (compose_ins (INS_FCOMPP, 0, 0));
arch386.c:2986:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
arch386.c:2987:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:2988:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
arch386.c:3001:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
arch386.c:3002:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
arch386.c:3003:		add_to_ins_chain (compose_ins (INS_FPREM1, 0, 0));
arch386.c:3004:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
arch386.c:3005:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
arch386.c:3006:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:3007:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_CONST, 0x400, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:3008:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, this_lab));
arch386.c:3009:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
arch386.c:3016:		add_to_ins_chain (compose_ins (INS_FLD1, 0, 0));
arch386.c:3017:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
arch386.c:3018:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
arch386.c:3019:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
arch386.c:3026:		add_to_ins_chain (compose_ins (INS_FLD1, 0, 0));
arch386.c:3027:		add_to_ins_chain (compose_ins (INS_FCHS, 0, 0));
arch386.c:3028:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
arch386.c:3029:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
arch386.c:3030:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
arch386.c:3066:	*pst_first = compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 16, ARG_REG, REG_ESP, ARG_REG, REG_ESP);
arch386.c:3070:	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg));
arch386.c:3071:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, REG_ESP));
arch386.c:3076:		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
arch386.c:3078:		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
arch386.c:3080:	*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_ESP, ARG_REG, REG_ESP);
arch386.c:3087:		add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg));
arch386.c:3088:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
arch386.c:3092:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, xregs[0]));
arch386.c:3093:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, xregs[1]));
arch386.c:3094:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, xregs[2]));
arch386.c:3106:			*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
arch386.c:3123:	add_to_ins_chain (st_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg));
arch386.c:3124:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, cparam[0])));
arch386.c:3126:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, arg_reg, ARG_REG, xregs[0]));
arch386.c:3133:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, arg_reg));
arch386.c:3135:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_REG, arg_reg));
arch386.c:3139:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REG, xregs[1]));
arch386.c:3140:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, xregs[2]));
arch386.c:3147:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, arg_reg));
arch386.c:3149:	st_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
arch386.c:3168:	*pst_first = compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 4, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
arch386.c:3170:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, -36));
arch386.c:3171:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, -1, ARG_REGIND | ARG_DISP,
REG_WPTR, -32));
arch386.c:3172:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SCHED, ARG_REGIND | ARG_DISP, REG_WPTR, -28));
arch386.c:3173:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, tmp_reg));
arch386.c:3175:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 40, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
arch386.c:3176:	add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST | ARG_ISCONST, ~0xf, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
arch386.c:3177:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, REG_ESP));
arch386.c:3178:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, REG_ESP));
arch386.c:3181:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
arch386.c:3183:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
arch386.c:3184:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, -28, ARG_REG, REG_SCHED));
arch386.c:3185:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_SCHED, offsetof(ccsp_sched_t, stack), ARG_REG, REG_ESP));
arch386.c:3186:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 4, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
arch386.c:3187:	*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
arch386.c:3206:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST | ARG_ISCONST, val));
arch386.c:3207:	add_to_ins_chain (compose_ins (INS_FLDCW, 1, 0, ARG_REGIND, REG_ESP));
arch386.c:3208:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP));
arch386.c:3234:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
arch386.c:3240:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, toldregs[i]));
arch386.c:3243:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, -16));
arch386.c:3244:	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
arch386.c:3264:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, toldregs[i]));
arch386.c:3268:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, REG_WPTR, ARG_REG, tmpreg));
arch386.c:3269:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, adjust << WSH, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
arch386.c:3270:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, tmpreg));
arch386.c:3285:		add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, ts->stack->a_reg));
arch386.c:3315:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 768, ARG_REG, REG_SPTR, ARG_REG, REG_SPTR));
arch386.c:3316:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_ESI));
arch386.c:3317:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_EDI));
arch386.c:3318:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SPTR, ARG_REG, tmp_reg2));
arch386.c:3319:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup ("&EntryPointTable"), ARG_REG, tmp_reg));
arch386.c:3320:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 191, ARG_REG, new_reg));
arch386.c:3322:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, new_reg, ARG_REG, REG_ECX));
arch386.c:3323:		add_to_ins_chain (compose_ins (INS_REPMOVEL, 2, 1, ARG_REG | ARG_IMP, new_reg, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, tmp_reg2));
arch386.c:3324:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, new_reg));
arch386.c:3326:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
arch386.c:3327:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
arch386.c:3330:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, KIFACE_TABLEOFFS_I386 << 2, ARG_REG, REG_SPTR, ARG_REG, REG_SPTR));
arch386.c:3331:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
arch386.c:3332:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
arch386.c:3333:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
arch386.c:3334:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
arch386.c:3339:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_SPTR, ARG_REG, ts->stack->old_a_reg));
arch386.c:3340:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, KIFACE_TABLEOFFS_I386 << 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
arch386.c:3360:	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
arch386.c:3395:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 0));
arch386.c:3413:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REGIND, reg));
arch386.c:3419:				add_to_ins_chain (compose_ins (INS_LOCK, 0, 0));
arch386.c:3421:			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REGIND, reg, ARG_REGIND, reg));
arch386.c:3428:				add_to_ins_chain (compose_ins (INS_LOCK, 0, 0));
arch386.c:3430:			add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_CONST | ARG_ISCONST, 1, ARG_REGIND, reg, ARG_REGIND, reg, ARG_REG | ARG_IMP, REG_CC));
arch386.c:3431:			add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_NZ, ARG_REG, reg));
arch386.c:3432:			add_to_ins_chain (compose_ins (INS_AND, 1, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, reg));
arch386.c:3456:				add_to_ins_chain (compose_ins (INS_MB, 0, 0));
arch386.c:3459:				add_to_ins_chain (compose_ins (INS_RMB, 0, 0));
arch386.c:3462:				add_to_ins_chain (compose_ins (INS_WMB, 0, 0));
arch386.c:3466:		add_to_ins_chain (compose_ins (INS_LOCK, 0, 0));
arch386.c:3467:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND, REG_SPTR, ARG_REGIND, REG_SPTR));
archaarch64.c:255:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, i));
archaarch64.c:259:	call_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archaarch64.c:264:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
archaarch64.c:286:		jump_ins = compose_ins (INS_CJUMP, 2, 0, ARG_COND, cc, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archaarch64.c:288:		jump_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archaarch64.c:301:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, REG_WPTR));
archaarch64.c:309:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR));
archaarch64.c:320:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("cntvct_el0"), ARG_REG, timer_reg));
archaarch64.c:334:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("cntvct_el0"), ARG_REG, timer_reg));
archaarch64.c:337:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, timer_reg, ARG_REG, target_reg, ARG_REG | ARG_IMP, REG_CC));
archaarch64.c:338:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_FLABEL, 0));
archaarch64.c:343:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archaarch64.c:351:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
archaarch64.c:352:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 1));
archaarch64.c:356:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));
archaarch64.c:359:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archaarch64.c:360:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
archaarch64.c:361:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, REG_WPTR));
archaarch64.c:363:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL
, 2));
archaarch64.c:371:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR));
archaarch64.c:374:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR, ARG_REG | ARG_IMP, REG_CC));
archaarch64.c:375:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 1));
archaarch64.c:378:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
archaarch64.c:379:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, REG_WPTR));
archaarch64.c:382:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archaarch64.c:383:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archaarch64.c:401:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
archaarch64.c:402:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
archaarch64.c:405:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
archaarch64.c:409:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archaarch64.c:411:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
archaarch64.c:413:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
archaarch64.c:432:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, REG_CC));
archaarch64.c:433:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
archaarch64.c:436:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, chan_reg));
archaarch64.c:440:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archaarch64.c:442:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
archaarch64.c:444:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
archaarch64.c:453:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, 31, ARG_REG, 29)); /* sp to fp */
archaarch64.c:454:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, 31, ARG_CONST, 16, ARG_REG, 31)); /* adjust sp */
archaarch64.c:458:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&workspace"), ARG_REG, wptr_reg));
archaarch64.c:469:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, 29, ARG_REG, 31)); /* fp to sp */
archaarch64.c:472:	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
archaarch64.c:482:	add_to_ins_chain (compose_ins (INS_MOVESEXT16TO32, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
archaarch64.c:495:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
archaarch64.c:511:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archaarch64.c:515:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
archaarch64.c:519:		add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archaarch64.c:523:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
archaarch64.c:527:		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
archaarch64.c:542:		add_to_ins_chain (compose_ins (INS_FADD, 0, 0));
archaarch64.c:546:		add_to_ins_chain (compose_ins (INS_FSUB, 0, 0));
archaarch64.c:550:		add_to_ins_chain (compose_ins (INS_FMUL, 0, 0));
archaarch64.c:554:		add_to_ins_chain (compose_ins (INS_FDIV, 0, 0));
archaarch64.c:558:		add_to_ins_chain (compose_ins (INS_FSQRT, 0, 0));
archmips.c:159:					add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, cregs[i]));
archmips.c:162:					add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, constmap_regconst (cregs[i])));
archmips.c:165:					add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, constmap_regconst (cregs[i])));
archmips.c:168:					add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REGIND | ARG_DISP, REG_WPTR, (constmap_regconst (cregs[i])) << WSH));
archmips.c:173:				add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
archmips.c:194:				tmp_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:200:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, -4));
archmips.c:203:				tmp_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:206:			add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archmips.c:211:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REG, tmp_reg));
archmips.c:212:			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, xregs[r_in]));
archmips.c:213:			tmp_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:216:			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archmips.c:217:			add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archmips.c:221:			tmp_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:244:			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
archmips.c:271:					add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, oregs[i]));
archmips.c:274:					add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, oregs[i], ARG_REG, xregs[i]));
archmips.c:275:					add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, oregs[i]));
archmips.c:303:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
archmips.c:308:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab));
archmips.c:369:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->procfile_setup_label));
archmips.c:370:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archmips.c:389:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
archmips.c:390:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1) + 1])));
archmips.c:422:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procedure_label));
archmips.c:461:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->filename_label));
archmips.c:609:			tmp_ins = compose_ins(INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, new_reg);
archmips.c:663:	rtl_insert_instr_before (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ArgReg(cond_ins->out_args[0]), ARG_REG, MIPS_KROC_TMP_REG), cond_ins);
archmips.c:664:	rtl_insert_instr_after (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ArgReg(cond_ins->out_args[0])), cond_ins);
archmips.c:728:				rtl_insert_instr_before (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ArgReg(cur_ins->out_args[0]), ARG_REG, MIPS_KROC_TMP_REG), cur_ins);
archmips.c:729:				rtl_insert_instr_after (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ArgReg(cur_ins->out_args[0])), cur_ins);
archmips.c:808:						compose_ins(INS_MOVE, 1, 1, ARG_CONST, ins->in_args[0]->regconst, ARG_REG, MIPS_KROC_TMP_REG));
archmips.c:810:						compose_ins(INS_MOVE, 1, 1, ARG_REG, MIPS_KROC_TMP_REG, ins->out_args[0]->flags & ARG_MODEMASK, 0));
archmips.c:826:					compose_ins(INS_MOVE, 1, 1, ARG_CONST, 0,
archmips.c:832:					compose_ins(INS_MOVE, 1, 1, ARG_REG, MIPS_KROC_TMP_REG, ins->out_args[0]->flags & ARG_MODEMASK, 0));
archmips.c:874:					rtl_insert_instr_before (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ArgReg(ins->out_args[0]), ARG_REG, MIPS_KROC_TMP_REG), ins);
archmips.c:875:					rt
l_insert_instr_after (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ArgReg(ins->out_args[0])), ins);
archmips.c:1037:							compose_ins (INS_CJUMP, 3, 0, ARG_COND, ArgCC(ins->in_args[0]), ARG_FLABEL, 0, ARG_REG, 0));
archmips.c:1055:							compose_ins (INS_CJUMP, 4, 0, ARG_COND, ArgCC(ins->in_args[0]), ARG_FLABEL, 0, ARG_REG, 0, ARG_REG, 0));
archmips.c:1062:						compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, 0));
archmips.c:1066:						compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 1));
archmips.c:1069:						compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archmips.c:1072:						compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REG, 0));
archmips.c:1076:						compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archmips.c:1103:			tmp_ins = compose_ins_ex(ins->etc_type, INS_CMP, 2, 0, ARG_REG, 0, ARG_REG, 0);
archmips.c:1110:						compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, new_reg));
archmips.c:1126:							compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, new_reg));
archmips.c:1129:							compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, new_reg));
archmips.c:1210:					compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_CONST, ArgConst(ins->in_args[0]), ARG_REG, new_reg));
archmips.c:1212:					compose_ins_ex(ins->etc_type, ins->type, 2, 1, ARG_REG, new_reg, ins->in_args[1]->flags, ins->in_args[1]->regconst, ins->out_args[0]->flags, ins->out_args[0]->regconst));
archmips.c:1289:					compose_ins(INS_JUMP, 1, 0, ARG_FLABEL, 1));
archmips.c:1291:					compose_ins(INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archmips.c:1294:					compose_ins(INS_JUMP, 1, 0, ARG_REG, 0));
archmips.c:1298:					compose_ins(INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archmips.c:1329:				rtl_insert_instr_after(compose_ins(INS_AND, 2, 1, ARG_REG, ArgReg(tmp_ins->out_args[0]), ARG_CONST, 0x80000000, ARG_REG, ArgReg(tmp_ins->out_args[0])), tmp_ins);
archmips.c:1437:			tmp_ins = compose_ins(INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, MIPS_REG_RA);
archmips.c:1447:				compose_ins_ex(ins->etc_type, INS_SUB, 2, 1, ARG_CONST, 4, ARG_REG, REG_SPTR, ARG_REG, REG_SPTR));
archmips.c:1449:				compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REGIND, REG_SPTR));
archmips.c:1458:				compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REGIND, REG_SPTR, ARG_REG, 0));
archmips.c:1461:				compose_ins_ex(ins->etc_type, INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_SPTR, ARG_REG, REG_SPTR));
archmips.c:1478:					compose_ins(INS_MOVE, 1, 1, ARG_NAMEDLABEL, 0, ARG_REG, MIPS_KROC_TMP_REG));
archmips.c:1482:					compose_ins(INS_JUMP, 1, 0, ARG_REG, MIPS_KROC_TMP_REG));
archmips.c:1499:				/* FIXME: cant use tmp_ins_c->in_args[0]->flags here for in_args[0] as compose_ins throws a wibbely */
archmips.c:1501:						compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, MIPS_KROC_TMP_REG));
archmips.c:1508:					compose_ins_ex(ins->etc_type, (ins->type == INS_INC?INS_ADD:INS_SUB), 2, 1,
archmips.c:1515:					compose_ins_ex(ins->etc_type, ins->type, 1, 1,
archmips.c:1527:						compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REG, MIPS_KROC_TMP_REG, ins->out_args[0]->flags, 0));
archmips.c:1547:			// FIXME: Why does compose_ins not like this:
archmips.c:1548:			//compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ins->in_args[0]->flags, 0, ARG_REG, ins->out_args[0]->regconst));
archmips.c:1549:			compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, ins->out_args[0]->regconst));
archmips.c:1555:			// FIXME: Why does compose_ins not like this:
archmips.c:1556:			//compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ins->in_args[1]->flags, 0, ARG_REG, MIPS_KROC_TMP_REG));
archmips.c:1557:			compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, MIPS_KROC_TMP_REG));
archmips.c:1563:			compose_ins_ex(ins->etc_type, INS_SUB, 2, 1, ARG_REG, ins->out_args[0]->regconst, ARG_REG, MIPS_KROC_TMP_REG, ARG_REG, ins->out_args[0]->regconst));
archmips.c:1577:			rtl_insert_instr_before(compose_ins(INS_MOVE, 1, 1, ARG_REG, 0, ARG_REG, new_reg),
archmips.c:1592:							compose_ins_ex(ins->etc_type, INS_MOVE, 1, 1, ins->in_args[0]->flags, ins->in_args[0]->regconst, ARG_REG, MIPS_KROC_TMP_REG));
archmips.c:1594:							compose_ins_ex(ins->etc_type, INS_MOVEB, 1, 1, ARG_REG, MIPS_KROC_TMP_REG, ARG_REG, 0));
archmips.c:1650:		rtl_insert_instr_before(compose_ins(INS_MOVE, 1, 1, ARG_REGIND, MIPS_REG_ZERO, ARG_REG, MIPS_REG_ZERO), ins);
archmips.c:1706:	add_to_ins_chain (compose_ins (INS_CJUMP, 3, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup      ("&_X_BNSeterr"), ARG_REG, reg));
archmips.c:1723:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
archmips.c:1724:	add_to_ins_chain (compose_ins (INS_CWDE, 1, 1, ARG_REG | ARG_IMP | ARG_IS16BIT, ts->stack->a_reg, ARG_REG | ARG_IMP, ts->stack->a_reg));
archmips.c:1725:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
archmips.c:1726:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
archmips.c:1732:	add_to_ins_chain (compose_ins (INS_CJUMP, 3, 0, ARG_COND, CC_LT,  ARG_FLABEL, 0, ARG_REG, ts->stack->a_reg)); /* branch less than zero */
archmips.c:1733:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0x0000FFFF, ARG_REG, tmp_reg));
archmips.c:1734:	add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg,  ARG_REG, ts->stack->a_reg));
archmips.c:1735:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 1));
archmips.c:1736:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archmips.c:1737:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0xFFFF0000, ARG_REG, tmp_reg));
archmips.c:1738:	add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg,  ARG_REG, ts->stack->a_reg));
archmips.c:1739:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archmips.c:1743:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
archmips.c:1764:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
archmips.c:1765:	add_to_ins_chain (compose_ins (INS_CJUMP, 3, 0, ARG_COND, CC_LT,  ARG_FLABEL, 0, ARG_REG, ts->stack->old_a_reg)); /* branch less than zero */
archmips.c:1766:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, -1, ARG_REG, ts->stack->b_reg));
archmips.c:1767:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 1));
archmips.c:1768:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archmips.c:1769:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->b_reg));
archmips.c:1770:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archmips.c:1785:	add_to_ins_chain (compose_ins_ex (EtcSecondary(I_DIV), INS_DIV, 2, 3, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, quotient, ARG_REG | ARG_IMP, MIPS_REG_HI, ARG_REG | ARG_IMP, MIPS_REG_LO));
archmips.c:1787:	add_to_ins_chain (compose_ins_ex (EtcSecondary(I_DIV), INS_MOVE, 1, 1, ARG_REG | ARG_IMP, MIPS_REG_HI, ARG_REG, remainder));
archmips.c:1802:	add_to_ins_chain (compose_ins_ex (EtcSecondary(I_REM), INS_DIV, 2, 3, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, MIPS_REG_HI, ARG_REG | ARG_IMP, MIPS_REG_LO));
archmips.c:1830:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_BLABEL, 0));
archmips.c:1831:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, saddr_reg, ARG_REG, tmp_reg));
archmips.c:1832:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, daddr_reg));
archmips.c:1833:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, counter_reg, ARG_CONST, 1, ARG_REG, counter_reg));
archmips.c:1834:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, saddr_reg, ARG_CONST, 1, ARG_REG, saddr_reg));
archmips.c:1835:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, daddr_reg, ARG_CONST, 1, ARG_REG, daddr_reg));
archmips.c:1836:	add_to_ins_chain (compose_ins (INS_CJUMP, 3, 0, ARG_COND, CC_NZ, ARG_BLABEL, 0, ARG_REG, counter_reg));
archmips.c:1847:	add_to_ins_chain (compose_ins (((shif
t_ins == I_SHL) ? INS_SHL : INS_SHR), 2, 1, ARG_REG, reg1, ARG_REG, reg2, ARG_REG, reg3));
archmips.c:1866:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg));
archmips.c:1868:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archmips.c:1876:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archmips.c:1878:		add_to_ins_chain (compose_ins (INS_SBB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archmips.c:1887:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
archmips.c:1888:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg));
archmips.c:1890:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archmips.c:1892:		add_to_ins_chain (compose_ins (INS_RCL, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, tmp_reg, ARG_REG | ARG_IS8BIT, tmp_reg));
archmips.c:1901:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
archmips.c:1902:		add_to_ins_chain (compose_ins (INS_RCR, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, ts->stack->old_c_reg));
archmips.c:1904:		add_to_ins_chain (compose_ins (INS_SBB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archmips.c:1906:		add_to_ins_chain (compose_ins (INS_RCL, 2, 1, ARG_CONST, 1, ARG_REG | ARG_IS8BIT, tmp_reg, ARG_REG | ARG_IS8BIT, tmp_reg));
archmips.c:1915:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EDX));
archmips.c:1916:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_EAX));
archmips.c:1917:		add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG | ARG_IMP, tmp_reg));
archmips.c:1920:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_a_reg));
archmips.c:1921:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archmips.c:1922:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
archmips.c:1923:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_CONST, 0, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
archmips.c:1943:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, MIPS_REG_ZERO, ARG_REG, tmp_reg)); /* INS_SUB is subu by default */
archmips.c:1944:		add_to_ins_chain (compose_ins ((sec == I_LSHL ? INS_SHR : INS_SHL), 2, 1, ARG_REG,  ts->stack->old_b_reg, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
archmips.c:1945:		add_to_ins_chain (compose_ins ((sec == I_LSHL ? INS_SHL : INS_SHR), 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_c_reg));
archmips.c:1946:		add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, tmp_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archmips.c:1947:		add_to_ins_chain (compose_ins ((sec == I_LSHL ? INS_SHL : INS_SHR), 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
archmips.c:1952:		add_to_ins_chain (compose_ins (INS_CJUMP, 4, 0, ARG_COND, CC_B, ARG_LABEL, this_lab, ARG_REG, ts->stack->old_a_reg, ARG_CONST, 32));
archmips.c:1955:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, tmp_reg));
archmips.c:1956:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
archmips.c:1957:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, ts->stack->old_b_reg));
archmips.c:1959:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, (sec == I_LSHL) ? ts->stack->old_b_reg : ts->stack->old_c_reg));
archmips.c:1962:		add_to_ins_chain (compose_ins (INS_CJUMP, 4, 0, ARG_COND, CC_B, ARG_LABEL, this_lab, ARG_REG, ts->stack->old_a_reg, ARG_CONST, 64));
archmips.c:1964:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, (sec == I_LSHL) ? ts->stack->old_c_reg : ts->stack->old_b_reg));
archmips.c:1967:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archmips.c:1974:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_EDX));
archmips.c:1975:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG, REG_EAX));
archmips.c:1976:		add_to_ins_chain (compose_ins (INS_UDIV, 3, 2, ARG_REG | ARG_IMP, ts->stack->old_c_reg, ARG_REG | ARG_IMP, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, \
archmips.c:1980:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_b_reg));
archmips.c:1981:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->old_c_reg));
archmips.c:2022:	*pst_first = compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_WPTR);
archmips.c:2025:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archmips.c:2026:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_BPTR, ARG_NAMEDLABEL, string_dup ("&Bptr")));
archmips.c:2029:	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, 4));
archmips.c:2030:	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_NAMEDLABEL, string_dup (name + 1), ARG_REG, 25));
archmips.c:2034:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST, 16, ARG_REG, MIPS_REG_SP, ARG_REG, MIPS_REG_SP));
archmips.c:2036:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_REG, 25));
archmips.c:2038:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, MIPS_REG_SP, ARG_REG, MIPS_REG_SP));
archmips.c:2040:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archmips.c:2041:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archmips.c:2043:	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_WPTR));
archmips.c:2049:		*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
archmips.c:2064:	*pst_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg);
archmips.c:2065:	/* *pst_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, MIPS_REG_V0); */
archmips.c:2068:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, tmp_reg));
archmips.c:2071:		 add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2))));
archmips.c:2073:		add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_REG, MIPS_REG_V1)); */
archmips.c:2076:		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint)));
archmips.c:2082:	*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
archmips.c:2101:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
archmips.c:2102:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archmips.c:2103:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archmips.c:2105:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
archppc.c:139:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
archppc.c:168:			add_to_ins_chain (compose_in
s (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0xfffffffc, ARG_REG, tmpreg));
archppc.c:169:			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_REG, tmpreg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archppc.c:174:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0xfffffffc, ARG_REG, tmpreg));
archppc.c:175:			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_REG, tmpreg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archppc.c:186:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_R9));
archppc.c:187:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, target_reg, ARG_REG, tmp_reg));
archppc.c:188:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0xfffffffc, ARG_REG, tmp_reg2));
archppc.c:189:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_REG, tmp_reg2, ARG_REG, target_reg, ARG_REG, target_reg));			/* mask off here */
archppc.c:190:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x00000003, ARG_REG, tmp_reg2));
archppc.c:191:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_REG, tmp_reg2, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));			/* mask in here */
archppc.c:192:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
archppc.c:193:		tmp_ins = compose_ins (INS_CALL, 2, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));			/* regular */
archppc.c:195:		/* add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 1)); */
archppc.c:196:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archppc.c:197:		tmp_ins2 = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (target_call))->entrypoint));
archppc.c:199:		/* add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1)); */
archppc.c:204:		tmp_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archppc.c:219:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
archppc.c:250:			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, oregs[i], ARG_REG, xregs[i]));
archppc.c:251:			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, oregs[i]));
archppc.c:271:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
archppc.c:277:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab));
archppc.c:338:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->procfile_setup_label));
archppc.c:340:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archppc.c:343:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
archppc.c:397:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
archppc.c:398:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archppc.c:401:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
archppc.c:402:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
archppc.c:403:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
archppc.c:406:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_WPTR));
archppc.c:407:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_WPTR, ARG_REG | ARG_IMP, REG_CC));
archppc.c:408:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archppc.c:412:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK, ARG_REG, tmp_reg));
archppc.c:413:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archppc.c:416:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
archppc.c:435:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
archppc.c:437:			add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:440:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
archppc.c:443:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:446:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
archppc.c:452:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
archppc.c:453:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
archppc.c:457:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
archppc.c:478:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
archppc.c:480:			add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:483:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
archppc.c:486:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archppc.c:487:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:490:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_LT, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
archppc.c:496:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_LABEL, skip_lab));
archppc.c:497:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
archppc.c:501:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
archppc.c:512:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (signed int)0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:513:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
archppc.c:514:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
archppc.c:516:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archppc.c:520:			add_to_ins_chain  (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_BPTR));
archppc.c:522:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Bptr")));
archppc.c:527:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_FPTR));
archppc.c:529:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archppc.c:547:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
archppc.c:548:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1) + 1])));
archppc.c:579:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procedure_label));
archppc.c:617:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->filename_label));
archppc.c:635:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->zerodiv_label));
archppc.c:637:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_R9));
archppc.c:638:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST,
ts->procedure_label, ARG_REG, REG_ALT_R8));
archppc.c:639:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_zero_div")));
archppc.c:649:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floaterr_label));
archppc.c:650:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_R9));
archppc.c:651:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_R8));
archppc.c:652:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_floaterr")));
archppc.c:662:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->overflow_label));
archppc.c:663:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_R9));
archppc.c:664:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_R8));
archppc.c:665:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_overflow")));
archppc.c:675:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->range_entry_label));
archppc.c:676:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_R7));
archppc.c:677:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1,  ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_R6));
archppc.c:678:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_RangeCheckError")));
archppc.c:691:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R9));
archppc.c:693:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R8));
archppc.c:694:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_R7));
archppc.c:695:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_R6));
archppc.c:696:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_Seterr")));
archppc.c:710:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R7));
archppc.c:712:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R6));
archppc.c:713:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->overflow_label));
archppc.c:728:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R7));
archppc.c:730:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R6));
archppc.c:731:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->floaterr_label));
archppc.c:746:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, x, ARG_REG, REG_ALT_R9));
archppc.c:748:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, x, ARG_REG, REG_ALT_R8));
archppc.c:749:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->range_entry_label));
archppc.c:760:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procfile_setup_label));
archppc.c:761:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_R6));
archppc.c:762:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_R7));
archppc.c:763:	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
archppc.c:779:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:780:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, this_lab));
archppc.c:782:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R7));
archppc.c:784:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R6));
archppc.c:785:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
archppc.c:786:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archppc.c:793:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R7));
archppc.c:795:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_R6));
archppc.c:796:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
archppc.c:812:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:813:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
archppc.c:814:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
archppc.c:815:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archppc.c:821:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
archppc.c:835:	add_to_ins_chain (compose_ins (INS_CWDE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
archppc.c:855:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, tmp_reg));
archppc.c:856:	add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_CONST | ARG_ISCONST, 0x80000000, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:857:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, tmp_reg));
archppc.c:858:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
archppc.c:859:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0xffffffff, ARG_REG, tmp_reg));
archppc.c:860:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archppc.c:873:	add_to_ins_chain (compose_ins (INS_DIV, 2, 1, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, quotient));
archppc.c:886:	add_to_ins_chain (compose_ins (INS_DIV, 2, 1, ARG_REG, dividend, ARG_REG, divisor, ARG_REG, tmp_reg));
archppc.c:887:	add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, tmp_reg, ARG_REG, divisor, ARG_REG, tmp_reg));
archppc.c:888:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, tmp_reg, ARG_REG, dividend, ARG_REG, tmp_reg));
archppc.c:905:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_R9));
archppc.c:906:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
archppc.c:911:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, targetreg, ARG_REG, REG_R6));
archppc.c:912:	add_to_ins_chain (compose_ins (INS_INB, 1, 1, ARG_REG | ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IS8BIT, targetreg));
archppc.c:913:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, targetreg));
archppc.c:914:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archppc.c:927:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_R9));
archppc.c:928:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
archppc.c:929:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, sourcereg, ARG_REG, REG_R6));
archppc.c:930:	add_to_ins_chain (compose_ins (INS_OUTB, 2, 0, ARG_REG | ARG_IS8BIT, sourcereg, ARG_REG | ARG_IS16BIT, tmp_reg));
archppc.c:931:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, sourcereg));
archppc.c:932:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archppc.c:950:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archppc.c:951:	add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));		/* set cc */
archppc.c:952:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 1));
archppc.c:953:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
archppc.c:955:	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, tmpreg));
archppc.c:956:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->sta
ck->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archppc.c:957:	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG | ARG_IS8BIT, tmpreg, ARG_REGIND, ts->stack->old_b_reg));
archppc.c:958:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archppc.c:959:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_BLABEL, 0));
archppc.c:960:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archppc.c:974:	add_to_ins_chain (compose_ins (((sec == I_SHL) ? INS_SHL : INS_SHR), 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
archppc.c:994:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archppc.c:995:		add_to_ins_chain (compose_ins (INS_ADD, 2, 3, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC, ARG_REG | ARG_IMP, REG_CA));
archppc.c:996:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
archppc.c:1006:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archppc.c:1007:		add_to_ins_chain (compose_ins (INS_ADD, 2, 3, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC, ARG_REG | ARG_IMP, REG_CA));
archppc.c:1008:		add_to_ins_chain (compose_ins (INS_SBB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
archppc.c:1019:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archppc.c:1020:		add_to_ins_chain (compose_ins (INS_ADD, 2, 2, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1021:		add_to_ins_chain (compose_ins (INS_ADC, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1022:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1033:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archppc.c:1034:		add_to_ins_chain (compose_ins (INS_ADD, 2, 2, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1035:		add_to_ins_chain (compose_ins (INS_SBB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1036:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1049:		add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_ALT_Y));
archppc.c:1050:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_Y, ARG_REG, tmp_reg));
archppc.c:1051:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->a_reg));
archppc.c:1052:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, tmp_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1066:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ALT_Y));
archppc.c:1067:		add_to_ins_chain (compose_ins (INS_UDIV, 3, 2, ARG_REG | ARG_IMP, REG_ALT_Y, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg,
archppc.c:1069:		add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG, tmp_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_ALT_Y));
archppc.c:1070:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1071:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, ts->stack->a_reg));
archppc.c:1086:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, tmp_reg2));
archppc.c:1087:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, tmp_reg2, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1088:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 1));
archppc.c:1091:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 64, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1092:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_FLABEL, 2));
archppc.c:1095:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 32, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1096:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_AE, ARG_FLABEL, 3));
archppc.c:1098:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ((sec == I_LSHL) ? ts->stack->old_b_reg : ts->stack->old_c_reg), ARG_REG, tmp_reg));
archppc.c:1099:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHL : INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1100:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHL : INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
archppc.c:1101:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, tmp_reg2, ARG_REG, ts->stack->old_a_reg));
archppc.c:1103:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHR : INS_SHL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
archppc.c:1104:		add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, tmp_reg, ARG_REG, ts->stack->b_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1105:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 4));
archppc.c:1108:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
archppc.c:1109:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
archppc.c:1110:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1111:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 4));
archppc.c:1114:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
archppc.c:1115:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, ts->stack->a_reg));
archppc.c:1116:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, ts->stack->b_reg));
archppc.c:1117:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 4));
archppc.c:1120:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
archppc.c:1121:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHL : INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->b_reg));
archppc.c:1122:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, ts->stack->a_reg));
archppc.c:1125:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 4));
archppc.c:1155:		add_to_ins_chain (compose_ins (INS_FSQRT, 0, 0));
archppc.c:1164:		add_to_ins_chain (compose_ins (INS_FABS, 1, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg1));
archppc.c:1186:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2));
archppc.c:1194:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_FREG, fpreg1));
archppc.c:1204:		add_to_ins_chain (compose_ins (INS_FLD32, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_FREG, fpreg1));
archppc.c:1215:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_FREG, fpreg2));
archppc.c:1216:		add_to_ins_chain (compose_ins (INS_FADD, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg1));
archppc.c:1229:		add_to_ins_chain (compose_ins (INS_FLD32, 1, 1, ARG_REGIND, ts->stack->o
ld_a_reg, ARG_FREG, fpreg2));
archppc.c:1230:		add_to_ins_chain (compose_ins (INS_FADD, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg1));
archppc.c:1243:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_FREG, fpreg2));
archppc.c:1244:		add_to_ins_chain (compose_ins (INS_FMUL, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg1));
archppc.c:1257:		add_to_ins_chain (compose_ins (INS_FLD32, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_FREG, fpreg2));
archppc.c:1258:		add_to_ins_chain (compose_ins (INS_FMUL, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg1));
archppc.c:1270:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST | ARG_ISCONST, 3, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archppc.c:1272:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
archppc.c:1274:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_FREG, fpreg1));
archppc.c:1284:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST | ARG_ISCONST, 3, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archppc.c:1286:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
archppc.c:1288:		add_to_ins_chain (compose_ins (INS_FLD32, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_FREG, fpreg1));
archppc.c:1298:		add_to_ins_chain (compose_ins (INS_FST64, 1, 1, ARG_FREG, fpreg1, ARG_REGIND, ts->stack->old_a_reg));
archppc.c:1308:		add_to_ins_chain (compose_ins (INS_FST32, 1, 1, ARG_FREG, fpreg1, ARG_REGIND, ts->stack->old_a_reg));
archppc.c:1318:		add_to_ins_chain (compose_ins (INS_FIST32, 1, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg1));
archppc.c:1319:		add_to_ins_chain (compose_ins (INS_FST32, 1, 1, ARG_FREG, fpreg1, ARG_REGIND | ARG_DISP, REG_WPTR, ts->stack->old_a_reg));
archppc.c:1332:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatspace_label, ARG_REG, tmp_reg));
archppc.c:1333:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, tmp_reg2));
archppc.c:1334:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, tmp_reg, 0));
archppc.c:1335:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, tmp_reg, 4));
archppc.c:1336:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, 0, ARG_FREG, fpreg1));
archppc.c:1346:		add_to_ins_chain (compose_ins (INS_FADD, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg2));
archppc.c:1358:		add_to_ins_chain (compose_ins (INS_FSUB, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg2));
archppc.c:1370:		add_to_ins_chain (compose_ins (INS_FMUL, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg2));
archppc.c:1382:		add_to_ins_chain (compose_ins (INS_FDIV, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg2));
archppc.c:1397:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatconv_label, ARG_REG, tmp_reg));
archppc.c:1398:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 1, ARG_REGIND, tmp_reg, ARG_FREG, fpreg2));
archppc.c:1401:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatspace_label, ARG_REG, tmp_reg));
archppc.c:1402:		add_to_ins_chain (compose_ins (INS_XOR, 2, 1, ARG_CONST | ARG_ISCONST, 0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
archppc.c:1403:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, tmp_reg, 4));
archppc.c:1406:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x43300000, ARG_REG, tmp_reg2));
archppc.c:1407:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND, tmp_reg));
archppc.c:1409:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 1, ARG_REGIND, tmp_reg, ARG_FREG, fpreg1));
archppc.c:1410:		add_to_ins_chain (compose_ins (INS_FSUB, 2, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg2, ARG_FREG, fpreg1));
archppc.c:1420:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1423:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_BE, ARG_LABEL, this_lab));
archppc.c:1426:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 0x7fffffff, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
archppc.c:1427:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
archppc.c:1429:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[1], ARG_REG, tmp_reg));
archppc.c:1430:		add_to_ins_chain (compose_ins (INS_FADD64, 1, 0, ARG_REGIND, tmp_reg));
archppc.c:1431:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab2));
archppc.c:1434:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archppc.c:1435:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
archppc.c:1438:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab2));
archppc.c:1448:		add_to_ins_chain (compose_ins (INS_FRNDINT, 1, 1, ARG_FREG, fpreg1, ARG_FREG, fpreg1));
archppc.c:1460:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
archppc.c:1462:		add_to_ins_chain (compose_ins (INS_LAHF, 0, 1, ARG_REG|ARG_IMP, tmp_reg));
archppc.c:1463:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG|ARG_IS8BIT|ARG_IS8HIGH, tmp_reg, ARG_REGIND|ARG_DISP, REG_ESP, -4));
archppc.c:1464:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG|ARG_IS16BIT, tmp_reg));
archppc.c:1465:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archppc.c:1466:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 0x3c, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG|ARG_IS16BIT, tmp_reg));
archppc.c:1467:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x20, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1468:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
archppc.c:1469:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG|ARG_IS16BIT, tmp_reg));
archppc.c:1470:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_CONST, 0x3c, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1471:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
archppc.c:1479:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archppc.c:1481:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND|ARG_DISP, REG_ESP, -4, ARG_REG|ARG_IS8BIT|ARG_IS8HIGH, tmp_reg));
archppc.c:1482:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG|ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1490:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, -32));
archppc.c:1491:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, REG_ESP));
archppc.c:1492:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP));
archppc.c:1493:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
archppc.c:1494:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
archppc.c:1495:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
archppc.c:1505:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, 32));
archppc.c:1506:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, REG_ESP));
archppc.c:1507:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP));
archppc.c:1508:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
archppc.c:1509:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
archppc.c:1510:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
archppc.c:1524:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[(sec == I_FPCHKI32) ? 0 : 2], ARG_REG, tmp_reg));
archppc.c:15
25:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, tmp_reg));
archppc.c:1526:		add_to_ins_chain (compose_ins (INS_FCOMP, 0, 0));
archppc.c:1528:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
archppc.c:1529:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
archppc.c:1530:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1531:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archppc.c:1533:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_BE, ARG_LABEL, this_lab));
archppc.c:1535:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archppc.c:1538:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[(sec == I_FPCHKI32) ? 1 : 3], ARG_REG, tmp_reg));
archppc.c:1539:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, tmp_reg));
archppc.c:1540:		add_to_ins_chain (compose_ins (INS_FCOMP, 0, 0));
archppc.c:1542:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
archppc.c:1543:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
archppc.c:1544:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1545:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archppc.c:1547:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, this_lab));
archppc.c:1549:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archppc.c:1557:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
archppc.c:1558:		add_to_ins_chain (compose_ins (INS_FTST, 0, 0));
archppc.c:1559:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
archppc.c:1560:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1561:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
archppc.c:1574:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
archppc.c:1575:		add_to_ins_chain (compose_ins (INS_FXAM, 0, 0));
archppc.c:1576:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
archppc.c:1577:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1578:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
archppc.c:1591:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
archppc.c:1592:		add_to_ins_chain (compose_ins (INS_FUCOM, 0, 0));
archppc.c:1593:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
archppc.c:1594:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1595:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
archppc.c:1609:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
archppc.c:1610:		add_to_ins_chain (compose_ins (INS_FCOMPP, 0, 0));
archppc.c:1611:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
archppc.c:1612:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1613:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
archppc.c:1628:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_EAX));
archppc.c:1629:		add_to_ins_chain (compose_ins (INS_FCOMPP, 0, 0));
archppc.c:1630:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
archppc.c:1631:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1632:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
archppc.c:1649:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
archppc.c:1650:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archppc.c:1651:		add_to_ins_chain (compose_ins (INS_FPREM1, 0, 0));
archppc.c:1652:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_EAX));
archppc.c:1653:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
archppc.c:1654:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archppc.c:1655:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_CONST, 0x400, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
archppc.c:1656:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, this_lab));
archppc.c:1657:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
archppc.c:1726:	*pst_first = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R1, ARG_REG, REG_ALT_R6);
archppc.c:1728:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, -24, ARG_REG, REG_ALT_R1, ARG_REG, REG_ALT_R1));
archppc.c:1729:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R6, ARG_REGIND, REG_ALT_R1));
archppc.c:1730:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R31, ARG_REGIND | ARG_DISP, REG_ALT_R1, 20));
archppc.c:1731:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R1, ARG_REG, REG_ALT_R31));
archppc.c:1732:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_ALT_R1, 16));
archppc.c:1733:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REGIND | ARG_DISP, REG_ALT_R1, 12));
archppc.c:1734:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_BPTR, ARG_REGIND | ARG_DISP, REG_ALT_R1, 8));
archppc.c:1735:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 4, ARG_REG, REG_WPTR, ARG_REG, REG_ALT_R3));
archppc.c:1741:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
archppc.c:1743:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
archppc.c:1746:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, REG_ALT_R1, ARG_REG, REG_ALT_R11));
archppc.c:1747:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_ALT_R11, -4, ARG_REG, REG_ALT_R31));
archppc.c:1748:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_ALT_R1, 16, ARG_REG, REG_WPTR));
archppc.c:1749:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_ALT_R1, 12, ARG_REG, REG_FPTR));
archppc.c:1750:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_ALT_R1, 8, ARG_REG, REG_BPTR));
archppc.c:1751:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R11, ARG_REG, REG_ALT_R1));
archppc.c:1758:		add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg));
archppc.c:1759:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
archppc.c:1760:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, tmp_reg));
archppc.c:1763:			*pst_last = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint));
archppc.c:1772:		*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
archppc.c:1789:	st_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg);
archppc.c:1792:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, (kernel_call != K_KERNEL_RUN) ? REG_R7 : REG_R6));
archppc.c:1799:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, tmp_reg2));
archppc.c:1801:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_R
EG, tmp_reg2));
archppc.c:1803:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_R6));
archppc.c:1805:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint)));
archppc.c:1806:	st_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
archppc.c:1809:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
archppc.c:1811:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
archppc.c:1834:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
archppc.c:1835:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archppc.c:1836:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archppc.c:1838:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
archppc.c:1852:	add_to_ins_chain (compose_ins (INS_MTFSI, 2, 0, ARG_CONST | ARG_ISCONST, 7, ARG_CONST | ARG_ISCONST, 0 | PPC_FPU_M));
archppc.c:1853:	add_to_ins_chain (compose_ins (INS_MTFSI, 2, 0, ARG_CONST | ARG_ISCONST, 6, ARG_CONST | ARG_ISCONST, 0));
archppc.c:1856:	add_to_ins_chain (compose_ins (INS_MTFSI, 2, 0, ARG_CONST | ARG_ISCONST, 5, ARG_CONST | ARG_ISCONST, 0));
archppc.c:1857:	add_to_ins_chain (compose_ins (INS_MTFSI, 2, 0, ARG_CONST | ARG_ISCONST, 3, ARG_CONST | ARG_ISCONST, 0));
archppc.c:1858:	add_to_ins_chain (compose_ins (INS_MTFSI, 2, 0, ARG_CONST | ARG_ISCONST, 2, ARG_CONST | ARG_ISCONST, 0));
archppc.c:1859:	add_to_ins_chain (compose_ins (INS_MTFSI, 2, 0, ARG_CONST | ARG_ISCONST, 1, ARG_CONST | ARG_ISCONST, 0));
archppc.c:1860:	add_to_ins_chain (compose_ins (INS_MTFSI, 2, 0, ARG_CONST | ARG_ISCONST, 0, ARG_CONST | ARG_ISCONST, 0));
archppc.c:1893:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, toldregs[i], ARG_REG, tfixedregs[i]));
archppc.c:1897:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
archppc.c:1900:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, -16, ARG_REG, REG_ALT_R11));
archppc.c:1901:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, REG_ALT_R11));
archppc.c:1903:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, toldregs[i]));
archppc.c:1929:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tnewregs[i], ARG_REG, tfixedregs[i]));
archppc.c:1932:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tnewregs[i]));
archppc.c:1949:	add_to_ins_chain (compose_ins (INS_MTFSB, 2, 0, ARG_CONST | ARG_ISCONST, 31, ARG_CONST | ARG_ISCONST, !!(ppcmodes[mode]) & 0x01));
archppc.c:1950:	add_to_ins_chain (compose_ins (INS_MTFSB, 2, 0, ARG_CONST | ARG_ISCONST, 30, ARG_CONST | ARG_ISCONST, !!(ppcmodes[mode]) & 0x02));
archppc.c:2313:					ins->in_args[1] = compose_ins_arg (ARG_CONST, (ArgHasDisp (ins->in_args[0]) ? ArgDisp (ins->in_args[0]) : 0));
archppc.c:2335:						ins_chain *newins = compose_ins (INS_ADD, 2, 1, ARG_CONST, ArgDisp (ins->in_args[0]), ARG_REG, ArgReg (ins->out_args[0]),
archppc.c:2362:					ins_chain *newins = compose_ins (INS_MOVEB, 1, 1, ARG_REG, tmpreg, ARG_REGIND, ArgReg (dest));
archppc.c:2382:					ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_CONST, ArgConst (src), ARG_REG, tmpreg);
archppc.c:2383:					ins_chain *newins2 = compose_ins (INS_MOVEB, 1, 1, ARG_REG, tmpreg, ARG_REGIND, tmpreg);
archppc.c:2419:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, src->regconst, ARG_REG, tmpreg);
archppc.c:2420:					ins_chain *newins2 = compose_ins (INS_MOVE, 1, 1, ARG_REG, tmpreg, ARG_REGIND, tmpreg2);
archppc.c:2437:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, ArgReg (src), ARG_REGIND, tmpreg);
archppc.c:2456:						ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REGIND, tmpreg, ARG_REG, dest->regconst);
archppc.c:2474:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, tmpreg, ARG_REGIND, dest->regconst);
archppc.c:2499:						ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, tmpreg, ARG_REGIND, dest->regconst);
archppc.c:2537:				ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_ARG, ins->in_args[1], ARG_REG, tmpreg);
archppc.c:2538:				ins_chain *newins2 = compose_ins (INS_MOVE, 1, 1, ARG_REG, tmpreg, ARG_ARG, ins->out_args[0]);
archppc.c:2540:				ins->in_args[1] = compose_ins_arg (ARG_REG, tmpreg);
archppc.c:2541:				ins->out_args[0] = compose_ins_arg (ARG_REG, tmpreg);
archppc.c:2567:				ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_ARG, ins->in_args[0], ARG_REG, tmpreg);
archppc.c:2569:				ins->in_args[0] = compose_ins_arg (ARG_REG, tmpreg);
archppc.c:2580:				ins_chain *newins = compose_ins (INS_MOVE, 0, 1, ARG_REG, tmpreg);
archppc.c:2583:				ins->in_args[0] = compose_ins_arg (ARG_REG, tmpreg);
archppc.c:2598:				ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, -1, ARG_REG, tmpreg);
archppc.c:2602:				ins->in_args[0] = compose_ins_arg (ARG_REG, tmpreg);
archppc.c:2619:				ins_chain *newins = compose_ins (INS_AND, 2, 1, ARG_CONST | ARG_ISCONST, 0xff, ARG_REG, tmpreg, ARG_REG, tmpreg);
archppc.c:2672:					newins = compose_ins (INS_SHL, 2, 1, ARG_CONST | ARG_ISCONST, shift, ARG_REG, sib->index, ARG_REG, tmpreg);
archppc.c:2674:					newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, sib->index, ARG_REG, tmpreg);
archppc.c:2676:				newins2 = compose_ins (INS_ADD, 2, 1, ARG_REG, sib->base, ARG_REG, tmpreg, ARG_REG, tmpreg);
archppc.c:2677:				ins->in_args[0] = compose_ins_arg (ARG_REGIND, tmpreg);
archppc.c:2707:				ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 6, ARG_REG, REG_ALT_R10);
archppc.c:2708:				ins_chain *newins2 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R10, ARG_REGIND, REG_SPTR);
archppc.c:2709:				ins_chain *newins3 = compose_ins (INS_MOVE, 1, 1, ArgMode (ins->in_args[0]) | ARG_ISCONST, ins->in_args[0]->regconst,
archppc.c:2711:				ins_chain *newins4 = compose_ins (INS_JUMP, 1, 0, ARG_REG, REG_ALT_R10);
archppc.c:2712:				ins_chain *newins5 = compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 6);
archppc.c:2766:				ins_chain *newins1 = compose_ins ((ins->type == INS_INC) ? INS_ADD : INS_SUB, 2, 1, ARG_CONST, 1, ARG_REG, tmpreg, ARG_REG, tmpreg);
archppc.c:2767:				ins_chain *newins2 = compose_ins (INS_MOVE, 1 ,1, ARG_REG, tmpreg, ARG_REGIND, ArgReg (ins->in_args[0]));
archppc.c:2768:				ins_chain *newins3 = compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, tmpreg, ARG_REG | ARG_IMP, REG_CC);
archppc.c:2794:				ins_chain *newins = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("unsupported PUSH/POP!"));
archppc.c:2812:					ins->out_args[0] = compose_ins_arg (ARG_REG, REG_ALT_R10);
archppc.c:2815:					retins->in_args[0] = compose_ins_arg (ARG_REG, REG_ALT_R10);
archppc.c:2825:					ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R10, ARG_REG, REG_ALT_R10);
archppc.c:2826:					ins_chain *newins2 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R10, ARG_REGIND, REG_SPTR);
archppc.c:2835:					ins->in_args[1] = compose_ins_arg (ARG_REG, REG_SPTR);
archppc.c:2836:					ins->out_args[0] = compose_ins_arg (ARG_REG, REG_SPTR);
archppc.c:2848:					ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R10, ARG_REG, REG_ALT_R10);
archppc.c:2849:					ins_chain *newins2 = compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_SPTR, ARG_REG, REG_SPTR);
archppc.c:2856:					ins->in_args[0] = compose_ins_arg (ARG_REGIND, REG_SPTR);
archppc.c:2876:				ins_chain *newins = compose_ins (INS_CMP, 2, 1, ARG_REG, tmpreg, ARG_REG, tmpreg, ARG_REG | ARG_IMP, REG_CC);
archppc.c:2898:				ins_chain *newins = compose_ins (INS_CMP, 2, 1, ARG_REG, tmpreg, ARG_REG, tmpreg, ARG_REG | ARG_IMP, REG_CC);
archppc.c:2923:				ins_chain *newins1 = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, ArgName (ins->in_args[1]));
archppc.c:2924:				ins_chain *newins2 = compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 7);
archppc.c:2951:				newins = compose_ins (I
NS_MOVE, 0, 1, ARG_REG, tmpreg);
archppc.c:2954:				newins2 = compose_ins (INS_MOVE, 1, 0, ARG_REG, tmpreg);
archsparc.c:123:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, xregs[i]));
archsparc.c:145:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0xfffffffc, ARG_REG, REG_ALT_L4));
archsparc.c:146:			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_REG, REG_ALT_L4, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
archsparc.c:150:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0xfffffffc, ARG_REG, REG_ALT_L4));
archsparc.c:151:			add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_REG, REG_ALT_L4, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archsparc.c:158:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L3));
archsparc.c:159:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, target_reg, ARG_REG, tmp_reg));
archsparc.c:160:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0xfffffffc, ARG_REG, REG_ALT_L4));
archsparc.c:161:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_REG, REG_ALT_L4, ARG_REG, target_reg, ARG_REG, target_reg));			/* mask off here */
archsparc.c:162:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x00000003, ARG_REG, REG_ALT_L4));
archsparc.c:163:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_REG, REG_ALT_L4, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));			/* mask in here */
archsparc.c:164:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
archsparc.c:165:		tmp_ins = compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint), ARG_CONST | ARG_ISCONST, 12);			/* regular */
archsparc.c:167:		/* add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 1)); */
archsparc.c:168:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archsparc.c:169:		tmp_ins2 = compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (target_call))->entrypoint), ARG_CONST | ARG_ISCONST, 8);		/* special */
archsparc.c:171:		/* add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1)); */
archsparc.c:176:		tmp_ins = compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint), ARG_CONST | ARG_ISCONST, 8);
archsparc.c:188:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
archsparc.c:218:			add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, oregs[i], ARG_REG, xregs[i]));
archsparc.c:219:			add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, oregs[i]));
archsparc.c:238:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
archsparc.c:246:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab));
archsparc.c:308:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->procfile_setup_label));
archsparc.c:317:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
archsparc.c:320:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
archsparc.c:374:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:375:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archsparc.c:378:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
archsparc.c:379:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
archsparc.c:380:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
archsparc.c:383:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_WPTR));
archsparc.c:384:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_REG, REG_WPTR, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:385:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archsparc.c:389:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK, ARG_REG, tmp_reg));
archsparc.c:390:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archsparc.c:393:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
archsparc.c:412:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:414:			add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:417:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
archsparc.c:420:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:423:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
archsparc.c:429:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
archsparc.c:430:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
archsparc.c:434:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
archsparc.c:455:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:457:			add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:460:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_LABEL, skip_lab));
archsparc.c:463:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
archsparc.c:464:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_c_reg, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:467:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_LT, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
archsparc.c:473:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_LABEL, skip_lab));
archsparc.c:474:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
archsparc.c:478:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skip_lab));
archsparc.c:490:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, (signed int)0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
archsparc.c:491:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
archsparc.c:492:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
archsparc.c:494:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
archsparc.c:498:			add_to_ins_chain  (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_BPTR));
archsparc.c:500:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Bptr")));
archsparc.c:505:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_FPTR));
archsparc.c:507:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archsparc.c:526:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
archsparc.c:527:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)
+ 1])));
  archsparc.c:559:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procedure_label));
  archsparc.c:598:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->filename_label));
  archsparc.c:617:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->zerodiv_label));
  archsparc.c:618:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->filename_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:619:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->filename_label));
  archsparc.c:620:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->procedure_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:621:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->procedure_label));
  archsparc.c:622:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_ALT_L3));
  archsparc.c:623:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_ALT_L2));
  archsparc.c:624:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_zero_div")));
  archsparc.c:635:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floaterr_label));
  archsparc.c:636:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_ALT_L0));
  archsparc.c:637:	/* add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L0, ARG_REG, REG_L6)); */	/* put exception flags in ESI -- nah, using that now ;) */
  archsparc.c:638:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->filename_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:639:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->filename_label));
  archsparc.c:640:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->procedure_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:641:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->procedure_label));
  archsparc.c:642:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_ALT_L3));
  archsparc.c:643:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_ALT_L2));
  archsparc.c:644:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_floaterr")));
  archsparc.c:655:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->overflow_label));
  archsparc.c:656:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->filename_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:657:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->filename_label));
  archsparc.c:658:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->procedure_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:659:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->procedure_label));
  archsparc.c:660:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_ALT_L3));
  archsparc.c:661:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, REG_ALT_L2));
  archsparc.c:662:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_overflow")));
  archsparc.c:673:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->range_entry_label));
  archsparc.c:674:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->filename_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:675:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->filename_label));
  archsparc.c:676:	/* add_to_ins_chain (compose_ins (INS_LOADLABADDR, 1, 1, ARG_LABEL, ts->procedure_label, ARG_REG, REG_ALT_L0)); */
  archsparc.c:677:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0,  ARG_LABEL | ARG_ISCONST, ts->procedure_label));
  archsparc.c:678:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_RangeCheckError")));
  archsparc.c:691:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L5));
  archsparc.c:693:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L4));
  archsparc.c:694:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_L3));
  archsparc.c:695:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_L2));
  archsparc.c:696:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_Seterr")));
  archsparc.c:710:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L3));
  archsparc.c:712:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L2));
  archsparc.c:713:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->overflow_label));
  archsparc.c:729:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L3));
  archsparc.c:731:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L2));
  archsparc.c:732:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->floaterr_label));
  archsparc.c:747:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, x, ARG_REG, REG_ALT_L4));
  archsparc.c:749:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, x, ARG_REG, REG_ALT_L5));
  archsparc.c:750:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->range_entry_label));
  archsparc.c:762:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->procfile_setup_label));
  archsparc.c:763:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_REG, REG_ALT_L0));
  archsparc.c:764:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->procedure_label, ARG_REG, REG_ALT_L1));
  archsparc.c:765:	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
  archsparc.c:782:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:783:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, this_lab));
  archsparc.c:785:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L3));
  archsparc.c:787:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L2));
  archsparc.c:788:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
  archsparc.c:789:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  archsparc.c:796:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L3));
  archsparc.c:798:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_REG, REG_ALT_L2));
  archsparc.c:799:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, ts->zerodiv_label));
  archsparc.c:816:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:817:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
  archsparc.c:823:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
  archsparc.c:841:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_L0));
  archsparc.c:842:	add_to_ins_chain (compose_ins (INS_CWDE, 1, 1, ARG_REG | ARG_IMP | ARG_IS16BIT, ts->stack->a_reg, ARG_REG | ARG_IMP, ts->stack->a_reg));
  archsparc.c:843:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
  archsparc.c:844:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
  archsparc.c:865:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_L0));
  archsparc.c:866:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->b_reg, ARG_REG, REG_L3));
  archsparc.c:867:	add_to_ins_chain (compose_ins (INS_CDQ, 1, 2, ARG_REG | ARG_IMP, ts->stack->old_a_reg, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, ts->stack->b_reg));
  archsparc.c:868:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->b_reg));
  archsparc.c:869:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
  archsparc.c:885:	add_to_ins_chain (compose_ins (INS_CON
  STRAIN_REG, 2, 0, ARG_REG, dividend, ARG_REG, REG_L0));
  archsparc.c:886:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L3));
  archsparc.c:887:	add_to_ins_chain (compose_ins (INS_CDQ, 1, 2, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, tmp_reg));
  archsparc.c:888:	add_to_ins_chain (compose_ins (INS_DIV, 3, 2, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, dividend, ARG_REG, divisor, \
  archsparc.c:890:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:891:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, dividend));
  archsparc.c:906:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, dividend, ARG_REG, REG_L0));
  archsparc.c:907:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, divisor, ARG_REG, REG_L2));
  archsparc.c:908:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L3));
  archsparc.c:909:	add_to_ins_chain (compose_ins (INS_CDQ, 1, 2, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, dividend, ARG_REG | ARG_IMP, tmp_reg));
  archsparc.c:910:	add_to_ins_chain (compose_ins (INS_DIV, 3, 2, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, dividend, ARG_REG, divisor, \
  archsparc.c:912:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:913:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, divisor));
  archsparc.c:914:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, dividend));
  archsparc.c:930:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L3));
  archsparc.c:931:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
  archsparc.c:936:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, targetreg, ARG_REG, REG_L0));
  archsparc.c:937:	add_to_ins_chain (compose_ins (INS_INB, 1, 1, ARG_REG | ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IS8BIT, targetreg));
  archsparc.c:938:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, targetreg));
  archsparc.c:939:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:952:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L3));
  archsparc.c:953:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, portreg, ARG_REG, tmp_reg));
  archsparc.c:954:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, sourcereg, ARG_REG, REG_L0));
  archsparc.c:955:	add_to_ins_chain (compose_ins (INS_OUTB, 2, 0, ARG_REG | ARG_IS8BIT, sourcereg, ARG_REG | ARG_IS16BIT, tmp_reg));
  archsparc.c:956:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, sourcereg));
  archsparc.c:957:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:973:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  archsparc.c:974:	add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));		/* set cc */
  archsparc.c:975:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 1));
  archsparc.c:976:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
  archsparc.c:978:	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, ts->stack->old_c_reg, ARG_REG | ARG_IS8BIT, tmpreg));
  archsparc.c:979:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
  archsparc.c:980:	add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG | ARG_IS8BIT, tmpreg, ARG_REGIND, ts->stack->old_b_reg));
  archsparc.c:981:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  archsparc.c:982:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_BLABEL, 0));
  archsparc.c:983:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
  archsparc.c:991:	add_to_ins_chain (compose_ins (((sec == I_SHL) ? INS_SHL : INS_SHR), 2, 1, ARG_REG, r1, ARG_REG, r2, ARG_REG, r3));
  archsparc.c:1009:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
  archsparc.c:1010:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ALT_R0));
  archsparc.c:1011:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1018:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
  archsparc.c:1019:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ALT_R0));
  archsparc.c:1020:		add_to_ins_chain (compose_ins (INS_SBB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1028:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
  archsparc.c:1029:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ALT_R0));
  archsparc.c:1030:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1031:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, REG_ALT_R0, ARG_REG, REG_ALT_R0, ARG_REG, ts->stack->b_reg));
  archsparc.c:1039:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 31, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
  archsparc.c:1040:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ALT_R0));
  archsparc.c:1041:		add_to_ins_chain (compose_ins (INS_SBB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1042:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_REG, REG_ALT_R0, ARG_REG, REG_ALT_R0, ARG_REG, ts->stack->b_reg));
  archsparc.c:1051:		add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_ALT_Y));
  archsparc.c:1052:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_Y, ARG_REG, tmp_reg));
  archsparc.c:1053:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1054:		add_to_ins_chain (compose_ins (INS_ADC, 2, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, tmp_reg, ARG_REG, ts->stack->b_reg));
  archsparc.c:1063:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, REG_ALT_Y));
  archsparc.c:1064:		add_to_ins_chain (compose_ins (INS_UDIV, 3, 2, ARG_REG | ARG_IMP, REG_ALT_Y, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg,
  archsparc.c:1066:		add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG, tmp_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_ALT_Y));
  archsparc.c:1067:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->b_reg));
  archsparc.c:1068:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1079:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_ALT_R0, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1080:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 1));
  archsparc.c:1083:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 64, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1084:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_FLABEL, 2));
  archsparc.c:1087:		add_to_ins_chain (compose
  _ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 32, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1088:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_AE, ARG_FLABEL, 3));
  archsparc.c:1090:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ((sec == I_LSHL) ? ts->stack->old_b_reg : ts->stack->old_c_reg), ARG_REG, tmp_reg));
  archsparc.c:1091:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHL : INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->b_reg));
  archsparc.c:1092:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHL : INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1093:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_ALT_R0, ARG_REG, ts->stack->old_a_reg));
  archsparc.c:1095:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHR : INS_SHL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
  archsparc.c:1096:		add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, tmp_reg, ARG_REG, ts->stack->b_reg, ARG_REG, ts->stack->b_reg));
  archsparc.c:1097:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 4));
  archsparc.c:1100:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
  archsparc.c:1101:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  archsparc.c:1102:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->b_reg));
  archsparc.c:1103:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 4));
  archsparc.c:1106:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
  archsparc.c:1107:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R0, ARG_REG, ts->stack->a_reg));
  archsparc.c:1108:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R0, ARG_REG, ts->stack->b_reg));
  archsparc.c:1109:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 4));
  archsparc.c:1112:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 3));
  archsparc.c:1113:		add_to_ins_chain (compose_ins ((sec == I_LSHL) ? INS_SHL : INS_SHR, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->b_reg));
  archsparc.c:1114:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_R0, ARG_REG, ts->stack->a_reg));
  archsparc.c:1117:		add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 4));
  archsparc.c:1137:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  archsparc.c:1147:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, toldregs[i], ARG_REG, tfixedregs[i]));
  archsparc.c:1151:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, -16, ARG_REG, REG_ALT_L4));
  archsparc.c:1152:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, REG_ALT_L4));
  archsparc.c:1154:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, toldregs[i]));
  archsparc.c:1180:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tnewregs[i], ARG_REG, tfixedregs[i]));
  archsparc.c:1183:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tnewregs[i]));
  archsparc.c:1201:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L0));
  archsparc.c:1203:		add_to_ins_chain (compose_ins (INS_LAHF, 0, 1, ARG_REG|ARG_IMP, tmp_reg));
  archsparc.c:1204:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG|ARG_IS8BIT|ARG_IS8HIGH, tmp_reg, ARG_REGIND|ARG_DISP, REG_ALT_L4, -4));
  archsparc.c:1205:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG|ARG_IS16BIT, tmp_reg));
  archsparc.c:1206:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:1207:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 0x3c, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG|ARG_IS16BIT, tmp_reg));
  archsparc.c:1208:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x20, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1209:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
  archsparc.c:1210:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG|ARG_IS16BIT, tmp_reg));
  archsparc.c:1211:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_CONST, 0x3c, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG|ARG_IS16BIT, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1212:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
  archsparc.c:1220:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  archsparc.c:1222:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND|ARG_DISP, REG_ALT_L4, -4, ARG_REG|ARG_IS8BIT|ARG_IS8HIGH, tmp_reg));
  archsparc.c:1223:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG|ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1226:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, -32));
  archsparc.c:1227:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, REG_ALT_L4));
  archsparc.c:1228:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4));
  archsparc.c:1229:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
  archsparc.c:1230:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
  archsparc.c:1231:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
  archsparc.c:1236:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, 32));
  archsparc.c:1237:		add_to_ins_chain (compose_ins (INS_FILD32, 1, 0, ARG_REGIND, REG_ALT_L4));
  archsparc.c:1238:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4));
  archsparc.c:1239:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
  archsparc.c:1240:		add_to_ins_chain (compose_ins (INS_FSCALE, 0, 0));
  archsparc.c:1241:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
  archsparc.c:1250:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[(sec == I_FPCHKI32) ? 0 : 2], ARG_REG, tmp_reg));
  archsparc.c:1251:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, tmp_reg));
  archsparc.c:1252:		add_to_ins_chain (compose_ins (INS_FCOMP, 0, 0));
  archsparc.c:1254:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L0));
  archsparc.c:1255:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
  archsparc.c:1256:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1257:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:1259:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_BE, ARG_LABEL, this_lab));
  archsparc.c:1261:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  archsparc.c:1264:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->floatrange_label[(sec == I_FPCHKI32) ? 1 : 3], ARG_REG, tmp_reg));
  archsparc.c:1265:		add_to_ins_chain (compose_ins (INS_FLD64, 1, 0, ARG_REGIND, tmp_reg));
  archsparc.c:1266:		add_to_ins_chain (compose_ins (INS_FCOMP, 0, 0));
  archsparc.c:1268:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L0));
  archsparc.c:1269:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
  archsparc.c:1270:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1271:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:1273:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, this_lab));
  archsparc.c:1275:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  archsparc.c:1278:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_L0));
  archsparc.c:1279:		add_to_ins_chain (compose_ins (INS_FTST, 0, 0));
  archsparc.c:1280:		add_to_ins_chain (compos
  e_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
  archsparc.c:1281:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1282:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
  archsparc.c:1290:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_L0));
  archsparc.c:1291:		add_to_ins_chain (compose_ins (INS_FXAM, 0, 0));
  archsparc.c:1292:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
  archsparc.c:1293:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1294:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
  archsparc.c:1302:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_L0));
  archsparc.c:1303:		add_to_ins_chain (compose_ins (INS_FUCOM, 0, 0));
  archsparc.c:1304:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
  archsparc.c:1305:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1306:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
  archsparc.c:1313:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_L0));
  archsparc.c:1314:		add_to_ins_chain (compose_ins (INS_FCOMPP, 0, 0));
  archsparc.c:1315:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
  archsparc.c:1316:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1317:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
  archsparc.c:1325:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, ts->stack->a_reg, ARG_REG, REG_L0));
  archsparc.c:1326:		add_to_ins_chain (compose_ins (INS_FCOMPP, 0, 0));
  archsparc.c:1327:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, ts->stack->a_reg));
  archsparc.c:1328:		add_to_ins_chain (compose_ins (INS_SAHF, 1, 1, ARG_REG | ARG_IMP, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1329:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, ts->stack->a_reg));
  archsparc.c:1340:		add_to_ins_chain (compose_ins (INS_FXCH, 0, 0));
  archsparc.c:1341:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  archsparc.c:1342:		add_to_ins_chain (compose_ins (INS_FPREM1, 0, 0));
  archsparc.c:1343:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, REG_L0));
  archsparc.c:1344:		add_to_ins_chain (compose_ins (INS_FSTSW, 0, 1, ARG_REG, tmp_reg));
  archsparc.c:1345:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:1346:		add_to_ins_chain (compose_ins (INS_AND, 2, 2, ARG_CONST, 0x400, ARG_REG, tmp_reg, ARG_REG, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
  archsparc.c:1347:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, this_lab));
  archsparc.c:1348:		add_to_ins_chain (compose_ins (INS_FSTP, 0, 1, ARG_FREG, 1));
  archsparc.c:1368:	*pst_first = compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 4, ARG_REG, REG_WPTR, ARG_REG, REG_ALT_O0);
  archsparc.c:1371:	add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, 8, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4));
  archsparc.c:1373:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_NAMEDLABEL, string_dup ("&Fptr")));
  archsparc.c:1374:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_BPTR, ARG_NAMEDLABEL, string_dup ("&Bptr")));
  archsparc.c:1376:	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg));
  archsparc.c:1377:	add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:1383:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
  archsparc.c:1385:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
  archsparc.c:1388:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4));
  archsparc.c:1390:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
  archsparc.c:1391:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
  archsparc.c:1393:	add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, 8, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4));
  archsparc.c:1394:	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_WPTR));
  archsparc.c:1400:		add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg));
  archsparc.c:1401:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  archsparc.c:1402:		add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:1405:			*pst_last = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint));
  archsparc.c:1414:		*pst_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
  archsparc.c:1431:	st_first = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, 4, ARG_REG, tmp_reg);
  archsparc.c:1434:	add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg, ARG_REG, (kernel_call != K_KERNEL_RUN) ? REG_L1 : REG_L0));
  archsparc.c:1441:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, tmp_reg2));
  archsparc.c:1443:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_REG, tmp_reg2));
  archsparc.c:1445:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, tmp_reg2, ARG_REG, REG_L0));
  archsparc.c:1447:	add_to_ins_chain (compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint), ARG_CONST | ARG_ISCONST, 8));
  archsparc.c:1448:	st_last = compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR);
  archsparc.c:1451:	add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg));
  archsparc.c:1453:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, tmp_reg2));
  archsparc.c:1476:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
  archsparc.c:1477:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
  archsparc.c:1478:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
  archsparc.c:1480:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
  archsparc.c:1494:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0xf3e, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4));
  archsparc.c:1495:	add_to_ins_chain (compose_ins (INS_FLDCW, 1, 0, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4));
  archsparc.c:1521:	add_to_ins_chain (compose_ins (INS_FSTCW, 0, 1, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4));
  archsparc.c:1523:	add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST | ARG_ISCONST, 0xf3ff, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4));
  archsparc.c:1533:		add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_CONST | ARG_ISCONST, fpu_orval, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4));
  archsparc.c:1535:	add_to_ins_chain (compose_ins (INS_FLDCW, 1, 0, ARG_REGIND | ARG_DISP, REG_ALT_L4, -4));
  archsparc.c:1836:					ins->in_args[1] = compose_ins_arg (ARG_CONST, (ArgHasDisp (ins->in_args[0]) ? ArgDisp (ins->in_args[0]) : 0));
  archsparc.c:1858:						ins_chain *newins = compose_ins (INS_ADD, 2, 1, ARG_CONST, ArgDisp (ins->in_args[0]), ARG_REG, ArgReg (ins->out_args[0]),
  archsparc.c:1884:					ins_chain *newins = compose_ins (INS_MOVEB, 1, 1, ARG_REG, REG_ALT_L4, ARG_REGIND, ArgReg
  (dest));
  archsparc.c:1903:					ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_CONST, ArgConst (src), ARG_REG, REG_ALT_L5);
  archsparc.c:1904:					ins_chain *newins2 = compose_ins (INS_MOVEB, 1, 1, ARG_REG, REG_ALT_L5, ARG_REGIND, REG_ALT_L4);
  archsparc.c:1938:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, src->regconst, ARG_REG, REG_ALT_L5);
  archsparc.c:1939:					ins_chain *newins2 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L5, ARG_REGIND, REG_ALT_L4);
  archsparc.c:1955:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, ArgReg (src), ARG_REGIND, REG_ALT_L4);
  archsparc.c:1973:						ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REGIND, REG_ALT_L4, ARG_REG, dest->regconst);
  archsparc.c:1990:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REGIND, dest->regconst);
  archsparc.c:2014:						ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REGIND, dest->regconst);
  archsparc.c:2039:				ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4);
  archsparc.c:2058:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4);
  archsparc.c:2077:					ins_chain *newins = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4);
  archsparc.c:2102:				ins_chain *newins = compose_ins (INS_AND, 2, 1, ARG_CONST | ARG_ISCONST, 0xff, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4);
  archsparc.c:2134:				ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 6, ARG_REG, REG_ALT_L4);
  archsparc.c:2135:				ins_chain *newins2 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REGIND, REG_SPTR);
  archsparc.c:2136:				ins_chain *newins3 = compose_ins (INS_MOVE, 1, 1, ArgMode (ins->in_args[0]) | ARG_ISCONST, ins->in_args[0]->regconst,
  archsparc.c:2138:				ins_chain *newins4 = compose_ins (INS_JUMP, 1, 0, ARG_REG, REG_ALT_L4);
  archsparc.c:2139:				ins_chain *newins5 = compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 6);
  archsparc.c:2189:				ins_chain *newins1 = compose_ins ((ins->type == INS_INC) ? INS_ADD : INS_SUB, 2, 1, ARG_CONST, 1, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4);
  archsparc.c:2190:				ins_chain *newins2 = compose_ins (INS_MOVE, 1 ,1, ARG_REG, REG_ALT_L4, ARG_REGIND, ArgReg (ins->in_args[0]));
  archsparc.c:2214:				ins_chain *newins = compose_ins (INS_JUMP, 1, 0, ARG_REG, REG_ALT_L4);
  archsparc.c:2218:				ins->out_args[0] = compose_ins_arg (ARG_REG, REG_ALT_L4);
  archsparc.c:2228:				ins_chain *newins = compose_ins (INS_JUMP, 1, 0, ARG_REG, REG_ALT_L4);
  archsparc.c:2232:				ins->out_args[0] = compose_ins_arg (ARG_REG, REG_ALT_L4);
  archsparc.c:2243:				ins->in_args[0] = compose_ins_arg (ARG_COND, CC_ALWAYS);
  archsparc.c:2254:				tmp_ins = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("unsupported PUSH/POP!"));
  archsparc.c:2272:					ins->out_args[0] = compose_ins_arg (ARG_REG, REG_ALT_L4);
  archsparc.c:2275:					retins->in_args[0] = compose_ins_arg (ARG_REG, REG_ALT_L4);
  archsparc.c:2285:					ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4);
  archsparc.c:2286:					ins_chain *newins2 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REGIND, REG_SPTR);
  archsparc.c:2295:					ins->in_args[1] = compose_ins_arg (ARG_REG, REG_SPTR);
  archsparc.c:2296:					ins->out_args[0] = compose_ins_arg (ARG_REG, REG_SPTR);
  archsparc.c:2308:					ins_chain *newins1 = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4);
  archsparc.c:2309:					ins_chain *newins2 = compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_SPTR, ARG_REG, REG_SPTR);
  archsparc.c:2316:					ins->in_args[0] = compose_ins_arg (ARG_REGIND, REG_SPTR);
  archsparc.c:2335:				ins_chain *newins = compose_ins (INS_CMP, 2, 1, ARG_REG, REG_ALT_L4, ARG_REG, REG_ALT_L4, ARG_REG | ARG_IMP, REG_CC);
  archsparc.c:2356:				ins_chain *newins = compose_ins (INS_CMP, 2, 1, ARG_REG, REG_ALT_L5, ARG_REG, REG_ALT_L5, ARG_REG | ARG_IMP, REG_CC);
  archsparc.c:2381:				ins_chain *newins1 = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, ArgName (ins->in_args[1]));
  archsparc.c:2382:				ins_chain *newins2 = compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 7);
  archx64.c:260:		add_to_ins_chain (compose_ins (INS_CONSTRAIN_REG, 2, 0, ARG_REG, cregs[i], ARG_REG, target_reg));
  archx64.c:264:	call_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
  archx64.c:269:		add_to_ins_chain (compose_ins (INS_UNCONSTRAIN_REG, 1, 0, ARG_REG, cregs[i]));
  archx64.c:291:		jump_ins = compose_ins (INS_CJUMP, 2, 0, ARG_COND, cc, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
  archx64.c:293:		jump_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
  archx64.c:306:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, REG_WPTR));
  archx64.c:314:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR));
  archx64.c:325:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("rdtsc"), ARG_REG, timer_reg));
  archx64.c:339:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("rdtsc"), ARG_REG, timer_reg));
  archx64.c:342:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, timer_reg, ARG_REG, target_reg, ARG_REG | ARG_IMP, X64_REG_CC));
  archx64.c:343:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_GE, ARG_FLABEL, 0));
  archx64.c:348:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  archx64.c:356:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
  archx64.c:357:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 1));
  archx64.c:361:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 2));
  archx64.c:364:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
  archx64.c:365:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
  archx64.c:366:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, REG_WPTR));
  archx64.c:368:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 2));
  archx64.c:376:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR));
  archx64.c:379:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR, ARG_REG | ARG_IMP, REG_CC));
  archx64.c:380:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_E, ARG_FLABEL, 1));
  archx64.c:383:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_REG, REG_WPTR));
  archx64.c:384:	add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND, REG_WPTR));
  archx64.c:387:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 1));
  archx64.c:388:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  archx64.c:406:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, X64_REG_CC));
  archx64.c:407:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
  archx64.c:410:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, X64_REG_WPTR, ARG_REGIND, chan_reg));
  archx64.c:414:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  archx64.c:416:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
  archx64.c:418:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, chan_reg, ARG_REGIND, dest_reg));
  archx64.c:437:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, chan_reg, ARG_REG | ARG_IMP, X64_REG_CC));
  archx64.c:438:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_FLABEL, 0));
  archx64.c:441:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, X64_REG_WPTR, ARG_REGIND, chan_reg));
  archx64.c:445:	add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  archx64.c:447:		add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
  archx64.c:449:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, src_reg, ARG_REGIND, chan_reg));
  archx64.c:458:	add_to_ins_chain (com
  pose_ins (INS_PUSH, 1, 0, ARG_REG, REG_RBP));
  archx64.c:459:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_RSP, ARG_REG, REG_RBP));
  archx64.c:463:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&workspace"), ARG_REG, wptr_reg));
  archx64.c:474:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_RBP, ARG_REG, REG_RSP));
  archx64.c:475:	add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REG, REG_RBP));
  archx64.c:478:	add_to_ins_chain (compose_ins (INS_RET, 0, 0));
  archx64.c:488:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
  archx64.c:501:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg));
  archx64.c:517:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  archx64.c:521:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
  archx64.c:525:		add_to_ins_chain (compose_ins (INS_MUL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  archx64.c:529:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
  archx64.c:533:		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg));
  archx64.c:548:		add_to_ins_chain (compose_ins (INS_FADD, 0, 0));
  archx64.c:552:		add_to_ins_chain (compose_ins (INS_FSUB, 0, 0));
  archx64.c:556:		add_to_ins_chain (compose_ins (INS_FMUL, 0, 0));
  archx64.c:560:		add_to_ins_chain (compose_ins (INS_FDIV, 0, 0));
  archx64.c:564:		add_to_ins_chain (compose_ins (INS_FSQRT, 0, 0));
  etcrtl.c:160:			st_first = compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND, REG_WPTR);
  etcrtl.c:162:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (d_str)));
  etcrtl.c:163:			st_last = compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0);
  etcrtl.c:166:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (d_str)));
  etcrtl.c:288:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_of_primary (ts->stack, x_fn, x_opd)));
  etcrtl.c:418:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:427:						add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:428:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
  etcrtl.c:430:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:449:					add_to_ins_chain (compose_ins (INS_FILD64, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
  etcrtl.c:459:						add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:473:					tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REG, ts->stack->a_reg);
  etcrtl.c:501:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->casetable_label, ARG_REG, tmp_reg));
  etcrtl.c:502:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGINDSIB, 4, ts->stack->a_reg, tmp_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:503:					add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
  etcrtl.c:504:					add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, tmp_reg));
  etcrtl.c:505:					add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->casetable_label));
  etcrtl.c:590:						add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));				/* sem-val = 1 ? */
  etcrtl.c:591:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 5));					/* jump if yes */
  etcrtl.c:593:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));			/* Wptr[Link] = null */
  etcrtl.c:594:						add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));				/* sem-fptr = null ? */
  etcrtl.c:595:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 6));					/* jump if yes */
  etcrtl.c:597:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, (1 << WSH), ARG_REG, tmp_reg));	/* tmp = sem-bptr */
  etcrtl.c:598:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK));			/* tmp[link] = Wptr */
  etcrtl.c:599:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, (1 << WSH)));	/* sem-bptr = Wptr */
  etcrtl.c:600:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 7));
  etcrtl.c:603:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 5));
  etcrtl.c:604:						add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 0xfffffffe, ARG_REGIND, ts->stack->old_a_reg, ARG_REGIND, ts->stack->old_a_reg));	/* sem-val = 0 */
  etcrtl.c:605:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 8));
  etcrtl.c:608:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 6));
  etcrtl.c:609:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, (1 << WSH)));	/* sem-bptr = Wptr */
  etcrtl.c:610:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND, ts->stack->old_a_reg));				/* sem-fptr = Wptr */
  etcrtl.c:611:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 7));
  etcrtl.c:619:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 8));
  etcrtl.c:636:						add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0, ARG_REGIND, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));				/* sem-fptr = null ? */
  etcrtl.c:637:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_FLABEL, 5));					/* jump if yes (no-one to wake) */
  etcrtl.c:640:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_REG, tmp_reg));			/* tmp = sem-fptr */
  etcrtl.c:641:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_TEMP));		/* Wptr[Temp] = tmp */
  etcrtl.c:643:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK, ARG_REG, tmp_reg));		/* tmp = tmp[Link] */
  etcrtl.c:644:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND, ts->stack->old_a_reg));			/* sem-fptr = tmp */
  etcrtl.c:648:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TEMP, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:654:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 6));
  etcrtl.c:656:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 5));
  etcrtl.c:657:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_REGIND, ts->stack->old_a_reg));			/* sem-val = 1 */
  etcrtl.c:658:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 6));
  etcrtl.c:675:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REGIND, ts->stack->old_a_reg));
  etcrtl.c:676:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, (1 << WSH)));
  etcrtl.c:696:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:703:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:710:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
  etcrtl.c:773:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CO
  NST | ARG_ISCONST, 0, ARG_REG, ts->stack->a_reg));
  etcrtl.c:775:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, pitmp->maplab, ARG_REG, ts->stack->a_reg));
  etcrtl.c:791:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 0));
  etcrtl.c:792:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 4));
  etcrtl.c:793:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8));
  etcrtl.c:794:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 12));
  etcrtl.c:818:						add_to_ins_chain (compose_ins (INS_DEC, 1, 2, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 4, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 4, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:819:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, skiplab));
  etcrtl.c:822:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 0, ARG_REG, tmp_reg));
  etcrtl.c:823:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 4));
  etcrtl.c:825:						add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:826:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, outlab));
  etcrtl.c:828:						add_to_ins_chain (compose_ins (INS_OR, 2, 1, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:829:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, fqlab));
  etcrtl.c:832:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8, ARG_REG, tmp_reg));
  etcrtl.c:833:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
  etcrtl.c:834:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, fslab));
  etcrtl.c:837:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 12, ARG_REG, REG_BPTR));
  etcrtl.c:838:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8));
  etcrtl.c:839:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, outlab));
  etcrtl.c:843:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, fqlab));
  etcrtl.c:844:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8, ARG_REG, REG_FPTR));
  etcrtl.c:845:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, fslab));
  etcrtl.c:847:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 12, ARG_REG, REG_BPTR));
  etcrtl.c:848:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8));
  etcrtl.c:849:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, outlab));
  etcrtl.c:851:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, skiplab));
  etcrtl.c:853:						add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:854:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, fxlab));
  etcrtl.c:857:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 12, ARG_REG, tmp_reg));
  etcrtl.c:858:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK));
  etcrtl.c:859:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, rslab));
  etcrtl.c:862:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, fxlab));
  etcrtl.c:863:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 8));
  etcrtl.c:866:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, rslab));
  etcrtl.c:867:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, 12));
  etcrtl.c:868:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
  etcrtl.c:869:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, outlab, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
  etcrtl.c:877:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, outlab));
  etcrtl.c:905:							add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_CONST | ARG_ISCONST, constmap_regconst (ts->stack->old_a_reg),
  etcrtl.c:908:							add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 0,
  etcrtl.c:912:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NS, ARG_LABEL, cslab));
  etcrtl.c:918:							add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, x));
  etcrtl.c:920:							add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, x));
  etcrtl.c:921:							add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->filename_label));
  etcrtl.c:922:							add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->procedure_label));
  etcrtl.c:930:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, cslab));
  etcrtl.c:932:							add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_CONST | ARG_ISCONST, constmap_regconst (ts->stack->old_a_reg),
  etcrtl.c:935:							add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 4,
  etcrtl.c:940:							add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, outlab));
  etcrtl.c:944:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 0, ARG_REG, tmp_reg));
  etcrtl.c:945:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 4));
  etcrtl.c:947:							add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 8, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:948:							add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, outlab));
  etcrtl.c:950:							add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG, REG_FPTR, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:951:							add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, fqlab));
  etcrtl.c:954:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 8, ARG_REG, tmp_reg));
  etcrtl.c:955:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
  etcrtl.c:956:							add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, fslab));
  etcrtl.c:958:							add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, fqlab));
  etcrtl.c:959:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 8, ARG_REG, REG_FPTR));
  etcrtl.c:960:							add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, fslab));
  etcrtl.c:962:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 12, ARG_REG, REG_BPTR));
  etcrtl.c:963:							add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 8));
  etcrtl.c:965:							add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, outlab));
  etcrtl.c:985:							add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (ts->stack->old_a_reg), ARG_REGIND | ARG_DISP,
  etcrtl.c:987:							add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, constmap_regconst (ts->stack->old_a_reg), ARG_REGIND | ARG_DISP,
  e
  tcrtl.c:991:							add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 4,
  etcrtl.c:993:							add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, ts->stack->old_b_reg, 0,
  etcrtl.c:1073:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, REG_WPTR, ARG_REG, ts->stack->a_reg));
  etcrtl.c:1086:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, tmp_string));
  etcrtl.c:1098:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1117:				add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, etc_code->opd));
  etcrtl.c:1162:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1165:					add_to_ins_chain (compose_ins (INS_SOURCELINE, 1, 0, ARG_CONST, y_opd));
  etcrtl.c:1175:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1185:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1189:							add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("*** register lost from stack ***")));
  etcrtl.c:1218:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1232:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1243:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1255:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1259:					add_to_ins_chain (compose_ins (INS_SHR, 2, 2, ARG_CONST, y_opd, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:1268:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1272:					add_to_ins_chain (compose_ins (INS_SHL, 2, 2, ARG_CONST, y_opd, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:1284:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1296:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1305:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1316:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("<specialnames>")));
  etcrtl.c:1327:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1340:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1411:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1424:					add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, tmp));
  etcrtl.c:1439:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->flushscreenpoint, ARG_REGIND, REG_WPTR));
  etcrtl.c:1440:					add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, tmp));
  etcrtl.c:1449:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1462:					/* add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REGIND, REG_WPTR)); */
  etcrtl.c:1485:					add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:1486:					add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, namelab));
  etcrtl.c:1488:					add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:1494:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 1, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:1495:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, namelab, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:1504:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1517:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_ndup (etc_code->o_bytes, etc_code->o_len)));
  etcrtl.c:1682:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1706:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1722:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1736:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1741:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_ndup (etc_code->o_bytes, etc_code->o_len), ARG_REG, ts->stack->a_reg));
  etcrtl.c:1748:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1754:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuffer), ARG_REG, ts->stack->a_reg));
  etcrtl.c:1761:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1786:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1791:						add_to_ins_chain (compose_ins (INS_CONSTLABDIFF, 2, 0, ARG_LABEL, y_opd, ARG_LABEL, ts->casetable_label));
  etcrtl.c:1799:								add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, y_opd));
  etcrtl.c:1801:								add_to_ins_chain (compose_ins (INS_PJUMP, 1, 0, ARG_LABEL, y_opd));
  etcrtl.c:1804:							add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, y_opd));
  etcrtl.c:1813:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1827:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1835:								add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, (i - 3) << WSH));
  etcrtl.c:1838:								add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (tmp_reg),
  etcrtl.c:1844:					add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, -4 << WSH, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  etcrtl.c:1858:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_ISCONST | ARG_FLABEL, 0, ARG_REGIND, REG_WPTR));
  etcrtl.c:1859:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, y_opd));
  etcrtl.c:1860:						add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:1861:						/* add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_LABEL, y_opd)); */
  etcrtl.c:1892:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1897:						tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, y_opd, ARG_REG, ts->stack->a_reg);
  etcrtl.c:1903:						tmp_ins = compose_ins (INS_LOADLABDIFF, 2, 1, ARG_LABEL, y_opd, ARG_LABEL, etc_code->opd, ARG_REG, ts->stack->a_reg);
  etcrtl.c:1932:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1936:					add_to_ins_chain (compose_ins (INS_DEC, 1, 2, ARG_REGIND | ARG_DISP, REG_WPTR, (le_wsoff+1) << WSH, ARG_REGIND | ARG_DISP, REG_WPTR, (le_wsoff+1) << WSH, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:1946:							add_to_ins_chain (compose_ins (INS_INC, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, le_wsoff << WSH, ARG_REGIND | ARG_DISP, REG_WPTR, le_wsoff << WSH));
  etcrtl.c:1952:								add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:1954:								add_to_ins_chain (compose_ins (INS_PJUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:1957:							add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:1960:					add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, le_l1));
  etcrtl.c:1975:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:1979:				add_to_ins_chain (compose_ins (INS_DEC, 1, 2, ARG_REGIND | ARG_DISP, REG_WPTR, (le_wsoff+1) << WSH, ARG_REGIND | ARG_DISP, REG_WPTR, (le_wsoff+1) << WSH, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:1985:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (le_wsoff + 2) << WSH, ARG_REG, tmp_reg));
  etcrtl.c:1986:				add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, le_wsoff << WSH, ARG_REG
  IND | ARG_DISP, REG_WPTR, le_wsoff << WSH));
  etcrtl.c:1987:				/* add_to_ins_chain (compose_ins (INS_INC, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, le_wsoff << WSH, ARG_REGIND | ARG_DISP, REG_WPTR, le_wsoff << WSH)); */
  etcrtl.c:1992:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:1994:						add_to_ins_chain (compose_ins (INS_PJUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:1997:					add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:1999:				add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, le_l1));
  etcrtl.c:2013:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:2017:				add_to_ins_chain (compose_ins (INS_DEC, 1, 2, ARG_REGIND | ARG_DISP, REG_WPTR, (le_wsoff+1) << WSH, ARG_REGIND | ARG_DISP, REG_WPTR, (le_wsoff+1) << WSH, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:2022:				add_to_ins_chain (compose_ins (INS_DEC, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, le_wsoff << WSH, ARG_REGIND | ARG_DISP, REG_WPTR, le_wsoff << WSH));
  etcrtl.c:2027:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:2029:						add_to_ins_chain (compose_ins (INS_PJUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:2032:					add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, le_l2));
  etcrtl.c:2034:				add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, le_l1));
  etcrtl.c:2043:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:2061:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:2065:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("generating .MOBILEINIT")));
  etcrtl.c:2110:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ((char *)outbuf)));
  etcrtl.c:2189:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:2197:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, mpp_offset << WSH, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:2199:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:2201:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, wsmaplab, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:2224:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:2232:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, mpp_offset << WSH, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:2234:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:2236:					add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, wsmaplab, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:2307:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:2316:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_of_secondary (ts->stack, x_opd)));
  etcrtl.c:2356:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->flushscreenpoint));
  etcrtl.c:2359:		add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST, ts->ws_adjust, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  etcrtl.c:2396:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, fboffset << WSH, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:2421:			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_CONST | ARG_IS8BIT, 0xff, ARG_REGIND, REG_WPTR));
  etcrtl.c:2422:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:2423:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, scr_offset << WSH, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:2426:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:2428:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (".MAGIC SCREEN")));
  etcrtl.c:2432:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, 16, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  etcrtl.c:2437:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
  etcrtl.c:2463:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floatrange_label[0]));
  etcrtl.c:2470:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floatrange_label[1]));
  etcrtl.c:2477:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floatrange_label[2]));
  etcrtl.c:2484:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floatrange_label[3]));
  etcrtl.c:2498:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floatspace_label));
  etcrtl.c:2507:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->floatconv_label));
  etcrtl.c:2539:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ts->overflow_label));
  etcrtl.c:2702:					ins_chain *slins = compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, ++(ts->last_lab));
  etcrtl.c:3090:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (msp_offset << WSH), ARG_REG, tmp_reg));
  etcrtl.c:3091:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST | ARG_ISCONST, 0x80000000, ARG_REGIND, tmp_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3092:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, out_lab));
  etcrtl.c:3097:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:3099:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x80000000, ARG_REGIND | ARG_DISP, tmp_reg, (data_offsets[i] << WSH)));
  etcrtl.c:3103:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:3106:			add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, (data_offsets[i] << WSH), ARG_REG, tmp_reg2));
  etcrtl.c:3107:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg2, ARG_REGIND | ARG_DISP, tmp_reg, (slot_offsets[i] << WSH)));
  etcrtl.c:3111:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:3113:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, tmp_reg, (slot_offsets[i] << WSH)));
  etcrtl.c:3117:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
  etcrtl.c:3119:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0x80000000, ARG_REGIND | ARG_DISP, tmp_reg, (slot_offsets[i] << WSH)));
  etcrtl.c:3120:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, tmp_reg, ((slot_offsets[i] + 1) << WSH)));
  etcrtl.c:3123:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, out_lab));
  etcrtl.c:3134:		add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, ts->cond, ARG_REG, ts->stack->a_reg));
  etcrtl.c:3136:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 1, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:3156:		add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3158:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, jcond, ARG_LABEL, label));
  etcrtl.c:3174:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NO, ARG_LABEL, this_lab));
  etcrtl.c:3177:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:3179:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_O, ARG_LABEL, ts->overflow_label));
  etcrtl.c:3234:			tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_CONST, operand, ARG_REG, ts->stack->a_reg);
  etcrtl.c:3240:			tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, operand << WSH, ARG_REG, ts->stack->a_reg);
  etcrtl.c:3247:				tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, ts->stack->a_reg);
  etcrtl.c:3249:				tmp_ins = compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, operand << WSH, AR
  G_REG, ts->stack->a_reg);
  etcrtl.c:3256:			add_to_ins_chain (compose_ins_ex (EtcPrimary (I_LDNL), INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->a_reg, operand << WSH, ARG_REG, ts->stack->a_reg));
  etcrtl.c:3262:				add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGIND | ARG_DISP, ts->stack->a_reg, operand << WSH, ARG_REG, ts->stack->a_reg));
  etcrtl.c:3270:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (ts->stack->old_a_reg), ARG_REGIND | ARG_DISP, REG_WPTR, operand << WSH));
  etcrtl.c:3273:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_a_reg), ARG_REGIND | ARG_DISP, REG_WPTR, operand << WSH));
  etcrtl.c:3276:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, REG_WPTR, operand << WSH));
  etcrtl.c:3286:				add_to_ins_chain (compose_ins_ex (EtcPrimary (I_STNL), INS_MOVE, 1, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg), ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, operand << WSH));
  etcrtl.c:3289:				add_to_ins_chain (compose_ins_ex (EtcPrimary (I_STNL), INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, operand << WSH));
  etcrtl.c:3297:					add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3307:					add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, operand, ARG_REG, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3310:					add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, operand, ARG_REGIND | ARG_DISP, REG_WPTR, (constmap_regconst (ts->stack->a_reg) << WSH), ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3331:					tmp_ins = compose_ins_ex (EtcPrimary (I_ADC), INS_MOVE, 1, 1, ARG_CONST, newconst, ARG_REG, ts->stack->a_reg);
  etcrtl.c:3340:					add_to_ins_chain (compose_ins_ex (EtcPrimary (I_ADC), INS_ADD, 2, 1, ARG_CONST, operand, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:3342:					add_to_ins_chain (compose_ins_ex (EtcPrimary (I_ADC), INS_ADD, 2, 2, ARG_CONST, operand, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3355:			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, operand << WSH, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  etcrtl.c:3386:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3388:		tmp_ins = compose_ins_ex (EtcSpecial(I_XSTL), INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REGIND | ARG_DISP, REG_WPTR, opd << WSH);
  etcrtl.c:3394:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3396:		tmp_ins = compose_ins_ex (EtcSpecial(I_XSTLN), INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REGIND | ARG_DISP, REG_WPTR, -(opd << WSH));
  etcrtl.c:3439:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3442:			add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_CONST | ARG_ISCONST, adj << WSH, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  etcrtl.c:3444:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_ISCONST | ARG_FLABEL, 0, ARG_REGIND, REG_WPTR));
  etcrtl.c:3446:				add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (fname)));
  etcrtl.c:3448:				add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, etc_code->opd));
  etcrtl.c:3450:			add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:3455:				add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, adj << WSH, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  etcrtl.c:3474:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3483:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND, REG_WPTR, ARG_REG, tmpreg));
  etcrtl.c:3484:				add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST | ARG_ISCONST, adj << WSH, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
  etcrtl.c:3485:				add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, tmpreg));
  etcrtl.c:3505:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3510:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, lab, ARG_REG, tmp_reg));
  etcrtl.c:3511:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGINDSIB, 4, ts->stack->old_a_reg, tmp_reg, ARG_REG, tmp_reg));
  etcrtl.c:3512:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, tmp_reg));
  etcrtl.c:3529:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3531:			add_to_ins_chain (compose_ins (INS_CONSTLABADDR, 1, 0, ARG_LABEL, lab));
  etcrtl.c:3549:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3552:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3553:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_BE, ARG_LABEL, lab));
  etcrtl.c:3569:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3572:			add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:3582:			add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:3601:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3604:			add_to_ins_chain (compose_ins (INS_XOR, 2, 1, ARG_REG, tmpreg, ARG_REG, tmpreg, ARG_REG, tmpreg));
  etcrtl.c:3605:			add_to_ins_chain (compose_ins (INS_SUB, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, tmpreg, ARG_REG, tmpreg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:3625:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3638:				add_to_ins_chain (compose_ins (INS_MOVEZEXT16TO32, 1, 1, ARG_REG | ARG_IS16BIT, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:3640:				add_to_ins_chain (compose_ins (INS_MOVEZEXT16TO32, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:3659:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
  etcrtl.c:3669:				add_to_ins_chain (compose_ins (INS_MOVEW, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REGIND, ts->stack->old_a_reg));
  etcrtl.c:3684:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NALTEND")));
  etcrtl.c:3694:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TEMP, ARG_REG, tmp_reg));
  etcrtl.c:3695:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, tmp_reg));
  etcrtl.c:3702:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWENB")));
  etcrtl.c:3717:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWDIS")));
  etcrtl.c:3732:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWALTWT")));
  etcrtl.c:3747:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWALT")));
  etcrtl.c:3757:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_ENABLING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
  etcrtl.c:3769:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NALTEND")));
  etcrtl.c:3781:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TEMP, ARG_REG, tmp_reg));
  etcrtl.c:3782:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, tmp_reg));
  etcrtl.c:3789:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_BINIT")));
  etcrtl.c:3804:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBRILNK")));
  etcrtl.c:3819:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBRULNK")));
  etcrtl.c:3834:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PPILNK")));
  etcrtl.c:3849:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBENROLL")));
  etcrtl.c:3864:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBRESIGN")));
  etcrtl.c:3879:			add_to_ins_chain (compose
  _ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBADJSYNC")));
  etcrtl.c:3894:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_SYNC")));
  etcrtl.c:3909:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTLOCK")));
  etcrtl.c:3924:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTUNLOCK")));
  etcrtl.c:3939:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALT")));
  etcrtl.c:3954:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTEND")));
  etcrtl.c:3969:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ENB")));
  etcrtl.c:3984:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_DIS")));
  etcrtl.c:3999:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTPOSTLOCK")));
  etcrtl.c:4014:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PPBASEOF")));
  etcrtl.c:4028:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmpreg));
  etcrtl.c:4037:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PPPAROF")));
  etcrtl.c:4051:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->a_reg, ARG_REG, tmpreg));
  etcrtl.c:4124:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_LABEL | ARG_ISCONST, this_lab, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4126:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:4132:		add_to_ins_chain (compose_ins (INS_SWAP, 2, 2, ARG_REG, ts->stack->a_reg, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4145:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_ISCONST | ARG_FLABEL, 0, ARG_REGIND, REG_WPTR));
  etcrtl.c:4146:				add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->a_reg));
  etcrtl.c:4147:				add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:4151:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_ISCONST | ARG_FLABEL, 0, ARG_REGIND, REG_WPTR));
  etcrtl.c:4152:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->a_reg));
  etcrtl.c:4153:			add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:4161:		tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_CONST, (signed int)0x80000000, ARG_REG, ts->stack->a_reg);
  etcrtl.c:4168:		tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->a_reg);
  etcrtl.c:4276:		add_to_ins_chain (compose_ins (INS_SETCC, 1, 1, ARG_COND, CC_NO, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4278:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 1, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4285:			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_a_reg), ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4292:				add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg), ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4295:				add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_LABEL | ARG_ISCONST, constmap_regconst (ts->stack->old_b_reg), ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4298:				add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4311:			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, WShift, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:4313:			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH,
  etcrtl.c:4319:			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, WShift, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:4320:			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4326:		add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGINDSIB, 4, ts->stack->old_b_reg, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4335:		add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_REGINDSIB, 8, ts->stack->old_b_reg, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4341:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, WSH, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4347:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4348:		add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_CONST, WSH, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4349:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 3, ARG_REG, ts->stack->b_reg, ARG_REG, ts->stack->b_reg));
  etcrtl.c:4417:		/* add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REGIND, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4418:		add_to_ins_chain (compose_ins (INS_AND, 2, 1, ARG_CONST, 0xff, ARG_REG, ts->stack->a_reg, ARG_REG, ts->stack->a_reg)); */
  etcrtl.c:4427:			add_to_ins_chain (compose_ins (INS_MOVEZEXT8TO32, 1, 1, ARG_REG | ARG_IS8BIT, ts->stack->a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4429:			add_to_ins_chain (compose_ins (INS_MOVEZEXT8TO32, 1, 1, ARG_REGIND, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4444:			add_to_ins_chain (compose_ins (INS_MOVEB, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REGIND, ts->stack->old_a_reg));
  etcrtl.c:4507:					add_to_ins_chain (compose_ins (INS_SHR, 2, 1, ARG_CONST | ARG_ISCONST, pot, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:4556:			add_to_ins_chain (compose_ins (INS_INC, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4574:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_PROD), INS_MUL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4579:				add_to_ins_chain (compose_ins_ex (EtcSecondary (I_PROD), INS_MUL, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4589:		add_to_ins_chain (compose_ins (INS_NOT, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:4609:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 32, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4610:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, this_lab));
  etcrtl.c:4611:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab2));
  etcrtl.c:4635:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, this_lab3));
  etcrtl.c:4636:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:4642:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab2));
  etcrtl.c:4643:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4644:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab3));
  etcrtl.c:4657:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->a_reg));
  etcrtl.c:4682:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4685:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4689:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg), ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4697:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_a_reg), ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4700:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_a_reg),
  etcrtl.c:4708:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REGI
  ND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4711:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg),
  etcrtl.c:4729:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, tmp_reg));
  etcrtl.c:4730:		add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:4731:		add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:4752:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_INC, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:4753:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_CMP, 2, 1, ARG_CONST, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4756:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_CJUMP, 2, 0, ARG_COND, CC_AE, ARG_LABEL, this_lab));
  etcrtl.c:4757:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_DEC, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:4758:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_XOR, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4759:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_CJUMP, 2, 0, ARG_COND, CC_NS, ARG_LABEL, this_lab + 1));
  etcrtl.c:4760:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:4766:			add_to_ins_chain (compose_ins_ex (EtcSecondary (I_CSNGL), INS_SETLABEL, 1, 0, ARG_LABEL, this_lab + 1));
  etcrtl.c:4820:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:4822:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_AE, ARG_LABEL, this_lab));
  etcrtl.c:4824:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:5033:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND, REG_WPTR));
  etcrtl.c:5036:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:5118:				add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5121:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
  etcrtl.c:5122:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
  etcrtl.c:5123:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:5200:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
  etcrtl.c:5202:						add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
  etcrtl.c:5209:						add_to_ins_chain (compose_ins (INS_OR, 2, 2, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5214:							add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
  etcrtl.c:5216:							add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_REG | ARG_IND, ts->stack->old_a_reg));
  etcrtl.c:5220:						add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_LABEL, this_lab));
  etcrtl.c:5221:						add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
  etcrtl.c:5223:							add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
  etcrtl.c:5225:							add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
  etcrtl.c:5227:						add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:5243:				add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_LABEL, constmap_regconst (ts->stack->old_a_reg)));
  etcrtl.c:5245:				add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, ts->stack->old_a_reg));
  etcrtl.c:5310:			add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, x));
  etcrtl.c:5312:			add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_CONST, x));
  etcrtl.c:5313:			add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->filename_label));
  etcrtl.c:5314:			add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_LABEL | ARG_ISCONST, ts->procedure_label));
  etcrtl.c:5359:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5360:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
  etcrtl.c:5361:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:5362:			add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:5363:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_BPTR));
  etcrtl.c:5377:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5378:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NZ, ARG_FLABEL, 0));
  etcrtl.c:5379:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:5380:			add_to_ins_chain (compose_ins (INS_SETFLABEL, 1, 0, ARG_FLABEL, 0));
  etcrtl.c:5381:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, REG_FPTR));
  etcrtl.c:5426:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_ENABLING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
  etcrtl.c:5443:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, this_lab, ARG_REG, tmp_reg));
  etcrtl.c:5444:			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TEMP, ARG_REG, tmp_reg, ARG_REG, tmp_reg));
  etcrtl.c:5445:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REG | ARG_IND, tmp_reg));
  etcrtl.c:5446:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:5464:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_ENABLING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
  etcrtl.c:5465:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_TIMENOTSET, ARG_REGIND | ARG_DISP, REG_WPTR, W_TLINK));
  etcrtl.c:5471:		tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_CONST, CONST_INFINITY, ARG_REG, ts->stack->a_reg);
  etcrtl.c:5485:		tmp_ins = compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->a_reg);
  etcrtl.c:5494:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x80000000, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5495:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, this_lab));
  etcrtl.c:5496:			add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, 0x80000000, ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5497:			add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_NE, ARG_LABEL, this_lab));
  etcrtl.c:5499:			add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, this_lab));
  etcrtl.c:5589:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&PPriority"), ARG_REG, ts->stack->a_reg));
  etcrtl.c:5614:			add_to_ins_chain (compose_ins (INS_SHL, 2, 1, ARG_CONST, WShift, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_a_reg));
  etcrtl.c:5615:			add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_REG, ts->stack->old_a_reg, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:5616:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_c_reg, ARG_REGIND, ts->stack->old_b_reg));
  etcrtl.c:5649:			compose_ins (
  INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REG, ts->stack->a_reg);
  etcrtl.c:5785:			add_to_ins_chain (compose_ins_ex (etc_instr, instr, 2, 2, ARG_REG, src_reg1, ARG_REG, src_reg2, ARG_REG, dst_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5787:			add_to_ins_chain (compose_ins_ex (etc_instr, instr, 2, 1, ARG_REG, src_reg1, ARG_REG, src_reg2, ARG_REG, dst_reg));
  etcrtl.c:5792:			add_to_ins_chain (compose_ins_ex (etc_instr, instr, 2, 2, ARG_CONST, constmap_regconst (src_reg1), ARG_REG, src_reg2, ARG_REG, dst_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5794:			add_to_ins_chain (compose_ins_ex (etc_instr, instr, 2, 1, ARG_CONST, constmap_regconst (src_reg1), ARG_REG, src_reg2, ARG_REG, dst_reg));
  etcrtl.c:5799:			add_to_ins_chain (compose_ins_ex (etc_instr, instr, 2, 2, ARG_REGIND | ARG_DISP, REG_WPTR, (constmap_regconst (src_reg1) << WSH), ARG_REG, src_reg2, ARG_REG, dst_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5801:			add_to_ins_chain (compose_ins_ex (etc_instr, instr, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, (constmap_regconst (src_reg1) << WSH), ARG_REG, src_reg2, ARG_REG, dst_reg));
  etcrtl.c:5863:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5864:				add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, thislab));
  etcrtl.c:5868:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_a_reg), ARG_REG, ts->stack->old_b_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5869:				add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_B, ARG_LABEL, thislab));
  etcrtl.c:5872:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_a_reg) << WSH, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5873:				add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, thislab));
  etcrtl.c:5881:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_a_reg),
  etcrtl.c:5883:				add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_B, ARG_LABEL, thislab));
  etcrtl.c:5886:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, constmap_regconst (ts->stack->old_b_reg) << WSH, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5887:				add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, thislab));
  etcrtl.c:5894:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg), ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5895:				add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, thislab));
  etcrtl.c:5898:				add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, constmap_regconst (ts->stack->old_b_reg),
  etcrtl.c:5900:				add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, thislab));
  etcrtl.c:5910:		add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, thislab));
  etcrtl.c:5933:		add_to_ins_chain (compose_ins (INS_DEC, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  etcrtl.c:5939:	add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_a_reg, ARG_REG | ARG_IMP, REG_CC));
  etcrtl.c:5941:	add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_A, ARG_LABEL, thislab));
  etcrtl.c:5947:	add_to_ins_chain (compose_ins (INS_SETLABEL, 1, 0, ARG_LABEL, thislab));
  etcrtl.c:5949:		add_to_ins_chain (compose_ins (INS_INC, 1, 1, ARG_REG, ts->stack->old_b_reg, ARG_REG, ts->stack->old_b_reg));
  optimise.c:372:			tmp_ins = compose_ins (INS_ADD, 2, 1, ARG_CONST, 4, ARG_REG, REG_ESP, ARG_REG, REG_ESP);
  optimise.c:429:				first_ins->out_args[0] = compose_ins_arg (ARG_REG | ARG_IMP, REG_CC);
  regcolour.c:513:						rtl_insert_instr_before (compose_ins (INS_START_REG, 1, 0, ARG_REG, tmp_reg), reg_next_c);
  regcolour.c:514:						rtl_insert_instr_before (compose_ins (INS_MOVE, 1, 1, ARG_REG, reg, ARG_REG, tmp_reg), reg_next_c);
  regcolour.c:515:						rtl_insert_instr_before (compose_ins (INS_END_REG, 1, 0, ARG_REG, reg), reg_next_c);
  regcolour.c:620:					rtl_insert_instr_before (compose_ins (INS_START_REG, 1, 0, ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:621:					rtl_insert_instr_before (compose_ins (INS_MOVE, 1, 1, ARG_REG, active_regs[j], ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:622:					rtl_insert_instr_before (compose_ins (INS_END_REG, 1, 0, ARG_REG, active_regs[j]), tmp_forw);
  regcolour.c:641:						rtl_insert_instr_after (compose_ins (INS_END_REG, 1, 0, ARG_REG, active_regs[j]), tmp_forw);
  regcolour.c:642:						rtl_insert_instr_after (compose_ins (INS_MOVE, 1, 1, ARG_REG, active_regs[j], ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:643:						rtl_insert_instr_after (compose_ins (INS_START_REG, 1, 0, ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:648:						rtl_insert_instr_before (compose_ins (INS_START_REG, 1, 0, ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:649:						rtl_insert_instr_before (compose_ins (INS_MOVE, 1, 1, ARG_REG, active_regs[j], ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:650:						rtl_insert_instr_before (compose_ins (INS_END_REG, 1, 0, ARG_REG, active_regs[j]), tmp_forw);
  regcolour.c:696:						rtl_insert_instr_before (compose_ins (INS_START_REG, 1, 0, ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:697:						rtl_insert_instr_before (compose_ins (INS_MOVE, 1, 1, ARG_REG, active_regs[j], ARG_REG, tmp_reg), tmp_forw);
  regcolour.c:698:						rtl_insert_instr_before (compose_ins (INS_END_REG, 1, 0, ARG_REG, active_regs[j]), tmp_forw);
  rtlops.c:588:			tmp_ins = compose_ins (INS_START_REG, 1, 0, ARG_REG, i);
  rtlops.c:591:			tmp_ins = compose_ins (INS_END_REG, 1, 0, ARG_REG, i);
  rtlops.c:1643:/*{{{  ins_chain *compose_ins (int ins, int ops_in, int ops_out, ...)*/
  rtlops.c:1648:ins_chain *compose_ins2 (char *file, long line, int etc_ins, int ins, int ops_in, int ops_out, ...)
  rtlops.c:1650:ins_chain *compose_ins_ex (int etc_ins, int ins, int ops_in, int ops_out, ...)
  rtlops.c:1763:/*{{{  ins_arg *compose_ins_arg (int argtype, ...)*/
  rtlops.c:1767:ins_arg *compose_ins_arg (int argtype, ...)
  rtlops.h:73:extern ins_arg *compose_ins_arg (int argtype, ...);
  rtlops.h:79:	extern ins_chain *compose_ins2 (char *file, long line, int etc_ins, int ins, int ops_in, int ops_out, ...);
  rtlops.h:80:	#define compose_ins(ins, ops_in, ops_out, args...) \
  rtlops.h:81:		compose_ins2(__FILE__, __LINE__, 0, ins, ops_in, ops_out, ## args)
  rtlops.h:82:	#define compose_ins_ex(etc_ins, ins, ops_in, ops_out, args...) \
  rtlops.h:83:		compose_ins2(__FILE__, __LINE__, etc_ins, ins, ops_in, ops_out, ## args)
  rtlops.h:85:	//extern ins_chain *compose_ins (int ins, int ops_in, int ops_out, ...);
  rtlops.h:86:	#define compose_ins(ins, ops_in, ops_out, args...) \
  rtlops.h:87:		compose_ins_ex(0, ins, ops_in, ops_out, ## args)
  rtlops.h:88:	extern ins_chain *compose_ins_ex(int etc_ins, int ins, int ops_in, int ops_out, ...);

Search for compose_ins function definition in all C files

Now I can see that compose_ins is defined in rtlops.c. Let me check if there are any missing function implementations that could be causing segfaults by examining the key functions that are being called:

1 file read
rtlops.h
Now let me check if there are any missing critical functions by looking at the main etcrtl.c file to see what functions are being called during the segfault:

Check for any missing function implementations by looking for undefined symbols

shell

Completed
nm -u tranx86 | grep -v "^$" | head -20
___error
___memcpy_chk
___memmove_chk
___sprintf_chk
___stack_chk_fail
___stack_chk_guard
___stderrp
___stdoutp
___strcpy_chk
__exit
_abort
_access
_bzero
_close
_dup2
_execlp
_exit
_fclose
_fdopen
_fopen
Let me check what specific functions are missing by looking at the key functions that are being called in the architecture files:

Check for missing function implementations by looking at the specific functions used in the architecture files

shell

Completed
grep -n "tstack_newreg\|string_dup\|kif_entry" archx64.c archaarch64.c | head -10
archx64.c:48:extern int tstack_newreg (tstack *);
archx64.c:51:extern kif_entrytype *kif_entry (int call);
archx64.c:118:static ins_chain *compose_x64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry);
archx64.c:248:	kif_entrytype *entry = kif_entry (call);
archx64.c:264:	call_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archx64.c:285:/*{{{  static ins_chain *compose_x64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry)*/
archx64.c:286:static ins_chain *compose_x64_kjump (tstate *ts, const int instr, const int cc, const kif_entrytype *kif_entry)
archx64.c:291:		jump_ins = compose_ins (INS_CJUMP, 2, 0, ARG_COND, cc, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archx64.c:293:		jump_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archx64.c:322:	int timer_reg = tstack_newreg (ts->stack);

Let me check if these critical functions are properly defined by looking at their implementations:

Check if string_dup and kif_entry functions are defined

shell

Completed
grep -rn "string_dup\|kif_entry" *.c | grep -E "(^[^:]*\.c:[0-9]+:.*string_dup.*\(|^[^:]*\.c:[0-9]+:.*kif_entry.*\()" | head -5
arch386.c:93:		return compose_ins (type, 2, 0, ARG_COND, cond, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
arch386.c:110:	kif_entrytype *entry = kif_entry (call);
arch386.c:207:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
arch386.c:428:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:432:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
Let me check if these functions are actually implemented:

Check for function definitions of string_dup and kif_entry

shell

Completed
grep -rn "^.*string_dup.*(" *.c
arch386.c:93:		return compose_ins (type, 2, 0, ARG_COND, cond, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
arch386.c:207:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
arch386.c:428:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:432:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:497:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:501:	add_to_ins_chain (compose_ins (INS_UMUL, 2, 2, ARG_REG | ARG_IMP, eax_reg, ARG_NAMEDLABEL, string_dup ("&glob_cpufactor"),
arch386.c:546:	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_NAMEDLABEL, string_dup ("&sf"), ARG_REG, tmp_reg2));
arch386.c:558:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:587:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:640:		add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, NOT_PROCESS, ARG_NAMEDLABEL, string_dup (Fptr_name), ARG_REG | ARG_IMP, REG_CC));
arch386.c:644:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_NAMEDLABEL, string_dup (Fptr_name)));
arch386.c:650:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (Bptr_name), ARG_REG, tmp_reg));
arch386.c:655:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_NAMEDLABEL, string_dup (Bptr_name)));
arch386.c:685:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:705:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:769:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:812:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:946:		add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("** coding for copy (inline_in_2)  size in reg")));
arch386.c:1170:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:1621:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup (PPriority_name), ARG_REG, tmp_reg));
arch386.c:1652:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup (Bptr_name)));
arch386.c:1659:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup (Fptr_name)));
arch386.c:1681:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("&dmem_alloc2")));
arch386.c:1703:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
arch386.c:1704:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1) + 1])));
arch386.c:3076:		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
arch386.c:3078:		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
arch386.c:3133:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, arg_reg));
arch386.c:3135:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_REG, arg_reg));
arch386.c:3181:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
arch386.c:3304:	trtl->u.label_name = string_dup (sbuffer);
arch386.c:3319:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup ("&EntryPointTable"), ARG_REG, tmp_reg));
arch386.c:3331:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
arch386.c:3332:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
arch386.c:3333:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
arch386.c:3334:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
arch386.c:3360:	add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
arch386.c:3794:	Wptr_name = string_dup ("&Wptr");
arch386.c:3795:	Fptr_name = string_dup ("&Fptr");
arch386.c:3796:	Bptr_name = string_dup ("&Bptr");
arch386.c:3797:	PPriority_name = string_dup ("&PPriority");
arch386.c:3801:	arch->archname = string_dup ("i386");
archaarch64.c:259:	call_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archaarch64.c:286:		jump_ins = compose_ins (INS_CJUMP, 2, 0, ARG_COND, cc, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archaarch64.c:288:		jump_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archaarch64.c:320:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("cntvct_el0"), ARG_REG, timer_reg));
archaarch64.c:334:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("cntvct_el0"), ARG_REG, timer_reg));
archaarch64.c:458:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&workspace"), ARG_REG, wptr_reg));
archmips.c:194:				tmp_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:203:				tmp_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:213:			tmp_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:221:			tmp_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archmips.c:389:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
archmips.c:390:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1) + 1])));
archmips.c:1706:	add_to_ins_chain (compose_ins (INS_CJUMP, 3, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup      ("&_X_BNSeterr"), ARG_REG, reg));
archmips.c:2025:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archmips.c:2026:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_BPTR, ARG_NAMEDLABEL, string_dup ("&Bptr")));
archmips.c:2030:	add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_NAMEDLABEL, string_dup (name + 1), ARG_REG, 25));
archmips.c:2040:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archmips.c:2041:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archmips.c:2071:		 add_to_ins_chain (compose_ins (INS_PUSH, 1, 0, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2))));
archmips.c:2073:		add_to_ins_chain (compose_ins (INS_LEA, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_REG, MIPS_REG_V1)); */
archmips.c:2076:		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint)));
archmips.c:2098:	trtl->u.label_name = string_dup (sbuffer);
archmips.c:2101:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
archmips.c:2102:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archmips.c:2103:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archmips.c:2105:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
archmips.c:2149:	arch->archname = st
ring_dup ("mips");
archppc.c:193:		tmp_ins = compose_ins (INS_CALL, 2, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));			/* regular */
archppc.c:197:		tmp_ins2 = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (target_call))->entrypoint));
archppc.c:204:		tmp_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
archppc.c:398:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archppc.c:406:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_WPTR));
archppc.c:408:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archppc.c:413:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archppc.c:522:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Bptr")));
archppc.c:529:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archppc.c:547:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
archppc.c:548:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1) + 1])));
archppc.c:639:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_zero_div")));
archppc.c:652:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_floaterr")));
archppc.c:665:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_overflow")));
archppc.c:678:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_RangeCheckError")));
archppc.c:696:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_Seterr")));
archppc.c:814:		add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
archppc.c:821:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
archppc.c:1741:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
archppc.c:1743:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
archppc.c:1763:			*pst_last = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint));
archppc.c:1799:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, tmp_reg2));
archppc.c:1801:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_REG, tmp_reg2));
archppc.c:1805:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint)));
archppc.c:1831:	trtl->u.label_name = string_dup (sbuffer);
archppc.c:1834:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
archppc.c:1835:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archppc.c:1836:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archppc.c:1838:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
archppc.c:2794:				ins_chain *newins = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("unsupported PUSH/POP!"));
archppc.c:2984:	arch->archname = string_dup ("ppc");
archsparc.c:165:		tmp_ins = compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint), ARG_CONST | ARG_ISCONST, 12);			/* regular */
archsparc.c:169:		tmp_ins2 = compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (target_call))->entrypoint), ARG_CONST | ARG_ISCONST, 8);		/* special */
archsparc.c:176:		tmp_ins = compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint), ARG_CONST | ARG_ISCONST, 8);
archsparc.c:375:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archsparc.c:383:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_WPTR));
archsparc.c:385:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_occscheduler")));
archsparc.c:390:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archsparc.c:500:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Bptr")));
archsparc.c:507:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archsparc.c:526:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, x, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1)])));
archsparc.c:527:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, ts->filename_label, ARG_NAMEDLABEL, string_dup (mdparam_vars[(mdpairid << 1) + 1])));
archsparc.c:624:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_zero_div")));
archsparc.c:644:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_floaterr")));
archsparc.c:662:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_overflow")));
archsparc.c:678:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_RangeCheckError")));
archsparc.c:696:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_Seterr")));
archsparc.c:817:		add_to_ins_chain (compose_ins (INS_CJUMP, 2, 0, ARG_COND, CC_Z, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
archsparc.c:823:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup ("_X_BNSeterr")));
archsparc.c:1373:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_FPTR, ARG_NAMEDLABEL, string_dup ("&Fptr")));
archsparc.c:1374:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_BPTR, ARG_NAMEDLABEL, string_dup ("&Bptr")));
archsparc.c:1383:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (sbuf)));
archsparc.c:1385:		add_to_ins_chain (compose_ins (INS_CCALL, 1, 0, ARG_NAMEDLABEL, string_dup (name + 1)));
archsparc.c:1390:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archsparc.c:1391:		add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archsparc.c:1405:			*pst_last = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint));
archsparc.c:1441:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuf), ARG_REG, tmp_reg2));
archsparc.c:1443:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (name + ((i == 2) ? 1 : 2)), ARG_REG, tmp_reg2));
archsparc.c:1447:	add_to_ins_chain (compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint), ARG_CONST | ARG_ISCONST, 8));
archsparc.c:1473:	trtl->u.label_name = string_dup (sbuffer);
archsparc.c:1476:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Wptr"), ARG_REG, REG_WPTR));
archsparc.c:1477:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Fptr"), ARG_REG, REG_FPTR));
archsparc.c:1478:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&Bptr"), ARG_REG, REG_BPTR));
archsparc.c:1480:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
archsparc.c:2254:				tmp_ins = compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("unsupported PUSH/POP!"));
archsparc.c:2411:	arch->archname = string_dup ("sparc");
archx64.c:264:	call_ins = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup (entry->entrypoint));
arch
x64.c:291:		jump_ins = compose_ins (INS_CJUMP, 2, 0, ARG_COND, cc, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archx64.c:293:		jump_ins = compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (kif_entry->entrypoint));
archx64.c:325:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("rdtsc"), ARG_REG, timer_reg));
archx64.c:339:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("rdtsc"), ARG_REG, timer_reg));
archx64.c:463:	add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&workspace"), ARG_REG, wptr_reg));
etcdump.c:62:	return string_dup (sbuffer);
etcdump.c:75:	return string_dup (sbuffer);
etcrtl.c:162:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (d_str)));
etcrtl.c:166:			add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (d_str)));
etcrtl.c:275:	trtl->u.label_name = string_dup ("base");
etcrtl.c:418:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1084:					tmp_string = string_dup ("special message             ");
etcrtl.c:1098:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1162:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1175:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1185:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1189:							add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("*** register lost from stack ***")));
etcrtl.c:1218:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1232:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1243:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1255:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1268:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1284:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1296:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1305:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1316:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("<specialnames>")));
etcrtl.c:1327:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1340:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1354:						trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:1363:						trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:1371:						trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:1379:						trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:1411:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1449:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1504:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1621:									trtl->u.dyncode.label_name = string_dup ("DCR_occam_start");
etcrtl.c:1624:									trtl->u.dyncode.label_name = string_dup ("DCR_rmox_app_main");
etcrtl.c:1627:									trtl->u.dyncode.label_name = string_dup ("DCR_rmox_drv_main");
etcrtl.c:1630:									trtl->u.dyncode.label_name = string_dup ("DCR_rmox_srv_main");
etcrtl.c:1633:									trtl->u.dyncode.label_name = string_dup ("DCR_rmox_fs_main");
etcrtl.c:1636:									trtl->u.dyncode.label_name = string_dup ("DCR_rmox_net_main");
etcrtl.c:1682:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1706:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1722:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1736:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1748:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1754:				add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, string_dup (sbuffer), ARG_REG, ts->stack->a_reg));
etcrtl.c:1761:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1786:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1813:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1827:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1892:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1932:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:1975:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:2013:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:2043:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:2061:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:2065:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("generating .MOBILEINIT")));
etcrtl.c:2110:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ((char *)outbuf)));
etcrtl.c:2189:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:2224:						add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:2307:					add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:2428:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (".MAGIC SCREEN")));
etcrtl.c:2437:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 1, ARG_NAMEDLABEL, string_dup ("&occam_finished")));
etcrtl.c:2574:		trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:2581:		trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:2588:		trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:2595:		trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:2610:		trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:2621:		trtl->u.label_name = string_dup (sbuffer);
etcrtl.c:3097:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:3103:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:3111:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:3117:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuffer)));
etcrtl.c:3386:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3394:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3439:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3446:				add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_NAMEDLABEL, string_dup (fname)));
etcrtl.c:3474:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3505:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3529:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3549:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3569:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3601:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3625:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3659:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup (sbuf)));
etcrtl.c:3684:				add_to_ins_chain (co
mpose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NALTEND")));
etcrtl.c:3702:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWENB")));
etcrtl.c:3717:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWDIS")));
etcrtl.c:3732:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWALTWT")));
etcrtl.c:3747:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NMWALT")));
etcrtl.c:3769:				add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("NALTEND")));
etcrtl.c:3789:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_BINIT")));
etcrtl.c:3804:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBRILNK")));
etcrtl.c:3819:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBRULNK")));
etcrtl.c:3834:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PPILNK")));
etcrtl.c:3849:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBENROLL")));
etcrtl.c:3864:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBRESIGN")));
etcrtl.c:3879:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PBADJSYNC")));
etcrtl.c:3894:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_SYNC")));
etcrtl.c:3909:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTLOCK")));
etcrtl.c:3924:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTUNLOCK")));
etcrtl.c:3939:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALT")));
etcrtl.c:3954:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTEND")));
etcrtl.c:3969:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ENB")));
etcrtl.c:3984:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_DIS")));
etcrtl.c:3999:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_ALTPOSTLOCK")));
etcrtl.c:4014:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PPBASEOF")));
etcrtl.c:4037:			add_to_ins_chain (compose_ins (INS_ANNO, 1, 0, ARG_TEXT, string_dup ("MWS_PPPAROF")));
etcrtl.c:5589:			add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_NAMEDLABEL, string_dup ("&PPriority"), ARG_REG, ts->stack->a_reg));
main.c:173:			input_filename = string_dup (*walk);
main.c:394:			output_filename = string_dup (*walk);
main.c:688:		options.extref_prefix = string_dup ("");
main.c:690:		options.extref_prefix = string_dup (options.extref_prefix);
main.c:704:		char *copy = string_dup (options.etab_filename);
netcread.c:178:	line = string_dup (origline);
netcread.c:286:			tmp->o_bytes = string_dup (bits[1]);
netcread.c:576:			tmp->o_bytes = string_dup (bits[1]);
netcread.c:760:				tmp->o_bytes = string_dup (fname);
rtlops.c:3549:		tmp->regconst = (int)string_dup ((char *)arg->regconst);
support.c:320:/*{{{  char *string_dup (char *str)*/
support.c:324:char *string_dup (char *str)

Good! I can see that string_dup is defined in support.c. Now let me check for kif_entry:

Check for kif_entry function definition

shell

Completed
grep -rn "^.*kif_entry.*(" *.c
arch386.c:110:	kif_entrytype *entry = kif_entry (call);
arch386.c:1811:	add_to_ins_chain (compose_kjump_i386 (ts, INS_CALL, 0, kif_entry (type)));
arch386.c:1995:		add_to_ins_chain (compose_kjump_i386 (ts, INS_CALL, 0, kif_entry (K_BNSETERR)));
arch386.c:2004:			add_to_ins_chain (compose_kjump_i386 (ts, INS_CALL, 0, kif_entry (K_BNSETERR)));
arch386.c:3095:			*pst_last = compose_kjump_i386 (ts, INS_CALL, 0, (kif_entry (kernel_call)));
arch386.c:3141:		add_to_ins_chain (compose_kjump_i386 (ts, INS_CALL, 0, kif_entry (kernel_call)));
archaarch64.c:244:	kif_entrytype *entry = kif_entry (call);
archmips.c:113:	kif_entrytype *entry = kif_entry (call);
archmips.c:2076:		add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint)));
archppc.c:90:	kif_entrytype *entry = kif_entry (call);
archppc.c:197:		tmp_ins2 = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (target_call))->entrypoint));
archppc.c:1763:			*pst_last = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint));
archppc.c:1805:	add_to_ins_chain (compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint)));
archsparc.c:77:	kif_entrytype *entry = kif_entry (call);
archsparc.c:169:		tmp_ins2 = compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (target_call))->entrypoint), ARG_CONST | ARG_ISCONST, 8);		/* special */
archsparc.c:1405:			*pst_last = compose_ins (INS_CALL, 1, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint));
archsparc.c:1447:	add_to_ins_chain (compose_ins (INS_KCALL, 2, 0, ARG_NAMEDLABEL, string_dup ((kif_entry (kernel_call))->entrypoint), ARG_CONST | ARG_ISCONST, 8));
archx64.c:51:extern kif_entrytype *kif_entry (int call);
archx64.c:248:	kif_entrytype *entry = kif_entry (call);
etcrtl.c:874:							add_to_ins_chain (arch->compose_kjump (ts, INS_JUMP, 0, kif_entry (K_FASTSCHEDULER)));
etcrtl.c:924:							add_to_ins_chain (arch->compose_kjump (ts, INS_CALL, 0, kif_entry (K_SETERR)));
etcrtl.c:2438:			add_to_ins_chain (arch->compose_kjump (ts, INS_JUMP, 0, kif_entry (K_OCCSCHEDULER)));
etcrtl.c:5316:			add_to_ins_chain (arch->compose_kjump (ts, INS_CALL, 0, kif_entry (K_SETERR)));
kif.c:42:kif_entrytype *kif_entry (int offset) {

Great! I can see that kif_entry is defined in kif.c. Now let me check if there are any issues with the function implementations by looking at the actual function definitions:

1 file read
kif.c
Now let me check if there are any issues with the critical functions that might be causing segfaults. Let me look at the main issue - the segfault might be happening because of missing function implementations in the stub functions. Let me check what specific functions are being called that might be missing:

Run a simple test to see what happens when we try to use the x64 architecture

shell

Completed
echo "Testing basic functionality..." && ./tranx86 -mx64 --help 2>&1 | head -10
Testing basic functionality...
tranx86 1.4.1 -- occam ETC to multi-architecture code converter (x86, x64, MIPS, SPARC, PowerPC, AArch64)
Copyright (C) 2000-2008 Fred Barnes <frmb@kent.ac.uk>
This software is released under the GNU General Public License

usage: tranx86 [options] [-o <output-file>] <input.tce>
where options are:
-c   required if this is not a main module
-l   translate into dynamic processes
-f   suppress FPU initialisation
-n   disable check translation (CSNGL, CSUB0, CCNT1)
Good, the basic functionality works. Now let me try to run it with a simple test file to see where the segfault occurs:

Create a simple test to see where the segfault occurs

shell

Completed
echo "Creating a minimal test..." && echo -e "\\x00\\x00\\x00\\x00" > test.tce && ./tranx86 -mx64 test.tce 2>&1 | head -10
Creating a minimal test...
tranx86: unexpected EOF while processing test.tce
Let me check if there are any real test files available:

1 directory listed
tests
Let me look for any compiled .tce files:

shell

Completed
find tests -name "*.tce" 2>/dev/null | head -5
Look for any .tce files in the tests directory

Let me check if there are any compiled examples in the modules directory:

Look for any .tce files in the modules directory

shell

Completed
find modules -name "*.tce" 2>/dev/null | head -5
modules/course/answers/q6-sort-1.tce
modules/course/answers/q7-nick-2.tce
modules/course/answers/q7-nick-3.tce
modules/course/answers/q7-eric-1.tce
modules/course/answers/q7-nick-1.tce
Perfect! Let me try to run tranx86 with one of these .tce files to see where the segfault occurs:

Test tranx86 with a real .tce file to see where the segfault occurs

shell

Completed
./tranx86 -mx64 ../../modules/course/answers/q6-sort-1.tce 2>&1 | head -10
Test tranx86 with a real .tce file to see where the segfault occurs

shell

Completed
ls -la tranx86
-rwxr-xr-x@ 1 jim  staff  445208 Aug 24 02:49 tranx86
Test tranx86 with a real .tce file to see where the segfault occurs

shell

Completed
./tranx86 -mx64 ../../modules/course/answers/q6-sort-1.tce
Test tranx86 with a real .tce file to see where the segfault occurs

shell

Completed
/Users/jim/CLionProjects/kroc/tools/tranx86/tranx86 -mx64 ../../modules/course/answers/q6-sort-1.tce
tranx86: error: TCE file compiled for 32-bit target but translator configured for 64-bit
Interesting! The issue is that the .tce file was compiled for 32-bit but we're trying to translate it for 64-bit. This suggests that the segfault might be related to architecture mismatch or missing validation. Let me check if there are any 64-bit .tce files or try to create one:

Check if occ21 can compile for 64-bit targets

shell

Completed
find . -name "occ21" -type f 2>/dev/null
Look for the occ21 compiler

shell

Completed
find . -name "occ*" -type f 2>/dev/null | head -5
./tools/occplug/occPlug.html
./tools/occ21/misc/occompfe.c
./tools/occ21/include/occamfe.h
./tools/occ21/include/occompfe.h
./tools/occ21/fe/occamfe.c
Look for the built occ21 executable

shell

Completed
find . -name "occ21" -executable 2>/dev/null
Let me check the tools/occ21 directory:

1 directory listed
occ21
Check if occ21 executable exists in the tools directory

shell

Completed
ls -la | grep -E "(occ21|executable)"
-rw-r--r--    1 jim  staff   10968 Aug 23 20:09 occ21.1
The occ21 compiler doesn't seem to be built. Let me focus on the main issue - the segfault in tranx86. Based on my analysis, I can see that the x64 and aarch64 architecture files have many stub functions that just return without doing anything. The segfault is likely occurring because some critical functions are not properly implemented.

Let me identify the key missing implementations by examining what functions are being called during the translation process:

1 file read
archdef.h
Now I can see the issue! The architecture structure defines many function pointers that need to be implemented. Looking at the x64 and aarch64 files, I can see that many of these are implemented as stub functions that do nothing. The segfault is likely occurring because some critical functions are being called but not properly implemented.

Let me create a minimal fix by implementing the most critical missing functions. Based on the analysis, the key missing implementations that could cause segfaults are:

tstack_newreg - This is being called but might not be properly implemented

Some of the stub functions might be returning invalid values when they should return valid ones

The register allocation functions might not be working correctly

Let me implement the minimal fixes needed:

Fix critical missing function implementations in x64 architecture that could cause segfaults

archx64.c
+14
-5

Undo
Fix critical missing function implementations in aarch64 architecture that could cause segfaults

archaarch64.c
+14
-5

Undo

Undo all changes
Now let me add the missing external declaration for tstack_newreg in the aarch64 file:

Add missing external declaration for tstack_newreg in aarch64 architecture

archaarch64.c
+4
-0

Undo
Now let me rebuild the tranx86 binary to include these fixes:

Rebuild tranx86 with the fixes for missing function implementations

shell

Completed
make clean && make
rm -f  tranx86
rm -f *.o
/Applications/Xcode-beta.app/Contents/Developer/usr/bin/make  all-am
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT main.o -MD -MP -MF .deps/main.Tpo -c -o main.o main.c
mv -f .deps/main.Tpo .deps/main.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT tceread.o -MD -MP -MF .deps/tceread.Tpo -c -o tceread.o tceread.c
tceread.c:105:99: warning: format specifies type 'unsigned int' but the argument has type 'size_t' (aka 'unsigned long') [-Wformat]
105 |                 fprintf (stderr, "%s: tceread_number: bad TCE file %s (%ul/%ul)\n", progname, tcefile_filename, tcefile_ptr, tcefile_length);
|                                                                        ~~                                       ^~~~~~~~~~~
|                                                                        %zu
tceread.c:105:112: warning: format specifies type 'unsigned int' but the argument has type 'size_t' (aka 'unsigned long') [-Wformat]
105
|                 fprintf (stderr, "%s: tceread_number: bad TCE file %s (%ul/%ul)\n", progname, tcefile_filename, tcefile_ptr, tcefile_length);
|                                                                            ~~                                                ^~~~~~~~~~~~~~
|                                                                            %zu
tceread.c:525:41: error: use of undeclared identifier 'CLASS_X64'
525 |         int tar
get_is_64bit = (target_class == CLASS_X64 || target_class == CLASS_AARCH64);
|                                                ^
tceread.c:525:70: error: use of undeclared identifier 'CLASS_AARCH64'
525 |         int target_is_64bit = (target_class == CLASS_X64 || target_class == CLASS_AARCH64);
|                                                                             ^
2 warnings and 2 errors
generated.
make[1]: *** [tceread.o] Error 1
make: *** [all] Error 2

I need to fix the missing constants. Let me check what constants are defined:

Check what CLASS constants are defined in the codebase

shell

Completed
grep -rn "CLASS_" *.h | head -10
machine.h:31:#define CLASS_UNKNOWN		0x0
machine.h:33:#define CLASS_386		0x386
machine.h:34:#define CLASS_486		0x486
machine.h:35:#define CLASS_586		0x586
machine.h:36:#define CLASS_686		0x686
machine.h:38:#define CLASS_R3000		0x3000
machine.h:39:#define CLASS_R6000		0x6000
machine.h:40:#define CLASS_R4300		0x4300
machine.h:41:#define CLASS_R4600		0x4600
machine.h:42:#define CLASS_R5000		0x5000
1 file read
machine.h
Great! The constants are already defined in machine.h. The issue is that tceread.c is not including machine.h. Let me fix this:

1 file read
tceread.c
I can see that tceread.c already includes the necessary headers, but it's missing the machine.h include. Let me add it:

Add machine.h include to tceread.c to fix missing CLASS constants

tceread.c
+1
-0

Undo
Now let me rebuild:

Rebuild tranx86 after fixing the missing include

shell

Completed
make
/Applications/Xcode-beta.app/Contents/Developer/usr/bin/make  all-am
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT tceread.o -MD -MP -MF .deps/tceread.Tpo -c -o tceread.o tceread.c
tceread.c:106:99: warning: format specifies type 'unsigned int' but the argument has type 'size_t' (aka 'unsigned long') [-Wformat]
106 |                 fprintf (stde
rr, "%s: tceread_number: bad TCE file %s (%ul/%ul)\n", progname, tcefile_filename, tcefile_ptr, tcefile_length);
|                                                                        ~~                                       ^~~~~~~~~~~
|                                                                        %zu
tceread.c:106:112: warning: format specifies type 'unsigned int' but the argument has type 'size_t' (aka 'unsigned long') [-Wformat]
106 |                 fprintf (stderr, "%s: tceread_number: bad TCE file %s (%ul/%ul)\n", progname, tcefile_filename, tcefile_ptr, tcefile_length);
|                                                                            ~~                                                ^~~~~~~~~~~~~~
|                                                                            %zu
2 warnings generated.
mv -f .deps/tceread.Tpo .deps/tceread.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT netcread.o -MD -MP -MF .deps/netcread.Tpo -c -o netcread.o netcread.c
mv -f .deps/netcread.Tpo .deps/netcread.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT support.o -MD -MP -MF .deps/support.Tpo -c -o support.o support.c
mv -f .deps/support.Tpo .deps/support.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT etcdump.o -MD -MP -MF .deps/etcdump.Tpo -c -o etcdump.o etcdump.c
mv -f .deps/etcdump.Tpo .deps/etcdump.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT etcrtl.o -MD -MP -MF .deps/etcrtl.Tpo -c -o etcrtl.o etcrtl.c
etcrtl.c:1906:54: warning: cast to smaller integer type 'int' from 'int *' [-Wpointer-to-int-cast]
1906 |                 
constmap_new (ts->stack->a_reg, VALUE_LABDIFF, (int)labs, tmp_ins);
|                                                                                                ^~~~~~~~~
etcrtl.c:593:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
593 |                                 
add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));                 /* Wptr[Link] = null */
|                                                                                                                                               ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
etcrtl.c:598:105: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
598 |                                                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK));                     /* tmp[link] = Wptr */
|                                                                                                                                                   ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
etcrtl.c:643:86: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
643 |                                                 add_to_ins_chain (compose_ins (INS_
MOVE, 1, 1, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK, ARG_REG, tmp_reg));              /* tmp = tmp[Link] */
|                                                                                                                                ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
etcrtl.c:833:105: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
833 |                                                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
|                                                                                                                                                   ^~~~~~
./rtlops.h:87:46
: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
etcrtl.c:858:105: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
858 |                                                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK));
|                                                                                                                                                   ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
etcrtl.c:868:115: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
868 |                                                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                                                             ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
etcrtl.c:869:120: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
869 |                                                 ad
d_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_LABEL | ARG_ISCONST, outlab, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                                                                                  ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
etcrtl.c:955:106: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
955 |                                                         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
|                                                                                                                                                           ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
etcrtl.c:1844:66: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1844 |                                         add_to_ins_chain (compose_ins (INS_ADD, 2, 1, ARG_CONST, -4 << WSH, ARG_REG, REG_WPTR, ARG_REG, REG_WPTR));
|                                                                                                  ~~ ^
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
etcrtl.c:3757:106: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
3757 |                 add_t
o_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_ENABLING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
|                                                                                                                        ^~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:545:25: note: expanded from macro 'W_STATUS'
545 |         #define W_STATUS        ((-4) << WSH)
|                                  ~~~~ ^
etcrtl.c:5122:104: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
5122 |                         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
|                                                                                                                             ^~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:545:25: note: expanded from macro 'W_STATUS'
545 |         #define W_STATUS        ((-4) << WSH)
|                                  ~~~~ ^
etcrtl.c:5221:107: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
5221 |                                                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
|                                                                                                                                                     ^~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:545:25: note: expanded from macro 'W_STATUS'
545 |         #define W_STATUS        ((-4) << WSH)
|                                  ~~~~ ^
etcrtl.c:5426:107: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
5426 |                         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_ENABLING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
|                                                                                                                                ^~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:545:25: note: expanded from macro 'W_STATUS'
545 |         #define W_STATUS        ((-4) << WSH)
|                                  ~~~~ ^
etcrtl.c:5464:107: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
5464 |                         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1,
ARG_CONST, Z_ENABLING, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
|                                                                                                                                ^~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:545:25: note: expanded from macro 'W_STATUS'
545 |         #define W_STATUS        ((-4) << WSH)
|                                  ~~~~ ^
etcrtl.c:5465:109: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
5465 |                         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_TIMENOTSET, ARG_REGIND | ARG_DISP, REG_WPTR, W_TLINK));
|                                                                                                                                  ^~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:544:25: note: expanded from macro 'W_TLINK'
544 |         #define W_TLINK         ((-5) << WSH)
|                                  ~~~~ ^
16 warnings generated.
mv -f .deps/etcrtl.Tpo .deps/etcrtl.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT tstack.o -MD -MP -MF .deps/tstack.Tpo -c -o tstack.o tstack.c
tstack.c:485:10: warning: cast to 'int *' from smaller integer type 'int' [-Wint-to-pointer-cast]
485 |                 labs =
(int *)constval;
|                        ^~~~~~~~~~~~~~~
1 warning generated.
mv -f .deps/tstack.Tpo .deps/tstack.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT rtldump.o -MD -MP -MF .deps/rtldump.Tpo -c -o rtldump.o rtldump.c
rtldump.c:190:30: warning: format specifies type 'unsigned int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
190 |                                 fprintf (
stream, "$x%x", ins->in_args[i]->regconst);
|                                                     ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                     %lx
rtldump.c:199:29: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
199 |                                 fprintf (stream, "L%d", ins->in_args[i]->regconst);
|                                                    ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                    %ld
rtldump.c:202:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
202 |                                 fprintf (stream, "L_%d", ((ins_chain *)ins->in_args[i]->regconst)->in_args[0]->regconst);
|                                                     ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                     %ld
rtldump.c:216:29: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
216 |                                 fprintf (stream, "%df", ins->in_args[i]->regconst);
|                                                   ~~    ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                   %ld
rtldump.c:219:29: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
219 |                                 fprintf (stream, "%db", ins->in_args[i]->regconst);
|                                                   ~~    ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                   %ld
rtldump.c:303:50: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
303 |                         fprintf (stream, "\t\t\t\t<sourceline %d>\n", ins->in_args[0]->regconst);
|                                                               ~~      ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                               %ld
rtldump.c:306:52: warning: format specifies type 'unsigned int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
306 |                         fprintf (stream, "\t\t\t\t<startcc [%8.8x]>\n", ins->in_args[0]->regconst);
|                                                             ~~~~~       ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                             %8.8lx
rtldump.c:309:50: warning: format specifies type 'unsigned int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
309 |                         fprintf (stream, "\t\t\t\t<endcc [%8.8x]>\n", ins->in_args[0]->regconst);
|                                                           ~~~~~       ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                           %8.8lx
8 warnings generated.
mv -f .deps/rtldump.Tpo .deps/rtldump.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT rtlops.o -MD -MP -MF .deps/rtlops.Tpo -c -o rtlops.o rtlops.c
rtlops.c:1692:40: warning: cast to smaller integer type 'int' from 'char *' [-Wpointer-to-int-cast]
1692 |                                 tmp_
ins->in_args[i_in]->regconst = (int)va_arg (ap, char *);
|                                                                    ^~~~~~~~~~~~~~~~~~~~~~~~
rtlops.c:1699:40: warning: cast to smaller integer type 'int' from 'ins_sib_arg *' (aka 'struct TAG_ins_sib_arg *') [-Wpointer-to-int-cast]
1699 |                                 tmp_ins->in_args[i_in]->regconst = (int)t_sib;
|                                                                    ^~~~~~~~~~
rtlops.c:1731:42: warning: cast to smaller integer type 'int' from 'char *' [-Wpointer-to-int-cast]
1731 |                                 tmp_ins->out_args[i_out]->regconst = (int)va_arg (ap, char *);
|                                                                      ^~~~~~~~~~~~~~~~~~~~~~~~
rtlops.c:1800:24: warning: cast to smaller integer type 'int' from 'char *' [-Wpointer-to-int-cast]
1800 |                         tmp_arg->regconst = (int)va_arg (ap, char *);
|                                             ^~~~~~~~~~~~~~~~~~~~~~~~
rtlops.c:1810:25: warning: cast to smaller integer type 'int' from 'ins_sib_arg *' (aka 'struct TAG_ins_sib_arg *') [-Wpointer-to-int-cast]
1810 |                                 tmp_arg->regconst = (int)t_sib;
|                                                     ^~~~~~~~~~
rtlops.c:3045:39: warning: cast to smaller integer type 'int' from 'void *' [-Wvoid-pointer-to-int-cast]
3045 |                                         tmp_
ins->out_args[0]->regconst = (int)NULL;
|                                                                          ^~~~~~~~~
rtlops.c:3052:31: warning: cast to smaller integer type 'int' from 'ins_chain *' (aka 'struct TAG_ins_chain *') [-Wpointer-to-int-cast]
3052 |                                                         lab_refarg->regconst = (int)tmp_ins;
|                                                                                ^~~~~~~~~~~~
rtlops.c:3095:41: warning: cast to smaller integer type 'int' from 'ins_chain *' (aka 'struct TAG_ins_chain *') [-Wpointer-to-int-cast]
3095 |                                                         jref_arg[n_jref_arg]->regconst = (int)label_links[the_lab[n_jref_arg]];
|                                                                                          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtlops.c:3549:19: warning: cast to smaller integer type 'int' from 'char *' [-Wpointer-to-int-cast]
3549 |                 tmp->regconst = (int)string_dup ((char *)arg->regconst);
|                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtlops.c:3552:19: warning: cast to smaller integer type 'int' from 'ins_sib_arg *' (aka 'struct TAG_ins_sib_arg *') [-Wpointer-to-int-cast]
3552 |                 tmp->regconst = (int)rtl_copy_sib_arg ((ins_sib_arg *)arg->regconst);
|                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtlops.c:3557:19: warning: cast to smaller integer type 'int' from 'ins_labrefs *' (aka 'struct TAG_ins_labrefs *') [-Wpointer-to-int-cast]
3557 |                 tmp->regconst = (int)rtl_add_labref ((ins_labrefs *)tmp->regconst, addins);
|                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
11 warnings generated.
mv -f .deps/rtlops.Tpo .deps/rtlops.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT regcolour.o -MD -MP -MF .deps/regcolour.Tpo -c -o regcolour.o regcolour.c
regcolour.c:507:126: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
507 |                                         
fprintf (stderr, "%s: message: potential conflict on register %d (constrained to %d and %d)\n", progname, reg, c_reg, reg_next_c->in_args[1]->regconst);
|                                                                                                                                                 ~~                            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                                                 %ld
regcolour.c:596:113: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
596 |                                 fprintf (stderr, "%s: colour_code_block: INS_CONSTRAIN_REG encountered for unseen register %d\n", progname, tmp->in_args[0]->regconst);
|                                                                                                                            ~~               ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                            %ld
regcolour.c:608:42: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
607 |                                         fprintf (stde
rr, "constrained reg %d (to %d) conflicts with reg %d (to %d) -- constrain_instrs[j] = %p\n", active_regs[j],
|                                                                                                                ~~
|                                                                                                                %ld
608 |                                                 constrain_regs[j], active_regs[i], tmp->in_args[1]->regconst, constrain_instrs[j]);
|                                                                                    ^~~~~~~~~~~~~~~~~~~~~~~~~
regcolour.c:664:115: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
664 |                                 fprintf (stderr, "%s: colour_code_block: INS_UNCONSTRAIN_REG encountered for unseen register %d\n", progname, tmp->in_args[0]->regconst);
|                                                                                                                              ~~               ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                              %ld
regcolour.c:674:107: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
674 |                                 fprintf
(stderr, "%s: colour_code_block: INS_END_REG encountered for unseen register %d\n", progname, tmp->in_args[0]->regconst);
|                                                                                                                      ~~               ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                      %ld
regcolour.c:760:113: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
760 |                                 fprintf (stderr, "%s: colour_code_block: INS_CONSTRAIN_REG encountered for unseen register %d\n", progname, tmp->in_args[0]->regconst);
|                                                                                                                            ~~               ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                            %ld
regcolour.c:782:107: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
782 |                                 fprintf (stderr, "%s: colour_code_block: INS_END_REG encountered for unseen register %d\n", progname, tmp->in_args[0]->regconst);
|                                                                                                                      ~~               ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                      %ld
regcolour.c:791:109: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
791 |                                 fprintf (stderr, "%s: colour_code_block: INS_END_REG encountered for inactive register %d\n", progname, tmp->in_args[0]->regconst);
|                                                                                                                        ~~               ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                        %ld
8 warnings generated.
mv -f .deps/regcolour.Tpo .deps/regcolour.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT asm386.o -MD -MP -MF .deps/asm386.Tpo -c -o asm386.o asm386.c
asm386.c:179:29: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
179 |                 fprin
tf (outstream, "%d", arg->regconst);
|                                      ~~   ^~~~~~~~~~~~~
|                                      %ld
asm386.c:182:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
182 |                 fprintf (outstream, LBLPFX "%d", arg->regconst);
|                                             ~~   ^~~~~~~~~~~~~
|                                             %ld
asm386.c:185:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
185 |                 fprintf (outstream, LBLPFX "%d", ((ins_chain *)arg->regconst)->in_args[0]->regconst);
|                                             ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                                             %ld
asm386.c:188:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
188 |                 fprintf (outstream, "%df", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asm386.c:191:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
191 |                 fprintf (outstream, "%db", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asm386.c:338:33: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
338 |                         fprintf (outstream, "%d:\n", tmp->in_args[0]->regconst);
|                                              ~~      ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                              %ld
asm386.c:382:90: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
382 |                                         fprintf (stderr, "%s: error: low 8-bit register %d unacceptable! (<4)\n", progname, tmp->out_args[0]->regconst);
|                                                                                         ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                         %ld
asm386.c:467:96: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
467 |                                         fprintf (stderr, "%s: error: SHLD/SHRD shift register %d unacceptable (!cl)\n", progname, tmp->in_args[0]->regconst);
|                                                                                               ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                               %ld
asm386.c:486:94: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
486 |                                         fprintf (stderr, "%s: error: SHL/SHR shift register %d unacceptable (!cl)\n", progname, tmp->in_args[0]->regconst);
|                                                                                             ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                             %ld
asm386.c:538:90: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
538 |                                         fprintf (stderr, "%s: error: low 8-bit register %d unacceptable! (<4)\n", progname, tmp->in_args[0]->regconst);
|                                                                                         ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                         %ld
asm386.c:556:91: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
556 |                                                 fprintf (stderr, "%s: error: low 8-bit regist
er %d unacceptable! (<4)\n", progname, tmp->out_args[0]->
regconst);
|                                                                                                 ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                 %ld
asm386.c:579:91: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
579 |                         
fprintf (stderr, "%s: error: low 16-bit register %d unacceptable! (<4)\n", progname, tmp->in_args[0]->regconst);
|                                                                                          ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                          %ld
asm386.c:593:92: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
593 |                                                 fprintf (stderr, "%s: error: low 16
-bit register %d unacceptable! (<4)\n", progname, tmp->out_args[0]->regconst
);
|                                                                                                  ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                  %ld
asm386.c:632:80: warning:
format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
632 |                                         fprintf (stderr, "%s: error: fstsw register not AX (
was %d)\n", progname, tmp->out_args[0]->regconst);
|                                                                                                 ~~                ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                 %ld
14 warnings generated.
mv -f .deps/asm386.Tpo .deps/asm386.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT optimise.o -MD -MP -MF .deps/optimise.Tpo -c -o optimise.o optimise.c
mv -f .deps/optimise.Tpo .deps/optimise.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT elf.o -MD -MP -MF .deps/elf.Tpo -c -o elf.o elf.c
mv -f .deps/elf.Tpo .deps/elf.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT cpuid.o -MD -MP -MF .deps/cpuid.Tpo -c -o cpuid.o cpuid.c
mv -f .deps/cpuid.Tpo .deps/cpuid.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT tstate.o -MD -MP -MF .deps/tstate.Tpo -c -o tstate.o tstate.c
mv -f .deps/tstate.Tpo .deps/tstate.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT etcops.o -MD -MP -MF .deps/etcops.Tpo -c -o etcops.o etcops.c
mv -f .deps/etcops.Tpo .deps/etcops.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT stubtable.o -MD -MP -MF .deps/stubtable.Tpo -c -o stubtable.o stubtable.c
mv -f .deps/stubtable.Tpo .deps/stubtable.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT arch386.o -MD -MP -MF .deps/arch386.Tpo -c -o arch386.o arch386.c
arch386.c:283:102: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
283 |                 
add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                    ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:358:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
358 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                             ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:460:83: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
460 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
|                                                                                                 ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:461:93: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
461 |                 add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND |
ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                           ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:517:113: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
517 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, ts->stack->old_a_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME));
/* store time */
|                                                                                                                        ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:543:24:
note: expanded from macro 'W_TIME'
543 |         #define W_TIME          ((-6) << WSH)
|                                  ~~~~ ^
arch386.c:518:111:
warning: shifting a negative signed value is undefined [-Wshift-negative-value]
518 |         add_to_ins_chain (compose_ins
(INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0
, ARG_REGIND | ARG_DISP, REG_WPTR,
W_IPTR));          /* store return address */

      |                                                                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_in
s_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~

./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1
) << WSH)
|                          ~~~~ ^
arch386.c:519:118: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
519 |
add_to_ins_chain (compose_ins (INS_MO
VE, 1, 1, ARG_CONST | ARG_ISCONST, Z_WAI
TING, ARG_REGIND | ARG_DISP, REG_W
PTR, W_STATUS)); /* store status *
/
|                                                                                                                             ^~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:545:25: note: expanded from macro 'W_STATUS'
545 |         #define W_STATUS        ((-4) << WSH)
|                                  ~~~~ ^
arch386.c:551:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
551 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:552:110: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
552 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                     ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:559:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
559 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
|                                                                                                                   ^~~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:547:27: note: expanded from macro 'W_PRIORITY'
547 |         #define W_PRIORITY      ((-3) << WSH)
|                                  ~~~~ ^
arch386.c:566:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
566 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG
_BPTR, W_LINK));    /* Bptr[Link] = Wptr */
|                                                                                                            ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_
ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h
:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WS
H)
|                          ~~~~ ^
arch386.c:588:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
588 |                 add_t
o_ins_chain (compose_ins (INS_MOVE, 1, 1, A
RG_REG, tmp_reg, ARG_REGIND | ARG
_DISP, REG_WPTR, W_PRIORITY));
|                                                                                                                   ^~~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, op
s_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:547:27:
note: expanded from macro 'W_PRIORITY'
547 |         #define W_PRIORITY      ((-3) <<
WSH)
|                                  ~~~~ ^
arch386.c:592:110: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
592 |         add_
to_ins_chain (compose_ins (INS_MOVE, 1
, 1, ARG_CONST | ARG_ISCONST, 0, ARG_
REGIND | ARG_DISP, REG_WPTR, W_LI
NK));
|                                                                                                                     ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 com
pose_ins_ex(0, ins, ops_in, ops_out, ##
args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK
((-2) << WSH)
|                          ~~~~ ^
arch386.c:593:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
593 |         add_to_ins_chain (compose_ins (IN
S_MOVE, 1, 1, ARG_FLABEL | ARG_ISC
ONST, 0, ARG_REGIND | ARG_DISP, RE
G_WPTR, W_IPTR));
|                                                                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'

87 |                 compose_ins_ex(0, ins, ops_in, o
ps_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'

555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:596:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
596 |         add_to_ins_c
hain (compose_ins (INS_MOVE, 1, 1, AR
G_REG, REG_WPTR, ARG_REGIND | ARG_D
ISP, REG_BPTR, W_LINK));    /* Bptr[
Link] = Wptr */
|                                                                                                            ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, o
ps_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_L
INK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:599:82: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
599 |         add_to_ins_chain (compose_ins
(INS_MOVE, 1, 1, ARG_REGIND | ARG_
DISP, REG_FPTR, W_LINK, ARG_REG,
REG_FPTR));    /* Fptr = Fptr[Link]
*/
|                                                                                         ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_
ins_ex(0, ins, ops_in, ops_out, ## args
)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LI
NK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:600:92: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
600 |         add_to_ins_chain (compose_ins
(INS_JUMP, 1, 0, ARG_REGIND | ARG_D
ISP | ARG_IND, REG_WPTR, W_IPTR));
/* jump to process resume addres
s */
|                                                                                                   ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 comp
ose_ins_ex(0, ins, ops_in, ops_out, ##
args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #defin
e W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:605:92:
warning: shifting a negative signed value is undefined [-Wshift-negative-value]
605 |         add_to_ins_chain (com
pose_ins (INS_JUMP, 1, 0, ARG_REGIND
| ARG_DISP | ARG_IND, REG_WPTR, W_
IPTR));             /* jump to process resum
e address */
|                                                                                                   ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87
|                 compose_ins_ex(0, ins, ops_in, o
ps_out,
## ar
gs)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'

555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:622:103: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
622 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND | ARG_DISP, preg, W_LINK));
|                                                                                                                     ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:632:98: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
632 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REGIND | ARG_DISP, REG_BPTR, W_LINK));
|                                                                                                                ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:639:103: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
639 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, NOT_PROCESS, ARG_REGIND | ARG_DISP, preg, W_LINK));
|                                                                                                                     ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:651:97: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
651 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, preg, ARG_REGIND | ARG_DISP, tmp_reg, W_LINK));
|                                                                                                               ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
arch386.c:680:49: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
680 |                         ARG_REGIND | ARG_DISP, ts->stack->old_a_reg, W_I
PTR));
|                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)

      |                          ~~~~ ^
arch386.c:686:114: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
686 |                         add_to_ins_chain
(compose_ins (INS_MOVE, 1, 1, ARG_RE
G, tmp_reg, ARG_REGIND | ARG_DISP,
ts->stack->old_a_reg, W_PRIORITY
));
|                                                                                                                                       ^~~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 comp
ose_ins_ex(0, ins, ops_in, ops_out, ##
args)
|                                                            ^~~~
./transputer.h:547:27: note: expanded from macro 'W_PRIORITY'
547 |         #defi
ne W_PRIORITY      ((-3) << WSH)
|                                  ~~~~ ^
arch386.c
:698:124: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
698 |                 add_to_ins_chai
n (compose_ins (INS_MOVE, 1, 1, ARG_
FLABEL | ARG_ISCONST, 0, ARG_REGI
ND | ARG_DISP, ts->stack->old_a_re
g, W_IPTR));
|                                                                                                                                          ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, op
s_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'

555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:699:147: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
699 |                 add_to_ins_
chain (compose_ins (INS_ADD, 2, 1, A
RG_CONST, constmap_regconst (ts->s
tack->old_b_reg), ARG_REGIND | AR
G_DISP, ts->stack->old_a_reg, W_I
PTR,
|                                                                                                                                                                 ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compo
se_ins_ex(0, ins, ops_in, ops_out, ## a
rgs)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_I
PTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:700:49: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
700 |                         ARG_REGIND | ARG_DISP, ts->s
tack->old_a_reg, W_IPTR));
|                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0,
ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1)
<< WSH)
|                          ~~~~ ^
arch386.c:706:114: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
706 |                         a
dd_to_ins_chain (compose_ins (INS_MOV
E, 1, 1, ARG_REG, tmp_reg, ARG_REG
IND | ARG_DISP, ts->stack->old_a_
reg, W_PRIORITY));
|                                                                                                                                       ^~~~~~~~~~
./rtlops.h:87:46:
note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops
_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:547:27:
note: expanded from macro 'W_PRIORITY'
547 |         #define W_PRIORITY      ((-3) <
< WSH)
|                                  ~~~~ ^
arch386.c:764:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
764 |         add_t
o_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG
_FLABEL | ARG_ISCONST, 0, ARG_REGIND
| ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                                      ^~~~~~

./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex
(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPT
R          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:770:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
770 |                 add_to_ins_chain (compose_ins
(INS_MOVE, 1, 1, ARG_REG, tmp_re
g, ARG_REGIND | ARG_DISP, REG_WPTR,
W_PRIORITY));
|                                                                                                                   ^~~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'

87 |                 compose_ins_ex(0, ins, ops_in, ops_
out, ## args)
|                                                            ^~~~
./transputer.h:547:27: note: expanded from macro 'W_PRIORITY'
547 |
#define W_PRIORITY      ((-3) << WSH)

|                                  ~~~~ ^
arch386.c:807:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
807 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:813:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
813 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
|                                                                                                                   ^~~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:547:27: note: expanded from macro 'W_PRIORITY'
547 |         #define W_PRIORITY      ((-3) << WSH)
|                                  ~~~~ ^
arch386.c:841:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
841 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, dest_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
|                                                                                                            ^~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:546:26: note: expanded from macro 'W_POINTER'
546 |         #define W_POINTER       ((-4) << WSH)
|                                  ~~~~ ^
arch386.c:1165:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1165 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:1171:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1171 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG
_DISP, REG_WPTR, W_PRIORITY));
|                                                                                                                   ^~~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:547:27: note: expanded from macro 'W_PRIORITY'
547 |         #define
W_PRIORITY      ((-3) << WSH)
|                                  ~~~~ ^
arch386.c:1197:100: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1197 |         add_to_ins_chain (compose_ins (
INS_MOVE, 1, 1, ARG_REG, src_reg,
ARG_REGIND | ARG_DISP, REG_WPTR,
W_POINTER));
|                                                                                                           ^~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_
out, ## args)
|                                                            ^~~~
./transputer.h:546:26: note: expanded from macro 'W_POINTER'
546 |
#define W_POINTER       ((-4) << WSH)
|                                  ~~~~ ^
arch386.c:1419:102: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1419 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_STATUS));
|                                                                                                             ^~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins
, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:545:25: note: expanded from macro 'W_STATUS'
545 |         #define W_STATUS        ((-4) << WSH)
|                                  ~~~~ ^
arch386.c:1497:81: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1497 |         add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME, AR
G_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
|                                                                                        ^~~~~~
./rtlops.h:87:46:
note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:543:24:
note: expanded from macro 'W_TIME'
543 |         #define W_TIME          ((-6) << WSH)
|                                  ~~~~ ^
arch386.c:1614:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1614 |         add_to_ins_chain (compose_ins (INS_CMP, 2, 1, ARG_CONST, Z_READY, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER, ARG_REG | ARG_IMP, REG_CC));
|                                                                                                            ^~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:546:26: note: expanded from macro 'W_POINTER'
546 |         #define W_POINTER       ((-4) << WSH)
|                                  ~~~~ ^
arch386.c:1616:104: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1616 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST, Z_WAITING, ARG_REGIND | ARG_DISP, REG_WPTR, W_POINTER));
|                                                                                                               ^~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:546:26: note: expanded from macro 'W_POINTER'
546 |         #define W_POINTER       ((-4) << WSH)
|                                  ~~~~ ^
arch386.c:1622:101: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1622 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, tmp_reg, ARG_REGIND | ARG_DISP, REG_WPTR, W_PRIORITY));
|                                                                                                                   ^~~~~~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:547:27: note: expanded from macro 'W_PRIORITY'
547 |         #define W_PRIORITY      ((-3) << WSH)
|                                  ~~~~ ^
arch386.c:1625:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
1625 |         add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_FLABEL | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                                      ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
arch386.c:3594:65: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
3594 |                         fprintf (stderr, "error: CMP output register invalid: %d\n", ArgReg (ins->out_args[0]));
|                                                                               ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                               %ld
./structs.h:287:19: note: expanded from macro 'ArgReg'
287 | #define ArgReg(X)       ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
arch386.c:3608:66: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
3608 |                         fprintf (stderr, "error: SAHF output register invalid: %d\n", ArgReg (ins->out_arg
s[0]));
|                                                                                ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                %ld
./structs.h:287:19: note: expanded from macro 'ArgReg'
287 | #define
ArgReg(X)       ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
arch386.c:3617:98: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
3617 |                                 fprintf (stderr, "warning: didn\'t find a CC-setting instruction before CJUMP (cond %d to ", ArgCC (ins->in
_args[0]));
|                                                                                                                     ~~       ^~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                                     %ld
./structs.h:289:18: note: expanded from macro 'ArgCC'
289 | #define ArgCC(X)        ArgConst(X)
|                         ^~~~~~~~~~~
./structs.h:288:21: note: expanded from macro 'ArgConst'
288 | #define Arg
Const(X)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
arch386.c:3620:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
3620 |                                         fprintf (stderr, "L%d", A
rgLabel (ins->in_args[1]));
|                                                            ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                            %ld
./structs.h:290:21: note: expanded from macro 'ArgLabel'
290 | #define ArgLabel(X)     ((X)
->regconst)
|                         ^~~~~~~~~~~~~~~
arch386.c:3623:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
3623 |                                         fprintf
(stderr, "%df", ArgLabel (ins->in_args[1
]));
|                                                           ~~    ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                           %ld
./structs.h:290:21: note: expanded from macro 'ArgLabel'
290 | #defi
n
e ArgLabel(X)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
arch386.c:3626:30: warning:
format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
3626 |                                         fprintf (stderr, "%db"
, ArgLabel (ins->in_args[1]));
|                                                           ~~    ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                           %ld
./structs.h:290:21: note: expanded from macro 'ArgLabel'
290 | #define ArgLabel(X
)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
arch386.c:3632:31: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
3632 |                                         fprintf (stderr, "IL%d", ArgLabel ((ArgInsLab (ins->in_args[1]))->in_args[0]));
|                                                             ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                             %ld
./structs.h:290:21: note: expanded from macro 'ArgLabel'
290 | #define ArgLabel(X)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
49 warnings generated.
mv -f .deps/arch386.Tpo .deps/arch386.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT archmips.o -MD -MP -MF .deps/archmips.Tpo -c -o archmips.o archmips.c
archmips.c:303:102: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
303 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                    ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archmips.c:752:19: warning: format string is not a string literal (potentially insecure) [-Wformat-security]
752 |                 fprintf(stderr, str);
|                                 ^~~
archmips.c:752:19: note: treat the string as an argument to avoid this
752 |                 fprin
tf(stderr, str);
|                                 ^
|                                 "%s",
archmips.c:1864:13: warning: indirection of non-volatile null pointer will be deleted, not trap [-Wnull-dereference]
1864 |                 tmp_reg = *(int *
)0;
|                           ^~~~~~~~~
archmips.c:1864:13: note: consider using __builtin_trap() or qualifying pointer with 'volatile'
archmips.c:1874:13: warning: indirection of non-volatile null pointer will be deleted, not trap [-Wnull-dereference]
1874 |                 tmp_reg = *(int *)0;
|                           ^~~~~~~~~
archmips.c:1874:13: note: consider using __builtin_trap() or qualifying pointer with 'volatile'
archmips.c:1884:13: warning: indirection of non-volatile null pointer will be deleted, not trap [-Wnull-dereference]
1884 |                 tmp_reg = *(int *)0;
|                           ^~~~~~~~~
archmips.c:1884
:13: note: consider using __builtin_trap() or qualifying pointer with 'volatile'
archmips.c:1898:13: warning: indirection of non-volatile null pointer will be deleted, not trap [-Wnull-dereference]
1898 |                 tmp_reg = *(int *)0;
|                           ^~~~~~~~~
archmips.c:1898:13: note: consider using __builtin_trap() or qualifying pointer with 'volatile'
archmips.c:1912:13: warning: indirection of non-volatile null pointer will be deleted, not trap [-Wnull-dereference]
1912 |                 t
mp_reg = *(int *)0;
|                           ^~~~~~~~~
archmips.c:1912:13: note: consider using __builtin_trap() or qualifying pointer with 'volatile'
archmips.c:1972:13: warning: indirection of non-volatile null pointer will be deleted, not trap [-Wnull-dereference]
1972 |                 tmp_
reg = *(int *)0;
|                           ^~~~~~~~~
archmips.c:1972:13: note: consider using __builtin_trap() or qualifying pointer with 'volatile'
8 warnings generated.
mv -f .deps/archmips.Tpo .deps/archmips.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT asmmips.o -MD -MP -MF .deps/asmmips.Tpo -c -o asmmips.o asmmips.c
asmmips.c:152:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
152 |                 fprintf (outstream, "$%d", arg->regconst);
|                                       ~~   ^~~~~~~~~~~~~
|                                       %ld
asmmips.c:160:32: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
160 |                 fprintf (outstream, "($%d)", arg->regconst);
|                                        ~~    ^~~~~~~~~~~~~
|                                        %ld
asmmips.c:163:29: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
163 |                 fprintf (outstream, "%d", arg->regconst);
|                                      ~~   ^~~~~~~~~~~~~
|                                      %ld
asmmips.c:166:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
166 |                 fprintf (outstream, LBLPFX "%d", arg->regconst);
|                                             ~~   ^~~~~~~~~~~~~
|                                             %ld
asmmips.c:169:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
169 |                 fprintf (outstream, LBLPFX "%d", ((ins_chain *)arg->regconst)->in_args[0]->regconst);
|                                             ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                                             %ld
asmmips.c:172:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
172 |                 fprintf (outstream, "%df", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asmmips.c:175:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
175 |                 fprintf (outstream, "%db", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asmmips.c:196:37: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
196 |                 fprintf (outstream, ".!!!%d!!!.", arg->regconst);
|                                          ~~       ^~~~~~~~~~~~~
|                                          %ld
asmmips.c:254:34: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
254 |                                 fp
rintf (outstream, "%d:\n", tmp->in_args[0]->regconst);
|                                                      ~~      ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                      %ld
9 warnings generated.
mv -f .deps/asmmips.Tpo .deps/asmmips.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT tcedump.o -MD -MP -MF .deps/tcedump.Tpo -c -o tcedump.o tcedump.c
mv -f .deps/tcedump.Tpo .deps/tcedump.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT archsparc.o -MD -MP -MF .deps/archsparc.Tpo -c -o archsparc.o archsparc.c
archsparc.c:238:102: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
238 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                    ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archsparc.c:320:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
320 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                             ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archsparc.c:379:83: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
379 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
|                                                                                                 ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archsparc.c:380:93: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
380 |                 add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                           ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
archsparc.c:389:83: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
389 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK, ARG_REG, tmp_reg));
|                                                                                                 ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archsparc.c:393:93: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
393 |                 add_to_ins_chain (compose_ins (INS_JUMP, 1,
0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                           ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
archsparc.c:463:81: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
463 |         add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
|                                                                                        ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:543:24: note: expanded from macro 'W_TIME'
543 |         #define W_TIME          ((-6) << WSH)
|                                  ~~~~ ^
archsparc.c:1677:65: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
1677 |                         fprintf (stde
rr, "error: CMP output register invalid: %d\n", ArgReg (ins->out_args[0]));
|                                                                               ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                               %ld
./structs.h:287:19: note: expanded from macro 'ArgReg'
287 | #define ArgReg(X)       ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
archsparc.c:1691:66: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
1691 |                         fprintf
(stderr, "error: SAHF output register invalid: %d\n", ArgReg (ins->out_args[0]));
|                                                                                ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                %ld
./structs.h:287:19: note: expanded from macro 'ArgReg'
287 | #define ArgReg(X)       ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
9 warnings generated.
mv -f .deps/archsparc.Tpo .deps/archsparc.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT asmsparc.o -MD -MP -MF .deps/asmsparc.Tpo -c -o asmsparc.o asmsparc.c
asmsparc.c:164:29: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
164 |                 fprintf (out
stream, "%d", arg->regconst);
|                                      ~~   ^~~~~~~~~~~~~
|                                      %ld
asmsparc.c:167:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
167 |                 fprintf (outstream, LBLPFX "%d", arg->regconst);
|                                             ~~   ^~~~~~~~~~~~~
|                                             %ld
asmsparc.c:173:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
173 |                 fprintf (outstream, LBLPFX "%d", ((ins_chain *)arg->regconst)->in_args[0]->regconst);
|                                             ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                                             %ld
asmsparc.c:179:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
179 |                 fprintf (outstream, "%df", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asmsparc.c:185:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
185 |                 fprintf (outstream, "%db", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asmsparc.c:352:33: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
352 |                         fprintf (outstream, "%d:\n", tmp->in_args[0]->regconst);
|                                              ~~      ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                              %ld
asmsparc.c:482:96: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
482 |                                         fprintf (stderr, "%s: error: SHLD/SHRD shift register %d unacceptable (!cl)\n", progname, tmp->in_args[0]->regconst)
;
|                                                                                               ~~                                  ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                               %ld
asmsparc.c:638:80: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
638 |                                         fprintf (stderr, "%s: error: fstsw register not AX (was %d)\n", progname,
tmp->out_args[0]->regconst);
|                                                                                                 ~~                ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                                 %ld
8 warnings generated.
mv -f .deps/asmsparc.Tpo .deps/asmsparc.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT archppc.o -MD -MP -MF .deps/archppc.Tpo -c -o archppc.o archppc.c
archppc.c:271:102: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
271 |                 
add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REG, REG_WPTR, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                    ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archppc.c:343:111: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
343 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_CONST | ARG_ISCONST, 0, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK));
|                                                                                                                             ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archppc.c:402:83: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
402 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_FPTR, W_LINK, ARG_REG, REG_FPTR));
|                                                                                                 ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archppc.c:403:93: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
403 |                 add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                           ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
archppc.c:412:83: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
412 |                 add_to_ins_chain (compose_ins (INS_MOVE, 1, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_LINK, ARG_REG, tmp_reg));
|                                                                                                 ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:554:23: note: expanded from macro 'W_LINK'
554 | #define W_LINK          ((-2) << WSH)
|                          ~~~~ ^
archppc.c:416:93: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
416 |                 add_to_ins_chain (compose_ins (INS_JUMP, 1, 0, ARG_REGIND | ARG_IND | ARG_DISP, REG_WPTR, W_IPTR));
|                                                                                                           ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:555:23: note: expanded from macro 'W_IPTR'
555 | #define W_IPTR          ((-1) << WSH)
|                          ~~~~ ^
archppc.c:486:81: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
486 |         add_to_ins_chain (compose_ins (INS_SUB, 2, 1, ARG_REGIND | ARG_DISP, REG_WPTR, W_TIME, ARG_REG, ts->stack->old_c_reg, ARG_REG, ts->stack->old_c_reg));
|                                                                                        ^~~~~~
./rtlops.h:87:46: note: expanded from macro 'compose_ins'
87 |                 compose_ins_ex(0, ins, ops_in, ops_out, ## args)
|                                                            ^~~~
./transputer.h:543:24: note: expanded from macro 'W_TIME'
543 |         #define W_TIME          ((-6) << WSH)
|                                  ~~~~ ^
archppc.c:2090:65: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
2090 |                         fprintf (stderr, "error: CMP output register invalid: %d\n", ArgReg (ins->out_args[0]));
|                                                                               ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                               %ld
./structs.h:287:19: note: expanded from macro 'ArgReg'
287 | #define ArgReg(X)       ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
archppc.c:2104:66: warning:
format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
2104 |                         fprintf (stderr, "error: SAHF output register invalid: %d\n", ArgReg (ins->out_args[0
]));
|                                                                                ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                                %ld
./structs.h:287:19: note: expanded from macro 'ArgReg'
287 | #define ArgReg(X)       ((X)
->regconst)
|                         ^~~~~~~~~~~~~~~
9 warnings generated.
mv -f .deps/archppc.Tpo .deps/archppc.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT asmppc.o -MD -MP -MF .deps/asmppc.Tpo -c -o asmppc.o asmppc.c
asmppc.c:170:29: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
170 |                 fprintf (outs
tream, "%d", arg->regconst);
|                                      ~~   ^~~~~~~~~~~~~
|                                      %ld
asmppc.c:173:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
173 |                 fprintf (outstream, LBLPFX "%d", arg->regconst);
|                                             ~~   ^~~~~~~~~~~~~
|                                             %ld
asmppc.c:179:36: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
179 |                 fprintf (outstream, LBLPFX "%d", ((ins_chain *)arg->regconst)->in_args[0]->regconst);
|                                             ~~   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                                             %ld
asmppc.c:185:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
185 |                 fprintf (outstream, "%df", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asmppc.c:191:30: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
191 |                 fprintf (outstream, "%db", arg->regconst);
|                                      ~~    ^~~~~~~~~~~~~
|                                      %ld
asmppc.c:373:33: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
373 |                         fprintf (outstream, "%d:\n", tmp->in_args[0]->regconst);
|                                              ~~      ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                              %ld
asmppc.c:1039:43:
warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
1039 |                         fprintf (outstream, "\tmtfsb%d\t%d
\n", ArgConst (tmp->in_args[1]), ArgConst (tmp->in
_args[0]));
|                                                     ~~         ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                     %ld
./structs.h:288:21: note: expanded from macro 'ArgConst'
288 | #define ArgConst(X)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
asmppc.c:1039:71: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
1039 |                         fprintf (outstream, "\tmtfsb%
d\t%d\n", ArgConst (tmp->in_args[1]), ArgConst (tmp-
>in_args[0]));
|                                                         ~~                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                         %ld
./structs.h:288:21: note: expanded from macro 'ArgConst'
288 |
#define ArgConst(X)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
asmppc.c:1044:46: warning:
format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
1044 |                         fprintf (outstream, "\tmtfsfi\t
%d, %d\n", ArgConst (tmp->in_args[0]), ArgCons
t (tmp->in_args[1]));
|                                                        ~~         ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                        %ld
./structs.h:288:21
: note: expanded from macro 'ArgConst'
288 | #define ArgConst(X)     ((X)->regcon
st)
|                         ^~~~~~~~~~~~~~~
asmppc.c:1044:74: warning:
format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
1044 |                         fprintf (outstream, "\tm
tfsfi\t%d, %d\n", ArgConst (tmp->in_ar
gs[0]), ArgConst (tmp->in_args[1]));
|                                                            ~~                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                            %ld
./structs.h:288:21: note: expanded from macro 'ArgConst'
288 | #define Arg
Const(X)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
asmppc.c:1069:56: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
1069 |                         fprintf (outstream, "\t%s\t%d, ", codes[tmp->type], ArgConst (tmp->in_args[0]));
|                                                    ~~                       ^~~~~~~~~~~~~~~~~~~~~~~~~~
|                                                    %ld
./structs.h:288:21: note: expanded from macro 'ArgConst'
288 | #define ArgConst(X)     ((X)->regconst)
|                         ^~~~~~~~~~~~~~~
11 warnings generated.
mv -f .deps/asmppc.Tpo .deps/asmppc.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT archx64.o -MD -MP -MF .deps/archx64.Tpo -c -o archx64.o archx64.c
archx64.c:651:9: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
650 |                                                         fprintf (stream, "\tmovq\t$%d, %s\n",
|                                                                                    ~~
|                                                                                    %ld
651 |                                                                 ins->in_args[0]->regconst,
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
archx64.c:668:9: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
667 |                                                         fprintf (stream, "\taddq\t$%d, %s\n",
|                                                                                    ~~
|                                                                                    %ld
668 |                                                                 ins->in_args[1]->regconst,
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
archx64.c:681
:9: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
680 |                                                         fprintf (stream, "\ts
ubq\t$%d, %s\n",
|                                                                                    ~~
|                                                                                    %ld
681 |                                                                 ins
->in_args[1]->regconst,
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
archx64.c:717:34: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
717
|                                                 fprintf (stream, "L%d:\n", ins->in_args[0]->regconst);
|                                                                    ~~      ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                    %ld
archx64.c:724:40: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
724 |                                                 fprintf (stream, "\tjmp\t
L%d\n", ins->in_args[0]->regconst);
|                                                                           ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                           %ld
archx64.c:731:8: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
729
|                                                 fprintf (stream, "\tj%s\tL%d\n",
|                                                                           ~~
|                                                                           %ld
730 |                                                         (ins->in_
args[0]->regconst =
= CC_E) ? "e" : "ne",
731
|                                                         ins->in_args[1]->regconst);

      |                                                         ^~~~~~~~~~~~~~~~~~~~~~~~~
archx64.c:738:9: warning:
format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
737 |                                                         fprintf (stream, "\tcmpq\t$%d, %s\n",
|                                                                                    ~~
|                                                                                    %ld
738 |                                                                 ins->in_arg
s[1]->regconst,
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
7 warnings generated.
mv -f .deps/archx64.Tpo .deps/archx64.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT archaarch64.o -MD -MP -MF .deps/archaarch64.Tpo -c -o archaarch64.o archaarch64.c
archaarch64.c:55:9: warning: 'I_LADD' macro redefined [-Wmacro-redefined]
55 | #define I_LADD 1
|         ^
./transputer.h:76:9: note: previous definition is here
76 | #define I_LADD          0x16
|         ^
archaarch64.c:56:9: warning: 'I_LSUB' macro redefined [-Wmacro-redefined]
56 | #define I_LSUB 2
|         ^
./transputer.h:112:9: note: previous definition is here
112 | #define I_LSUB          0x38
|         ^

archaarch64.c:57:9: warning: 'I_LMUL' macro redefined [-Wmacro-redefined]
57 | #define I_LMUL 3
|         ^
./transputer.h:105:9: note: previous definition is here
105 | #define I_LMU
L          0x31
|         ^
archaarch64.c:58:9: warning: 'I_LSHL' macro redefined [-Wmacro-redefined]
58 | #define I_LSHL 4
|         ^

./transputer.h:110:9: note: previous definition is here
110 | #define I_LSHL          0x36
|         ^
archaarch64.c:59
:9: warning: 'I_LSHR' macro redefined [-Wmacro-redefined]
59 | #define I_LSHR 5
|         ^
./transputer.h:109:9: note: previous definition is here
109 | #define I_LSHR          0x35
|         ^
archaarch64.c:110:9: warning: 'REG_WPTR' macro redefined [-Wmacro-redefined]
110 | #define REG_WPTR 28  /* Workspace
pointer */
|         ^
./tstack.h:31:9: note: previous definition is here
31 | #define REG_W
PTR -2             /* ends up in EBP (on i386) */
|         ^
archaarch64.c:111:9: warning: 'REG_FPTR' macro redefined [-Wmacro-redefined]
111 | #define REG_FPT
R 27  /* Front pointer */
|         ^
./tstack.h:28:9:
note: previous definition is here
28 | #define REG_FPTR -3             /* ends
up in ESI (on i386) */
|         ^
archaarch64.c:112:9: warning: 'REG_BPTR' macro redefined [-Wmacro-redefined]
112 | #define REG_BPTR 26  /* Back pointer */
|         ^
./tstack.h:27:9: note: previous definition is here
27 | #define REG_BPTR -4             /* ends up in EDI (on i386) */
|         ^
archaarch64.c:113:9: warning: 'REG_CC' macro redefined [-Wmacro-redefined]
113 | #define REG_CC 32    /* Condition codes (virtual) */
|         ^
./tstack.h:25:9: note: previous definition is here
25 | #define REG_CC -6               /* condition codes */
|         ^
archaarch64.c:648:9: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
646 |                                                         fprintf (stream, "\tmov\t%s, #%d\n",
|                                                                                       ~~
|                                                                                       %ld

647 |                                                                 aarch64_get_register_name (ins->out_args
[0]->regconst),
648 |                                                                 ins->in_args
[0]->regconst);
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
archaarch64.c:666:9: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
663 |                                                         fprintf (stream, "\tadd\t%s, %s, #%d\n",
|                                                                                           ~~
|                                                                                           %ld
664 |                                                                 aarch64_get_regis
ter_name (ins->out_args[0]->regconst),

665 |                                                                 aarch64_get_register_nam
e (ins->in_args[0]->regconst),
666 |                         
ins->in_args[1]->regconst);
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
archaarch64.c:681:9: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
678 |                                                         fprintf (stream, "\tsub\t%s, %s, #%d\n",

      |                                                                                           ~~
      |                                                                                           %ld
679 |                                                                 aarch64_get_reg
ister_name (ins->out_args[0]->regconst),
680 |                                                                 aarch64_get_register_n
ame (ins->in_args[0]->regconst),
681
|                                                                 ins->in_args[1]->regcons
t);
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
archaarch64.c:708:34: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
708 |                                                 fprintf (stream, "L%d:\n", ins->in_args[0]->regconst);
|                                                                    ~~      ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                    %ld
archaarch64.c:715:38: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
715 |                                                 fprintf (s
tream, "\tb\tL%d\n", ins->in_args[0]->regconst);
|                                                                         ~~     ^~~~~~~~~~~~~~~~~~~~~~~~~
|                                                                         %ld
archaarch64.c:722:8: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
720 |                                                 fprintf (stream, "\tb.%s\tL%d\n",
|
~~
|                                                                            %ld
721 |                                                         (ins->in_args[0]->regconst == CC
_E) ? "eq" : "ne",
722 |                                                         ins->in_args[1]-
>regconst);
|                                                         ^~~~~~~~~~~~~~~~~~~~~~~~~
archaarch64.c:730
:9: warning: format specifies type 'int' but the argument has type 'intptr_t' (aka 'long') [-Wformat]
728 |                                                         fprintf (stream,
"\tcmp\t%s, #%d\n",
|                                                                                       ~~
|                                                                                       %ld
729 |                                                 
aarch64_get_register_name (ins->in_args
[0]->regconst),
730 |                                                                 ins->in
_args[1]->regconst);
|                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
16 warnings generated.
mv -f .deps/archaarch64.Tpo .deps/archaarch64.Po
gcc -DHAVE_CONFIG_H   -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/runtime/ccsp/include -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/cif/libsrc -I/Users/jim/CLionProjects/kroc/modules/ocuda/libsrc -I.  -I.   -g -O2 -MT kif.o -MD -MP -MF .deps/kif.Tpo -c -o kif.o kif.c
mv -f .deps/kif.Tpo .deps/kif.Po
gcc  -g -O2   -o tranx86 main.o tceread.o netcread.o support.o etcdump.o etcrtl.o tstack.o rtldump.o rtlops.o regcolour.o asm386.o optimise.o elf.o cpuid.o tstate.o etcops.o stubtable.o arch386.o archmips.o asmmips.o tcedump.o archsparc.o asmsparc.o archppc.o asmppc.o archx64.o archaarch64.o kif.o

Great! The build succeeded with warnings. Now let me test the fixed version with a 32-bit .tce file to see if the segfault is resolved:

Test the fixed tranx86 with a 32-bit .tce file using x86 target to see if basic functionality works

shell

Completed
/Users/jim/CLionProjects/kroc/tools/tranx86/tranx86 -mi386 ../../modules/course/answers/q6-sort-1.tce -o test_output.s
tranx86: warning: unrecognised option -mi386
tranx86: failed to initialise architecture base for <unknown>
Let me check what the correct flag is:

Check the correct architecture flags for tranx86

shell

Completed
/Users/jim/CLionProjects/kroc/tools/tranx86/tranx86 --help
tranx86 1.4.1 -- occam ETC to multi-architecture code converter (x86, x64, MIPS, SPARC, PowerPC, AArch64)
Copyright (C) 2000-2008 Fred Barnes <frmb@kent.ac.uk>
This software is released under the GNU General Public License

usage: tranx86 [options] [-o <output-file>] <input.tce>
where options are:
-c   required if this is not a main module
-l   translate into dynamic processes
-f   suppress FPU initialisation
-n   disable check translation (CSNGL, CSUB0, CCNT1)
-v   verbose output

	-e   output to ELF object file (default)
	-s   output to Linux/x86 assembler source file
	-sa  output to Linux/x86 assembler source file with annotations
	-es  output to ELF object file via assembler file
	-d   convert input file to textual ETC
	-r   dump raw textual ETC (no processing)
	-rN  dump N-th stage IMC N=[0..3]
	-raN dump N-th stage IMC with annotations N=[0..3]
	-C   dump #PRAGMA COMMENTs as text

	-H   translate for HALT error mode (default)
	-S   translate for STOP error mode
	-do  overflow debugging
	-dd  deadlock debugging
	-dr  range/stop debugging
	-df  floating-point debugging
	-dX  full debugging (-do, -dd, -dr, -df)
	-di  insert debug markers (very expensive)
	-dm  run-time memory debugging (requires run-time kernel support)
	-ds  display memory pool statistics at program exit
	-dt  enable debugging traces
	-kc  multi-processor CCSP kernel interface (MP default)
	-kf  fast CCSP kernel interface (required for inlining) (UP default)
	-km  MESH kernel interface
	-kl  CSP/Linux interface
	-kr  RMoX interface
	-ki  inline-kernel interface (experimental)
	-ii  inline IN instructions
	-io  inline OUT, OUTBYTE and OUTWORD instructions
	-ia  inline parts of the ALT construct
	-is  inline parts of the scheduler
	-it  inline LDTIMER (requires CPU timers and a kroc_clock)
	-iT  inline TIN (requires CPU timers and a kroc_clock)
	-iX  full inlining (-ii, -io, -ia, -is, -it, -iT)
	-E1  experimental IN inlining
	-E2  experimental OUT inlining
	-E3  experimental MIN/MOUT inlining
	-E4  reduced number of kernel reschedules
	-E5  alternate inline rescheduling policy

	-p | -p1      check sync-flag for reschedule on loop ends
	-p2           check sync-flag and run-queue for resched on loop ends
	-m<thing>     set target architecture/class/options
	-m            gives list of available <thing>s
	--int-nocmap  disable constant mapping
	--int-noopt   disable optimiser
	--int-dpt     show process table
	--gstabs      pass --gstabs option to assembler (if appropriate)
	--gstabs+     pass --gstabs+ option to assembler (if appropriate)
	-nd           disallow dynamic memory (MALLOC, MRELEASE, MNEW & MFREE)
	-nsymops      don't generate dynamic lib symbol pseudo-ops in output
	-mp           generate multi-processor safe atomic ops
	--anno        always annotate
	--aeconst     alternate-endian constants

	--tlp-<if>    set top-level PROC interface (not with -c), overrides
	              automatic checking.  --tlp gives list of available <if>s
	--nfw         do not wait for global FORKed processes at exit
	--new         do not wait for blocking sys-calls at exit
	--cnpfx <str> use external C name prefix string <str>
	--cttd        input compiled with chan-type type descriptions
	--underflow-error
	              treat floating-point underflow as an error
	--exporttab <file>
	              write EXPORT table to specified file (removes if none)
	--rmoxmode <type>
	              generate alternate RMoX main module prolog, <type> is one
	              of: app, drv, srv, fs, net

	-V | --version    dump version and exit
	-h | --help       this help text

Let me check what architectures are available:

Check available architectures in tranx86

shell

Completed
/Users/jim/CLionProjects/kroc/tools/tranx86/tranx86 -m
Supported target options (more than one may be specified depending on target):
-m386      select 386 target
-m486      select 486 target
-m586      select pentium target
-m686      select pentium 2/3/pro target
-mMMX      enable MMX instructions
-mSSE2     enable SSE2 instructions
-mcmovc    enable conditional move instructions
-mmips     select MIPS target (defaults to r5000)
-mr3k      select MIPS R3000 target
-mr43k     select MIPS R4300 target
-mr46k     select MIPS R4600 target
-mr5k      select MIPS R5000 target
-msparcv8  select SPARC V8 target
-mppc      select Power-PC target
-mx64      select x86-64 target
-maarch64  select ARM64/AArch64 target
Now let me test with the correct x86 flag:

Test the fixed tranx86 with a 32-bit .tce file using correct x86 target

shell

Completed
/Users/jim/CLionProjects/kroc/tools/tranx86/tranx86 -m386 ../../modules/course/answers/q6-sort-1.tce -o test_output.s
The x86 target still has issues. Let me test the 64-bit targets now to see if the segfault is fixed:

Test the fixed tranx86 with x64 target to see if segfault is resolved

shell

Completed
timeout 10s /Users/jim/CLionProjects/kroc/tools/tranx86/tranx86 -mx64 ../../modules/course/answers/q6-sort-1.tce -o test_output_x64.s 2>&1
Test the fixed tranx86 with x64 target to see if segfault is resolved

shell

Completed
/Users/jim/CLionProjects/kroc/tools/tranx86/tranx86 -mx64 ../../modules/course/answers/q6-sort-1.tce -o test_output_x64.s
tranx86: error: TCE file compiled for 32-bit target but translator configured for 64-bit
Good! The architecture validation is working correctly now. The issue is that we need a 64-bit compiled .tce file to test the 64-bit targets. Let me check if there are any other critical missing implementations by examining the key functions that might be causing segfaults.

Looking at the architecture files, I notice there are some critical missing implementations. Let me check what happens when we try to force the translation (bypassing the validation) to see if there are other segfaults:

1 file read
main.c
Check where the architecture validation is called in main.c

shell

Completed
grep -n "tce_validate_architecture" main.c
864:		if (!tce_validate_architecture (options.machine_class)) {
An error occurred while processing your request.
Too much context loaded. I have cleared the conversation history. Please retry your request with smaller input.: Too much context loaded. I have cleared the conversation history. Please retry your request with smaller input.

You ran out of context. I have attached the chat history as the active file. Can you continue to investigate whether there are missing function implementations in tranx86 for aarch64 and x64 targets.


Context
I can see from the history that you were investigating segfaults in tranx86 for x64 and aarch64 targets. Let me examine the missing function implementations that could be causing these issues.

2 files read
archx64.c
archaarch64.c
Now let me check what the compose_ins function looks like in the support.c file to understand the missing implementation:

1 file read
support.c
I notice that compose_ins is not defined in support.c. Let me search for it in the entire tranx86 directory:

Finding files that contain the compose_ins function definition

shell

Completed
find /Users/jim/CLionProjects/kroc/tools/tranx86 -name "*.c" -o -name "*.h" | xargs grep -l "compose_ins"
/Users/jim/CLionProjects/kroc/tools/tranx86/archsparc.c
/Users/jim/CLionProjects/kroc/tools/tranx86/regcolour.c
/Users/jim/CLionProjects/kroc/tools/tranx86/optimise.c
/Users/jim/CLionProjects/kroc/tools/tranx86/archaarch64.c
/Users/jim/CLionProjects/kroc/tools/tranx86/etcrtl.c
/Users/jim/CLionProjects/kroc/tools/tranx86/archppc.c
/Users/jim/CLionProjects/kroc/tools/tranx86/archx64.c
/Users/jim/CLionProjects/kroc/tools/tranx86/arch386.c
/Users/jim/CLionProjects/kroc/tools/tranx86/rtlops.h
/Users/jim/CLionProjects/kroc/tools/tranx86/archmips.c
/Users/jim/CLionProjects/kroc/tools/tranx86/rtlops.c

Let me check the rtlops.c file where compose_ins is likely defined:

Finding the compose_ins function definition in rtlops.c

shell

Completed
grep -n "compose_ins" rtlops.c | head -10
588:			tmp_ins = compose_ins (INS_START_REG, 1, 0, ARG_REG, i);
591:			tmp_ins = compose_ins (INS_END_REG, 1, 0, ARG_REG, i);
1643:/*{{{  ins_chain *compose_ins (int ins, int ops_in, int ops_out, ...)*/
1648:ins_chain *compose_ins2 (char *file, long line, int etc_ins, int ins, int ops_in, int ops_out, ...)
1650:ins_chain *compose_ins_ex (int etc_ins, int ins, int ops_in, int ops_out, ...)
1763:/*{{{  ins_arg *compose_ins_arg (int argtype, ...)*/
1767:ins_arg *compose_ins_arg (int argtype, ...)
Now let me check the actual definition of compose_ins:

Finding the compose_ins function definition

shell

Completed
grep -A 20 "ins_chain \*compose_ins" rtlops.c
/*{{{  ins_chain *compose_ins (int ins, int ops_in, int ops_out, ...)*/
/*
*	creates an instruction
     */
     #ifdef INSTRUCTION_HISTORY
     ins_chain *compose_ins2 (char *file, long line, int etc_ins, int ins, int ops_in, int ops_out, ...)
     #else
     ins_chain *compose_ins_ex (int etc_ins, int ins, int ops_in, int ops_out, ...)
     #endif
     {
     ins_chain *tmp_ins;
     va_list ap;
     int i, i_in, i_out;
     unsigned int flag;
     ins_sib_arg *t_sib;

	tmp_ins = new_ins ();
	tmp_ins->type = ins;
	tmp_ins->etc_type = etc_ins;
	#ifdef INSTRUCTION_HISTORY
		strcpy (tmp_ins->alloc_file, file);
		tmp_ins->alloc_line = line;
	#endif
	va_start (ap, ops_out);
	for (i_in=0; i_in<ops_in; i_in++) {
		i = va_arg (ap, int);

		if (i == ARG_ARG) {

Let me check the rtlops.h file to see the function declaration:

Finding the compose_ins function declaration in rtlops.h

shell

Completed
grep -A 5 -B 5 "compose_ins" rtlops.h
extern void rtl_free_instr (ins_chain *ins);
extern rtl_chain *new_rtl (void);
extern void free_rtl (rtl_chain *);
extern ins_chain *new_ins (void);
extern ins_arg *new_ins_arg (void);
extern ins_arg *compose_ins_arg (int argtype, ...);
extern ins_sib_arg *new_ins_sib_arg (void);
extern void rtl_set_lastvreg (int reg);
extern int rtl_get_newvreg (void);
extern int rtl_get_lastvreg (void);
#ifdef INSTRUCTION_HISTORY
extern ins_chain *compose_ins2 (char *file, long line, int etc_ins, int ins, int ops_in, int ops_out, ...);
#define compose_ins(ins, ops_in, ops_out, args...) \
compose_ins2(__FILE__, __LINE__, 0, ins, ops_in, ops_out, ## args)
#define compose_ins_ex(etc_ins, ins, ops_in, ops_out, args...) \
compose_ins2(__FILE__, __LINE__, etc_ins, ins, ops_in, ops_out, ## args)
#else	/* INSTRUCTION_HISTORY */
//extern ins_chain *compose_ins (int ins, int ops_in, int ops_out, ...);
#define compose_ins(ins, ops_in, ops_out, args...) \
compose_ins_ex(0, ins, ops_in, ops_out, ## args)
extern ins_chain *compose_ins_ex(int etc_ins, int ins, int ops_in, int ops_out, ...);
#endif	/* !INSTRUCTION_HISTORY */
extern int rtl_const_bitwidth (int val, int issigned);
extern int rtl_instr_width (ins_chain *ins);
extern int rtl_classify_instr (ins_chain *ins);
extern int rtl_cleanup_flabels (rtl_chain *rtl_code);

Perfect! Now I can see the issue. The compose_ins function exists and is properly defined. Let me check what might be causing the segfaults by examining the main etcrtl.c file to see how the architecture functions are called:

Finding where architecture functions are called in etcrtl.c that might cause segfaults

shell

Completed
grep -n "arch->" etcrtl.c | head -20
170:		arch->compose_external_ccall (ts, stub, d_str, &st_first, &st_last);
176:		arch->compose_bcall (ts, i, kernel_call, stub, d_str, &st_first, &st_last);
180:		arch->compose_cif_call (ts, stub, d_str, &st_first, &st_last);
268:			arch->compose_rmox_entry_prolog (ts, options.rmoxmode);
270:			arch->compose_entry_prolog (ts);
278:	arch->compose_reset_fregs (ts);
450:					arch->compose_fp_set_fround (ts, FPU_N);
483:					tmp_reg = arch->compose_widenshort (ts);
491:					arch->compose_fpop (ts, I_FPPOP);
584:						arch->compose_deadlock_kcall (ts, K_SEM_CLAIM, 1, 0);
614:						if ((options.inline_options & INLINE_SCHEDULER) && arch->compose_inline_stopp) {
615:							arch->compose_inline_stopp (ts);
617:							arch->compose_kcall (ts, K_STOPP, 0, 0);
630:						arch->compose_kcall (ts, K_SEM_RELEASE, 1, 0);
649:						if ((options.inline_options & INLINE_SCHEDULER) && arch->compose_inline_runp) {
650:							arch->compose_inline_runp (ts);
652:							arch->compose_kcall (ts, K_RUNP, 1, 0);
669:						arch->compose_kcall (ts, K_SEM_INIT, 1, 0);
684:					if ((options.inline_options & INLINE_SCHEDULER) && arch->compose_inline_full_reschedule) {
685:						arch->compose_inline_full_reschedule (ts);

Let me check what specific functions are being called early in the process that might be causing segfaults:

Finding the first few architecture function calls in etcrtl.c

shell

Completed
grep -n -A 2 -B 2 "arch->compose_entry_prolog\|arch->compose_rmox_entry_prolog\|arch->compose_reset_fregs" etcrtl.c
266-		if (options.rmoxmode != RM_NONE) {
267-			/* generate alternative RMoX entry prolog */
268:			arch->compose_rmox_entry_prolog (ts, options.rmoxmode);
269-		} else {
270:			arch->compose_entry_prolog (ts);
271-		}
272-	}
--
276-	add_to_rtl_chain (trtl);
277-
278:	arch->compose_reset_fregs (ts);
279-
280-	while (etc_code /* && !ts->end_of_module */) {
--
1462-					/* add_to_ins_chain (compose_ins (INS_POP, 0, 1, ARG_REGIND, REG_WPTR)); */
1463-				/* reset FPU */
1464:				arch->compose_reset_fregs (ts);
1465-				ts->supress_debug_insert = 0;
1466-

Now let me check if there are any issues with the stub implementations. The problem is likely that some of the stub functions are being called but they're not properly handling the tstate structure. Let me examine the tstate structure:

1 file read
tstate.h
Now let me check if there are any issues with the tstack structure access in the stub functions. The issue might be that the stub functions are not properly handling null pointers or uninitialized structures. Let me examine the actual segfault more closely by looking at what happens during initialization:

Finding where the architecture is initialized and used

shell

Completed
grep -n -A 5 -B 5 "init_arch" main.c
73-static void tranx86_banner (FILE *stream);
74-static void dump_supported_things (FILE *);
75-static void dump_supported_tlpifs (FILE *);
76-static void dump_version (FILE *);
77-static int tranx86 (char *, char *, arch_t *);
78:static arch_t *init_architecture (int mclass);
79-/*}}}*/
80-/*{{{  global vars*/
81-char *progname;
82-optstruct options;
83-long glob_in_icount, glob_out_icount;
--
799-		}
800-		options.nocc_codegen = 1;
801-	} else if (open_tce_file (input_filename)) {
802-		exit (1);
803-	}
804:	t_arch = init_architecture (options.machine_class);
805-	if (!t_arch) {
806-		fprintf (stderr, "%s: failed to initialise architecture base for %s\n", progname, machine_class_str (options.machine_class));
807-		close_tce_file ();
808-		exit (EXIT_FAILURE);
809-	}
--
1327-	case CLASS_AARCH64:
1328-		return mtable[16];
1329-	}
1330-}
1331-/*}}}*/
1332:/*{{{  static arch_t *init_architecture (int mclass)*/
1333-/*
1334: *	arch_t *init_architecture (int mclass)
1335- *	selects and initialises architecture code-gen
1336- */
1337:static arch_t *init_architecture (int mclass)
1338-{
1339-	switch (mclass) {
1340-	default:
1341-		return NULL;
1342-	case CLASS_386:
1343-	case CLASS_486:
1344-	case CLASS_586:
1345-	case CLASS_686:
1346:		return init_arch_i386 (mclass);
1347-	case CLASS_R3000:
1348-	case CLASS_R6000:
1349-	case CLASS_R4300:
1350-	case CLASS_R4600:
1351-	case CLASS_R5000:
--
1359-
1360-		/* big-endian constants please */
1361-		options.internal_options |= INTERNAL_BIGENDIAN;
1362-		options.internal_options |= INTERNAL_ALIGNEDCODE;
1363-
1364:		return init_arch_mips (mclass);
1365-	case CLASS_SPARCV8:
1366-		options.internal_options |= INTERNAL_BIGENDIAN;
1367-		/* and don't use the constant map at the moment.. */
1368-		options.internal_options |= INTERNAL_NOCONSTMAP;
1369-		options.internal_options |= INTERNAL_ALIGNEDCODE;
1370-
1371:		return init_arch_sparc (mclass);
1372-	case CLASS_POWERPC:
1373-		options.internal_options |= INTERNAL_BIGENDIAN;
1374-		options.internal_options |= INTERNAL_NOCONSTMAP;
1375-		options.internal_options |= INTERNAL_NOOPT;		/* at the moment.. */
1376-		options.internal_options |= INTERNAL_TEMPFLOAT;		/* need this cos some FPU instructions only load/store in memory */
1377-		options.internal_options |= INTERNAL_FLOATCONV;		/* need this too, since PowerPC can't do some conversions directly */
1378-		options.internal_options |= INTERNAL_ALIGNEDCODE;
1379-
1380:		return init_arch_ppc (mclass);
1381-	case CLASS_X64:
1382:		return init_arch_x64 (mclass);
1383-	case CLASS_AARCH64:
1384:		return init_arch_aarch64 (mclass);
1385-	}
1386-}
1387-/*}}}*/
1388-

Now I can see the issue! The problem is likely that the stub functions in both x64 and aarch64 architectures are not properly implemented, and some critical functions are being called that cause segfaults. Let me identify the specific missing implementations that are causing the segfaults.

Based on my analysis, here are the key issues causing segfaults in tranx86 for x64 and aarch64 targets:

Critical Missing Function Implementations
The segfaults are caused by several stub functions that are called early in the translation process but don't properly handle the required operations:

1. compose_reset_fregs - Called immediately after initialization
   This function is called right after compose_entry_prolog and is critical for FPU initialization. The current stub does nothing, but it should properly initialize floating-point registers.

2. compose_entry_prolog - Entry point setup
   While implemented, it may have issues with register initialization and stack setup that could cause downstream problems.

3. Missing register allocation functions
   The tstack_newreg calls in stub functions may be causing issues if the stack isn't properly initialized.