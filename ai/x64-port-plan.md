# x64 Port Implementation Plan

## Register Convention (MUST be consistent between tranx86 and CCSP runtime)

### Physical Register Numbering (x64)
```
 0 = rax    4 = rsp     8 = r8     12 = r12
 1 = rcx    5 = rbp     9 = r9     13 = r13
 2 = rdx    6 = rsi    10 = r10    14 = r14
 3 = rbx    7 = rdi    11 = r11    15 = r15
```

### Special Register Mappings (tranx86 REG_* constants to physical)
```
REG_WPTR  (-2) -> r14 (14)  Workspace Pointer (current process state)
REG_FPTR  (-3) -> r13 (13)  Front Pointer (run queue head)
REG_BPTR  (-4) -> r12 (12)  Back Pointer (run queue tail)
REG_SCHED (-8) -> r15 (15)  Scheduler Pointer (access to calltable, cparam)
REG_SPTR  (-5) -> rsp  (4)  Hardware stack pointer
REG_CC    (-6) -> rflags     Condition codes (virtual, no physical reg)
REG_CA    (-7) -> (virtual)  Carry-out (virtual, no physical reg)
```

### Rationale
- r12-r15 are callee-saved in System V AMD64 ABI, so they survive C function calls
- This matches the aarch64 pattern where runtime registers use callee-saved regs (x25-x28)
- i386 uses ESI for both SCHED and FPTR (register pressure); x64 has enough regs to separate them

### Allocatable Registers (for register colouring)
```
rax  (0)  - scratch, function return value
rcx  (1)  - scratch, 4th argument
rdx  (2)  - scratch, 3rd argument  
rbx  (3)  - callee-saved, general purpose
rsi  (6)  - scratch, 2nd argument
rdi  (7)  - scratch, 1st argument
r8   (8)  - scratch, 5th argument
r9   (9)  - scratch, 6th argument
r10  (10) - scratch
r11  (11) - scratch
Total: 10 allocatable registers (vs 4 on i386, 25 on aarch64)
```

### Reserved Registers (NOT available for allocation)
```
rsp  (4)  - hardware stack pointer
rbp  (5)  - frame pointer (reserved for C ABI interop)
r12  (12) - BPTR
r13  (13) - FPTR
r14  (14) - WPTR
r15  (15) - SCHED
```

### Sub-register Names (needed for assembly output)
```
64-bit:  rax  rcx  rdx  rbx  rsp  rbp  rsi  rdi  r8   r9   r10  r11  r12  r13  r14  r15
32-bit:  eax  ecx  edx  ebx  esp  ebp  esi  edi  r8d  r9d  r10d r11d r12d r13d r14d r15d
16-bit:  ax   cx   dx   bx   sp   bp   si   di   r8w  r9w  r10w r11w r12w r13w r14w r15w
8-bit:   al   cl   dl   bl   spl  bpl  sil  dil  r8b  r9b  r10b r11b r12b r13b r14b r15b
```

## Calling Conventions

### CCSP Kernel Calls (tranx86-generated code -> C kernel)
The System V AMD64 ABI is used (no regparm on x64):
```
param0 -> rdi  (1st argument)
sched  -> rsi  (2nd argument) 
Wptr   -> rdx  (3rd argument)
return -> rax  (return value)
```

### Kernel Call Marshalling (in compose_x64_kcall)
1. Store extra arguments (beyond param0) in sched->cparam[i-1]
2. Constrain param0 register to rdi
3. Move SCHED (r15) to rsi
4. Move WPTR (r14) to rdx
5. Load calltable entry: call *offsetof(sched_t,calltable[call])(r15)
6. On return, results come back in rax and sched->cparam[]

### External C Calls (occam calling C functions)
System V AMD64 ABI:
```
Arguments: rdi, rsi, rdx, rcx, r8, r9 (then stack)
Return: rax (integer), xmm0 (float)
Caller-saved: rax, rcx, rdx, rsi, rdi, r8-r11
Callee-saved: rbx, rbp, r12-r15
Stack: 16-byte aligned before call
```

### CIF Calls (C calling into occam kernel)
Wptr passed in rdi (1st C argument), sched loaded from Wptr[SchedPtr].

## CCSP Runtime Changes

### x64/sched_asm_inserts.h (COMPLETE REWRITE)
Current state: Copy of i386 with regparm(3) and 32-bit assembly.
Required changes:
- Remove `__attribute__((regparm(3)))` (no-op on x64 but misleading)
- Add `__attribute__((noinline))` (like aarch64) for reliable return address capture
- K_CALL_HEADER: use `__builtin_return_address(0)` (reliable on x64 unlike aarch64)
- K_ENTRY: use 64-bit registers (movq, %rsp)
- K_ZERO_OUT_JRET: use 64-bit registers, 8-byte Iptr offset
- All CIF macros: rewrite for x64 register conventions

### K_ENTRY macro (x64)
```c
#define K_ENTRY(init,stack,Wptr,Fptr) \
    __asm__ __volatile__ ( \
        "movq %0, %%rsp\n\t" \
        "movq %0, %%rdi\n\t" \
        "movq %2, %%rsi\n\t" \
        "movq %1, %%rdx\n\t" \
        "callq *%3\n\t" \
        : /* no outputs */ \
        : "r" ((unsigned long)(stack)), "r" ((unsigned long)(Wptr)), \
          "r" ((unsigned long)(Fptr)), "r" (init) \
        : "memory", "rdi", "rsi", "rdx", "rcx", "r8", "r9", \
          "r10", "r11", "rax", "cc")
```

### K_ZERO_OUT_JRET macro (x64)
```c
#define K_ZERO_OUT_JRET() \
    do { \
        TRACE_RETURN(Wptr[Iptr]); \
        __asm__ __volatile__ ( \
            "movq %0, %%r14\n\t" \
            "movq %1, %%r15\n\t" \
            "movq (%%r15), %%rsp\n\t" \
            "jmpq *-8(%%r14)\n\t" \
            : /* no outputs */ \
            : "r" (Wptr), "r" (sched) \
            : "memory"); \
    } while (0)
```

### x64/asm_ops.h
Current state: Copy of i386 with 32-bit assembly (bsf/bsr use unsigned int).
Required changes:
- Use 64-bit registers for serialise() (rax instead of eax)
- Use SSE2 barriers (mfence/lfence/sfence) - always available on x64
- Fix pick_random_bit to use 64-bit rdtsc output
- Fix xmemcpy to use 64-bit registers

### x64/atomics.h 
Current state: Uses __atomic_* GCC builtins - already correct for 64-bit.
No changes needed.

### x64/alignment.h
Current state: CACHELINE_WORDS=8 (correct for 64-bit, 8 bytes/word, 64-byte cacheline).
No changes needed.

### x64/ccsp_types.h
Current state: Disables _PACK_STRUCT for 64-bit safety.
No changes needed.

## tranx86 archx64.c Changes (COMPLETE REWRITE)

### File Structure (following archaarch64.c pattern)
1. Includes and constants (~100 lines)
2. Register definitions and helpers (~200 lines)
3. Symbol name handling (~150 lines)
4. Large immediate helpers (~50 lines)
5. Kernel call functions (~300 lines)
   - compose_x64_kcall
   - compose_x64_kjump
   - compose_x64_deadlock_kcall
6. Inline scheduler operations (~600 lines)
   - compose_x64_inline_quick_reschedule
   - compose_x64_inline_full_reschedule
   - compose_x64_inline_startp
   - compose_x64_inline_endp
   - compose_x64_inline_stopp
   - compose_x64_inline_runp
7. Channel I/O operations (~600 lines)
   - compose_x64_inline_in / in_2
   - compose_x64_inline_out / out_2
   - compose_x64_inline_min / mout
8. ALT operations (~400 lines)
   - compose_x64_inline_enbc
   - compose_x64_inline_disc
   - compose_x64_inline_altwt
9. Timer operations (~200 lines)
   - compose_x64_inline_ldtimer
   - compose_x64_inline_tin
10. Memory operations (~100 lines)
    - compose_x64_inline_stlx
    - compose_x64_inline_malloc
11. External call functions (~400 lines)
    - compose_x64_external_ccall
    - compose_x64_bcall
    - compose_x64_cif_call
12. Code generation (~500 lines)
    - compose_x64_entry_prolog
    - compose_x64_return / nreturn
    - compose_x64_funcresults
    - compose_x64_move / move_loadptrs
    - compose_x64_shift
    - compose_x64_division / remainder
    - compose_x64_divcheck_zero
    - compose_x64_widenshort / widenword
    - compose_x64_iospace_* (6 functions)
13. Long operations (~400 lines)
    - compose_x64_longop (all I_L* opcodes)
14. Floating point operations (~500 lines)
    - compose_x64_fpop (all I_FP* opcodes)
    - compose_x64_fp_set_fround
    - compose_x64_fp_init
    - compose_x64_reset_fregs
15. Debugging functions (~300 lines)
    - compose_x64_debug_insert
    - compose_x64_debug_procnames / filenames
    - compose_x64_debug_zero_div / floaterr / overflow / rangestop / seterr
    - compose_x64_overflow_jumpcode / floaterr_jumpcode / rangestop_jumpcode
    - compose_x64_debug_deadlock_set
16. Miscellaneous (~200 lines)
    - compose_x64_refcountop
    - compose_x64_memory_barrier
    - compose_x64_pre_enbc / pre_enbt
17. Register allocation (~100 lines)
    - x64_regcolour_special_to_real
    - x64_regcolour_get_regs
    - x64_regcolour_fp_regs
18. Assembly output engine (~1500 lines)
    - x64_code_to_asm / x64_code_to_asm_stream
    - x64_get_register_name (all widths)
    - x64_modify_name (symbol mangling)
    - x64_drop_arg (operand output)
    - x64_disassemble_code (instruction output)
    - x64_disassemble_data
19. RTL validation (~100 lines)
    - x64_rtl_validate_instr
    - x64_rtl_prevalidate_instr
20. Init function and arch_t struct (~50 lines)

### Estimated total: ~5500-6000 lines

## Key Implementation Notes

### x64 vs i386 Differences
1. **Word size**: 8 bytes (WSH=3) vs 4 bytes (WSH=2)
2. **Pointer size**: 8 bytes vs 4 bytes
3. **Iptr offset**: Wptr[-1] = Wptr - 8 bytes (not -4)
4. **Register width**: All default operations use 64-bit (movq, addq, etc.)
5. **Calling convention**: System V ABI (args in rdi,rsi,rdx,rcx,r8,r9) vs regparm(3)
6. **Stack alignment**: 16-byte aligned before call (required by ABI)
7. **RIP-relative addressing**: Available and preferred for position-independent code
8. **REX prefix**: Needed for r8-r15 access (handled by assembler)
9. **Immediate size**: movq can load 64-bit immediates; most others limited to 32-bit sign-extended
10. **Division**: idivq uses rdx:rax / operand -> rax (quotient), rdx (remainder)

### x64 vs aarch64 Similarities
1. Same word size (8 bytes)
2. Same WSH (3)
3. Same workspace layout (offsets scaled by 8)
4. Both need large immediate helpers (aarch64: movz/movk, x64: movabs)
5. Both use callee-saved registers for runtime pointers
6. Both need symbol name cleanup functions

### Assembly Syntax (AT&T, matching i386)
- Source, Destination order: `movq %rax, %rbx` (rax -> rbx)
- Register prefix: %
- Immediate prefix: $
- Memory: offset(%base, %index, scale) e.g., -8(%r14)
- Instruction suffix: q (64-bit), l (32-bit), w (16-bit), b (8-bit)

### Symbol Naming (must match arch386.c/modify_name)
Follow the same mangling rules as i386:
- Dot conversion: . -> _
- Auto-prefix: O_ (standard), E_ (entrypoint ^), M_ (module *)
- Exception: no prefix for _, &, @, O_, DCR_ prefixes
- Platform prefix: options.extref_prefix prepended when O_/E_/M_ added
- C. calls: skip 2 chars, prepend extref_prefix
- B./BX. calls: skip 1 or 2 chars, prepend extref_prefix
- CIF. calls: skip 4 chars, prepend @extref_prefix (@ stripped later)

## Build System Changes

### tools/tranx86/Makefile.am
- Already includes archx64.c and archx64.h
- No changes needed unless asmx64.c is created as separate file

### tools/tranx86/configure.ac
- Currently sets ARCH_DEFS="-m32" for x86_64 host: REMOVE this
- x64 tranx86 should compile as native 64-bit binary

### runtime/ccsp/configure.ac
- Already detects x86_64 and sets TARGET_CPU_X64
- May need AM_CONDITIONAL([CCSP_X64]) if x64-specific .S files needed

### m4/kroc.m4 (KROC_CCSP_FLAGS)
- Currently has -m32 fallback for x64: should default to 64-bit
- Verify HAVE_IA32_SSE2 is defined for x64 (always has SSE2)

## Testing Strategy

### Phase 1: Minimal SKIP program
```
occ21 -X64 -etc simple.occ -o simple_x64.tce
tranx86 -mx64 -s simple_x64.tce -o simple_x64.s
# Inspect assembly manually
as simple_x64.s -o simple_x64.o
# Link against CCSP (once runtime is rebuilt)
```

### Phase 2: Simple output
Test programs that write single characters to stdout via channel communication.

### Phase 3: CIF tests
modules/cif/examples/cift1 through cift15

### Phase 4: cgtest suite
tests/cgtests/cgtest00 through cgtest96

## Workspace Layout Reference (64-bit)
```
Offset from Wptr (bytes):
+16  = Wptr[+2]  SavedPriority
+8   = Wptr[+1]  Count / IptrSucc
 0   = Wptr[0]   Temp
-8   = Wptr[-1]  Iptr (instruction pointer / resume address)
-16  = Wptr[-2]  Link (next process in queue)
-24  = Wptr[-3]  Priofinity (priority/affinity)
-32  = Wptr[-4]  Pointer / State
-40  = Wptr[-5]  TLink (timer queue link)
-48  = Wptr[-6]  Time_f (time field)
-56  = Wptr[-7]  SchedPtr / StackPtr (for CIF)
-64  = Wptr[-8]  BarrierPtr (for CIF)
-72  = Wptr[-9]  EscapePtr (for CIF)
```

## Calltable Access
The kernel calltable is accessed via the SCHED pointer (r15):
```asm
# Call kernel function K_FOO:
movq offsetof(sched_t, calltable[K_FOO])(%r15), %rax
callq *%rax
# Or combined:
callq *offsetof(sched_t, calltable[K_FOO])(%r15)
```

## NotProcess_p and Constants
```
NotProcess_p = 0
MostNeg = 0x8000000000000000 (64-bit)
NoneSelected_o = MostNeg
TimeSet_p = 1
TimeNotSet_p = 2
```
