#pragma once

#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/variable_item_list.h>

#include <storage/storage.h>
#include <notification/notification_messages.h>

#include "views/action_menu.h"

#include "item.h"

#define QUAC_NAME "Quac!"
#define TAG "Quac" // log statement id

// Location of our actions and folders
#define QUAC_PATH "apps_data/quac"
// Full path to actions
#define QUAC_DATA_PATH EXT_PATH(QUAC_PATH)

typedef enum { QUAC_APP_PORTRAIT, QUAC_APP_LANDSCAPE } QuacAppLayout;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    ActionMenu* action_menu;
    VariableItemList* vil_settings;
    DialogEx* dialog;

    Storage* storage;
    NotificationApp* notifications;

    ItemsView* items_view;
    int depth;
    int selected_item;

    struct {
        QuacAppLayout layout; // Defaults to Portrait
        bool show_icons; // Defaults to True
        bool show_headers; // Defaults to True
        uint32_t rfid_duration; // Defaults to 2500 ms
    } settings;

} App;

App* app_alloc();
void app_free(App* app);