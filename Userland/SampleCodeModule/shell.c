// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// #include <userasm.h>

#include "include/libc.h"
#include "include/userasm.h"
#include "include/syscall.h"
#include "include/eliminator.h"
#include "../include/scheduler.h"

#define BUFFER_LENGTH 256
#define MAX_PARAMETERS 2 // todavia no sabemos cuantos parametros se van a enviar como maximo
#define PARAMETERS_LENGTH 256

static void dividebyzero(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("DivideByZero doesn't need parameters\n");
		return;
	}
	dividebyzeroexception();
}

static void eliminator(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("Eliminator doesn't need parameters\n");
		return;
	}
	start_eliminator();
	clearscreen();
	setlettersize(1);
}

static void invalidoperation(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("InvalidOperation doesn't need parameters\n");
		return;
	}

	invalidoperationexception();
}

static void lettersize(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 1) {
		printf("You must insert ONE parameter indicating the letter size you desire\n");
	}
	else if (parameters[0][0] > 3 || parameters[0][0] < 1) {
		printf("The letter size must be a number between 1 and 3\n");
	}
	else {
		setlettersize(parameters[0][0]);
	}
	return;
}

static void time(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("time doesn't need parameters\n");
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

static void clear(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("Clear doesn't need parameters\n");
		return;
	}
	clearscreen();
}

static void help(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("Help doesn't need parameters\n");
		return;
	}

	const char *manual =
		"DIVIDEBYZERO               Command to verify the exception routine \"Divide by zero\"\n"
		"ELIMINATOR                 Challenge yourself or you and a friend to an elimination game\n"
		"HELP                       Display a menu with all the available commands in StarShell\n"
		"INVALIDOPERATION           Command to verify the exception routine \"Invalid Opcode\"\n"
		"LETTERSIZE                 Change the letter size to your preferences\n"
		"TIME                       Show current time\n"
		"CLEAR                      Clears the screen\n"
		"REGISTERS                  Prints each register with it's values at the moment of the snapshot\n"
		"TESTMM						Test the memory manager\n";
	printf(manual);
}

static void testMM(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("Memory doesn't need parameters\n");
		return;
	}
	int aux;
	testmm(&aux);
	printf("Memory manager test result: %d\n", aux);
}

static void createProcessInShell(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 1) {
        printf("Usage: CREATE <process_name>\n");
        return;
    }
    
    char *process_name = parameters[0];
    int pid = createProcess(process_name);
    
    if (pid >= 0) {
        printf("Process '%s' created successfully. PID: %d\n", process_name, pid);
    } else {
        printf("Failed to create process '%s'. Error code: %d\n", process_name, pid);
    }	
}

static char *regs[] = {"RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8",  "R9",
					   "R10", "R11", "R12", "R13", "R14", "R15", "RSP", "RIP", "RFLAGS"};

static void registers(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("Registers doesn't need parameters\n");
		return;
	}
	int len = sizeof(regs) / sizeof(char *);
	uint64_t snapShot[len];
	uint64_t flag = 0;
	getRegs(snapShot, &flag);
	if (flag == 1) {
		for (int i = 0; i < len; i++) {
			printf("%s: 0x%x\n", regs[i], snapShot[i]);
		}
	}
	else {
		printf("Registers are not available, you need to press control.\n");
	}
}

static const char *allCommands[] = {"CLEAR", "CREATE"	,  "DIVIDEBYZERO", "ELIMINATOR", "HELP", "INVALIDOPERATION",
									"LETTERSIZE", "TESTMM",  "REGISTERS",	"TIME"};
static void (*commandsFunction[])(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) = {
	clear, createProcessInShell, dividebyzero, eliminator, help, invalidoperation, lettersize, testMM, registers, time}; // funciones a hacer

int scanCommand(char *command, char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], char *buffer) {
	// buffer = "command arg1 arg2"
	int i, j, k;

	for (i = 0, j = 0; buffer[i] != ' ' && buffer[i] != 0; i++, j++) {
		command[j] = buffer[i];
	}

	command[j] = 0;

	if (buffer[i] == 0) {
		return 0;
	}

	while (buffer[i] == ' ') {
		i++;
	}

	int toReturn = 1;

	for (j = 0, k = 0; buffer[i] != 0;) {
		if (buffer[i] != ' ') {
			if (!isNumber(buffer[i])) {
				parameters[j][k++] = buffer[i++];
			}
			else {
				parameters[j][k++] = (buffer[i++] - '0'); // lo paso de su ascii
			}
		}
		else {
			parameters[j][k] = 0;
			k = 0;
			j++;
			toReturn++;
			while (buffer[i] == ' ') {
				i++;
			}
		}
	}
	return toReturn;
}

int commandId(char *command) {
	char *aux = command;
	for (int i = 0; allCommands[i] != 0; i++) {
		int cmp = strcmp(aux, allCommands[i]);
		if (cmp < 0) { // ordenado alfabeticamente
			return -1;
		}
		else if (cmp == 0) {
			return i;
		}
	}
	return -1;
}

int main() {
	printf("Welcome to our shell! Write which module you want to use. To see our modules write help\n");
	printf("~$");

	char buffer[BUFFER_LENGTH] = {0};
	while (1) {
		int rta = scanf(buffer);
		if (rta == 1) {
			char command[BUFFER_LENGTH] = {0};
			char params[MAX_PARAMETERS][PARAMETERS_LENGTH] = {{0}};
			int cantParams = scanCommand(command, params, buffer);
			int id;
			if ((id = commandId(command)) >= 0) {
				commandsFunction[id](params, cantParams);
			}
			else {
				printf(command);
				printf(": command not found\n");
			}
			for (int i = 0; buffer[i] != 0; i++) { // vaciamos el buffer
				buffer[i] = 0;
			}
		}
		printf("~$");
	}
	return 0;
}