#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <furi_hal_uart.h>

#ifdef HEAP_PRINT_DEBUG
#define FURI_HAL_CONSOLE_BAUDRATE 1843200
#else
#define FURI_HAL_CONSOLE_BAUDRATE 230400
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FuriHalConsoleTxCallback)(const uint8_t* buffer, size_t size, void* context);

void furi_hal_console_init(FuriHalUartId channel, uint32_t baudrate);

void furi_hal_console_deinit();

void furi_hal_console_enable();

void furi_hal_console_disable();

void furi_hal_console_set_tx_callback(FuriHalConsoleTxCallback callback, void* context);

void furi_hal_console_tx(const uint8_t* buffer, size_t buffer_size);

void furi_hal_console_tx_with_new_line(const uint8_t* buffer, size_t buffer_size);

/**
 * Printf-like plain uart interface
 * @warning Will not work in ISR context
 * @param format 
 * @param ... 
 */
void furi_hal_console_printf(const char format[], ...) _ATTRIBUTE((__format__(__printf__, 1, 2)));

void furi_hal_console_puts(const char* data);

#ifdef __cplusplus
}
#endif
