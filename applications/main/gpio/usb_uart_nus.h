#pragma once

#include <furi.h>

/* Notifications fit the default ATT MTU (23 bytes). */
#define USB_UART_NUS_PACKET_SIZE  20
#define USB_UART_NUS_NAME_SIZE    6
#define USB_UART_NUS_ADDRESS_SIZE 18

typedef struct UsbUartNus UsbUartNus;
typedef void (*UsbUartNusCallback)(void* context);

/** Start an unbonded Nordic UART Service profile, or return NULL on failure. */
UsbUartNus*
    usb_uart_nus_alloc(UsbUartNusCallback ready, UsbUartNusCallback disconnected, void* context);

/** Stop the service and restore the default Bluetooth profile. */
void usb_uart_nus_free(UsbUartNus* transport);

/** Read available client-to-UART data without blocking. */
size_t usb_uart_nus_read(UsbUartNus* transport, uint8_t* data, size_t capacity);

/** Notify the client; return false if not subscribed or the update failed. */
bool usb_uart_nus_send(UsbUartNus* transport, const uint8_t* data, size_t length);

/** Get the advertised name and bridge-specific Bluetooth address. */
void usb_uart_nus_get_identity(
    char name[USB_UART_NUS_NAME_SIZE],
    char address[USB_UART_NUS_ADDRESS_SIZE]);
