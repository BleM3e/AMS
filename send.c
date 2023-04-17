#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include <string.h>

#define MAX_VALUES 1024

void init_bits_to_send(int *bits, int size) {
    for (int i = 0; i < size; i++) {
        bits[i] = i % 2;
    }
}

void send_bit(void *mem_ptr, int bit) {
    if (bit) {
        memaccess(mem_ptr);
    } else {
        clflush(mem_ptr);
    }
}

void send_bits(void *mem_ptr, int *bits, int size) {
    for (int i = 0; i < size; i++) {
        send_bit(mem_ptr, bits[i]);
        usleep(10000); // Attente entre l'envoi de chaque bit
    }
}

int main(int argc, char **argv) {
    argc--; argv++;
    if (argc < 1) {
        printf("Not enough arguments, need a file name\n");
        return 1;
    }
    
    char *cache_address;
    cache_address = map_offset(*argv, 64);

    int bits_to_sends[MAX_VALUES];
    init_bits_to_send(bits_to_sends, MAX_VALUES);
    
    while (rdtscp64() >> 35 & 1LL);

    send_bits(cache_address, bits_to_sends, MAX_VALUES);
    
    return 0;
}
