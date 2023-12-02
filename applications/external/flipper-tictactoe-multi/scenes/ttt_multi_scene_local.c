#include "../ttt_multi.h"

static void ttt_multi_scene_local_callback(void* context, TttMultiCustomEvent event) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;

    view_dispatcher_send_custom_event(ttt_multi->view_dispatcher, event);
}

void ttt_multi_scene_local_on_enter(void* context) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;
    TttMultiGameView* view = ttt_multi->game_view;

    ttt_multi_game_view_set_callback(view, ttt_multi_scene_local_callback, ttt_multi);
    ttt_multi_game_view_set_local_play(view);
    view_dispatcher_switch_to_view(ttt_multi->view_dispatcher, TttMultiViewGame);
}

void ttt_multi_scene_local_on_exit(void* context) {
    furi_assert(context);
    TttMultiApp* ttt_multi = context;

    ttt_multi_game_view_reset(ttt_multi->game_view);
}

bool ttt_multi_scene_local_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    TttMultiApp* ttt_multi = context;

    if(event.type == SceneManagerEventTypeCustom) {
        TttMultiCustomEvent custom_event = event.event;
        switch(custom_event) {
        case TttMultiCustomEventGameMove: {
            TttMultiGameMove last_move = {};
            ttt_multi_game_view_get_last_move(ttt_multi->game_view, &last_move);
            ttt_multi_game_view_move(ttt_multi->game_view, &last_move);
            return true;
        }
        case TttMultiCustomEventGameFinish: {
            TttMultiGameResult result = ttt_multi_game_view_get_result(ttt_multi->game_view);
            if(result == TttMultiGameResultDraw) {
                notification_message(ttt_multi->notifications, &sequence_error);
            } else {
                notification_message(ttt_multi->notifications, &sequence_success);
            }
            return true;
        }
        default:
            break;
        }
    }

    return false;
}