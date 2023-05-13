#pragma once

#include "ac_remote_app.h"
#include "scenes/ac_remote_scene.h"
#include "ac_remote_custom_event.h"
#include "ac_remote.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <storage/storage.h>
#include <notification/notification_messages.h>

#define SAVING_DIRECTORY "/ext/apps_data/ac_remote"
#define SAVING_FILENAME SAVING_DIRECTORY "/mitsubishi_ac_remote.conf"

#define NUM_MENU_ITEMS (5)

typedef struct {
    int selected_menu_index;
    int selected_option_index[NUM_MENU_ITEMS];
} AppState;

struct AC_RemoteApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    VariableItemList* var_item_list;
    AppState app_state;
};

typedef enum {
    AC_RemoteAppViewVarItemList,
} AC_RemoteAppView;
