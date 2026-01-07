#include <stdio.h>
#include <stdlib.h>

/* CCSP runtime functions */
extern int ccsp_init(void);
extern void ccsp_exit(int status, int dump_core);

/* Generated occam main function */
extern void _main(void);

int main(int argc, char **argv)
{
    /* Initialize CCSP runtime */
    if (!ccsp_init()) {
        fprintf(stderr, "Failed to initialize CCSP runtime\n");
        return 1;
    }
    
    /* Call the generated occam main function */
    _main();
    
    /* Clean shutdown */
    ccsp_exit(0, 0);
    return 0;
}