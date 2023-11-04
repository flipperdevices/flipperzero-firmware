#ifndef __flipper_structs_h__
#define __flipper_structs_h__

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>
#include <gui/modules/button_panel.h>
#include <gui/modules/menu.h>
#include <core/thread.h>
#include <core/message_queue.h>

#include "game_structs.h"

/* Application context structure */
struct ApplicationContext {
    /* GUI */
    Gui *gui;
    SceneManager *scene_manager;
    ViewDispatcher *view_dispatcher;
    /* Modules for GUI */
    Loading *loading_module;
    ButtonPanel *button_module;
    Menu *menu_module;

    /* Others */
    FuriThread *secondary_thread;
    FuriMessageQueue *threads_message_queue; // Message queue between main thread and secondary thread
    struct GameState *game_state; // Read by draw_callback thread, written by the secondary thread
};

#endif
