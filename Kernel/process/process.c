#include <process.h>
#include <videoDriver.h>
#include <memoryManager.h>

#define STACK_SIZE 4096 // 4KB

static char ** allocArgv(char ** argv);

void initProcess(PCB * pcb, uint16_t pid, uint16_t parentPid, Function function, char ** argv, char * name, uint8_t priority, uint16_t fileDescriptors[]){
    pcb->pid = pid;
    pcb->parentPid = parentPid;
    pcb->priority = priority;
    pcb->argv = allocArgv(argv); 
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

static char ** allocArgv(char ** argv){
    int argc = arrLen(argv);
    int totalLen = 0;
    for(int i = 0; i < argc; i++){
        totalLen += strlen(argv[i]) + 1;
    }
    char ** newArgv = (char **) malloc(totalLen + sizeof(char **) * (argc + 1));
    char * current = (char *) newArgv + ( sizeof(char **) * (argc + 1) );
    for(int i = 0; i < argc; i++){
        newArgv[i] = current;
        strcpy(current, argv[i]);
        current += strlen(argv[i]) + 1;
    }
    newArgv[argc] = NULL;
    return newArgv;
}