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

static void *coalesce(void *bp) {
    if (bp == NULL) {
        return NULL;
    }

    header_t *h = get_header(bp);
    size_t size = (size_t)h->size;

    footer_t *prev_footer = (footer_t *)((uint8_t *)get_header(bp) - WSIZE);
    int prev_alloc = prev_footer != NULL ? (int)prev_footer->alloc : 1;

    void *next_bp = next_payload(bp);
    header_t *next_header = next_bp != NULL ? get_header(next_bp) : NULL;
    int next_alloc = next_header != NULL ? (int)next_header->alloc : 1;

    if (prev_alloc && next_alloc) {
        return bp;
    }

    if (prev_alloc && !next_alloc) {
        size += (size_t)next_header->size;
        write_header(get_header(bp), size, 0, 1);
        write_footer(get_footer(next_bp), size, 0, 1);
        return bp;
    }

    if (!prev_alloc && next_alloc) {
        void *prev_bp = prev_payload(bp);
        header_t *prev_header = get_header(prev_bp);
        size += (size_t)prev_header->size;
        write_header(prev_header, size, 0, 1);
        write_footer(get_footer(bp), size, 0, 1);
        return prev_bp;
    }

    void *prev_bp = prev_payload(bp);
    header_t *prev_header = get_header(prev_bp);
    size += (size_t)prev_header->size + (size_t)next_header->size;
    write_header(prev_header, size, 0, 1);
    write_footer(get_footer(next_bp), size, 0, 1);
    return prev_bp;
}

static void place(void *p, size_t asize) {
    if (p == NULL) {
        return;
    }
    header_t *h = get_header(p);
    size_t csize = (size_t)h->size;

    if (csize < asize) {
        return;
    }

    if (csize - asize >= (size_t)DSIZE) {
        write_header(get_header(p), asize, 1, 1);
        write_footer(get_footer(p), asize, 1, 1);

        void *next = (uint8_t *)p + asize;
        size_t free_size = csize - asize;
        write_header(get_header(next), free_size, 0, 1);
        write_footer(get_footer(next), free_size, 0, 1);
    } else {
        write_header(get_header(p), csize, 1, 1);
        write_footer(get_footer(p), csize, 1, 1);
    }
}

static size_t align_size(size_t size) {
    return (size + (size_t)(DSIZE - 1)) & ~(size_t)(DSIZE - 1);
}

static void *find_fit(size_t asize) {
    void *p = heap_listp;
    while (p != NULL) {
        header_t *h = get_header(p);
        if (h == NULL) {
            break;
        }
        if (h->size == 0) {
            break;
        }
        if (h->alloc == 0 && (size_t)h->size >= asize) {
            return p;
        }
        p = next_payload(p);
    }
    return NULL;
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
    if (size == 0) {
        return NULL;
    }

    size_t asize = align_size(size + (size_t)DSIZE);
    if (asize < (size_t)DSIZE) {
        asize = (size_t)DSIZE;
    }

    void *p = find_fit(asize);
    if (p != NULL) {
        place(p, asize);
        return p;
    }

    p = extend_heap();
    if (p == (void *)-1 || p == NULL) {
        return NULL;
    }

    p = coalesce(p);
    place(p, asize);
    return p;
}

void mm_free(void *bp) {
    if (bp == NULL) {
        return;
    }
    header_t *h = get_header(bp);
    size_t size = (size_t)h->size;
    write_header(get_header(bp), size, 0, 1);
    write_footer(get_footer(bp), size, 0, 1);
    coalesce(bp);
}
