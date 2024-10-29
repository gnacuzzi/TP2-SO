// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <builtins.h>
#include <syscall.h>
#include "include/libc.h"

void memState(int argc, char *argv[]) {
    if(argc != 0){
        printf("Mem doesn't need parameters\n");
        return;
    }

    mem_info memInfo;

    if(sysgetMemInfo(&memInfo) == -1) {
        printf("Couldn't retrive memory information\n");
        return;
    }

    printf("Used memory: %d bytes\n", (int)memInfo.used);
    printf("Free memory: %d bytes\n", (int)memInfo.free);
    printf("Total memory: %d bytes\n", (int)memInfo.total);

    return;
}

/* Process Managing */

void killProcess(int argc, char *argv[]){
    if(argc != 1){
        printf("You must insert ONE parameter indicating the PID of the process you desire to kill\n");
        return;
    }

    int pid = atoi(argv[0]);

    if(pid < 1){ 
        printf("PID must be greater than 0\n");
        return;
    }

    int out = syskillProcess(pid);
    printf("Process %d %s\n", pid, out == 0 ? "killed" : "not killed");
    return;
}

void changePriority(int argc, char *argv[]){
    if(argc != 2){
        printf("You must insert TWO parameters indicating the PID of the process you desire to change the priority and the new priority\n");
        return;
    }

    int pid = atoi(argv[0]);
    int priority = atoi(argv[1]);

    if(pid < 0){
        printf("PID must be greater than 0\n");
        return;
    }

    if(priority < 1 || priority > 5){
        printf("Priority must be between 0 and 5\n");
        return;
    }

    int out = syschangePriority(pid, priority);

    printf("Priority of process %d %s\n", pid, out >= 0 ? "changed" : "not changed");

    return;
}

void blockProcess(int argc, char *argv[]){
    if(argc != 1){
        printf("You must insert ONE parameter indicating the PID of the process you desire to block\n");
        return;
    }

    int pid = atoi(argv[0]);

    if(pid < 0){
        printf("PID must be greater than 0\n");
        return;
    }

    int out = sysblockProcess(pid);

    printf("Process %d %s\n", pid, out == 0 ? "blocked" : "not blocked");

    return;
}

static void printProcInfo(PSinfo proc) {
    putchar('\n');
    printf("NAME: %s\n", proc.name == NULL ? "unnamed" : proc.name);

    printf("PID: %d\n", (int) proc.pid);

    printf("Priority: %d | Stack base: 0x%d | Stack pointer: 0x%d | Ground: %s\n", 
           (int) proc.priority, 
           (int) proc.stackBase, 
           (int) proc.stackPos, 
           proc.ground == 0 ? "foreground" : "background");
}

void listProcesses(int argc, char *argv[]) {
    uint16_t procAmount;
    
    PSinfo *processes = sysps(&procAmount);

    if (procAmount == 0 || processes == NULL) {
        printf("No processes found\n");
        return;
    }

    printf("There are %d processes:\n", procAmount);

    for (int i = 0; i < procAmount; i++) {
        printProcInfo(processes[i]);
    }

    for (int i = 0; i < procAmount; i++) {
        sysfree(processes[i].name);
    }

    sysfree(processes);
}
