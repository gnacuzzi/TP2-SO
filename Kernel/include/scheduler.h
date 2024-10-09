#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>
#include <process.h>

#define SCHEDULER_ADRESS 0x705000

typedef struct schedulerCDT *schedulerADT;

void createScheduler();

void * schedule(void * prevRSP);

uint16_t createProcess(Function code, char **args, int argc,char *name, uint8_t priority, int16_t fileDescriptors[]);
void blockProcess(uint16_t pid);
void readyProcess(uint16_t pid);
PCB *findProcess(uint16_t pid);
void yield();
void killCurrentProcess();
void killProcess(uint16_t pid);
uint16_t getPid();
//para poder hacer la de imprimir los procesos necesito la funcion de process que imprima?

#endif
