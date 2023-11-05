#ifndef FLIP_GRAV_SYNC_H
#define FLIP_GRAV_SYNC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_flip_struct.h"
#include "uart_terminal_app_i.h"

bool syncNextToken(uint8_t **syncString, GravitySyncItem *tokenItem, char *tokenValue);
bool syncProcessResponse(UART_TerminalApp *app);
void uart_terminal_sync_rx_data_cb(uint8_t* buf, size_t len, void* context);
void syncCleanup();
void do_sync(UART_TerminalApp *app);

#endif