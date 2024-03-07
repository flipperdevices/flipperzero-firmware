#include "concurrent_runner.h"

#include <furi.h>

#include <core/timer.h>
#include <core/thread.h>

typedef struct {
    FuriThread* thread;
    ConcurrentRunnerCallback run_callback;
    ConcurrentRunnerCallback finished_callback;
    void* context;
} ConcurrentRunner;

static void concurrent_runner_timer_callback(void* context, uint32_t arg) {
    UNUSED(arg);

    ConcurrentRunner* instance = context;
    furi_thread_join(instance->thread);

    if(instance->finished_callback) {
        instance->finished_callback(instance->context);
    }

    furi_thread_free(instance->thread);
    free(instance);
}

static int32_t concurrent_runner_thread_callback(void* context) {
    ConcurrentRunner* instance = context;
    instance->run_callback(instance->context);
    return 0;
}

static void concurrent_runner_thread_state_callback(FuriThreadState state, void* context) {
    if(state == FuriThreadStateStopped) {
        furi_timer_pending_callback(concurrent_runner_timer_callback, context, 0);
    }
}

void concurrent_runner_start(
    uint32_t stack_size,
    ConcurrentRunnerCallback run_callback,
    ConcurrentRunnerCallback finished_callback,
    void* context) {
    furi_check(run_callback);

    ConcurrentRunner* instance = malloc(sizeof(ConcurrentRunner));

    instance->thread = furi_thread_alloc_ex(
        "ConcurrentRunner", stack_size, concurrent_runner_thread_callback, instance);
    furi_thread_set_state_callback(instance->thread, concurrent_runner_thread_state_callback);
    furi_thread_set_state_context(instance->thread, instance);

    instance->run_callback = run_callback;
    instance->finished_callback = finished_callback;
    instance->context = context;

    furi_thread_start(instance->thread);
}
