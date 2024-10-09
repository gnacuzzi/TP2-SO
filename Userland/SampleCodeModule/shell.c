// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// #include <userasm.h>

#include "include/libc.h"
#include "include/userasm.h"
#include "include/syscall.h"
#include "include/eliminator.h"
#include "include/processes.h"

#define BUFFER_LENGTH 256
#define MAX_PARAMETERS 2 // todavia no sabemos cuantos parametros se van a enviar como maximo
#define PARAMETERS_LENGTH 256
#define BUILTIN_COMMANDS_COUNT (sizeof(builtInCommands) / sizeof(command))
#define PROCESS_COMMANDS_COUNT (sizeof(processCommands) / sizeof(command))

typedef void (*functionPointer)(int argc, char *argv[]);

typedef struct command {
    char * name;
    functionPointer exec;
} command;

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
		"-------------BUILT-INS-------------\n"
		"DIVIDEBYZERO               Command to verify the exception routine \"Divide by zero\"\n"
		"ELIMINATOR                 Challenge yourself or you and a friend to an elimination game\n"
		"HELP                       Display a menu with all the available commands in StarShell\n"
		"INVALIDOPERATION           Command to verify the exception routine \"Invalid Opcode\"\n"
		"LETTERSIZE                 Change the letter size to your preferences\n"
		"TIME                       Show current time\n"
		"CLEAR                      Clears the screen\n"
		"REGISTERS                  Prints each register with it's values at the moment of the snapshot\n"
		"-------------PROCESSES-------------\n"
		"TESTMM						Test the memory manager\n"
		"TESTPRIO					\n"
		"TESTPROCESSES				Test the processes\n"
		;
	printf(manual);
}

static void testMM(char parameters[MAX_PARAMETERS][PARAMETERS_LENGTH], int cantParams) {
	if (cantParams != 0) {
		printf("Memory doesn't need parameters\n");
		return;
	}
	uint64_t rip = (uint64_t)test_mm;
	int fileDescriptors[] = {0, 1, 2};
	int pid = syscreateProcess(rip, parameters, "TestMM", 0, fileDescriptors);
	sysunblockProcess(pid);
	syswaitProcess(pid);
	if(pid < 0){
		printf("Error creating process\n");
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

static const command builtInCommands[] = {
	{"CLEAR", (functionPointer)clear},
    {"DIVIDEBYZERO", (functionPointer)dividebyzero},
    {"ELIMINATOR", (functionPointer)eliminator},
    {"HELP", (functionPointer)help},
    {"INVALIDOPERATION", (functionPointer)invalidoperation},
    {"LETTERSIZE", (functionPointer)lettersize},
    {"REGISTERS", (functionPointer)registers},
    {"TIME", (functionPointer)time}

};

static const command processCommands[] = {
	{"TESTMM", (functionPointer)testMM},
	{"TESTPRIO", (functionPointer)test_prio},
	{"TESTPROCESSES", (functionPointer)test_processes}
};

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
	for (int i = 0; i < BUILTIN_COMMANDS_COUNT; i++) {
        if (strcmp(aux, builtInCommands[i].name) == 0) {
            return i;
        }
    }
	for (int i = 0; i < PROCESS_COMMANDS_COUNT; i++) {
        if (strcmp(aux, processCommands[i].name) == 0) {
            return -(i + 1); // Return negative index for process commands
        }
    }
	return -BUILTIN_COMMANDS_COUNT;
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
				builtInCommands[id].exec(params, cantParams);
			}else if(id > -BUILTIN_COMMANDS_COUNT){
				int fileDescriptors[] = {0, 1, 2};
				uint64_t rip = (uint64_t)processCommands[-id-1].exec;
				int pid = syscreateProcess(rip, params, processCommands[-id-1].name , 1, fileDescriptors);
            	sysunblock(pid);
                syswaitProcess(pid);
			}else {
				printf(command);
				printf(": command not found\n");
			}
			for (int i = 0; buffer[i] != 0; i++) { // vaciamos el buffer
				buffer[i] = 0;
			}
		}
		printf("~$");
	}
	sysexit();
}