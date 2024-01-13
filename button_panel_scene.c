#include "button_panel_scene.h"

// called when selection is made in button panel
void button_panel_on_select_callback(void* context, uint32_t index) {
    SpotifyRemoteApp* app = context;
    switch(index) {
    case PREV:
        uart_helper_send(app->uart_helper, "1\n", 2);
        break;
    case NEXT:
        uart_helper_send(app->uart_helper, "2\n", 2);
        break;
    case PLAY:
        uart_helper_send(app->uart_helper, "3\n", 2);
        break;
    case PAUSE:
        uart_helper_send(app->uart_helper, "4\n", 2);
        break;
    case SHUFFLE:
        uart_helper_send(app->uart_helper, "5\n", 2);
        break;
    case REPEAT:
        uart_helper_send(app->uart_helper, "6\n", 2);
        break;
    default:
        break;
    }
}

// button panel callbacks
void spotify_remote_button_panel_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;

    button_panel_reserve(app->button_panel, 2, 3);
    button_panel_add_item(
        app->button_panel,
        PREV,
        0,
        0,
        6,
        16,
        &I_prev_19x20,
        &I_prev_hover_19x20,
        button_panel_on_select_callback,
        context);
    button_panel_add_icon(app->button_panel, 6, 38, &I_prev_text_19x5);
    button_panel_add_item(
        app->button_panel,
        NEXT,
        1,
        0,
        39,
        16,
        &I_next_19x20,
        &I_next_hover_19x20,
        button_panel_on_select_callback,
        context);
    button_panel_add_icon(app->button_panel, 39, 38, &I_next_text_19x6);

    button_panel_add_item(
        app->button_panel,
        PLAY,
        0,
        1,
        6,
        50,
        &I_play_19x20,
        &I_play_hover_19x20,
        button_panel_on_select_callback,
        context);
    button_panel_add_icon(app->button_panel, 6, 72, &I_play_text_19x5);

    button_panel_add_item(
        app->button_panel,
        PAUSE,
        1,
        1,
        39,
        50,
        &I_pause_19x20,
        &I_pause_hover_19x20,
        button_panel_on_select_callback,
        context);
    button_panel_add_icon(app->button_panel, 39, 72, &I_pause_text_23x5);

    button_panel_add_item(
        app->button_panel,
        SHUFFLE,
        0,
        2,
        6,
        84,
        &I_shuffle_19x20,
        &I_shuffle_hover_19x20,
        button_panel_on_select_callback,
        context);
    button_panel_add_icon(app->button_panel, 5, 106, &I_shuffle_text_21x5);

    button_panel_add_item(
        app->button_panel,
        REPEAT,
        1,
        2,
        39,
        84,
        &I_repeat_19x20,
        &I_repeat_hover_19x20,
        button_panel_on_select_callback,
        context);
    button_panel_add_icon(app->button_panel, 40, 106, &I_repeat_text_18x5);

    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_BUTTON_PANEL_VIEW);
}

bool spotify_remote_button_panel_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return false;
}

void spotify_remote_button_panel_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    button_panel_reset(app->button_panel);
}