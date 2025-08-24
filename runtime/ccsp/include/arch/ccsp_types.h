#ifndef _ARCH_CCSP_TYPES_H
#define _ARCH_CCSP_TYPES_H

#ifdef TARGET_CPU_386
#include <i386/ccsp_types.h>
#endif

#ifdef TARGET_CPU_AARCH64
#include <aarch64/ccsp_types.h>
#endif

#ifdef TARGET_CPU_X64
#include <x64/ccsp_types.h>
#endif

#endif /* !_ARCH_CCSP_TYPES_H */
