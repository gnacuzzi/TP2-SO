// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdint.h>
#include <videoDriver.h>
#include <keyboard.h>
#include <lib.h>
#include <time.h>
#include <memoryManager.h>
#include <vars.h>
#include <scheduler.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

extern const uint64_t registers[18];
extern const uint64_t capturedReg;

static void syscallRead(uint64_t buffer);
static void syscallWrite(uint32_t fd, char c);
static void syscallClear();
static void syscallSeconds(uint64_t arg0);
static void syscallMinutes(uint64_t arg0);
static void syscallHours(uint64_t arg0);
static void syscallLettersize(int size);
static void syscallRegisters(uint64_t *buffer, uint64_t *flag);
static void makeSound(uint64_t freq, uint64_t time);
static void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
static uint64_t syscallTicks();
static void syscallWait(uint64_t time);
static int64_t syscallMemInfo(mem_info * memInfo);
static int16_t syscallCreateProcess(uint64_t rip, char **args, int argc,char *name, uint8_t priority, int16_t fileDescriptors[]);
static PCB * processDump(uint16_t * proccesQty);
void syscallExit();

typedef uint64_t (*syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,
						   uint64_t arg5) {
	static syscall syscalls[] = {(syscall) syscallRead,		  (syscall) syscallWrite,	  (syscall) syscallClear,
								 (syscall) syscallSeconds,	  (syscall) syscallMinutes,	  (syscall) syscallHours,
								 (syscall) syscallLettersize, (syscall) syscallRegisters, (syscall) makeSound,
								 (syscall) drawRectangle,	  (syscall) syscallTicks,	  (syscall) syscallWait,
								 (syscall) malloc,			  (syscall) free, 			  (syscall) syscallMemInfo,
								 (syscall) syscallCreateProcess, (syscall) getPid,		  (syscall) processDump,
								 (syscall) killProcess, 	  (syscall) changePriority,	  (syscall) blockProcess,
								 (syscall) readyProcess,	  (syscall) yield,			  (syscall) waitProcess,
								 (syscall) syscallExit };
	return syscalls[nr](arg0, arg1, arg2, arg3, arg4, arg5);
}
//0
static void syscallRead(uint64_t buffer) {
	((char *) buffer)[0] = next();
}
//1
static void syscallWrite(uint32_t fd, char c) {
	if (fd == STDERR)
		return;
	else if (fd != STDOUT)
		return;
	draw_char(c);
}
//2
static void syscallClear() {
	clear_screen();
}
//3
static void syscallSeconds(uint64_t sec) {
	*((int *) sec) = get_sec();
}
//4
static void syscallMinutes(uint64_t min) {
	*((int *) min) = get_min();
}
//5
static void syscallHours(uint64_t hour) {
	*((int *) hour) = get_hour();
}
//6
static void syscallLettersize(int size) {
	set_lettersize(size);
}
//7
static void syscallRegisters(uint64_t *buffer, uint64_t *flag) {
	*flag = capturedReg;
	if (capturedReg) {
		for (int i = 0; i < 18; i++) {
			buffer[i] = registers[i];
		}
	}
}
//8
static void makeSound(uint64_t freq, uint64_t tick) {
	if (freq > 0) {
		beep(freq);
	}
	wait_time(tick);
	if (freq > 0)
		stop_beep();
}
//9
static void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	ColorInt mycolor = {bits: color};
	draw_rect(x, y, width, height, mycolor.color);
}
//10
static uint64_t syscallTicks() {
	return ticks_elapsed();
}
//11
static void syscallWait(uint64_t time) {
	wait_time((int) time);
}
//12 malloc
//13 free
//14
static int64_t syscallMemInfo(mem_info * memInfo){
 	if(memInfo == NULL) {
        return -1;
    }
    meminfo(memInfo);
    return 0;
}
//15
static int16_t syscallCreateProcess(uint64_t rip, char **args, int argc,char *name, uint8_t priority, int16_t fileDescriptors[]) {
	return createProcess(rip, args, argc, name, priority, fileDescriptors);
}

//16 getPid
//17 ps
static PCB * processDump(uint16_t * proccesQty){
	return ps(proccesQty);
}
//18 killProcess
//19 changePriority
//20 blockProcess
//21 unblockProcess
//22 yield
//23 waitProcess

//24
void syscallExit(){
	killCurrentProcess();
	yield();
}