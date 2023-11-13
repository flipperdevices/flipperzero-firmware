#pragma once

#include "seader_bridge.h"

int32_t seader_uart_tx_thread(void* context);
void seader_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context);
void seader_uart_serial_init(SeaderUartBridge* seader_uart, uint8_t uart_ch);
void seader_uart_serial_deinit(SeaderUartBridge* seader_uart, uint8_t uart_ch);
void seader_uart_set_baudrate(SeaderUartBridge* seader_uart, uint32_t baudrate);
int32_t seader_uart_worker(void* context);

SeaderUartBridge* seader_uart_enable(SeaderUartConfig* cfg, Seader* seader);
void seader_uart_disable(SeaderUartBridge* seader_uart);
void seader_uart_set_config(SeaderUartBridge* seader_uart, SeaderUartConfig* cfg);
void seader_uart_get_config(SeaderUartBridge* seader_uart, SeaderUartConfig* cfg);
void seader_uart_get_state(SeaderUartBridge* seader_uart, SeaderUartState* st);

SeaderUartBridge* seader_uart_alloc();
void seader_uart_free(SeaderUartBridge* seader_uart);
