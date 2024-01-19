#pragma once

#include "evil_portal_app.h"
#include "evil_portal_custom_event.h"
#include "evil_portal_uart.h"
#include "scenes/evil_portal_scene.h"
#include "evil_portal_icons.h"

#include <gui/gui.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/view_stack.h>
#include <dialogs/dialogs.h>

#include <xtreme/xtreme.h>

#define NUM_MENU_ITEMS (6)

#define EVIL_PORTAL_TEXT_BOX_STORE_SIZE (4096)
#define UART_CH (xtreme_settings.uart_esp_channel)

#define SET_HTML_CMD "sethtml"
#define SET_AP_CMD "setap"
#define RESET_CMD "reset"

struct Evil_PortalApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    FuriString* portal_logs;
    FuriMutex* portal_logs_mutex;

    FuriString* text_box_store;
    size_t text_box_store_strlen;
    TextBox* text_box;

    VariableItemList* var_item_list;
    Evil_PortalUart* uart;
    TextInput* text_input;
    DialogsApp* dialogs;
    FuriString* file_path;
    Loading* loading;
    ViewStack* view_stack;

    int selected_menu_index;
    int selected_option_index[NUM_MENU_ITEMS];
    const char* selected_tx_string;
    bool is_command;
    bool is_custom_tx_string;
    bool focus_console_start;
    bool show_stopscan_tip;
    bool sent_reset;
    int BAUDRATE;
    char text_store[2][128 + 1];

    bool capture_line;
    FuriString* captured_line;

    uint8_t* index_html;
    uint8_t* ap_name;
};

typedef enum {
    Evil_PortalAppViewVarItemList,
    Evil_PortalAppViewConsoleOutput,
    Evil_PortalAppViewStartPortal,
    Evil_PortalAppViewTextInput,
} Evil_PortalAppView;
