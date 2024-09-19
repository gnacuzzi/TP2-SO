
#include <lib/memory_manager.h>
#define FREE 0
#define USED 1
#define BORDER 2

static uint8_t *bitmap;
static void *arena;
static uint64_t blocks_qty;
static uint64_t used_blocks;

void mminit(void *start, uint64_t size) {

    //chequeo de que el size no se vaya

    blocks_qty = (size - STRUCT_SIZE) / BLOCK_SIZE;

    bitmap = (uint8_t *)start;
    arena = bitmap + blocks_qty;

    for (uint64_t i = 0; i < blocks_qty; i++) {
        bitmap[i] = FREE;
    }

    used_blocks = 0;
}

void *malloc(uint64_t size) {
    if (size == 0) {
        return NULL;
    }

    uint64_t blocks_needed = (size - STRUCT_SIZE) / BLOCK_SIZE + ((size % BLOCK_SIZE) ? 1 : 0);

    uint64_t free_blocks = 0;
    for (uint64_t i = 0; i < blocks_qty; i++) {
        if (bitmap[i] == FREE) {
            free_blocks++;
            if (free_blocks == blocks_needed) {

                bitmap[i - free_blocks + 1] = BORDER;

                for (uint64_t j = i - free_blocks + 2; j <= i; j++) {
                    bitmap[j] = USED;
                }

                used_blocks += blocks_needed;

                return (uint8_t *)arena + (i - free_blocks + 1) * BLOCK_SIZE;
            }
        } else {
            free_blocks = 0;
        }
    }

    return NULL;
}

void free(void *p) {
    if (p == NULL) {
        return;
    }

    uint64_t block_index = ((uint8_t *)p - (uint8_t *)arena) / BLOCK_SIZE;

    if (bitmap[block_index] != BORDER) {
        return;
    }

    bitmap[block_index] = FREE;
    used_blocks--;

    for (uint64_t i = block_index + 1; i < blocks_qty && bitmap[i] == USED; i++) {
        bitmap[i] = FREE;
        used_blocks--;
    }
}

void meminfo(mem_info *info) {
    info->total = blocks_qty * BLOCK_SIZE;
    info->used = used_blocks * BLOCK_SIZE;
    info->free = (blocks_qty - used_blocks) * BLOCK_SIZE;
}

