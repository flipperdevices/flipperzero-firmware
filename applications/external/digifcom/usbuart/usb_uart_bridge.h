#pragma once

/*
USB CDC controller linkage copied from flipper zero firmware
and modified to remove the serial bridge and just link to
some buffer streams
*/

#include "flipper.h"
#include <stdint.h>
#include <stdbool.h>
#include <furi_hal.h>
#include <furi_hal_usb_cdc.h>

extern FuriHalUsbInterface usb_cdc_fcom;

typedef void (*UsbUartCallback)();
typedef struct UsbUartBridge UsbUartBridge;

typedef struct {
    uint8_t vcp_ch;
    uint8_t baudrate_mode;
    uint32_t baudrate;
    UsbUartCallback cb; // called when data tx is ready
    void* ctx;
} UsbUartConfig;

typedef struct {
    uint32_t rx_cnt;
    uint32_t tx_cnt;
    uint32_t baudrate_cur;
} UsbUartState;

UsbUartBridge* usb_uart_enable(UsbUartConfig* cfg);

void usb_uart_send(UsbUartBridge* usb_uart, const uint8_t* data, size_t len);

void usb_uart_disable(UsbUartBridge* usb_uart);

void usb_uart_set_config(UsbUartBridge* usb_uart, UsbUartConfig* cfg);

void usb_uart_get_config(UsbUartBridge* usb_uart, UsbUartConfig* cfg);

void usb_uart_get_state(UsbUartBridge* usb_uart, UsbUartState* st);
