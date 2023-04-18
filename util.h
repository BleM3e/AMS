#ifndef UTIL_H
#define UTIL_H 1

//DELAYLOOP_VALUE : durée d'attente entre les mesures
#define DELAYLOOP_VALUE 3500

//DELAY_FOR_EACH_BIT : durée entre les transmissions de bits
#define DELAY_FOR_EACH_BIT 600000

// CACHE_SYNC_INIT : durée initiale de synchronisation
#define CACHE_SYNC_INIT 36

// CACHE_SYNC_LOOP : durée de synchronisation pour chaque itération
#define CACHE_SYNC_LOOP 18

void synchronisation(int N);

#endif // UTIL_H
