# AArch64 Target Testing Results

## Test Setup
- Compiled simple occam program: `PROC simple () SKIP :`
- Used occ21 with T805 target to generate TCE file
- Tested tranx86 with different architecture targets

## Results

### x86 Target (-m386)
- ✅ RTL generation works (`-r0` option)
- ❌ Assembly generation fails with assertion: `(sizeof(codes) == sizeof(code_sizes)) (960 != 480) failed`

### x64 Target (-mx64)  
- ❌ Segmentation fault during RTL generation
- ❌ Segmentation fault during assembly generation

### AArch64 Target (-maarch64)
- ❌ Segmentation fault during RTL generation  
- ❌ Segmentation fault during assembly generation

## Analysis

1. **64-bit targets are unstable**: Both x64 and aarch64 segfault, indicating incomplete or buggy implementations
2. **AArch64 code exists but is mostly stubs**: The archaarch64.c file contains basic structure but many functions are stubs
3. **Code table mismatch in x86**: The assertion failure suggests internal inconsistencies in instruction tables

## Conclusion

The aarch64 target is **not functional** in its current state. While the architecture framework exists, it segfaults immediately and cannot generate any target code. The implementation appears to be incomplete stub code that needs significant development work to become functional.

The x64 target also has similar issues, suggesting that 64-bit support in general is incomplete in the current codebase.