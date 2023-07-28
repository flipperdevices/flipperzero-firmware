#include "../esubghz_chat_i.h"

/* Sends FreqEntered event to scene manager and displays the frequency in the
 * text box. */
static void freq_input_cb(void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    furi_string_cat_printf(state->chat_box_store, "Frequency: %lu", state->frequency);

    scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_FreqEntered);
}

/* Validates the entered frequency. */
static bool freq_input_validator(const char* text, FuriString* error, void* context) {
    furi_assert(text);
    furi_assert(error);

    furi_assert(context);
    ESubGhzChatState* state = context;

    int ret = sscanf(text, "%lu", &(state->frequency));
    if(ret != 1) {
        furi_string_printf(error, "Please enter\nfrequency\nin Hz!");
        return false;
    }

    if(!subghz_devices_is_frequency_valid(state->subghz_device, state->frequency)) {
        furi_string_printf(error, "Frequency\n%lu\n is invalid!", state->frequency);
        return false;
    }

#ifdef FW_ORIGIN_Official
    if(!furi_hal_region_is_frequency_allowed(state->frequency)) {
#else /* FW_ORIGIN_Official */
    if(!furi_hal_subghz_is_tx_allowed(state->frequency)) {
#endif /* FW_ORIGIN_Official */
        furi_string_printf(error, "TX forbidden\non frequency\n%lu!", state->frequency);
        return false;
    }

    return true;
}

/* Prepares the frequency input scene. */
void scene_on_enter_freq_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_freq_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    snprintf(state->text_input_store, TEXT_INPUT_STORE_SIZE, "%lu", (uint32_t)DEFAULT_FREQ);
    text_input_reset(state->text_input);
    text_input_set_result_callback(
        state->text_input,
        freq_input_cb,
        state,
        state->text_input_store,
        sizeof(state->text_input_store),
        true);
    text_input_set_validator(state->text_input, freq_input_validator, state);
    text_input_set_header_text(state->text_input, "Frequency");

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

/* Handles scene manager events for the frequency input scene. */
bool scene_on_event_freq_input(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_freq_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to password input scene */
        case ESubGhzChatEvent_FreqEntered:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_KeyMenu);
            consumed = true;
            break;
        }
        break;

    case SceneManagerEventTypeBack:
        /* stop the application if the user presses back here */
        view_dispatcher_stop(state->view_dispatcher);
        consumed = true;
        break;

    default:
        consumed = false;
        break;
    }

    return consumed;
}

/* Cleans up the frequency input scene. */
void scene_on_exit_freq_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_freq_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_input_reset(state->text_input);
}
