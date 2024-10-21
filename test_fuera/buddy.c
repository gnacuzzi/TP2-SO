#ifdef BUDDY
#include <memoryManager.h>

#define LEVELS 28
#define MIN_EXPONENT 6
#define TRUE 1
#define FALSE !TRUE

typedef struct MemoryBlock {
    uint64_t exponent;
    uint8_t free;
    struct MemoryBlock *next;
} MemoryBlock;

typedef struct MemoryManagerCDT {
    uint8_t maxExponent;
    MemoryBlock * list[LEVELS];
    uint64_t totalMemory;
    uint64_t freeMemory;
} MemoryManagerCDT;


static void *firstAddress;
static MemoryManagerADT getMemoryManager();
static MemoryBlock *createBlock(void *ptr, uint8_t exponent, MemoryBlock *next);

static unsigned int int_log(uint64_t n, int base) {
	unsigned int count = 1;
	while (n /= base)
		count++;
	return count;
}

void mminit(void *start, uint64_t size){
    firstAddress = start;

	MemoryManagerADT memoryManager = (MemoryManagerADT) firstAddress;

    memoryManager->maxExponent = int_log(size, 2);

    if (memoryManager->maxExponent < MIN_EXPONENT) return;

    for(int i = 0; i < LEVELS; i++){
        memoryManager->list[i] = NULL;
    }

    memoryManager->totalMemory = size;
    memoryManager->freeMemory = size;

    memoryManager->list[memoryManager->maxExponent - 1] = createBlock(firstAddress + sizeof(MemoryManagerCDT), memoryManager->maxExponent, NULL);
}

void *malloc(uint64_t size){
    MemoryManagerADT memoryManager = getMemoryManager();

    if(size == 0 || size > memoryManager->freeMemory) return NULL;

    uint8_t exponent = int_log(size + sizeof(MemoryBlock), 2);

    if(exponent < MIN_EXPONENT) exponent = MIN_EXPONENT;

    if(exponent >= memoryManager->maxExponent) return NULL;

    uint8_t i = exponent - MIN_EXPONENT;

    while(i < LEVELS && memoryManager->list[i] == NULL){
        i++;
    }

    if(i == LEVELS) return NULL;

    MemoryBlock *block = memoryManager->list[i];
    memoryManager->list[i] = block->next;
    block->free = FALSE;

    while(i > exponent - MIN_EXPONENT){
        i--;
        MemoryBlock *buddy = createBlock((void *) (firstAddress + memoryManager->totalMemory / (1 << (i + MIN_EXPONENT))), i + MIN_EXPONENT, memoryManager->list[i]);
        if(buddy == NULL) return NULL;
        memoryManager->list[i] = buddy;
    }

    memoryManager->freeMemory -= 1 << (exponent - MIN_EXPONENT);

    return (void *) block;
}

void free(void *p){
    MemoryManagerADT memoryManager = getMemoryManager();

    MemoryBlock *block = (MemoryBlock *) p;
    uint8_t exponent = block->exponent;

    uint8_t i = exponent - MIN_EXPONENT;

    while(i < LEVELS && memoryManager->list[i] != NULL){
        i++;
    }

    if(i == LEVELS) return;

    block->free = TRUE;

    while(i > exponent - MIN_EXPONENT){
        i--;
        MemoryBlock *buddy = (MemoryBlock *) (firstAddress + memoryManager->totalMemory / (1 << (i + MIN_EXPONENT)) * ((uint64_t) block - (uint64_t) firstAddress) / (1 << (exponent - MIN_EXPONENT)));
        if(buddy->free == FALSE) return;
        if(buddy->exponent != exponent) return;
        if(buddy < block) block = buddy;
        memoryManager->list[i] = buddy->next;
    }

    memoryManager->freeMemory += 1 << (exponent - MIN_EXPONENT);
}

void meminfo(mem_info *mem_info){
    MemoryManagerADT memoryManager = getMemoryManager();

    mem_info->total = memoryManager->totalMemory;
    mem_info->free = memoryManager->freeMemory;
    mem_info->used = memoryManager->totalMemory - memoryManager->freeMemory;
}

MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) firstAddress;
}


static MemoryBlock *createBlock(void *ptr, uint8_t exponent, MemoryBlock *next) {
	MemoryBlock *memoryBlock = (MemoryBlock *) ptr;
	memoryBlock->exponent = exponent;
	memoryBlock->free = TRUE;
	memoryBlock->next = next;
	
	return memoryBlock;
}

#endif
