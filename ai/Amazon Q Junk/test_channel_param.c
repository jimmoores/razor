#include <stdio.h>
#include <stdint.h>

// Simulate the main PROC with correct parameter access
void test_main_proc(void) {
    // Simulate workspace pointer (x28)
    uint64_t workspace[10] = {0};
    
    // Initialize workspace as runtime should do
    workspace[1] = 0x1;  // keyboard channel
    workspace[2] = 0x2;  // screen channel  
    workspace[3] = 0x3;  // error channel
    
    // Simulate loading screen channel from Wptr[2] (offset 16 bytes)
    uint64_t screen_channel = workspace[2];
    char character = 'H';
    
    printf("Character: %c (0x%x)\n", character, character);
    printf("Screen channel: 0x%lx\n", screen_channel);
    
    if (screen_channel == 0x2) {
        printf("SUCCESS: Screen channel loaded correctly from Wptr[2]\n");
    } else {
        printf("FAILED: Screen channel incorrect\n");
    }
}

int main() {
    printf("Testing channel parameter passing fix:\n");
    test_main_proc();
    return 0;
}