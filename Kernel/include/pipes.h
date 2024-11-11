#ifndef _PIPES_H
#define _PIPES_H
#include <stdint.h>

typedef struct PipesCDT *PipesADT;

void startPipes();
int64_t openPipe(int16_t pid, uint8_t mode);
int64_t closePipe(int64_t fd);
int64_t writePipe(int64_t fd, const char *buffer, int64_t size);
int64_t readPipe(int64_t fd, char *buffer, int64_t size);

#endif