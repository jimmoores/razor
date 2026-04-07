# x64 Port Implementation Status

## Overview
Complete implementation of x64 (Intel/AMD 64-bit) support for tranx86 code generator and CCSP runtime.

## Current State (2026-04-07)
The x64 port is architecturally complete. The full build compiles through the standard libraries
(inmoslibs, bsclib, course, useful, trap) and only fails at optional SDL bindings (pre-existing
64-bit type issues). A SKIP test program compiles, assembles, links, and runs but crashes during
runtime initialization at the MAGIC SCREEN / kernel call sequence.

### Build Progress
- [x] tranx86 compiles as native 64-bit binary
- [x] CCSP runtime compiles for x64
- [x] CIF stubs compile for x64
- [x] libkrocif builds (occam I/O processes + C runtime)
- [x] inmoslibs (forall, convert, maths) build
- [x] bsclib (filelib, socklib, httplib) build
- [x] course, useful, time, cif, selector, trap modules build
- [x] x64_cif.S assembly support created and linked
- [ ] occSDL fails (pre-existing 64-bit type issues, not x64 port related)

### Runtime Status
- [x] SKIP program compiles to .tce (occ21 -X64)
- [x] SKIP program translates to .s (tranx86 -mx64)
- [x] SKIP program assembles to .o (gas)
- [x] SKIP program links against CCSP
- [~] SKIP program runs but crashes (SIGSEGV in JENTRY/MAGIC SCREEN sequence)

### Root Cause of Current Crash
The entry point code stores the continuation label at `Wptr[0]` but `I_RET`
reads from `new_Wptr[-1]` after adjusting Wptr by 4 words. This is the
same class of issue that was debugged for aarch64 (see ai/skiptest.md item 6
about I_CALL/I_RET frame alignment). The `ws_adjust` and `JENTRY` handling
in `etcrtl.c` needs to match the x64 calling convention.

## Files Modified (16 files)
- `tools/tranx86/archx64.c` - Complete rewrite (3300+ lines)
- `tools/tranx86/configure.ac` - x64 native build, HOST_CPU_IS_X64
- `runtime/ccsp/include/x64/sched_asm_inserts.h` - Complete rewrite
- `runtime/ccsp/include/x64/asm_ops.h` - 64-bit updates
- `runtime/ccsp/include/x64/timer.h` - 64-bit rdtsc
- `runtime/ccsp/include/x64/deadlock.h` - 64-bit callq
- `runtime/ccsp/include/arch/*.h` - 6 dispatcher headers with TARGET_CPU_X64
- `runtime/ccsp/kernel/deadlock.c` - TARGET_CPU_X64 case
- `runtime/ccsp/kernel/x64_cif.S` - NEW: x64 CIF assembly support
- `runtime/ccsp/kernel/Makefile.am` - x64_cif.S inclusion
- `runtime/ccsp/Makefile.am` - x64_cif.o in libccsp
- `runtime/ccsp/utils/make-header.py` - x64 CIF stub generator
- `m4/kroc.m4` - -fPIC for x64 shared objects
- `ai/x64-port-plan.md` - Register convention and plan
- `ai/x64-port-status.md` - This file

## Key Design Decisions
- WPTR: r14 (callee-saved)
- FPTR: r13 (callee-saved)
- BPTR: r12 (callee-saved)
- SCHED: r15 (callee-saved)
- 10 allocatable registers (rax,rcx,rdx,rbx,rsi,rdi,r8-r11)
- System V AMD64 ABI for kernel calls (param0→rdi, sched→rsi, Wptr→rdx)
- x87 FPU for floating point (matches RTL infrastructure)
- RIP-relative addressing for PIC compatibility
- @PLT for external function calls, @GOTPCREL for external symbol addresses
- Entry point symbol: _occam_start (matching C extern declaration)
