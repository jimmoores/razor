# KRoC Toolchain Drivers Analysis

This document provides an analysis of the primary command-line tools used to build and manage occam-pi programs within the KRoC ecosystem: `kroc` and `occbuild`. These tools are located in `tools/kroc/`.

## 1. `kroc` - The Main Compiler Driver

`kroc` is a shell script acting as the primary front-end driver for the compilation system, functioning analogously to `gcc` for C. It orchestrates the pipeline of compiling, translating, assembling, and linking occam-pi code.

### Core Functionality
The script processes command-line arguments and input files, driving the following pipeline:
1.  **Compilation (`occ21`)**: Transforms occam source code (`.occ`) into Extended Transputer Code (`.tce` or `.etc`).
2.  **Translation (`tranx86`)**: Translates ETC/TCE into target-specific assembly code (`.s`) or native object code (`.o`).
3.  **Assembly (`as`)**: Assembles the `.s` files into native object files (`.o`), if the translator produced assembly.
4.  **Linking (`ld` / `gcc`)**: Links the object files with the KRoC runtime system (`CCSP`) and other libraries to produce an executable or a shared library.

### Key Features & Internal Logic
*   **Architecture Support**: It detects the target CPU (e.g., `i386`, `x86_64`, `aarch64`) and OS (Linux, Darwin, Cygwin) to pass appropriate flags to the underlying tools.
    *   *Example:* On AArch64, it adds `-maarch64` to the translator options and `-AARCH64` to the compiler options.
*   **Path Management**: It manages search paths for occam libraries (`ISEARCH` environment variable, passed to `occ21`) and native libraries (`-L` flags passed to the linker).
*   **Modes of Operation**:
    *   **Compile-only (`-c`)**: Stops after generating the object file.
    *   **Library generation (`-l`, `--library`)**: Configures the linker to produce shared objects (`.so` or `.dylib`).
    *   **Error Modes**: Supports switching between HALT, STOP, and REDUCED error modes via flags (`-H`, `-S`).
*   **Runtime Integration**: automatically links against the runtime kernel (`libccsp`) and interface libraries (`libkrocif`).

### Configuration
The script is generated from `kroc.in` by `configure`, injecting paths to the installed compiler binaries (`occ21`, `tranx86`), default library paths, and version information.

## 2. `occbuild` - The Build System Abstraction

`occbuild` is a Python script that provides a higher-level, standardized interface for building occam-pi components. It abstracts the differences between various underlying toolchains (Native KRoC, TVM, Tock) and provides a command-set tailored for building modules and libraries.

### Core Functionality
It operates in specific **modes**:
*   `--object`: Compiles a single source file to an intermediate object format (`.tce`).
*   `--library`: Links object files into a library bundle (`.lib`). It also generates the `.module` file, which serves as a header file for the library, automating `#USE` directives and pragmas.
*   `--program`: Builds a complete executable.
*   `--install`: Handles installation of build artifacts to system directories (`bin`, `lib`, `include`).
*   `--cflags`: Exports C flags required for C code to interface with the selected toolchain.

### Toolchain Abstraction
`occbuild` defines a `Toolchain` class with implementations for:
1.  **`kroc`**: Wraps the `kroc` shell script described above. Used for native compilation.
2.  **`tvm`**: Wraps `occ21` and `plinker.pl` (the Transterpreter linker). Used for bytecode generation.
3.  **`tock`**: Wraps the `tock` compiler.

### Dependency Management
Unlike `kroc`, which is a straight compiler driver, `occbuild` has logic to help manage library metadata:
*   It generates `.module` files that contain `#INCLUDE` and `#USE` directives.
*   It supports a `--need` flag to declare dependencies on other modules, ensuring the correct include paths are passed to the compiler.
*   It handles the `in-tree` build scenario, allowing the KRoC build system to bootstrap itself.

## 3. `kmakef` - Makefile Generator

`kmakef` is a Perl script that scans occam source files for `#INCLUDE` and `#USE` directives to calculate dependencies and generates a simple `Makefile`.

*   **Usage**: `kmakef program.occ` produces `program.kmf`.
*   **Purpose**: Provides a quick way to bootstrap a build for a simple project without writing manual Makefiles, though for complex projects (like KRoC itself), Automake or `occbuild` is preferred.

## Relationship Summary

*   **User Level**: Users typically run `kroc` directly to compile simple programs.
*   **Build System Level**: `occbuild` is heavily used by the KRoC build system (Automake) to build the standard libraries and modules in a platform-agnostic way.
*   **Execution**: `occbuild --toolchain=kroc` calls `kroc`, which calls `occ21` -> `tranx86` -> `gcc`.
