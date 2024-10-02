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
    //PCB currentProcess;
    uint16_t processQty;
}schedulerCDT;


void createScheduler(){
    schedulerADT scheduler = (schedulerADT) SCHEDULER_ADRESS;
    
    scheduler->processList = createDoubleLinkedListADT();
    scheduler->readyProcess = createDoubleLinkedListADT();
    scheduler->blockedProcess = createDoubleLinkedListADT();
    scheduler->zombieProcess = createDoubleLinkedListADT();
    scheduler->processQty = 0;
    //scheduler->currentPid = idle
    //scheduler->currentProcess = NULL;
}




