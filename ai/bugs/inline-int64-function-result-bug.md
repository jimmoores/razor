# Inline INT64 Function Result Not Stored Before I64TOREAL Conversion

## Summary

When an INT64 function result is used directly in a `REAL64 ROUND` conversion
(without first storing to a variable), the I64TOREAL conversion reads from a
workspace slot that contains the function call's return address instead of the
INT64 value. This produces garbage REAL64 results.

The bug only manifests when the INT64 expression is an inline function call.
Storing the function result to a variable first and then converting works correctly.

## Reproducer

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
  PROC show64(CHAN BYTE out!, VAL REAL64 v)
    VAL []INT w RETYPES v :
    SEQ
      hex8(out, w[1])
      hex8(out, w[0])
  :
  PROC msg(CHAN BYTE out!, VAL []BYTE s)
    SEQ i = 0 FOR SIZE s
      out ! s[i]
  :
  INT64 FUNCTION INT64.fn(VAL INT x) IS INT64 x :
  INT FUNCTION id(VAL INT x) IS x :
  REAL64 local.i :
  INT int.var :
  INT64 i64.var :
  SEQ
    int.var := id(77)

    -- WORKS: store to variable first, then convert
    i64.var := INT64.fn(int.var)
    local.i := REAL64 ROUND i64.var
    msg(scr, "stored=")
    show64(scr, local.i)
    scr ! '*n'

    -- BROKEN: inline function result directly in conversion
    local.i := REAL64 ROUND INT64.fn(int.var)
    msg(scr, "inline=")
    show64(scr, local.i)
    scr ! '*n'
:
```

Build and run:
```bash
cd ~/razor/kroc64/kroc
./tools/kroc/kroc --in-tree . -I modules/inmoslibs/libsrc/forall \
    -L modules/inmoslibs/libsrc/forall -L runtime/ccsp \
    /tmp/test_i64_fn_bug.occ -o /tmp/test_i64_fn_bug
/tmp/test_i64_fn_bug
```

### Expected output
```
stored=4053400000000000
inline=4053400000000000
```
(Both should be 77.0 = `0x4053400000000000`)

### Actual output
```
stored=4053400000000000
inline=C3E0000000000000
```
The "stored" path gives 77.0 (correct). The "inline" path gives approximately
`-9.22e18` (garbage — close to `-2^63`, suggesting the conversion read a pointer
value instead of 77).

## Root Cause

The `I64TOREAL` special operation in `etcrtl.c` (line 489) expects the INT64
value to be at a **memory address**:

```c
add_to_ins_chain(compose_ins(INS_FILD64, 1, 0, ARG_REGIND, ts->stack->old_a_reg));
```

The `old_a_reg` should contain a **pointer** to the INT64 value in workspace.
The compiler generates `LDLP n` to produce this address.

### Working path (stored to variable first)

```
i64.var := INT64.fn(int.var)    -- function returns INT64 in register
                                -- compiler stores result to i64.var workspace slot
local.i := REAL64 ROUND i64.var -- LDLP points to i64.var (contains INT64 value)
                                -- I64TOREAL loads from i64.var → correct
```

### Broken path (inline)

```
local.i := REAL64 ROUND INT64.fn(int.var)
```

Generated AArch64 assembly:
```asm
sub  x28, x28, #32        ; AJW: allocate call frame
adr  x16, 0f              ; return address
str  x16, [x28]           ; store return address at ws[0]
b    L10                   ; call INT64.fn
0:                         ; return point — result in register x19
mov  x0, x28               ; LDLP 0: x0 = &ws[0]
ldr  x17, [x0, #0]        ; INS_FILD64: load from ws[0] = RETURN ADDRESS!
scvtf d0, x17              ; convert return address to REAL64 = garbage
```

The function call stores the return address at `ws[0]`. After the function
returns, the result is in register `x19` (constrained by `compose_aarch64_return`).
But the `I64TOREAL` conversion reads from `ws[0]` which still contains the
return address. The INT64 function result was **never stored** to `ws[0]`.

### Why INT32 works but INT64 doesn't

`REAL64 ROUND INT32.fn(x)` uses `I_FPI32TOR64` (opcode 0x98) which takes
the value from the eval stack register, not from a memory address. So it
reads the function result directly from the register.

`REAL64 ROUND INT64.fn(x)` uses `I64TOREAL` (SPE) which uses `INS_FILD64`
that requires a memory address. The compiler must store the INT64 value to
workspace before generating the `LDLP`/`INS_FILD64` sequence, but it
doesn't do this for inline function calls.

## Impact

- **cgtest10**: REAL64-45 (`REAL64 ROUND INT64.fn(int)`) and REAL32-45
  (`REAL32 ROUND INT64.fn(int)`) — 2 failures
- Any occam code that does `REAL64 ROUND <inline INT64 function call>`
  or `REAL32 ROUND <inline INT64 function call>`

## Files involved

- `tools/occ21/be/code1k.c` — generates the `I64TOREAL` SPE (line ~2993).
  The function `geni64tor` should ensure the INT64 value is stored to
  workspace before emitting `LDLP` + `I64TOREAL`.
- `tools/tranx86/etcrtl.c` — `I64TOREAL` handler (line 480-493) uses
  `INS_FILD64` with `ARG_REGIND` (memory address).
- `tools/tranx86/archaarch64.c` — `INS_FILD64` asm handler (line 5077)
  only handles `ARG_REGIND`, not `ARG_REG`.

## Possible fixes

1. **Compiler fix (preferred)**: In `geni64tor` (code1k.c), before emitting
   `I64TOREAL`, check if the INT64 value is in a register (from a function
   call or expression) and emit `STL` to store it to a temporary workspace
   slot first. This matches how the transputer would handle it (the eval
   stack value must be in memory for FILD64).

2. **Translator fix (alternative)**: Modify `I64TOREAL` in etcrtl.c to
   handle the case where `old_a_reg` holds the INT64 value directly (not
   an address). Store it to workspace first, then pass the address to
   `INS_FILD64`. Or modify `INS_FILD64` to accept `ARG_REG` and emit
   `scvtf d0, xN` directly without the memory load.

3. **AArch64 asm fix (simplest)**: Add `ARG_REG` handling to the
   `INS_FILD64` case in `archaarch64.c`. If the input is a register
   (not a memory address), emit `scvtf d0, xN` directly:
   ```c
   } else if (mode == ARG_REG) {
       fprintf(stream, "\tscvtf\td0, %s\n",
           aarch64_get_register_name(ins->in_args[0]->regconst));
       aarch64_fp_from_i64 = 1;
   }
   ```
   However, this won't work because the compose always passes `ARG_REGIND`
   — the fix needs to be upstream in `etcrtl.c` or `code1k.c`.
