/**
 * @file furi_hal_serial.h
 * 
 * Serial HAL API
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "furi_hal_serial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize Serial
 *
 * Configures GPIO, configures and enables transceiver.
 *
 * @param      handle  Serial handle
 * @param      baud    baud rate
 */
void furi_hal_serial_init(FuriHalSerialHandle* handle, uint32_t baud);

/** De-initialize Serial
 *
 * Configures GPIO to analog, clears callback and callback context, disables
 * hardware
 *
 * @param      handle  Serial handle
 */
void furi_hal_serial_deinit(FuriHalSerialHandle* handle);

/** Suspend operation
 *
 * Suspend hardware, settings and callbacks are preserved
 *
 * @param      handle  Serial handle
 */
void furi_hal_serial_suspend(FuriHalSerialHandle* handle);

/** Resume operation
 *
 * Resumes hardware from suspended state
 *
 * @param      handle  Serial handle
 */
void furi_hal_serial_resume(FuriHalSerialHandle* handle);

/** Changes baud rate
 *
 * @param      handle  Serial handle
 * @param      baud    baud rate
 */
void furi_hal_serial_set_br(FuriHalSerialHandle* handle, uint32_t baud);

/** Transmits data in semi-blocking mode
 *
 * Fills transmission pipe with data, returns as soon as all bytes from buffer
 * are in the pipe.
 *
 * Real transmission will be completed later. Use
 * `furi_hal_serial_tx_wait_complete` to wait for completion if you need it.
 *
 * @param      handle       Serial handle
 * @param      buffer       data
 * @param      buffer_size  data size (in bytes)
 */
void furi_hal_serial_tx(FuriHalSerialHandle* handle, const uint8_t* buffer, size_t buffer_size);

/** Wait transmission complete.
 *
 * Ensures that all data has been sent.
 *
 * @param      handle  Serial handle
 */
void furi_hal_serial_tx_wait_complete(FuriHalSerialHandle* handle);

/** Receive callback
 *
 * @warning    Callback will be called in interrupt context, ensure thread
 *             safety on your side.
 * @param      handle   Serial handle
 * @param      data     Received data
 * @param      context  Callback context provided earlier
 */
typedef void (*FuriHalSerialRxCallback)(FuriHalSerialHandle* handle, uint8_t data, void* context);

/** Sets Serial Receive callback
 *
 * @warning Callback will be called in interrupt context, ensure thread safety on your side
 *
 * @param      handle    Serial handle
 * @param      callback  callback pointer
 * @param      context   callback context
 */
void furi_hal_serial_set_rx_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxCallback callback,
    void* context);

/* DMA based Serial API */

#define FURI_HAL_SERIAL_DMA_BUFFER_SIZE (256u)

/** Serial DMA events */
typedef enum {
    FuriHalSerialDmaRxEventRx,
    FuriHalSerialDmaRxEventEnd,
} FuriHalSerialDmaRxEvent;

/** Receive DMA callback
 *
 * @warning    DMA Callback will be called in interrupt context, ensure thread
 *             safety on your side.
 * @param      handle       Serial handle
 * @param      ev           FuriHalSerialDmaRxEvent
 * @param      data_len     Received data
 * @param      context      Callback context provided earlier
 */
typedef void (*FuriHalSerialDmaRxCallback)(
    FuriHalSerialHandle* handle,
    FuriHalSerialDmaRxEvent ev,
    size_t data_len,
    void* context);

/**
 * Sets Serial event callback receive DMA
 * @param   handle      Serial handle
 * @param   callback    callback pointer
 * @param   context     callback context
 */
void furi_hal_serial_dma_start(
    FuriHalSerialHandle* handle,
    FuriHalSerialDmaRxCallback callback,
    void* context);

/** Get data Serial receive DMA
 * @warning   This function is called only from the callback FuriHalSerialDmaRxCallback
 *  
 * @param   handle      Serial handle
 * @param   data        pointer to data buffer
 * @param   len         get data size (in bytes)
 *
 * @return     size actual data receive (in bytes)
 */
size_t furi_hal_serial_dma_rx(FuriHalSerialHandle* handle, uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif
