#include "../minesweeper.h"
#include "../views/minesweeper_game_screen.h"

#include <input/input.h>

typedef enum {
    MineSweeperGameScreenContinueEvent,
} MineSweeperGameScreenEvent;

bool mineweeper_scene_game_screen_input_callback(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    
    MineSweeperApp* app = context;

    bool consumed = false;

    if (event->type == InputTypePress) {
        consumed = scene_manager_handle_custom_event(app->scene_manager, MineSweeperGameScreenContinueEvent);
    }

    return consumed;
}

void minesweeper_scene_game_screen_on_enter(void* context) {
    furi_assert(context);
    MineSweeperApp* app = context;
    
    furi_assert(app->game_screen);

    mine_sweeper_game_screen_set_context(app->game_screen, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperGameScreenView);
}

bool minesweeper_scene_game_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    MineSweeperApp* app = context;
    bool consumed = false;

    if (event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(app->scene_manager, MineSweeperSceneMenu);
        consumed = true;
    }

    return consumed;
}

void minesweeper_scene_game_screen_on_exit(void* context) {
    furi_assert(context);
    MineSweeperApp* app = context;

    UNUSED(app);
}
