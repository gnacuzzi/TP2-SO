#include <doubleLinkedList.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdint.h>
#include <process.h>
#include <lib.h>
#include <interrupts.h>
#include <stdio.h>

#define KERNEL_PID -1
#define IDLE_PID 0


typedef struct schedulerCDT{
    doubleLinkedListADT processList;
    doubleLinkedListADT readyProcess;
    doubleLinkedListADT blockedProcess;
    int16_t currentPid;
    PCB * currentProcess;
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
    created = 1;
    scheduler->currentPid = -1;
    scheduler->currentProcess = NULL;
    scheduler->quantums = 1;
    char *argsIdle[2] = {"idle"};
    initProcess((uint64_t)idle, argsIdle, 1,"idle", 1, NULL);//no se tema fds
}

uint64_t schedule(uint64_t prevRSP) {
    if (!created) return prevRSP;  
    schedulerADT scheduler = getScheduler();

    scheduler->quantums--;
    if (scheduler->quantums > 0) return prevRSP;  

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

int64_t addProcessList(PCB * process){
    schedulerADT scheduler = getScheduler();

    if(process == NULL){
        return -1;
    }
    if(addNode(scheduler->processList, process) == NULL){
        return -1;
    }
    return 0;
}

int64_t addBlockProcess(PCB * process){
    schedulerADT scheduler = getScheduler();
    if(process == NULL){
        return -1;
    }
    if(addNode(scheduler->blockedProcess, process) == NULL){
        return -1;
    }
    if(removeNode(scheduler->readyProcess, process) == NULL){
        return -1;
    }
    return 0;
}

int64_t addReadyProcess(PCB * process){
    schedulerADT scheduler = getScheduler();
    if(process == NULL){
        return -1;
    }
    if(addNode(scheduler->readyProcess, process) == NULL){
        return -1;
    }
    if(removeNode(scheduler->blockedProcess, process) == NULL){
        return -1;
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


    if(process->pid == scheduler->currentPid){
        yield();
    }

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
