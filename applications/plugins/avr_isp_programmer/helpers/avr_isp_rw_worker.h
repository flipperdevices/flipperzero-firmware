#pragma once

#include <furi_hal.h>

typedef struct AvrIspRWWorker AvrIspRWWorker;

typedef void (*AvrIspRWWorkerCallback)(void* context, const char* name);

/** Allocate AvrIspRWWorker
 * 
 * @param context AvrIsp* context
 * @return AvrIspRWWorker* 
 */
AvrIspRWWorker* avr_isp_rw_worker_alloc(void* context);

/** Free AvrIspRWWorker
 * 
 * @param instance AvrIspRWWorker instance
 */
void avr_isp_rw_worker_free(AvrIspRWWorker* instance);

/** Callback AvrIspRWWorker
 *
 * @param instance AvrIspRWWorker instance
 * @param callback AvrIspRWWorkerOverrunCallback callback
 * @param context
 */
void avr_isp_rw_worker_set_callback(
    AvrIspRWWorker* instance,
    AvrIspRWWorkerCallback callback,
    void* context);

/** Start AvrIspRWWorker
 * 
 * @param instance AvrIspRWWorker instance
 */
void avr_isp_rw_worker_start(AvrIspRWWorker* instance);

/** Stop AvrIspRWWorker
 * 
 * @param instance AvrIspRWWorker instance
 */
void avr_isp_rw_worker_stop(AvrIspRWWorker* instance);

/** Check if worker is running
 * @param instance AvrIspRWWorker instance
 * @return bool - true if running
 */
bool avr_isp_rw_worker_is_running(AvrIspRWWorker* instance);

/** Detect chip
 * 
 * @param instance AvrIspRWWorker instance
 */
void avr_isp_rw_worker_detect_chip(AvrIspRWWorker* instance);


bool avr_isp_rw_worker_rx (AvrIspRWWorker* instance);
