#include "include/builtins.h"
#include "include/syscall.h"
#include "include/libc.h"

void memState(char *argv[], int argc) {
    if(argc != 0){
        printf("Mem doesn't need parameters\n");
        sysexit();
        return;
    }

    mem_info memInfo;

    if(sysgetMemInfo(&memInfo) == -1) {
        printf("Couldn't retrive memory information\n");
        sysexit();
        return;
    }

    printf("Used memory: %d MB\n", memInfo.used / 1024 / 1024);
    printf("Free memory: %d MB\n", memInfo.free / 1024 / 1024);
    printf("Total memory: %d MB\n", memInfo.total / 1024 / 1024);
    sysexit();
    return;
}

/* Process Managing */

void killProcess(char *argv[], int argc){
    if(argc != 1){
        printf("You must insert ONE parameter indicating the PID of the process you desire to kill\n");
        return;
    }

    const char *aux;
    int pid = strtoi(argv[1], &aux);

    if(pid < 0){
        printf("PID must be greater than 0\n");
        return;
    }

    int out = syskillProcess(pid);

    printf("Process %d %s\n", pid, out == 0 ? "killed" : "not killed");

    return;
}

void changePriority(char *argv[], int argc){
    if(argc != 2){
        printf("You must insert TWO parameters indicating the PID of the process you desire to change the priority and the new priority\n");
        return;
    }
    const char *aux;
    int pid = strtoi(argv[1], &aux);
    int priority = strtoi(argv[2], &aux);

    if(pid < 0){
        printf("PID must be greater than 0\n");
        return;
    }

    if(priority < 1 || priority > 5){
        printf("Priority must be between 0 and 5\n");
        return;
    }

    int out = syschangePriority(pid, priority);

    printf("Priority of process %d %s\n", pid, out == 0 ? "changed" : "not changed");

    return;
}

void blockProcess(char *argv[], int argc){
    if(argc != 1){
        printf("You must insert ONE parameter indicating the PID of the process you desire to block\n");
        return;
    }

    const char *aux;
    int pid = strtoi(argv[1], &aux);

    if(pid < 0){
        printf("PID must be greater than 0\n");
        return;
    }

    int out = sysblockProcess(pid);

    printf("Process %d %s\n", pid, out == 0 ? "blocked" : "not blocked");

    return;
}

