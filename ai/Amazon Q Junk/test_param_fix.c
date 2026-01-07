#include <stdio.h>
#include <stdlib.h>

// Test the workspace initialization function
extern void* init_aarch64_workspace();

int main() {
    void** wptr = (void**)init_aarch64_workspace();
    
    printf("Workspace pointer: %p\n", wptr);
    printf("Wptr[1] (keyboard): %p\n", wptr[1]);
    printf("Wptr[2] (screen): %p\n", wptr[2]);
    printf("Wptr[3] (error): %p\n", wptr[3]);
    
    // Test if the values are correctly set
    if (wptr[1] == (void*)1 && wptr[2] == (void*)2 && wptr[3] == (void*)3) {
        printf("SUCCESS: Channel parameters correctly initialized!\n");
        return 0;
    } else {
        printf("FAILED: Channel parameters not initialized correctly\n");
        return 1;
    }
}