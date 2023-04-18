#ifndef UTIL_H
#define UTIL_H 1

//DELAYLOOP_VALUE : durée d'attente entre les mesures
#define DELAYLOOP_VALUE 3500

//DELAY_PER_BIT : durée entre les transmissions de bits
#define DELAY_PER_BIT 600000

// SYNC_CYCLES_INIT : durée initiale de synchronisation
#define SYNC_CYCLES_INIT 36

// SYNC_CYCLES_LOOP : durée de synchronisation pour chaque itération
#define SYNC_CYCLES_LOOP 18

void synchronisation(int N);

#endif // UTIL_H
