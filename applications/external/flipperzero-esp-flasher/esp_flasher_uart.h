#pragma once

#include "furi_hal.h"

#define RX_BUF_SIZE (2048)

typedef struct EspFlasherUart EspFlasherUart;

void esp_flasher_uart_set_handle_rx_data_cb(
    EspFlasherUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context));
void esp_flasher_uart_tx(uint8_t* data, size_t len);
EspFlasherUart* esp_flasher_usart_init(EspFlasherApp* app);
void esp_flasher_uart_free(EspFlasherUart* uart);
