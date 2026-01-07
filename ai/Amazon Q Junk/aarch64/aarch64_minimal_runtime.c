#include <stdio.h>
#include <stdlib.h>

// Minimal workspace initialization for aarch64
void* init_aarch64_workspace() {
    void* workspace = malloc(1024 * sizeof(void*));
    return (char*)workspace + (512 * sizeof(void*));
}

// Minimal kernel functions with correct calling convention for aarch64
void kernel_Y_outbyte(int character) {
    putchar(character);
    fflush(stdout);
}

void kernel_Y_shutdown() {
    exit(0);
}

void kernel_Y_BNSeterr() {
    // No-op
}