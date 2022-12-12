#pragma once

#include <furi_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PocsagTxRxWorkerCallbackHaveRead)(void* context);

typedef struct PocsagTxRxWorker PocsagTxRxWorker;

typedef enum {
    PocsagTxRxWorkerStatusIDLE,
    PocsagTxRxWorkerStatusTx,
    PocsagTxRxWorkerStatusRx,
} PocsagTxRxWorkerStatus;

/**
 * SubGhzTxRxWorker, add data to transfer
 * @param instance  Pointer to a SubGhzTxRxWorker instance
 * @param data      *data
 * @param size      data size
 * @return bool     true if ok
 */
bool pocsag_tx_rx_worker_write(PocsagTxRxWorker* instance, uint8_t* data, size_t size);

/**
 * SubGhzTxRxWorker, get available data
 * @param instance   Pointer to a SubGhzTxRxWorker instance
 * @return size_t    data size
 */
size_t pocsag_tx_rx_worker_available(PocsagTxRxWorker* instance);

/**
 * SubGhzTxRxWorker, read data
 * @param instance   Pointer to a SubGhzTxRxWorker instance
 * @param data       *data
 * @param size       max data size, which can be read
 * @return size_t    data size, how much is actually read
 */
size_t pocsag_tx_rx_worker_read(PocsagTxRxWorker* instance, uint8_t* data, size_t size);

/**
 * Сallback SubGhzTxRxWorker when there is data to read in an empty buffer
 * @param instance Pointer to a SubGhzTxRxWorker instance
 * @param callback SubGhzTxRxWorkerCallbackHaveRead callback
 * @param context
 */
void pocsag_tx_rx_worker_set_callback_have_read(
        PocsagTxRxWorker* instance,
        PocsagTxRxWorkerCallbackHaveRead callback,
        void* context);

/**
 * Allocate SubGhzTxRxWorker
 * @return SubGhzTxRxWorker* Pointer to a SubGhzTxRxWorker instance
 */
PocsagTxRxWorker* pocsag_tx_rx_worker_alloc();

/**
 * Free PocsagTxRxWorker
 * @param instance Pointer to a SubGhzTxRxWorker instance
 */
void pocsag_tx_rx_worker_free(PocsagTxRxWorker* instance);

/**
 * Start SubGhzTxRxWorker
 * @param instance Pointer to a SubGhzTxRxWorker instance
 * @return bool - true if ok
 */
bool pocsag_tx_rx_worker_start(PocsagTxRxWorker* instance, uint32_t frequency);

/**
 * Stop SubGhzTxRxWorker
 * @param instance Pointer to a SubGhzTxRxWorker instance
 */
void pocsag_tx_rx_worker_stop(PocsagTxRxWorker* instance);

/**
 * Check if worker is running
 * @param instance Pointer to a SubGhzTxRxWorker instance
 * @return bool - true if running
 */
bool pocsag_tx_rx_worker_is_running(PocsagTxRxWorker* instance);

#ifdef __cplusplus
}
#endif
