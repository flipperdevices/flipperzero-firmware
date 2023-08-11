#pragma once

#include <furi_hal.h>

typedef struct LinChackerWorker LinChackerWorker;

typedef void (*LinChackerWorkerCallback)(void* context, bool connect_usb);

/** Allocate LinChackerWorker
 * 
 * @param context AvrIsp* context
 * @return LinChackerWorker* 
 */
LinChackerWorker* lin_hacker_worker_alloc(void* context);

/** Free LinChackerWorker
 * 
 * @param instance LinChackerWorker instance
 */
void lin_hacker_worker_free(LinChackerWorker* instance);

/** Callback LinChackerWorker
 *
 * @param instance LinChackerWorker instance
 * @param callback LinChackerWorkerOverrunCallback callback
 * @param context
 */
void lin_hacker_worker_set_callback(
    LinChackerWorker* instance,
    LinChackerWorkerCallback callback,
    void* context);

/** Start LinChackerWorker
 * 
 * @param instance LinChackerWorker instance
 */
void lin_hacker_worker_start(LinChackerWorker* instance);

/** Stop LinChackerWorker
 * 
 * @param instance LinChackerWorker instance
 */
void lin_hacker_worker_stop(LinChackerWorker* instance);

/** Check if worker is running
 * @param instance LinChackerWorker instance
 * @return bool - true if running
 */
bool lin_hacker_worker_is_running(LinChackerWorker* instance);
