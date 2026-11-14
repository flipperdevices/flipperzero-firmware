#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct UsbUartBridge UsbUartBridge;

typedef enum {
    UsbUartTransportUsb,
    UsbUartTransportBle,
} UsbUartTransport;

/* Startup stays asynchronous so USB service calls never block scene entry. */
typedef enum {
    UsbUartStartupPending,
    UsbUartStartupReady,
    UsbUartStartupFailed,
} UsbUartStartup;

typedef struct {
    UsbUartTransport transport;
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

UsbUartBridge* usb_uart_enable(UsbUartConfig* cfg);

void usb_uart_disable(UsbUartBridge* usb_uart);

bool usb_uart_set_config(UsbUartBridge* usb_uart, UsbUartConfig* cfg);

void usb_uart_get_config(UsbUartBridge* usb_uart, UsbUartConfig* cfg);

void usb_uart_get_state(UsbUartBridge* usb_uart, UsbUartState* st);

UsbUartStartup usb_uart_get_startup(UsbUartBridge* usb_uart);
