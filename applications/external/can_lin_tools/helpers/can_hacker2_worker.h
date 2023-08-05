#pragma once

#include <furi_hal.h>

typedef struct CanChacker2Worker CanChacker2Worker;

typedef void (*CanChacker2WorkerCallback)(void* context, bool connect_usb);

/** Allocate CanChacker2Worker
 * 
 * @param context AvrIsp* context
 * @return CanChacker2Worker* 
 */
CanChacker2Worker* can_hacker2_worker_alloc(void* context);

/** Free CanChacker2Worker
 * 
 * @param instance CanChacker2Worker instance
 */
void can_hacker2_worker_free(CanChacker2Worker* instance);

/** Callback CanChacker2Worker
 *
 * @param instance CanChacker2Worker instance
 * @param callback CanChacker2WorkerOverrunCallback callback
 * @param context
 */
void can_hacker2_worker_set_callback(
    CanChacker2Worker* instance,
    CanChacker2WorkerCallback callback,
    void* context);

/** Start CanChacker2Worker
 * 
 * @param instance CanChacker2Worker instance
 */
void can_hacker2_worker_start(CanChacker2Worker* instance);

/** Stop CanChacker2Worker
 * 
 * @param instance CanChacker2Worker instance
 */
void can_hacker2_worker_stop(CanChacker2Worker* instance);

/** Check if worker is running
 * @param instance CanChacker2Worker instance
 * @return bool - true if running
 */
bool can_hacker2_worker_is_running(CanChacker2Worker* instance);
