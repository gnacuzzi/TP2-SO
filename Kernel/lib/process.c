// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process.h>
#include <scheduler.h>
#include <videoDriver.h>
#include <memoryManager.h>
#include <doubleLinkedList.h>

#define MAX_PRIORITY 5
#define MIN_PRIORITY 1

static char **allocArgv(PCB *pcb, char **argv, int argc);

static void freeArgv(PCB *pcb, char **argv, int argc);

int initProcess(PCB *process, int16_t pid, uint64_t rip, char **args, int argc, uint8_t priority,
				int16_t fileDescriptors[], int ground) {
	process->pid = pid;
	process->stackBase = (uint64_t) malloc(STACK_SIZE) + STACK_SIZE;
	if (process->stackBase - STACK_SIZE == 0) {
		return -1;
	}
	process->argv = allocArgv(process, args, argc);
	if (process->argv == NULL) {
		free((void *) (process->stackBase - STACK_SIZE));
		return -1;
	}
	process->argc = argc;
	process->name = malloc(strlen(args[0]) + 1);
	if (process->name == NULL) {
		free((void *) (process->stackBase - STACK_SIZE));
		freeArgv(process, process->argv, argc);
		return -1;
	}
	strcpy(process->name, args[0]);
	process->priority = priority;
	process->rip = rip;
	process->stackPos = setupStackFrame(process->stackBase, process->rip, argc, process->argv);
	if (process->pid > 1) {
		process->status = BLOCKED;
	}
	else {
		process->status = READY;
	}
	process->waitingList = createDoubleLinkedListADT();
	if (process->waitingList == NULL) {
		free((void *) (process->stackBase - STACK_SIZE));
		freeArgv(process, process->argv, argc);
		free(process->name);
		return -1;
	}

	for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
		process->fileDescriptors[i] = fileDescriptors[i];
	}

	process->ground = ground;

	return 0;
}

void freeProcess(PCB *pcb) {
	freeArgv(pcb, pcb->argv, pcb->argc);
	free(pcb->name);
	free((void *) pcb->stackBase - STACK_SIZE);
	freeLinkedListADT(pcb->waitingList);
	free(pcb);
}

static char **allocArgv(PCB *pcb, char **argv, int argc) {
	char **newArgv = malloc((argc + 1) * sizeof(char *));
	if (newArgv == NULL) {
		return NULL;
	}
	for (int i = 0; i < argc; i++) {
		newArgv[i] = malloc(strlen(argv[i]) + 1);

		if (newArgv[i] == NULL) {
			for (int j = 0; j < i; j++) {
				free(newArgv[j]);
			}
			free(newArgv);
			return NULL;
		}

		strcpy(newArgv[i], argv[i]);
	}
	newArgv[argc] = NULL;
	return newArgv;
}

static void freeArgv(PCB *pcb, char **argv, int argc) {
	if (argv == NULL) {
		return;
	}

	for (int i = 0; i < argc; i++) {
		if (argv[i] != NULL) {
			free(argv[i]);
		}
	}

	free(argv);
}

int changePriority(int16_t pid, uint8_t priority) {
	if (priority > MAX_PRIORITY || priority < MIN_PRIORITY) {
		return -1;
	}

	PCB *process = findProcess(pid);
	if (process == NULL) {
		return -1;
	}
	process->priority = priority;
	return priority;
}

int waitProcess(int16_t pid) {
	PCB *process = findProcess(pid);
	int16_t currentPid = getPid();

	if (process == NULL || currentPid == pid) {
		return -1;
	}
	PCB *currentProcess = findProcess(currentPid);
	addNode(process->waitingList, currentProcess);
	blockProcess(currentPid);
	return 0;
}

int changeFds(int16_t pid, int16_t fileDescriptors[]) {
	PCB *process = findProcess(pid);
	if (process == NULL) {
		return -1;
	}
	for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
		process->fileDescriptors[i] = fileDescriptors[i];
	}
	return 0;
}