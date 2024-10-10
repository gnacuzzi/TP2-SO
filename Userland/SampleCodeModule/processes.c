// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/processes.h"

void testMemory(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams){
    
    if(cantParams != 2){
        printf("TestMM needs two parameters");
    }

    char * mmParams[] = { parameters[1] };
    int out = test_mm(1, mmParams);

    printf("Memory test %s\n", out == 0 ? "passed" : "failed");

}

void testPrio(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams){

}

void testProcesses(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams){

}
