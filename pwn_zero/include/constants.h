#pragma once

#include <furi_hal_uart.h>

/// Defines the channel that the pwnagotchi uses
#define PWNAGOTCHI_UART_CHANNEL FuriHalUartIdUSART1

/// Defines the baudrate that the pwnagotchi will use
#define PWNAGOTCHI_UART_BAUD 57600

/// Height of flipper screen
#define FLIPPER_SCREEN_HEIGHT 64

/// Width of flipper screen
#define FLIPPER_SCREEN_WIDTH 128

/// Length of the protocol transmission in bytes
#define PWNAGOTCHI_PROTOCOL_BYTE_LEN 5

/// Number or messages that can be stored in the queue at one time
#define PWNAGOTCHI_PROTOCOL_QUEUE_SIZE 5