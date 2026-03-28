
# tranx86 Code Review

## Overview
`tranx86` is the backend code generator for the kroc compiler. Despite its name, it is a multi-architecture translator that converts Extended Transputer Code (ETC) or TCE intermediate code into target machine code (assembly or ELF object files).

It employs a modular architecture where a central driver (`main.c`) and common RTL (Register Transfer Language) manipulation logic (`rtlops.c`, `regcolour.c`, `optimise.c`) interact with architecture-specific backends defined via the `arch_t` vtable interface (`archdef.h`).

## Supported Architectures
The tool supports several architectures, selected via the `-m` flag:
- **x86 (32-bit):** `-m386` (and variants)
- **x64 (64-bit):** `-mx64`
- **AArch64 (ARM64):** `-maarch64`
- **MIPS:** `-mmips`
- **SPARC:** `-msparc`
- **PowerPC:** `-mppc`

This review focuses on the modern 64-bit targets: **x64** and **AArch64**.

## Structure
- **Core:**
    - `main.c`: Entry point, argument parsing, architecture selection.
    - `archdef.h`: Defines the `arch_t` struct, which is the interface for all backends.
    - `etc_to_rtl`: Converts stack-based ETC instructions to register-based RTL.
    - `regcolour.c`: Generic register allocator (graph coloring).
- **Backends:**
    - `archx64.c`: Implementation for x86-64.
    - `archaarch64.c`: Implementation for AArch64.
    - `arch386.c`: Legacy 32-bit x86.

## Architecture Analysis

### 1. x64 (archx64.c)

#### Registers
The backend uses the standard x64 register set (`rax`-`r15`).
- **Workspace Pointer (WPTR):** Mapped to `r14`.
- **Front Pointer (FPTR):** Mapped to `r13` (used for the process queue).
- **Back Pointer (BPTR):** Mapped to `r12` (used for the process queue).
- **Available for Allocator:** `rax`, `rcx`, `rdx`, `rbx`, `rsi`, `rdi`, `r8`-`r11`, `r15`.
- **Reserved:** `rsp` (Stack Pointer), `rbp` (Frame Pointer), `r12`-`r14` (Runtime Ptrs).

#### Calling Conventions
- **Kernel Calls:** Adheres to the System V ABI for the first 3 arguments.
    - Arguments are passed in `rdi`, `rsi`, `rdx`.
    - Input registers from the occam stack (`old_a_reg`, etc.) are constrained to these registers before the `call` instruction.
- **External C Calls:**
    - Implemented simply as `callq <symbol>`.
    - Does not appear to have deep logic for complex argument marshalling within `archx64.c` itself, seemingly relying on the generic RTL generation or `compose_kcall` patterns.

#### Symbol Naming
- **Prefixes:** The code does not explicitly enforce a complex symbol mangling scheme within `x64_code_to_asm_stream`.
- **Labels:** It emits labels directly as they appear in the RTL (e.g., `callq symbol`).
- **Darwin Compatibility:** `main.c` detects Darwin and sets `options.extref_prefix` to `_`. However, `archx64.c` does not explicitly use `aarch64_convert_symbol_name`-style logic to apply this universally, though it might be handled upstream or simply relies on the assembler to handle some aspects (which is risky for cross-platform consistency).

#### Sizing
- **Word Size:** 64-bit.
- **Operations:** Uses 64-bit instructions (`movq`, `addq`, `subq`) by default.
- **Widen:** Explicit support for widening 16-bit (`compose_x64_widenshort`) and 32-bit (`compose_x64_widenword`) values to 64-bit.

### 2. AArch64 (archaarch64.c)

#### Registers
The backend uses the standard AArch64 register set (`x0`-`x30`, `sp`).
- **Workspace Pointer (WPTR):** Mapped to `x28`.
- **Front Pointer (FPTR):** Mapped to `x27`.
- **Back Pointer (BPTR):** Mapped to `x26`.
- **Scheduler Pointer (SCHED):** Mapped to `x25`.
- **Stack Pointer (SPTR):** Mapped to `x24` (Distinct from hardware `sp`).
- **Scratch/Temp:** `x16`, `x17`.
- **Available for Allocator:** `x0`-`x15`, `x18`-`x23`.

#### Calling Conventions
- **Kernel Calls (`compose_aarch64_kcall`):**
    - `x0`: Parameter 0.
    - `x1`: Scheduler pointer (passed from `x16`).
    - `x2`: Workspace Pointer (`x28`).
    - `x16`: Loaded with the address of `local_scheduler` (or used to access `sched->cparam` for extra args).
- **External C Calls (`compose_cif_call_aarch64`):**
    - `x0`: Workspace Pointer (`WPTR`).
    - `bl <symbol>`: Calls the function.
    - Uses `aarch64_cleanup_symbol_name` to handle platform-specific prefixes.

#### Symbol Naming (`aarch64_cleanup_symbol_name`)
This architecture has a robust symbol name conversion function designed to mimic the 386 reference implementation:
1.  **Prefix Stripping:** Removes `__`, `C.`, `CIF.`, `B.`, `BX.`, `KR.` prefixes.
2.  **Kernel Mapping:** Maps `Y_` -> `kernel_Y_` and `X_` -> `kernel_X_`.
3.  **Sanitization:** Replaces `.` with `_`, strips characters like `$`, `^`, `*`, `@`, `&`.
4.  **Internal Prefixes:** Adds `E_` (entry), `M_` (module), or `O_` (occam) prefixes if not already present and not a C/Kernel call.
5.  **Platform Prefix:** **Crucially**, it appends `options.extref_prefix` (e.g., `_` on macOS) to the final symbol **only** if a KRoC prefix was added or if it matches specific patterns (like internal label markers). This aligns with the subtle logic of `arch386.c`.

#### Sizing
- **Word Size:** 64-bit.
- **Operations:** Uses 64-bit instructions (e.g., `add`, `sub` with `x` registers).
- **Immediates:** Has special logic (`aarch64_emit_large_immediate`) to handle constants larger than what immediate fields allow, using `movz`/`movk` sequences.

## Functional Summary
The `tranx86` tool is the bridge between the portable ETC bytecode and the specific machine architecture.
1.  **Read:** Parses `.etc` or `.tce` files.
2.  **Translate:** `etc_to_rtl` maps stack operations to a virtual register machine.
3.  **Optimize:** Performs standard optimizations (dead code elimination, constant propagation).
4.  **Register Allocation:** `colour_registers` maps virtual registers to physical architecture registers defined by the backend.
5.  **Emit:** The backend (`arch*.c`) emits the final assembly code, handling instruction selection, calling conventions, and symbol naming.

## Recommendations
1.  **Symbol Naming Consistency:** The `aarch64` backend now has robust symbol naming logic aligned with `arch386`. The `x64` backend should likely adopt similar logic to ensure consistent handling of prefixes (especially the `_` on macOS) and internal occam prefixes (`O_`, `M_`).
2.  **Code Sharing:** There is duplication in logic between backends (e.g., helper functions). Refactoring common logic into `support.c` or a new `arch_common.c` would be beneficial.

## AArch64 Gap Analysis and Fixes

The following issues were identified when comparing `archaarch64.c` with the reference `arch386.c`:

- [ ] **Calling Convention Mismatch**: `compose_aarch64_kcall` calculates parameter offsets using `offsetof(ccsp_sched_t, cparam[...])`. The AArch64 structure packing/padding might differ from `i386`. We need to ensure `offsetof` usage is consistent with `sched_types.h` on the target.
- [ ] **Stack Alignment for External Calls**: `compose_external_ccall_aarch64` subtracts 16 bytes from `sp` for the stack frame. This maintains 16-byte alignment (required by AArch64 ABI), which is correct.
- [ ] **Missing Floating Point logic**: `compose_fp_set_fround_aarch64` and `compose_fp_init_aarch64` are stubs. While they might not be strictly necessary for basic functionality, fully compliant IEEE 754 support requires implementation.
- [ ] **Simplified `compose_cif_call_aarch64`**: The current implementation of `compose_cif_call_aarch64` just puts `WPTR` in `x0` and branches. The `i386` version has complex logic for stack switching or argument marshalling (`ccsp_cif_external_call` macro vs inline assembly). We must ensure that the AArch64 CIF (C Interface) expects `WPTR` in `x0` and handles the rest. (Note: `make-header.py` was updated to generate a stub that takes `WPTR` in `x0`, `sched` in `x1`, `stack` in `x2`... wait, `ccsp_cif_call_aarch64` in `archaarch64.c` only sets `x0`. This might be a mismatch if the stub expects more).
- [ ] **Stubbed Long Operations**: `compose_aarch64_longop` implements `I_TESTSTS`, `I_TESTSTE`, `I_TESTSTD` as trivial stubs returning 0. This might mask errors.
- [ ] **Missing `compose_move_loadptrs` logic**: `i386` has specific logic for `compose_move_loadptrs`. `archaarch64.c` implements it but we should double check pointer widening if dealing with 32-bit addresses in 64-bit space (though KRoC on 64-bit usually implies 64-bit pointers).

**Priority Task List:**

1.  [x] **Verify CIF Calling Convention**: Check if `compose_cif_call_aarch64` needs to pass more than just `WPTR` in `x0`. The `ccsp_cif_call` macro in `sched_asm_inserts.h` (or generated header) likely expects a specific environment. (Verified: `compose_cif_call_aarch64` calls C functions directly (e.g. `C.foo`), which expect `WPTR` in `x0`. This matches the standard AArch64 PCS if the function signature is `void foo(word *wptr)`. The `make-header.py` stubs are for kernel calls and are consistent).
2.  [x] **Fix `compose_aarch64_return`**: The comment mentions "CRITICAL FIX: ... NO RET after kernel cleanup". Verify this logic is sound and doesn't leave the stack unbalanced if called from a context that *expects* a return (though `flushscreenpoint` usually implies termination). (Implemented: Added stack frame restoration (pop `x29`, `x30`, adjust `sp`) before `RET`).
3.  [x] **Implement `compose_aarch64_entry_prolog`**: It's currently just a comment. It should likely set up the frame pointer (`x29`) and `sp` properly if they aren't guaranteed by the caller (CCSP kernel). (Implemented: Added standard AArch64 prologue: save `x29`, `x30`, setup frame pointer).
4.  [x] **Review `compose_aarch64_kcall` parameter passing**: Ensure `sched->cparam` is addressed correctly. (Fixed: Updated `sched_t` definition in `archaarch64.c` to match `sched_types.h`, ensuring correct `offsetof` calculation).
5.  [x] **Implement Robust Symbol Mangling**: Update `aarch64_cleanup_symbol_name` and associated compose functions to match the 386 behavior for `C.`, `B.`, `BX.`, and `CIF.` prefixes, ensuring correct linking on Darwin (macOS).

## Reference Implementation Analysis (x86/i386)

The following analysis of `arch386.c` serves as the reference for porting/fixing other architectures.

### 1. Register Mapping
Special registers are mapped to physical x86 registers via `regcolour_special_to_real_i386`:
- **`REG_WPTR` (Workspace Pointer):** Mapped to `ebp` (`%ebp`).
- **`REG_SCHED` (Scheduler Pointer):** Mapped to `esi` (`%esi`).
- **`REG_SPTR` (Stack Pointer):** Mapped to `esp` (`%esp`).
- **`REG_JPTR`:** Mapped to `esi` (`%esi`). (Context dependent usage, often scratch).
- **`REG_LPTR`:** Mapped to `edi` (`%edi`).
- **Scratch/Arguments:** `eax`, `ecx`, `edx`.

### 2. Kernel Call Marshalling (`compose_kcall_i386`)
The `compose_kcall_i386` function handles the complex task of marshalling arguments from the occam stack to the CCSP C-kernel interface.

*   **Virtual Registers to Physical:**
    *   `xregs[0]`: `eax`
    *   `xregs[1]`: `edx`
    *   `xregs[2]`: `ecx`
*   **Arguments:**
    *   **Argument 0 (Input):** Constrained to `eax` (`xregs[0]`).
    *   **Arguments 1..N (Input):** Moved into memory slots `sched->cparam[i-1]`.
        *   `sched` base address is `REG_SCHED` (`esi`).
        *   Offset calculated via `offsetof(ccsp_sched_t, cparam[...])`.
*   **Pointers:**
    *   **Scheduler Pointer:** `REG_SCHED` (`esi`) is moved to `edx` (`xregs[1]`) immediately before the call.
    *   **Workspace Pointer:** `REG_WPTR` (`ebp`) is moved to `ecx` (`xregs[2]`) immediately before the call.
*   **Invocation:**
    *   The kernel function address is loaded from `sched->calltable[offset]`.
    *   The call is an indirect call: `call *offset(REG_SCHED)`.
*   **Return Values:**
    *   **Result 0:** Expected in `eax` (standard C return).
    *   **Results 1..N:** Loaded back from `sched->cparam[i-1]` into target registers.

**Key Insight for Porting:** The first argument goes in a register (`eax`), subsequent arguments go to the `cparam` array in the `sched` structure. The `sched` pointer and `Wptr` are passed as the 2nd and 3rd arguments (in `edx`, `ecx` respectively) to the C function.

### 3. Pseudo-Instruction Implementation
The `arch_t` interface defines several high-level pseudo-instructions that `arch386.c` implements via `compose_inline_...`.

#### Scheduler & Process Control
*   **`compose_inline_startp` (`STARTP`):**
    *   Initializes the new workspace at `areg` (IPTR, Priority, Link).
    *   Enqueues the process via `compose_inline_enqueue_i386`.
    *   Logic handles both constant and variable start addresses.
*   **`compose_inline_endp` (`ENDP`):**
    *   Decrements the reference count at `W_COUNT(Wptr)`.
    *   If zero, continues execution.
    *   If non-zero, calls `compose_inline_quick_reschedule_i386`.
*   **`compose_inline_quick_reschedule`:**
    *   Checks `Fptr` (Front Pointer).
    *   If `Fptr != NotProcess.p`, jumps to it (context switch).
    *   If `Fptr == NotProcess.p`, jumps to `K_OCCSCHEDULER` (entry point in kernel to handle idle/scheduling).
*   **`compose_inline_full_reschedule`:**
    *   More complex. Checks sync flags (`sf`), priority, and run queue.
    *   Used for `K_PAUSE` or loop rescheduling.

#### Channel I/O (`IN`, `OUT`)
*   **`compose_inline_in` / `out`:**
    *   Checks the channel word (`*chan_reg`).
    *   **If `NotProcess.p` (Empty):**
        *   Stores `Wptr` into the channel word.
        *   Saves state (IPTR, Pointer to dest/src) into `Wptr`.
        *   Reschedules (blocks).
    *   **If valid process (Ready):**
        *   Calls `K_FASTIN` / `K_FASTOUT` (or sized variants).
        *   These kernel calls handle the data copy and re-enqueuing of the waiting process.
*   **`compose_inline_in_2` / `out_2` (Optimized):**
    *   Performs the data copy (using `rep movs` or simple loads/stores) directly inline if possible.
    *   Enqueues the waiting process directly.
    *   Only calls kernel/reschedule if blocking is required.

#### Timer
*   **`compose_inline_ldtimer`:**
    *   Uses `rdtsc` to get cycle count.
    *   Performs 64-bit multiply/add with `glob_cpufactor` to convert cycles to microseconds.
*   **`compose_inline_tin` (Timer Input):**
    *   Reads current time (as above).
    *   Compares with target time.
    *   If `target > now`, calls `K_FASTTIN` to add process to timer queue and reschedule.
    *   Else continues immediately.

#### Alternatives (`ALT`)
*   **`compose_inline_altwt` (`ALTWT`):**
    *   Sets `Wptr[Temp] = NoneSelected`.
    *   Sets `Wptr[State] = Waiting`.
    *   Reschedules.
*   **`compose_inline_enbc` (`ENBC`):**
    *   "Enable Channel". Used during ALT setup.
    *   Checks if channel is ready. If so, updates state to `Ready`.
*   **`compose_inline_disc` (`DISC`):**
    *   "Disable Channel". Used during ALT teardown.
    *   Checks if this channel was the one that fired.

### 4. Workspace Handling
*   **Workspace Struct:** The backend assumes a specific layout for the workspace (offsets defined in `tstack.h` or implied):
    *   `W_IPTR` (-4): Instruction Pointer (return address).
    *   `W_LINK` (-8): Link to next process (run queue).
    *   `W_PRIORITY`: Process priority.
    *   `W_STATUS` / `W_POINTER`: Used for channel communication state.
    *   `W_TIME`: Used for timer waits.
*   The `Wptr` register (`ebp`) always points to the base of the current process workspace.
*   Variable access is typically `offset(%ebp)`.

### 5. C Interface (CIF) Calls
*   **`compose_cif_call_i386`:**
    *   Adjusts `Wptr` (stack growth/params).
    *   Saves `sched`, `Wptr` state to memory.
    *   Aligns stack (`esp`).
    *   Calls the C function `C.Name` (prefixed with `_` or `@`).
    *   Restores `sched`, `Wptr` from memory.

## Symbol Handling & Mangling (386 Reference & AArch64 Fixes)

The `tranx86` tool employs a multi-stage process for handling symbols, splitting responsibilities between the high-level `generate_call` (in `etcrtl.c`) and the architecture-specific backend (`arch386.c`, `asm386.c`, `archaarch64.c`). The AArch64 implementation has been updated to match the 386 behavior precisely.

### 1. `modify_name` / `aarch64_cleanup_symbol_name`
This function acts as the final "mangler" before a symbol is emitted to the assembly stream.
- **Dot Conversion:** All `.` characters are converted to `_`.
- **Automatic Prefixing:**
    - Adds `O_` by default to standard symbols.
    - Adds `E_` if the symbol starts with `^` (EntryPoint).
    - Adds `M_` if the symbol starts with `*` (Module).
    - **Exception:** Prefixes are *skipped* if the symbol starts with `_`, `&`, `@`, `O_`, or `DCR_`.
- **Platform Prefixing (`options.extref_prefix`):**
    - If a prefix (`O_`, `E_`, `M_`) was added, the platform-specific prefix (e.g., `_` on Darwin) is also prepended.
    - **Note:** This logic implies that symbols starting with `@` (like CIF calls, see below) do *not* get the automatic `O_` prefix or the automatic `extref_prefix` insertion *within this function*.

### 2. External Call Handling (Aligned AArch64 Implementation)

#### C Calls (`C.Name`)
Handled by `compose_external_ccall_i386` in `arch386.c` and updated `compose_aarch64_external_ccall` in `archaarch64.c`.
- **Prefix Handling:** The code explicitly skips the first 2 characters (`C.`) of the ETC symbol name: `string_dup(name + 1)`. **Note:** This implies it keeps the dot (`.Name`).
- **Mangling:**
    - It manually applies `options.extref_prefix` via `sprintf`.
    - **Result:** `C.printf` -> `.printf` -> `_.printf` (on Darwin) -> `__printf` (after cleanup converts `.` to `_`).
    - **Linux:** `C.printf` -> `.printf` -> `_printf`.

#### Blocking Calls (`B.Name`, `BX.Name`)
Handled by `compose_bcall_i386` in `arch386.c` and updated `compose_aarch64_bcall` in `archaarch64.c`.
- **Prefix Handling:**
    - For `B.`, it uses offset 1 (`name + 1`). Keeps the dot: `.Name`.
    - For `BX.`, it uses offset 2 (`name + 2`). Keeps the dot: `.Name`.
- **Mangling:**
    - Similar to C calls, it manually prepends `options.extref_prefix`.
    - **Result:** `B.foo` -> `.foo` -> `_.foo` (Darwin) -> `__foo` (after cleanup).

#### CIF Calls (`CIF.Name`)
Handled by `compose_cif_call_i386` in `arch386.c` and updated `compose_aarch64_cif_call` in `archaarch64.c`.
- **Prefix Handling:** It explicitly skips the first 4 characters (`CIF.`) using `name + 4`. `CIF.MyProc` becomes `MyProc`.
- **Mangling:**
    - It prepends `@` and `options.extref_prefix`: `sprintf(sbuf, "@%s%s", prefix, name + 4)`.
    - **Result (Darwin):** `CIF.MyProc` -> `MyProc` -> `@_MyProc`.
    - **Result (Linux):** `CIF.MyProc` -> `MyProc` -> `@MyProc`.
- **Final Output:** `aarch64_cleanup_symbol_name` (like `modify_name`) sees the `@` and:
    1.  Skips the default `O_` prefixing logic.
    2.  **Strips** the `@` character during the copy loop (it is not copied).
    - **Final Symbol:** `_MyProc` (Darwin) or `MyProc` (Linux).

### 3. Intrinsics
KRoC "intrinsics" generally map to ETC instructions handled in `do_code_primary` or "ETC Special" handling.
- **Timer (`LDTIMER`):** Mapped to `compose_inline_ldtimer` (AArch64 `cntvct_el0`) / `compose_inline_ldtimer_i386` (`rdtsc`).
- **Floating Point:** Mapped to x87 FPU instructions or AArch64 vector instructions.

## 64-bit Pointer Arithmetic and Constant Handling Fixes

Significant updates were made to `etcrtl.c` and architecture backends to ensure 64-bit compliance for pointer arithmetic and constant handling.

### 1. Variadic Constant Handling
The `compose_ins` and `compose_ins_ex` functions are variadic and use `va_arg(ap, intptr_t)` to retrieve constant operands. On 64-bit systems, passing a 32-bit `int` directly can result in incorrect sign-extension or garbage in the upper 32 bits if the caller and callee disagree on the size.
- **Fix:** All constant workspace adjustments, pointer offsets, and numeric constants in `etcrtl.c` were explicitly cast to `(intptr_t)` before being passed to `compose_ins`.
- **Critical Instructions:** `I_AJW`, `I_CALL`, `I_NCALL`, `I_NRET`, `I_LDC`, `I_STL`, `I_STNL`, `I_EQC`, `I_ADC`, `I_TALT`, and `I_LDINF`.

### 2. Word Index Calculation (`I_WSUB`, `I_PROC_PARAM`)
The word index shift was previously hardcoded as `WShift` (2) in some parts of `etcrtl.c`. On 64-bit architectures, this must be `WSH` (3) to correctly calculate 8-byte offsets.
- **Fix:** Replaced `WShift` with `WSH` in `I_WSUB` and `I_PROC_PARAM` logic. This ensures that indexing into arrays of words or passing parameters correctly handles the 64-bit word size.

### 3. Architecture Backends (`archaarch64.c`, `archx64.c`)
Similar fixes were applied to the architecture-specific backends where constants (like `0` for null pointers or `16` for stack alignment) were passed to `compose_ins`.
- **Fix:** Ensured consistent use of `(intptr_t)` casts for all `ARG_CONST` operands in `archaarch64.c` and `archx64.c`.

### 4. Mobile Space Initialization
Mobile-space initialization logic in `gen_mobilespace_init` was updated to use the architecture-appropriate `MostNeg` constant (64-bit `0x8000000000000000` on AArch64/x64) and ensure correct constant representation via `intptr_t` casts.