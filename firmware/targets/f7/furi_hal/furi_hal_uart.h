/**
 * @file furi_hal_uart.h
 * @version 1.0
 * @date 2021-11-19
 * 
 * UART HAL api interface
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * UART events
 */
typedef enum {
    UartIrqEventRxByte,
    UartIrqEventRxDMAEnd,
    UartIrqEventRxDMA,
} UartIrqEvent;

typedef void (*FuriHalUartRxByteCallback)(UartIrqEvent ev, uint8_t data, void* context);
typedef void (*FuriHalUartRxDMACallback)(UartIrqEvent ev, size_t data_len, void* context);

/**
 * UART channels
 */
typedef enum {
    FuriHalUartIdUSART1,
    FuriHalUartIdLPUART1,
    // Service value
    FuriHalUartIdMAX,
} FuriHalUartId;

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
 * Sets UART event callback
 * @param channel UART channel
 * @param callback callback pointer
 * @param context callback context
 */
void furi_hal_uart_set_irq_cb(FuriHalUartId ch, FuriHalUartRxByteCallback callback, void* context);

void furi_hal_uart_set_dma_callback(
    FuriHalUartId ch,
    FuriHalUartRxDMACallback callback,
    void* context);

size_t furi_hal_uart_rx_dma(FuriHalUartId ch, uint8_t* data, size_t len);

size_t furi_hal_uart_dma_available(FuriHalUartId ch);

#ifdef __cplusplus
}
#endif
