
Notes about invoking tools:
occ21 is invoked with -AARCH64 for aarch64 output and -X64 for x64 compatible output.
tranx86 is invoked with -maarch64 for aarch64 output and -mx64 for x64 output.
Testing Results:

This is for historical context, issues since at least partly resolved.

occ21 compilation: ✅ Successfully compiles occam to TCE bytecode with aarch64 target

tranx86 with aarch64: ❌ Segmentation fault - cannot generate any target code

tranx86 with x64: ❌ Segmentation fault - same issue as aarch64

tranx86 with x86: ✅ RTL generation works, ⚠️ assembly generation fails (but assertion error fixed)

Conclusion: The aarch64 target in tranx86 is not functional. While the architecture framework exists in archaarch64.c, it contains mostly stub implementations and segfaults immediately when used. The 64-bit support in general (both x64 and aarch64) appears incomplete and unstable in the current codebase.

The pointer truncation fix we applied earlier resolved the basic segfault issue, but the 64-bit architecture implementations themselves need significant development work to become functional.



@
Active file
