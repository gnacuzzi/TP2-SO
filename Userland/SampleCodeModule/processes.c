// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/processes.h"
#include "include/tests.h"
#include <stdio.h>

void testMemory(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams){
    
    if(cantParams != 2){
        printf("TestMM needs two parameters\n");
    }

    char *params[] = { parameters[1] };
    int out = test_mm(1, params);

    printf("Memory test %s\n", out == 0 ? "passed" : "failed");

}

void testPrio(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams){
    test_prio();
}

void testProcesses(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams){
    if (cantParams != 2) {
        printf("TestProcesses needs two parameters\n");
        return;
    }

    if (satoi(parameters[1]) <= 0) {
        printf("Number of processes must be greater than 0\n");
        return;
    }

    char *params[] = {parameters[1]};

    int out = test_processes(1, params);

    printf("Process test %s\n", out == 0 ? "passed" : "failed");
}
