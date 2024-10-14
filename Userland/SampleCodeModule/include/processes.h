#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include "tests.h"

#define MAX_PROCESSES 40

void testMemory(int argc, char *argv[]);
void testPrio(int argc, char *argv[]);
void testProcesses(int argc, char *argv[]);

#endif