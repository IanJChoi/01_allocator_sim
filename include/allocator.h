#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *bp);
typedef struct {
    uint64_t size : 63;
    uint64_t alloc : 1;
} header_t;

typedef struct {
    uint64_t size : 63;
    uint64_t alloc : 1;
} footer_t;

void print_heap_layout_mm_init(void);
void init_print(void);
void *mm_heap_listp(void);
header_t *get_header(void *payload);
footer_t *get_footer(void *payload);

#endif
