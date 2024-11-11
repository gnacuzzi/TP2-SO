#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <stdint.h>

typedef struct SemaphoreCDT *SemaphoreADT;

void startSemaphores();
int8_t semInit(uint16_t id, uint32_t initialValue);
int8_t semOpen(uint16_t id);
int8_t semClose(uint16_t id);
int8_t post(uint16_t id);
int8_t wait(uint16_t id);

#endif