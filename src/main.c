#include "allocator.h"

#include <stdio.h>

enum { MAX_PTRS = 1024 };

int main(void) {
    if (mm_init() != 0) {
        fprintf(stderr, "mm_init failed\n");
        return 1;
    }

    init_print();
    print_heap_layout();

    void *allocs[MAX_PTRS];
    size_t alloc_count = 0;

    for (;;) {
        char action[16];
        printf("\nChoose action (malloc/free/quit): ");
        if (scanf("%15s", action) != 1) {
            fprintf(stderr, "input error\n");
            return 1;
        }

        if (action[0] == 'q') {
            break;
        }

        if (action[0] == 'm') {
            size_t bytes = 0;
            printf("How many bytes to malloc? ");
            if (scanf("%zu", &bytes) != 1) {
                fprintf(stderr, "invalid size\n");
                return 1;
            }
            void *p = mm_malloc(bytes);
            if (p == NULL) {
                printf("malloc failed\n");
            } else if (alloc_count < MAX_PTRS) {
                allocs[alloc_count++] = p;
                printf("allocated index %zu\n", alloc_count - 1);
            } else {
                printf("allocation list full\n");
            }
            print_heap_layout();
            continue;
        }

        if (action[0] == 'f') {
            if (alloc_count == 0) {
                printf("no allocations to free\n");
                continue;
            }
            printf("allocated blocks:\n");
            for (size_t i = 0; i < alloc_count; i++) {
                printf("  [%zu] %p\n", i, allocs[i]);
            }
            size_t idx = 0;
            printf("Choose index to free: ");
            if (scanf("%zu", &idx) != 1 || idx >= alloc_count) {
                fprintf(stderr, "invalid index\n");
                return 1;
            }
            mm_free(allocs[idx]);
            for (size_t i = idx + 1; i < alloc_count; i++) {
                allocs[i - 1] = allocs[i];
            }
            alloc_count--;
            print_heap_layout();
            continue;
        }

        printf("unknown action\n");
    }

    return 0;
}
