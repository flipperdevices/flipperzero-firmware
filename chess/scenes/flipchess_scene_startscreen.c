#include "../flipchess.h"
#include "../helpers/flipchess_voice.h"
#include "../helpers/flipchess_file.h"
#include "../helpers/flipchess_custom_event.h"
#include "../views/flipchess_startscreen.h"

void flipchess_scene_startscreen_callback(FlipChessCustomEvent event, void* context) {
    furi_assert(context);
    FlipChess* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flipchess_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    FlipChess* app = context;

    if(flipchess_has_file(FlipChessFileBoard, NULL, false)) {
        if(flipchess_load_file(app->import_game_text, FlipChessFileBoard, NULL)) {
            app->import_game = 1;
        }
    }

    flipchess_startscreen_set_callback(
        app->flipchess_startscreen, flipchess_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipChessViewIdStartscreen);
}

bool flipchess_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    FlipChess* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipChessCustomEventStartscreenLeft:
        case FlipChessCustomEventStartscreenRight:
            break;
        case FlipChessCustomEventStartscreenUp:
        case FlipChessCustomEventStartscreenDown:
            break;
        case FlipChessCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, FlipChessSceneMenu);
            consumed = true;
            break;
        case FlipChessCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, FlipChessSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void flipchess_scene_startscreen_on_exit(void* context) {
    FlipChess* app = context;

    if(app->sound == 1) {
        flipchess_voice_shall_we_play();
    }
}