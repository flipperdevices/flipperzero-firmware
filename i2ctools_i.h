#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#include "i2csniffer.h"

// Menu
typedef enum {
    MAIN_VIEW,
    SCAN_VIEW,
    SNIFF_VIEW,
    SEND_VIEW,
    PLAY_VIEW,

    /* Know menu Size*/
    MENU_SIZE
} i2cToolsMainMenu;

// Bus scanner
#define MAX_I2C_ADDR 0x7F
typedef struct {
    uint8_t addresses[MAX_I2C_ADDR + 1];
    uint8_t found;
    uint8_t menu_index;
    bool scanned;
} _scanner;

// Sender
typedef struct {
    uint8_t address_idx;
    uint8_t value;
    uint8_t recv[2];
    bool must_send;
    bool sended;
    bool error;
} _sender;

// App datas
typedef struct {
    ViewPort* view_port;
    i2cToolsMainMenu current_menu;
    uint8_t main_menu_index;

    _scanner scanner;
    i2csniffer* sniffer;
    _sender sender;
} i2cTools;
