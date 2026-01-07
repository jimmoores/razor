/*
 * aarch64_init.c -- ARM64 workspace initialization for CCSP
 * This provides the missing initialization function for aarch64 CCSP runtime
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* Initialize aarch64 workspace - allocate memory and return pointer */
void *_init_aarch64_workspace(void) {
    /* Allocate 64KB workspace - enough for most occam programs */
    void *workspace = malloc(65536);
    if (!workspace) {
        /* If malloc fails, return a static buffer as fallback */
        static char static_workspace[65536];
        return static_workspace;
    }
    return workspace;
}

/* Kernel interface stubs for missing Y_ functions */
void _kernel_Y_shutdown(void) {
    exit(0);
}

void _kernel_Y_BNSeterr(void) {
    exit(1);
}

void _kernel_Y_outbyte(int ch) {
    putchar(ch);
    fflush(stdout);
}