#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_add_code.h"
#include <furi_hal_cortex.h>

bool add_code_text_input_validator(const char* text, FuriString* error, void* context) {
    UNUSED(context);
    int len = strlen(text);
    // TODO: Add code validation
    // First character is YVXC
    // Second is 012
    // Dash
    // Group of hex (with possible additional @ or ^ characters preceeding each hex digit)
    // repeat
    // Perhaps length of groups based on code type?
    if(len < 4) {
        furi_string_printf(error, "Code is invalid.");
        return false;
    }
    return true;
}

void add_code_text_input_callback(void* context) {
    App* app = context;
    FURI_LOG_I(TAG, "save_text_input_callback %s", app->state->result_code);
    scene_manager_handle_custom_event(app->scene_manager, SaveCodeInputRead);
}

void fcom_add_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_enter");
    App* app = context;

    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter Code");
    text_input_set_validator(app->text_input, add_code_text_input_validator, NULL);
    text_input_set_result_callback(
        app->text_input,
        add_code_text_input_callback,
        app,
        app->state->result_code,
        MAX_DIGIROM_LEN,
        true);
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomKeyboardView);
}

bool fcom_add_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_event");
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SaveCodeInputRead:
            app->state->save_code_return_scene = FcomMainMenuScene;
            scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
            consumed = true;
            break;
        }
        break;
    default: // eg. SceneManagerEventTypeBack, SceneManagerEventTypeTick
        consumed = false;
        break;
    }
    return consumed;
}

void fcom_add_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_exit");
    UNUSED(context);
}
