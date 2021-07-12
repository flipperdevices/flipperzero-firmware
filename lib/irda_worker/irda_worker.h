#pragma once

#include <irda.h>
#include <api-hal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IrdaWorker IrdaWorker;
typedef struct IrdaWorkerReceivedData IrdaWorkerReceivedSignal;

struct IrdaWorkerReceivedData {
    bool decoded;
    size_t timings_cnt;
    union {
        IrdaMessage message;
        uint32_t timings[500];
    } data;
};


typedef void (*IrdaWorkerReceivedSignalCallback)(void* context, IrdaWorkerReceivedSignal* received_signal);

/** Allocate IrdaWorker
 * 
 * @return IrdaWorker* 
 */
IrdaWorker* irda_worker_alloc();

/** Free IrdaWorker
 * 
 * @param instance IrdaWorker instance
 */
void irda_worker_free(IrdaWorker* instance);

/** Received data callback IrdaWorker
 * 
 * @param instance IrdaWorker instance
 * @param callback IrdaWorkerReceivedSignalCallback callback
 */
void irda_worker_set_received_data_callback(IrdaWorker* instance, IrdaWorkerReceivedSignalCallback callback);

/** Context callback IrdaWorker
 * 
 * @param instance IrdaWorker instance
 * @param context 
 */
void irda_worker_set_context(IrdaWorker* instance, void* context);

/** Start IrdaWorker
 * 
 * @param instance IrdaWorker instance
 */
void irda_worker_start(IrdaWorker* instance);

/** Stop IrdaWorker
 * 
 * @param instance IrdaWorker instance
 */
void irda_worker_stop(IrdaWorker* instance);

#ifdef __cplusplus
}
#endif

