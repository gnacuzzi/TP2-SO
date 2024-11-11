// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdint.h>
#include <stddef.h>
#include <pipes.h>
#include <scheduler.h>
#include <memoryManager.h>

#define PIPE_SIZE 512
#define MAX_PIPES 30
#define READ 0
#define WRITE 1

typedef struct pipe {
	char buffer[PIPE_SIZE];
	int readIndex;
	int writeIndex;
	int size;
	int64_t fd;
	int16_t inputPid, outputPid;
	int readBlocked, writeBlocked;
} pipe;

typedef struct PipesCDT {
	pipe pipes[MAX_PIPES];
	uint16_t cantPipes;
} PipesCDT;

static PipesADT pipesManager = NULL;

static PipesADT getPipes() {
	return pipesManager;
}

void startPipes() {
	pipesManager = (PipesADT) malloc(sizeof(PipesCDT));
	if (pipesManager == NULL) {
		return;
	}
	pipesManager->cantPipes = 0;
	for (int i = 0; i < MAX_PIPES; i++) {
		pipesManager->pipes[i].fd = -1;
	}
}

static pipe createPipe() {
	pipe newPipe = {.readIndex = 0,
					.writeIndex = 0,
					.size = 0,
					.fd = -1,
					.inputPid = -1,
					.outputPid = -1,
					.readBlocked = 0,
					.writeBlocked = 0};
	return newPipe;
}

int64_t openPipe(int16_t pid, uint8_t mode) {
	PipesADT pipes = getPipes();
	if (pipes->cantPipes >= MAX_PIPES) {
		return -1;
	}

	for (int i = 0; i < MAX_PIPES; i++) {
		if (pipes->pipes[i].fd != -1) {
			if (mode == READ && pipes->pipes[i].outputPid != -1) {
				pipes->pipes[i].inputPid = pid;
				return pipes->pipes[i].fd;
			}
			if (mode == WRITE && pipes->pipes[i].inputPid != -1) {
				pipes->pipes[i].outputPid = pid;
				return pipes->pipes[i].fd;
			}
		}
	}

	int availablePipeIdx = -1;
	for (int i = 0; i < MAX_PIPES; i++) {
		if (pipes->pipes[i].fd == -1) {
			availablePipeIdx = i;
			break;
		}
	}
	if (availablePipeIdx == -1) {
		return -1;
	}

	pipe newPipe = createPipe();
	newPipe.fd = availablePipeIdx + 3;
	if (mode == READ) {
		newPipe.inputPid = pid;
	}
	else if (mode == WRITE) {
		newPipe.outputPid = pid;
	}

	pipes->pipes[availablePipeIdx] = newPipe;
	pipes->cantPipes++;
	return newPipe.fd;
}

int64_t closePipe(int64_t fd) {
	PipesADT pipes = getPipes();
	if (fd < 3 || fd >= MAX_PIPES + 3) {
		return -1;
	}

	pipe *p = &pipes->pipes[fd - 3];
	if (p->inputPid == -1 && p->outputPid == -1) {
		return -1;
	}

	p->inputPid = -1;
	p->outputPid = -1;
	p->fd = -1;
	p->readIndex = 0;
	p->writeIndex = 0;
	p->size = 0;

	pipes->cantPipes--;
	return 0;
}

int64_t writePipe(int64_t fd, const char *buffer, int64_t size) {
	PipesADT pipes = getPipes();
	if (fd < 3 || fd >= MAX_PIPES + 3 || size < 0) {
		return -1;
	}

	pipe *p = &pipes->pipes[fd - 3];

	int i;
	for (i = 0; i < size; i++) {
		while (p->size == PIPE_SIZE) {
			p->writeBlocked = 1;
			blockProcess(p->outputPid);
		}

		p->buffer[p->writeIndex] = buffer[i];
		p->writeIndex = (p->writeIndex + 1) % PIPE_SIZE;
		p->size++;

		if (p->readBlocked) {
			p->readBlocked = 0;
			readyProcess(p->inputPid);
		}
	}

	return i;
}

int64_t readPipe(int64_t fd, char *buffer, int64_t size) {
	PipesADT pipes = getPipes();
	if (fd < 3 || fd >= MAX_PIPES + 3 || size < 0) {
		return -1;
	}

	pipe *p = &pipes->pipes[fd - 3];

	int i;
	for (i = 0; i < size; i++) {
		while (p->size == 0) {
			p->readBlocked = 1;
			blockProcess(p->inputPid);
		}

		buffer[i] = p->buffer[p->readIndex];
		p->readIndex = (p->readIndex + 1) % PIPE_SIZE;
		p->size--;

		if (p->writeBlocked) {
			p->writeBlocked = 0;
			readyProcess(p->outputPid);
		}
	}

	return i;
}
