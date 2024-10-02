#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>

#define SCHEDULER_ADRESS 0x705000

typedef struct schedulerCDT *schedulerADT;

void createScheduler();
/*
void * schedule(void * prevRSP);

uint16_t createProcess();//faltan los parametros que recibe
void blockProcess(uint16_t pid);
void readyProcess(uint16_t pid);
PCB *findProcess(pid_t pid);

//comento todo porque en realidad deberiamos mergear las ramas porque yo uso cosas que hizo agos
*/

//aca me faltaria hacer la funcion que mata a los procesos
//yield

#endif