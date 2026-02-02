#ifndef ALLOCATOR_INTERNAL_H
#define ALLOCATOR_INTERNAL_H

#include "allocator.h"

#include <stddef.h>
#include <stdint.h>

enum { SIM_HEAP_SIZE = 4 * 1024 * 1024 };
enum { CHUNK_SIZE = 4 * 1024 };
enum { WSIZE = 8, DSIZE = 16 };

extern uint8_t *heap_start;
extern uint8_t *heap_brk;
extern uint8_t *heap_max;
extern uint8_t *heap_listp;

void mem_init(void);
void *mem_sbrk(size_t incr);

void write_header(void *p, size_t size, int alloc, int tag);
void write_footer(void *p, size_t size, int alloc, int tag);

#endif
