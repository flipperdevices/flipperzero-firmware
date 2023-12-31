#include "../minesweeper.h"

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

    dialog_ex_set_header(app->menu, "Leave to start menu?", 64, 5, AlignCenter, AlignTop);

    dialog_ex_set_icon(app->menu, 21, 13, &I_Cry_dolph_55x52);

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
                //if (!scene_manager_previous_scene(app->scene_manager)) { // Exit in the case of no prev scene
                //    scene_manager_stop(app->scene_manager);
                //    view_dispatcher_stop(app->view_dispatcher);
                //    consumed = true;
                //} else {
                //    // We are dereferencing a null ptr when this happens
                    consumed = scene_manager_previous_scene(app->scene_manager);
                //}
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
