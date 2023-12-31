#include "../minesweeper.h"

static const char* farewellPrompts[] = {
    "Leaving so\n   soon?",
    "Will you\n   stay?",
    "Don't swim\n   away.",
    "Feeling\n   farewell?",
    "Sad to part\n   ways.",
    "Ocean of\n   goodbyes.",
    "Dolphin tears,\n   why?",
    "Lonely without\n   you.",
    "End of our\n   wave?",
    "Will you\n   return?",
    "Drowning in\n   goodbye.",
    "Farewell\n   ripples.",
    "Flipper's\n   frown.",
    "Sea of\n   solitude.",
    "Parting\n   currents.",
    "Goodbye, old\n   friend.",
    "Will tide\n  bring back?",
    "Echoes of\n  departure.",
    "Dolphin's last\n   dance.",
    "Ocean misses\n   you."
};

static void minesweeper_menu_scene_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);

    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void minesweeper_scene_menu_on_enter(void* context) {
    furi_assert(context);
    App* app = (App*)context;

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperLoadingView);
    
    dialog_ex_set_context(app->menu, app);

    dialog_ex_set_header(app->menu, "Leave Minesweeper?", 64, 4, AlignCenter, AlignTop);

    uint32_t prompt_index = furi_hal_random_get() % sizeof(farewellPrompts) / sizeof(char*);

    dialog_ex_set_text(app->menu, farewellPrompts[prompt_index], 65, 20, AlignLeft, AlignTop);

    dialog_ex_set_icon(app->menu, 11, 13, &I_Cry_dolph_55x52);

    dialog_ex_set_left_button_text(app->menu, "Back");

    dialog_ex_set_right_button_text(app->menu, "Exit");

    dialog_ex_set_result_callback(app->menu, minesweeper_menu_scene_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperMenuView);
}

bool minesweeper_scene_menu_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    App* app = context; 
    bool consumed = false;
    
    if (event.type == SceneManagerEventTypeCustom) {
        switch (event.event) {

            case DialogExResultLeft :
                if (!scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, MineSweeperSceneStartScreen)) {

                    scene_manager_stop(app->scene_manager);
                    view_dispatcher_stop(app->view_dispatcher);
                }

                consumed = true;
                break;

            case DialogExResultRight : 
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
                consumed = true;
                break;

            default :
                break;
        }
    }

    return consumed;
}

void minesweeper_scene_menu_on_exit(void* context) {
    furi_assert(context);
    App* app = (App*)context;

    dialog_ex_reset(app->menu);
}
