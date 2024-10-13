#include <process.h>
#include <scheduler.h>
#include <videoDriver.h>
#include <memoryManager.h>
#include <doubleLinkedList.h>

#define MAX_PRIORITY 5
#define MIN_PRIORITY 1
#define MAX_PROCESS 30
#define KERNEL_PID -1

static int16_t nextPid = 0;


int16_t initProcess(uint64_t rip, char **args, int argc, char *name,
                 uint8_t priority, int16_t fileDescriptors[]) {
    
    PCB * newProcess = malloc(sizeof(PCB));
    if(newProcess == NULL){
        return -1;
    }

    newProcess->stackBase = (uint64_t) malloc(STACK_SIZE) + STACK_SIZE;
    if(newProcess->stackBase - STACK_SIZE == 0) {
        free(newProcess);
        return -1;
    }

    newProcess->rip = rip;
    newProcess->argc = argc;
    newProcess->name = name;

    newProcess->argv = malloc((argc) * sizeof(char *));
    if(newProcess->argv == NULL){
        free((void *) (newProcess->stackBase - STACK_SIZE));
        free(newProcess);
        return -1;
    }

    for (int i = 0; i < argc; i++){
        newProcess->argv[i] = malloc(strlen(args[i])+1);
        if(newProcess->argv[i] == NULL){
            free((void *) (newProcess->stackBase - STACK_SIZE));
            free(newProcess->argv);
            free(newProcess);
            return -1;
        }

        strcpy(newProcess->argv[i], args[i]);
    }
    newProcess->argv[argc] = NULL;

    newProcess->status = READY;
    newProcess->priority = priority;
    newProcess->pid = nextPid;
    nextPid++;
    newProcess->waitingList = createDoubleLinkedListADT();
    if(newProcess->waitingList == NULL){
        freeProcess(newProcess);
        return -1;
    }

    newProcess->stackPos = setupStackFrame(newProcess->stackBase, newProcess->rip, newProcess->argc, newProcess->argv);

    for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
        newProcess->fileDescriptors[i] = fileDescriptors[i];
    }

    if(addProcessList(newProcess) == -1){
        freeProcess(newProcess);
        return -1;
    }
    
    if(nextPid != 0){
        if(addReadyProcess(newProcess) == -1){
            freeProcess(newProcess);
            return -1;
        }
    }

    return newProcess->pid;
}

void freeProcess(PCB * pcb){
    for (int i = 0; i < pcb->argc; i++) {
        free(pcb->argv[i]);
    }

    free(pcb->argv);
    free((void *)(pcb->stackBase - STACK_SIZE));
    freeLinkedListADT(pcb->waitingList);
    free(pcb);
}

int changePriority(int16_t pid, uint8_t priority) {
    if(priority>MAX_PRIORITY || priority<MIN_PRIORITY) {
        return -1;
    }

    PCB * process = findProcess(pid);
    if(process == NULL) {
        return -1;
    }
    process->priority = priority;
    return priority;
}

int waitProcess(int16_t pid) {
    PCB * process = findProcess(pid);
    int16_t currentPid = getPid();

    if(process == NULL || currentPid == pid) {
        return -1;
    }
    if(addNode(process->waitingList, process) == NULL){
        return -1;
    }
    
    blockProcess(currentPid);
    return 0;
}


int64_t readyProcess(int16_t pid){
    PCB * process = findProcess(pid);

    if (process == NULL || process->status == READY) {
        return -1;
    }
    if(addReadyProcess(process) == -1){
        return -1;
    }
    process->status = READY;

    return 0;
}

int64_t blockProcess(int16_t pid){
    PCB * process = findProcess(pid);

    if(process == NULL || process->status == BLOCKED) {
        return -1;
    }

    if (addBlockProcess(process) == -1){
        return -1;
    }
    
    process->status = BLOCKED;
    if (pid == getPid()) {
        yield();
    }

    return 0;
}
