#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include "color.h"

typedef struct mem_info {
	uint64_t total;
	uint64_t free;
	uint64_t used;
} mem_info;

typedef struct PSinfo{
    int16_t pid;
    uint64_t stackBase;
    uint64_t stackPos;
    char *name;
    uint8_t priority;
    int ground;    
} PSinfo;

#define STDIN 0
#define STDOUT 1
#define STDERR 2

void sysRead(int64_t fd, char * buffer, uint64_t size);
void sysWrite(uint32_t fd, char * buffer, uint64_t size);

void clear_screen();

void get_seconds(int *sec);
void get_minutes(int *min);
void get_hours(int *hours);

void setlettersize(int size);

uint64_t *getRegs(uint64_t *regarr, uint64_t *flag);

void make_sound(uint64_t freq, uint64_t time);

void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

uint64_t get_ticks();

void wait_delta(uint64_t time);

void *sysmalloc(uint64_t size);

void sysfree(void * p);

int64_t sysgetMemInfo(mem_info *memInfo);

int syscreateProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);

int16_t sysgetpid();

PSinfo * sysps(uint16_t * proccesQty);

int64_t syskillProcess(int16_t pid);

int syschangePriority(int16_t pid, uint8_t priority);

int64_t sysblockProcess(int16_t pid);

int64_t sysunblockProcess(int16_t pid);

void sysyield();

int syswaitProcess(int16_t pid);

void sysexit();

int syschangeFds(int16_t pid, int16_t fileDescriptors[]);

int8_t syssemOpen(uint16_t id);
int8_t syssemClose(uint16_t id);
int8_t syspost(uint16_t id);
int8_t syswait(uint16_t id);
int8_t syssemInit(uint16_t id, uint32_t initialValue);

int64_t sysopenPipe(int16_t pid, uint8_t mode);
int64_t sysclosePipe(int64_t fd);

int scanLine(char *buffer, int maxLen);


#endif