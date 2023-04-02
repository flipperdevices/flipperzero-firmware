#pragma once

#include "scenes/xremote_scene.h"
#include "views/xremote_infoscreen.h"
#include "views/xremote_transmit.h"
#include "views/xremote_pause_set.h"
#include "helpers/xremote_storage.h"
#include "models/infrared/xremote_ir_remote.h"
#include "models/cross/xremote_remote.h"
#include "xremote_i.h"

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
    XRemoteTransmit* xremote_transmit;
    XRemotePauseSet* xremote_pause_set;
    InfraredRemote* ir_remote_buffer;
    InfraredWorker* ir_worker;
    CrossRemote* cross_remote;
    uint32_t haptic; 
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
    bool transmitting;
    char text_store[XREMOTE_TEXT_STORE_NUM][XREMOTE_TEXT_STORE_SIZE + 1];
} XRemote;

typedef enum {
    XRemoteViewIdInfoscreen,
    XRemoteViewIdMenu,
    XRemoteViewIdCreate,
    XRemoteViewIdCreateAdd,
    XRemoteViewIdSettings,
    XRemoteViewIdWip,
    XRemoteViewIdIrRemote,
    XRemoteViewIdStack,
    XRemoteViewIdTextInput,
    XRemoteViewIdTransmit,
    XRemoteViewIdPauseSet,
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