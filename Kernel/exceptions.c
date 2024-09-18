#include <videoDriver.h>
#include <lib.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OP_CODE_ID 6

void zero_division();
void invalid_op_code();


uint64_t reg_snap[REGS] = {0};      //valores de los registros
static char * regs[] = {"RAX","RBX","RCX","RDX","RSI","RDI","RBP","R8","R9","R10","R11","R12","R13","R14","R15", "RSP","RIP", "RFLAGS"};
extern const uint64_t exceptregs[18];

void print_regs() {
	for (int i = 0; i<REGS; i++){
		printf("%s: 0x%x\n", regs[i], exceptregs[i]);
    }
}

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID){
		zero_division();
	} else if (exception == INVALID_OP_CODE_ID){
		invalid_op_code();
	}
}

void zero_division() {
	printf("\nerror: divide by zero exception\n");
	print_regs();
	printf("\nshell is restarting\n");
	putchar('\n');
}

void invalid_op_code() {
	printf("\nerror: invalid op code exception\n");
	print_regs();
	printf("\nshell is restarting\n");
	putchar('\n');
}