#pragma once

#include "furi_hal.h"

#define RX_BUF_SIZE (320)

typedef struct Pi_TerminalUart Pi_TerminalUart;

void Pi_Terminal_uart_set_handle_rx_data_cb(
    Pi_TerminalUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context));
void Pi_Terminal_uart_tx(Pi_TerminalUart* uart, uint8_t* data, size_t len);
Pi_TerminalUart* Pi_Terminal_uart_init(Pi_TerminalApp* app);
void Pi_Terminal_uart_free(Pi_TerminalUart* uart);
void Pi_Terminal_uart_send_ctrl_c(Pi_TerminalUart* uart);