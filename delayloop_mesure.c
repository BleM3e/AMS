#include <stdio.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    struct timespec start, end;
    long iterations = 25000;

    // Enregistrement du temps de début
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Appel à delayloop
    delayloop(iterations);

    // Enregistrement du temps de fin
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calcul de la différence de temps en secondes
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("delayloop(%ld) a pris %lf secondes.\n", iterations, elapsed_time);
    return 0;
}
