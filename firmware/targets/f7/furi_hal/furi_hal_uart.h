#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** UART IRQ events */
typedef enum {
    UartIrqEventRXNE,
} UartIrqEvent;

/**
 * UART channels
 */
typedef enum {
    FuriHalUartIdUSART1,
    FuriHalUartIdLPUART1,
    // Service value
    FuriHalUartIdMAX,
} FuriHalUartId;

typedef void (*FuriHalUartRxByteCallback)(UartIrqEvent ev, uint8_t data, void* context);

/**
 * Uart wait tx complete
 * @param ch UART channel
 */
void furi_hal_uart_wait_tx_complete(FuriHalUartId ch);

/**
 * Init UART
 * Configures GPIO to UART function, сonfigures UART hardware, enables UART hardware
 * @param channel UART channel
 * @param baud baudrate
 */
void furi_hal_uart_init(FuriHalUartId channel, uint32_t baud);

/**
 * Deinit UART
 * Configures GPIO to analog, clears callback and callback context, disables UART hardware
 * @param channel UART channel
 */
void furi_hal_uart_deinit(FuriHalUartId channel);

/**
 * Suspend UART operation
 * Disables UART hardware, settings and callbacks are preserved
 * @param channel UART channel
 */
void furi_hal_uart_suspend(FuriHalUartId channel);

/**
 * Resume UART operation
 * Resumes UART hardware from suspended state
 * @param channel UART channel
 */
void furi_hal_uart_resume(FuriHalUartId channel);

/**
 * Changes UART baudrate
 * @param channel UART channel
 * @param baud baudrate
 */
void furi_hal_uart_set_br(FuriHalUartId channel, uint32_t baud);

/**
 * Transmits data
 * @param channel UART channel
 * @param buffer data
 * @param buffer_size data size (in bytes)
 */
void furi_hal_uart_tx(FuriHalUartId channel, uint8_t* buffer, size_t buffer_size);

/**
 * Sets UART event callback receive 1 byte
 * @param channel UART channel
 * @param callback callback pointer
 * @param context callback context
 */
void furi_hal_uart_set_irq_cb(FuriHalUartId ch, FuriHalUartRxByteCallback callback, void* context);

/* DMA based UART API */

/** UART DMA events */
typedef enum {
    FuriHalUartDmaRxEventRx,
    FuriHalUartDmaRxEventEnd,
} FuriHalUartDmaRxEvent;

/** UART IRQ events */
typedef void (*FuriHalUartDmaRxCallback)(
    FuriHalUartDmaRxEvent ev,
    FuriHalUartId id_uart,
    size_t data_len,
    void* context);

/**
 * Sets UART event callback receive DMA
 * @param channel UART channel
 * @param callback callback pointer
 * @param context callback context
 */
void furi_hal_uart_dma_start(FuriHalUartId ch, FuriHalUartDmaRxCallback callback, void* context);

/** Get data UART receive DMA
 *
 * @param[in]  ch    UART instance
 * @param[in]  data  pointer to data buffer
 * @param      len   get data size (in bytes)
 * @param      channel  UART channel
 *
 * @return     size actual data receive (in bytes)
 */
size_t furi_hal_uart_dma_rx(FuriHalUartId ch, uint8_t* data, size_t len);

/** Get received data size in DMA buffer
 * 
 * @param channel UART channel
 * @return received data size (in bytes)
 */
size_t furi_hal_uart_dma_bytes_available(FuriHalUartId ch);

#ifdef __cplusplus
}
#endif
