#include "minesweeper.h" 

typedef enum {
    None,
} MineSweeperGameEvent;

static void init_app_state(App* app) {
    memset(app->app_state, 0, sizeof(*app->app_state));
}

void minesweeper_scenes_game_on_enter(void* context) {
    App* app = (App*)context;

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperLoadingView); 

    init_app_state(app);

    app->empty_screen_view = empty_screen_get_view(app->empty_screen);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperEmptyScreenView);
}

bool minesweeper_scenes_game_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    bool consumed = false;
    return consumed;
 }

void minesweeper_scenes_game_on_exit(void* context) {
    UNUSED(context);
}

