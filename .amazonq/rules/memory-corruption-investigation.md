# Memory Corruption Investigation

## Current Status
Segfault occurs in rtl_remove_deadnamelabels() at rtlops.c:838 during strcmp() call.
Address 0x23c0180 indicates memory corruption, not NULL pointer.

## Stack Trace Analysis
```
* frame #0: 0x0000000196277200 libsystem_platform.dylib`_platform_strcmp$VARIANT$Base + 144
  frame #1: 0x000000010001fe04 tranx86`rtl_remove_deadnamelabels(rtl_code=0x00006000034c4040) at rtlops.c:838:60
  frame #2: 0x0000000100001b34 tranx86`tranx86 at main.c:935:10
  frame #3: 0x0000000100001b18 tranx86`main at main.c:813:6
```

## Root Cause Hypothesis
The corrupt pointer (0x23c0180) suggests:
1. Use-after-free of string memory
2. Pointer truncation from 64-bit to 32-bit storage
3. Memory corruption in x64/aarch64 architecture code

## Investigation Focus
- **DO NOT** wrap strcmp() or add NULL checks
- **DO** find where corrupt string pointers originate
- Check ARG_NAMEDLABEL and ARG_TEXT argument creation in x64/aarch64 code
- Verify string_dup() calls and memory management
- Look for pointer size mismatches in regconst field

## Key Areas to Investigate
1. compose_ins() calls in archx64.c and archaarch64.c with ARG_NAMEDLABEL
2. String memory allocation in x64/aarch64 architecture functions
3. Pointer storage in ins_arg->regconst field (int vs pointer size)
4. Memory management in string_dup() and related functions