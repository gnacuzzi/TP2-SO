#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct mem_info {
    uint64_t total;
    uint64_t free;
    uint64_t used;
} mem_info;

typedef MemoryManagerCDT * MemoryManagerADT;

#define HEAP_SIZE 0x40000 // 256 KiB

#define BLOCK_SIZE 64
#define STRUCT_SIZE (HEAP_SIZE / BLOCK_SIZE)

void mminit(void * start, uint64_t size);
void * malloc(uint64_t size);
void free(void * p);
void meminfo(mem_info * mem_info);

#endif 