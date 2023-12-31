#include "../minesweeper.h"
#include "../views/start_screen.h"

typedef enum {
    MineSweeperSceneStartScreenContinueEvent,
} MineSweeperSceneStartScreenEvent;

//void minesweeper_scene_start_screen_timer_callback(void* context) {
//    furi_assert(context);
//    UNUSED(context);
//}

bool minesweeper_scene_start_screen_input_callback(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    App* app = context;
    bool consumed = false;

    // right now we continue if back is not pressed
    if (event->key != InputKeyBack) {
       return scene_manager_handle_custom_event(app->scene_manager, MineSweeperSceneStartScreenContinueEvent); 
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
    App* app = context;
    
    furi_assert(app->start_screen);

    start_screen_set_context(app->start_screen, app);

    // Set callbacks
    //start_screen_set_timer_callback(app->start_screen, minesweeper_scene_start_screen_timer_callback);
    start_screen_set_input_callback(
            app->start_screen,
            minesweeper_scene_start_screen_input_callback);
    start_screen_set_secondary_draw_callback(
            app->start_screen,
            minesweeper_scene_start_screen_secondary_draw_callback);
    

    // Right now Icon is hardcoded, need to implmement generic version;
    //start_screen_set_icon_animation(app->start_screen, 0, 0);

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperStartScreenView);
}

bool minesweeper_scene_start_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    App* app = context;
    bool consumed = false;

    //if (event.type == SceneManagerEventTypeBack) {
    //    //exit app
    //    scene_manager_stop(app->scene_manager);
    //    view_dispatcher_stop(app->view_dispatcher);
    //    consumed = true;
    //}
    if (event.type == SceneManagerEventTypeCustom && event.event == MineSweeperSceneStartScreenContinueEvent) {
        scene_manager_next_scene(app->scene_manager, MineSweeperSceneMenu); 
        consumed = true;
    }

    return consumed;
}

void minesweeper_scene_start_screen_on_exit(void* context) {
    furi_assert(context);

    App* app = context;
    start_screen_reset(app->start_screen);
}
