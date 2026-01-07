#include <stdio.h>
#include <stddef.h>
#include "runtime/ccsp/include/sched_types.h"

int main() {
    printf("sizeof(sched_t) = %zu\n", sizeof(sched_t));
    printf("offsetof(sched_t, stack) = %zu\n", offsetof(sched_t, stack));
    printf("offsetof(sched_t, cparam) = %zu\n", offsetof(sched_t, cparam));
    printf("offsetof(sched_t, cparam[0]) = %zu\n", offsetof(sched_t, cparam[0]));
    printf("offsetof(sched_t, cparam[1]) = %zu\n", offsetof(sched_t, cparam[1]));
    return 0;
}
