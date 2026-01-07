/*
 * kernel_stubs_aarch64.c - minimal kernel function stubs for aarch64
 */

#include <stdio.h>
#include <stdlib.h>

/* Kernel function stubs - note: C compiler adds underscore prefix on macOS */
void kernel_Y_shutdown(void)
{
    printf("Y_shutdown called - exiting\n");
    exit(0);
}

void kernel_Y_BNSeterr(void)
{
    printf("Y_BNSeterr called\n");
    /* This function typically sets error state, but for minimal test just return */
}

void kernel_Y_outbyte(int character, void* sched, void* wptr)
{
    /* Debug: show what parameters we received */
    printf("[DEBUG] kernel_Y_outbyte called with character=0x%x (%d), sched=%p, wptr=%p\n", 
           character, character, sched, wptr);
    if (character >= 32 && character <= 126) {
        printf("[DEBUG] Character is printable: '%c'\n", (char)character);
    }
    /* Output character to stdout */
    putchar((unsigned char)character);
    fflush(stdout);
}