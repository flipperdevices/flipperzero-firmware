#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FuriHalConsoleTxCallback)(const uint8_t* buffer, size_t size, void* context);

void furi_hal_console_init();

/// Enable the console output through the serial TX pin
void furi_hal_console_enable_serial();

/// Disable the console output through the serial TX pin
void furi_hal_console_disable_serial();

/**
 * Add an alternative output to the console
 * To remove it, call this function again with NULL pointers
 *
 * @note Only one callback can be active at a time
 */
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
