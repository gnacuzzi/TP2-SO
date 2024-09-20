
#include <memory_manager.h>
#define FREE 0
#define USED 1
#define BORDER 2

static MemoryManagerADT getMemoryManager();

static void * firstAddress;

typedef struct MemoryManagerCDT {
    uint8_t *bitmap;
    void *arena;
    uint64_t blocks_qty;
    uint64_t used_blocks;    
} MemoryManagerCDT;

void mminit(void *start, uint64_t size) {

    firstAddress = start;

    MemoryManagerADT memoryManager = (MemoryManagerADT)firstAddress;
    //chequeo de que el size no se vaya

    memoryManager->blocks_qty = (size - STRUCT_SIZE) / BLOCK_SIZE;

    memoryManager->bitmap = (uint8_t *)start;
    memoryManager->arena = memoryManager->bitmap + memoryManager->blocks_qty;

    for (uint64_t i = 0; i < memoryManager->blocks_qty; i++) {
        memoryManager->bitmap[i] = FREE;
    }

    memoryManager->used_blocks = 0;
}

void *malloc(uint64_t size) {

    MemoryManagerADT memoryManager = getMemoryManager();

    if (size == 0) {
        return NULL;
    }

    uint64_t blocks_needed = (size - STRUCT_SIZE) / BLOCK_SIZE + ((size % BLOCK_SIZE) ? 1 : 0);

    uint64_t free_blocks = 0;
    for (uint64_t i = 0; i < memoryManager->blocks_qty; i++) {
        if (memoryManager->bitmap[i] == FREE) {
            free_blocks++;
            if (free_blocks == blocks_needed) {

                memoryManager->bitmap[i - free_blocks + 1] = BORDER;

                for (uint64_t j = i - free_blocks + 2; j <= i; j++) {
                    memoryManager->bitmap[j] = USED;
                }

                memoryManager->used_blocks += blocks_needed;

                return (uint8_t *)memoryManager->arena + (i - free_blocks + 1) * BLOCK_SIZE;
            }
        } else {
            free_blocks = 0;
        }
    }

    return NULL;
}

void free(void *p) {

    MemoryManagerADT memoryManager = getMemoryManager();

    if (p == NULL) {
        return;
    }

    uint64_t block_index = ((uint8_t *)p - (uint8_t *)memoryManager->arena) / BLOCK_SIZE;

    if (memoryManager->bitmap[block_index] != BORDER) {
        return;
    }

    memoryManager->bitmap[block_index] = FREE;
    memoryManager->used_blocks--;

    for (uint64_t i = block_index + 1; i < memoryManager->blocks_qty && memoryManager->bitmap[i] == USED; i++) {
        memoryManager->bitmap[i] = FREE;
        memoryManager->used_blocks--;
    }
}

void meminfo(mem_info *info) {

    MemoryManagerADT memoryManager = getMemoryManager();

    info->total = memoryManager->blocks_qty * BLOCK_SIZE;
    info->used = memoryManager->used_blocks * BLOCK_SIZE;
    info->free = (memoryManager->blocks_qty - memoryManager->used_blocks) * BLOCK_SIZE;
}

MemoryManagerADT getMemoryManager() {
    return (MemoryManagerADT)firstAddress;
}