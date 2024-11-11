#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>
#include <process.h>

typedef struct schedulerCDT *schedulerADT;

void createScheduler();

uint64_t schedule(uint64_t prevRSP);

int16_t createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);
int64_t blockProcess(int16_t pid);
int64_t readyProcess(int16_t pid);
PCB *findProcess(int16_t pid);
void yield();
int64_t killCurrentProcess();
int64_t killProcess(int16_t pid);
int64_t killForegroundProcess();
int16_t getPid();
int64_t getFileDescriptor(int64_t fd);

typedef struct PSinfo {
	int16_t pid;
	uint64_t stackBase;
	uint64_t stackPos;
	char *name;
	uint8_t priority;
	int ground;
	uint8_t status;
} PSinfo;

PSinfo *ps(uint16_t *proccesQty);
int16_t copyProcess(PSinfo *dest, PCB *src);

#endif
