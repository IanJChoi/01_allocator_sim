#include "allocator_internal.h"

#include <stdio.h>

static void print_block(void *p) {
    if (p == NULL) {
        return;
    }

    header_t *h = get_header(p);
    footer_t *f = get_footer(p);
    size_t size = (size_t)h->size;
    int alloc = (int)h->alloc;
    size_t payload_size = size >= (size_t)DSIZE ? size - (size_t)DSIZE : 0;
    char header_sz[16];
    char payload_sz[16];
    char footer_sz[16];

    snprintf(header_sz, sizeof(header_sz), "%zuB", (size_t)WSIZE);
    snprintf(payload_sz, sizeof(payload_sz), "%zuB", payload_size);
    snprintf(footer_sz, sizeof(footer_sz), "%zuB", (size_t)WSIZE);

    printf(
        "%p %6s %-7s [size=%zu | %d]\n"
        "%p %6s %-7s\n"
        "%p %6s %-7s [size=%zu | %d]\n",
        (void *)h, header_sz, "header", size, alloc,
        (void *)p, payload_sz, "payload",
        (void *)f, footer_sz, "footer", (size_t)f->size, (int)f->alloc
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
