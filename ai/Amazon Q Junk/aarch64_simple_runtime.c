#include <stdio.h>
#include <stdlib.h>

void* init_aarch64_workspace() {
    void* workspace = malloc(1024 * sizeof(void*));
    return (char*)workspace + (512 * sizeof(void*));
}

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