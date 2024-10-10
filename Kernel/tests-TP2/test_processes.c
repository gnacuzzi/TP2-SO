// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include "test_util.h"
#include <scheduler.h>

extern void syscallExit();

enum State { RUN, BLOCK, KILL };

typedef struct P_rq {
	int32_t pid;
	enum State state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
	uint8_t rq;
	uint8_t alive = 0;
	uint8_t action;
	uint64_t max_processes;
	char *argvAux[] = {"endless_loop", NULL};

	if (argc != 2)
		syscallExit();

	if ((max_processes = satoi(argv[1])) <= 0)
		syscallExit();

	p_rq p_rqs[max_processes];

	while (1) {
		// Create max_processes processes
		for (rq = 0; rq < max_processes; rq++) {
			p_rqs[rq].pid = createProcess((uint64_t)endless_loop, argvAux, 1,"endless_loop", 0, 0);

			if (p_rqs[rq].pid == -1) {
				printf("test_processes: ERROR creating process\n");
				syscallExit();
			}
			else {
				p_rqs[rq].state = RUN;
				alive++;
			}
		}

		// Randomly kills, blocks or unblocks processes until every one has been killed
		while (alive > 0) {
			for (rq = 0; rq < max_processes; rq++) {
				action = GetUniform(100) % 2;

				switch (action) {
					case 0:
						if (p_rqs[rq].state == RUN || p_rqs[rq].state == BLOCK) {
							if (killProcess(p_rqs[rq].pid) == -1) {
								printf("test_processes: ERROR killing process\n");
								syscallExit();
							}
							p_rqs[rq].state = KILL;
							alive--;
						}
						break;

					case 1:
						if (p_rqs[rq].state == RUN) {
							if (blockProcess(p_rqs[rq].pid) == -1) {
								printf("test_processes: ERROR blocking process\n");
								syscallExit();
							}
							p_rqs[rq].state = BLOCK;
						}
						break;
				}
			}

			// Randomly unblocks processes
			for (rq = 0; rq < max_processes; rq++)
				if (p_rqs[rq].state == BLOCK && GetUniform(100) % 2) {
					if (readyProcess(p_rqs[rq].pid) == -1) {
						printf("test_processes: ERROR unblocking process\n");
						syscallExit();
					}
					p_rqs[rq].state = RUN;
				}
		}
	}
	//return 0;
	syscallExit(); //no se bien si esto es correcto, creo que cuando lo agreguemos en la shell es mejor poner un wrapper
}
