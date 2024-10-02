#include <doubleLinkedList.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdint.h>


typedef struct schedulerCDT{
    doubleLinkedListADT processList;
    doubleLinkedListADT readyProcess;
    doubleLinkedListADT blockedProcess;
    doubleLinkedListADT zombieProcess;
    uint16_t currentPid;
    //PCB * currentProcess;
    uint16_t processQty;
    int quantums; //cantidad de quantums segun prioridad
}schedulerCDT;

static int created = 0;
static schedulerADT getScheduler();

void createScheduler(){
    schedulerADT scheduler = (schedulerADT) SCHEDULER_ADRESS;

    scheduler->processList = createDoubleLinkedListADT();
    scheduler->readyProcess = createDoubleLinkedListADT();
    scheduler->blockedProcess = createDoubleLinkedListADT();
    scheduler->zombieProcess = createDoubleLinkedListADT();
    scheduler->processQty = 0;
    created = 1;
    //scheduler->currentPid = idle
    //scheduler->currentProcess = NULL;
}

/*
void *schedule(void *prevRSP) {
    if (!created) return prevRSP;  
    schedulerADT scheduler = getScheduler();

    scheduler->quantums--;
    if (!scheduler->processQty || scheduler->quantums > 0) return prevRSP;  

    if (scheduler->currentProcess != NULL) {
        scheduler->currentProcess->rsp = prevRSP;      
        scheduler->currentProcess->status = READY;     
        addNode(scheduler->readyProcess, scheduler->currentProcess); 
    }

    PBC *firstProcess = getFirstData(scheduler->readyProcess);
    if (firstProcess == NULL) {
        return idle();  
    }

    scheduler->currentProcess = firstProcess;  
    scheduler->quantums = scheduler->currentProcess->priority;  
    scheduler->currentProcess->status = RUNNING;  

    return scheduler->currentProcess->rsp; 
}


uint16_t createProcess() {//no tengo en claro cuales serian los parametros todavia
    schedulerADT scheduler = getScheduler();

    PCB *newProcess = malloc(sizeof(PCB));  
    //initProcess(); //la hizo agos
    
    addElement(scheduler->processList, newProcess);  
    addElement(scheduler->readyProcess, newProcess);  

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

PCB *findProcess(pid_t pid) {
    schedulerADT scheduler = getScheduler();
    toBegin(scheduler->processList);
    PCB *aux;
    while (hasNext(scheduler->processList)) {
        next(scheduler->processList);
        if (aux->pid == pid)
            return aux;
    }
    return NULL;
}
*/



schedulerADT getScheduler(){
    return (schedulerADT) SCHEDULER_ADRESS;
}
