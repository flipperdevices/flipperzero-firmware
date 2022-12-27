#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <assets_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/widget.h>

#include <notification/notification_messages.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#include <toolbox/path.h>

#include "scenes/mag_scene.h"

#define MAG_KEY_NAME_SIZE 22
#define MAG_TEXT_STORE_SIZE 40

#define MAG_APP_FOLDER ANY_PATH("mag")
#define MAG_SD_FOLDER EXT_PATH("mag")
#define MAG_APP_EXTENSION ".mag"
#define MAG_APP_SHADOW_EXTENSION ".shd"

enum MagCustomEvent {
    MagEventNext = 100,
    MagEventExit,
    MagEventPopupClosed,
};

typedef struct Mag Mag;

struct Mag {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    SceneManager* scene_manager;
    Storage* storage;
    DialogsApp* dialogs;
    Widget* widget;

    char text_store[MAG_TEXT_STORE_SIZE + 1];
    FuriString* file_path;
    FuriString* file_name;

    // Common views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    TextInput* text_input;
    ByteInput* byte_input;

    // Custom views?
};

typedef enum {
    MagViewSubmenu,
    MagViewDialogEx,
    MagViewPopup,
    MagViewWidget,
    MagViewTextInput,
    MagViewByteInput,
} MagView;

bool mag_save_key(Mag* mag);

bool mag_load_key_from_file_select(Mag* mag);

bool mag_delete_key(Mag* mag);

bool mag_load_key_data(Mag* mag, FuriString* path, bool show_dialog);

bool mag_save_key_data(Mag* mag, FuriString* path);

void mag_make_app_folder(Mag* mag);

void mag_text_store_set(Mag* mag, const char* text, ...);

void mag_text_store_clear(Mag* mag);

void mag_popup_timeout_callback(void* context);

void mag_widget_callback(GuiButtonType result, InputType type, void* context);

void mag_text_input_callback(void* context);
