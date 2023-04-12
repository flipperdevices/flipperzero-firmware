#include "../wifi_marauder_app_i.h"
#include "wifi_marauder_script_worker.h"

WifiMarauderScriptWorker* wifi_marauder_script_worker_alloc() {
    WifiMarauderScriptWorker* worker = malloc(sizeof(WifiMarauderScriptWorker));
    if (worker == NULL) {
        return NULL;
    }
    worker->callback = NULL;
    worker->worker_thread = NULL;
    worker->is_running = false;
    return worker;
}

int32_t _wifi_marauder_script_worker_task(void* worker) {
    WifiMarauderScriptWorker* script_worker = worker;
    WifiMarauderScript *script = script_worker->script;
    if (script == NULL) {
        return 0;    
    }

    for (int i = 0; i < script->repeat; i++) {
        WifiMarauderScriptStage *current_stage = script->first_stage;
        while (current_stage != NULL && script_worker->is_running) {
            script_worker->callback(current_stage, script_worker->context);
            current_stage = current_stage->next_stage;
        }
        if (!script_worker->is_running) {
            break;
        }
    }
    return 0;
}

bool wifi_marauder_script_worker_start(WifiMarauderScriptWorker* instance, WifiMarauderScript* script, void (*callback)(WifiMarauderScriptStage*, void*), void* context) {
    if (!instance || !script) {
        return false;
    }
    instance->callback = callback;
    instance->script = script;
    instance->context = context;
    instance->is_running = true;
    instance->worker_thread = furi_thread_alloc_ex("WifiMarauderScriptWorker", 1024, _wifi_marauder_script_worker_task, instance);
    if (!instance->worker_thread) {
        return false;
    }
    furi_thread_start(instance->worker_thread);
    return true;
}

void wifi_marauder_script_worker_free(WifiMarauderScriptWorker *worker) {
    if (worker != NULL) {
        if (worker->worker_thread != NULL) {
            worker->is_running = false;
            furi_thread_join(worker->worker_thread);
            furi_thread_free(worker->worker_thread);
        }
        free(worker);
    }
}