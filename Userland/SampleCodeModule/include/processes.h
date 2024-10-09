#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include "test_util.h"

void test_mm(uint64_t argc, char *argv[]);
void test_prio();
int64_t test_processes(uint64_t argc, char *argv[]);


#endif
