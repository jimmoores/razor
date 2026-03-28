# CIF Module Analysis

## Overview
The **CIF (C Interface)** module, located in `kroc64/kroc/modules/cif/libsrc`, provides the support layer for interoperability between occam-pi processes and C functions within the KRoC/CCSP ecosystem. It implements the "Inmos-toolkit / CCSP style C API," allowing C code to perform channel I/O, manage processes, handle timers, and manipulate mobile types using mechanisms compatible with the underlying CCSP runtime.

## File Structure & Build System
*   **`cif.h`**: The primary C header file. It defines a set of `static inline` convenience functions that wrap lower-level CCSP kernel calls (imported from `ccsp_cif.h`). This ensures that C code usually includes this logic directly rather than linking against a large binary blob for these primitives.
*   **`cif.inc`**: The occam header file. Currently, it serves as a placeholder/marker (`-- @module cif`), ensuring that CIF support is enabled in the occam compilation context, likely via `occbuild`.
*   **`configure.ac` / `Makefile.am`**: Standard Autotools build configuration. It uses `occbuild` to compile and install `cif.lib` and headers. It conditionally builds only if the toolchain is `kroc`.

## Data Structures
The module relies on types defined in the runtime's `ccsp_cif.h` (not fully shown but inferred from usage):
*   **`Workspace wptr`**: A pointer to the lightweight process workspace. This is the handle to the "context" of the running process.
*   **`Channel *c`**: Represents an occam channel.
*   **`Time`**: Represents system time.
*   **`Process`**: A function pointer type for spawning processes.
*   **`LightProcBarrier`**: A structure used to synchronize parallel processes (`ProcPar`).
*   **`mt_array_t` / `mt_cb_t`**: Structures representing mobile type arrays and channel bundles.

## Functionality & API
The `cif.h` header exposes high-level primitives to the C programmer:

### Channel I/O
*   **`ChanInInt`, `ChanOutInt`**: Wrappers around `ChanInWord` / `ChanOutWord` for integer communication.
*   **`ChanInChar`, `ChanOutChar`**: Wrappers around `ChanInByte` / `ChanOutByte` for character communication.

### Process Management
*   **`ProcPar(wptr, numprocs, ...)`**: Implements `PAR` behavior in C. It initializes a `LightProcBarrier`, iterates through a variable argument list of `(Workspace, Process)` pairs, starts them via `LightProcStart`, and waits on the barrier.
*   **`ProcAlt(wptr, ...)`**: Implements `ALT` behavior. It performs the `enable` sequence on a list of channels, waits if none are ready (`AltWait`), performs the `disable` sequence, and returns the index of the successfully selected guard (`AltEnd`).

### Timer
*   **`TimerDelay(wptr, delay)`**: Reads the current time and waits until `current + delay`.

### Mobile Types
*   **`MTAllocArray`, `MTAllocDataArray`**: allocate multi-dimensional mobile arrays, handling the packing of dimension sizes into the variable argument list.
*   **`MTAllocChanType`**: Allocates mobile channel bundles.
*   **`MTResize1D`**: Resizes 1D mobile arrays.

## Calling Conventions & Symbol Handling

### C to Kernel Translation
The CIF module acts as a bridge between standard C calling conventions and the CCSP kernel requirements:
1.  **Workspace Passing**: Every function in `cif.h` takes `Workspace wptr` as its first argument. In the CCSP architecture, the workspace pointer is akin to the `this` pointer or stack frame base for the lightweight process.
2.  **Kernel Invocation**: The inline functions delegate to symbols like `ChanInWord`, `Alt`, etc. These symbols are part of the CCSP runtime (`libccsp`). In the KRoC architecture, these runtime functions typically handle the context switch into the kernel (e.g., via `tranx86` generated assembly glue or direct function calls if the runtime is linked).

### Relation to occ21 and tranx86
*   **C. Prefix**: When an occam program calls a C function `MyFunc`, it typically declares it as `PROC MyFunc (...) IS "C.MyFunc":`. `tranx86` (the code generator) usually handles `C.` prefixed calls by preparing the arguments according to the C calling convention (platform specific, e.g., passing arguments in registers `x0`-`x7` on AArch64 or stack on x86) and passing the `Wptr` as the first argument.
*   **Execution Flow**:
    1.  **Occam**: `CALL "C.MyProc"` -> **tranx86**: Generates assembly to setup `x0=Wptr`, `x1=arg1`, `call _MyProc`.
    2.  **C Implementation**: `void MyProc(Workspace wptr, int arg1) { ... }`. The `wptr` is received in `x0`.
    3.  **CIF Usage**: Inside `MyProc`, the user calls `ChanInInt(wptr, c, &val)`.
    4.  **CIF Inline**: `ChanInInt` expands to `ChanInWord(wptr, c, val)`.
    5.  **Runtime**: `ChanInWord` (in `libccsp`) performs the actual channel logic, potentially descheduling the process by saving state into `wptr` and jumping to the scheduler.

### Symbol Prefixes
*   The `cif` library itself does not appear to mangle names. It relies on the standard C naming convention of the host compiler.
*   The `C.` stripping is handled by the compiler/transputer-code-translator (`tranx86`), linking the occam symbol `C.Foo` to the C symbol `Foo` (or `_Foo` on Darwin).
