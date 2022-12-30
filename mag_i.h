#pragma once

#include "mag_device.h"

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>

#include <notification/notification_messages.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#include <toolbox/path.h>

#include "scenes/mag_scene.h"

#define MAG_TEXT_STORE_SIZE 128

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
    MagDevice* mag_dev;

    char text_store[MAG_TEXT_STORE_SIZE + 1];
    FuriString* file_path;
    FuriString* file_name;

    // Common views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    Widget* widget;

    // Custom views?
};

typedef enum {
    MagViewSubmenu,
    MagViewDialogEx,
    MagViewPopup,
    MagViewLoading,
    MagViewWidget,
    MagViewTextInput,
} MagView;

void mag_text_store_set(Mag* mag, const char* text, ...);

void mag_text_store_clear(Mag* mag);

void mag_show_loading_popup(void* context, bool show);

// all below this comment are destined for deprecation (now handled by mag_device)

bool mag_save_key(Mag* mag);

bool mag_load_key_from_file_select(Mag* mag);

bool mag_delete_key(Mag* mag);

bool mag_load_key_data(Mag* mag, FuriString* path, bool show_dialog);

bool mag_save_key_data(Mag* mag, FuriString* path);

void mag_make_app_folder(Mag* mag);

void mag_popup_timeout_callback(void* context);

void mag_widget_callback(GuiButtonType result, InputType type, void* context);

void mag_text_input_callback(void* context);
