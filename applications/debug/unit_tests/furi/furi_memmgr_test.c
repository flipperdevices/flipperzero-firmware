#include "../minunit.h"
#include <furi.h>

void test_furi_memmgr(void) {
    void* ptr;

    // allocate memory case
    ptr = malloc(100);
    mu_check(ptr != NULL);
    // test that memory is zero-initialized after allocation
    for(int i = 0; i < 100; i++) {
        mu_assert_int_eq(0, ((uint8_t*)ptr)[i]);
    }
    free(ptr);

    // reallocate memory case
    ptr = malloc(100);
    memset(ptr, 66, 100);
    ptr = realloc(ptr, 200);
    mu_check(ptr != NULL);

    // test that memory is really reallocated
    for(int i = 0; i < 100; i++) {
        mu_assert_int_eq(66, ((uint8_t*)ptr)[i]);
    }

    free(ptr);

    // allocate and zero-initialize array (calloc)
    ptr = calloc(100, 2);
    mu_check(ptr != NULL);
    for(int i = 0; i < 100 * 2; i++) {
        mu_assert_int_eq(0, ((uint8_t*)ptr)[i]);
    }
    free(ptr);
}

static void test_memmgr_malloc(const size_t allocation_size) {
    uint8_t* ptr = NULL;
    const char* error_message = NULL;

    FURI_CRITICAL_ENTER();

    ptr = malloc(allocation_size);

    // test that we can allocate memory
    if(ptr == NULL) {
        error_message = "malloc failed";
    }

    // test that memory is zero-initialized after allocation
    for(size_t i = 0; i < allocation_size; i++) {
        if(ptr[i] != 0) {
            error_message = "memory is not zero-initialized after malloc";
            break;
        }
    }
    memset(ptr, 0x55, allocation_size);
    free(ptr);

    // test that memory is zero-initialized after free
    // allocator can use this memory for inner purposes
    // so we check that memory at least partially zero-initialized

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"

    size_t zero_count = 0;
    for(size_t i = 0; i < allocation_size; i++) {
        if(ptr[i] == 0) {
            zero_count++;
        }
    }

#pragma GCC diagnostic pop

    // chech that at least 75% of memory is zero-initialized
    if(zero_count < (allocation_size * 0.75)) {
        error_message = "seems memory is not zero-initialized after free";
    }

    FURI_CRITICAL_EXIT();

    if(error_message != NULL) {
        mu_fail(error_message);
    }
}

void test_furi_memmgr_advanced(void) {
    test_memmgr_malloc(50);
    test_memmgr_malloc(100);
    test_memmgr_malloc(500);
    test_memmgr_malloc(1000);
    test_memmgr_malloc(5000);
    test_memmgr_malloc(10000);
}