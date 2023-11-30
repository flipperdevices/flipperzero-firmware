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

/** Wait until transmission is completed
 *
 * Ensures that all data has been sent.
 *
 * @param      handle  Serial handle
 */
void furi_hal_serial_tx_wait_complete(FuriHalSerialHandle* handle);

/** Serial events */
typedef enum {
    FuriHalSerialRxEventOffError = (1 << 0), /**< Bus OFF detected error */
    FuriHalSerialRxEventRx = (1 << 1), /**< Data available and must be picked up */
    FuriHalSerialRxEventEnd =
        (1 << 2), /**< Bus idle detected, data may be available (check data_len) */
    FuriHalSerialRxEventFrameError = (1 << 3), /**< Bus FE detected, frame error */
    FuriHalSerialRxEventNoiseLineError = (1 << 4), /**< Bus NE detected, noise line */
    FuriHalSerialRxEventOverrunError = (1 << 5), /**< Bus ORE detected, overrun error */
} FuriHalSerialRxEvent;

/** Receive callback
 *
 * @warning    Callback will be called in interrupt context, ensure thread
 *             safety on your side.
 * @param      handle   Serial handle
 * @param      event    FuriHalSerialRxEvent
 * @param      context  Callback context provided earlier
 */
typedef void (*FuriHalSerialRxCallback)(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context);

/** Start and sets Serial Receive callback
 *
 * @warning Callback will be called in interrupt context, ensure thread safety on your side
 *
 * @param      handle    Serial handle
 * @param      callback  callback pointer
 * @param      context   callback context
 * @param      event     FuriHalSerialRxEvent
 */
void furi_hal_serial_rx_start(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxCallback callback,
    void* context,
    FuriHalSerialRxEvent event);

/** Stop Serial Receive
 *
 * @param      handle    Serial handle
 */
void furi_hal_serial_rx_stop(FuriHalSerialHandle* handle);

/** Get data Serial receive
 *
 * @warning    This function must be called only from the callback
 *             FuriHalSerialRxCallback
 *
 * @param      handle  Serial handle
 *
 * @return     data
 */
uint8_t furi_hal_serial_rx(FuriHalSerialHandle* handle);

/* DMA based Serial API */

#define FURI_HAL_SERIAL_DMA_BUFFER_SIZE (256u)

/** Receive DMA callback
 *
 * @warning    DMA Callback will be called in interrupt context, ensure thread
 *             safety on your side.
 *
 * @param      handle    Serial handle
 * @param      event     FuriHalSerialDmaRxEvent
 * @param      data_len  Received data
 * @param      context   Callback context provided earlier
 */
typedef void (*FuriHalSerialDmaRxCallback)(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    size_t data_len,
    void* context);

/** Start and sets Serial event callback receive DMA 
 *
 * @param      handle    Serial handle
 * @param      callback  callback pointer
 * @param      context   callback context
 */
void furi_hal_serial_dma_rx_start(
    FuriHalSerialHandle* handle,
    FuriHalSerialDmaRxCallback callback,
    void* context,
    FuriHalSerialRxEvent event);

/** Stop Serial receive DMA
 *
 * @param      handle    Serial handle
 */
void furi_hal_serial_dma_rx_stop(FuriHalSerialHandle* handle);

/** Get data Serial receive DMA
 *
 * @warning    This function must be called only from the callback
 *             FuriHalSerialDmaRxCallback
 *
 * @param      handle  Serial handle
 * @param      data    pointer to data buffer
 * @param      len     get data size (in bytes)
 *
 * @return     size actual data receive (in bytes)
 */
size_t furi_hal_serial_dma_rx(FuriHalSerialHandle* handle, uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif
