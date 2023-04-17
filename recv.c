#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mastik/low.h>
#include <mastik/util.h>

#define MAX_VALUES 1024

#define CACHE_ACCESS_THRESHOLD 400

int recv_bit(void *mem_ptr) {
    uint32_t access_time = memaccesstime(mem_ptr);
    return access_time;
    //return access_time < CACHE_ACCESS_THRESHOLD ? 0 : 1;
}

void recv_bits(void *mem_ptr, int *bits, int size) {
    for (int i = 0; i < size; i++) {
        bits[i] = recv_bit(mem_ptr);
        usleep(10000); // Attente entre la réception de chaque bit
    }
}

void moyenne(int *bits_received, int size) {
    int somme = 0;
    for (int i = 0; i < size; i++) {
        somme += bits_received[i];
    }
    printf("Palier moyen : %d\n", somme/size);
}

int main(int argc, char **argv) {
    argc--; argv++;
    if (argc < 1) {
        printf("Not enough arguments, need a file name\n");
        return 1;
    }
    FILE *fp = fopen("result.txt", "w");
    
    char *cache_address;
    cache_address = map_offset(*argv, 64);

    int bits_received[MAX_VALUES];

    while (rdtscp64() >> 35 & 1LL);

    usleep(1000);

    recv_bits(cache_address, bits_received, MAX_VALUES);

    // Afficher les bits reçus
    for (int i = 0; i < MAX_VALUES; i++) {
        fprintf(fp ,"%d ", bits_received[i]);
    }
    moyenne(bits_received, MAX_VALUES);

    fclose(fp);
    
    return 0;
}
