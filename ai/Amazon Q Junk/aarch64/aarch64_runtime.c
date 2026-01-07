#include <stdlib.h>
#include <stdio.h>

// aarch64 workspace initialization for CCSP
void* init_aarch64_workspace() {
    // Allocate 64KB workspace (16384 * 8 bytes = 128KB for 64-bit pointers)
    void* workspace = malloc(16384 * sizeof(void*));
    if (!workspace) {
        return (void*)0x100000;  // fallback address
    }
    
    // Return pointer to middle of workspace for negative offset support
    void* wptr = (char*)workspace + (8192 * sizeof(void*));
    
    // Initialize CCSP process state at negative offsets (from ccsp_consts.h)
    // Iptr = -1, Link = -2, Priofinity = -3, Pointer = -4
    ((long*)wptr)[-1] = 0;  // Iptr - instruction pointer
    ((long*)wptr)[-2] = 0;  // Link - process link  
    ((long*)wptr)[-3] = 0;  // Priofinity - priority
    ((long*)wptr)[-4] = 0;  // Pointer - process pointer
    
    return wptr;
}

// Note: Kernel functions are provided by the main CCSP runtime in sched.c
// We only need to provide the workspace initialization for aarch64