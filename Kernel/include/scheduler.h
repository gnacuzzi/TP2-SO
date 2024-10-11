#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>
#include <process.h>

#define SCHEDULER_ADRESS 0x705000

typedef struct schedulerCDT *schedulerADT;

void createScheduler();

uint64_t schedule(uint64_t prevRSP);

int16_t createProcess(uint64_t rip, char **args, int argc,char *name, uint8_t priority, int16_t fileDescriptors[]);
int64_t blockProcess(int16_t pid);
int64_t readyProcess(int16_t pid);
PCB *findProcess(int16_t pid);
void yield();
int64_t killCurrentProcess();
int64_t killProcess(int16_t pid);
int16_t getPid();
//para poder hacer la de imprimir los procesos necesito la funcion de process que imprima?

#endif
