set pagination off
set confirm off
set breakpoint pending on

# Break at error process entry
break O_kroc_error_process

run

# Error process entered
printf "=== Error process entry ===\n"
printf "x28=%p x28[1]=%p (should be err_chan)\n", $x28, *(void**)($x28+8)

# Step through error process to the bl kernel_Y_in8 call
# The error process code is:
#   mov x16, #0; str x16, [x28, #24]  // clear buffer
#   add x0, x28, #24                   // buffer ptr
#   ldr x1, [x28, #8]                  // channel
#   mov x2, x25; mov x3, x28          // sched, Wptr
#   mov x1, x0; mov x0, x1            // shuffle regs
#   bl kernel_Y_in8
stepi 10
printf "=== Before kernel_Y_in8 ===\n"
printf "x0=%p x1=%p x2=%p x3=%p\n", $x0, $x1, $x2, $x3
printf "PC=%p\n", $pc
info symbol $pc
# Check if we're about to call kernel_Y_in8
x/1i $pc

stepi
printf "=== After one more step ===\n"
printf "x0=%p x1=%p x2=%p x3=%p\n", $x0, $x1, $x2, $x3
info symbol $pc
x/1i $pc

stepi
printf "=== Next ===\n"
printf "x0=%p x1=%p x2=%p x3=%p\n", $x0, $x1, $x2, $x3
x/1i $pc

continue
