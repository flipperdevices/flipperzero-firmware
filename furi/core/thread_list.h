#pragma once

#include "base.h"
#include "common_defines.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FuriThread* thread;

    const char* app_id;
    const char* name;

    FuriThreadPriority priority;
    uint32_t stack_address;
    size_t heap;
    uint32_t stack_size;
    uint32_t stack_min_free;

    const char* state;

    float cpu;

    uint32_t counter_previous;
    uint32_t counter_current;

    uint32_t tick;
} FuriThreadListItem;

typedef struct FuriThreadList FuriThreadList;

FuriThreadList* furi_thread_list_alloc(void);

void furi_thread_list_free(FuriThreadList* instance);

size_t furi_thread_list_size(FuriThreadList* instance);

FuriThreadListItem* furi_thread_list_get_at(FuriThreadList* instance, size_t position);

FuriThreadListItem* furi_thread_list_get_or_insert(FuriThreadList* instance, FuriThread* thread);

void furi_thread_list_cleanup(FuriThreadList* instance, uint32_t runtime, uint32_t tick);

#ifdef __cplusplus
}
#endif
