#include "memmgr.h"
#include <string.h>

extern void* pvPortMalloc(size_t xSize);
extern void vPortFree(void* pv);
extern size_t xPortGetFreeHeapSize(void);
extern size_t xPortGetMinimumEverFreeHeapSize(void);

void* malloc(size_t size) {
    return pvPortMalloc(size);
}

void free(void* ptr) {
    vPortFree(ptr);
}

void* realloc(void* ptr, size_t size) {
    vPortFree(ptr);
    return pvPortMalloc(size);
}

void* calloc(size_t count, size_t size) {
    void* ptr = pvPortMalloc(count * size);
    if(ptr) {
        // zero the memory
        memset(ptr, 0, count * size);
    }
    return ptr;
}

size_t memmgr_get_free_heap(void) {
    return xPortGetFreeHeapSize();
}

size_t memmgr_get_minimum_free_heap(void) {
    return xPortGetMinimumEverFreeHeapSize();
}