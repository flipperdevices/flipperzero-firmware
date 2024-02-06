#ifndef __flipper_structs_h__
#define __flipper_structs_h__

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/loading.h>
#include "gui/custom_modules/button_panel.h" // Custom version of this module
#include <gui/modules/variable_item_list.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_box.h>
#include <gui/modules/popup.h>

#include <core/thread.h>
#include <core/message_queue.h>

#include "game_structs.h"

/* Application context structure */
struct ApplicationContext {
    /* GUI */
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    /* Modules for GUI */
    Loading* loading_module;
    ButtonPanel* button_module;
    VariableItemList* variable_item_list_module;
    DialogEx* dialog_ex_module;
    TextBox* text_box_module;
    Popup* popup_module;

    /* Others */
    FuriThread* secondary_thread;
    FuriMessageQueue*
        threads_message_queue; // Message queue between main thread, GUI and secondary thread
    struct GameState* game_state; // Read by draw_callback thread, written by the secondary thread
};

#endif
