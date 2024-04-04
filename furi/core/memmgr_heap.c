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

// Furi heap extension
#include <m-dict.h>

// Allocation tracking types
DICT_DEF2(MemmgrHeapAllocDict, uint32_t, uint32_t) //-V1048

DICT_DEF2( //-V1048
    MemmgrHeapThreadDict,
    uint32_t,
    M_DEFAULT_OPLIST,
    MemmgrHeapAllocDict_t,
    DICT_OPLIST(MemmgrHeapAllocDict))

// Thread allocation tracing storage
static MemmgrHeapThreadDict_t memmgr_heap_thread_dict = {0};
static volatile uint32_t memmgr_heap_thread_trace_depth = 0;

static inline void memmgr_lock(void) {
    vTaskSuspendAll();
}

static inline void memmgr_unlock(void) {
    xTaskResumeAll();
}

static inline size_t memmgr_get_heap_size(void) {
    return (size_t)&__heap_end__ - (size_t)&__heap_start__;
}

// Initialize tracing storage on start
void memmgr_heap_init(void) {
    MemmgrHeapThreadDict_init(memmgr_heap_thread_dict);
}

void memmgr_heap_enable_thread_trace(FuriThreadId thread_id) {
    memmgr_lock();
    {
        memmgr_heap_thread_trace_depth++;
        furi_check(MemmgrHeapThreadDict_get(memmgr_heap_thread_dict, (uint32_t)thread_id) == NULL);
        MemmgrHeapAllocDict_t alloc_dict;
        MemmgrHeapAllocDict_init(alloc_dict);
        MemmgrHeapThreadDict_set_at(memmgr_heap_thread_dict, (uint32_t)thread_id, alloc_dict);
        MemmgrHeapAllocDict_clear(alloc_dict);
        memmgr_heap_thread_trace_depth--;
    }
    memmgr_unlock();
}

void memmgr_heap_disable_thread_trace(FuriThreadId thread_id) {
    memmgr_lock();
    {
        memmgr_heap_thread_trace_depth++;
        furi_check(MemmgrHeapThreadDict_erase(memmgr_heap_thread_dict, (uint32_t)thread_id));
        memmgr_heap_thread_trace_depth--;
    }
    memmgr_unlock();
}

size_t memmgr_heap_get_thread_memory(FuriThreadId thread_id) {
    size_t leftovers = MEMMGR_HEAP_UNKNOWN;
    vTaskSuspendAll();
    {
        memmgr_heap_thread_trace_depth++;
        MemmgrHeapAllocDict_t* alloc_dict =
            MemmgrHeapThreadDict_get(memmgr_heap_thread_dict, (uint32_t)thread_id);
        if(alloc_dict) {
            leftovers = 0;
            MemmgrHeapAllocDict_it_t alloc_dict_it;
            for(MemmgrHeapAllocDict_it(alloc_dict_it, *alloc_dict);
                !MemmgrHeapAllocDict_end_p(alloc_dict_it);
                MemmgrHeapAllocDict_next(alloc_dict_it)) {
                MemmgrHeapAllocDict_itref_t* data = MemmgrHeapAllocDict_ref(alloc_dict_it);
                if(data->key != 0) {
                    block_header_t* block = block_from_ptr((uint8_t*)data->key);
                    if(!block_is_free(block)) {
                        // TODO: with tlsf we know the size of the block, so we don't need to store it on the dict
                        leftovers += data->value;
                    }
                }
            }
        }
        memmgr_heap_thread_trace_depth--;
    }
    (void)xTaskResumeAll();
    return leftovers;
}

static bool tlsf_walker_max_free(void* ptr, size_t size, int used, void* user) {
    UNUSED(ptr);

    bool free = !used;
    size_t* max_free_block_size = (size_t*)user;
    if(free && size > *max_free_block_size) {
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

typedef struct {
    BlockWalker walker;
    void* context;
} BlockWalkerWrapper;

static bool tlsf_walker_wrapper(void* ptr, size_t size, int used, void* user) {
    BlockWalkerWrapper* wrapper = (BlockWalkerWrapper*)user;
    return wrapper->walker(ptr, size, used, wrapper->context);
}

void memmgr_heap_walk_blocks(BlockWalker walker, void* context) {
    memmgr_lock();

    BlockWalkerWrapper wrapper = {walker, context};
    pool_t pool = tlsf_get_pool(tlsf);
    tlsf_walk_pool(pool, tlsf_walker_wrapper, &wrapper);

    memmgr_unlock();
}

static inline void memmgr_heap_trace_malloc(void* pointer, size_t size) {
    FuriThreadId thread_id = furi_thread_get_current_id();
    if(thread_id && memmgr_heap_thread_trace_depth == 0) {
        memmgr_heap_thread_trace_depth++;
        MemmgrHeapAllocDict_t* alloc_dict =
            MemmgrHeapThreadDict_get(memmgr_heap_thread_dict, (uint32_t)thread_id);
        if(alloc_dict) {
            MemmgrHeapAllocDict_set_at(*alloc_dict, (uint32_t)pointer, (uint32_t)size);
        }
        memmgr_heap_thread_trace_depth--;
    }
}

static inline void memmgr_heap_trace_free(void* pointer) {
    FuriThreadId thread_id = furi_thread_get_current_id();
    if(thread_id && memmgr_heap_thread_trace_depth == 0) {
        memmgr_heap_thread_trace_depth++;
        MemmgrHeapAllocDict_t* alloc_dict =
            MemmgrHeapThreadDict_get(memmgr_heap_thread_dict, (uint32_t)thread_id);
        if(alloc_dict) {
            // In some cases thread may want to release memory that was not allocated by it
            const bool res = MemmgrHeapAllocDict_erase(*alloc_dict, (uint32_t)pointer);
            UNUSED(res);
        }
        memmgr_heap_thread_trace_depth--;
    }
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
        memmgr_heap_init();
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

    // trace allocation
    memmgr_heap_trace_malloc(data, xSize);

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

        // trace free
        memmgr_heap_trace_free(pv);

        memmgr_unlock();
    }
}

size_t xPortGetFreeHeapSize(void) {
    return memmgr_get_heap_size() - heap_used - tlsf_size(tlsf);
}

size_t xPortGetTotalHeapSize(void) {
    return memmgr_get_heap_size();
}

size_t xPortGetMinimumEverFreeHeapSize(void) {
    return memmgr_get_heap_size() - heap_max_used - tlsf_size(tlsf);
}