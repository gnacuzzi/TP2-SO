// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <memoryManager.h>
#include <stdio.h>
#include <stdint.h>

#define MIN_EXPONENT 6
#define MIN_ALLOC 64
#define MAX_EXPONENT 28   
#define MAX_ALLOC 268435456
#define NODES 8388607

#define TRUE 1
#define FALSE !TRUE


typedef struct {
    unsigned int used;
    unsigned int usedChildren;
} Node;

typedef struct MemoryManagerCDT {
    void *firstAddress;
    Node *nodes;
    uint64_t totalMemory;
    uint64_t usedMemory;
} MemoryManagerCDT;

static uint64_t getExponent(uint64_t size);
static int64_t freeNode(uint64_t exponent);
static void treeUpdate(int64_t node, int64_t used);
static void setFlag(int64_t node, int64_t used);
static void * adress(int64_t node, uint64_t exponent);
static int64_t getMaxExponent(void *ptr);
static int64_t searchNode(int64_t startIdx, uint64_t *level, void *ptr);
static int64_t getStartNode(void *ptr, int64_t maxLevel);


MemoryManagerADT memoryManager = NULL;
static MemoryManagerADT getMemoryManagerCDT(){
    return (MemoryManagerADT) memoryManager;
}


void mminit(void *start, uint64_t size){
    memoryManager = start;

    if(size < MIN_ALLOC){
        return;
    }

    memoryManager->nodes = (Node *)(start + sizeof(MemoryManagerCDT));
    memoryManager->firstAddress = memoryManager->nodes + sizeof(Node) * NODES;

    memoryManager->totalMemory = size;
    memoryManager->usedMemory = 0;

    for(int i = 0; i < NODES; i++){
        memoryManager->nodes[i].used = FALSE;
        memoryManager->nodes[i].usedChildren = 0;
    }

}

void * buddy_malloc(uint64_t size) {
    MemoryManagerADT memoryManager = getMemoryManagerCDT();
    uint64_t exponent = 0;
    if(size > MAX_ALLOC || size == 0 || (exponent = getExponent(size)) > MAX_EXPONENT){
        return NULL;
    }
    if(exponent < MIN_EXPONENT){
        exponent = MIN_EXPONENT;
    }

    int64_t node = freeNode(exponent);
    if(node == -1){
        return NULL;
    }

    treeUpdate(node, 1);
    setFlag(node, TRUE);

    memoryManager->usedMemory += (1 << exponent);

    return adress(node, exponent);   
}

void buddy_free(void *ptr) {
    MemoryManagerADT memoryManager = getMemoryManagerCDT();
    if (ptr == NULL){
        return;
    }

    uint64_t exponent = getMaxExponent(ptr);
    int64_t node = searchNode(getStartNode(ptr, exponent), &exponent, ptr);

    if (node < 0) return;

    treeUpdate(node, -1);
    setFlag(node, FALSE);

    memoryManager->usedMemory -= (1 << exponent);
}


void meminfo(mem_info *mem_info) {
    MemoryManagerADT memoryManager = getMemoryManagerCDT();
    mem_info->total = memoryManager->totalMemory;
    mem_info->used = memoryManager->usedMemory;
    mem_info->free = memoryManager->totalMemory - memoryManager->usedMemory;
}



static uint64_t getExponent(uint64_t size){
    uint64_t exponent = 0;
    while(size > MIN_ALLOC){
        size = size >> 1;
        exponent++;
    }
    return exponent;
}

static int64_t freeNode(uint64_t exponent) {
    MemoryManagerADT memoryManager = getMemoryManagerCDT();

    int node = (1 << (MAX_EXPONENT - exponent)) - 1;
    int lastNode = (1 << (MAX_EXPONENT - (exponent - 1))) - 1;

    while (node < lastNode && (memoryManager->nodes[node].used || memoryManager->nodes[node].usedChildren)) {
        ++node;
    }

    if (node == lastNode) {
        return -1;
    }

    return node;
}

static void treeUpdate(int64_t node, int64_t used) {
    MemoryManagerADT memoryManager = getMemoryManagerCDT();

    while (node) {
        memoryManager->nodes[node].usedChildren += used;
        node = ((node + 1) >> 1) - 1;
    }
}

static void setFlag(int64_t node, int64_t used) {
    if(node < 0 || node >= NODES){
        return;
    }
    MemoryManagerADT memoryManager = getMemoryManagerCDT();

    memoryManager->nodes[node].used = used;
    setFlag((node << 1) + 1, used);
    setFlag((node << 1) + 2, used);
}

static void * adress(int64_t node, uint64_t exponent) {
    MemoryManagerADT memoryManager = getMemoryManagerCDT();

    return memoryManager->firstAddress + (node - ((1 << (MAX_EXPONENT - exponent)) - 1)) * (1 << exponent);
}

static int64_t getMaxExponent(void *ptr) {
    uintptr_t relativeAddress = (uintptr_t)ptr - (uintptr_t)memoryManager->firstAddress; 
    int64_t level = MAX_EXPONENT; 
    uint64_t powOfTwo = 1 << MAX_EXPONENT; 

    while ((relativeAddress % powOfTwo) != 0) {
        level--; 
        powOfTwo >>= 1; 
    }
    return level; 
}

static int64_t searchNode(int64_t startIdx, uint64_t *level, void *ptr) {
    if (*level < MIN_EXPONENT) {
        return -1;
    }

    if (!memoryManager->nodes[startIdx].used) {
        (*level)--; 
        int64_t leftChildIdx = (startIdx << 1) + 1; 
        int64_t rightChildIdx = (startIdx << 1) + 2; 

        void *address = adress(rightChildIdx, *level); 
        return searchNode(ptr < address ? leftChildIdx : rightChildIdx, level, ptr);
    }

    return ptr == adress(startIdx, *level) ? startIdx : -1;
}

static int64_t getStartNode(void *ptr, int64_t maxLevel) {
    uintptr_t relativeAddress = (uintptr_t)ptr - (uintptr_t)memoryManager->firstAddress; 
    int64_t firstIndexOfMaxExponent = (1 << (MAX_EXPONENT - maxLevel)) - 1; 
    return firstIndexOfMaxExponent + (relativeAddress / (1 << maxLevel));
}

