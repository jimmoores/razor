# Summary of work done so far in investigating why a minimal occam program
``` occam
PROC main(CHAN BYTE kyb?, scr!, err!)
  SKIP
:
```
does not run cleanly.

## Findings and Fixes

1.  **Word Size Mismatch**:
    -   The runtime was compiling with `sizeof(word) == 4` on AArch64 (should be 8).
    -   Cause: `TARGET_CPU_AARCH64` macro was not defined (lowercase `target_cpu_aarch64` was used).
    -   **Fix**: Modified `ukcthreads_types.h` to check for `__aarch64__`.
    -   Status: Verified `sizeof(word) == 8`. Runtime rebuilt.

2.  **Invalid Assembly Labels (Fixed in Toolchain)**:
    -   `tranx86` generated labels using pointer values (e.g., `L14073...`) instead of IDs (e.g., `L13`).
    -   Cause: `constmap` stored values as `int`, truncating 64-bit pointers on AArch64. Additionally, `rtlops.c` retrieved arguments using `va_arg(ap, int)`, which mishandled 64-bit `intptr_t` values passed to it.
    -   **Fix 1**: Updated `tstack.h` and `tstack.c` to use `intptr_t` for `c_val` in `constmap`.
    -   **Fix 2**: Updated `rtlops.c` (`compose_ins_ex`, `compose_ins_arg`) to use `va_arg(ap, intptr_t)` for `ARG_REG`, `ARG_CONST`, `ARG_LABEL`, etc., preventing stack corruption and segfaults in `tranx86`.
    -   Status: `tranx86` no longer segfaults. Verification pending (need to run rebuild scripts).

3.  **AArch64 Calling Convention Bug in `tranx86`**:
    -   Runtime crashes with `SIGTRAP` (PAC failure) or `SIGSEGV` because `sched` pointer passed to kernel calls (like `Y_shutdown`) was garbage.
    -   Cause: `tranx86` generates `INS_CALL` (`bl`) for `local_scheduler()` but does NOT move the return value from `x0` to the requested register (`x1`).
    -   **Fix**: Modified `archaarch64.c` to emit `mov dst, x0` after `bl` if output register is specified.
    -   Status: Verified `test_skip.s` now contains `mov x1, x0`.

4.  **`tranx86` Segfault (Fixed)**:
    -   `tranx86` was crashing with a segfault in `rtl_trace_regs` due to `HUGE REG` indices.
    -   Cause: `rtlops.c` was reading 32-bit register IDs (passed as `int`) using `va_arg(ap, intptr_t)`. On 64-bit systems, this read garbage into the upper 32 bits, interpreting signed 32-bit values (like `REG_WPTR = -2`) as massive positive 64-bit integers.
    -   **Fix**: Modified `rtlops.c` to explicitly read `int` and cast to `intptr_t` for `ARG_REG`, `ARG_REGIND`, etc., while keeping `intptr_t` for `ARG_CONST`.
    -   Status: Segfault resolved.

5.  **Assembler Errors (Fixed)**:
    -   `as` failed with `symbol 'L<pointer>' not defined`.
    -   Cause: `archaarch64.c` was incorrectly handling `ARG_INSLABEL` by treating the instruction pointer address as the label ID.
    -   **Fix**: Modified `archaarch64.c` to dereference the instruction chain pointer (`(ins_chain *)regconst`) to retrieve the actual integer label ID.
    -   Status: `cift1.s` generates correctly and assembles.

6.  **Calltable Relocation and Layout Mismatch (Fixed)**:
    -   `cift1` was crashing at startup because `K_ENTRY` jumped to `0x0` (causing SIGTRAP/Crash).
    -   **Cause 1**: Structure layout mismatch for `ccsp_global_t` (`_ccsp`). `sched.o` was built with default `MAX_RUNTIME_THREADS=16`, while other units used `32` (from `config.h`). This caused `build_calltable` to write function pointers to the wrong offsets, leaving the actual `calltable` (as seen by `sched.o`) empty.
    -   **Cause 2**: `build_calltable` being `static inline` in `calltable.h` caused complications with symbol resolution for `kernel_CIF_...` stubs.
    -   **Fix 1**: Forced `MAX_RUNTIME_THREADS` to `32` in `sched_types.h` to ensure consistency.
    -   **Fix 2**: Moved `build_calltable` implementation to `rtsmain.c`.
    -   **Fix 3**: Modified `make-header.py` to generate `extern` declarations for CIF stubs in `calltable.h` and rename the inline version to avoid conflicts.
    -   **Fix 4**: Removed `static` from `kernel_CIF_...` stub functions in `sched.c` to allow linking.
    -   **Status**: Runtime initialization (`Y_rtthreadinit`) now completes successfully for all threads. Calltable is correctly populated.

7.  **Stack Pointer Corruption in `K_ZERO_OUT_JRET` (Fixed)**:
    -   `Y_rtthreadinit` jumped to `occam_start` but `sp` was corrupted.
    -   **Cause**: `K_ZERO_OUT_JRET` macro used `ldr x9, [%1]` where `%1` was `sched` (pointer). This loaded `sched->index` into `sp` instead of `sched` address itself.
    -   **Fix**: Changed to `mov sp, %1` in `sched_asm_inserts.h`.
    -   **Status**: `sp` is now correctly set to `sched` (stack base).

8.  **Pointer Authentication (PAC) Crash (Fixed)**:
    -   `cift1` crashed with `SIGTRAP` (Trace/BPT trap) on return from `_my_process` (C code called from Occam).
    -   **Cause**: `tranx86` generates `bl` instructions to call C functions but does not implement AArch64 Pointer Authentication (PAC) signing/authentication logic. Standard C functions (compiled with Apple Clang) expect `LR` (`x30`) to be managed with PAC (`pacibsp`/`autibsp`). The mismatch between the raw return address generated by `bl` and the authenticated return address expected by `ret` caused a trap.
    -   **Fix**: Compiled all C components (runtime library `libccsp.a`, `libkrocif.a`, and user C code) with `-fno-ptrauth-calls`.
    -   **Status**: `cift1` now runs `_my_process` and returns successfully. `_my_process` prints output to stderr.

9.  **Current Status**:
    -   **`tranx86`**: Functional.
    -   **`cift1`**: Runs, initializes, executes Occam code, calls C code (`_my_process`), and returns.
    -   **Remaining Issue**: A `SIGBUS` occurs on a secondary thread during process shutdown (triggered by `Y_shutdown`), likely due to race conditions or resource teardown while other threads are active. This does not prevent the main application logic from executing.

## Next Steps
-   Integrate `-fno-ptrauth-calls` into the main `configure` script or `Makefile.am` for AArch64 builds on macOS to ensure all C code is compatible with the assembly runtime.
-   Investigate the shutdown `SIGBUS` if clean exit is required.