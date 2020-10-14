#include "minunit.h"
#include <stdlib.h>

// do not include memmgr.h here
// we also test that we are linking against stdlib
extern size_t memmgr_get_free_heap(void);
extern size_t memmgr_get_minimum_free_heap(void);

void test_furi_memmgr() {
    size_t heap_size = 0;
    size_t heap_size_old = 0;
    const int alloc_size = 128;

    // do not include furi memmgr.h case
#ifdef FURI_MEMMGR_GUARD
    mu_fail("do not link against furi memmgr.h");
#endif

    // allocate memory case
    heap_size_old = memmgr_get_free_heap();
    void* ptr = malloc(alloc_size);
    heap_size = memmgr_get_free_heap();
    mu_assert_int_eq(heap_size_old - alloc_size, heap_size);

    // free memory case
    free(ptr);
    heap_size = memmgr_get_free_heap();
    mu_assert_int_eq(heap_size_old, heap_size);

    // reallocate memory case


    // allocate and zero-initialize array (calloc)
}