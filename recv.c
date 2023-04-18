#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include <string.h>

#include "util.h"

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

void binary_to_message(int *binary_input, char *message_output, int message_length)
{
    for (int i = 0; i < message_length; i++)
    {
        char current_char = 0;
        for (int j = 0; j < 8; j++)
        {
            current_char |= (binary_input[i * 8 + j] << (7 - j));
        }
        message_output[i] = current_char;
    }
    message_output[message_length] = '\0';
}

double calc_error(const int *received_bits, const int *expected_bits, int bit_count)
{
    int error_count = 0;
    for (int i = 0; i < bit_count; i++)
    {
        if (received_bits[i] != expected_bits[i])
        {
            error_count++;
        }
    }
    return ((double)error_count / bit_count) * 100;
}

int main()
{
    const char *filename = "message.txt";
    char *file_content = read_file_content(filename);
    if (file_content == NULL)
        return 1;

    int message_length = strlen(file_content);
    int *received_binary = (int *)malloc(message_length * 8 * sizeof(int));

    char *cache_address;
    cache_address = map_offset("file", 64);

    uint32_t time_mesured;
    int i, somme, nb_val;

    printf("Pret à recevoir le message\n");

    while (access("temp_file.txt", F_OK) == -1)
    {
        usleep(1000); // Attendre 1 ms
    }

    clflush(cache_address);
    synchronisation(SYNC_CYCLES_INIT);
    for (i = 0; i < (message_length * 8); i++)
    {
        synchronisation(SYNC_CYCLES_LOOP);
        uint64_t time = rdtscp64();
        somme = 0;
        nb_val = 0;

        do
        {
            time_mesured = memaccesstime(cache_address);
            clflush(cache_address);
            somme += (time_mesured < L3_THRESHOLD);
            nb_val++;
            delayloop(DELAYLOOP_VALUE);
        } while (rdtscp64() - time < DELAY_PER_BIT);

        received_binary[i] = ((float)somme / nb_val) > 0.5;
    }

    char *received_message = (char *)malloc(message_length + 1);
    binary_to_message(received_binary, received_message, message_length);

    int *message_binary = (int *)malloc(message_length * 8 * sizeof(int));
    message_to_binary(file_content, message_binary, message_length);
    double perc_error = calc_error(received_binary, message_binary, message_length * 8);

    printf("Message reçu: %s\n\n\nTaux d'erreur : %f\n", received_message, perc_error);

    free(message_binary);
    free(received_message);
    free(received_binary);

    return 0;
}

void synchronisation(int N)
{
    while ((rdtscp64() & ((1LL << N) - 1)) > 1000)
        ;
}
