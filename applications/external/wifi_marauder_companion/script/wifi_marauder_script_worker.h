#pragma once

#include "wifi_marauder_script.h"

typedef struct WifiMarauderScriptWorker {
    WifiMarauderScript *script;
    FuriThread *worker_thread;
    void (*callback)(WifiMarauderScriptStage*, void*);
    void *context;
    bool is_running;
} WifiMarauderScriptWorker;

/**
 * @brief Allocates a new instance of WifiMarauderScriptWorker.
 *
 * @return A pointer to the allocated instance or NULL if allocation fails.
 */
WifiMarauderScriptWorker* wifi_marauder_script_worker_alloc();

/**
 * @brief Starts the execution of the worker and sets the callback function to be called after each stage is executed.
 *
 * @param instance A pointer to the instance of WifiMarauderScriptWorker to start.
 * @param script Script to be executed
 * @param callback A pointer to the callback function to be called after each stage is executed.
 * The callback function should receive a parameter of type WifiMarauderScriptStage* and return nothing.
 * @return True if the worker was successfully started, false otherwise.
 */
bool wifi_marauder_script_worker_start(WifiMarauderScriptWorker* instance, WifiMarauderScript* script, void (*callback)(WifiMarauderScriptStage*, void*), void* context);

/**
 * @brief Frees the memory used by the instance of WifiMarauderScriptWorker.
 *
 * @param script A pointer to the instance of WifiMarauderScriptWorker to free.
 */
void wifi_marauder_script_worker_free(WifiMarauderScriptWorker* script);
