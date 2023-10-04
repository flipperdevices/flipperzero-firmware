#pragma once

typedef struct MjsThread MjsThread;

typedef enum {
    MjsThreadEventDone,
    MjsThreadEventError,
    MjsThreadEventPrint,
    MjsThreadEventErrorTrace,
    // TODO: input wait, ....
} MjsThreadEvent;

typedef void (*MjsThreadCallback)(MjsThreadEvent event, const char* msg, void* context);

MjsThread* mjs_thread_run(const char* script_path, MjsThreadCallback callback, void* context);

void mjs_thread_stop(MjsThread* worker);

void mjs_thread_free(MjsThread* worker);
