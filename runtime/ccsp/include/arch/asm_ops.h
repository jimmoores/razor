#ifndef _ARCH_ASM_OPS_H
#define _ARCH_ASM_OPS_H

#ifdef TARGET_CPU_386
#include <i386/asm_ops.h>
#endif

#ifdef TARGET_CPU_AARCH64
#include <aarch64/asm_ops.h>
#endif

#ifdef TARGET_CPU_X64
#include <x64/asm_ops.h>
#endif

#endif /* !_ARCH_ASM_OPS_H */
