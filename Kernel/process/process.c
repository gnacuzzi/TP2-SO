#include <process.h>
#include <videoDriver.h>
#include <memoryManager.h>

#define STACK_SIZE 4096 // 4KB

void initProcess(PCB * pcb, uint16_t pid, uint16_t parentPid, Function function, char ** argv, char * name, uint8_t priority, uint16_t fileDescriptors[]){
    pcb->pid = pid;
    pcb->parentPid = parentPid;
    pcb->priority = priority;
    pcb->argv = allocArgv(argv); //TODO: allocArgv
    pcb->name = malloc(strlen(name) + 1);
    strcpy(pcb->name, name);
    pcb->stackBase = malloc(STACK_SIZE);
    pcb->stackPointer = pcb->stackBase + STACK_SIZE; // creo que aca hay que agragr más cosas
    pcb->status = READY;
}

void freeProcess(PCB * pcb){
    free(pcb->argv);
    free(pcb->name);
    free(pcb->stackBase);
    free(pcb);
}