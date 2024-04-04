#include <furi.h>
#include <tlsf.h>
#include <tlsf_block_functions.h>
#include <FreeRTOS.h>
#include <task.h>

extern const void __heap_start__;
extern const void __heap_end__;

static tlsf_t tlsf = NULL;
static size_t heap_used = 0;
static size_t heap_max_used = 0;

static inline void memmgr_lock(void) {
    vTaskSuspendAll();
}

static inline void memmgr_unlock(void) {
    xTaskResumeAll();
}

static inline size_t memmgr_get_heap_size(void) {
    return (size_t)&__heap_end__ - (size_t)&__heap_start__;
}

void memmgr_heap_enable_thread_trace(FuriThreadId thread_id) {
    UNUSED(thread_id);
}

void memmgr_heap_disable_thread_trace(FuriThreadId thread_id) {
    UNUSED(thread_id);
}

size_t memmgr_heap_get_thread_memory(FuriThreadId thread_id) {
    UNUSED(thread_id);
    return 0;
}

static bool tlsf_walker_max_free(void* ptr, size_t size, int used, void* user) {
    UNUSED(ptr);

    size_t* max_free_block_size = (size_t*)user;
    if(!used && size > *max_free_block_size) {
        *max_free_block_size = size;
    }

    return true;
}

size_t memmgr_heap_get_max_free_block(void) {
    size_t max_free_block_size = 0;

    memmgr_lock();

    pool_t pool = tlsf_get_pool(tlsf);
    tlsf_walk_pool(pool, tlsf_walker_max_free, &max_free_block_size);

    memmgr_unlock();

    return max_free_block_size;
}

void memmgr_heap_printf_free_blocks(void) {
}

void* pvPortMalloc(size_t xSize) {
    // memory management in ISR is not allowed
    if(FURI_IS_IRQ_MODE()) {
        furi_crash("memmgt in ISR");
    }

    memmgr_lock();

    // initialize tlsf, if not initialized
    if(tlsf == NULL) {
        size_t pool_size = (size_t)&__heap_end__ - (size_t)&__heap_start__;
        tlsf = tlsf_create_with_pool((void*)&__heap_start__, pool_size, pool_size);
    }

    // allocate block
    void* data = tlsf_malloc(tlsf, xSize);
    if(data == NULL) {
        furi_crash("out of memory");
    }

    // update heap usage
    heap_used += tlsf_block_size(data);
    heap_used += tlsf_alloc_overhead();
    if(heap_used > heap_max_used) {
        heap_max_used = heap_used;
    }

    // clear block content
    memset(data, 0, xSize);

    memmgr_unlock();

    return data;
}

void vPortFree(void* pv) {
    // memory management in ISR is not allowed
    if(FURI_IS_IRQ_MODE()) {
        furi_crash("memmgt in ISR");
    }

    // ignore NULL pointer
    if(pv != NULL) {
        memmgr_lock();

        // clear block content
        size_t block_size = tlsf_block_size(pv);
        memset(pv, 0, block_size);

        // update heap usage
        heap_used -= block_size;
        heap_used -= tlsf_alloc_overhead();

        // free
        tlsf_free(tlsf, pv);

        memmgr_unlock();
    }
}

size_t xPortGetFreeHeapSize(void) {
    return memmgr_get_heap_size() - heap_used;
}

size_t xPortGetTotalHeapSize(void) {
    return memmgr_get_heap_size();
}

size_t xPortGetMinimumEverFreeHeapSize(void) {
    return memmgr_get_heap_size() - heap_max_used;
}