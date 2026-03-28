# Summary 
 kroc is an occam compiler and runtime system.  We are in the process of updating it from a legacy 32-bit codebase to a 64-bit codebase.  The 32-bit compiler produced various output for various architectures including SPARC, PPC, MIPS, x86, with x86 being the most important.There is a simulator-based runtime to emulate the transputer on microcontrollers like the Atmel AVR. 

# AArch64 KRoC Runtime Porting Status
## Build environments
I have created multiple environments, each with its own copy of the code, pulled from common git repositories.  The code sits in ~/razor/kroc32/kroc and ~/razor/kroc64/kroc.  This project used to use a separate folder under ~/razor/kroc32/<machine-name> to hold the autoconf configure.sh output as the codebase used to be shared over NFS, but this is no longer necessary as we switched to git for performance reasons.  Now ./configure can be
run in the kroc folder directly on each host system.

Prompts applicable to both the 32-bit and 64-bit code are stored in ~/razor/ai. 
- ai - prompts covering general background applicable to kroc32 and kroc64 snapshots.
- kroc32/kroc for a patched version of the original 32-bit kroc distribution.The distribution originally ran only on debian8 (GCC 4.x), but is now fixed to run on debian9 and later 32-bit x86 targets.These changes were forward ported to the kroc64 version of teh code, but only for x86.
  - kroc - the source code 
- kroc64/ai - prompts and docs for the 64-bit port.
- kroc64/kroc - which is intended to become an updated version of the source code that will run on both 32-bit targets and new 64-bit targets x64 and aarch64. 

## Previous issues fixed
- **Stack Alignment:** Fixed `SIGBUS` crashes by increasing `CIF_STACK_LINKAGE` to 2 words (16 bytes) in `ccsp_consts.h`. This ensures 16-byte 
 stack alignment required by AArch64. 
- **Trampolines:** Implemented `K_CIF_PROC` and `K_CIF_PROC_IND` assembly trampolines in `sched_asm_inserts.h`. These macros correctly switch between the kernel stack and the process workspace stack, passing arguments in `x0`. 
- **Scheduler Pointer:** Corrected the `SchedPtr` loading offset in the trampoline code (`ldr x1, [x28, #-56]`). 
- **Execution:** The `cift15` test now successfully initializes the CCSP runtime, starts the scheduler, launches the main C process (`proc_main`), and spawns light processes.
 
- This directory structure is replicated on multiple servers to allow changes to be tested easily across them.The servers are: 
 - mbp-13 - the host Mac- running macOS 64-bit on Apple M1 (aarch64)
 - debian13-aarch64 - running Debian 13 on Apple M1 (aarch64) 
 - debian13 - running Debian 13 (x64) on Intel
 - debian8 - running an old debian 8 (x86) with GCC 4.x 
 - debian9 - running an old debian 9 (x86) with GCC 6.x 
- Each server can be accessed via password-less ssh and the same directory structure under ~/razor.

As stated previously, syncing source code on each server can be done via its upstream git repository (which is hosted on debian13 serparate from the working code repo on debian13).

## Outstanding Issue: SchedPtr Corruption
- **Symptoms:** `cift15` crashes with `SIGSEGV` at address `0xa` during `ChanOutWord`. 
- **Analysis:** The crash happens because the `sched` pointer (expected in `x1`) is `0x2`. This value is loaded from `Wptr[SchedPtr]` (offset -7). 
- **Root Cause:** `Wptr[-7]` is being corrupted or overwritten with `0x2` (possibly `TimeNotSet_p` or related to `StackPtr` initialization). 
- `LightProcInit` writes stack size (`1032` aka `0x408`) to `ws[-7]` (as `StackPtr`).
- `kernel_X_runp` (patched) overwrites `ws[-7]` with the correct `sched` pointer.
- Sometime after enqueueing and before `ChanOutWord`, `ws[-7]` becomes `0x2`.
- **Suspicion:** Memory corruption, stack variable collision, or subtle interaction between `LightProcInit`/`ProcPar` and the `SchedPtr`/`StackPtr` collision at offset -7. `0x2` is also `TimeNotSet_p`, which might implicate timer logic or structure offsets.

- Each of ~/razor/ai, ~/razor/kroc32/kroc and ~/razor/kroc64/kroc are separate git repositories.The common remote repos for all servers are on debian13 under ~/razor/remotes,So if you need to push/pull changes, push up to the debian13 remotes and pull down from it as required.This goes for the copies on debian13 in the same places as all other servers as well.

So to build kroc64 for aarch64 on Linux you: 
```
ssh debian13-aarch64 
cd ~/razor/kroc64
autoreconf -fi # only if autotools files changed.
make 
```
- There should also been tools like objdump, gdb (suggested with scripts rather than interactive), and so on to be able to track down bugs on specific platforms.

# Structure of the source
The source code uses GNU autotools for configuration.  The system is made up of a legacy Transputer compiler, occ21, that compiles occam source into (among other things) a pseudo-Transputer target called ETC (extended transputer code).These files have a .tce postfix.  There is then a build-time translator module called tranx86 that converts psuedo-transputer instructions into various target architectures, including x86.This output is then linked against the CCSP runtime system using a custom calling convention where 'kernel' arguments are written into variables prior to a call into the runtime system.There is a C interface to CCSP called cif, and this is important in testing the runtime system outside of the compiler. 

The layout of the kroc package is that the compiler and translator, amongst other tools, are under the tools/ sub-directory. The CCSP runtime is under runtime/ and various libraries and modules (including the driver script 'kroc' and the cif c interface library, with examples) is under modules/. 

The patched 32-bit kroc system can be found in ~/razor/kroc32/kroc, and the new 64-bit kroc system can be found in ~/razor/kroc64/kroc under each you can find the tools/, runtime/ and modules/ directories described previously.

# What has been done 
- Some work has been done on getting 64-bit output from occ21, in the form of a 64-bit version of the .tce files (ETC code).There has also been work on adding 64-bit support for the CCSP run-time, focussed initially on aarch64 target on macOS. 

The stumbling block has been getting symbol generation for each architecture to work consistently.macOS requires an extra underscore prefix that has caused an enormous number of linker issues.There are also a number of ways symbols get transformed as they flow from occ21 through tranx86, including mapping of calls to underlying C functions, blocking functions, mapping of dots to underscores, etc.We are really just wanting these mappings to be consistent with the older 32-bit compiler.

More information on the symbol naming issue can be found in
 [kroc64/kroc/.junie/symbol-naming-summary.md](kroc64/kroc/.junie/symbol-naming-summary.md).
There is a general information page under [kroc64/overview.md](kroc64/overview.md).
There is information about how to run tests here [kroc64/testing-procedures.md](kroc64/testing-procedures.md). 

# Some pointers. 
- You should not modify the build system lightly, and given it's complexity, you should take care to regenerate from the autotools configuration rather than hack around with Makefiles and m4.Regeneration can be done of course using autoreconf (probably with -fi options) but then the configure script should be run in each servers kroc directory.Currently if you build on the local machine mbp-13, aarch64 is not complete. 

There are some existing issues where distclean doesn't recurse down to lower directories, esp. those libsrc/ directories in modules/ and tests/.It would be nice to fix this at some point. 

Make sure if you use gdb you use a script, as you will otherwise often get blocked waiting for interactive feedback.Make sure you build on the correct machine (not the Mac) and make sure you execute on the correct machine.Don't forget the full filesystem is identical and shared between all machines. 

# The task 
Our first task is to get a clean build on all architectures and then test the minimal runtime test executables in modules/cif/examlples like cift1. Note that the more complete example, cift-commstime, will not self-terminate, so will need to be interrupted after a few seconds.Use `timeout -k 1 5 cif-commstime` as a HUP signal alone will not end the task.Note that even thought the cif samples are written in C and compiled against just CCSP, there are a some occam generated processes in the kroc runtime that handles stdin, stdout, stderr that do require a minimal working occam compiler output.
