#include "../esubghz_chat_i.h"

/* Sets the entered bytes as the key, enters the chat and sends a HexKeyEntered
 * event to the scene manager. */
static void hex_key_input_cb(void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    /* initiate the crypto context */
    bool ret = crypto_ctx_set_key(state->crypto_ctx, state->hex_key_input_store);

    /* cleanup */
    crypto_explicit_bzero(state->hex_key_input_store, sizeof(state->hex_key_input_store));

    if(!ret) {
        crypto_ctx_clear(state->crypto_ctx);
        return;
    }

    state->encrypted = true;

    enter_chat(state);

    scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_HexKeyEntered);
}

/* Prepares the hex key input scene. */
void scene_on_enter_hex_key_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_hex_key_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    byte_input_set_result_callback(
        state->hex_key_input,
        hex_key_input_cb,
        NULL,
        state,
        state->hex_key_input_store,
        sizeof(state->hex_key_input_store));

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_HexKeyInput);
}

/* Handles scene manager events for the hex key input scene. */
bool scene_on_event_hex_key_input(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_hex_key_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to message input scene */
        case ESubGhzChatEvent_HexKeyEntered:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_ChatInput);
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

/* Cleans up the hex key input scene. */
void scene_on_exit_hex_key_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_hex_key_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    crypto_explicit_bzero(state->hex_key_input_store, sizeof(state->hex_key_input_store));
}
