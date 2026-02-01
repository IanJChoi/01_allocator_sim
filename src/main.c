#include "allocator.h"

#include <stdio.h>

int main(void) {
    if (mm_init() != 0) {
        fprintf(stderr, "mm_init failed\n");
        return 1;
    }

    init_print();
    print_heap_layout_mm_init();
    return 0;
}
