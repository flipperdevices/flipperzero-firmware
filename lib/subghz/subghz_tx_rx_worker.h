#pragma once

#include <furi-hal.h>

typedef void (*SubGhzTxRxWorkerCallbackEnd)(void* context);

typedef struct SubGhzTxRxWorker SubGhzTxRxWorker;

typedef enum {
    SubGhzTxRxWorkerStatusIDLE,
    SubGhzTxRxWorkerStatusTx,
    SubGhzTxRxWorkerStatusRx,
} SubGhzTxRxWorkerStatus;

bool subghz_tx_rx_worker_add_tx(SubGhzTxRxWorker* instance, uint8_t* data, size_t size);
size_t subghz_tx_rx_worker_available_rx(SubGhzTxRxWorker* instance);
size_t subghz_tx_rx_worker_read_rx(SubGhzTxRxWorker* instance, uint8_t* data, size_t size);

/** End callback SubGhzTxRxWorker
 * 
 * @param instance SubGhzTxRxWorker instance
 * @param callback SubGhzTxRxWorkerCallbackEnd callback
 */
void subghz_tx_rx_worker_callback_end(
    SubGhzTxRxWorker* instance,
    SubGhzTxRxWorkerCallbackEnd callback_end,
    void* context_end);

/** Allocate SubGhzTxRxWorker
 * 
 * @return SubGhzTxRxWorker* 
 */
SubGhzTxRxWorker* subghz_tx_rx_worker_alloc();

/** Free SubGhzTxRxWorker
 * 
 * @param instance SubGhzTxRxWorker instance
 */
void subghz_tx_rx_worker_free(SubGhzTxRxWorker* instance);


/** Start SubGhzTxRxWorker
 * 
 * @param instance SubGhzTxRxWorker instance
 * @return bool - true if ok
 */
bool subghz_tx_rx_worker_start(SubGhzTxRxWorker* instance);

/** Stop SubGhzTxRxWorker
 * 
 * @param instance SubGhzTxRxWorker instance
 */
void subghz_tx_rx_worker_stop(SubGhzTxRxWorker* instance);

/** Check if worker is running
 * 
 * @param instance SubGhzTxRxWorker instance
 * @return bool - true if running
 */
bool subghz_tx_rx_worker_is_running(SubGhzTxRxWorker* instance);
