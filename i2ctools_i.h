#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#include "i2csniffer.h"
#include "i2cscanner.h"

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

    i2cScanner* scanner;
    i2cSniffer* sniffer;
    _sender sender;
} i2cTools;
