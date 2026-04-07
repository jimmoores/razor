#ifndef _ARCH_DEADLOCK_H
#define _ARCH_DEADLOCK_H

#ifdef TARGET_CPU_386
#include <i386/deadlock.h>
#endif

#ifdef TARGET_CPU_AARCH64
#include <aarch64/deadlock.h>
#endif

#ifdef TARGET_CPU_X64
#include <x64/deadlock.h>
#endif

#endif /* !_ARCH_DEADLOCK_H */
