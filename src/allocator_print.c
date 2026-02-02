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
    unsigned int tag = (unsigned int)h->tag;
    size_t payload_size = size >= (size_t)DSIZE ? size - (size_t)DSIZE : 0;
    const char *header_label = "header";
    const char *payload_label = "payload";
    const char *footer_label = "footer";
    char header_sz[16];
    char payload_sz[16];
    char footer_sz[16];

    snprintf(header_sz, sizeof(header_sz), "%zuB", (size_t)WSIZE);
    snprintf(payload_sz, sizeof(payload_sz), "%zuB", payload_size);
    snprintf(footer_sz, sizeof(footer_sz), "%zuB", (size_t)WSIZE);

    switch (tag) {
        case 0:
            header_label = "pro_H";
            payload_label = "pro_P";
            footer_label = "pro_F";
            break;
        case 1:
            if (alloc == 0) {
                header_label = "free_H";
                payload_label = "free_P";
                footer_label = "free_F";
            } else {
                header_label = "alloc_H";
                payload_label = "alloc_P";
                footer_label = "alloc_F";
            }
            break;
        default:
            break;
    }

    printf(
        "%p %6s %-7s [size=%zu | alloc=%d]\n"
        "%p %6s %-7s\n"
        "%p %6s %-7s [size=%zu | alloc=%d]\n",
        (void *)h, header_sz, header_label, size, alloc,
        (void *)p, payload_sz, payload_label,
        (void *)f, footer_sz, footer_label, (size_t)f->size, (int)f->alloc
    );
}

static void print_epilogue(header_t *h) {
    if (h == NULL) {
        return;
    }
    char header_sz[16];
    snprintf(header_sz, sizeof(header_sz), "%zuB", (size_t)WSIZE);
    printf(
        "%p %6s %-7s [size=%zu | alloc=%d]\n",
        (void *)h, header_sz, "epi_H", (size_t)h->size, (int)h->alloc
    );
}

static void print_padding(uint8_t *padding) {
    if (padding == NULL) {
        return;
    }
    char pad_sz[16];
    snprintf(pad_sz, sizeof(pad_sz), "%zuB", (size_t)WSIZE);
    printf(
        "%p %6s %-7s\n",
        (void *)padding, pad_sz, "padding"
    );
}

void print_heap_layout(void) {
    void *p = heap_listp;
    printf("------------------------------------------------\n");
    print_padding(heap_start);
    while (p != NULL) {
        header_t *h = get_header(p);
        if (h == NULL) {
            break;
        }
        if (h->size == 0) {
            print_epilogue(h);
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
