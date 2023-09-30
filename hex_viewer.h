#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <assets_icons.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/dialog_ex.h>
#include "scenes/hex_viewer_scene.h"
#include "views/hex_viewer_startscreen.h"
#include "views/hex_viewer_scene_1.h"
#include "views/hex_viewer_scene_2.h"
#include "helpers/hex_viewer_storage.h"

#define TAG "HexViewer"

#define SUBGHZ_APP_EXTENSION ".sub"
#define SUBGHZ_APP_FOLDER ANY_PATH("subghz")

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    HexViewerStartscreen* hex_viewer_startscreen;
    HexViewerScene1* hex_viewer_scene_1;
    HexViewerScene2* hex_viewer_scene_2;
    DialogsApp* dialogs; // File Browser
    FuriString* file_path; // File Browser
    uint32_t haptic; 
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
    ButtonMenu* button_menu; // Button Menu
} HexViewer;

typedef enum {
    HexViewerViewIdStartscreen,
    HexViewerViewIdMenu,
    HexViewerViewIdScene1,
    HexViewerViewIdScene2,
    HexViewerViewIdScene3,
    HexViewerViewIdScene4,
    HexViewerViewIdScene5,
    HexViewerViewIdSettings,
} HexViewerViewId;

typedef enum {
    HexViewerHapticOff,
    HexViewerHapticOn,
} HexViewerHapticState;

typedef enum {
    HexViewerSpeakerOff,
    HexViewerSpeakerOn,
} HexViewerSpeakerState;

typedef enum {
    HexViewerLedOff,
    HexViewerLedOn,
} HexViewerLedState;

typedef enum {
    HexViewerSettingsOff,
    HexViewerSettingsOn,
} HexViewerSettingsStoreState;
