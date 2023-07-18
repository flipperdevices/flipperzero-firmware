#pragma once

#include <gui/gui.h>
#include <gui/view.h>
#include <assets_icons.h>
#include <gui/view_stack.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <storage/storage.h>
#include <notification/notification_messages.h>

#include "ac_remote_app.h"
#include "scenes/ac_remote_scene.h"
#include "ac_remote_custom_event.h"
#include "ac_remote.h"
#include "views/ac_remote_panel.h"

#define SAVING_DIRECTORY "/ext/apps_data/ac_remote"
#define SAVING_FILENAME SAVING_DIRECTORY "/mitsubishi_ac_remote.conf"
#define MAGIC 0xF0AC

#define NUM_BUTTON_ITEMS (6)

typedef struct
{
    uint16_t magic;
    int currient_value_index[NUM_BUTTON_ITEMS];
    ACState ac_state;
} AppState;

struct AC_RemoteApp
{
    Gui *gui;
    ViewDispatcher *view_dispatcher;
    SceneManager *scene_manager;
    ViewStack *view_stack;
    ACRemotePanel *ac_remote_panel;
    AppState app_state;
};

typedef enum
{
    AC_RemoteAppViewStack,
} AC_RemoteAppView;
