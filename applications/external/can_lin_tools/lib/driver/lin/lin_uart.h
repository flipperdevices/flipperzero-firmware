#pragma once

#include <furi.h>

void lin_uart_init(uint32_t baudrate, void* context);

void lin_uart_set_br(uint32_t baudrate);

void lin_uart_deinit(void);

void lin_uart_suspend(void);

void lin_uart_resume(void);

uint8_t lin_uart_get_char(void);

void lin_uart_put_char(uint8_t data);

void lin_uart_reset(void);

void lin_uart_tx_irq_enable(void);

void lin_uart_tx_irq_disable(void);

void lin_uart_rx_irq_enable(void);

void lin_uart_rx_irq_disable(void);

void lin_uart_break_irq_enable(void);

void lin_uart_break_irq_disable(void);

void lin_uart_put_break(void);

void lin_uart_timeout_start(void);

void lin_uart_timeout_stop(void);
