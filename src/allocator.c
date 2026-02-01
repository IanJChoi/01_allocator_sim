#include "allocator.h"

#include <stdint.h>
#include <stdio.h>

// 4MB simulated heap, allocated upfront.
enum { SIM_HEAP_SIZE = 4 * 1024 * 1024 };
enum { CHUNK_SIZE = 4 * 1024 };
enum { WSIZE = 8, DSIZE = 16 };

static uint8_t sim_heap[SIM_HEAP_SIZE];
static uint8_t *heap_start = NULL;
static uint8_t *heap_brk = NULL;
static uint8_t *heap_max = NULL;
static uint8_t *heap_listp = NULL;

#define PACK(size, alloc) ((size) | (alloc))
#define PUT(p, val) (*(uint64_t *)(p) = (uint64_t)(val))

static void mem_init(void) {
    if (heap_start != NULL) {
        return;
    }
    heap_start = sim_heap;
    heap_brk = sim_heap;
    heap_max = sim_heap + SIM_HEAP_SIZE;
}

static void *mem_sbrk(size_t incr) {
    uint8_t *old_brk = heap_brk;
    if (heap_brk + incr > heap_max) {
        return (void *)-1;
    }
    heap_brk += incr;
    return old_brk;
}

int mm_init(void) {
    mem_init();

    // Initial block: padding + prologue header/footer + epilogue header.
    if (mem_sbrk(4 * WSIZE) == (void *)-1) {
        return -1;
    }

    PUT(heap_start, 0);                                // padding
    PUT(heap_start + (1 * WSIZE), PACK(DSIZE, 1));     // prologue header
    PUT(heap_start + (2 * WSIZE), PACK(DSIZE, 1));     // prologue footer
    PUT(heap_start + (3 * WSIZE), PACK(0, 1));         // epilogue header

    heap_listp = heap_start + (2 * WSIZE);

    return 0;
}

void *mm_malloc(size_t size) {
    (void)size;
    return NULL;
}

void mm_free(void *bp) {
    (void)bp;
}

void print_heap_layout_mm_init(void) {
    printf(
        "Heap Layout (total = 32 bytes)\n"
        "-----------------------------------------\n"
        "%p  8B, pad\n"
        "%p  8B, prologue header  [size=16 | 1]\n"
        "%p  8B, prologue footer  [size=16 | 1]\n"
        "%p  8B, epilogue header  [size=0  | 1]\n"
        "-----------------------------------------\n"
        "heap_listp -> %p\n",
        (void *)(heap_start + (0 * WSIZE)),
        (void *)(heap_start + (1 * WSIZE)),
        (void *)(heap_start + (2 * WSIZE)),
        (void *)(heap_start + (3 * WSIZE)),
        (void *)heap_listp
    );
}


void init_print(void) {
    printf(
        "This program demonstrates how a memory allocator works by visualizing the memory layout.\n"
        "It uses a 4MB simulated heap, which is extended in 4KB increments.\n"
        "The allocator assumes a 64-bit system with 16-byte alignment.\n"
        "Each block contains an 8-byte header and an 8-byte footer.\n"
        "\n"
        "Below is the initial state of the memory:\n"
        "\n"
    );
}