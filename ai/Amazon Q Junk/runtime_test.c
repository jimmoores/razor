// Test program that uses the full CCSP runtime
#include <stdio.h>
#include <stdlib.h>

// External runtime functions
extern void ccsp_kernel_entry(void);
extern void ccsp_occam_entry(void);

// External occam main function
extern void occam_main(void) asm("$main");

int main(int argc, char *argv[]) {
    printf("Testing ARM64 occam with full CCSP runtime...\n");
    
    // Initialize the CCSP runtime and call the occam main
    // This is how the runtime is supposed to be used
    ccsp_kernel_entry();
    
    printf("Runtime returned normally\n");
    return 0;
}