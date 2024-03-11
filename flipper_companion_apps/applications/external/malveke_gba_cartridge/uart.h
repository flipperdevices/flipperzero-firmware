#ifndef UART_H
#define UART_H

#pragma once

#include <furi.h>
#include <furi_hal.h>

#define RX_BUF_SIZE (1024)

typedef struct Uart Uart;

void uart_set_handle_rx_data_cb(
    Uart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context));
void uart_tx(void* app, uint8_t* data, size_t len);
Uart* usart_init(void* app);
Uart* lp_uart_init(void* app);
void uart_free(Uart* uart);

#endif /* UART_H */