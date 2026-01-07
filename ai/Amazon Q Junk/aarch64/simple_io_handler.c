/*
 * simple_io_handler.c - Simple I/O handler for aarch64 CCSP runtime
 * Handles special channels for stdout/stderr without full CCSP channel infrastructure
 */

#include <stdio.h>
#include <stdlib.h>

/* Simple channel I/O handler that detects stdout/stderr channels */
void handle_simple_channel_output(unsigned long channel_addr, char byte_value) {
    /* Check if this is a special channel (stdout/stderr) */
    if (channel_addr == 0 || *(unsigned long*)channel_addr == 0) {
        /* This is likely a stdout channel - output the character */
        putchar(byte_value);
        fflush(stdout);
        return;
    }
    
    /* For now, treat any other channel as stdout */
    putchar(byte_value);
    fflush(stdout);
}

/* Kernel function that handles outbyte with proper channel detection */
void kernel_Y_outbyte_simple(unsigned long value, void* sched, void* wptr) {
    /* Extract the character from the value parameter */
    char byte_to_output = (char)(value & 0xFF);
    
    /* Get the channel address from the scheduler's cparam[0] */
    unsigned long* scheduler = (unsigned long*)sched;
    unsigned long channel_addr = scheduler[0];  /* cparam[0] */
    
    printf("DEBUG: kernel_Y_outbyte_simple: value=0x%lx, char='%c', channel_addr=0x%lx\n", 
           value, byte_to_output, channel_addr);
    
    /* Handle the output */
    handle_simple_channel_output(channel_addr, byte_to_output);
}