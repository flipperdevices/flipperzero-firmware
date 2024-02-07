#pragma once

#include "furi_hal.h"

#include <cfw/cfw.h>

#define UART_CH \
    (CFW_SETTINGS()->uart_esp_channel == UARTDefault ? FuriHalUartIdUSART1 : FuriHalUartIdLPUART1)
#define DEFAULT_BAUDRATE (115200)
#define FAST_BAUDRATE (921600)

#define RX_BUF_SIZE (2048)

typedef struct EspFlasherUart EspFlasherUart;

void esp_flasher_uart_set_handle_rx_data_cb(
    EspFlasherUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context));
void esp_flasher_uart_tx(uint8_t* data, size_t len);
EspFlasherUart* esp_flasher_usart_init(EspFlasherApp* app);
void esp_flasher_uart_free(EspFlasherUart* uart);
