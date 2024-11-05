#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include "tests.h"

#define MAX_PROCESSES 40

/* Tests */
void testMemory(int argc, char *argv[]);
void testPrio(int argc, char *argv[]);
void testProcesses(int argc, char *argv[]);
void testSynchronization(int argc, char *argv[]);
void testNoSynchronization(int argc, char *argv[]);

/* Others */
void loop(int argc, char *argv[]);

/* Inter Process Communication */
void cat(int argc, char *argv[]);
void wc(int argc, char *argv[]);
void filter(int argc, char *argv[]);
void phylo(int argc, char *argv[]);

#endif