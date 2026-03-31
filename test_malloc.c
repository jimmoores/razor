#include <stdio.h>
#include <stdlib.h>
int main() {
    void *p1 = malloc(100);
    void *p2 = malloc(100);
    void *p3 = malloc(100);
    printf("%p %p %p\n", p1, p2, p3);
    return 0;
}
