#ifndef _ARCH_ATOMICS_H
#define _ARCH_ATOMICS_H

#ifdef TARGET_CPU_386
#include <i386/atomics.h>
#endif

#ifdef TARGET_CPU_AARCH64
#include <aarch64/atomics.h>
#endif

#endif	/* !_ARCH_ATOMICS_H */
