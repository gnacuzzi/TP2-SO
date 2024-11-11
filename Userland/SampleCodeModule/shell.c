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
#define MAX_PARAMETERS 3 // todavia no sabemos cuantos parametros se van a enviar como maximo
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
		"UNBLOCK                    Command to unblock a process\n"
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
		"PHYLO                      Solves the problem of the dining philosophers\n"
		;
	printf("%s",manual);
}

static const command builtInCommands[] = {
	{"BLOCK", (functionPointer)blockProcess}, 
	{"UNBLOCK", (functionPointer)unblockProcess},
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
	{"PHYLO", (functionPointer)phylo},
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
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR}; 
    int16_t leftPid = syscreateProcess((uint64_t)processCommands[leftId].exec, (char **)leftParams, leftCantParams, 1, fileDescriptors, isBackground1);
    if (leftPid == -1) {
        printf("Error creating process for command: %s\n", leftCommand);
        return;
    }
	int readFd, writeFd;
	if ((writeFd = sysopenPipe(leftPid, WRITE)) == -1) {
        printf("Error creating pipe write\n");
		syskillProcess(leftPid);
        return;
    }
	if(syschangeFds(leftPid, (int16_t[]){STDIN, writeFd, STDERR}) == -1){
		printf("Couldn't change file descriptors for pipe write\n");
		sysclosePipe(writeFd);
		syskillProcess(leftPid);
		return;
	}
	
    int16_t rightPid = syscreateProcess((uint64_t)processCommands[rightId].exec, (char **)rightParams, rightCantParams, 1, fileDescriptors, isBackground2);
    if (rightPid == -1) {
        printf("Error creating process for command: %s\n", rightCommand);
		syskillProcess(leftPid);
        return;
    }
	if ((readFd = sysopenPipe(rightPid, READ)) == -1) {
        printf("Error creating pipe write\n");
		syskillProcess(rightPid);
		sysclosePipe(writeFd);
		syskillProcess(leftPid);
        return;
    }
	if(syschangeFds(rightPid, (int16_t[]){readFd, STDOUT, STDERR}) == -1){
		printf("Couldn't change file descriptors for pipe read\n");
		sysclosePipe(writeFd);
		syskillProcess(leftPid);
		syskillProcess(rightPid);
		return;
	}

	if(sysunblockProcess(rightPid) == -1){
		printf("Couldn't unblock left process\n");
		sysclosePipe(writeFd);
		syskillProcess(leftPid);
		syskillProcess(rightPid);
		return;
	}
	if(sysunblockProcess(leftPid) == -1){
		printf("Couldn't unblock left process\n");
		sysclosePipe(writeFd);
		syskillProcess(leftPid);
		syskillProcess(rightPid);
		return;
	}

	if (!isBackground1) {
        syswaitProcess(leftPid);
		syswaitProcess(rightPid);
    }
	
    if (sysclosePipe(readFd) == -1) {
        printf("Error closing pipe read\n");
        syskillProcess(leftPid);
        syskillProcess(rightPid);
        return;
    }
}

int main() {
	printf("Welcome to our shell! Write which module you want to use. To see our modules write help\n");
	printf("~$");

	char *buffer = sysmalloc(BUFFER_LENGTH * sizeof(char *));
	if(buffer == NULL){
		printf("Error allocating memory for buffer\n");
		sysexit();
		return 0;
	}
	while (1) {
		int rta = scanf(buffer);
		if (rta == 1) {
			char *leftCommand= sysmalloc(BUFFER_LENGTH * sizeof(char *));
			if(leftCommand == NULL){
				printf("Error allocating memory for left command\n");
				continue;
			}
			char **leftParams = sysmalloc(PARAMETERS_LENGTH * sizeof(char *)); 
			if(leftParams == NULL){
				printf("Error allocating memory for left argv[]\n");
				sysfree(leftCommand);
				continue;
			}
			int leftCantParams;
			char *pipe;
			
			if((pipe = strchr(buffer, '|')) != NULL){
				char **rightParams = sysmalloc(PARAMETERS_LENGTH * sizeof(char *)); 
				if(rightParams == NULL){
					printf("Error allocating memory for right argv[]\n");
					sysfree(leftParams);
					sysfree(leftCommand);
					continue;
				}
				char *rightCommand = sysmalloc(BUFFER_LENGTH * sizeof(char *));
				if(rightCommand == NULL){
					printf("Error allocating memory for right command\n");
					sysfree(leftParams);
					sysfree(leftCommand);
					sysfree(rightParams);
					continue;
				}
				*pipe = '\0';
    			leftCantParams = scanCommand(leftCommand, leftParams, buffer);
    			int rightCantParams = scanCommand(rightCommand, rightParams, pipe + 2);
				int leftId = commandId(leftCommand) - builtinsDim;
				int rightId = commandId(rightCommand) - builtinsDim;
				if(strcmp(leftCommand, processCommands[leftId].name) == 0 && strcmp(rightCommand, processCommands[rightId].name) == 0){
					char *newParams1[MAX_PARAMETERS + 1] = {0};
					newParams1[0] = processCommands[leftId].name;
            		for (int i = 0; i < leftCantParams; i++) {
						newParams1[i + 1] = leftParams[i];
            		}
					int isBackground1 = 0;
					if(leftCantParams != 0){
						isBackground1 = strcmp(leftParams[leftCantParams-1], "BACK") == 0;
					}

					char *newParams2[MAX_PARAMETERS + 1] = {0};
					newParams2[0] = processCommands[rightId].name;
            		for (int i = 0; i < rightCantParams; i++) {
						newParams2[i + 1] = rightParams[i];
            		}
					int isBackground2 = 0;
					if(rightCantParams != 0){
						isBackground2 = strcmp(rightParams[rightCantParams-1], "BACK") == 0;
					}
					if(isBackground1 || isBackground2){
						printf("Can't have background processes with pipes\n");
					}else{
						executePipedCommands(leftCommand, newParams1, leftCantParams + 1,leftId, isBackground1, rightCommand, newParams2, rightCantParams + 1,rightId, isBackground2);

					}
		
					for (int i = 0; i < leftCantParams; i++) {
						sysfree(leftParams[i]);
					}
					for (int i = 0; i < rightCantParams; i++) {
						sysfree(rightParams[i]);
					}
					sysfree(rightParams);
					sysfree(rightCommand);
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
					int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
					int isBackground = 0;
					if(leftCantParams != 0){
						isBackground = strcmp(leftParams[leftCantParams-1], "BACK") == 0;
					}
					leftCantParams += isBackground ? -1 : 0;
					int16_t pid = syscreateProcess(rip, newParams, leftCantParams + 1, 1, fileDescriptors, isBackground);
					if(pid == -1){
						printf("Error creating process\n");
						sysfree(leftCommand);
						for (int i = 0; i < leftCantParams; i++) {
							sysfree(leftParams[i]);
						}
						continue;
					}
					if(sysunblockProcess(pid) == -1){
						printf("Couldn't unblock process\n");
						syskillProcess(pid);
						sysfree(leftCommand);
						for (int i = 0; i < leftCantParams; i++) {
							sysfree(leftParams[i]);
						}
						continue;
					}
					if (!isBackground) {
						syswaitProcess(pid);
					}
				} else {
					printf("%s", leftCommand);
					printf(": command not found\n");
				}
			}
			
			for (int i = 0; buffer[i] != 0; i++) { 
				buffer[i] = '\0';
			}
			for (int i = 0; i < leftCantParams; i++) {
					sysfree(leftParams[i]);
			}
			sysfree(leftParams);
			sysfree(leftCommand);
		}
		printf("~$");
	}
	sysfree(buffer);
	sysexit();
	return 0;
}