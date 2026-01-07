/*
 * kiface_aarch64.c - aarch64 kernel interface symbols
 * Provides the missing symbols that the i386 entry prolog expects
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ukcthreads_types.h>
#include <kiface.h>
#include <kernel.h>
#include <rts.h>
#include <ccsp_if.h>

/* Global kernel interface symbols required by generated code */
ccsp_entrytype EntryPointTable[K_MAX_SUPPORTED];
word *Wptr = NULL;
word *Fptr = NULL; 
word *Bptr = NULL;
int occam_finished = 0;

/* Workspace allocation for aarch64 runtime */
static word aarch64_workspace[1024] __attribute__((aligned(16)));
static int runtime_initialized = 0;

/* Initialize the kernel interface table */
void ccsp_init_kiface_aarch64(void)
{
    int i;
    
    /* Copy the static table to the global table */
    for (i = 0; i < K_MAX_SUPPORTED && i < (sizeof(ccsp_entrytable)/sizeof(ccsp_entrytable[0])); i++) {
        EntryPointTable[i] = ccsp_entrytable[i];
    }
    
    /* Initialize remaining entries as unsupported */
    for (; i < K_MAX_SUPPORTED; i++) {
        EntryPointTable[i].call_offset = i;
        EntryPointTable[i].entrypoint = "Y_unsupported";
        EntryPointTable[i].input = 0;
        EntryPointTable[i].output = 0;
        EntryPointTable[i].support = KCALL_UNSUPPORTED;
    }
}

/* Initialize aarch64 CCSP runtime pointers */
void ccsp_aarch64_runtime_init(void)
{
    if (runtime_initialized) {
        return;
    }
    
    /* Clear workspace memory first */
    memset(aarch64_workspace, 0, sizeof(aarch64_workspace));
    
    /* Initialize workspace pointers to valid memory */
    Wptr = &aarch64_workspace[512];  /* Middle of workspace */
    Fptr = NULL;  /* No processes in front queue initially */
    Bptr = NULL;  /* No processes in back queue initially */
    
    /* Initialize basic workspace fields using correct negative offsets */
    Wptr[Iptr] = (word) 0;        /* Instruction pointer (-1) */
    Wptr[Link] = (word) 0;        /* Process link (-2) */
    Wptr[Pointer] = (word) 0;     /* Data pointer (-4) */
    Wptr[Priofinity] = (word) 0;  /* Priority and affinity (-3) */
    
    /* Initialize CCSP runtime if not already done */
    if (!runtime_initialized) {
        ccsp_init_kiface_aarch64();
        runtime_initialized = 1;
    }
}

/* Entry point for aarch64 occam programs */
void ccsp_aarch64_main_entry(void)
{
    /* Initialize runtime */
    ccsp_aarch64_runtime_init();
    
    /* Initialize CCSP system */
    if (ccsp_init()) {
        /* Start the main occam process */
        ccsp_kernel_entry(Wptr, Fptr);
    } else {
        fprintf(stderr, "CCSP initialization failed\n");
        exit(1);
    }
}