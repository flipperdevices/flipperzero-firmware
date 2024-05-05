#pragma once

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>
#include <dialogs/dialogs.h>

#include <storage/storage.h>
#include <notification/notification_messages.h>

#include "views/action_menu.h"
#include "item.h"

#define QUAC_NAME "Quac!"
#define QUAC_VERSION "v0.6"
#define QUAC_ABOUT                                    \
    "Quick Action remote control\n" QUAC_VERSION "\n" \
    "github.com/rdefeo/quac"
#define TAG "Quac" // log statement id

// Location of our actions and folders
#define QUAC_SETTINGS_FILENAME ".quac.conf"
#define QUAC_SETTINGS_PATH APP_DATA_PATH(QUAC_SETTINGS_FILENAME)

typedef enum { QUAC_APP_PORTRAIT, QUAC_APP_LANDSCAPE } QuacAppLayout;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    ActionMenu* action_menu;
    VariableItemList* vil_settings;
    DialogsApp* dialog;
    Submenu* sub_menu;
    TextInput* text_input;
    Popup* popup;

    Storage* storage;
    NotificationApp* notifications;

    ItemsView* items_view;
    int depth;
    int selected_item;

    FuriString* temp_str; // used for renames/etc
    char temp_cstr[MAX_NAME_LEN]; // used for renames/etc

    struct {
        QuacAppLayout layout; // Defaults to Portrait
        bool show_icons; // Defaults to True
        bool show_headers; // Defaults to True
        uint32_t rfid_duration; // Defaults to 2500 ms
        uint32_t nfc_duration; // Defaults to 1000 ms
        uint32_t subghz_repeat; // Defaults to 10, just like the CLI
        bool subghz_use_ext_antenna; // Defaults to False
        bool show_hidden; // Defaults to False
    } settings;

} App;

App* app_alloc();
void app_free(App* app);