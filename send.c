#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include <string.h>
#include <time.h>

#include "util.h"

// Convertit un message en représentation binaire
void message_to_binary(const char *message, int *binary_output, int message_length)
{
    for (int i = 0; i < message_length; i++)
    {
        char character = message[i];
        for (int j = 7; j >= 0; j--)
        {
            int bit = (character >> j) & 1;
            binary_output[i * 8 + (7 - j)] = bit;
        }
    }
}

// Lit le contenu d'un fichier et retourne un pointeur vers le contenu
char *read_file_content(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erreur d'ouverture du fichier %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    if (file_content == NULL)
    {
        printf("Erreur d'allocation de mémoire\n");
        fclose(file);
        return NULL;
    }

    fread(file_content, 1, file_size, file);
    fclose(file);

    file_content[file_size] = '\0';

    return file_content;
}

int main(void)
{
    const char *filename = "message.txt";
    char *file_content = read_file_content(filename);

    if (file_content != NULL)
    {
        int message_length = strlen(file_content);
        int *binary_output = (int *)malloc(message_length * 8 * sizeof(int));

        message_to_binary(file_content, binary_output, message_length);

        void *cache_address;
        cache_address = map_offset("file", 0);
        int i;

        clock_t start, end;
        double elapsed;

        clflush(cache_address);
        synchronisation(SYNC_CYCLES_INIT);
        start = clock();

        // Envoi du message bit par bit
        for (i = 0; i < (message_length * 8); i++)
        {
            synchronisation(SYNC_CYCLES_LOOP);
            uint64_t time = rdtscp64();

            if (binary_output[i])
                while (rdtscp64() - time < DELAY_PER_BIT)
                    memaccess(cache_address);
            else
                while (rdtscp64() - time < DELAY_PER_BIT)
                    clflush(cache_address);
        }
        end = clock();
        elapsed = ((double)(end - start)) / CLOCKS_PER_SEC; // Conversion en secondes

        printf("%.4f secondes entre start et end.\n", elapsed);

        free(binary_output);
        free(file_content);
    }
    return 0;
}

// Synchronisation des horloges
void synchronisation(int N)
{
    while ((rdtscp64() & ((1LL << N) - 1)) > 1000)
        ;
}
