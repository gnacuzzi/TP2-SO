// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <doubleLinkedList.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdint.h>
#include <process.h>
#include <lib.h>
#include <interrupts.h>
#include <videoDriver.h>

#define MAX_PROCESS 60
#define KERNEL_PID -1
#define IDLE_PID 0


typedef struct schedulerCDT{
    doubleLinkedListADT processList;
    doubleLinkedListADT readyProcess;
    doubleLinkedListADT blockedProcess;
    int16_t currentPid;
    int16_t nextPid;
    PCB * currentProcess;
    uint16_t processQty;
    int quantums; 
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
    scheduler->processQty = 0;
    created = 1;
    scheduler->nextPid = 0;
    scheduler->currentPid = -1;
    scheduler->currentProcess = NULL;
    char *argsIdle[2] = {"idle"};
    int16_t fileDescriptors[] = {-1, -1, STDERR}; //devnull, devnull, stderror
    createProcess((uint64_t)idle, argsIdle, 1, 1, fileDescriptors);
}

uint64_t schedule(uint64_t prevRSP) {
    if (!created) return prevRSP;  
    schedulerADT scheduler = getScheduler();

    scheduler->quantums--;
    if (!scheduler->processQty || scheduler->quantums > 0) return prevRSP;  

    if(scheduler->currentPid == KERNEL_PID){
        scheduler->currentProcess = getFirstData(scheduler->readyProcess);
        if(scheduler->currentProcess == NULL){
            return prevRSP;
        }
        scheduler->currentPid = scheduler->currentProcess->pid;
        scheduler->quantums = scheduler->currentProcess->priority;
        scheduler->currentProcess->status = RUNNING;
        return scheduler->currentProcess->stackPos;
    }

    if (scheduler->currentProcess != NULL) {
        scheduler->currentProcess->stackPos = prevRSP;
        if(scheduler->currentProcess->status == RUNNING){
            scheduler->currentProcess->status = READY;     
            addNode(scheduler->readyProcess, scheduler->currentProcess); 
        }else if(scheduler->currentProcess->status == KILLED){
            freeProcess(scheduler->currentProcess);
        }
    }

    PCB *firstProcess = getFirstData(scheduler->readyProcess);
    if (firstProcess == NULL) {
        PCB *process = findProcess(IDLE_PID);
        if(process == NULL){
            return prevRSP;
        }else{
            scheduler->currentProcess = process;
            return process->stackPos;
        }
    }

    scheduler->currentProcess = firstProcess;  
    scheduler->quantums = scheduler->currentProcess->priority;
    scheduler->currentPid = firstProcess->pid;  
    scheduler->currentProcess->status = RUNNING;  

    return scheduler->currentProcess->stackPos; 
}

int16_t createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[]) {
    schedulerADT scheduler = getScheduler();
    
    if(scheduler->processQty > MAX_PROCESS) return -1;

    PCB *newProcess = malloc(sizeof(PCB));  
    if (newProcess == NULL) {
        return -1;
    }
    if(initProcess(newProcess, scheduler->nextPid, rip, args, argc, priority, fileDescriptors) == -1){
        free(newProcess);
        return -1;
    }

    
    addNode(scheduler->processList, newProcess);  
    if(scheduler->nextPid != IDLE_PID){
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
    if (process->status == READY || process->status == RUNNING) {
        if (process->status == READY){
            if(removeNode(scheduler->readyProcess, process) == NULL){
                return -1;
            }
        }
        if(addNode(scheduler->blockedProcess, process) == NULL){
            return -1;
        }
        process->status = BLOCKED;
    }
    if (pid == getPid()) {
        yield();
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
        readyProcess(aux->pid);
    }
    if(removeNode(scheduler->processList, process) == NULL){
        return -1;
    }
    process->status = KILLED;

    scheduler->processQty--;

    free(process);

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

PCB * ps(uint16_t * proccesQty){
    schedulerADT scheduler = getScheduler();
    if(scheduler->processList == NULL){
        *proccesQty = 0;
        return NULL;
    }
    
    PCB * processes = malloc(sizeof(PCB) * scheduler->processQty);
    if(processes == NULL){
        *proccesQty = 0;
        return NULL;
    }

    toBegin(scheduler->processList);
    PCB *aux;
    for (int i = 0; i < scheduler->processQty; i++){
        aux = nextInList(scheduler->processList);
        if(copyProcess(&processes[i], aux) == -1){
            free(processes[i].name);
            free(processes);
            *proccesQty = 0;
            return NULL;
        }
    }
    *proccesQty = scheduler->processQty;
    return processes;
}

int16_t copyProcess(PCB *dest, PCB *src){
    dest->pid = src->pid;
    dest->stackBase = src->stackBase;
    dest->stackPos = src->stackPos;
    dest->priority = src->priority;

    if(src->name != NULL){
        dest->name = malloc(strlen(src->name) + 1);
        if(dest->name == NULL){
            return -1;
        }
        strcpy(dest->name, src->name);

    }else{
        dest->name = NULL;
    }

    return 0;
}

schedulerADT getScheduler(){
    return scheduler;
}

static void idle() {
    while (1) {
        _hlt();
    }
}
