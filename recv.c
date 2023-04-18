#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "util.h"

// Cette fonction lit le contenu d'un fichier et le renvoie sous forme de chaîne de caractères
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

// Cette fonction convertit un message en binaire et stocke le résultat dans un tableau d'entiers
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

// Cette fonction convertit un tableau d'entiers en binaire en une chaîne de caractères
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

// Cette fonction calcule le taux d'erreur entre deux tableaux de bits
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

// Cette fonction écrit les temps de réception des bits dans un fichier
void write_to_file(int *timing_input, int message_length)
{
    FILE *f = fopen("result.txt", "w");
    for (int i = 0; i < message_length; i++)
    {
        fprintf(f, "%d\n", timing_input[i]);
    }
    fclose(f);
}

// Cette fonction calcule la capacité réelle d'un canal de communication en prenant en compte le taux d'erreur
double calc_true_capacity(double C, double p)
{
    double true_capacity = C * (1 + ((1 - p) * log2(1 - p) + p * log2(p)));
    return true_capacity;
}

int main()
{
    // Lecture du contenu du fichier "message.txt"
    const char *filename = "message.txt";
    char *file_content = read_file_content(filename);
    if (file_content == NULL)
        return 1;

    // Détermination de la longueur du message en octets
    int message_length = strlen(file_content);

    // Allocation de mémoire pour les tableaux de bits du message reçu
    int *received_binary = (int *)malloc(message_length * 8 * sizeof(int));
    int *received_timing = (int *)malloc(message_length * 8 * sizeof(int));

    // Mappage d'une adresse mémoire pour l'utilisation de la fonction clflush()
    void *cache_address;
    cache_address = map_offset("file", 0);

    uint32_t time_mesured;
    int i, somme, nb_val;

    clock_t start, end;
    double elapsed;

    // Nettoyage du cache avant de commencer la réception du message
    clflush(cache_address);

    // Synchronisation avec le cache pour garantir la bonne réception des bits
    synchronisation(SYNC_CYCLES_INIT);

    // Lancement de la réception des bits du message
    start = clock();
    for (i = 0; i < (message_length * 8); i++)
    {
        // Synchronisation avec le cache avant la lecture de chaque bit
        synchronisation(SYNC_CYCLES_LOOP);

        // Lecture du temps d'accès à l'adresse mémoire mappée pour chaque bit
        uint64_t time = rdtscp64();
        somme = 0;
        nb_val = 0;

        do
        {
            time_mesured = memaccesstime(cache_address);
            clflush(cache_address);
            somme += time_mesured;
            nb_val++;

            // Attente d'un délai pour garantir la bonne réception des bits
            delayloop(DELAYLOOP_VALUE);

        } while (rdtscp64() - time < DELAY_PER_BIT);

        // Conversion du temps d'accès en un bit 0 ou 1
        received_timing[i] = (double)somme / nb_val;
        received_binary[i] = ((double)somme / nb_val) < L3_THRESHOLD;
    }

    // Fin de la réception du message
    end = clock();
    elapsed = ((double)(end - start)) / CLOCKS_PER_SEC; /* Conversion en secondes  */

    // Conversion du tableau de bits reçu en une chaîne de caractères
    char *received_message = (char *)malloc(message_length + 1);
    binary_to_message(received_binary, received_message, message_length);

    // Écriture du tableau de temps d'accès dans un fichier "result.txt"
    write_to_file(received_timing, message_length * 8);

    // Conversion du message d'entrée en un tableau de bits
    int *message_binary = (int *)malloc(message_length * 8 * sizeof(int));
    message_to_binary(file_content, message_binary, message_length);

    // Calcul du taux d'erreur de réception
    double perc_error = calc_error(received_binary, message_binary, message_length * 8);

    // Calcul du débit en bits/seconde
    double C = (double)(message_length * 8) / elapsed;

    // Calcul de la capacité théorique maximale du canals
    double p = perc_error / 100;
    double true_capacity = calc_true_capacity(C, p);

    // Affichage des résultats
    printf("Message reçu: %s\n\n\nTaux d'erreur : %f\n", received_message, perc_error);
    printf("Débit : %f bits/s\n", C);
    printf("Capacité réelle (T) : %f bits/s\n", true_capacity);
    printf("%.4f secondes entre start et end.\n", elapsed);

    // Libération de la mémoire
    free(message_binary);
    free(received_timing);
    free(received_message);
    free(received_binary);

    return 0;
}

// Cette fonction permet de synchroniser le programme avec le cache du processeur
// Elle attend que les derniers bits de l'instruction RDTSCP soient égaux à 0
// avant de continuer l'exécution du programme
void synchronisation(int N)
{
    while ((rdtscp64() & ((1LL << N) - 1)) > 1000)
        ;
}
