#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef uint64_t word;

struct simple_sched {
    unsigned int stack;     // 4 bytes
    word cparam[5];         // 5 * 8 bytes = 40 bytes
};

int main() {
    printf("sizeof(unsigned int) = %zu\n", sizeof(unsigned int));
    printf("sizeof(word) = %zu\n", sizeof(word));
    printf("sizeof(simple_sched) = %zu\n", sizeof(struct simple_sched));
    printf("offsetof(simple_sched, stack) = %zu\n", offsetof(struct simple_sched, stack));
    printf("offsetof(simple_sched, cparam) = %zu\n", offsetof(struct simple_sched, cparam));
    printf("offsetof(simple_sched, cparam[0]) = %zu\n", offsetof(struct simple_sched, cparam[0]));
    printf("offsetof(simple_sched, cparam[1]) = %zu\n", offsetof(struct simple_sched, cparam[1]));
    return 0;
}
