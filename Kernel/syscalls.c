// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <videoDriver.h>
#include <keyboard.h>
#include <lib.h>
#include <time.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ 0
#define WRITE 1
#define CLEAR 2
#define SECONDS 3
#define MINUTES 4
#define HOURS 5
#define LETTERSIZE 6
#define REGISTERS 7
#define SOUND 9
#define RECTANGLE 10
#define TICKS 11
#define WAIT 12

extern const uint64_t registers[18];
extern const uint64_t capturedReg;

static void syscall_write(uint32_t fd, char c);
static void syscall_read( uint64_t buffer);
static void syscall_clear();
static void syscall_seconds(uint64_t arg0);
static void syscall_minutes(uint64_t arg0);
static void syscall_hours(uint64_t arg0);
static void syscall_lettersize(int size);
static void syscall_registers(uint64_t * buffer, uint64_t * flag);
static void make_sound(uint64_t freq, uint64_t time);
static void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
static uint64_t syscall_ticks();
static void syscall_wait(uint64_t time);


uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	switch (nr) {
        case READ:
            syscall_read(arg0);
		case WRITE:
			syscall_write((uint32_t)arg0, (char)arg1);
            break;
        case CLEAR:
            syscall_clear();
            break;
        case SECONDS:
            syscall_seconds(arg0);
            break;
        case MINUTES:
            syscall_minutes(arg0);
            break;
        case HOURS:
            syscall_hours(arg0);
            break;
        case LETTERSIZE:
            syscall_lettersize((int) arg0);
            break;
        case REGISTERS:
            syscall_registers((uint64_t *) arg0, (uint64_t *) arg1);
            break;
        case SOUND:
            make_sound(arg0, arg1);
            break;
        case RECTANGLE:
            draw_rectangle((uint16_t)arg0, (uint16_t)arg1, (uint16_t)arg2, (uint16_t)arg3, (uint32_t)arg4);
            break;
        case TICKS:
            return syscall_ticks();
            break;
        case WAIT:
            syscall_wait(arg0);
            break;
	}
	return 0;
}


static void syscall_read(uint64_t buffer){
    ((char*) buffer)[0] = next();
}

static void syscall_write(uint32_t fd, char c){
    if(fd == STDERR)
        return; 
    else if(fd != STDOUT)
        return;
    draw_char(c);
}

static void syscall_clear(){
    clear_screen();
}

static void syscall_seconds(uint64_t sec){
    *((int *)sec) = get_sec();
}

static void syscall_minutes(uint64_t min){
    *((int *)min) = get_min();
}

static void syscall_hours(uint64_t hour){
    *((int *)hour) = get_hour();
}


static void syscall_lettersize(int size){
    set_lettersize(size);
}

static void syscall_registers(uint64_t * buffer, uint64_t * flag){
    *flag = capturedReg;
    if (capturedReg){
        for(int i = 0; i<18; i++){
            buffer[i] = registers[i];
        }
    }
}

static void make_sound(uint64_t freq, uint64_t tick){
    if(freq > 0){
        beep(freq);
    }
    wait_time(tick);
    if(freq > 0)
        stop_beep();
}


static void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    ColorInt mycolor = { bits: color };
    draw_rect(x, y, width, height, mycolor.color);
}

static uint64_t syscall_ticks(){
    return ticks_elapsed();
}

static void syscall_wait(uint64_t time){
    wait_time((int)time);
}
