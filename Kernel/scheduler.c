#include <doubleLinkedList.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdint.h>
#include <process.h>
#include <lib.h>
#include <interrupts.h>

typedef struct schedulerCDT{
    doubleLinkedListADT processList;
    doubleLinkedListADT readyProcess;
    doubleLinkedListADT blockedProcess;
    doubleLinkedListADT zombieProcess; //todavia no tengo muy en claro si esta hace falta
    uint16_t currentPid;
    PCB * currentProcess;
    uint16_t processQty;
    int quantums; //cantidad de quantums segun prioridad
}schedulerCDT;

static int created = 0;
static schedulerADT getScheduler();
static void *idle();

void createScheduler(){
    schedulerADT scheduler = (schedulerADT) SCHEDULER_ADRESS;

    scheduler->processList = createDoubleLinkedListADT();
    scheduler->readyProcess = createDoubleLinkedListADT();
    scheduler->blockedProcess = createDoubleLinkedListADT();
    scheduler->zombieProcess = createDoubleLinkedListADT();
    scheduler->processQty = 0;
    created = 1;
    //scheduler->currentPid = idle
    scheduler->currentProcess = NULL;
}


void *schedule(void *prevRSP) {
    if (!created) return prevRSP;  
    schedulerADT scheduler = getScheduler();

    scheduler->quantums--;
    if (!scheduler->processQty || scheduler->quantums > 0) return prevRSP;  

    if (scheduler->currentProcess != NULL) {
        scheduler->currentProcess->stackPointer = prevRSP;      
        scheduler->currentProcess->status = READY;     
        addNode(scheduler->readyProcess, scheduler->currentProcess); 
    }

    PCB *firstProcess = getFirstData(scheduler->readyProcess);
    if (firstProcess == NULL) {
        return idle();  
    }

    scheduler->currentProcess = firstProcess;  
    scheduler->quantums = scheduler->currentProcess->priority;
    scheduler->currentPid = firstProcess->pid;  
    scheduler->currentProcess->status = RUNNING;  

    return scheduler->currentProcess->stackPointer; 
}


uint16_t createProcess() {//no tengo en claro cuales serian los parametros todavia
//como que desde aca tengo que definir que pid le doy y no se si poner una cantidad maxima 
//de procesos
    schedulerADT scheduler = getScheduler();

    PCB *newProcess = malloc(sizeof(PCB));  
    if (newProcess == NULL) {
        return -1;
    }
    //initProcess(); //la hizo agos
    
    addNode(scheduler->processList, newProcess);  
    addNode(scheduler->readyProcess, newProcess);  

    scheduler->processQty++;
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
    scheduler->quantums = 1;
    callTimerTick();
}

uint16_t getPid(){
    schedulerADT scheduler = getScheduler();
	return scheduler->currentPid;
}

schedulerADT getScheduler(){
    return (schedulerADT) SCHEDULER_ADRESS;
}

static void *idle() {
    while (1) {
        _hlt();
    }
    return NULL; //por ahora lo dejo asi, es solo para evitar advertencias
}