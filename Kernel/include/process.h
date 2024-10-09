#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <vars.h>
#include <doubleLinkedList.h>

#define CANT_FILE_DESCRIPTORS 5

typedef struct PCB{
    int16_t pid;
    void *stackBase;
    void *stackPos;
    void *rip;
    char **argv;
    int argc;
    char *name;
    uint8_t priority;
    Status status;
    int16_t fileDescriptors[CANT_FILE_DESCRIPTORS];
    doubleLinkedListADT waitingList;
} PCB;


void initProcess(PCB *process, int16_t pid, Function code, char **args, int argc, char *name, uint8_t priority, int16_t fileDescriptors[]);
void * setupStackFrame(void * stackBase, uint64_t code,int argc, char * args[]);
int changePriority(uint16_t pid, uint8_t priority);
void freeProcess(PCB * pcb);
int waitProcess(uint16_t pid);

#endif