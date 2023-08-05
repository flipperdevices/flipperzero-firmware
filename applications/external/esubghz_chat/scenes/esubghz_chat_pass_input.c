#include "../esubghz_chat_i.h"

/* Sends PassEntered event to scene manager and enters the chat. */
static void pass_input_cb(void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    crypto_explicit_bzero(state->text_input_store, sizeof(state->text_input_store));

    enter_chat(state);

    view_dispatcher_send_custom_event(state->view_dispatcher, ESubGhzChatEvent_PassEntered);
}

/* If a password was entered this derives a key from the password using a
 * single pass of SHA256 and initiates the AES-GCM context for encryption. If
 * the initiation fails, the password is rejected. */
static bool pass_input_validator(const char* text, FuriString* error, void* context) {
    furi_assert(text);
    furi_assert(error);

    furi_assert(context);
    ESubGhzChatState* state = context;

    if(strlen(text) == 0) {
        furi_string_printf(error, "Enter a\npassword!");
        return false;
    }

    unsigned char key[KEY_BITS / 8];

    /* derive a key from the password */
    sha256((unsigned char*)text, strlen(text), key);

    /* initiate the crypto context */
    bool ret = crypto_ctx_set_key(state->crypto_ctx, key, state->name_prefix, furi_get_tick());

    /* cleanup */
    crypto_explicit_bzero(key, sizeof(key));

    if(!ret) {
        crypto_ctx_clear(state->crypto_ctx);
        furi_string_printf(error, "Failed to\nset key!");
        return false;
    }

    state->encrypted = true;

    return true;
}

/* Prepares the password input scene. */
void scene_on_enter_pass_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_pass_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    state->text_input_store[0] = 0;
    text_input_reset(state->text_input);
    text_input_set_result_callback(
        state->text_input,
        pass_input_cb,
        state,
        state->text_input_store,
        sizeof(state->text_input_store),
        true);
    text_input_set_validator(state->text_input, pass_input_validator, state);
    text_input_set_header_text(state->text_input, "Password");

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

/* Handles scene manager events for the password input scene. */
bool scene_on_event_pass_input(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_pass_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to message input scene */
        case ESubGhzChatEvent_PassEntered:
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

/* Cleans up the password input scene. */
void scene_on_exit_pass_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_pass_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_input_reset(state->text_input);
    crypto_explicit_bzero(state->text_input_store, sizeof(state->text_input_store));
}
