#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>
#include "scenes/xremote_scene.h"
#include "views/xremote_infoscreen.h"
#include "views/xremote_scene_2.h"
#include "helpers/xremote_storage.h"

#define TAG "XRemote"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    ButtonMenu* button_menu;
    Popup* popup;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    XRemoteInfoscreen* xremote_infoscreen;
    XRemoteScene2* xremote_scene_2;
    uint32_t haptic; 
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
} XRemote;

typedef enum {
    XRemoteViewIdInfoscreen,
    XRemoteViewIdMenu,
    XRemoteViewIdCreate,
    XRemoteViewIdCreateAdd,
    XRemoteViewIdScene2,
    XRemoteViewIdSettings,
    XRemoteViewIdWip,
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