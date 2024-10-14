#include <doubleLinkedList.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdint.h>
#include <process.h>
#include <lib.h>
#include <interrupts.h>
#include <stdio.h>

#define MAX_PROCESS 60
#define KERNEL_PID -1
#define IDLE_PID 0


typedef struct schedulerCDT{
    doubleLinkedListADT processList;
    doubleLinkedListADT readyProcess;
    doubleLinkedListADT blockedProcess;
    doubleLinkedListADT zombieProcess; //todavia no tengo muy en claro si esta hace falta
    int16_t currentPid;
    int16_t nextPid;
    PCB * currentProcess;
    uint16_t processQty;
    int quantums; //cantidad de quantums segun prioridad
}schedulerCDT;

static int created = 0;
static schedulerADT getScheduler();
static void idle();
schedulerADT scheduler = NULL;

void createScheduler(){
    scheduler = (schedulerADT) malloc(sizeof(schedulerCDT));
    if(scheduler == NULL){
        return;
    }

    scheduler->processList = createDoubleLinkedListADT();
    scheduler->readyProcess = createDoubleLinkedListADT();
    scheduler->blockedProcess = createDoubleLinkedListADT();
    scheduler->zombieProcess = createDoubleLinkedListADT();
    scheduler->processQty = 0;
    created = 1;
    scheduler->nextPid = 0;
    scheduler->currentPid = -1;
    scheduler->currentProcess = NULL;
    char *argsIdle[2] = {"idle", NULL};
    createProcess((uint64_t)idle, argsIdle, 1,"idle", 1, NULL);//no se tema fds
}

uint64_t schedule(uint64_t prevRSP) {
    if (!created) return prevRSP;  
    schedulerADT scheduler = getScheduler();

    scheduler->quantums--;
    if (!scheduler->processQty || scheduler->quantums > 0) return prevRSP;  

    // Manejo del caso del kernel
    if (scheduler->currentPid == KERNEL_PID) {
        scheduler->currentProcess = getFirstData(scheduler->readyProcess);
        if(scheduler->currentProcess == NULL){//no llego a crearse la shell
            return prevRSP;
        }
        scheduler->currentPid = scheduler->currentProcess->pid;
        scheduler->quantums = scheduler->currentProcess->priority;
        scheduler->currentProcess->status = RUNNING;
        return scheduler->currentProcess->stackPos;
    }

    // Guardar el contexto del proceso actual
    if (scheduler->currentProcess != NULL) {
        scheduler->currentProcess->stackPos = prevRSP;      
        if (scheduler->currentProcess->status != BLOCKED) {
            scheduler->currentProcess->status = READY;     
            addNode(scheduler->readyProcess, scheduler->currentProcess); 
        }
    }

    // Seleccionar el próximo proceso
    while (1) {
        PCB *nextProcess = getFirstData(scheduler->readyProcess);
        if (nextProcess == NULL) {
            // Si no hay procesos listos, ejecutar el proceso idle
            PCB *idleProcess = findProcess(IDLE_PID);
            if (idleProcess == NULL) {
                return prevRSP;
            }
            scheduler->currentProcess = idleProcess;
            scheduler->currentPid = IDLE_PID;
        } else {
            scheduler->currentProcess = nextProcess;
            scheduler->currentPid = nextProcess->pid;
        }

        // Verificar los límites del stack
        if (scheduler->currentProcess->stackPos < scheduler->currentProcess->stackBase - STACK_SIZE || 
            scheduler->currentProcess->stackPos > scheduler->currentProcess->stackBase) {
            printf("Error: stackPos fuera de los límites del stack para el proceso %d\n", scheduler->currentProcess->pid);
            killProcess(scheduler->currentProcess->pid);
            continue;
        }

        break;
    }

    scheduler->quantums = scheduler->currentProcess->priority;
    scheduler->currentProcess->status = RUNNING;

    return scheduler->currentProcess->stackPos; 
}

int16_t createProcess(uint64_t rip, char **args, int argc,char *name, uint8_t priority, int16_t fileDescriptors[]) {
    schedulerADT scheduler = getScheduler();
    
    if(scheduler->processQty > MAX_PROCESS) return -1;

    PCB *newProcess = malloc(sizeof(PCB));  
    if (newProcess == NULL) {
        return -1;
    }
    if(initProcess(newProcess, scheduler->nextPid, rip, args, argc,name, priority, fileDescriptors) == -1){
        free(newProcess);
        return -1;
    }

    
    addNode(scheduler->processList, newProcess);  
    if(scheduler->nextPid != IDLE_PID){//no quiero que el idle este en la lista de ready
        addNode(scheduler->readyProcess, newProcess);
    }
      
    scheduler->processQty++;
    scheduler->nextPid++;
    return newProcess->pid;  
}

int64_t blockProcess(int16_t pid) {
    schedulerADT scheduler = getScheduler();
    PCB *process = findProcess(pid);
    if(process == NULL){
        return -1;
    }  
    if (process->status == READY) {
        if(removeNode(scheduler->readyProcess, process) == NULL){
            return -1;
        }
        if(addNode(scheduler->blockedProcess, process) == NULL){
            return -1;
        }
        process->status = BLOCKED;
    }
    return 0;
}

int64_t readyProcess(int16_t pid) {
    schedulerADT scheduler = getScheduler();
    PCB *process = findProcess(pid);
    if(process == NULL){
        return -1;
    }  
    if (process->status == BLOCKED) {
        if(removeNode(scheduler->blockedProcess, process) == NULL){
            return -1;
        }
        if(addNode(scheduler->readyProcess, process) == NULL){
            return -1;
        }
        process->status = READY;
    }
    return 0;
}

int64_t killCurrentProcess(){
    schedulerADT scheduler = getScheduler();
    return killProcess(scheduler->currentProcess->pid);
}

int64_t killProcess(int16_t pid) {
    schedulerADT scheduler = getScheduler();
    PCB *process = findProcess(pid);  
    if (process == NULL) {
        return -1;  
    }

    if (process->status == READY) {
        if(removeNode(scheduler->readyProcess, process) == NULL){
            return -1;
        }
    } else if (process->status == BLOCKED) {
        if(removeNode(scheduler->blockedProcess, process) == NULL){
            return -1;
        }
    }

    toBegin(process->waitingList);
    PCB *aux;
    while(hasNext(process->waitingList)) {
        aux = nextInList(process->waitingList);
        if(readyProcess(aux->pid) == -1){
            return -1;
        }
    }
    if(removeNode(scheduler->processList, process) == NULL){
        return -1;
    }

    scheduler->processQty--;
    freeLinkedListADT(process->waitingList);
    freeProcess(process);
    return 0;
}


PCB *findProcess(int16_t pid) {
    schedulerADT scheduler = getScheduler();
    toBegin(scheduler->processList);  
    PCB *aux;
    while (hasNext(scheduler->processList)) {
        aux = nextInList(scheduler->processList);  
        if (aux->pid == pid) {
            return aux;
        }
    }
    return NULL;  
}

void yield() {
    schedulerADT scheduler = getScheduler();
    scheduler->quantums = 0;
    callTimerTick();
}

int16_t getPid(){
    schedulerADT scheduler = getScheduler();
	return scheduler->currentPid;
}

schedulerADT getScheduler(){
    return scheduler;
}

static void idle() {
    while (1) {
        _hlt();
    }
}
