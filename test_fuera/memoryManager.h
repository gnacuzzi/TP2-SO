#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct mem_info {
	uint64_t total;
	uint64_t free;
	uint64_t used;
} mem_info;

typedef struct MemoryManagerCDT *MemoryManagerADT;

#define HEAP_SIZE 0x10000000 // 256 MiB

#define BLOCK_SIZE 64
#define STRUCT_SIZE (HEAP_SIZE / BLOCK_SIZE)

#ifdef BUDDY
#undef STRUCT_SIZE
#define STRUCT_SIZE sizeof(MemoryManagerCDT)
#endif

void mminit(void *start, uint64_t size);
void *malloc(uint64_t size);
void free(void *p);
void meminfo(mem_info *mem_info);

#endif