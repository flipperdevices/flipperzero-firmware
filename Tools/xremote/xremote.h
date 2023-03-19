#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

#include <assets_icons.h>
#include <gui/view_stack.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <input/input.h>
#include <stdlib.h>
#include <stdio.h>
#include <notification/notification_messages.h>

#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_input.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/button_panel.h>
#include <gui/modules/variable_item_list.h>

#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include "scenes/xremote_scene.h"
#include "views/xremote_infoscreen.h"
#include "views/xremote_scene_2.h"
#include "helpers/xremote_storage.h"
#include "models/infrared/xremote_ir_remote.h"
#include "models/cross/xremote_remote.h"

#define TAG "XRemote"
#define INFRARED_APP_EXTENSION ".ir"
#define INFRARED_APP_FOLDER ANY_PATH("infrared")

#define XREMOTE_TEXT_STORE_NUM 2
#define XREMOTE_TEXT_STORE_SIZE 128
#define XREMOTE_APP_EXTENSION ".xr"
#define XREMOTE_MAX_ITEM_NAME_LENGTH 22
#define XREMOTE_MAX_REMOTE_NAME_LENGTH 22

typedef struct {
    Gui* gui;
    DialogsApp* dialogs;
    FuriString* file_path;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    ButtonMenu* button_menu_create;
    ButtonMenu* button_menu_create_add;
    ButtonMenu* button_menu_ir;
    TextInput* text_input;
    Popup* popup;
    Loading* loading;
    ViewStack* view_stack;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    XRemoteInfoscreen* xremote_infoscreen;
    XRemoteScene2* xremote_scene_2;
    InfraredRemote* ir_remote_buffer;
    CrossRemote* cross_remote;
    uint32_t haptic; 
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
    char text_store[XREMOTE_TEXT_STORE_NUM][XREMOTE_TEXT_STORE_SIZE + 1];
} XRemote;

typedef enum {
    XRemoteRemoteItemTypeInfrared,
    XRemoteRemoteItemTypeSubGhz,
    XRemoteRemoteItemTypePause,
} XRemoteRemoteItemType;

typedef enum {
    XRemoteViewIdInfoscreen,
    XRemoteViewIdMenu,
    XRemoteViewIdCreate,
    XRemoteViewIdCreateAdd,
    XRemoteViewIdScene2,
    XRemoteViewIdSettings,
    XRemoteViewIdWip,
    XRemoteViewIdIrRemote,
    XRemoteViewIdStack,
    XRemoteViewIdTextInput,
} XRemoteViewId;

typedef enum {
    XRemoteHapticOff,
    XRemoteHapticOn,
} XRemoteHapticState;

typedef enum {
    XRemoteSpeakerOff,
    XRemoteSpeakerOn,
} XRemoteSpeakerState;

typedef enum {
    XRemoteLedOff,
    XRemoteLedOn,
} XRemoteLedState;

typedef enum {
    XRemoteSettingsOff,
    XRemoteSettingsOn,
} XRemoteSettingsStoreState;

void xremote_popup_closed_callback(void* context);
void xremote_text_input_callback(void* context);
void xremote_show_loading_popup(XRemote* xremote, bool show);