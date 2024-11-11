// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <semaphore.h>
#include <stdint.h>
#include <doubleLinkedList.h>
#include <stddef.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <process.h>

#define MAX_SEMAPHORES 30

typedef struct semaphore {
	uint32_t value;
	uint8_t lock;
	uint8_t used;
	doubleLinkedListADT waiting;
} semaphore_t;

typedef struct SemaphoreCDT {
	semaphore_t semaphores[MAX_SEMAPHORES];
} SemaphoreCDT;

extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);

SemaphoreADT semaphore = NULL;
static SemaphoreADT getSemaphore();

void startSemaphores() {
	semaphore = (SemaphoreADT) malloc(sizeof(SemaphoreCDT));

	if (semaphore == NULL) {
		return;
	}

	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		semaphore->semaphores[i].value = 0;
		semaphore->semaphores[i].lock = 0;
		semaphore->semaphores[i].used = 0;
		semaphore->semaphores[i].waiting = createDoubleLinkedListADT();

		if (semaphore->semaphores[i].waiting == NULL) {
			free(semaphore);
			return;
		}
	}
}

int8_t semInit(uint16_t id, uint32_t initialValue) {
	if (id >= MAX_SEMAPHORES) {
		return -1;
	}

	SemaphoreADT sem = getSemaphore();

	if (!sem->semaphores[id].used) {
		sem->semaphores[id].value = initialValue;
		sem->semaphores[id].used = 1;
		return 0;
	}

	return -1;
}

int8_t semOpen(uint16_t id) {
	if (id >= MAX_SEMAPHORES) {
		return -1;
	}

	SemaphoreADT sem = getSemaphore();
	return -1 * (sem->semaphores[id].used == 0);
}

int8_t semClose(uint16_t id) {
	if (id >= MAX_SEMAPHORES) {
		return -1;
	}

	SemaphoreADT sem = getSemaphore();

	if (!sem->semaphores[id].used) {
		return -1;
	}

	freeLinkedListADT(sem->semaphores[id].waiting);
	sem->semaphores[id].waiting = createDoubleLinkedListADT();
	sem->semaphores[id].value = 0;
	sem->semaphores[id].used = 0;
	sem->semaphores[id].lock = 0;

	return 0;
}

int8_t post(uint16_t id) {
	if (id >= MAX_SEMAPHORES) {
		return -1;
	}
	SemaphoreADT sem = getSemaphore();
	if (!sem->semaphores[id].used) {
		return -1;
	}

	acquire(&sem->semaphores[id].lock);

	while (!isEmpty(sem->semaphores[id].waiting)) {
		int16_t *pidPtr = (int16_t *) getFirstData(sem->semaphores[id].waiting);
		int16_t pid = *pidPtr;
		PCB *process = findProcess(pid);

		if (process == NULL || process->status == KILLED) {
			free(pidPtr);
			continue;
		}

		free(pidPtr);
		readyProcess(pid);
		break;
	}

	if (isEmpty(sem->semaphores[id].waiting)) {
		sem->semaphores[id].value++;
	}

	release(&sem->semaphores[id].lock);

	return 0;
}

int8_t wait(uint16_t id) {
	if (id >= MAX_SEMAPHORES) {
		return -1;
	}
	SemaphoreADT sem = getSemaphore();
	if (!sem->semaphores[id].used) {
		return -1;
	}

	acquire(&sem->semaphores[id].lock);

	if (sem->semaphores[id].value > 0) {
		sem->semaphores[id].value--;
		release(&sem->semaphores[id].lock);
		return 0;
	}
	int16_t *pidPtr = (int16_t *) malloc(sizeof(int16_t));
	if (pidPtr == NULL) {
		release(&sem->semaphores[id].lock);
		return -1;
	}
	*pidPtr = getPid();
	addNode(sem->semaphores[id].waiting, (void *) pidPtr);
	release(&sem->semaphores[id].lock);
	blockProcess(*pidPtr);
	return 0;
}

static SemaphoreADT getSemaphore() {
	return semaphore;
}
