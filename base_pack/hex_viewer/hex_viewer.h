#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <hex_viewer_icons.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/dialog_ex.h>
#include "scenes/hex_viewer_scene.h"
#include "views/hex_viewer_startscreen.h"
#include "helpers/hex_viewer_storage.h"

#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/buffered_file_stream.h>
#include <toolbox/stream/file_stream.h>

#define TAG "HexViewer"

#define HEX_VIEWER_APP_PATH_FOLDER "/any" // TODO ANY_PATH
#define HEX_VIEWER_APP_EXTENSION "*"
#define HEX_VIEWER_PERCENT_INPUT 16

#define HEX_VIEWER_BYTES_PER_LINE 4u
#define HEX_VIEWER_LINES_ON_SCREEN 4u
#define HEX_VIEWER_BUF_SIZE (HEX_VIEWER_LINES_ON_SCREEN * HEX_VIEWER_BYTES_PER_LINE)

typedef struct {
    uint8_t file_bytes[HEX_VIEWER_LINES_ON_SCREEN][HEX_VIEWER_BYTES_PER_LINE];
    uint32_t file_offset;
    uint32_t file_read_bytes;
    uint32_t file_size;

    Stream* stream;
} HexViewerModel;

typedef struct {
    HexViewerModel* model;

    Gui* gui;
    Storage* storage;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    TextInput* text_input;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    HexViewerStartscreen* hex_viewer_startscreen;
    DialogsApp* dialogs; // File Browser
    FuriString* file_path; // File Browser
    uint32_t haptic;
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
    char percent_buf[HEX_VIEWER_PERCENT_INPUT];
} HexViewer;

typedef enum {
    HexViewerViewIdStartscreen,
    HexViewerViewIdMenu,
    HexViewerViewIdScroll,
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
