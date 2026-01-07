#include <stdio.h>
#include <stdlib.h>

// External occam entry point
extern void $main(void);

// Minimal CCSP stubs
void _kernel_Y_shutdown(void) {
    exit(0);
}

void _kernel_Y_BNSeterr(void) {
    // Do nothing for now
}

int main(int argc, char **argv) {
    printf("Starting occam program...\n");
    $main();
    printf("Occam program completed.\n");
    return 0;
}