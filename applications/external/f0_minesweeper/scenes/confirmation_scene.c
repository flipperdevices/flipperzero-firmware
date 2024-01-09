#include "../minesweeper.h"

static void confirmation_scene_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);

    MineSweeperApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void minesweeper_scene_confirmation_screen_on_enter(void* context) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperLoadingView);

    dialog_ex_set_context(app->confirmation_screen, app);

    dialog_ex_set_header(
        app->confirmation_screen, "Save Settings?", 128 / 2, 4, AlignCenter, AlignTop);

    dialog_ex_set_text(
        app->confirmation_screen,
        "Warning: Saving will reset\nthe game with the\nselected settings.",
        128 / 2,
        64 / 2,
        AlignCenter,
        AlignCenter);

    dialog_ex_set_left_button_text(app->confirmation_screen, "Back");

    dialog_ex_set_center_button_text(app->confirmation_screen, "Cancel");

    dialog_ex_set_right_button_text(app->confirmation_screen, "Save");

    dialog_ex_set_result_callback(app->confirmation_screen, confirmation_scene_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperConfirmationView);
}

bool minesweeper_scene_confirmation_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    MineSweeperApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultLeft:
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, MineSweeperSceneSettingsScreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            break;

        case DialogExResultRight:

            view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperLoadingView);

            // Commit changes to actual buffer for settings data
            app->settings_info.board_width = app->t_settings_info.board_width;
            app->settings_info.board_height = app->t_settings_info.board_height;
            app->settings_info.difficulty = app->t_settings_info.difficulty;
            app->settings_info.ensure_solvable_board = app->t_settings_info.ensure_solvable_board;

            mine_sweeper_save_settings(app);

            // This is used to let the settings view know it can save the main settings_info
            // to the temp one on the next on enter
            app->is_settings_changed = false;

            // Reset the game board
            mine_sweeper_game_screen_reset(
                app->game_screen,
                app->settings_info.board_width,
                app->settings_info.board_height,
                app->settings_info.difficulty,
                app->settings_info.ensure_solvable_board);

            // Go to reset game view
            scene_manager_search_and_switch_to_another_scene(
                app->scene_manager, MineSweeperSceneGameScreen);
            break;

        case DialogExResultCenter:
            // Do not commit changes to actual buffer on cancel

            app->is_settings_changed = false;

            // we want to just switch back to the game screen without resetting
            scene_manager_search_and_switch_to_another_scene(
                app->scene_manager, MineSweeperSceneGameScreen);

            break;

        default:
            break;
        }
        consumed = true;
    }

    return consumed;
}

void minesweeper_scene_confirmation_screen_on_exit(void* context) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;

    dialog_ex_reset(app->confirmation_screen);
}
