#include "../minesweeper.h"

typedef enum {
    MineSweeperSceneStartScreenExitEvent,
    MineSweeperSceneStartScreenContinueEvent,
} MineSweeperSceneStartScreenEvent;

bool minesweeper_scene_start_screen_input_callback(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    MineSweeperApp* app = context;
    bool consumed = false;

    // Right now we continue if back is not pressed
    if(event->key == InputKeyBack) {
        consumed = scene_manager_handle_custom_event(
            app->scene_manager, MineSweeperSceneStartScreenExitEvent);
    } else {
        consumed = scene_manager_handle_custom_event(
            app->scene_manager, MineSweeperSceneStartScreenContinueEvent);
    }

    return consumed;
}

void minesweeper_scene_start_screen_secondary_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);
    UNUSED(_model);
    UNUSED(canvas);
}

void minesweeper_scene_start_screen_on_enter(void* context) {
    furi_assert(context);
    MineSweeperApp* app = context;

    furi_assert(app->start_screen);

    start_screen_set_context(app->start_screen, app);

    start_screen_set_input_callback(
        app->start_screen, minesweeper_scene_start_screen_input_callback);

    start_screen_set_secondary_draw_callback(
        app->start_screen, minesweeper_scene_start_screen_secondary_draw_callback);

    start_screen_set_icon_animation(app->start_screen, 0, 0, &A_StartScreen_128x64);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperStartScreenView);
}

bool minesweeper_scene_start_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    MineSweeperApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MineSweeperSceneStartScreenContinueEvent) {
            mine_sweeper_game_screen_reset_clock(app->game_screen);
            scene_manager_next_scene(app->scene_manager, MineSweeperSceneGameScreen);
            consumed = true;
        } else if(event.event == MineSweeperSceneStartScreenExitEvent) {
            // Exit app
            scene_manager_stop(app->scene_manager);
            view_dispatcher_stop(app->view_dispatcher);
            consumed = true;
        }
    }

    return consumed;
}

void minesweeper_scene_start_screen_on_exit(void* context) {
    furi_assert(context);

    MineSweeperApp* app = context;
    start_screen_reset(app->start_screen);
}
