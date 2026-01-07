#include <stdlib.h>
#include <stdio.h>

// Forward declaration of the occam entry point (actual symbol from nm is _main)
// We need to avoid naming conflict with our main function
extern void occam_program_main(void) asm("_main");

// Kernel function stubs for simple execution
void kernel_Y_shutdown() {
    exit(0);
}

void kernel_Y_BNSeterr() {
    // No-op for basic implementation
}

void kernel_Y_outbyte(int channel) {
    // Character is in the channel parameter
    putchar(channel);
    fflush(stdout);
}

// CCSP workspace initialization for aarch64
void* init_aarch64_workspace() {
    // Allocate 64KB workspace (16384 * 8 bytes = 128KB for 64-bit pointers)
    void* workspace = malloc(16384 * sizeof(void*));
    if (!workspace) {
        return (void*)0x100000;  // fallback address
    }
    
    // Return pointer to middle of workspace for negative offset support
    return (char*)workspace + (8192 * sizeof(void*));
}

// Simple main function that calls the occam program directly
int main(int argc, char *argv[]) {
    // Call the occam program entry point
    occam_program_main();
    return 0;
}