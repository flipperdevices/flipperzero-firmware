#pragma once

#include "bad_kb_app.h"
#include "scenes/bad_kb_scene.h"
#include "helpers/ducky_script.h"
#include "helpers/ble_hid.h"
#include "bad_kb_paths.h"

#include <gui/gui.h>
#include <assets_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/loading.h>
#include <gui/modules/widget.h>
#include "views/bad_kb_view.h"
#include <furi_hal_usb.h>

#define BAD_KB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_KB_APP_LAYOUT_EXTENSION ".kl"

extern uint8_t BAD_KB_BOUND_MAC[GAP_MAC_ADDR_SIZE]; // For remember mode

typedef enum BadKbCustomEvent {
    BadKbAppCustomEventTextInputDone,
    BadKbAppCustomEventByteInputDone,
    BadKbCustomEventErrorBack
} BadKbCustomEvent;

typedef enum {
    BadKbAppErrorNoFiles,
} BadKbAppError;

typedef struct {
    BleProfileHidParams ble;
    FuriHalUsbHidConfig usb;
} BadKbConfig;

typedef enum {
    BadKbConnModeNone,
    BadKbConnModeUsb,
    BadKbConnModeBt,
} BadKbConnMode;

struct BadKbApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    VariableItemList* var_item_list;
    TextInput* text_input;
    ByteInput* byte_input;
    Loading* loading;

    char bt_name_buf[FURI_HAL_BT_ADV_NAME_LENGTH];
    uint8_t bt_mac_buf[GAP_MAC_ADDR_SIZE];
    char usb_name_buf[32];
    uint16_t usb_vidpid_buf[2];

    BadKbAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadKb* bad_kb_view;
    BadKbScript* bad_kb_script;

    Bt* bt;
    bool is_bt;
    bool bt_remember;
    BadKbConfig config; // User options
    BadKbConfig id_config; // ID and BT_ID values

    bool set_bt_id;
    bool set_usb_id;
    bool has_bt_id;
    bool has_usb_id;

    FuriHalBleProfileBase* ble_hid;
    FuriHalUsbInterface* prev_usb_mode;

    BleProfileHidParams cur_ble_cfg;
    FuriHalUsbHidConfig* cur_usb_cfg;

    BadKbConnMode conn_mode;
    FuriThread* conn_init_thread;
};

typedef enum {
    BadKbAppViewWidget,
    BadKbAppViewWork,
    BadKbAppViewVarItemList,
    BadKbAppViewByteInput,
    BadKbAppViewTextInput,
    BadKbAppViewLoading,
} BadKbAppView;

void bad_kb_app_show_loading_popup(BadKbApp* app, bool show);

void bad_kb_load_settings(BadKbApp* app);

int32_t bad_kb_conn_apply(BadKbApp* app);

void bad_kb_conn_reset(BadKbApp* app);

void bad_kb_config_refresh(BadKbApp* app);

void bad_kb_config_adjust(BadKbConfig* cfg);
