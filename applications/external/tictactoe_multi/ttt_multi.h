#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>

#include <input/input.h>

#include "scenes/ttt_multi_scene.h"
#include "views/ttt_multi_game_view.h"

#include "ttt_multi_icons.h"

typedef struct {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    // Common views
    Submenu* submenu;
    TttMultiGameView* game_view;
} TttMultiApp;

typedef enum {
    TttMultiViewMenu,
    TttMultiViewGame,
} TttMultiView;