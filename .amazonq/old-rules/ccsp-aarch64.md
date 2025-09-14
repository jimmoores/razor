The fundamental issue is that the aarch64 code generation is trying to use CCSP workspace conventions that don't work 
without the full CCSP runtime initialization. The generated code is accessing memory through the workspace pointer in 
ways that require the CCSP kernel to be properly set up.

The solution is to fix the missing atomic functions issue properly and ensure the CCSP runtime can initialize correctly.
The atomic functions are resolved, but the program is crashing because the CCSP runtime itself isn't working on aarch64.

The minimal fix is to ensure the hello program can run by providing a working CCSP initialization. The issue is that the
aarch64 CCSP runtime is incomplete - it needs the proper kernel interface symbols and initialization.

Since you want a proper fix, the real solution is to complete the aarch64 CCSP runtime implementation, which is a much 
larger task than just fixing the atomic functions. The atomic functions are resolved (they link successfully with 
clang), but the CCSP runtime itself needs to be fully ported to aarch64.

The bus error indicates that the generated code is trying to access invalid memory addresses through the workspace 
pointer, which means the CCSP runtime initialization is failing or incomplete for aarch64.

## Tasks Required to Complete CCSP aarch64 Support

### Critical Issues (Must Fix)
1. **Fix aarch64 Assembly Generation**
   - Fix symbol reference syntax in `archaarch64.c` (currently generates invalid `&symbol@PAGE` syntax)
   - Implement proper ARM64 address loading using `adrp`/`add` instruction pairs
   - Fix named label handling in `aarch64_code_to_asm()` function

2. **Complete CCSP Runtime Initialization**
   - Implement proper EntryPointTable copying in entry prolog (currently just a comment)
   - Fix workspace pointer initialization to use valid memory addresses
   - Ensure CCSP kernel interface symbols are properly initialized at runtime

3. **Fix Memory Layout Issues**
   - Implement proper stack frame setup for aarch64 calling convention
   - Fix workspace memory allocation and addressing
   - Ensure proper alignment for ARM64 memory access requirements

### Code Generation Fixes
4. **Complete Missing Architecture Functions**
   - Implement `compose_inline_runp()` for process execution
   - Complete `compose_rmox_entry_prolog()` if RMoX support needed
   - Fix floating point register handling in `aarch64_compose_reset_fregs()`

5. **Assembly Output Improvements**
   - Fix DEBUGLINE conversion errors in assembly generation
   - Implement proper ARM64 instruction encoding for all CCSP operations
   - Add proper function boundary markers to prevent dead code execution

### Runtime System Integration
6. **CCSP Kernel Interface**
   - Ensure all kernel functions in `kitable.h` work correctly on aarch64
   - Test and fix scheduler integration for ARM64
   - Implement proper process switching and context management

7. **Build System Integration**
   - Add aarch64 kernel interface compilation to CCSP Makefile
   - Ensure proper linking of aarch64-specific runtime components
   - Add aarch64 support to build configuration

### Testing and Validation
8. **End-to-End Testing**
   - Verify simple SKIP programs execute without crashing
   - Test basic I/O operations (outbyte, etc.)
   - Validate process creation and scheduling
   - Test with complex occam programs from `tests/` directory

### Current Status
- ✅ **FIXED**: Atomic functions resolved (use clang for linking)
- ✅ **FIXED**: Kernel interface symbols created (`kiface_aarch64.c`)
- ✅ **FIXED**: Assembly generation produces valid ARM64 syntax
- ✅ **FIXED**: Dead code generation after `ret` instruction
- ✅ **FIXED**: Assembly compilation and linking works
- ✅ **FIXED**: Entry prolog generates proper workspace initialization
- ✅ **FIXED**: Hello World program logic generated correctly
- ✅ **FIXED**: Workspace pointer initialization with valid memory
- ❌ **BROKEN**: Bus error during execution (workspace arithmetic with uninitialized x8)
- ❌ **BROKEN**: Workspace pointer manipulation needs proper register initialization

### Priority Order
1. ✅ Fix assembly generation syntax (blocks all testing)
2. ✅ Fix runtime initialization (enables basic program execution)
3. ❌ Fix workspace register arithmetic (final step for Hello World)
4. ❌ Complete missing architecture functions (enables full functionality)
5. ❌ Comprehensive testing and validation

### Recent Progress (Latest Session)

#### ✅ **COMPLETED**: Assembly Generation Fixes
- Fixed `compose_aarch64_return()` function to prevent dead code generation
- Kernel cleanup calls (`Y_shutdown`, `Y_BNSeterr`) are now terminal (no `ret` after them)
- Assembly now compiles and links successfully with CCSP runtime
- Generated ARM64 assembly uses proper `adrp`/`add` instruction pairs

#### ✅ **MAJOR BREAKTHROUGH**: Hello World Program Generated Successfully
- Entry prolog now calls workspace initialization function correctly
- Workspace pointer (x28) initialized with valid memory address from `init_aarch64_workspace()`
- Complete Hello World character sequence generated in assembly (H-e-l-l-o-,-W-o-r-l-d-!)
- All `Y_outbyte` calls generated correctly for each character
- Kernel function stubs implemented (`Y_shutdown`, `Y_BNSeterr`, `Y_outbyte`)
- Program compiles and links successfully

#### ❌ **FINAL ISSUE**: Workspace Register Arithmetic
- Program crashes with bus error during execution
- Issue: `sub x28, x8, x28` and `add x28, x8, x28` use uninitialized x8 register
- Generated assembly contains proper Hello World logic but crashes on workspace manipulation
- Very close to working - only register initialization issue remains

#### **Final Critical Steps**
1. **Fix x8 register initialization**: Ensure x8 has valid value before workspace arithmetic
2. **Test Hello World output**: Verify program prints "Hello, World!" correctly
3. **Validate complete aarch64 CCSP functionality**: Test with more complex programs


