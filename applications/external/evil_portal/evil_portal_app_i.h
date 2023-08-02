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
#include <cfw.h>
#include <storage/storage.h>

#define NUM_MENU_ITEMS (6)

#define EVIL_PORTAL_TEXT_BOX_STORE_SIZE (4096)
#define UART_CH \
    (CFW_SETTINGS()->uart_esp_channel == UARTDefault ? FuriHalUartIdUSART1 : FuriHalUartIdLPUART1)

#define SET_HTML_CMD "sethtml"
#define SET_AP_CMD "setap"
#define RESET_CMD "reset"

#define HTML_EXTENSION ".html"
#define HTML_FOLDER ANY_PATH("apps_data/evil_portal/html")

struct Evil_PortalApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Storage* storage;

    FuriString* portal_logs;
    const char* command_queue[1];
    int command_index;
    bool has_command_queue;

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
    bool sent_ap;
    bool sent_html;
    bool sent_reset;
    int BAUDRATE;

    // AP SSID length can be maximum 32.
    // Make the buffer 33 to accommodate the terminator char.
    char ap_name[33];
};

typedef enum {
    Evil_PortalAppViewVarItemList,
    Evil_PortalAppViewConsoleOutput,
    Evil_PortalAppViewStartPortal,
    Evil_PortalAppViewTextInput,
} Evil_PortalAppView;
