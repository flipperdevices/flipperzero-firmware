#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <dialogs/dialogs.h>

#include <gui/modules/submenu.h>

#include "scenes/rpc_debug_app_scene.h"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
} RpcDebugApp;

typedef enum {
    RpcDebugAppViewSubmenu,
} RpcDebugAppView;
