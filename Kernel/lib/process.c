#include <process.h>
#include <scheduler.h>
#include <videoDriver.h>
#include <memoryManager.h>

#define STACK_SIZE 4096 // 4KB
#define MAX_PRIORITY 5
#define MIN_PRIORITY 1

static char ** allocArgv(char ** argv, int argc);

void initProcess(PCB *process, uint16_t pid,
                 uint64_t rip, char **args, int argc, char *name,
                 uint8_t priority, int16_t fileDescriptors[]) {
    process->pid = pid;
    process->stackBase = (uint64_t) malloc(STACK_SIZE);
    process->argv = allocArgv(args, argc);
    process->argc = argc;
    process->name = malloc(strlen(name) + 1);
    strcpy(process->name, name);
    process->priority = priority;
    process->rip = rip;
    process->stackPos = setupStackFrame(process->stackBase, process->rip, argc, process->argv);
    process->status = READY;

    // Inicializar descriptores de archivo básicos
    for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
        process->fileDescriptors[i] = fileDescriptors[i];
    }
}

void freeProcess(PCB * pcb){
    free(pcb->argv);
    free(pcb->name);
    free((void *)pcb->stackBase);
    free(pcb);
}

static char ** allocArgv(char ** argv, int argc){
    int cant = argc;
    int totalLen = 0;
    for(int i = 0; i < cant; i++){
        totalLen += strlen(argv[i]) + 1;
    }
    char ** newArgv = (char **) malloc(totalLen + sizeof(char **) * (cant + 1));
    char * current = (char *) newArgv + ( sizeof(char **) * (cant + 1) );
    for(int i = 0; i < cant; i++){
        newArgv[i] = current;
        strcpy(current, argv[i]);
        current += strlen(argv[i]) + 1;
    }
    newArgv[cant] = NULL;
    return newArgv;
}

int changePriority(uint16_t pid, uint8_t priority) {
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

int waitProcess(uint16_t pid) {
    PCB * process = findProcess(pid);
    uint16_t currentPid = getPid();

    if(process == NULL || currentPid == pid) {
        return -1;
    }
    addNode(process->waitingList, process);
    blockProcess(currentPid);
    return 0;
}