/*
 * main_entry_aarch64.c - minimal aarch64 CCSP entry point
 * Provides basic runtime initialization for aarch64 programs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Basic type definitions for aarch64 */
typedef unsigned long word;
typedef unsigned char byte;

/* Workspace constants */
#define Iptr        -1
#define Link        -2
#define Priofinity  -3
#define Pointer     -4

/* Global workspace allocation */
static word aarch64_workspace[1024] __attribute__((aligned(16)));
static word *Wptr = NULL;

/* Minimal CCSP runtime initialization - note: C compiler adds underscore prefix on macOS */
void ccsp_aarch64_main_entry(void)
{
    /* Clear workspace memory */
    memset(aarch64_workspace, 0, sizeof(aarch64_workspace));
    
    /* Initialize workspace pointer to middle of workspace */
    Wptr = &aarch64_workspace[512];
    
    /* Initialize basic workspace fields */
    Wptr[Iptr] = (word) 0;        /* Instruction pointer */
    Wptr[Link] = (word) 0;        /* Process link */
    Wptr[Pointer] = (word) 0;     /* Data pointer */
    Wptr[Priofinity] = (word) 0;  /* Priority and affinity */
    
    /* Initialize screen channel parameter for h.occ */
    /* Use special address that kernel recognizes as stdout */
    Wptr[1] = (word) 1;           /* Screen channel address (stdout) */
    
    /* Initialize runtime and return to let occam code execute */
    printf("CCSP aarch64 runtime initialized\n");
    /* Return to allow generated occam code to run */
}