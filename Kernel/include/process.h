#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <vars.h>
#include <doubleLinkedList.h>

#define CANT_FILE_DESCRIPTORS 5

typedef struct PCB{
    int16_t pid;
    uint64_t stackBase;
    uint64_t stackPos;
    uint64_t rip;
    char **argv;
    int argc;
    char *name;
    uint8_t priority;
    Status status;
    int16_t fileDescriptors[CANT_FILE_DESCRIPTORS];
    doubleLinkedListADT waitingList;
} PCB;


int initProcess(PCB *process, int16_t pid, uint64_t rip, char **args, int argc, char *name, uint8_t priority, int16_t fileDescriptors[]);
uint64_t setupStackFrame(uint64_t stackBase, uint64_t code,int argc, char * args[]);
int changePriority(int16_t pid, uint8_t priority);
void freeProcess(PCB * pcb);
int waitProcess(int16_t pid);
#endif