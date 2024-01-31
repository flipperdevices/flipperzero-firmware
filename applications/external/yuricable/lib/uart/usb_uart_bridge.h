#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <log_saver.h>

#define COMMAND_LENGTH 16

typedef struct UsbUartBridge UsbUartBridge;

typedef struct {
    uint8_t vcp_ch;
    uint8_t uart_ch;
    uint8_t flow_pins;
    uint8_t baudrate_mode;
    uint32_t baudrate;
    uint8_t software_de_re;
} UsbUartConfig;

typedef struct {
    uint32_t rx_cnt;
    uint32_t tx_cnt;
    uint32_t baudrate_cur;
} UsbUartState;

typedef FuriString* (*UsbUartBridgeCommand)(char* command, void* context);

UsbUartBridge* usb_uart_enable(UsbUartConfig* cfg);

void usb_uart_disable(UsbUartBridge* usb_uart);

void usb_uart_set_config(UsbUartBridge* usb_uart, UsbUartConfig* cfg);

void usb_uart_set_command_callback(UsbUartBridge* usb_uart, UsbUartBridgeCommand callback, void* context);

void usb_uart_get_config(UsbUartBridge* usb_uart, UsbUartConfig* cfg);

void usb_uart_get_state(UsbUartBridge* usb_uart, UsbUartState* st);

void usb_uart_send_data(UsbUartBridge* usb_uart, uint8_t* data, size_t data_size);
