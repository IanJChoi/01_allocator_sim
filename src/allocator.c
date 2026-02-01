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

void *extend_heap(void) {
    void *old_brk = mem_sbrk(CHUNK_SIZE);
    if (old_brk == (void *)-1) {
        return (void *)-1;
    }

    void *payload = old_brk;
    size_t free_size = (size_t)CHUNK_SIZE;

    write_header(get_header(payload), free_size, 0);
    write_footer(get_footer(payload), free_size, 0);
    write_header((uint8_t *)get_header(payload) + free_size, 0, 1);

    return payload;
}

void *next_payload(void *payload) {
    if (payload == NULL) {
        return NULL;
    }
    header_t *h = get_header(payload);
    size_t size = (size_t)h->size;
    return (void *)((uint8_t *)payload + size);
}

void *prev_payload(void *payload) {
    if (payload == NULL) {
        return NULL;
    }
    footer_t *prev_footer = (footer_t *)((uint8_t *)get_header(payload) - WSIZE);
    size_t size = (size_t)prev_footer->size;
    return (void *)((uint8_t *)payload - size);
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

    if (extend_heap() == (void *)-1) {
        return -1;
    }

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

header_t *get_header(void *payload) {
    if (payload == NULL) {
        return NULL;
    }
    return (header_t *)((uint8_t *)payload - WSIZE);
}

footer_t *get_footer(void *payload) {
    if (payload == NULL) {
        return NULL;
    }
    header_t *h = get_header(payload);
    size_t size = (size_t)h->size;
    return (footer_t *)((uint8_t *)payload + size - DSIZE);
}

static void print_block(void *p) {
    if (p == NULL) {
        return;
    }

    header_t *h = get_header(p);
    footer_t *f = get_footer(p);
    size_t size = (size_t)h->size;
    int alloc = (int)h->alloc;
    size_t payload_size = size >= (size_t)DSIZE ? size - (size_t)DSIZE : 0;

    printf(
        "%p  %zuB, header  [size=%zu | %d]\n"
        "%p  %zuB, payload\n"
        "%p  %zuB, footer  [size=%zu | %d]\n",
        (void *)h, (size_t)WSIZE, size, alloc,
        (void *)p, payload_size,
        (void *)f, (size_t)WSIZE, (size_t)f->size, (int)f->alloc
    );
}

void print_heap_layout(void) {
    void *p = heap_listp;
    printf("------------------------------------------------\n");
    while (p != NULL) {
        header_t *h = get_header(p);
        if (h == NULL) {
            break;
        }
        if (h->size == 0) {
            break;
        }
        print_block(p);
        p = next_payload(p);
    }
    printf("------------------------------------------------\n");
}

void print_heap_layout_mm_init(void) {
    size_t total_bytes = (4 * (size_t)WSIZE) + (size_t)CHUNK_SIZE;
    void *pro_payload = heap_listp;
    header_t *pro_header = get_header(pro_payload);
    footer_t *pro_footer = get_footer(pro_payload);
    uint8_t *padding = (uint8_t *)pro_header - WSIZE;

    void *free_payload = next_payload(pro_payload);
    header_t *free_header = get_header(free_payload);
    footer_t *free_footer = get_footer(free_payload);
    size_t free_size = (size_t)free_header->size;
    size_t free_payload_size = free_size - (size_t)DSIZE;
    header_t *new_epilogue = (header_t *)((uint8_t *)free_header + free_size);

    printf(
        "Heap Layout (total = %zu bytes)\n"
        "------------------------------------------------\n"
        "%p  %zuB, padding\n"
        "%p  %zuB, prologue header  [size=%zu | %d]\n"
        "%p  %zuB, prologue footer  [size=%zu | %d]\n"
        "%p  %zuB, free header      [size=%zu | %d]\n"
        "%p  %zuB, free payload\n"
        "%p  %zuB, free footer      [size=%zu | %d]\n"
        "%p  %zuB, epilogue header  [size=%zu  | %d]\n"
        "------------------------------------------------\n"
        "heap_listp -> %p\n",
        total_bytes,
        padding, (size_t)WSIZE,
        pro_header, (size_t)WSIZE, (size_t)pro_header->size, (int)pro_header->alloc,
        pro_footer, (size_t)WSIZE, (size_t)pro_footer->size, (int)pro_footer->alloc,
        free_header, (size_t)WSIZE, free_size, (int)free_header->alloc,
        (uint8_t *)free_payload, free_payload_size,
        free_footer, (size_t)WSIZE, (size_t)free_footer->size, (int)free_footer->alloc,
        new_epilogue, (size_t)WSIZE, (size_t)new_epilogue->size, (int)new_epilogue->alloc,
        heap_listp
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
