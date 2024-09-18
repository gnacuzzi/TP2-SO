#include "include/rand.h"

#define A 1664525
#define C 1013904223
#define M 4294967296 
unsigned long seed = 1; 

void srand(unsigned long s);

void srand(unsigned long s) {
    seed = s;
}

char rand_char() {
    seed = (A * seed + C) % M;
    return 'A' + (seed % 26);
}