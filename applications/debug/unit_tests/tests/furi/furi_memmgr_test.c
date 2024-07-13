#include "../test.h" // IWYU pragma: keep
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

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

    // test that memory is aligned by 8 bytes
    if(((uintptr_t)ptr % 8) != 0) {
        error_message = "memory is not aligned by 8 bytes after malloc";
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
    // we know that allocator can use this memory for inner purposes
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

    // check that at least 75% of memory is zero-initialized
    if(zero_count < (allocation_size * 0.75)) {
        error_message = "seems that memory is not zero-initialized after free (malloc)";
    }

    FURI_CRITICAL_EXIT();

    if(error_message != NULL) {
        mu_fail(error_message);
    }
}

static void test_memmgr_realloc(const size_t allocation_size) {
    uint8_t* ptr = NULL;
    const char* error_message = NULL;

    FURI_CRITICAL_ENTER();

    ptr = realloc(ptr, allocation_size);

    // test that we can allocate memory
    if(ptr == NULL) {
        error_message = "realloc(NULL) failed";
    }

    // test that memory is aligned by 8 bytes
    if(((uintptr_t)ptr % 8) != 0) {
        error_message = "memory is not aligned by 8 bytes after realloc";
    }

    // test that memory is zero-initialized after allocation
    for(size_t i = 0; i < allocation_size; i++) {
        if(ptr[i] != 0) {
            error_message = "memory is not zero-initialized after realloc(NULL)";
            break;
        }
    }

    memset(ptr, 0x55, allocation_size);

    ptr = realloc(ptr, allocation_size * 2);

    // test that we can reallocate memory
    if(ptr == NULL) {
        error_message = "realloc failed";
    }

    // test that memory content is preserved
    for(size_t i = 0; i < allocation_size; i++) {
        if(ptr[i] != 0x55) {
            error_message = "memory is not reallocated after realloc";
            break;
        }
    }

    // test that memory is aligned by 8 bytes
    if(((uintptr_t)ptr % 8) != 0) {
        error_message = "memory is not aligned by 8 bytes after realloc";
    }

    // test that remaining memory is zero-initialized
    size_t non_zero_count = 0;
    for(size_t i = allocation_size; i < allocation_size * 2; i++) {
        if(ptr[i] != 0) {
            non_zero_count += 1;
        }
    }

    // check that at most of memory is zero-initialized
    // we know that allocator not always can restore content size from a pointer
    // so we check against small threshold
    if(non_zero_count > 4) {
        error_message = "seems that memory is not zero-initialized after realloc";
    }

    uint8_t* null_ptr = realloc(ptr, 0);

    // test that we can free memory
    if(null_ptr != NULL) {
        error_message = "realloc(0) failed";
    }

    // test that memory is zero-initialized after realloc(0)
    // we know that allocator can use this memory for inner purposes
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

    // check that at least 75% of memory is zero-initialized
    if(zero_count < (allocation_size * 0.75)) {
        error_message = "seems that memory is not zero-initialized after realloc(0)";
    }

    FURI_CRITICAL_EXIT();

    if(error_message != NULL) {
        mu_fail(error_message);
    }
}

static void test_memmgr_alloc_aligned(const size_t allocation_size, const size_t alignment) {
    uint8_t* ptr = NULL;
    const char* error_message = NULL;

    FURI_CRITICAL_ENTER();

    ptr = aligned_alloc(alignment, allocation_size);

    // test that we can allocate memory
    if(ptr == NULL) {
        error_message = "aligned_alloc failed";
    }

    // test that memory is aligned
    if(((uintptr_t)ptr % alignment) != 0) {
        error_message = "memory is not aligned after aligned_alloc";
    }

    // test that memory is zero-initialized after allocation
    for(size_t i = 0; i < allocation_size; i++) {
        if(ptr[i] != 0) {
            error_message = "memory is not zero-initialized after aligned_alloc";
            break;
        }
    }
    memset(ptr, 0x55, allocation_size);
    free(ptr);

    // test that memory is zero-initialized after free
    // we know that allocator can use this memory for inner purposes
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

    // check that at least 75% of memory is zero-initialized
    if(zero_count < (allocation_size * 0.75)) {
        error_message = "seems that memory is not zero-initialized after free (aligned_alloc)";
    }

    FURI_CRITICAL_EXIT();

    if(error_message != NULL) {
        mu_fail(error_message);
    }
}

void test_furi_memmgr_advanced(void) {
    const size_t sizes[] = {50, 100, 500, 1000, 5000, 10000};
    const size_t sizes_count = sizeof(sizes) / sizeof(sizes[0]);
    const size_t alignments[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024};
    const size_t alignments_count = sizeof(alignments) / sizeof(alignments[0]);

    // do test without memory fragmentation
    {
        for(size_t i = 0; i < sizes_count; i++) {
            test_memmgr_malloc(sizes[i]);
        }

        for(size_t i = 0; i < sizes_count; i++) {
            test_memmgr_realloc(sizes[i]);
        }

        for(size_t i = 0; i < sizes_count; i++) {
            for(size_t j = 0; j < alignments_count; j++) {
                test_memmgr_alloc_aligned(sizes[i], alignments[j]);
            }
        }
    }

    // do test with memory fragmentation
    {
        void* blocks[sizes_count];
        void* guards[sizes_count - 1];

        // setup guards
        for(size_t i = 0; i < sizes_count; i++) {
            blocks[i] = malloc(sizes[i]);
            if(i < sizes_count - 1) {
                guards[i] = malloc(sizes[i]);
            }
        }

        for(size_t i = 0; i < sizes_count; i++) {
            free(blocks[i]);
        }

        // do test
        for(size_t i = 0; i < sizes_count; i++) {
            test_memmgr_malloc(sizes[i]);
        }

        for(size_t i = 0; i < sizes_count; i++) {
            test_memmgr_realloc(sizes[i]);
        }

        for(size_t i = 0; i < sizes_count; i++) {
            for(size_t j = 0; j < alignments_count; j++) {
                test_memmgr_alloc_aligned(sizes[i], alignments[j]);
            }
        }

        // cleanup guards
        for(size_t i = 0; i < sizes_count - 1; i++) {
            free(guards[i]);
        }
    }
}

void test_furi_memmgr_aligned8(void) {
    const size_t repeat_count = 100;
    for(size_t i = 0; i < repeat_count; i++) {
        size_t alloc_size = 10 * (i + 1);

        uintptr_t ptr = (uintptr_t)malloc(alloc_size);
        mu_assert_int_eq(0, ptr % 8);
        ptr = (uintptr_t)realloc((void*)ptr, alloc_size * 2);
        mu_assert_int_eq(0, ptr % 8);
        ptr = (uintptr_t)realloc((void*)ptr, alloc_size * 3);
        mu_assert_int_eq(0, ptr % 8);
        free((void*)ptr);
    }

    for(size_t i = 0; i < repeat_count; i++) {
        size_t alloc_size = 4 * (i + 1);

        uintptr_t ptr = (uintptr_t)malloc(alloc_size);
        mu_assert_int_eq(0, ptr % 8);

        void* barrier_1 = malloc(alloc_size * (rand() % 10 + 1));
        ptr = (uintptr_t)realloc((void*)ptr, alloc_size * 2);
        mu_assert_int_eq(0, ptr % 8);

        barrier_1 = realloc(barrier_1, alloc_size * (rand() % 10 + 1));
        void* barrier_2 = malloc(alloc_size * (rand() % 10 + 1));
        ptr = (uintptr_t)realloc((void*)ptr, alloc_size * 3);
        mu_assert_int_eq(0, ptr % 8);

        free((void*)ptr);
        free(barrier_1);
        free(barrier_2);
    }
}