# Symbol Naming Issues Investigation Summary

## Problem
kroc (Kent Retargetable occam Compiler) produces or expects different external symbol spellings across components and platforms. On Darwin/macOS (including arm64), this interacts with the platform’s C symbol prefixing rules and leads to mismatches at link time, especially between:
- occ21’s notion of “named labels” (e.g., for #PRAGMA EXTERNAL),
- tranx86’s emission of target assembly symbol references,
- CCSP/runtime symbols defined via inline-assembly labels/macros, and
- names for occam libraries vs C libraries.

## Key conclusion about Darwin/macOS
- Darwin/Mach-O uses a single leading underscore for C/assembly symbol spelling (e.g., C identifier foo becomes assembly/global symbol _foo). This applies to both x86 and arm64 (AArch64) targets.
- There is no platform requirement to use “double underscore” for all external symbols. Some system/private symbols happen to begin with two underscores in their identifiers, but that’s not a universal rule. The assembler/linker expects the usual single “_” C-to-asm mapping for user code.

Implication: Any component that unconditionally generates “__name” for Darwin will generally not match calls that are emitted as “_name” (or vice versa).

## Root Causes and Categories
There are multiple naming sources and transformation points:

1) occam #PRAGMA EXTERNAL (library symbols)
- What occ21 emits: a named label for the target callable (sometimes with dotted namespace-like forms such as “socket.accept”).
- What tranx86 should do: apply the target’s platform ABI spelling when emitting an external reference:
  - Darwin/Mach-O: emit “_socket.accept” (single underscore).
  - ELF targets: emit “socket.accept” (no underscore).
- Status: The recent aarch64 path in tranx86 now normalizes symbol references through a single conversion point. This has addressed most external library call mismatches observed on Darwin.

2) CCSP Runtime and Kernel Calls (e.g., X_alt, X_getpas, C.killcall, C.sl.bind)
- Where defined: via inline-assembly label macros in CCSP headers, and referenced from generated code by tranx86.
- What must be consistent:
  - On Darwin: runtime must define symbols as “_X_alt”, “_X_getpas”, “_C.killcall”, “_C.sl.bind”, etc. (single underscore in assembly/global symbol).
  - On ELF: define as “X_alt”, “X_getpas”, “C.killcall”, “C.sl.bind” (no underscore).
- Mismatch pattern we saw: inline-assembly label macros for Darwin sometimes introduced double-underscored global labels (e.g., “__X_alt”). Meanwhile, callers in generated code were emitting the standard single-underscore Darwin spelling (“_X_alt”), or vice versa, causing unresolved externals.

3) Occam processes exposed to C (CIF “O_...”)
- Conventions like O_do_stuff are expected to appear as normal C symbols:
  - Darwin: assembler/global symbol “_O_do_stuff”.
  - ELF: symbol “O_do_stuff”.
- tranx86’s conversion for occam process symbols (e.g., “do.stuff” → “O_do_stuff”) must then apply the platform prefix rules consistently.

4) Intrinsics and special names (%CHK, %O, etc.)
- These occasionally appear in intermediate names and must be canonicalized before final emission. The translator should clean these and then apply the platform prefix.

## Key Files for Investigation

### Symbol Generation Macros
- `runtime/ccsp/include/aarch64/sched_asm_inserts.h` - Contains `_K_SETGLABEL` and `_K_SETGGLABEL` macros that generate symbol labels
- `runtime/ccsp/kernel/sched.c` - Defines CCSP symbols using `K_CALL_DEFINE_*` macros

### Symbol Detection Scripts  
- `runtime/ccsp/utils/make-header.py` - Generates CIF stubs and symbol tables by scanning source code

### Configuration
- `runtime/ccsp/include/ccsp_config.h` - Defines `TARGET_OS_DARWIN` flag for platform detection

## Current Status
- External library symbols (from #PRAGMA EXTERNAL): Fixed in the AArch64 backend by unifying symbol name conversion and emission. References now respect Darwin’s single-underscore convention and ELF’s no-underscore convention.
- CCSP runtime/kernel symbols: IN PROGRESS. The inline-assembly label macros must be brought into line with the same platform rules (single underscore on Darwin; none on ELF) and not invent “double underscore” forms unless the identifier itself truly begins with two underscores.

## Recommended Naming Rules (single source of truth)
Adopt the following consistent rules everywhere (compiler, translator, runtime):

- Platform prefix:
  - Darwin/Mach-O: symbols are emitted/declared with one leading underscore in assembly (e.g., “_foo”).
  - ELF platforms: no leading underscore (e.g., “foo”).

- Categories:
  - C-exposed occam processes: final C identifiers like O_do_stuff → “_O_do_stuff” (Darwin) / “O_do_stuff” (ELF).
  - CCSP runtime/kernel calls (X_*, C.*): identifiers remain as written (e.g., X_alt, C.killcall), then mapped to platform spelling:
    - Darwin: “_X_alt”, “_C.killcall”
    - ELF: “X_alt”, “C.killcall”
  - #PRAGMA EXTERNAL library.function: preserve the library.function stem, then apply platform spelling:
    - Darwin: “_library.function”
    - ELF: “library.function”
  - Intrinsics/specials: canonicalize (%CHK → _CHK or similar internal form) prior to prefixing; avoid leaving percent signs in the final external symbol.

- Avoid “double underscore” fabrications:
  - Do not generally rewrite X_alt to “__X_alt” on Darwin. That will not match callers that correctly request “_X_alt” by applying the standard C-to-asm mapping for Mach-O.

## Action Items
1) Runtime inline-assembly label macros
- Ensure the macros that define global labels for CCSP/runtime symbols emit:
  - Darwin: one leading underscore in the assembler/global symbol name (e.g., .globl _X_alt; _X_alt:).
  - ELF: no underscore (e.g., .globl X_alt; X_alt:).
- Remove any Darwin paths that emit “__name” for these runtime symbols unless the identifier itself begins with “__”.

2) Translator (tranx86)
- Keep symbol conversion unified:
  - Decide symbol stem (including any library.function, “X_*”, “C.*”, “O_*”).
  - Canonicalize special characters once (dot, percent, caret handling as required by the existing conventions).
  - Apply platform prefix at the final emission site:
    - Darwin/Mach-O: prepend “_” to the final stem.
    - ELF: emit the stem unchanged.

3) Compiler (occ21)
- Verify/confirm that named-label emission for:
  - #PRAGMA EXTERNAL symbols,
  - intrinsics and kernel entries,
  produces consistent stems that the translator already expects. The translator should be the one responsible for final ABI spelling (prefix/no prefix), so the compiler should not hardcode platform-specific underscores.

4) End-to-end tests
- Rebuild and verify with nm/objdump:
  - CCSP runtime archive/shared object: verify symbols appear as “_X_alt, _X_getpas, _C.killcall, _C.sl.bind” on Darwin; “X_alt, X_getpas, C.killcall, C.sl.bind” on ELF.
  - A simple occam program with #PRAGMA EXTERNAL referencing a library symbol (like “socket.accept”): verify the object references “_socket.accept” on Darwin and “socket.accept” on ELF.
  - A CIF case (O_do_stuff): verify calls are made to “_O_do_stuff” on Darwin and “O_do_stuff” on ELF.
- Confirm no references or definitions use the unintended double-underscore form unless it’s part of the actual identifier.

## Detailed Symbol Flow Analysis

### Symbol Flow Pipeline: occ21 → tranx86 → Assembly

**1. occ21 Compiler Symbol Generation**
- Emits GLOBNAME/GLOBNAMEEND markers in ETC bytecode for exported symbols
- Processes #PRAGMA EXTERNAL, function names, and process names
- Generates symbols with various prefixes: C., CIF., B., BX., KR., or plain dotted names

**2. tranx86 ETC Processing (etcrtl.c)**
- `generate_call()` function categorizes symbols by prefix:
  - Case 0: Normal occam calls (no prefix) → direct jump to symbol name
  - Case 1: C. calls → `arch->compose_external_ccall`
  - Case 2/3: B./BX. calls → `arch->compose_bcall` 
  - Case 4: KR. calls → `arch->compose_bcall`
  - Case 6: CIF. calls → `arch->compose_cif_call`
- Each case delegates to architecture-specific transformation functions

**3. Architecture-Specific Symbol Transformation**

#### i386 Implementation (arch386.c)
**Consistent and Simple Pattern:**
- **C calls (`compose_external_ccall_i386`)**: 
  ```c
  sprintf(sbuf, "%s%s", options.extref_prefix, name + 1);  // Strip "C."
  ```
- **CIF calls (`compose_cif_call_i386`)**:
  ```c
  sprintf(sbuf, "@%s%s", options.extref_prefix, name + 4);  // Strip "CIF.", add "@"
  ```
- **B/BX/KR calls (`compose_bcall_i386`)**:
  ```c
  sprintf(sbuf, "%s%s", options.extref_prefix, name + offset);  // Strip prefix
  ```

#### aarch64 Implementation (archaarch64.c) 
**Inconsistent Pattern - Major Issue:**
- **C calls (`compose_external_ccall_aarch64`)**: ✅ Correct
  ```c
  char *call_name = aarch64_convert_occam_symbol(name);  // Comprehensive handling
  ```
- **CIF calls (`compose_cif_call_aarch64`)**: ❌ **BROKEN**
  ```c  
  add_to_ins_chain(compose_ins(INS_CALL, 1, 0, ARG_NAMEDLABEL, strdup(name)));  // NO TRANSFORMATION!
  ```
- **B/BX/KR calls (`compose_bcall_aarch64`)**: ❌ **BROKEN**
  ```c
  add_to_ins_chain(compose_ins(INS_MOVE, 1, 1, ARG_NAMEDLABEL | ARG_ISCONST, strdup(name), ARG_REG, REG_X1));  // NO TRANSFORMATION!
  ```

### Symbol Transformation Examples

#### Expected Transformations (based on i386 working implementation):

**C Function Calls:**
- Input: `"C.printf"`
- i386 output: `"_printf"` (Darwin) / `"printf"` (ELF)
- aarch64 output: `"_printf"` (via aarch64_convert_occam_symbol) ✅

**CIF Function Calls:**  
- Input: `"CIF.some_function"`
- i386 output: `"@_some_function"` (Darwin) / `"@some_function"` (ELF)  
- aarch64 output: `"CIF.some_function"` (no transformation) ❌

**Blocking Calls:**
- Input: `"B.read"` 
- i386 output: `"_read"` (Darwin) / `"read"` (ELF)
- aarch64 output: `"B.read"` (no transformation) ❌

**Kernel Calls:**
- Input: `"KR.suspend"`
- i386 output: `"_suspend"` (Darwin) / `"suspend"` (ELF)  
- aarch64 output: `"KR.suspend"` (no transformation) ❌

### Critical Issues Identified

1. **aarch64 CIF calls completely broken**: No symbol transformation applied
2. **aarch64 blocking calls broken**: B./BX./KR. prefixes not stripped
3. **Commented-out code exists**: aarch64 functions have proper transformation code commented out
4. **Inconsistent approach**: aarch64 uses comprehensive `aarch64_convert_occam_symbol()` for C calls but bypasses it for CIF/blocking calls

### Root Cause
The aarch64 implementation appears to be incomplete/stubbed out. The proper symbol transformation logic exists in commented-out code but is not being executed. This explains the recent linker errors for CIF and runtime symbols.

# Symbol Naming Summary

This document summarizes the conventions for symbol naming in the kroc project.

## `O_` and `_O_` Prefixes

The `O_` and `_O_` prefixes are used to denote symbols that are part of the occam runtime and are visible to the generated assembly code. The prefixes are added by the `tranx86` translater.

The logic for this is located in the `modify_name` function in `tools/tranx86/asm386.c` and is as follows:

The logic for prepending `O_` is as follows:

- If a symbol name does not already have a special prefix (`_`, `O_`, `DCR_`, `&`, or `@`), the `O_` prefix is added. This is the default case for most symbols.
- The prefix `E_` is used for symbols starting with `^`.
- The prefix `M_` is used for symbols starting with `*`.

The `_O_` prefix is likely a result of the `modify_name` function being called with a symbol that already starts with an underscore.

## Symbol Transformations

The `modify_name` function in `tools/tranx86/asm386.c` performs several transformations on symbol names before they are emitted in the assembly output. These transformations are applied in two stages: character-level transformation and prefixing.

### Character-Level Transformations

These transformations are performed by a `switch` statement inside a loop within the `modify_name` function in `tools/tranx86/asm386.c`.

1.  **`.` (dot) is replaced with `_` (underscore)**: For example, a symbol like `C.write.screen` becomes `C_write_screen`. This applies to prefixes like `C.`, `B.`, `BX.`, `CIF.`, and `KR.`.
2.  **`$`, `^`, `*`, `@`, `&` are stripped from the name**: These characters are removed from the symbol name.
3.  **Truncation at `%`**: The symbol name is truncated at the first occurrence of a `%` character.

### Prefixing Rules

After the character-level transformations, the following prefixing rules are applied based on the *original* symbol name, still within the `modify_name` function in `tools/tranx86/asm386.c`:

- **`O_` prefix**: This is the default prefix. It is added if the symbol name does not start with `_`, `O_`, `DCR_`, `&`, or `@`. For example, `C.write.screen` becomes `O_C_write_screen`.
- **`E_` prefix**: This prefix is added if the original symbol name starts with `^`.
- **`M_` prefix**: This prefix is added if the original symbol name starts with `*`.
- **No prefix**: No prefix is added if the symbol name starts with `_`, `O_`, `DCR_`, `&`, or `@`.

These rules ensure that symbols are correctly formatted for the target assembler and linker, and that they do not conflict with other symbols.