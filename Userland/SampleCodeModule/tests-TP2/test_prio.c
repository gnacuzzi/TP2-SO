// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdint.h>
#include <stdio.h>
#include "../include/syscall.h"
#include "../include/tests.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT                                                                                                           \
	10000000 // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 5 // TODO: Change as required

uint8_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
	int64_t pids[TOTAL_PROCESSES];
	char *argv[] = {"endless_loop_print"};
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR}; // stdin, stdout, stderr

	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++) {
		pids[i] = syscreateProcess((uint64_t) endless_loop_print, argv, 1, LOWEST, fileDescriptors, 1);
		sysunblockProcess(pids[i]);
	}
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
