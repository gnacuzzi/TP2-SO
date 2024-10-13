#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>
#include <process.h>


typedef struct schedulerCDT *schedulerADT;

void createScheduler();

uint64_t schedule(uint64_t prevRSP);

int64_t addProcessList(PCB * process);
int64_t addBlockProcess(PCB * process);
int64_t addReadyProcess(PCB * process);
int64_t killProcess(int16_t pid);
int64_t killCurrentProcess();

PCB *findProcess(int16_t pid);
void yield();
int16_t getPid();
//para poder hacer la de imprimir los procesos necesito la funcion de process que imprima?

#endif
