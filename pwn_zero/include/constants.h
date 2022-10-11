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