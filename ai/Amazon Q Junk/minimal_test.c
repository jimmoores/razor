#include <stdio.h>
#include <stdlib.h>

/* External occam function aliased from $test */
extern void test_entry_point(void);

/* Stub implementations for missing Y_ functions */
void _kernel_Y_shutdown(void) {
    printf("Y_shutdown called - program completed successfully\n");
    exit(0);
}

void _kernel_Y_BNSeterr(void) {
    printf("Y_BNSeterr called - error occurred\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    printf("Starting minimal aarch64 occam test\n");
    
    /* Call the occam function directly without CCSP initialization */
    test_entry_point();
    
    printf("Program completed successfully\n");
    return 0;
}