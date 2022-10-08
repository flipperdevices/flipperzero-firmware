#pragma once

#include <furi_hal_uart.h>

/// Defines the channel that the pwnagotchi uses
const FuriHalUartId PWNAGOTCHI_UART_CHANNEL = FuriHalUartIdUSART1;

/// Defines the baudrate that the pwnagotchi will use
const uint32_t PWNAGOTCHI_UART_BAUD = 57600;