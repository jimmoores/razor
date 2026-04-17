# Analysis: Refactoring the CCSP Kernel Interface to Standard C Calling Conventions

## 1. Executive Summary

This document analyses the feasibility of two related refactoring proposals for the CCSP runtime:

1. **Replace the custom argument-passing convention** (cparam memory block + calltable indirection) with standard C function calls.
2. **Unify the Workspace Pointer (Wptr) with the hardware stack pointer (SP)**, eliminating the dual-pointer model.

Both are feasible but represent significant architectural changes. The argument-passing refactoring is relatively straightforward and low-risk. The Wptr/SP unification is more complex due to the negative-offset thread metadata region and interactions with C code, but is achievable with careful design.

A `setjmp`/`longjmp`-style primitive approach for context switching is not only feasible but would dramatically simplify the architecture, reducing the per-architecture assembly to just two small routines.

---

## 2. Current Architecture

### 2.1 The Custom Calling Convention

Every kernel function currently has this signature:
```c
void kernel_Name(word param0, sched_t *sched, word *Wptr);
```

Arguments are passed as follows:
- **param0**: First argument in a standard register (rdi/x0)
- **sched**: Scheduler pointer in second register (rsi/x1)
- **Wptr**: Workspace pointer in third register (rdx/x2)
- **Additional args**: Written to `sched->cparam[0..4]` memory slots before the call

Return values work similarly:
- **First result**: Standard C return register (rax/x0)
- **Additional results**: Read back from `sched->cparam[]` after the call

The call is dispatched through a function pointer table (`sched->calltable[]`), which adds an indirection.

### 2.2 Historical Reasons for This Design

The original x86 (32-bit) CCSP used an even stranger convention: arguments were passed on the x86 stack, results returned on the stack, and the *caller* (not the callee) cleaned up the stack. This was because:

1. The Transputer hardware had no registers - everything was stack-based.
2. The original runtime was modelled on Transputer microcode, which operated on the hardware process workspace directly.
3. x86 had very few general-purpose registers (6 usable), making register-based calling impractical.

The "cparam memory block" approach was an improvement over the stack-based convention, designed to be easier to implement in C. But it was always intended as an interim solution (as noted in the project documentation: "long term is not optimal").

### 2.3 The Dual Pointer Model

Currently, two pointers are maintained simultaneously:

| Pointer | AArch64 | x64 | Purpose |
|---------|---------|-----|---------|
| Wptr | x28 | r14 | Process workspace - holds occam locals, args, and thread metadata at negative offsets |
| SP | sp | rsp | Hardware stack - used only during kernel calls for C function frames |

During **user code execution**: Wptr is the active "stack pointer" for the occam process. SP is essentially unused (or points to the kernel stack from the last kernel return). The generated code grows Wptr downward with `I_AJW` (adjust workspace) and accesses locals as positive offsets from Wptr.

During **kernel calls**: SP is switched to a dedicated kernel stack. The C kernel functions use SP normally. Wptr is passed as a parameter so the kernel can access process state.

### 2.4 The Inline Fast Path

The critical performance optimisation is the **inline quick reschedule**. When a process blocks (e.g., on a channel), the generated code can often avoid calling into the kernel entirely:

```
// Inline quick reschedule (x64 pseudocode)
if (Fptr != NotProcess) {
    Wptr = Fptr;                    // Switch to next process
    Fptr = Fptr[Link];             // Advance run queue head
    jump *Wptr[Iptr];             // Resume next process
} else {
    call kernel_K_PAUSE();         // No runnable process, enter scheduler
}
```

This runs entirely in user code, with no stack switch, no function call overhead, and no kernel involvement. It directly manipulates Fptr/Bptr (the run queue) and jumps to the next process's saved instruction pointer.

### 2.5 Workspace Negative Offsets (Thread Metadata)

The workspace has a dual nature:

```
Offset    Field        Purpose
------    -----        -------
+2        SavedPriority  Saved process priority
+1        Count          Reference count (PAR construct)
 0        Temp/IptrSucc  Temporary slot
-1        Iptr           Saved instruction pointer (return address)
-2        Link           Run queue link
-3        Priofinity     Priority + affinity bits
-4        Pointer/State  Channel data pointer / ALT state
-5        TLink          Timer queue link
-6        Time_f         Timer target value
-7        SchedPtr       Scheduler pointer (CIF only)
-8        BarrierPtr     Barrier pointer (CIF only)
-9        EscapePtr      Escape/exception pointer (CIF only)
-10..-24  (CIF regs)     Callee-saved register save area (64-bit)
```

The positive offsets are the process's "stack" - local variables and arguments. The negative offsets are the process's "thread descriptor" - metadata used by the scheduler.

This is exactly how a Transputer worked: the workspace pointer pointed to the base of local storage, and the microcode used negative offsets to access scheduling state. It is an elegant design in that the process descriptor and process stack are a single contiguous allocation.

---

## 3. Analysis: Standard C Calling Convention for Kernel Calls

### 3.1 What Would Change

Instead of:
```c
// Current: unified signature, extra args in memory
void kernel_Y_in(word param0, sched_t *sched, word *Wptr);
// Called with: param0=length, sched->cparam[0]=channel, sched->cparam[1]=buffer
```

We would have:
```c
// Proposed: standard C function with natural parameters
void kernel_Y_in(sched_t *sched, word *Wptr, word length, word *channel, void *buffer);
```

Each kernel function would have its own natural C signature. The calltable indirection would be replaced by direct function calls (the linker resolves the address at link time).

### 3.2 Feasibility: HIGH

**Arguments in favour:**
- Modern architectures (AArch64, x64) have plenty of registers for argument passing (8 on AArch64, 6 on x64). Even the most complex kernel calls take at most 5 arguments.
- The calltable indirection adds an extra memory load + indirect branch on every kernel call. Direct calls are cheaper and more branch-predictor-friendly.
- The `cparam[]` memory writes before the call add latency. Register-only calls avoid these stores entirely.
- The code in tranx86 already marshals arguments into registers *before* writing them to cparam - so the register values are already available; the memory write is redundant work.
- Standard C signatures make the kernel functions self-documenting and easier to debug.

**Concerns:**
- The calltable allowed dynamic dispatch and potential runtime patching. In practice, this is never used - the calltable is built once at init and never modified.
- The unified signature simplified the generic K_CALL_DEFINE macro system. With per-function signatures, each kernel entry point needs its own declaration. This is more code but more explicit.
- The x86 (32-bit) backend has only 6 GPRs and would struggle with >3 register arguments. However, x86 is the legacy target and could retain the old convention or use the standard cdecl convention (stack-based), which is actually what GCC expects on x86 anyway.

### 3.3 Impact on Components

| Component | Change Required | Difficulty |
|-----------|----------------|------------|
| `sched.c` (kernel functions) | Change K_CALL_DEFINE macros to standard signatures | Low |
| `calltable.h` | Remove calltable entirely, or keep as optional for legacy | Low |
| `sched_asm_inserts.h` | Simplify K_ENTRY/K_ZERO_OUT_JRET (see section 5) | Medium |
| `archaarch64.c` (tranx86) | Change kcall generation to emit direct `bl` with register args | Medium |
| `archx64.c` (tranx86) | Same | Medium |
| `arch386.c` (tranx86) | Could keep old convention or move to cdecl | Low-Medium |
| `ccsp_cif_stubs.h` | Simplify CIF stubs - no cparam writes needed | Medium |
| `kitable.h` | Remove K_* index definitions | Low |
| `sched_types.h` | Remove cparam[] and calltable[] from sched_t | Low |

### 3.4 Estimated Effort

This is a clean, mechanical refactoring:
1. Define new signatures for each kernel function (~100 functions)
2. Update tranx86 `compose_*_kcall` for each architecture (3 backends)
3. Update CIF stubs (auto-generated by make-header.py)
4. Remove calltable infrastructure
5. Test on all platforms

**Risk: Low-Medium.** The calling convention is well-understood and the change is local to the kernel interface boundary.

---

## 4. Analysis: setjmp/longjmp-Style Context Switching

### 4.1 The Insight

The core of CCSP's context switching boils down to two operations:

1. **Save**: Store the current execution context (registers, instruction pointer) and switch to another context.
2. **Restore**: Load a previously saved context and resume execution.

This is precisely what `setjmp`/`longjmp` do. But we can do better with a custom pair of primitives that are tailored to our needs.

### 4.2 Proposed Primitives

```c
// Save current context into the workspace, return 0.
// When restored, returns non-zero.
int ccsp_save_context(word *Wptr);

// Restore a previously saved context from workspace.
// The target process resumes from its ccsp_save_context call, 
// which now returns `resume_value`.
void ccsp_restore_context(word *Wptr, int resume_value) __attribute__((noreturn));
```

These would be small assembly routines (roughly 10-15 instructions each), one pair per architecture:

**AArch64 example:**
```asm
// ccsp_save_context: save callee-saved regs to Wptr negative offsets, return 0
ccsp_save_context:
    stp x19, x20, [x0, #-16*8]   // Save callee-saved registers
    stp x21, x22, [x0, #-14*8]
    stp x23, x24, [x0, #-12*8]
    stp x25, x26, [x0, #-10*8]
    stp x27, x28, [x0, #-8*8]
    stp x29, x30, [x0, #-6*8]
    mov x1, sp
    str x1, [x0, #-4*8]          // Save SP
    mov x0, #0                    // Return 0 (first time)
    ret

// ccsp_restore_context: restore regs from Wptr, return resume_value
ccsp_restore_context:
    ldp x19, x20, [x0, #-16*8]
    ldp x21, x22, [x0, #-14*8]
    ldp x23, x24, [x0, #-12*8]
    ldp x25, x26, [x0, #-10*8]
    ldp x27, x28, [x0, #-8*8]
    ldp x29, x30, [x0, #-6*8]
    ldr x2, [x0, #-4*8]
    mov sp, x2                    // Restore SP
    mov x0, x1                    // Return resume_value
    ret
```

### 4.3 How This Simplifies the Architecture

Currently, each architecture needs:
- `K_ENTRY` macro (stack switch + register setup)
- `K_ZERO_OUT_JRET` macro (register restore + indirect jump to Iptr)
- `K_CIF_PROC` / `K_CIF_ENDP_RESUME` macros (process creation trampolines)
- Inline assembly in `ccsp_cif_stubs.h` for every kernel call type
- Complex register clobber lists

With save/restore primitives, a kernel call becomes:
```c
// In generated code (or CIF):
Wptr[Iptr] = <return address>;    // Already done today
ccsp_save_context(Wptr);          // Save full state
kernel_function(sched, Wptr, args...);  // Standard C call
// Execution resumes here when this process is next scheduled
```

And the scheduler's "run next process" becomes:
```c
void run_next(sched_t *sched, word *next_wptr) {
    ccsp_restore_context(next_wptr, 1);  // Resume the process
}
```

### 4.4 Preserving the Fast Path

The inline quick reschedule is the most performance-critical path. With the save/restore approach, we have two options:

**Option A: Keep the current inline reschedule as-is.** The save/restore primitives are only used for the "slow path" when the kernel needs to be involved (no runnable processes, timer management, multi-threaded scheduling). The inline reschedule continues to work by direct Wptr/Fptr manipulation and jumping to Iptr. This is already what happens today and would continue to work unchanged.

**Option B: Use save/restore for all context switches.** This means the inline reschedule would call `ccsp_save_context`, manipulate the run queue, then call `ccsp_restore_context`. This adds ~20 instructions to the fast path (saving/restoring callee-saved registers that are probably not modified). This may be acceptable on modern out-of-order CPUs but would need benchmarking.

**Recommendation: Option A.** The inline reschedule path is elegant as-is and doesn't need to change. The save/restore primitives replace only the K_ENTRY, K_ZERO_OUT_JRET, and CIF trampoline macros.

### 4.5 Feasibility: HIGH

This is essentially how Go, Lua coroutines, and many modern green-thread runtimes work. The CCSP architecture is already 90% of the way there - the Wptr negative offsets already serve as a save area, and Iptr is already the saved instruction pointer. The main change is making the save/restore explicit rather than scattered across multiple inline assembly macros.

---

## 5. Analysis: Unifying Wptr and SP

### 5.1 The Core Question

The compiler uses Wptr as a stack pointer: `I_AJW` (adjust workspace) grows it downward, locals are at positive offsets, and it shrinks back on return. This is exactly how SP works on a conventional architecture. Why maintain two separate concepts?

### 5.2 Arguments For Unification

1. **Eliminates stack switching.** Currently, every kernel call requires switching SP to the kernel stack and back. If Wptr IS SP, the process is already on a valid stack.

2. **Simplifies C interop.** C functions called from occam currently need complex trampolines (K_CIF_PROC) to set up a valid C stack. If the occam workspace IS the C stack, C functions can be called normally.

3. **Reduces register pressure.** Currently, 4 registers are reserved on AArch64: x28 (Wptr), x25 (Sched), x27 (Fptr), x26 (Bptr). If Wptr=SP, x28 is freed. That's one more register for the allocator.

4. **Simplifies the generated code.** No need for separate Wptr manipulation - standard push/pop or SP-relative addressing works.

5. **Better tooling support.** Debuggers, profilers, and unwinders understand SP-based stacks. They cannot follow Wptr-based stacks.

### 5.3 The Problem: Negative Offset Metadata

The negative offset metadata exists because of the **Transputer's ascending stack**. On the Transputer, the workspace pointer pointed to the base of the process's local variable area and the stack grew *upward* (toward higher addresses). The thread metadata (Iptr, Link, Priofinity, etc.) was stored at negative offsets - physically *below* Wptr in memory - and was safe because the stack grew in the opposite direction:

```
Transputer (ascending stack):
[low address]
  Wptr[-2] = Link          <- metadata below Wptr, safe
  Wptr[-1] = Iptr
  Wptr[0]  = Temp          <- Wptr points here
  Wptr[1]  = local 1       <- stack grows UPWARD
  Wptr[2]  = local 2
  ...
[high address]
```

When this was mapped onto conventional descending-stack architectures (x86, ARM), the compiler was made to emit negative `I_AJW` adjustments to grow the workspace downward, but the metadata offset signs were preserved. This created an inversion:

```
Current (descending stack, Transputer-style offsets):
[high address]
  ... older frames ...
  Wptr[0] = Temp            <- Wptr points here
  Wptr[-1] = Iptr           <- metadata BELOW Wptr
  Wptr[-2] = Link              (in the danger zone for descending stacks!)
  ...
  Wptr[-9] = EscapePtr
[low address]
```

If Wptr=SP, then the area *below* SP contains thread metadata. But C function calls grow the stack downward from SP, which would **overwrite the thread metadata**. This is the fundamental reason the dual-pointer model was needed: the metadata is on the wrong side of the stack pointer for a descending-stack architecture.

### 5.4 The Natural Fix: Flip the Metadata to Positive Offsets

The fix follows directly from understanding the root cause. On the Transputer, metadata was at negative offsets because the stack grew upward, *away* from it. On descending-stack machines, we should put metadata at **positive offsets** from the stack base, so the stack grows downward, *away* from it:

```
Proposed (descending stack, corrected offsets):
[high address]
  +--- Stack allocation top ---+
  | Process descriptor:        |  <- positive offsets from stack base
  |   [+0] Iptr               |
  |   [+1] Link               |
  |   [+2] Priofinity         |
  |   [+3] Pointer/State      |
  |   [+4] TLink              |
  |   [+5] Time_f             |
  |   [+6] SchedPtr           |
  |   [+7] BarrierPtr         |
  |   [+8] EscapePtr          |
  |   [+9..+24] CIF save area |
  +--- Initial SP/Wptr --------+  <- process starts executing here
  | Frame 0 locals             |  <- stack grows DOWNWARD, away from metadata
  | Frame 1 (after I_CALL)     |
  | ...                        |
  | Current SP                 |
  +--- Stack allocation bottom -+
[low address]
```

This is the mirror image of the Transputer layout, adapted for descending stacks. The metadata is at the top of the allocation, safely above all stack frames. The stack grows downward and never touches it.

### 5.4.1 Accessing the Descriptor

The process needs to find its descriptor to read/write metadata (e.g., when blocking on a channel, the resume address must be stored in the descriptor's Iptr field). There are several options:

**Option 1: Dedicated descriptor register.** Use a register (e.g., x24 on AArch64, which is currently SPTR but barely used) to hold the descriptor base address. The descriptor is then accessed as `desc[Iptr]`, `desc[Link]`, etc. with positive offsets.

**Pros:** Fast (single register + offset). No memory lookup.
**Cons:** Uses a register. But we *gain* a register from Wptr/SP unification (x28 freed), so net register count is unchanged.

**Option 2: Descriptor at known offset from stack base.** Store the descriptor pointer at the top of the stack allocation. The stack base address can be derived from SP by masking (if stacks are power-of-2 aligned) or stored in the scheduler structure.

**Pros:** No dedicated register needed.
**Cons:** Extra memory load to find descriptor. Stack alignment constraints.

**Option 3: Descriptor pointer in the stack frame.** Each stack frame stores a pointer to the process descriptor at a fixed offset. This is similar to how some runtimes store TLS pointers.

**Recommendation: Option 1** (dedicated register) for performance-critical paths. The descriptor register replaces the freed Wptr register, maintaining the same register pressure. It also maps naturally to what Fptr/Bptr point to in the run queue.

### 5.4.2 The Descriptor as a Proper Structure

Factoring the metadata out of the workspace and into a proper C struct has additional benefits:

```c
typedef struct process_descriptor {
    word iptr;           // Saved instruction pointer
    word link;           // Run queue link (next descriptor)
    word priofinity;     // Priority + affinity
    word pointer_state;  // Channel pointer / ALT state
    word tlink;          // Timer link
    word time_f;         // Timer value
    word sched_ptr;      // Scheduler pointer
    word barrier_ptr;    // Barrier pointer
    word escape_ptr;     // Exception pointer
    word stack_base;     // Top of stack allocation (for overflow checks)
    word stack_ptr;      // Saved SP (for context save/restore)
} process_descriptor_t;
```

- **Type safety:** No more `Wptr[-7]` magic numbers scattered through the codebase
- **Cache efficiency:** Run queue traversal walks small descriptors, not full workspaces
- **Debugging:** `gdb` can print `desc->iptr` instead of requiring manual offset arithmetic
- **Extensibility:** Adding new per-process fields doesn't require changing workspace allocation math

### 5.4.3 Allocation: Contiguous or Separate?

The descriptor can be allocated either:

**(a) Contiguously at the top of the stack allocation** (Jim's proposal):
```c
void *alloc = malloc(sizeof(process_descriptor_t) + stack_size);
process_descriptor_t *desc = (process_descriptor_t *)alloc;
word *stack_top = (word *)(alloc + sizeof(process_descriptor_t));
// SP starts at stack_top, grows downward
```

**(b) As a separate small allocation:**
```c
process_descriptor_t *desc = malloc(sizeof(process_descriptor_t));
word *stack = malloc(stack_size);
desc->stack_base = (word)stack + stack_size;
```

Option (a) is preferred: single allocation, better locality, and the descriptor register can derive the stack base (or vice versa) with a known constant offset. This is exactly how the current workspace allocation works, just with the metadata at the top instead of interleaved with the stack.

### 5.5 Impact on the Compiler

The occam compiler (occ21) already emits **descending** workspace adjustments: `I_AJW(-n)` to allocate, `I_AJW(+n)` to deallocate. tranx86 translates these directly: `Wptr += (operand << WSH)`, which decrements Wptr for negative operands. So the stack is already descending in practice - the compiler doesn't need to change its frame allocation direction.

The ETC instructions that access workspace slots use signed offsets from Wptr:
- `I_LDL n` loads from `Wptr[n]`, i.e. `[Wptr + n*wordsize]`
- `I_STL n` stores to `Wptr[n]`

Positive `n` accesses the current frame's locals and the caller's data (above Wptr). Negative `n` accesses the metadata region (below Wptr). With Wptr=SP unification and a separate descriptor, positive offsets work unchanged (they're standard SP-relative addressing into the stack). The few negative-offset accesses that the inline scheduling code generates would be redirected to the descriptor register instead.

**The key insight is that the compiler rarely generates direct accesses to the negative offset region.** The negative-offset metadata (Iptr, Link, Priofinity, etc.) is accessed almost exclusively by tranx86's inline scheduling code and the CCSP runtime - not by occ21's ETC output. The specific places where metadata is written are:

1. **STARTP (inline):** tranx86 writes Iptr and Priofinity to the new process's descriptor
2. **ENDP (inline):** tranx86 reads/decrements Count
3. **Channel ops (inline):** tranx86 writes Iptr, Pointer before blocking; reads them after wakeup
4. **ALT (inline):** tranx86 reads/writes State, Pointer
5. **Timer ops (inline):** tranx86 reads/writes TLink, Time_f

All of these are in tranx86's architecture backends (`archx64.c`, `archaarch64.c`), not in the ETC bytecode. With the descriptor at positive offsets, these change from `Wptr[W_IPTR]` to `desc[D_IPTR]` - a mechanical transformation in tranx86.

**occ21 itself would need minimal changes** - primarily the workspace size calculations, which currently account for the negative-offset metadata region. With separate descriptors, the workspace size is just the stack space needed for locals and call frames.

### 5.6 Impact on the Runtime

The runtime (sched.c) accesses workspace offsets extensively:
```c
Wptr[Iptr] = saved_ip;
Wptr[Link] = next_process;
Wptr[Priofinity] = priority;
```

With separate descriptors, these become:
```c
desc->iptr = saved_ip;
desc->link = next_process;
desc->priofinity = priority;
```

This is a find-and-replace refactoring. The semantics are identical.

### 5.7 Impact on the Run Queue

Currently, the run queue is a linked list of Wptr values. `Fptr` points to the Wptr of the head process. `Wptr[Link]` points to the next Wptr.

With separate descriptors, the run queue would link descriptors instead:
```c
Fptr -> desc_A -> desc_B -> desc_C -> NULL
         |          |          |
         v          v          v
       stack_A   stack_B   stack_C
```

This is actually better for cache behaviour: the descriptors are small and contiguous, while the stacks are large and scattered. Walking the run queue (which the scheduler does frequently) touches less memory.

### 5.8 Feasibility Assessment

| Aspect | Difficulty | Risk |
|--------|-----------|------|
| Separate descriptor allocation | Low | Low |
| Update tranx86 inline code | Medium | Medium |
| Update sched.c kernel functions | Medium | Low |
| Update CIF interface | Medium | Medium |
| Preserve fast-path performance | Medium | Medium |
| Update occ21 | Low | Low |
| Backward compatibility (x86) | Medium | Medium |

**Overall Feasibility: MEDIUM-HIGH.** The concept is sound and would simplify the architecture significantly, but it touches many components and requires careful testing.

### 5.9 The CIF Interaction Problem

The most delicate aspect is CIF (C Interface) processes. Currently, when a C function is started as a lightweight process:

1. `LightProcInit` allocates a workspace with `CIF_PROCESS_WORDS` extra negative-offset space
2. `K_CIF_PROC` switches SP to the workspace, calls the C function, then switches back
3. The C function uses Wptr (passed as first arg) for channel operations
4. Channel operations save/restore state via inline assembly that switches stacks

With Wptr=SP unification:
1. The workspace IS the stack, so no stack switching is needed
2. C functions run directly on the process stack (which is what they'd expect)
3. Channel operations are normal C function calls (with save/restore primitives)
4. The `K_CIF_PROC` trampoline becomes trivial or unnecessary

This is actually a **major simplification**. The entire `ccsp_cif_stubs.h` file (hundreds of lines of architecture-specific inline assembly) could potentially be replaced by normal C function calls.

---

## 6. Combined Proposal: The Unified Architecture

### 6.1 Design Overview

```
Process Structure:
  [process_descriptor_t]     <- 72 bytes, separately allocated
    - iptr, link, priofinity, pointer, tlink, time_f
    - sched_ptr, barrier_ptr, escape_ptr
    - stack_base, stack_limit  (new: for stack overflow detection)
    
  [stack]                    <- SP-based, grows downward
    - occam locals and args (positive from base)
    - C function frames
    - save/restore register save area (at bottom)

Register Usage (AArch64):
  sp    = Stack pointer (IS the workspace pointer)
  x28   = Process descriptor pointer (was Wptr)
  x25   = Scheduler pointer (unchanged)
  x27   = Fptr - run queue head (now points to descriptors)
  x26   = Bptr - run queue tail (now points to descriptors)
  x19-x24 = Available for register allocator (gain of x28)

Context Switch:
  ccsp_save_context(descriptor)    - saves regs + SP to descriptor
  ccsp_restore_context(descriptor) - restores regs + SP from descriptor
```

### 6.2 Kernel Call Sequence (New)

```c
// Generated code for ChanOut:
// 1. Save return address in descriptor
descriptor->iptr = <return_label>;

// 2. Standard C function call (no stack switch, no cparam, no calltable)
kernel_Y_out(sched, descriptor, length, channel, buffer);

// 3. Execution resumes here when process is next scheduled
<return_label>:
```

The kernel function is a normal C function. If it needs to deschedule the process:
```c
void kernel_Y_out(sched_t *sched, process_descriptor_t *desc, 
                  word length, word *channel, void *buffer) {
    if (channel_ready(channel)) {
        // Fast path: copy data, wake waiting process
        memcpy(dest, buffer, length);
        enqueue(sched, waiting_desc);
        return;  // Returns to caller normally
    } else {
        // Slow path: block this process
        desc->pointer = (word)buffer;
        *channel = (word)desc;  // Store descriptor in channel word
        
        // Switch to next process
        process_descriptor_t *next = dequeue(sched);
        ccsp_restore_context(next, 1);  // Does not return
    }
}
```

### 6.3 Inline Fast Path (Preserved)

The inline quick reschedule can still avoid the kernel entirely:

```asm
// AArch64 inline quick reschedule
cbnz x27, .have_process        // if (Fptr != NULL)
bl kernel_K_PAUSE               // slow path: enter scheduler

.have_process:
mov x28, x27                    // descriptor = Fptr
ldr x27, [x28, #LINK_OFFSET]   // Fptr = Fptr->link
// Restore context from descriptor
ldp x19, x20, [x28, #SAVE_OFFSET]
... (restore registers)
ldr x0, [x28, #SP_OFFSET]
mov sp, x0
ldr x0, [x28, #IPTR_OFFSET]
br x0                           // Jump to saved IP
```

This is essentially the same cost as today. The run queue head (Fptr) now points to a descriptor instead of a workspace, but the dispatch logic is identical.

---

## 7. Risks and Mitigations

### 7.1 Performance Risk
**Concern:** Adding a level of indirection (descriptor -> stack) might slow down context switching.
**Mitigation:** The descriptor is small (fits in 1-2 cache lines) and is the first thing accessed during a context switch. Modern CPUs handle this efficiently. The removal of cparam writes, calltable loads, and stack switching may actually *improve* performance.

### 7.2 Memory Layout Risk
**Concern:** Changing the workspace layout could break subtle assumptions in occ21 or the standard libraries.
**Mitigation:** The occam standard libraries (under modules/) use workspace offsets only through the well-defined channel/process/ALT constructs. They don't access raw negative offsets directly. The changes are confined to the tranx86/runtime boundary.

### 7.3 Backward Compatibility Risk
**Concern:** The x86 (32-bit) backend might not support the new model well.
**Mitigation:** The 32-bit backend can retain the old convention. The refactoring can be implemented as a new "kernel interface" mode alongside the existing KRNLIFACE_NEWCCSP, allowing gradual migration.

### 7.4 Debugging Risk
**Concern:** A large refactoring could introduce subtle bugs.
**Mitigation:** The CIF test suite provides an excellent incremental testing path. Start with CIF-only changes (which don't require compiler changes), verify correctness, then extend to compiler-generated code.

---

## 8. Implementation Plan

### Phase 1: Standard Calling Convention (No Wptr/SP change)
*Estimated scope: ~1 week of focused work*

1. **Define new kernel function signatures** in a new header `kernel_api.h`
2. **Update `compose_*_kcall` in archaarch64.c and archx64.c** to emit direct calls with register arguments instead of cparam writes + calltable indirection
3. **Update sched.c** to use new function signatures, removing K_CALL_DEFINE macros
4. **Update CIF stubs** to call kernel functions directly (remove inline assembly for argument marshalling, keep stack switching for now)
5. **Remove calltable** from sched_t, remove kitable.h indices
6. **Test**: CIF examples, then full compilation chain

### Phase 2: setjmp/longjmp-Style Primitives
*Estimated scope: ~3-4 days*

1. **Implement `ccsp_save_context` / `ccsp_restore_context`** in assembly for AArch64 and x64
2. **Replace K_ENTRY** with `ccsp_restore_context` for initial process dispatch
3. **Replace K_ZERO_OUT_JRET** with `ccsp_restore_context` in kernel functions that reschedule
4. **Simplify K_CIF_PROC** to use save/restore instead of manual register shuffling
5. **Test**: CIF examples, verify context switching works correctly
6. **Keep inline quick reschedule** unchanged (it doesn't need save/restore)

### Phase 3: Descriptor at Top of Stack (Ascending-to-Descending Fix)
*Estimated scope: ~1 week*

This is the key architectural change motivated by the Transputer ascending/descending stack insight. The thread metadata moves from negative offsets below Wptr to positive offsets at the top of each stack allocation.

1. **Define `process_descriptor_t`** structure with the fields currently at negative offsets (Iptr, Link, Priofinity, Pointer/State, TLink, Time_f, SchedPtr, BarrierPtr, EscapePtr, plus saved SP and stack base)
2. **Update workspace allocation** (`LightProcInit`, `ProcAllocInitial`, `WORKSPACE_SIZE` macro) to place the descriptor contiguously at the top of each stack allocation, with the usable stack starting immediately below it
3. **Introduce a descriptor register** (e.g., x24 on AArch64) - this replaces the freed Wptr register, so net register pressure is unchanged
4. **Update run queue** to link descriptors instead of workspaces (Fptr/Bptr now point to descriptors)
5. **Update all `Wptr[negative_offset]` accesses** in sched.c to use `desc->field` notation
6. **Update tranx86 inline code** (startp, endp, channel ops, ALT, timers) to use descriptor register for scheduling metadata
7. **Flip `W_IPTR`, `W_LINK`, etc. in transputer.h** to use the descriptor register + positive offsets instead of Wptr + negative offsets
8. **Test thoroughly**: all CIF examples, then compiler-generated code

### Phase 4: Wptr/SP Unification
*Estimated scope: ~1-2 weeks*

With the descriptor separated (Phase 3), the workspace is now purely a stack. This phase makes it the hardware stack.

1. **Modify tranx86** to emit SP-based addressing: `I_AJW` becomes `sub sp, sp, #n` / `add sp, sp, #n`; `I_STL`/`I_LDL` become SP-relative loads/stores
2. **Remove the dedicated Wptr register** (x28 on AArch64, r14 on x64) - SP takes its role
3. **Update save/restore primitives** to save/restore SP as part of the context (stored in descriptor->stack_ptr)
4. **Simplify CIF** - C functions run directly on the process stack, no more K_CIF_PROC trampolines or stack switching
5. **Remove all stack-switching code** from CIF stubs and sched_asm_inserts.h
6. **Extensive testing**: full compilation chain, all tests, cif-commstime benchmark

### Phase 5: Cleanup and Optimisation
*Estimated scope: ~3-4 days*

1. **Remove dead code**: old calltable infrastructure, old CIF assembly stubs, old K_* macros
2. **Optimise the save/restore primitives**: only save registers actually in use
3. **Update x86 (32-bit) backend** to work with new infrastructure or keep legacy path
4. **Performance benchmarking**: commstime, microbenchmarks for context switch time
5. **Documentation update**

---

## 9. Conclusion

The proposed refactoring is not only feasible but would significantly improve the CCSP architecture:

1. **Standard C calling convention**: Straightforward, low-risk, immediate benefits in code clarity and debuggability. The original reasons for the custom convention (limited x86 registers, Transputer heritage) no longer apply.

2. **setjmp/longjmp-style primitives**: Reduce per-architecture assembly from hundreds of lines to ~30 lines. Make context switching explicit and testable. Already the standard approach in modern green-thread runtimes.

3. **Wptr/SP unification**: The most ambitious change, but architecturally the most valuable. The workspace pointer IS a stack pointer - maintaining the fiction that they're different adds complexity everywhere. The root cause of the negative-offset problem is the Transputer's ascending stack: metadata was below Wptr because the stack grew upward, away from it. On descending-stack machines, the fix is to put the metadata above SP (at the top of the stack allocation) so the stack grows downward, away from it. This gives us a proper process descriptor as a natural consequence.

The phased approach allows each step to be tested and validated independently. Phases 1-2 can be done without changing the fundamental workspace model, providing immediate benefits. Phases 3-4 are the bigger architectural changes but build on the foundation laid by the earlier phases.

The end result would be a CCSP that:
- Uses standard C calling conventions throughout
- Has minimal architecture-specific assembly (~30 lines per target)
- Supports new architectures with just save/restore primitives
- Is debuggable with standard tools (gdb, perf, valgrind)
- Maintains the performance characteristics that make CCSP competitive (inline fast-path scheduling, zero-overhead process creation)
