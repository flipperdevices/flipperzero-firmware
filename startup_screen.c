#include "minesweeper.h"

// Custom event enumeration
typedef enum {
     MineSweeperStartEvent,
     MineSweeperBackEvent,
} MineSweeperStartupEvent;


static void minesweeper_scenes_popup_callback(void* context) {
    furi_assert(context);
    App* app = context;

    scene_manager_handle_custom_event(app->scene_manager, MineSweeperStartEvent);
}

void minesweeper_scenes_startup_scene_on_enter(void* context) {
    App* app = (App*)context;

    furi_assert(app);

    // fake loading
    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperLoadingView);

    popup_reset(app->popup);
    
    popup_set_context(app->popup, app);

    popup_set_callback(app->popup, minesweeper_scenes_popup_callback);

    app->popup_view = popup_get_view(app->popup);

    popup_set_header(app->popup, "MINE\n    Sweeper", 50, 25, AlignLeft, AlignCenter);

    //popup_set_text(app->popup, "Sweeper", 70, 35, AlignLeft, AlignCenter);

    popup_set_icon(app->popup, 5, 32-18, &I_MineSweeperIcon_36x36);
    
    for (int i = 0; i < 10000; ++i) {}

    view_dispatcher_switch_to_view(app->view_dispatcher, MineSweeperPopupView);
}

bool minesweeper_scenes_startup_scene_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    App* app = (App*)context;

    bool consumed = false;

    switch (event.type) {
        case SceneManagerEventTypeCustom :
            switch (event.event) {
                case MineSweeperStartEvent :
                    scene_manager_next_scene(app->scene_manager, MineSweeperGameScene); 
                    consumed = true;
                    break;
                default:
                    break;
            }
            break;
        default :
            break;
    }

    return consumed;
}

void minesweeper_scenes_startup_scene_on_exit(void* context) {
    furi_assert(context);
    App* app = context;
    app->popup_view = NULL;
}
