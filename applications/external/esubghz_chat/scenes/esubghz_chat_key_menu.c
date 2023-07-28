#include "../esubghz_chat_i.h"

typedef enum {
    ESubGhzChatKeyMenuItems_NoEncryption,
    ESubGhzChatKeyMenuItems_Password,
} ESubGhzChatKeyMenuItems;

static void key_menu_cb(void* context, uint32_t index) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    switch(index) {
    case ESubGhzChatKeyMenuItems_NoEncryption:
        state->encrypted = false;
        enter_chat(state);

        scene_manager_handle_custom_event(
            state->scene_manager, ESubGhzChatEvent_KeyMenuNoEncryption);
        break;

    case ESubGhzChatKeyMenuItems_Password:
        scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_KeyMenuPassword);
        break;

    default:
        break;
    }
}

/* Prepares the key menu scene. */
void scene_on_enter_key_menu(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_key_menu");

    furi_assert(context);
    ESubGhzChatState* state = context;

    menu_reset(state->menu);

    menu_add_item(
        state->menu,
        "No encryption",
        NULL,
        ESubGhzChatKeyMenuItems_NoEncryption,
        key_menu_cb,
        state);
    menu_add_item(
        state->menu, "Password", NULL, ESubGhzChatKeyMenuItems_Password, key_menu_cb, state);

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Menu);
}

/* Handles scene manager events for the key menu scene. */
bool scene_on_event_key_menu(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_key_menu");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to message input scene */
        case ESubGhzChatEvent_KeyMenuNoEncryption:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_ChatInput);
            consumed = true;
            break;
        /* switch to password input scene */
        case ESubGhzChatEvent_KeyMenuPassword:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_PassInput);
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

/* Cleans up the key menu scene. */
void scene_on_exit_key_menu(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_key_menu");

    furi_assert(context);
    ESubGhzChatState* state = context;

    menu_reset(state->menu);
}
