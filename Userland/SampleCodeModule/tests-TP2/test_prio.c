// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include "../include/syscall.h"
#include "test_util.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT                                                                                                           \
	10000000 // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 1  // TODO: Change as required
#define HIGHEST 2 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
	int64_t pids[TOTAL_PROCESSES];
	char *argv[] = {"endless_loop_print"};
	
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++)
		pids[i] = syscreateProcess(&endless_loop_print, argv, "endless_loop_print", LOWEST, 0);

	bussy_wait(WAIT);
	printf("\nCHANGING PRIORITIES...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		syschangePriority(pids[i], prio[i]);

	bussy_wait(WAIT);
	printf("\nBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		sysblockProcess(pids[i]);

	printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		syschangePriority(pids[i], MEDIUM);

	printf("UNBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		sysunblockProcess(pids[i]);

	bussy_wait(WAIT);
	printf("\nKILLING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		syskillProcess(pids[i]);
}
