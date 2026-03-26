# Register Allocator Operand Order Bug on AArch64

## Summary

On AArch64, the checked subtraction instruction `I_SUB` (occam `-` operator) produces
`Places - Xexp` instead of `Xexp - Places` in specific code patterns involving many local
variables and a preceding 3-output kernel call (NORMALISE). The unchecked subtraction
`I_DIFF` (occam `MINUS` operator) produces the correct result in the same context.

This bug is the root cause of the cgtest19 FP emulation failures, cgtest10 REAL64/REAL32
conversion failures, and likely cgtest14 extended type function failures. It affects ANY
occam code that uses checked subtraction after a multi-output predefined function
(NORMALISE, SHIFTLEFT, SHIFTRIGHT, LONGDIV) in a context with many live variables.

## Reproducer

File: `/tmp/test_nc2.occ` (also saved below)

```occam
PROC main(CHAN BYTE kyb?, scr!, err!)
  PROC hex8(CHAN BYTE out!, VAL INT v)
    SEQ i = 0 FOR 8
      SEQ
        VAL INT nibble IS (v >> ((7-i)*4)) /\ #F :
        IF
          nibble < 10
            out ! BYTE(nibble + (INT '0'))
          TRUE
            out ! BYTE((nibble - 10) + (INT 'A'))
  :
  PROC msg(CHAN BYTE out!, VAL []BYTE s)
    SEQ i = 0 FOR SIZE s
      out ! s[i]
  :
  INT FUNCTION id(VAL INT x) IS x :
  INT Xexp, Xfrac, Yexp, Yfrac, Carry, Guard, Places, Ans :
  SEQ
    Xexp := id(100)
    Xfrac := id(#40000000)
    Yexp := id(50)
    Yfrac := id(0)
    Carry := id(0)
    Guard := id(0)
    Ans := id(0)
    Places := id(0)
    IF
      Xexp > 0
        SEQ
          Carry := 1
          Guard := 1
      TRUE
        SKIP
    #PRAGMA DEFINED Ans
    IF
      Carry = 0
        SKIP
      TRUE
        SEQ
          Places := Xexp - Yexp
          IF
            Places > 32
              Yfrac := 0
            Places > 0
              SKIP
            TRUE
              SKIP
          Carry, Guard := LONGDIFF(0, Guard, 0)
          Places, Ans := LONGDIFF(Xfrac, Yfrac, Carry)
          Xfrac := Ans
          Ans := 0
          IF
            Xfrac = 0
              Xexp := 0
            Xexp > 1
              SEQ
                msg(scr, "Pre-NORM: Xe=")
                hex8(scr, Xexp)
                msg(scr, " Xf=")
                hex8(scr, Xfrac)
                msg(scr, " G=")
                hex8(scr, Guard)
                scr ! '*n'
                Places, Xfrac, Guard := NORMALISE(Xfrac, Guard)
                msg(scr, "Post-NORM: P=")
                hex8(scr, Places)
                msg(scr, " Xe=")
                hex8(scr, Xexp)
                scr ! '*n'
                Xexp := Xexp - Places
                msg(scr, "After sub: Xe=")
                hex8(scr, Xexp)
                scr ! '*n'
            TRUE
              Xexp := 0
:
```

Build and run:
```bash
cd ~/razor/kroc64/kroc
./tools/kroc/kroc --in-tree . -I modules/inmoslibs/libsrc/forall \
    -L modules/inmoslibs/libsrc/forall -L runtime/ccsp \
    /tmp/test_nc2.occ -o /tmp/test_nc2
/tmp/test_nc2
```

### Expected output
```
Pre-NORM: Xe=00000064 Xf=3FFFFFFF G=FFFFFFFF
Post-NORM: P=00000002 Xe=00000064
After sub: Xe=00000062
```
(100 - 2 = 98 = 0x62)

### Actual output
```
Pre-NORM: Xe=00000064 Xf=3FFFFFFF G=FFFFFFFF
Post-NORM: P=00000002 Xe=00000064
After sub: Xe=FFFFFF9E
```
(2 - 100 = -98 = 0xFFFFFF9E — operands are swapped)

### Verification: MINUS works correctly

Replacing `Xexp := Xexp - Places` with `Xexp := Xexp MINUS Places` (unchecked subtraction,
`I_DIFF` instead of `I_SUB`) produces the correct result `0x62` (98).

## Root Cause Analysis

### The transputer eval stack convention

The transputer SUB instruction computes `Breg - Areg` (second-on-stack minus top-of-stack).
For `Xexp - Places`: the compiler should push Xexp first (becomes Breg), then Places
(becomes Areg), then SUB gives `Breg - Areg = Xexp - Places`.

### What actually happens

Looking at the generated AArch64 assembly for the reproducer, after NORMALISE:

```asm
; NORMALISE result stores:
str  x0, [x28, #24]    ; Guard  → ws[3]
str  x1, [x28, #32]    ; Xfrac  → ws[4]
str  x2, [x28, #16]    ; Places → ws[2]

; ... debug output code ...

; Subtraction Xexp - Places:
ldr  x0, [x28, #16]    ; load ws[2] = Places  ← loaded FIRST (becomes Breg)
ldr  x17, [x28, #8]    ; load ws[1] = Xexp    ← loaded SECOND (becomes Areg)
subs x17, x0, x17      ; x17 = x0 - x17 = Places - Xexp  ← WRONG!
str  x17, [x28, #8]    ; store to ws[1] = Xexp
```

The register allocator loads **Places before Xexp**, putting Places in Breg and Xexp in
Areg. The SUB then computes `Breg - Areg = Places - Xexp` — the opposite of what's needed.

### Comparison with I_DIFF (which works correctly)

Using `MINUS` (I_DIFF, unchecked) instead of `-` (I_SUB, checked):

```asm
ldr  x0, [x28, #8]     ; load ws[1] = Xexp    ← loaded FIRST (Breg)
ldr  x1, [x28, #16]    ; load ws[2] = Places   ← loaded SECOND (Areg)
sub  x0, x0, x1         ; x0 = Xexp - Places    ← CORRECT
```

The loads are in the **correct order** for I_DIFF.

### The compose-level code

Both `I_SUB` and `I_DIFF` use the same compose function with the same operand order:

```c
// I_SUB (checked) — etcrtl.c line 4624:
generate_constmapped_21instr(ts, EtcSecondary(I_SUB), INS_SUB,
    ts->stack->old_a_reg, ts->stack->old_b_reg, ts->stack->a_reg, 1);

// I_DIFF (unchecked) — etcrtl.c line 4688:
generate_constmapped_21instr(ts, EtcSecondary(I_DIFF), INS_SUB,
    ts->stack->old_a_reg, ts->stack->old_b_reg, ts->stack->a_reg, 0);
```

The **only difference** is the last argument (`usecc`): 1 for I_SUB, 0 for I_DIFF.
With `usecc=1`, the compose adds `ARG_REG | ARG_IMP, REG_CC` as an implied output
(2 outputs instead of 1). With `usecc=0`, there is only 1 output.

### What was ruled out

1. **REG_CC → AARCH64_REG_CC(32) mapping**: Changed to return REG_CC(-6) like x86. No effect.
2. **usecc=0 for I_SUB**: Changed I_SUB to use usecc=0 (no REG_CC output). The operand
   order was STILL wrong — the bug persists even without REG_CC. So REG_CC is NOT the cause.
3. **Pre-swapping operands**: Swapping in_args[0] and in_args[1] in the compose breaks
   basic subtraction (the allocator doesn't always swap — only in specific contexts).

### Key finding: the bug is context-dependent

- **Simple subtraction** (`a := id(100); b := id(30); c := a - b`): Works correctly.
  The allocator loads left operand first (Breg), right second (Areg).
- **Post-NORMALISE subtraction** with many live variables: Fails. The allocator loads
  right operand first (Breg), left second (Areg), WITHOUT emitting I_REV.

The bug manifests when:
1. There are 8+ local variables (Xexp, Xfrac, Yexp, Yfrac, Carry, Guard, Places, Ans)
2. A 3-output kernel call (NORMALISE) precedes the subtraction
3. Multiple IF branches with complex control flow
4. The checked subtraction `I_SUB` is used

## Impact

This bug affects the `IEEE32OP` and `IEEE64OP` functions in the inmoslibs
(`modules/inmoslibs/libsrc/forall/ie32op.occ` and `ie64op.occ`), which implement
software IEEE 754 floating-point arithmetic. These functions use `NORMALISE` followed
by `Xexp := Xexp - Places` in the normalize-and-pack step. The swapped subtraction
produces a negated exponent, which cascades into completely wrong FP results.

Specifically:
- **cgtest19**: 25 failures in REAL32OP/REAL64OP — software FP emulation gives garbage
- **cgtest10**: 2 failures in INT64→REAL conversion paths
- **cgtest14**: 4 failures in extended type function returns (likely same root cause)
- Any other complex occam FUNCTION using checked subtraction after NORMALISE

## Files involved

- `tools/tranx86/regcolour.c` — register colouring algorithm (likely source of bug)
- `tools/tranx86/etcrtl.c` — ETC-to-RTL translation (I_SUB and I_DIFF handlers, lines 4622-4627 and 4686-4691)
- `tools/tranx86/archaarch64.c` — AArch64 code emission (INS_SUB handler, line 3957+)
- `tools/tranx86/tstack.h` — eval stack tracking (REG_CC = -6)

## Suggested investigation approach

1. **Dump RTL before and after register colouring** for the reproducer, focusing on the
   I_SUB instruction and its input operands. Compare with I_DIFF for the same code.

2. **Trace the register colouring algorithm** for the specific context: which virtual
   registers are assigned to which physical registers, and why the load order differs
   between usecc=0 and usecc=1 (even though the final operand swap persists with usecc=0).

3. **Compare with x86 backend**: The x86 register allocator produces correct operand
   order for the same ETC code. The difference may be in how the AArch64 register set
   (25 general-purpose registers) vs x86 (6 registers) affects the allocation heuristics.

4. The RTL dump options `-ra0` through `-ra3` may help visualize the allocation stages.
   Use `./tools/tranx86/tranx86 -maarch64 -ra0 test.tce` to dump stage 0.
