#pragma once

#include <furi_hal.h>


typedef struct CarBusAnalyzerWorker CarBusAnalyzerWorker;

typedef enum {
    CarBusAnalyzerWorkerStatusModuleDisconnect,
    CarBusAnalyzerWorkerStatusProcDisconnected,
    CarBusAnalyzerWorkerStatusProcConnected,
} CarBusAnalyzerWorkerStatus;

typedef void (*CarBusAnalyzerWorkerCallback)(void* context, CarBusAnalyzerWorkerStatus status);

/** Allocate CarBusAnalyzerWorker
 * 
 * @param context AvrIsp* context
 * @return CarBusAnalyzerWorker* 
 */
CarBusAnalyzerWorker* carbus_analyzer_worker_alloc(void* context);

/** Free CarBusAnalyzerWorker
 * 
 * @param instance CarBusAnalyzerWorker instance
 */
void carbus_analyzer_worker_free(CarBusAnalyzerWorker* instance);

/** Callback CarBusAnalyzerWorker
 *
 * @param instance CarBusAnalyzerWorker instance
 * @param callback CarBusAnalyzerWorkerOverrunCallback callback
 * @param context
 */
void carbus_analyzer_worker_set_callback(
    CarBusAnalyzerWorker* instance,
    CarBusAnalyzerWorkerCallback callback,
    void* context);

/** Start CarBusAnalyzerWorker
 * 
 * @param instance CarBusAnalyzerWorker instance
 */
void carbus_analyzer_worker_start(CarBusAnalyzerWorker* instance);

/** Stop CarBusAnalyzerWorker
 * 
 * @param instance CarBusAnalyzerWorker instance
 */
void carbus_analyzer_worker_stop(CarBusAnalyzerWorker* instance);

/** Check if worker is running
 * @param instance CarBusAnalyzerWorker instance
 * @return bool - true if running
 */
bool carbus_analyzer_worker_is_running(CarBusAnalyzerWorker* instance);
