# Fix Plan: tranx86 and sdltest

## 1. Fix tranx86 register lifetime tracing
- Problem: `tranx86` register allocator and optimizer fail when a virtual register lifetime spans across multiple `RTL_CODE` blocks. This happens in complex programs like `butterflies.occ` where code blocks are split by in-line data or labels.
- Solution: Modernize `rtl_scan_*` functions in `rtlops.c` to correctly follow the execution flow across consecutive or non-consecutive `RTL_CODE` blocks.
- Specifically:
    - Update `rtl_scan_start_forward`, `rtl_scan_end_forward`, `rtl_scan_constrain_forward`, and `rtl_scan_unconstrain_forward`.
    - Update the verification loop in `rtl_trace_regs`.
    - Ensure `codeblock_reg_squeeze` in `optimise.c` either handles cross-block lifetimes or safely skips them.

## 2. Investigate and fix sdltest crash
- Problem: `sdltest` and other programs crash or segfault on AArch64.
- Hypothesis: Continued `word` size mismatches between Occam and C components.
- Actions:
    - Check all hand-written C components in `modules/occSDL` and `modules/sdlraster` for 32-bit assumptions (e.g., `int w[]`).
    - Verify that `occ.SDL.blit.raster` and other helper functions use correct types.
    - Check if `sdlraster.occ`'s use of `MOBILE` arrays is compatible with the 64-bit runtime's representation.

## 3. Butterflies Compilation
- Once `tranx86` is fixed, `butterflies.occ` should compile. Verify it and then test its execution.
