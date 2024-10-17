#include "include/builtins.h"
#include "include/syscall.h"
#include <stdio.h>

void memState(int argc, char *argv[]) {
    if(argc != 1) {
        printf("MemState doesn't need parameters\n");
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