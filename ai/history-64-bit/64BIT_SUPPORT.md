# 64-bit Architecture Support for KRoC

This document describes the new 64-bit architecture support added to the KRoC occam compiler system.

## Overview

The KRoC system has been extended to support 64-bit target architectures, specifically:
- x86-64 (x64)
- ARM64/AArch64

## Changes Made

### 1. Compiler (occ21) Changes

- **gen2.c**: Added 64-bit pointer size support and INT64 check masks
- Added `TARGET_64BIT` preprocessor flag support
- Updated pointer type handling for 64-bit targets

### 2. Translator (tranx86) Changes

- **machine.h**: Added new target definitions for x64 and aarch64
- **main.c**: Added command-line support for new architectures (`-mx64`, `-maarch64`)
- **archx64.c/h**: New x64 architecture implementation
- **archaarch64.c/h**: New aarch64 architecture implementation
- **Makefile.am**: Updated to include new source files

### 3. Build System Changes

- **kroc.m4**: Removed hardcoded 32-bit flags, added `--enable-64bit` option
- **configure.ac**: Updated to support new architectures

## Usage

### Compiling for x86-64

```bash
# Enable 64-bit mode during configure
./configure --enable-64bit

# Use tranx86 with x64 target
tranx86 -mx64 input.tce -o output.o
```

### Compiling for AArch64

```bash
# Configure for aarch64 target
./configure --host=aarch64-linux-gnu --enable-64bit

# Use tranx86 with aarch64 target
tranx86 -maarch64 input.tce -o output.o
```

### Backward Compatibility

The system maintains backward compatibility:
- Default behavior remains 32-bit compilation on x86_64 systems
- Existing 32-bit code continues to work unchanged
- Use `--enable-64bit` configure flag to enable 64-bit support

## Architecture Implementation Status

### x86-64 (x64)
- ✅ Basic architecture framework
- ⚠️  Minimal implementation (stubs for most functions)
- 🔄 Needs full code generation implementation

### AArch64
- ✅ Basic architecture framework  
- ⚠️  Minimal implementation (stubs for most functions)
- 🔄 Needs full code generation implementation

## Next Steps

1. **Complete Code Generation**: Implement full code generation for x64 and aarch64
2. **Runtime Support**: Update CCSP runtime for 64-bit pointer handling
3. **Testing**: Comprehensive testing on 64-bit targets
4. **Optimization**: Architecture-specific optimizations

## Testing

A basic test program `test64.occ` is provided to verify 64-bit ETC code generation:

```occam
PROC test64 ()
  INT64 x, y, z:
  SEQ
    x := 42
    y := 1000000000000
    z := x + y
:
```

