#include "allocator_internal.h"

static uint8_t sim_heap[SIM_HEAP_SIZE];

uint8_t *heap_start = NULL;
uint8_t *heap_brk = NULL;
uint8_t *heap_max = NULL;
uint8_t *heap_listp = NULL;

void mem_init(void) {
    if (heap_start != NULL) {
        return;
    }
    heap_start = sim_heap;
    heap_brk = sim_heap;
    heap_max = sim_heap + SIM_HEAP_SIZE;
}

void *mem_sbrk(size_t incr) {
    uint8_t *old_brk = heap_brk;
    if (heap_brk + incr > heap_max) {
        return (void *)-1;
    }
    heap_brk += incr;
    return old_brk;
}

void *mm_heap_listp(void) {
    return heap_listp;
}
