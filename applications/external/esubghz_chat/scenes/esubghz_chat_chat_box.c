#include "../esubghz_chat_i.h"

/* Prepares the text box scene. */
void scene_on_enter_chat_box(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_chat_box");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_box_reset(state->chat_box);
    text_box_set_text(state->chat_box, furi_string_get_cstr(state->chat_box_store));
    text_box_set_focus(state->chat_box, TextBoxFocusEnd);

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_ChatBox);
}

/* Handles scene manager events for the text box scene. */
bool scene_on_event_chat_box(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_chat_box");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to message input scene */
        case ESubGhzChatEvent_GotoMsgInput:
            if(!scene_manager_previous_scene(state->scene_manager)) {
                view_dispatcher_stop(state->view_dispatcher);
            }
            consumed = true;
            break;
        case ESubGhzChatEvent_GotoKeyDisplay:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_KeyDisplay);
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

/* Cleans up the text box scene. */
void scene_on_exit_chat_box(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_chat_box");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_box_reset(state->chat_box);
}
