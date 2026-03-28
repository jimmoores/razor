# CCSP Runtime System Analysis

## Overview
CCSP (Concurrency Control System for Parallelism) is the runtime kernel for KRoC. It provides the scheduling, memory management, and channel communication mechanisms required by the occam-pi language. It is designed as a hybrid C/Assembly kernel, where the core scheduler and lightweight processes are managed in C with architecture-specific assembly inserts for context switching and atomic operations.

## Directory Structure
- **`common/`**: Architecture-independent implementations (allocators, user-process wrapping, tracing).
- **`kernel/`**: The core kernel logic (`sched.c`), entry points (`entry.c`), blocking system calls (`bsyscalls.c`), and deadlock detection (`deadlock.c`).
- **`include/`**: Public and private headers.
    - **`arch/`**: Symlinks to the selected architecture's headers.
    - **`aarch64/`, `i386/`, `x64/`**: Architecture-specific headers.

## Calling Conventions & Architecture Interface

### Generic Kernel Call Interface
The kernel interacts with compiled code via a "Call Table" (`ccsp_calltable`). Compiled code loads the address of a kernel routine from this table and branches to it.

The generic signature for a kernel C function is:
```c
void kernel_Name (word param0, sched_t *sched, word *Wptr);
```
- **`param0`**: The first parameter passed from the caller (often data or a count).
- **`sched`**: Pointer to the current scheduler structure (often passed in a register).
- **`Wptr`**: Pointer to the current process workspace.

### AArch64 Implementation
On AArch64, the calling convention typically maps to:
- **`x0`**: `param0`
- **`x1`**: `sched` (Scheduler Pointer)
- **`x2`**: `Wptr` (Workspace Pointer)
- **`x28`**: Conventionally used for `Wptr` in compiled code.
- **`x25`**: Conventionally used for `SchedPtr` in compiled code.

**Stack**: The CCSP kernel runs on its own stack, distinct from the user process stacks. `K_ENTRY` switches the stack pointer (`sp`) to the kernel stack before calling C functions.

## Initialization & Start-up Flow

1.  **`main()` (C entry)**: Located in `common/rtsmain.c` (or provided by user). Calls `ccsp_init()`.
2.  **`ccsp_init()`**:
    *   Initializes dynamic memory (`dmem_init`).
    *   Calibrates timers (`ccsp_calibrate_timers`).
    *   Initializes kernel data (`ccsp_kernel_init`).
    *   Initializes user process environment (`ccsp_user_process_init`).
3.  **`ccsp_kernel_init()`** (`kernel/sched.c`):
    *   Sets up the global `_ccsp` structure.
    *   Initializes the `boot_scheduler`.
    *   Builds the call table (`build_calltable`).
    *   **Note**: Uses `dmem_new_allocator` which may return `NULL` if `ALLOC_MALLOC` is defined (default for portability).
4.  **`user_process()`** (`common/userproc.c`):
    *   Sets up signal handling and `sigsetjmp` for recovery/exit.
    *   Calls `_occ_enter()`.
5.  **`_occ_enter()`** (`libkrocif/rts/occam_entry.c`):
    *   Allocates the initial workspace for the root occam process.
    *   Calls `ccsp_occam_entry()`.
6.  **`ccsp_occam_entry()`** (`kernel/entry.c`):
    *   Sets up the workspace with `Iptr` (Instruction Pointer) and `SchedPtr`.
    *   Calls `ccsp_kernel_entry()`.
7.  **`ccsp_kernel_entry()`** (`kernel/entry.c`):
    *   Calculates the initial kernel stack address.
    *   **Crucial Transition**: Switches to the kernel stack and calls `kernel_Y_rtthreadinit` via `K_ENTRY`.
8.  **`kernel_Y_rtthreadinit()`** (`kernel/sched.c`):
    *   The "real" kernel startup.
    *   Initializes the scheduler structure passed on the stack.
    *   Enqueues the initial process (`Fptr`).
    *   Starts the scheduling loop (`kernel_scheduler`).

## 64-bit Porting Risks & Issues

### 1. Pointer Truncation in `dmem.c`
The block allocator in `common/dmem.c` is fundamentally broken for 64-bit systems where the heap extends beyond 4GB.
```c
static inline unsigned int addr_to_slabid (const void *ptr) {
    // ...
    return SLABV (slab_largeslabs[(unsigned int)ptr >> LARGE_SLAB_SHIFT]);
}
```
The cast `(unsigned int)ptr` truncates the 64-bit pointer to 32 bits. This logic assumes a 32-bit address space (`ADDRSPACE_SHIFT 32`).
*   **Mitigation**: The default configuration usually defines `ALLOC_MALLOC`, which bypasses this allocator in favor of system `malloc`. However, if `ALLOC_BLOCK` or `ALLOC_SBLOCK` were enabled, this would cause immediate corruption or faults.

### 2. Structure Packing (`_PACK_STRUCT`)
Headers like `sched_types.h` use `__attribute__ ((packed))` for key structures (`sched_t`, `ccsp_global_t`).
*   **Risk**: On AArch64, atomic operations (like `ldxr`/`stxr` used in `atomics.h`) **require** natural alignment. If `_PACK_STRUCT` forces fields to be unaligned, atomic operations on them will raise `SIGBUS` (Alignment Fault).
*   **Specific Risk**: `sched_t` contains `atomic_t` fields (e.g., `rqstate`, `sync`). If previous fields cause these to be misaligned, the kernel will crash.
*   **Example**: `pad1` in `sched_t` is `CACHELINE_ALIGN`, but if the struct is packed, the compiler might strip inter-field padding that guarantees alignment of subsequent fields.

### 3. Dynamic Process Pointers (`dynproc.c`)
The dynamic process loading code (`kernel/dynproc.c`) contains numerous casts from pointers to `int`.
*   Example: `(int)tp->ws_base`
*   **Consequence**: This code is effectively non-functional on 64-bit systems. It truncates pointers, making dynamic process loading impossible without significant refactoring.

### 4. `tranx86` Generated Code
The `tranx86` tool must generate assembly that respects the 64-bit calling convention (passing 64-bit pointers in `x` registers, not `w` registers) and stack alignment (16-byte alignment for AArch64). Misalignment of the stack pointer `sp` is a common cause of faults on AArch64.

## Kernel Entry Points (Key Selection)

*   **`K_RTTHREADINIT`**: Initializes a runtime thread (scheduler).
*   **`K_SHUTDOWN`**: Signals the kernel to terminate.
*   **`K_PAUSE`**: Voluntarily yields the processor (reschedule).
*   **`K_ALT` / `K_ALTWT` / `K_ALTEND`**: Implements the Alternation construct (waiting on multiple channels).
*   **`K_IN` / `K_OUT`**: Channel communication.
*   **`K_STARTP` / `K_ENDP`**: Process creation and termination.

## Debugging Faults
If the runtime faults during startup:
1.  **Alignment**: Check if `K_ENTRY` sets `sp` to a 16-byte aligned address.
2.  **Atomicity**: Check if `sched_t` fields accessed via `att_...` macros are naturally aligned.
3.  **Calling Convention**: Ensure `tranx86` puts arguments in `x0, x1, x2` correctly for the kernel function signature.
4.  **Pointer Truncation**: Ensure no `(int)` casts are truncating pointers in the path.

## Fix Task List

- [x] **Fix Pointer Truncation in `common/dmem.c`**: Update `addr_to_slabid` and other functions to use `uintptr_t` or `unsigned long` instead of `unsigned int` when casting pointers.
- [x] **Fix Structure Packing in `include/sched_types.h`**: Remove `_PACK_STRUCT` from `sched_t` and related structures, or ensure manual padding maintains alignment for atomic fields on 64-bit architectures. (Verified: `_PACK_STRUCT` is defined as empty for AArch64/GCC, avoiding packing).
- [x] **Fix Dynamic Process Loading in `kernel/dynproc.c`**: Replace `(int)` casts with `(intptr_t)` or `(word)` when handling pointers.
- [x] **Fix `not_on_any_queue`**: Updated signature in `kernel/sched.c` and `include/kernel.h` to use `word` instead of `unsigned int`.
- [x] **Verify/Fix AArch64 Stack Alignment**: Check `include/aarch64/sched_asm_inserts.h` (or equivalent) to ensure `K_ENTRY` enforces 16-byte stack alignment. (Verified: `kernel/entry.c` enforces 16-byte alignment before calling `K_ENTRY`).
- [x] **Verify AArch64 Calling Convention**: Ensure `tranx86` and `make-header.py` (for CIF stubs) use 64-bit registers (`x0`...) for parameters. (Verified & Fixed: Updated `make-header.py` `ccsp_cif_occam_call` to use operands instead of hardcoded registers).

## Current Status (Progress & Next Steps)

**Goal:** Fix the KRoC AArch64 backend (`tranx86` compiler and CCSP runtime) so that the `skip.occ` and `test_hello.occ` tests build and execute successfully without crashing.

**Status:** **SUCCESS**. The segmentation faults, hangs, and underlying architectural issues in the AArch64 KRoC port have been successfully resolved. The `test_hello.occ` program now executes perfectly, prints "Hello, World!", and terminates cleanly with exit code 0.

**Critical Fixes Implemented:**
1. **64-bit System Alignment:** Fixed the definition of `word` in the CCSP runtime headers (`ukcthreads_types.h`, `sched_types.h`) to be a true 64-bit integer on AArch64. Without this, pointers stored in the `sched_t` structure were being truncated to 32 bits, causing wild branches.
2. **Pointer Truncation in Scheduler Initialization:** In `kernel_Y_rtthreadinit`, the `stack` parameter was originally defined as an `unsigned int`, truncating the 64-bit C stack pointer and causing `init_sched_t` to write to corrupted memory. Changed to `word`.
3. **64-Byte Cacheline Alignment:** The `sched_t` structure requires strict 64-byte alignment due to `CACHELINE_ALIGN` fields. The `stack_buffer` in `entry.c` was updated from 16-byte to 64-byte alignment, preventing `EXC_BAD_ACCESS` during SIMD zeroing operations.
4. **Safe Stack Initialization & Frame Pointer Protection:** Fixed the `ccsp_kernel_entry` logic to safely allocate an aligned stack buffer, preventing OS guard page violations. Removed clobbering of the `x29` frame pointer in the `K_ZERO_OUT_JRET` macro.
5. **Comparison Semantics (`cmp`):** Fixed `INS_CMP` in `archaarch64.c`. Translated x86 AT&T syntax `cmp src, dest` (which computes `dest - src`) correctly to AArch64 `cmp Xn, Xm` (`Xn - Xm`). Evaluated operands in the correct order to fix reversed conditional jumps (`b.lo`) during range checks.
6. **Correct Stack Frame Allocation for `I_CALL` / `I_RET`:** AArch64 `I_CALL` allocates 32 bytes (`4 * WSH`). Fixed `compose_aarch64_return` (`I_RET`) to correctly pop 32 bytes and read the return address from `Wptr[-32]`. Added the missing 32-byte allocation to `compose_aarch64_entry_prolog` to simulate an `I_CALL` frame, preventing the `JENTRY` Magic Screen block from corrupting the Wptr.
7. **Magic Screen Parameter Resolution:** Because `AJW -2` allocates 16 bytes at the start of the `L15` Magic Screen block, `Wptr` drops by 2 words. Adjusted `etcrtl.c` to read `(scr_offset + 2) << WSH`, correctly locating the `scr` channel pointer.
8. **Calltable Offset Resolution:** Replaced a manually mocked `sched_t` struct in `archaarch64.c` with the actual `ccsp_sched_t` from `ccsp.h`. This fixed incorrect `offsetof` calculations that caused kernel calls (like `K_SHUTDOWN` and `K_BRANGERR`) to jump to garbage memory addresses.
9. **Return Address Overwriting:** Disabled the `save_return` C function in the CCSP kernel for AArch64. It was incorrectly fetching `__builtin_return_address(0)` (the inline C call's LR) and overwriting `W_IPTR`, destroying the true Occam resumption label explicitly saved by `tranx86`.
10. **Kernel Exit Gracefulness:** Modified `ccsp_kernel_exit` to safely call `exit(cause)` directly for AArch64, preventing the process from falling off the end of a `NO_RETURN` function and crashing.
11. **Assembly Generation Rules (AArch64):** Patched `INS_MOVE` and `INS_MOVEB` for `ARG_CONST` literals, fixed `ARG_FLABEL` and `ARG_LABEL` secure resolution using `adr`/`adrp`, and corrected `strb` syntax to use `w` registers.
12. **Top-Level Parameter Alignment:** Set default `ws_adjust` to 0 in `tstate.c`. This ensures the root Occam process correctly identifies its channel parameters (like `scr`) and prevents the "MAGIC SCREEN" shutdown block from loading the return address as a channel pointer.
13. **Byte Extension & Logical Consistency:** Implemented `INS_MOVEZEXT8TO32`, `INS_MOVEZEXT16TO32`, and `INS_MOVESEXT16TO32` in `archaarch64.c` using `ldrb`, `ldrh`, and `ldrsh`. This fixed broken `CASE` statements in the I/O runtime where byte comparisons were failing due to missing zero-extension.
14. **Register Aliasing for Byte Ops:** Introduced `aarch64_get_w_register_name` to ensure `ldrb`/`strb` instructions always use the correct 32-bit `w` register aliases, preventing assembly errors or incorrect 64-bit register usage.
15. **Flag-Setting Arithmetic for Control Flow:** Updated `INS_ADD`, `INS_SUB`, `INS_INC`, and `INS_DEC` in `archaarch64.c` to use the `s` suffix (`adds`, `subs`) when the instruction is marked as setting condition flags (i.e., `REG_CC` is in the output arguments). This fixed a critical bug where loops (like `SEQ i=0 FOR 10`) would only execute once because the loop counter decrement did not set the flags for the subsequent conditional branch.

**Next Steps:**
* Continue testing with more complex Occam-pi programs to verify channel communication, concurrency, and blocking system calls under the AArch64 backend.
* Upstream or format these fixes cleanly for the KRoC repository.