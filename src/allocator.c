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

typedef struct {
    uint64_t size : 63;
    uint64_t alloc : 1;
} header_t;

typedef struct {
    uint64_t size : 63;
    uint64_t alloc : 1;
} footer_t;

static void write_header(void *p, size_t size, int alloc) {
    header_t *h = (header_t *)p;
    h->size = size;
    h->alloc = alloc ? 1 : 0;
}

static void write_footer(void *p, size_t size, int alloc) {
    footer_t *f = (footer_t *)p;
    f->size = size;
    f->alloc = alloc ? 1 : 0;
}

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

    *(uint64_t *)heap_start = 0;                               // padding
    write_header(heap_start + (1 * WSIZE), DSIZE, 1);          // prologue header
    write_footer(heap_start + (2 * WSIZE), DSIZE, 1);          // prologue footer
    write_header(heap_start + (3 * WSIZE), 0, 1);              // epilogue header

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

void *mm_heap_listp(void) {
    return (void *)heap_listp;
}

void print_heap_layout_mm_init(void) {
    size_t total_bytes = 4 * (size_t)WSIZE;
    size_t word_bytes = (size_t)WSIZE;
    size_t pro_size = (size_t)DSIZE;
    size_t epi_size = 0;
    int alloc = 1;

    printf(
        "Heap Layout (total = %zu bytes)\n"
        "------------------------------------------------\n"
        "%p  %zuB, padding\n"
        "%p  %zuB, prologue header  [size=%zu | %d]\n"
        "%p  %zuB, prologue footer  [size=%zu | %d]\n"
        "%p  %zuB, epilogue header  [size=%zu  | %d]\n"
        "------------------------------------------------\n"
        "heap_listp -> %p\n",
        total_bytes,
        (void *)(heap_start + (0 * WSIZE)), word_bytes,
        (void *)(heap_start + (1 * WSIZE)), word_bytes, pro_size, alloc,
        (void *)(heap_start + (2 * WSIZE)), word_bytes, pro_size, alloc,
        (void *)(heap_start + (3 * WSIZE)), word_bytes, epi_size, alloc,
        (void *)heap_listp
    );
}


void init_print(void) {
    printf(
        "\n"
        "This program demonstrates how a memory allocator works by visualizing the memory layout.\n"
        "It uses a 4MB simulated heap, which is extended in 4KB increments.\n"
        "The allocator assumes a 64-bit system with 16-byte alignment.\n"
        "Each block contains an 8-byte header and an 8-byte footer.\n"
        "\n"
        "Below is the initial state of the memory:\n"
        "\n"
    );
}
