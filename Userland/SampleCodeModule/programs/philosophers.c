// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <syscall.h>
#include <libc.h>
#include <processes.h>

#define MAX_PHYLOS 10
#define MIN_PHYLOS 5
#define PHYLO_BUFFER_SIZE 3

#define MUTEX_ID 16
#define PRINT_ID 17

#define LEFT(phyloId) (((phyloId) + phylosCount - 1) % phylosCount)
#define RIGHT(phyloId) (((phyloId) + 1) % phylosCount)

typedef enum { THINKING, HUNGRY, EATING } state_t;

int phylosCount = 0;

state_t state[MAX_PHYLOS] = {0};
int philosopherPids[MAX_PHYLOS] = {0};

void takeForks(int phyloId);
void putForks(int phyloId);
void test(int phyloId);
void wait();
void render();

int philosopher(int argc, char *argv[]);
void addPhilosopher();
void removePhilosopher();

void startDining() {
	char c;
	while ((c = readchar()) != 'Q') {
		if (c == 'A') {
			if (phylosCount < MAX_PHYLOS) {
				printf("Adding philosopher\n");
				addPhilosopher();
			}
			else {
				printf("Maximum philosophers reached\n");
			}
		}
		if (c == 'R') {
			removePhilosopher();
		}
	}

	for (int i = 0; i < phylosCount; i++) {
		if (syskillProcess(philosopherPids[i]) == -1) {
			printf("Error killing philosopher %d\n", i);
			sysexit();
			return;
		}
		if (syssemClose(i) == -1) {
			printf("Error closing semaphore %d\n", i);
			sysexit();
			return;
		}
	}
	if (syssemClose(MUTEX_ID) == -1) {
		printf("Error closing semaphpore mutex\n");
		sysexit();
		return;
	}
	if (syssemClose(PRINT_ID) == -1) {
		printf("Error closing semaphpore print\n");
		sysexit();
		return;
	}
	return;
}

void addPhilosopher() {
	syswait(MUTEX_ID);
	state[phylosCount] = THINKING;
	char phyloBuff[PHYLO_BUFFER_SIZE] = {0};

	if (syssemInit(phylosCount, 0) == -1) {
		printf("Error creating semaphore %d\n", phylosCount);
		sysexit();
		return;
	}

	itoa(phylosCount, phyloBuff, 10);
	char *params[] = {"philosopher", phyloBuff};
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};

	philosopherPids[phylosCount] = syscreateProcess((uint64_t) philosopher, params, 2, 1, fileDescriptors, 1);
	if (philosopherPids[phylosCount] < 0) {
		printf("Error creating philosopher %d\n", phylosCount);
		sysexit();
		return;
	}
	if (sysunblockProcess(philosopherPids[phylosCount]) == -1) {
		sysexit();
		return;
	}

	phylosCount++;
	syspost(MUTEX_ID);
}

void removePhilosopher() {
	if (phylosCount <= MIN_PHYLOS) {
		printf("Minimum philosophers reached\n");
		return;
	}
	printf("Removing philosopher\n");
	phylosCount--;

	syswait(MUTEX_ID);
	while (state[LEFT(phylosCount)] == EATING && state[RIGHT(phylosCount)] == EATING) {
		syspost(MUTEX_ID);
		syswait(phylosCount);
		syswait(MUTEX_ID);
	}
	if (syskillProcess(philosopherPids[phylosCount]) == -1) {
		printf("Error killing philosopher %d\n", phylosCount);
		sysexit();
		return;
	}
	if (syssemClose(phylosCount) == -1) {
		printf("Error closing semaphore %d\n", phylosCount);
		sysexit();
		return;
	}

	syspost(MUTEX_ID);
}

int philosopher(int argc, char *argv[]) {
	int i = atoi(argv[1]);
	while (1) {
		wait();
		takeForks(i);
		wait();
		putForks(i);
	}
	return 0;
}

void takeForks(int phyloId) {
	syswait(MUTEX_ID);
	state[phyloId] = HUNGRY;
	test(phyloId);
	syspost(MUTEX_ID);
	syswait(phyloId);
}

void putForks(int phyloId) {
	syswait(MUTEX_ID);
	state[phyloId] = THINKING;
	test(LEFT(phyloId));
	test(RIGHT(phyloId));
	syspost(MUTEX_ID);
}

void test(int phyloId) {
	if (state[phyloId] == HUNGRY && state[LEFT(phyloId)] != EATING && state[RIGHT(phyloId)] != EATING) {
		state[phyloId] = EATING;
		syspost(phyloId);
	}
	render();
}

void render() {
	syswait(PRINT_ID);
	for (int i = 0; i < phylosCount; i++) {
		printf(state[i] == EATING ? "E " : ". ");
	}
	putchar('\n');
	syspost(PRINT_ID);
}

void wait() {
	for (int i = 0; i < 500000; i++)
		;
}

void phylo(int argc, char *argv[]) {
	if (argc != 2) {
		printf("You must insert ONE parameter indicating the amount of philosophers you desire to start with\n");
		sysexit();
		return;
	}
	int aux = atoi(argv[1]);
	if (aux < MIN_PHYLOS || aux > MAX_PHYLOS) {
		printf("The amount of philosophers must be a number between %d and %d\n", MIN_PHYLOS, MAX_PHYLOS);
		sysexit();
		return;
	}
	if (syssemInit(MUTEX_ID, 1) == -1) {
		printf("Error creating semaphore mutex\n");
		sysexit();
		return;
	}
	if (syssemInit(PRINT_ID, 1) == -1) {
		printf("Error creating semaphore print\n");
		sysexit();
		return;
	}

	printf("Welcome to the Dining Philosophers\n");
	printf("Commands: (A)dd, (R)emove, (Q)uit\n");
	printf("Starting...\n");
	for (int i = 0; i < aux; i++) {
		addPhilosopher();
	}

	startDining();
	phylosCount = 0;
	sysexit();
	return;
}
