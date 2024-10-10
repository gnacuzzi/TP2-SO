#include <doubleLinkedList.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdint.h>
#include <process.h>
#include <lib.h>
#include <interrupts.h>

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

void createScheduler(){
    schedulerADT scheduler = (schedulerADT) SCHEDULER_ADRESS;

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

    if(scheduler->currentPid == KERNEL_PID){
        scheduler->currentProcess = getFirstData(scheduler->readyProcess);
        if(scheduler->currentProcess == NULL){//no llego a crearse la shell
            return prevRSP;
        }
        scheduler->currentPid = scheduler->currentProcess->pid;
        scheduler->quantums = scheduler->currentProcess->priority;
        scheduler->currentProcess->status = RUNNING;
        return scheduler->currentProcess->stackPos;
    }

    if (scheduler->currentProcess != NULL) {
        scheduler->currentProcess->stackPos = prevRSP;      
        scheduler->currentProcess->status = READY;     
        addNode(scheduler->readyProcess, scheduler->currentProcess); 
    }

    PCB *firstProcess = getFirstData(scheduler->readyProcess);
    if (firstProcess == NULL) {
        PCB *process = findProcess(IDLE_PID);
        if(process == NULL){
            return prevRSP;
        }else{
            return process->stackPos;
        }
    }

    scheduler->currentProcess = firstProcess;  
    scheduler->quantums = scheduler->currentProcess->priority;
    scheduler->currentPid = firstProcess->pid;  
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
    initProcess(newProcess, scheduler->nextPid, rip, args, argc,name, priority, fileDescriptors);
    
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
    return (schedulerADT) SCHEDULER_ADRESS;
}

static void idle() {
    while (1) {
        _hlt();
    }
}