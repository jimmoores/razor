#include <stdio.h>

// External occam main function
extern void $main(void);

int main(int argc, char *argv[]) {
    printf("Starting occam program...\n");
    $main();
    printf("Occam program completed.\n");
    return 0;
}