#include <stdio.h>
#include <stdlib.h>

// Minimal workspace initialization
void* init_aarch64_workspace() {
    void* workspace = malloc(1024 * sizeof(void*));
    return (char*)workspace + (512 * sizeof(void*));
}

// Simple kernel functions that match the assembly calling convention
void kernel_Y_outbyte(int character) {
    // The assembly puts the character in x0, which becomes the first parameter
    putchar(character);
    fflush(stdout);
}

void kernel_Y_shutdown() {
    exit(0);
}

void kernel_Y_BNSeterr() {
    // No-op
}