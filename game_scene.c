#include "minesweeper.h" 

static void reset_app_state(AppState* app_state) {
    memset(app_state, 0, sizeof(*app_state));
}

// Custom callbacks for view attached to EmptyScreen
static void minesweeper_empty_screen_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(canvas);
    furi_assert(context);
    
    //App* app = (App*)context;
}

static bool minesweeper_empty_screen_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    furi_assert(event);

    App* app = (App*)context;
    
    MineSweeperGameKeyEvent minesweeper_game_custom_input_event = MineSweeperGameKeyUpShortEvent; 

    if (event->type == InputTypeLong) {
        minesweeper_game_custom_input_event = MineSweeperGameKeyUpLongEvent;
    }

    minesweeper_game_custom_input_event += event->key;
    
    return scene_manager_handle_custom_event(app->scene_manager, minesweeper_game_custom_input_event);
}

void minesweeper_scenes_game_on_enter(void* context) {
    furi_assert(context);
    App* app = (App*)context;

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperLoadingView); 

    reset_app_state(app->app_state);

    app->empty_screen_view = empty_screen_get_view(app->empty_screen);
    view_set_context(app->empty_screen_view, app);
    view_set_draw_callback(app->empty_screen_view, minesweeper_empty_screen_view_draw_callback);
    view_set_input_callback(app->empty_screen_view, minesweeper_empty_screen_view_input_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperEmptyScreenView);
}

bool minesweeper_scenes_game_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    App* app = (App*)context;
    bool consumed = false;

    switch(event.type) {
        case SceneManagerEventTypeCustom :
            switch(event.event) {
                case MineSweeperGameKeyUpShortEvent :
                case MineSweeperGameKeyDownShortEvent :
                case MineSweeperGameKeyRightShortEvent :
                case MineSweeperGameKeyLeftShortEvent :
                case MineSweeperGameKeyOkShortEvent :
                case MineSweeperGameKeyBackShortEvent :
                
                case MineSweeperGameKeyUpLongEvent : 
                case MineSweeperGameKeyDownLongEvent :
                case MineSweeperGameKeyRightLongEvent :
                case MineSweeperGameKeyLeftLongEvent :
                case MineSweeperGameKeyOkLongEvent :
                    consumed = true;
                    break;
                case MineSweeperGameKeyBackLongEvent : // Reset Game to startup screen
                    scene_manager_search_and_switch_to_another_scene(app->scene_manager, MineSweeperStartupScene);
                    consumed = true;
                    break;
                default :
                    break;
            }
            break;
        default:
            break;
    }

    return consumed;
 }

void minesweeper_scenes_game_on_exit(void* context) {
    furi_assert(context);
    App* app = (App*)context;
    UNUSED(app);
}

