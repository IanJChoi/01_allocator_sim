#include "allocator_internal.h"

void write_header(void *p, size_t size, int alloc) {
    header_t *h = (header_t *)p;
    h->size = size;
    h->alloc = alloc ? 1 : 0;
}

void write_footer(void *p, size_t size, int alloc) {
    footer_t *f = (footer_t *)p;
    f->size = size;
    f->alloc = alloc ? 1 : 0;
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
