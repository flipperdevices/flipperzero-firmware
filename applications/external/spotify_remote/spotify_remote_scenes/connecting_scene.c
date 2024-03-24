#include "connecting_scene.h"

/**
 * On enter handler for the connecting scene 
 * 
 * @param context the app object
*/
void spotify_remote_connecting_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;

    text_box_reset(app->text_box);

    text_box_set_text(app->text_box, "Connecting to wifi...");
    text_box_set_font(app->text_box, TextBoxFontText);

    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_TEXT_BOX_VIEW);
}

/**
 * On event handler for the connecting scene
 * 
 * @param context the app object
 * @param event the event to handle
*/
bool spotify_remote_connecting_scene_on_event(void* context, SceneManagerEvent event) {
    SpotifyRemoteApp* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SPOTIFY_REMOTE_ON_RECEIVE_EVENT: 
            // if we receive a message from the flipper prefixed with 'IP' we move to the ip scene
            if(furi_string_get_cstr(app->message_recieved)[0] == 'H' &&
               furi_string_get_cstr(app->message_recieved)[1] == 'I') {
                uart_helper_send(app->uart_helper, "7\n", 2);
            } else if(furi_string_get_cstr(app->message_recieved)[0] == 'I' &&
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

/**
 * On exit handler for the connecting scene
 * 
 * @param context the app object
*/
void spotify_remote_connecting_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    text_box_reset(app->text_box);
}