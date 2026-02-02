#include "allocator_internal.h"

void *extend_heap(void) {
    void *old_brk = mem_sbrk(CHUNK_SIZE);
    if (old_brk == (void *)-1) {
        return (void *)-1;
    }

    void *payload = old_brk;
    size_t free_size = (size_t)CHUNK_SIZE;

    write_header(get_header(payload), free_size, 0, 1);
    write_footer(get_footer(payload), free_size, 0, 1);
    write_header((uint8_t *)get_header(payload) + free_size, 0, 1, 2);

    return payload;
}

int mm_init(void) {
    mem_init();

    // Initial block: padding + prologue header/footer + epilogue header.
    if (mem_sbrk(4 * WSIZE) == (void *)-1) {
        return -1;
    }

    *(uint64_t *)heap_start = 0;                               // padding
    write_header(heap_start + (1 * WSIZE), DSIZE, 1, 0);       // prologue header
    write_footer(heap_start + (2 * WSIZE), DSIZE, 1, 0);       // prologue footer
    write_header(heap_start + (3 * WSIZE), 0, 1, 2);           // epilogue header

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
