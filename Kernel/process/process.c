#include <process.h>
#include <videoDriver.h>
#include <memoryManager.h>

#define STACK_SIZE 4096 // 4KB

static char ** allocArgv(char ** argv);
static void executeProcess(Function code, char ** argv);

void initProcess(PCB *process, uint16_t pid,
                 Function code, char **args, char *name,
                 uint8_t priority, int16_t fileDescriptors[]) {
    process->pid = pid;
    process->stackBase = malloc(STACK_SIZE);
    process->argv = allocArgv(args);
    process->name = malloc(strlen(name) + 1);
    strcpy(process->name, name);
    process->priority = priority;
    void *stackEnd = (void *)((uint64_t)process->stackBase + STACK_SIZE);
    process->stackPos = setupStackFrame(&executeProcess, code, stackEnd, (void *)process->argv);
    process->status = READY;

    // Inicializar descriptores de archivo básicos
    for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
        process->fileDescriptors[i] = fileDescriptors[i];
    }
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

void executeProcess(Function code, char ** argv){
    code(arrLen(argv), argv);
    killCurrentProcess(); //TODO ver como hacer esto
}