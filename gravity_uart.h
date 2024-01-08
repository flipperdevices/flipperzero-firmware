#pragma once

#include "furi_hal.h"

#define RX_BUF_SIZE (320)

typedef struct GravityUart GravityUart;

void gravity_uart_set_handle_rx_data_cb(
    GravityUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context));
void gravity_uart_tx(uint8_t* data, size_t len);
GravityUart* gravity_uart_init(GravityApp* app);
void gravity_uart_free(GravityUart* uart);
