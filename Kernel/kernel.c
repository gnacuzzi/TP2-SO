// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <interrupts.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const sampleDataModuleAddress = (void *) 0x500000;
static void *const heapAddress = (void *) 0x600000;

extern void test_mm(int argc, char *argv[]);
extern void test_processes(int argc, char *argv[]);
extern void test_prio(int argc, char *argv[]);

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
	return (void *) ((uint64_t) &endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					 - sizeof(uint64_t)						// Begin at the top of the stack
	);
}

void *initializeKernelBinary() {
	void *moduleAddresses[] = {sampleCodeModuleAddress, sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	mminit(heapAddress, HEAP_SIZE + STRUCT_SIZE);

	return getStackBase();
}

int main() {
	load_idt();
	
	_cli();
	createScheduler();
	char *argv[] = {"shell"};
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
	createProcess((uint64_t) sampleCodeModuleAddress, argv, 1,"shell", 5, fileDescriptors);
	//descomentar si se quiere testear
	
	char *argvp[] = {"test_processes", "56"};
	createProcess((uint64_t) test_processes, argvp, 2,"test_processes", 1, fileDescriptors);
	/*
	char *argvpi[] = {"test_prio"};
	createProcess((uint64_t) test_prio, argvpi, 1,"test_prio", 1, fileDescriptors);
	
	char *argvm[] = {"test_mm", "1064960"};
	createProcess((uint64_t) test_mm, argvm, 2,"test_mm", 1, fileDescriptors);
	*/
	_sti();

	while (1);

	return 0;
}
