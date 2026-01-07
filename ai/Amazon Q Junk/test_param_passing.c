#include <stdio.h>
#include <stdlib.h>

// Mock scheduler structure
typedef struct {
    unsigned int stack;
    unsigned long cparam[5];
} mock_sched_t;

// Mock kernel function
void kernel_Y_outbyte(unsigned long param0, mock_sched_t *sched, void *Wptr) {
    printf("param0 (character): 0x%lx ('%c')\n", param0, (char)param0);
    printf("sched->cparam[0] (channel): 0x%lx\n", sched->cparam[0]);
    printf("Wptr: %p\n", Wptr);
    
    if (sched->cparam[0] == 2) {
        printf("SUCCESS: Channel parameter correctly passed!\n");
        putchar((char)param0);
        fflush(stdout);
    } else {
        printf("FAILED: Channel parameter corrupted (expected 2, got 0x%lx)\n", sched->cparam[0]);
    }
}

// Mock workspace with channel parameters
void* init_workspace() {
    void** workspace = malloc(64 * sizeof(void*));
    void** wptr = workspace + 32;  // Middle of workspace for negative offsets
    
    wptr[1] = (void*)1;  // keyboard channel
    wptr[2] = (void*)2;  // screen channel (stdout)
    wptr[3] = (void*)3;  // error channel
    
    return wptr;
}

// Mock scheduler
mock_sched_t* local_scheduler() {
    static mock_sched_t sched = {0};
    return &sched;
}

// Test the parameter passing mechanism
int main() {
    void* wptr = init_workspace();
    mock_sched_t* sched = local_scheduler();
    
    // Simulate the fixed parameter passing:
    // 1. Load screen channel from Wptr[2] 
    unsigned long screen_channel = (unsigned long)((void**)wptr)[2];
    
    // 2. Store in scheduler cparam[0]
    sched->cparam[0] = screen_channel;
    
    // 3. Call kernel function with 'H' character
    kernel_Y_outbyte('H', sched, wptr);
    
    free((void**)wptr - 32);
    return 0;
}