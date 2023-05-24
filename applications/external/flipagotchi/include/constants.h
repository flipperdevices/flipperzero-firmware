#pragma once

#include <furi_hal_uart.h>

/// Defines the channel that the pwnagotchi uses
#define PWNAGOTCHI_UART_CHANNEL FuriHalUartIdUSART1

/// Defines the baudrate that the pwnagotchi will use
#define PWNAGOTCHI_UART_BAUD 115200

/// Max length of channel data at top left
#define PWNAGOTCHI_MAX_CHANNEL_LEN 4

/// Max length of APS captured at top left
#define PWNAGOTCHI_MAX_APS_LEN 11

/// Max length for uptime
#define PWNAGOTCHI_MAX_UPTIME_LEN 11

/// Maximum length of pwnagotchi hostname
#define PWNAGOTCHI_MAX_HOSTNAME_LEN 11

/// Maximum length of pwnagotchi message
#define PWNAGOTCHI_MAX_MESSAGE_LEN 101

/// Maximum length of handshakes info at the bottom
#define PWNAGOTCHI_MAX_HANDSHAKES_LEN 21

/// Maximum length of a pwnagotchi SSID info displayed at the bottom
#define PWNAGOTCHI_MAX_SSID_LEN 26

/// Maximum length for pwnagotchi friend name and stats
#define PWNAGOTCHI_MAX_FRIEND_STAT_LEN 21

/// Height of flipper screen
#define FLIPPER_SCREEN_HEIGHT 64

/// Width of flipper screen
#define FLIPPER_SCREEN_WIDTH 128