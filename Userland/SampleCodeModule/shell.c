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

static void dividebyzero(int argc, char *argv[]) {
	if (argc != 0) {
		printf("DivideByZero doesn't need parameters\n");
		return;
	}
	dividebyzeroexception();
}

static void eliminator(int argc, char *argv[]) {
	if (argc != 0) {
		printf("Eliminator doesn't need parameters\n");
		return;
	}
	start_eliminator();
	clearscreen();
	setlettersize(1);
}

static void invalidoperation(int argc, char *argv[]) {
	if (argc != 0) {
		printf("InvalidOperation doesn't need parameters\n");
		return;
	}

	invalidoperationexception();
}

static void lettersize(int argc, char *argv[]) {
	if (argc != 1) {
		printf("You must insert ONE parameter indicating the letter size you desire\n");
	}
	int num = atoi(argv[0]);
	if (num > 3 || num < 1) {
		printf("The letter size must be a number between 1 and 3\n");
	}
	else {
		setlettersize(num);
	}
	return;
}

static void time(int argc, char *argv[]) {
	if (argc != 0) {
		printf("Time doesn't need parameters\n");
		return;
	}
	printf("CURRENT TIME: \n");
	int seconds, minutes, hours;
	get_seconds(&seconds);
	get_minutes(&minutes);
	get_hours(&hours);
	printf("%1x:%1x:%1x", hours, minutes, seconds); // el 1 para que se agregue un 0 adelante si es menor a 10
	printf("\n");
}

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
		"DIVIDEBYZERO               Command to verify the exception routine \"Divide by zero\"\n"
		"ELIMINATOR                 Challenge yourself or you and a friend to an elimination game\n"
		"HELP                       Display a menu with all the available commands in StarShell\n"
		"INVALIDOPERATION           Command to verify the exception routine \"Invalid Opcode\"\n"
		"LETTERSIZE                 Change the letter size to your preferences\n"
		"TIME                       Show current time\n"
		"CLEAR                      Clears the screen\n"
		"REGISTERS                  Prints each register with it's values at the moment of the snapshot\n"
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


static char *regs[] = {"RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8",  "R9",
					   "R10", "R11", "R12", "R13", "R14", "R15", "RSP", "RIP", "RFLAGS"};

static void registers(int argc, char *argv[]) {
	if (argc != 0) {
		printf("Registers doesn't need parameters\n");
		return;
	}
	int len = sizeof(regs) / sizeof(char *);
	uint64_t snapShot[len];
	uint64_t flag = 0;
	getRegs(snapShot, &flag);
	if (flag == 1) {
		for (int i = 0; i < len; i++) {
			printf("%s: 0x%x\n", regs[i], (uint32_t)snapShot[i]);
		}
	}
	else {
		printf("Registers are not available, you need to press control.\n");
	}
}

static const command builtInCommands[] = {
	{"BLOCK", (functionPointer)blockProcess}, 
	{"CLEAR", (functionPointer)clear},
	{"DIVIDEBYZERO", (functionPointer)dividebyzero},
	{"ELIMINATOR", (functionPointer)eliminator},
	{"HELP", (functionPointer)help},
	{"KILL", (functionPointer)killProcess},
	{"LETTERSIZE", (functionPointer)lettersize},
	{"MEM", (functionPointer)memState},
	{"NICE", (functionPointer)changePriority},
	{"PS", (functionPointer)listProcesses},
	{"REGISTERS", (functionPointer)registers},
	{"TIME", (functionPointer)time},
	{"INVALIDOPERATION", (functionPointer)invalidoperation}
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

    // Inicializa el comando
    for (i = 0, j = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++, j++) {
        command[j] = buffer[i];
    }
    command[j] = '\0'; // Terminar el comando

    // Si no hay más argumentos, retornar
    if (buffer[i] == '\0') {
        return 0;
    }

    // Saltar espacios en blanco
    while (buffer[i] == ' ') {
        i++;
    }

    int toReturn = 0;

    // Inicializar cada parámetro (asumiendo que cada parámetro tiene un tamaño definido por BUFFER_LENGTH)
    for (j = 0; j < PARAMETERS_LENGTH; j++) {
        parameters[j] = sysmalloc(BUFFER_LENGTH); // Asignar memoria para cada parámetro
        if (parameters[j] == NULL) {
            // Manejo de error: no se pudo asignar memoria
            return -1;
        }
    }

    // Leer parámetros
    for (k = 0; buffer[i] != '\0';) {
        if (buffer[i] != ' ') {
            parameters[toReturn][k++] = buffer[i++]; // Copiar el carácter al parámetro
        } else {
            parameters[toReturn][k] = '\0'; // Terminar el parámetro
            k = 0;
            toReturn++;
            while (buffer[i] == ' ') {
                i++;
            }
            // Comprobar si se supera el límite de parámetros
            if (toReturn >= PARAMETERS_LENGTH) {
                break;
            }
        }
    }

    // Agregar el último parámetro si hay alguno
    if (k > 0) {
        parameters[toReturn][k] = '\0'; // Terminar el último parámetro
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

static int parseCommands(char *buffer, char *leftCommand, char *leftParams[], int *leftCantParams, char *rightCommand, char *rightParams[], int *rightCantParams) {
    char *pipe = strchr(buffer, '/');
    if (pipe == NULL) {
        return 0; 
    }
    *pipe = '\0';
    *leftCantParams = scanCommand(leftCommand, leftParams, buffer);
    *rightCantParams = scanCommand(rightCommand, rightParams, pipe + 2);

    return 1; 
}

void executePipedCommands(char *leftCommand, char *leftParams[], int leftCantParams, int leftId, int isBackground1, char *rightCommand, char *rightParams[], int rightCantParams, int rightId, int isBackground2) {
    int readPipe = sysgetpid();
    if (sysopenPipe(readPipe, READ) == -1) {
        printf("Error creating pipe read\n");
        return;
    }
    int16_t pid1 = syscreateProcess((uint64_t)processCommands[leftId].exec, (char **)leftParams, leftCantParams, 1, (int16_t[]){readPipe, STDOUT, STDERR}, isBackground1);
    if (pid1 == -1) {
        printf("Error creating process for command: %s\n", leftCommand);
        return;
    }

	int writePipe = sysgetpid();
	if (sysopenPipe(writePipe, WRITE) == -1) {
        printf("Error creating pipe write\n");
        return;
    }
    int16_t pid2 = syscreateProcess((uint64_t)processCommands[rightId].exec, (char **)rightParams, rightCantParams, 1, (int16_t[]){STDIN, writePipe, STDERR}, isBackground2);
    if (pid2 == -1) {
        printf("Error creating process for command: %s\n", rightCommand);
		syskillProcess(pid1);
        return;
    }

    if (sysclosePipe(readPipe) == -1) {
        printf("Error closing pipe read\n");
        syskillProcess(pid1);
        syskillProcess(pid2);
        return;
    }
    if (sysclosePipe(writePipe) == -1) {
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
			char *leftParams[PARAMETERS_LENGTH] = {0};
			char rightCommand[BUFFER_LENGTH] = {0};
			char *rightParams[PARAMETERS_LENGTH] = {0};
			int leftCantParams;
			int rightCantParams;
			
			if(parseCommands(buffer, leftCommand, leftParams, &leftCantParams, rightCommand, rightParams, &rightCantParams)){
				int leftId = commandId(leftCommand) - builtinsDim;
				int rightId = commandId(rightCommand) - builtinsDim;
				if(strcmp(leftCommand, processCommands[leftId].name) == 0 && strcmp(rightCommand, processCommands[rightId].name) == 0){
					char *newParams1[MAX_PARAMETERS + 1] = {0};
					newParams1[0] = processCommands[leftId].name;
            		for (int i = 0; i < leftCantParams; i++) {
               			newParams1[i + 1] = leftParams[i];
            		}
					leftCantParams++;
					int isBackground1 = strcmp(leftParams[leftCantParams-1], "BACK") == 0;

					char *newParams2[MAX_PARAMETERS + 1] = {0};
					newParams2[0] = processCommands[rightId].name;
            		for (int i = 0; i < rightCantParams; i++) {
               			newParams2[i + 1] = rightParams[i];
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
               			newParams[i + 1] = leftParams[i];
            		}
					int16_t fileDescriptors[] = {0, 1, 2};	
					int isBackground = strcmp(leftParams[leftCantParams-1], "BACK") == 0;
					int16_t pid = syscreateProcess(rip, (char **)newParams, leftCantParams + 1, 1, fileDescriptors, isBackground);
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