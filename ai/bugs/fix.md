# KRoC 64-bit Porting Progress: INT64 and Range Errors

## Achievements So Far

1. **Resolved Range Errors (The `cgtest22` crashes):**
   - **Issue**: The `I_MINT` instruction (which loads the most negative integer) loaded `0x80000000` into a 64-bit register as an unsigned value (`0x0000000080000000`). When the Occam test suite performed operations like `NOT MOSTNEG` or `< MOSTNEG`, it failed because the 64-bit hardware treated it as a positive number.
   - **Fix**: Modified `I_MINT` in `tools/tranx86/etcrtl.c` to explicitly cast `0x80000000` to a signed 32-bit integer first, ensuring it correctly sign-extends to `0xFFFFFFFF80000000` on 64-bit platforms. This fixed the `B1333` bounds checks and eliminated the range error crashes.

2. **Resolved `I_CWORD` Semantics for 64-bit Platforms:**
   - **Issue**: The `I_CWORD` instruction checks if a value fits within a 32-bit signed integer. The original Transputer algorithm `(value + MOSTNEG) < (MOSTNEG << 1)` relies on 32-bit integer wrapping, which fails completely when evaluated in 64-bit registers.
   - **Fix**: Rewrote the 64-bit path for `I_CWORD` in `etcrtl.c`. It now natively sign-extends the lower 32 bits of the value to 64 bits (`INS_SIGNEXT32`), and compares it to the original 64-bit value. If they are equal, the value safely fits in a 32-bit signed integer.

3. **Resolved `I_SHL` and `I_SHR` Shift Limits:**
   - **Issue**: The shift bounds check in `etcrtl.c` was hardcoded to limit shifts to `32`, even on 64-bit targets.
   - **Fix**: Updated the `max_shift` check to dynamically use `BytesPerWord * 8`, allowing `INT64` shifts up to 64 on AArch64.

4. **Added Native `I_SXT32` and `I_LW`/`I_SW` Support:**
   - Introduced a new `I_SXT32` instruction to explicitly sign-extend 32-bit integers to 64 bits when loading them from memory, preventing false positives in 64-bit comparisons.
   - Updated `I_LW` and `I_SW` to correctly use `ldrsw` and `str` with 32-bit `w` registers on AArch64, ensuring memory layout compatibility with the standard C interface.

## Current State

The `cgtest22` (INT64 arithmetic) test suite now completes execution without crashing! Out of 771 tests, 770 tests pass perfectly.

**The single remaining failure is:**
```text
I64-R64-1 Failed: #80000000#00000000  #40140000#00000000
```
- **Analysis**: This test converts an `INT64` (value `5`) to a `REAL64` (value `5.0`).
- **Expected**: `#40140000#00000000` (which is the IEEE 754 representation of `5.0`).
- **Actual**: `#80000000#00000000` (which is `-0.0`).
- **Root Cause**: The compiler emits a special `SPE_I64TOREAL` op, which `tranx86` translates into `INS_FILD64`. I implemented `INS_FILD64` to load the 64-bit integer from a memory address into `x17` and run `scvtf d0, x17`. The fact that it returns `-0.0` suggests that either the memory pointer passed to `INS_FILD64` is offset incorrectly, or it is reading garbage/uninitialized memory instead of the actual `INT64` value.