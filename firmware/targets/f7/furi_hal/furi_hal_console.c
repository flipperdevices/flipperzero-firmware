#include <furi_hal_console.h>
#include <furi_hal_uart.h>

#include <stdbool.h>
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_usart.h>

#include <utilities_conf.h>

#include <furi.h>

#define TAG "FuriHalConsole"

typedef struct {
    bool alive;
    bool initialized;
    FuriHalUartId channel;
    uint32_t baudrate;
    FuriHalConsoleTxCallback tx_callback;
    void* tx_callback_context;
} FuriHalConsole;

FuriHalConsole furi_hal_console = {
    .initialized = false,
    .alive = false,
    .channel = FuriHalUartIdUSART1,
    .tx_callback = NULL,
    .baudrate = CONSOLE_BAUDRATE,
    .tx_callback_context = NULL,
};

void furi_hal_console_init(FuriHalUartId channel, uint32_t baudrate) {
    furi_check(!furi_hal_console.initialized);

    furi_hal_console.channel = channel;
    furi_hal_console.baudrate = baudrate;
    furi_hal_uart_init(channel, baudrate);
    furi_hal_console.alive = true;
    furi_hal_console.initialized = true;
}

void furi_hal_console_deinit() {
    furi_check(furi_hal_console.initialized);
    furi_hal_uart_deinit(furi_hal_console.channel);
    furi_hal_console.channel = FuriHalUartIdUSART1;
    furi_hal_console.baudrate = CONSOLE_BAUDRATE;
    furi_hal_console.alive = false;
    furi_hal_console.initialized = false;
}

void furi_hal_console_enable() {
    furi_check(furi_hal_console.initialized);
    furi_hal_uart_set_irq_cb(furi_hal_console.channel, NULL, NULL);
    furi_hal_uart_wait_tx_complete(furi_hal_console.channel);

    furi_hal_uart_set_br(furi_hal_console.channel, furi_hal_console.baudrate);
    furi_hal_console.alive = true;
}

void furi_hal_console_disable() {
    furi_check(furi_hal_console.initialized);
    furi_hal_uart_wait_tx_complete(furi_hal_console.channel);
    furi_hal_console.alive = false;
}

void furi_hal_console_set_tx_callback(FuriHalConsoleTxCallback callback, void* context) {
    FURI_CRITICAL_ENTER();
    furi_hal_console.tx_callback = callback;
    furi_hal_console.tx_callback_context = context;
    FURI_CRITICAL_EXIT();
}

void furi_hal_console_tx(const uint8_t* buffer, size_t buffer_size) {
    if(!furi_hal_console.alive) return;

    FURI_CRITICAL_ENTER();
    // Transmit data

    if(furi_hal_console.tx_callback) {
        furi_hal_console.tx_callback(buffer, buffer_size, furi_hal_console.tx_callback_context);
    }

    furi_hal_uart_tx(furi_hal_console.channel, (uint8_t*)buffer, buffer_size);
    // Wait for TC flag to be raised for last char
    furi_hal_uart_wait_tx_complete(furi_hal_console.channel);
    FURI_CRITICAL_EXIT();
}

void furi_hal_console_tx_with_new_line(const uint8_t* buffer, size_t buffer_size) {
    if(!furi_hal_console.alive) return;

    FURI_CRITICAL_ENTER();
    // Transmit data
    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)buffer, buffer_size);
    // Transmit new line symbols
    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)"\r\n", 2);
    // Wait for TC flag to be raised for last char
    furi_hal_uart_wait_tx_complete(furi_hal_console.channel);
    FURI_CRITICAL_EXIT();
}

void furi_hal_console_printf(const char format[], ...) {
    FuriString* string;
    va_list args;
    va_start(args, format);
    string = furi_string_alloc_vprintf(format, args);
    va_end(args);
    furi_hal_console_tx((const uint8_t*)furi_string_get_cstr(string), furi_string_size(string));
    furi_string_free(string);
}

void furi_hal_console_puts(const char* data) {
    furi_hal_console_tx((const uint8_t*)data, strlen(data));
}
