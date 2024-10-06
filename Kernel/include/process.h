#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <vars.h>

#define CANT_FILE_DESCRIPTORS 5

typedef struct PCB{
    uint16_t pid;
    void *stackBase;
    void *stackPos;
    char **argv;
    char *name;
    uint8_t priority;
    Status status;
    int16_t fileDescriptors[CANT_FILE_DESCRIPTORS];
} PCB;


void initProcess(PCB *process, uint16_t pid, Function code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[]);
void * setupStackFrame(void *executeProcess, Function code, void *stackEnd, void *args);
int changePriority(uint16_t pid, uint8_t priority);
void freeProcess(PCB * pcb);


#endif