// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/processes.h"
#include "include/tests.h"
#include "include/syscall.h"
#include <stdio.h>

void testMemory(int argc, char *argv[]){
    if(argc != 2){
        printf("You must insert ONE parameter indicating the memory size you desire to test\n");
        sysexit();
    }

    char *params[] = { argv[1] };
    int out = test_mm(1, params);

    printf("Memory test %s\n", out == 0 ? "passed" : "failed");
    sysexit();
}

void testPrio(int argc, char *argv[]){
    if(argc != 1){
        printf("TestPrio doesn't need parameters\n");
        sysexit();
    }
    test_prio();
    sysexit();
}

void testProcesses(int argc, char *argv[]){
    printf("%d", argc);
    if (argc != 2) {
        printf("You must insert ONE parameter indicating the number of processes you desire to test\n");
        sysexit();;
    }

    if (satoi(argv[1]) <= 0) {
        printf("Number of processes must be greater than 0\n");
        sysexit();;
    }

    char *params[] = {argv[1]};

    int out = test_processes(1, params);

    printf("Process test %s\n", out == 0 ? "passed" : "failed");
    sysexit();
}
