// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include/processes.h"
#include "include/tests.h"
#include "include/syscall.h"
#include "include/libc.h"
#include <stddef.h>

#define TICKS 0.055
#define MAX_BUFF 256

/* Tests */

void testMemory(int argc, char *argv[]){
    if(argc < 2){
        printf("You must insert ONE parameter indicating the memory size you desire to test\n");
        sysexit();
        return;
    }

    char *params[] = { argv[1] };
    int out = test_mm(1, params);

    printf("Memory test %s\n", out == 0 ? "passed" : "failed");
    sysexit();
    return;
}

void testPrio(int argc, char *argv[]){
    if(argc > 1){
        printf("TestPrio doesn't need parameters\n");
        sysexit();
        return;
    }
    test_prio();
    sysexit();
    return;
}

void testProcesses(int argc, char *argv[]){
    if (argc < 2) {
        printf("You must insert ONE parameter indicating the number of processes you desire to test\n");
        sysexit();
        return;
    }

    if (satoi(argv[1]) <= 0 || satoi(argv[1]) > MAX_PROCESSES) {
        printf("Number of processes must be between 0 and 40\n");
        sysexit();
        return;
    }

    char *params[] = {argv[1]};

    int out = test_processes(1, params);

    printf("Process test %s\n", out == 0 ? "passed" : "failed");
    sysexit();
    return;
}

void testSynchronization(int argc, char *argv[]){//{n, use_sem, 0}
    if (argc < 3) {
        printf("You must insert TWO parameters, one indicating the number of iterations and an other one indicating if you wish to use or not semaphores\n");
        sysexit();
        return;
    }

    if(satoi(argv[1]) <= 0){
        printf("Number of iterations must be greater than 0\n");
        sysexit();
        return;
    }

    if(satoi(argv[2]) < 0){
        printf("Write 0 for no-sem, write > 0 for sem\n");
        sysexit();
        return;
    }

    char * params[] = {argv[1], argv[2]};

    int out = test_sync(2, params);

    printf("Synchronization test finished %s errors\n", out == 0 ? "without" : "with");
    sysexit();
    return;
}


/* Other */

void loop(int argc, char *argv[]){
    if(argc != 2){
        printf("You must insert ONE parameter indicating the number of seconds you desire to test\n");
        sysexit();
        return;
    }
    int secs = atoi(argv[1]);

    if (secs < 0) {
        printf("Number of seconds must be greater than 0\n");
        sysexit();
        return;
    }
    int realTime = secs / TICKS;

    while (1) {
        printf("Hello World! PID: %d\n", sysgetpid());
        wait_delta(realTime);
    }
    
    sysexit();
    return;
}

/* Inter Process Communication */

void cat(int argc, char *argv[]){
    if(argc != 1){
        printf("Cat doesn't need parameters\n");
        sysexit();
        return;
    }

    char buffer[MAX_BUFF] = {0};
    printf("To stop double enter\n"); //en teoria esto deberia cortar cuando hago ctrl d pero no lo tenemos implementado todavia

    while (scanf(buffer) != -1) { 
        if(*buffer == '\0'){
            break;
        } 
    }

    sysexit();
    return;
}

void wc(int argc, char *argv[]){
    if(argc != 1){
        printf("Wc doesn't need parameters\n");
        sysexit();
        return;
    }

    printf("To stop click shift\n"); //en teoria esto deberia cortar cuando hago ctrl d pero no lo tenemos implementado todavia
    char c;
    int counter = 0;

    while (1) {
        c = readchar();

        if (c == '\n') { 
            counter++;
        }
        if (c == '\0') { 
            break; 
        } 
    }

    printf("Number of lines: %d\n", counter);

    sysexit();
    return;
}

static int isVowel(char c){
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {
        return 1;
    }
    return 0;
}

void filter(int argc, char *argv[]){
    if(argc != 1){
        printf("Filter doesn't need parameters\n");
        sysexit();
        return;
    }
    printf("To stop click shift\n"); //en teoria esto deberia cortar cuando hago ctrl d pero no lo tenemos implementado todavia

    char c;

    while(1){
        c = readchar();
        if(isVowel(c)){
            printf("%c\t", c);
        }
        if (c == '\0') { 
            printf("\n");
            break; 
        } 
    }

    sysexit();
    return;
}
