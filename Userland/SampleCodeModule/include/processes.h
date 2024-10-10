#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include "tests.h"

#define BUFFER_LENGTH 256
#define MAX_PARAMETERS 2 // todavia no sabemos cuantos parametros se van a enviar como maximo
#define PARAMETERS_LENGTH 256

void testMemory(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams);
void testPrio(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams);
void testProcesses(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams);

#endif