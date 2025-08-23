#ifndef _ARCH_SCHED_ASM_INSERTS_H
#define _ARCH_SCHED_ASM_INSERTS_H

#ifdef TARGET_CPU_386
#include <i386/sched_asm_inserts.h>
#endif

#ifdef TARGET_CPU_AARCH64
#include <aarch64/sched_asm_inserts.h>
#endif

#endif /* !_ARCH_SCHED_ASM_INSERTS_H */
