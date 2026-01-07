/*
 * workspace_init.c - aarch64 workspace initialization
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Initialize workspace pointer - called from assembly */
unsigned long *init_aarch64_workspace(void)
{
    static unsigned long *workspace_base = NULL;
    unsigned long *wptr;
    
    /* Allocate workspace on first call */
    if (!workspace_base) {
        workspace_base = (unsigned long*)malloc(4096 * sizeof(unsigned long));
        if (!workspace_base) {
            printf("ERROR: Failed to allocate workspace\n");
            exit(1);
        }
        memset(workspace_base, 0, 4096 * sizeof(unsigned long));
    }
    
    /* Set workspace pointer well into the middle with plenty of space for negative offsets */
    wptr = &workspace_base[2048];
    
    /* Verify alignment */
    if (((unsigned long)wptr) % 8 != 0) {
        printf("ERROR: Workspace pointer not 8-byte aligned: %p\n", wptr);
    }
    
    /* Initialize workspace fields using negative offsets (like CCSP expects) */
    wptr[-1] = 0;  /* Iptr */
    wptr[-2] = 0;  /* Link */
    wptr[-3] = 0;  /* Priofinity */
    wptr[-4] = 0;  /* Pointer/State */
    
    /* Initialize screen channel parameter for h.occ program */
    /* Parameter 1 (screen channel) - use same approach as x86 */
    wptr[1] = 1;  /* Screen channel address (stdout) - matches x86 behavior */
    printf("DEBUG: Initialized wptr[1] = %lu (stdout channel)\n", wptr[1]);
    
    return wptr;
}