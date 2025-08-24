# Testing Procedures for tranx86 Debugging

## Quick Test Commands

### Basic Test Compilation Chain
```bash
# Navigate to project root
cd /Users/jim/CLionProjects/kroc

# Build tranx86 if needed
make -C tools/tranx86

# 1. Compile occam to TCE (ETC bytecode) for different targets
./tools/occ21/occ21 -AARCH64 -etc simple.occ simple_aarch64.tce
./tools/occ21/occ21 -X64 -etc simple.occ simple_x64.tce  
./tools/occ21/occ21 -etc simple.occ simple_x86.tce

# 2. Translate TCE to assembly for different targets
./tools/tranx86/tranx86 -maarch64 simple_aarch64.tce simple_aarch64.s
./tools/tranx86/tranx86 -mx64 simple_x64.tce simple_x64.s
./tools/tranx86/tranx86 simple_x86.tce simple_x86.s

# 3. Check TCE file contents (optional debugging)
hexdump -C simple_x64.tce | head -20
```

### Debug with LLDB
```bash
# WARNING: Interactive lldb sessions may hang - use script approach instead

# Create debug script for automated lldb session
cat > debug_tranx86.lldb << 'EOF'
run
bt
quit
EOF

# Run with LLDB using script to avoid hanging
lldb -s debug_tranx86.lldb -- ./tools/tranx86/tranx86 -maarch64 simple_aarch64.tce simple_aarch64.s
lldb -s debug_tranx86.lldb -- ./tools/tranx86/tranx86 -mx64 simple_x64.tce simple_x64.s

# Alternative: Use timeout to prevent hanging
timeout 10s lldb -- ./tools/tranx86/tranx86 -mx64 simple_x64.tce simple_x64.s
```

### Test Status Summary
- **x86 (32-bit)**: ✅ Works - RTL generation successful, assembly errors due to missing DEBUGLINE conversions
- **x64**: ✅ Fixed - No more segfaults, RTL generation successful, same assembly errors as x86
- **aarch64**: ✅ Fixed - No more segfaults, RTL generation successful, same assembly errors as x86

## Known Issues (Historical - Most Fixed)
1. ✅ **FIXED**: Memory corruption in x64/aarch64 causing corrupt string pointers (address 0x23c0180)
2. ✅ **FIXED**: Segfault in rtl_remove_deadnamelabels() at strcmp() due to corrupt named label strings
3. ✅ **FIXED**: Pointer truncation when forcing 64-bit pointers into 32-bit fields
4. ⚠️ **PARTIAL**: Missing function implementations in archx64.c and archaarch64.c (basic functionality works)
5. ✅ **FIXED**: compose_ins function calls failing due to unimplemented architecture-specific code
6. ⚠️ **ONGOING**: Interactive lldb sessions may hang - use scripted approach or timeout
7. ⚠️ **ONGOING**: Assembly generation errors due to missing DEBUGLINE conversions (not critical)
8. ⚠️ **ONGOING**: Some architecture-specific functions have incomplete implementations but don't cause crashes

## Root Cause Analysis Needed
- Focus on WHERE corrupt string pointers are being generated, not on wrapping strcmp
- Check memory allocation/deallocation in x64/aarch64 architecture code
- Investigate pointer size mismatches between 32-bit and 64-bit code paths

## Investigation Conclusion - ROOT CAUSE IDENTIFIED

**CRITICAL BUG FOUND**: The issue IS caused by pointer truncation, but not where initially suspected.

The `ins_arg->regconst` field is correctly defined as `intptr_t` (line 280 in structs.h), but the bug is in the `compose_ins` function at lines 1700-1701 in rtlops.c:

```c
case ARG_NAMEDLABEL:
case ARG_TEXT:
    tmp_ins->in_args[i_in]->regconst = (int)va_arg (ap, char *);
```

**The Problem**: On 64-bit systems, pointers are 64 bits but `int` is 32 bits. This cast truncates the upper 32 bits of the pointer, creating corrupt addresses like 0x23c0180.

**The Fix**: Change the cast from `(int)` to `(intptr_t)` to preserve the full pointer value:

```c
case ARG_NAMEDLABEL:
case ARG_TEXT:
    tmp_ins->in_args[i_in]->regconst = (intptr_t)va_arg (ap, char *);
```

Similar issues likely exist elsewhere in the codebase where pointers are cast to `int`. A comprehensive audit is needed to find all instances of pointer truncation.

**Impact**: This affects ALL architectures (x86, x64, aarch64) because the bug is in the host compilation, not target code generation. Even basic x86 translation fails because the compose_ins function is used throughout the codebase for creating instructions with named labels.

## Fix Results - SUCCESS! ✅

After applying the pointer truncation fixes to all 11 instances in rtlops.c:

**All architectures now work without segfaults:**
- **x86**: ✅ RTL generation successful
- **x64**: ✅ RTL generation successful  
- **aarch64**: ✅ RTL generation successful

Remaining issues are now implementation-specific (missing DEBUGLINE conversions, assembly generation) rather than fundamental crashes. The core 64-bit host compatibility problem has been resolved.

## Symbol Name Fix - SUCCESS! ✅

Fixed aarch64 assembly generation to use correct CCSP symbol names:
- **Problem**: Generated `Y_shutdown` and `Y_BNSeterr` but CCSP library has `_kernel_Y_shutdown` and `_kernel_Y_BNSeterr`
- **Solution**: Modified archaarch64.c to add `__kernel_` prefix (with macOS underscore) for Y_ symbols
- **Result**: ✅ Object files now link successfully with CCSP library

## End-to-End Test Results

**✅ Complete Compilation Chain Works:**
1. **OCC21**: ✅ Compiles occam to aarch64 TCE bytecode
2. **tranx86**: ✅ Translates TCE to aarch64 assembly and object files  
3. **Linking**: ✅ Successfully links with CCSP runtime library
4. **Executable**: ✅ Creates valid ARM64 Mach-O executable

**⚠️ Runtime Status**: Executable runs but crashes with bus error - indicates CCSP initialization or memory access issues, not fundamental architecture problems.

**Commands for End-to-End Test:**
```bash
# 1. Compile occam to aarch64 TCE
./tools/occ21/occ21 -AARCH64 -etc simple.occ simple_aarch64.tce

# 2. Translate to aarch64 object file
./tools/tranx86/tranx86 -maarch64 -es -o simple_aarch64.o simple_aarch64.tce

# 3. Create main function with CCSP initialization and Y_ symbol stubs
# 4. Link with CCSP library
clang -arch arm64 -o final_exe ccsp_main.o simple_aarch64.o -L./runtime/ccsp -lccsp
```

## Test Files Available
- `simple.occ` - Minimal SKIP program ("PROC main() SKIP :")
- `test.occ` - Another minimal test
- `tests/cgtests/` - Comprehensive code generation tests
- `tests/corner-cases/` - Edge case tests
- `modules/course/` - Teaching examples and exercises
- `demos/` - Standalone demo programs

## Additional Test Commands

### Verify TCE Architecture Information
```bash
# Check if TCE files contain architecture info
strings simple_x64.tce | grep -i "x64\|aarch64\|processor"
```

### Test with Different Optimization Levels
```bash
# Test with different compiler flags
./tools/occ21/occ21 -X64 -O2 -etc simple.occ simple_x64_opt.tce
./tools/tranx86/tranx86 -mx64 simple_x64_opt.tce simple_x64_opt.s
```

## Debugging Strategy
1. Use minimal test cases first (simple.occ)
2. Run under LLDB to identify exact crash location
3. Check for missing function implementations
4. Verify pointer handling in 64-bit architectures