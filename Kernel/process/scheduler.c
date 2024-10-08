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
    uint16_t currentPid;
    uint16_t nextPid;
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
    createProcess((Function) &idle, argsIdle, "idle", 1, NULL);//no se tema fds
}


void *schedule(void *prevRSP) {
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


uint16_t createProcess(Function code, char **args, char *name, uint8_t priority, int16_t fileDescriptors[]) {
    schedulerADT scheduler = getScheduler();
    
    if(scheduler->processQty > MAX_PROCESS) return -1;

    PCB *newProcess = malloc(sizeof(PCB));  
    if (newProcess == NULL) {
        return -1;
    }
    initProcess(newProcess, scheduler->nextPid, code, args, name, priority, fileDescriptors);
    
    addNode(scheduler->processList, newProcess);  
    if(scheduler->nextPid != IDLE_PID){//no quiero que el idle este en la lista de ready
        addNode(scheduler->readyProcess, newProcess);
    }
      
    scheduler->processQty++;
    scheduler->nextPid++;
    return newProcess->pid;  
}

void blockProcess(uint16_t pid) {
    schedulerADT scheduler = getScheduler();
    PCB *process = findProcess(pid);  
    if (process != NULL && process->status == READY) {
        removeNode(scheduler->readyProcess, process);  
        addNode(scheduler->blockedProcess, process);  
        process->status = BLOCKED;
    }
}

void readyProcess(uint16_t pid) {
    schedulerADT scheduler = getScheduler();
    PCB *process = findProcess(pid);  
    if (process != NULL && process->status == BLOCKED) {
        removeNode(scheduler->blockedProcess, process);  
        addNode(scheduler->readyProcess, process);  
        process->status = READY;
    }
}

void killCurrentProcess(){
    schedulerADT scheduler = getScheduler();
    killProcess(scheduler->currentProcess->pid);
}

void killProcess(uint16_t pid) {
    schedulerADT scheduler = getScheduler();
    PCB *process = findProcess(pid);  
    if (process == NULL) {
        return;  
    }

    if (process->status == READY) {
        removeNode(scheduler->readyProcess, process);
    }
    else if (process->status == BLOCKED) {
        removeNode(scheduler->blockedProcess, process);
    }

    toBegin(process->waitingList);
    PCB *aux;
    while(hasNext(process->waitingList)) {
        aux = nextInList(process->waitingList);
        readyProcess(aux->pid);
    }

    removeNode(scheduler->processList, process);

    scheduler->processQty--;
    freeProcess(process);
}


PCB *findProcess(uint16_t pid) {
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

uint16_t getPid(){
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