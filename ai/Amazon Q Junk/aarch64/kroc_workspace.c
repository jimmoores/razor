#include <stdlib.h>
#include <stdio.h>

// Workspace initialization for aarch64 CCSP
void* init_aarch64_workspace() {
    // Allocate 64KB workspace (16384 * 4 bytes = 64KB)
    void* workspace = malloc(16384 * sizeof(void*));
    if (!workspace) {
        return (void*)0x100000;  // fallback address
    }
    
    // Return pointer to middle of workspace for negative offset support
    return (char*)workspace + (8192 * sizeof(void*));
}

// Kernel function stubs
void Y_shutdown() {
    exit(0);
}

void Y_BNSeterr() {
    // No-op for now
}

void Y_outbyte(int channel) {
    // Character is in the channel parameter (first parameter)
    putchar(channel);
    fflush(stdout);
}

// Kernel functions with single underscore prefix for Darwin
void kernel_Y_shutdown() {
    Y_shutdown();
}

void kernel_Y_BNSeterr() {
    Y_BNSeterr();
}

void kernel_Y_outbyte(int channel) {
    Y_outbyte(channel);
}