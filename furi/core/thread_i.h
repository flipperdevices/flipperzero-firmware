#pragma once

#include "thread.h"
#include "string.h"

typedef struct {
    FuriThreadStdoutWriteCallback write_callback;
    FuriString* buffer;
} FuriThreadStdout;

struct FuriThread {
    FuriThreadState state;
    int32_t ret;

    FuriThreadCallback callback;
    void* context;

    FuriThreadStateCallback state_callback;
    void* state_context;

    char* name;
    char* appid;

    FuriThreadPriority priority;

    TaskHandle_t task_handle;
    size_t heap_size;

    FuriThreadStdout output;

    // Keep all non-alignable byte types in one place,
    // this ensures that the size of this structure is minimal
    bool is_service;
    bool heap_trace_enabled;

    configSTACK_DEPTH_TYPE stack_size;
};