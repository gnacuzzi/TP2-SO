// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/memoryManager.h"
#define FREE 0
#define USED 1
#define BORDER 2


static void * firstAddress;

typedef struct MemoryManagerCDT {
    uint8_t *bitmap;
    void *arena;
    uint64_t blocksQty;
    uint64_t usedBlocks;    
} MemoryManagerCDT;

static MemoryManagerADT getMemoryManager();

void mminit(void *start, uint64_t size) {

    firstAddress = start;

    MemoryManagerADT memoryManager = (MemoryManagerADT)firstAddress;

    uint64_t sizeMM = sizeof(MemoryManagerCDT);

    memoryManager->blocksQty = (size - sizeMM) / BLOCK_SIZE;

    memoryManager->bitmap = (uint8_t *)(firstAddress + sizeMM);

    memoryManager->arena = memoryManager->bitmap + (memoryManager->blocksQty / 8);

    for (uint64_t i = 0; i < memoryManager->blocksQty; i++) {
        memoryManager->bitmap[i] = FREE;
    }

    memoryManager->usedBlocks = 0;
}

void *malloc(uint64_t size) {

    MemoryManagerADT memoryManager = getMemoryManager();

    if (size == 0) {
        return NULL;
    }

    uint64_t blocksNeeded = size / BLOCK_SIZE + ((size % BLOCK_SIZE) ? 1 : 0);

    uint64_t freeBlocks = 0;

    for (uint64_t i = 0; i < memoryManager->blocksQty; i++) {
        if (memoryManager->bitmap[i] == FREE) {
            freeBlocks++;
            if (freeBlocks == blocksNeeded) {

                memoryManager->bitmap[i - freeBlocks + 1] = BORDER;

                for (uint64_t j = i - freeBlocks + 2; j <= i; j++) {
                    memoryManager->bitmap[j] = USED;
                }

                memoryManager->usedBlocks += blocksNeeded;

                return (uint8_t *)memoryManager->arena + (i - freeBlocks + 1) * BLOCK_SIZE;
            }
        } else {
            freeBlocks = 0; 
        }
    }

    return NULL;
}

void free(void *p) {

    MemoryManagerADT memoryManager = getMemoryManager();

    if (p == NULL) {
        return;
    }

    uint64_t blockIndex = ((uint8_t *)p - (uint8_t *)memoryManager->arena) / BLOCK_SIZE;

    if (memoryManager->bitmap[blockIndex] != BORDER) {
        return;
    }

    memoryManager->bitmap[blockIndex] = FREE;
    memoryManager->usedBlocks--;

    for (uint64_t i = blockIndex + 1; i < memoryManager->blocksQty && memoryManager->bitmap[i] == USED; i++) {
        memoryManager->bitmap[i] = FREE;
        memoryManager->usedBlocks--;
    }
}

void meminfo(mem_info *info) {

    MemoryManagerADT memoryManager = getMemoryManager();

    info->total = memoryManager->blocksQty * BLOCK_SIZE;
    info->used = memoryManager->usedBlocks * BLOCK_SIZE;
    info->free = (memoryManager->blocksQty - memoryManager->usedBlocks) * BLOCK_SIZE;
}

MemoryManagerADT getMemoryManager() {
    return (MemoryManagerADT)firstAddress;
}