#include <stdint.h>
#include <syscall.h>
#include <test_util.h>
#include <stddef.h>
#include <stdio.h>

#define SEM_ID 11
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  sysyield(); 
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 4){
    sysexit();
    return -1;
  }
  if ((n = satoi(argv[1])) <= 0){
    sysexit();
    return -1;
  }
  if ((inc = satoi(argv[2])) == 0){
    sysexit();
    return -1;
  }
  if ((use_sem = satoi(argv[3])) < 0){
    sysexit();
    return -1;
  }
  if (use_sem)
    if (syssemOpen(SEM_ID) == -1) {
      printf("test_sync: ERROR opening semaphore\n");
      sysexit();
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem){
      syswait(SEM_ID);
    }
    slowInc(&global, inc);
    if (use_sem){
      syspost(SEM_ID);
    }
  }

  sysexit();
  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  int8_t use_sem = satoi(argv[1]);
	if (use_sem) {
		if (syssemInit(SEM_ID, 1) == -1) {
			printf("test_sync: ERROR creating semaphore\n");
			return -1;
		}
	}

  char *argvDec[] = {"my_process_inc", argv[0], "-1", argv[1]};
  char *argvInc[] = {"my_process_inc", argv[0], "1", argv[1]};
  int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR}; // stdin, stdout, stderr

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = syscreateProcess((uint64_t)my_process_inc, argvDec, 4, 1, fileDescriptors, 1);
    pids[i + TOTAL_PAIR_PROCESSES] = syscreateProcess((uint64_t)my_process_inc, argvInc, 4, 1, fileDescriptors, 1);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    syswaitProcess(pids[i]);
    syswaitProcess(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  printf("test_sync: Final value: %d\n", global);

  if (use_sem)
    syssemClose(SEM_ID);

  return 0;
}
