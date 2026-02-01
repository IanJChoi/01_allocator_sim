#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *bp);
void print_heap_layout(void);

#endif
