#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *bp);
typedef struct {
    uint64_t size : 61;
    uint64_t tag : 2;
    uint64_t alloc : 1;
} header_t;

typedef struct {
    uint64_t size : 61;
    uint64_t tag : 2;
    uint64_t alloc : 1;
} footer_t;

void print_heap_layout(void);
void init_print(void);
void *mm_heap_listp(void);
header_t *get_header(void *payload);
footer_t *get_footer(void *payload);
void *extend_heap(void);
void *next_payload(void *payload);
void *prev_payload(void *payload);

#endif
