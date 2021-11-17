#pragma once

#include <stdint.h>

typedef struct {
    uint8_t vcp_ch;
    uint8_t uart_ch;
    uint8_t flow_pins;
    uint8_t baudrate_mode;
    uint32_t baudrate;
} UsbUartConfig;

typedef struct {
    uint32_t rx_cnt;
    uint32_t tx_cnt;
    uint32_t baudrate_cur;
} UsbUartState;

void usb_uart_enable(UsbUartConfig* cfg);

void usb_uart_disable();

void usb_uart_set_config(UsbUartConfig* cfg);

void usb_uart_get_config(UsbUartConfig* cfg);

void usb_uart_get_state(UsbUartState* st);
