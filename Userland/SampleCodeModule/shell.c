// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "include/libc.h"
#include "include/userasm.h"
#include "include/syscall.h"
#include "include/eliminator.h"
#include "include/processes.h"
#include "include/builtins.h"
#include <stddef.h>

#define BUFFER_LENGTH 256
#define MAX_PARAMETERS 2 // todavia no sabemos cuantos parametros se van a enviar como maximo
#define PARAMETERS_LENGTH 256

#define READ 0
#define WRITE 1

typedef void (*functionPointer)(int argc, char *argv[]);

typedef struct command {
    char * name;
    functionPointer exec;
} command;

static void clear(int argc, char *argv[]) {
	if (argc != 0) {
		printf("Clear doesn't need parameters\n");
		return;
	}
	clearscreen();
}

static void help(int argc, char *argv[]) {
	if (argc != 0) {
		printf("Help doesn't need parameters\n");
		return;
	}

	const char *manual =
		"-------------COMMENTS-------------\n"
		"If you wish to send a process to background write BACK at the end of the command\n\n"
		"-------------BUILT-INS-------------\n"
		"HELP                       Display a menu with all the available commands in StarShell\n"
		"CLEAR                      Clears the screen\n"
		"MEM                        Prints the memory state\n"
		"KILL                       Command to kill a process\n"
		"NICE                       Command to change the priority of a process\n"
		"BLOCK                      Command to block a process\n"
		"PS                         Command to list all the processes\n	"
		"-------------PROCESSES-------------\n"
		"TESTMM                     Test the memory manager\n"
		"TESTPRIO                   Test priorities in processes\n"
		"TESTPROC                   Test processes - Use: TESTPROC <amount of processes>\n"
		"TESTSYNC                   Test synchronization - Use: TESTSYNCRO <iterations> <use_sem>\n"
		"LOOP                       Command to print its ID with a greeting every specified number of seconds\n"
		"CAT                        Command to print stdin\n"
		"WC                         Prints the amount of lines on input\n"
		"FILTER                     Prints only the vowels from the input\n"
		;
	printf("%s",manual);
}

static const command builtInCommands[] = {
	{"BLOCK", (functionPointer)blockProcess}, 
	{"CLEAR", (functionPointer)clear},
	{"HELP", (functionPointer)help},
	{"KILL", (functionPointer)killProcess},
	{"MEM", (functionPointer)memState},
	{"NICE", (functionPointer)changePriority},
	{"PS", (functionPointer)listProcesses},
};

static const command processCommands[] = {
	{"CAT", (functionPointer)cat},
	{"FILTER", (functionPointer)filter},
	{"LOOP", (functionPointer)loop},
	{"TESTMM", (functionPointer)testMemory},
	{"TESTPRIO", (functionPointer)testPrio},
	{"TESTPROC", (functionPointer)testProcesses},
	{"TESTSYNC", (functionPointer)testSynchronization},
	{"WC", (functionPointer)wc}
};

static int processDim = sizeof(processCommands) / sizeof(processCommands[0]);
static int builtinsDim = sizeof(builtInCommands) / sizeof(builtInCommands[0]);

int scanCommand(char *command, char *parameters[PARAMETERS_LENGTH], char *buffer) {
    // buffer = "command arg1 arg2 ..."
    int i, j, k;

    for (i = 0, j = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++, j++) {
        command[j] = buffer[i];
    }
    command[j] = '\0'; 

    if (buffer[i] == '\0') {
        return 0;
    }

    while (buffer[i] == ' ') {
        i++;
    }

    int toReturn = 0;

    for (j = 0; j < PARAMETERS_LENGTH; j++) {
        parameters[j] = sysmalloc(BUFFER_LENGTH); 
        if (parameters[j] == NULL) {
            return -1;
        }
    }

    for (k = 0; buffer[i] != '\0';) {
        if (buffer[i] != ' ') {
            parameters[toReturn][k++] = buffer[i++]; 
        } else {
            parameters[toReturn][k] = '\0'; 
            k = 0;
            toReturn++;
            while (buffer[i] == ' ') {
                i++;
            }
            if (toReturn >= PARAMETERS_LENGTH) {
                break;
            }
        }
    }

    if (k > 0) {
        parameters[toReturn][k] = '\0'; 
        toReturn++;
    }

    return toReturn;
}


int commandId(char *command) {
	char *aux = command;
	for (int i = 0; i < builtinsDim; i++) {
        if (strcmp(aux, builtInCommands[i].name) == 0) {
            return i;
        }
    }
    for (int i = 0; i < processDim; i++) {
        if (strcmp(aux, processCommands[i].name) == 0) {
            return builtinsDim + i; 
        }
    }
    return -1;
}

void executePipedCommands(char *leftCommand, char *leftParams[], int leftCantParams, int leftId, int isBackground1, char *rightCommand, char *rightParams[], int rightCantParams, int rightId, int isBackground2) {
	int readFd, writeFd;
    if ((readFd = sysopenPipe(sysgetpid(), READ)) == -1) {
        printf("Error creating pipe read\n");
        return;
    }
    int16_t pid1 = syscreateProcess((uint64_t)processCommands[leftId].exec, (char **)leftParams, leftCantParams, 1, (int16_t[]){readFd, STDOUT, STDERR}, isBackground1);
    if (pid1 == -1) {
        printf("Error creating process for command: %s\n", leftCommand);
        return;
    }

	if ((writeFd = sysopenPipe(sysgetpid(), WRITE)) == -1) {
        printf("Error creating pipe write\n");
        return;
    }
    int16_t pid2 = syscreateProcess((uint64_t)processCommands[rightId].exec, (char **)rightParams, rightCantParams, 1, (int16_t[]){STDIN, writeFd, STDERR}, isBackground2);
    if (pid2 == -1) {
        printf("Error creating process for command: %s\n", rightCommand);
		syskillProcess(pid1);
        return;
    }

    if (sysclosePipe(readFd) == -1) {
        printf("Error closing pipe read\n");
        syskillProcess(pid1);
        syskillProcess(pid2);
        return;
    }
    if (sysclosePipe(writeFd) == -1) {
        printf("Error closing pipe write\n");
        syskillProcess(pid1);
        syskillProcess(pid2);
        return;
    }

    if (!isBackground1) {
        syswaitProcess(pid1);
    }
    if (!isBackground2) {
        syswaitProcess(pid2);
    }
}

int main() {
	printf("Welcome to our shell! Write which module you want to use. To see our modules write help\n");
	printf("~$");

	char buffer[BUFFER_LENGTH] = {0};
	while (1) {
		int rta = scanf(buffer);
		if (rta == 1) {
			char leftCommand[BUFFER_LENGTH] = {0};
			char **leftParams = sysmalloc(PARAMETERS_LENGTH * sizeof(char *)); 
			char **rightParams = sysmalloc(PARAMETERS_LENGTH * sizeof(char *)); 
			int leftCantParams;
			char *pipe;
			
			if((pipe = strchr(buffer, '|')) != NULL){
				char rightCommand[BUFFER_LENGTH] = {0};
				*pipe = '\0';
    			leftCantParams = scanCommand(leftCommand, leftParams, buffer);
    			int rightCantParams = scanCommand(rightCommand, rightParams, pipe + 2);
				int leftId = commandId(leftCommand) - builtinsDim;
				int rightId = commandId(rightCommand) - builtinsDim;
				if(strcmp(leftCommand, processCommands[leftId].name) == 0 && strcmp(rightCommand, processCommands[rightId].name) == 0){
					char *newParams1[MAX_PARAMETERS + 1] = {0};
					newParams1[0] = processCommands[leftId].name;
            		for (int i = 0; i < leftCantParams; i++) {
               			if(strcmp(leftParams[i] , "BACK") != 0){
							newParams1[i + 1] = leftParams[i];
						}
            		}
					leftCantParams++;
					int isBackground1 = strcmp(leftParams[leftCantParams-1], "BACK") == 0;

					char *newParams2[MAX_PARAMETERS + 1] = {0};
					newParams2[0] = processCommands[rightId].name;
            		for (int i = 0; i < rightCantParams; i++) {
               			if(strcmp(rightParams[i] , "BACK") != 0){
							newParams2[i + 1] = rightParams[i];
						}
            		}
					rightCantParams++;
					int isBackground2 = strcmp(rightParams[rightCantParams-1], "BACK") == 0;

					executePipedCommands(leftCommand, newParams1, leftCantParams,leftId, isBackground1, rightCommand, newParams2, rightCantParams,rightId, isBackground2);
				} else {
					printf("Pipes only support processes\n");
				}

			} else {
				leftCantParams = scanCommand(leftCommand, leftParams, buffer);
				int id = commandId(leftCommand);

				if (id >= 0 && id < builtinsDim) {
					builtInCommands[id].exec(leftCantParams, leftParams);
				} else if (id >= builtinsDim && id < builtinsDim + processDim) {
					int processIndex = id - builtinsDim;	
					uint64_t rip = (uint64_t)processCommands[processIndex].exec;

					char *newParams[MAX_PARAMETERS + 1] = {0};
					newParams[0] = processCommands[processIndex].name;
            		for (int i = 0; i < leftCantParams; i++) {
						if(strcmp(leftParams[i] , "BACK") != 0){
							newParams[i + 1] = leftParams[i];
						}
            		}
					int16_t fileDescriptors[] = {0, 1, 2};	
					int isBackground = strcmp(leftParams[leftCantParams-1], "BACK") == 0;
					int16_t pid = syscreateProcess(rip, newParams, leftCantParams + 1, 1, fileDescriptors, isBackground);
					if(pid == -1){
						printf("Error creating process\n");
					}
					if (!isBackground) {
						syswaitProcess(pid);
					}
				} else {
					printf("%s", leftCommand);
					printf(": command not found\n");
				}
			}
			
			for (int i = 0; buffer[i] != 0; i++) { // vaciamos el buffer
				buffer[i] = '\0';
				// También vaciamos los parámetros para evitar residuos
				for (int j = 0; j < PARAMETERS_LENGTH; j++) {
					leftParams[j] = NULL;
					rightParams[j] = NULL;
				}
			}
		}
		printf("~$");
	}
	sysexit();
}