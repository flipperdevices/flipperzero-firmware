#include "../esubghz_chat_i.h"

void key_display_result_cb(DialogExResult result, void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    switch(result) {
    case DialogExResultLeft:
        scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_KeyDisplayBack);
        break;

    default:
        break;
    }
}

/* Prepares the key display scene. */
void scene_on_enter_key_display(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_key_display");

    furi_assert(context);
    ESubGhzChatState* state = context;

    if(state->encrypted) {
        uint8_t key[KEY_BITS / 8];
        crypto_ctx_get_key(state->crypto_ctx, key);
        snprintf(
            state->key_hex_str,
            KEY_HEX_STR_SIZE,
            "%02hX%02hX%02hX%02hX"
            "%02hX%02hX%02hX%02hX\n"
            "%02hX%02hX%02hX%02hX"
            "%02hX%02hX%02hX%02hX\n"
            "%02hX%02hX%02hX%02hX"
            "%02hX%02hX%02hX%02hX\n"
            "%02hX%02hX%02hX%02hX"
            "%02hX%02hX%02hX%02hX",
            key[0],
            key[1],
            key[2],
            key[3],
            key[4],
            key[5],
            key[6],
            key[7],
            key[8],
            key[9],
            key[10],
            key[11],
            key[12],
            key[13],
            key[14],
            key[15],
            key[16],
            key[17],
            key[18],
            key[19],
            key[20],
            key[21],
            key[22],
            key[23],
            key[24],
            key[25],
            key[26],
            key[27],
            key[28],
            key[29],
            key[30],
            key[31]);
        crypto_explicit_bzero(key, sizeof(key));
    } else {
        strcpy(state->key_hex_str, "No Key");
    }

    dialog_ex_reset(state->key_display);

    dialog_ex_set_text(state->key_display, state->key_hex_str, 64, 2, AlignCenter, AlignTop);

    dialog_ex_set_icon(state->key_display, 0, 0, NULL);

    dialog_ex_set_left_button_text(state->key_display, "Back");

    dialog_ex_set_result_callback(state->key_display, key_display_result_cb);
    dialog_ex_set_context(state->key_display, state);

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_KeyDisplay);
}

/* Handles scene manager events for the key display scene. */
bool scene_on_event_key_display(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_key_display");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to message input scene */
        case ESubGhzChatEvent_KeyDisplayBack:
            if(!scene_manager_previous_scene(state->scene_manager)) {
                view_dispatcher_stop(state->view_dispatcher);
            }
            consumed = true;
            break;
        }
        break;

    default:
        consumed = false;
        break;
    }

    return consumed;
}

/* Cleans up the key display scene. */
void scene_on_exit_key_display(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_key_display");

    furi_assert(context);
    ESubGhzChatState* state = context;

    dialog_ex_reset(state->key_display);
    crypto_explicit_bzero(state->key_hex_str, sizeof(state->key_hex_str));
}
