#ifndef STORAGE_H
#define STORAGE_H

/* Module to save and load settings to and from Flipper's internal storage */
#include "uart_terminal_app_i.h"
#include <storage/storage.h>

bool save_settings(UART_TerminalApp* app);
bool load_settings(UART_TerminalApp* app);
bool save_data(UART_TerminalApp* app);
bool load_data(UART_TerminalApp* app);

#endif