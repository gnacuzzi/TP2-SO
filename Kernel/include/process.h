#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <vars.h>

#define CANT_FILE_DESCRIPTORS 5

typedef struct PCB{
    uint16_t pid;
    uint16_t parentPid;
    uint8_t priority;
    int16_t fileDescriptors[CANT_FILE_DESCRIPTORS];
    void * stackBase;
    void * stackPointer;
    char ** argv;
    char * name;
    Status status;
} PCB;

void initProcess(PCB * pcb, uint16_t pid, uint16_t parentPid, Function function, char ** argv, char * name, uint8_t priority, uint16_t fileDescriptors[]);
void freeProcess(PCB * pcb);

#endif