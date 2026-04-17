#include <unistd.h>
#include <stdio.h>

/* Temporary debug: trace when ccsp_cif_process_call is entered */
extern void ccsp_cif_process_call(void *wptr, void *func);

void __attribute__((constructor)) debug_init(void) {
    write(2, "DEBUG: shared lib loaded\n", 25);
}
