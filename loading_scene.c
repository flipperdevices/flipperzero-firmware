#include "loading_scene.h"

// loading scene manager callbacks
void spotify_remote_loading_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_LOADING_VIEW);
}

bool spotify_remote_loading_scene_on_event(void* context, SceneManagerEvent event) {
    SpotifyRemoteApp* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SPOTIFY_REMOTE_ON_RECEIVE_EVENT:
            if(furi_string_get_cstr(app->message_recieved)[0] == 'I' &&
               furi_string_get_cstr(app->message_recieved)[1] == 'P') {
                scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_IP_SCENE);
                consumed = true;
            }
            break;
        }
    default:
        break;
    }

    return consumed;
}

void spotify_remote_loading_scene_on_exit(void* context) {
    UNUSED(context);
}