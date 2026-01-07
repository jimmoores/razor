void _kernel_Y_shutdown() { /* stub */ }
void _kernel_Y_BNSeterr() { /* stub */ }

// Use asm to define the $main symbol correctly
__asm__(".global $main\n$main:\nret\n");

int main() { 
    return 0; 
}
