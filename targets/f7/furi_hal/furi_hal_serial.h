/**
 * @file furi_hal_serial.h
 * @version 1.0
 * @date 2021-11-19
 * 
 * UART HAL api interface
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "furi_hal_serial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * UART events
 */
typedef enum {
    UartIrqEventRXNE,
} UartIrqEvent;

/**
 * Init UART
 * Configures GPIO to UART function, сonfigures UART hardware, enables UART hardware
 * @param channel UART channel
 * @param baud baudrate
 */
void furi_hal_serial_init(FuriHalSerialHandle* handle, uint32_t baud);

/**
 * Deinit UART
 * Configures GPIO to analog, clears callback and callback context, disables UART hardware
 * @param channel UART channel
 */
void furi_hal_serial_deinit(FuriHalSerialHandle* handle);

/**
 * Suspend UART operation
 * Disables UART hardware, settings and callbacks are preserved
 * @param channel UART channel
 */
void furi_hal_serial_suspend(FuriHalSerialHandle* handle);

/**
 * Resume UART operation
 * Resumes UART hardware from suspended state
 * @param channel UART channel
 */
void furi_hal_serial_resume(FuriHalSerialHandle* handle);

/**
 * Changes UART baudrate
 * @param channel UART channel
 * @param baud baudrate
 */
void furi_hal_serial_set_br(FuriHalSerialHandle* handle, uint32_t baud);

/**
 * Transmits data
 * @param channel UART channel
 * @param buffer data
 * @param buffer_size data size (in bytes)
 */
void furi_hal_serial_tx(FuriHalSerialHandle* handle, const uint8_t* buffer, size_t buffer_size);

/** Flush transmission pipe. Ensures that all data has been sent.
 *
 * @param      handle  UART channel
 */
void furi_hal_serial_tx_wait_complete(FuriHalSerialHandle* handle);

/**
 * Sets UART event callback
 * @param channel UART channel
 * @param callback callback pointer
 * @param context callback context
 */
void furi_hal_serial_set_irq_cb(
    FuriHalSerialHandle* handle,
    void (*callback)(UartIrqEvent event, uint8_t data, void* context),
    void* context);

#ifdef __cplusplus
}
#endif
