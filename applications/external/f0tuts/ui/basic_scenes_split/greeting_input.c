#include "basic_scenes_split.h"

typedef enum {
    BasicScenesGreetingInputSceneSaveEvent,
} BasicScenesGreetingInputEvent;

static bool textInputValidator(const char* text, FuriString* error, void* context) {
    UNUSED(context);
    if(strlen(text) < 3) {
        furi_string_set(
            error, "Enter at least\n3 characters\nfor name!");
        return false;
    }

    return true;
}

static void basic_scenes_text_input_callback(void* context) {
    App* app = context;
    scene_manager_handle_custom_event(app->scene_manager, BasicScenesGreetingInputSceneSaveEvent);
}

void basic_scenes_greeting_input_scene_on_enter(void* context) {
    App* app = context;
    bool clear_text = true;
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter your name");
    text_input_set_validator(app->text_input, textInputValidator, app);
    text_input_set_result_callback(
        app->text_input,
        basic_scenes_text_input_callback,
        app,
        app->user_name,
        app->user_name_size,
        clear_text);
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesTextInputView);
}

bool basic_scenes_greeting_input_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BasicScenesGreetingInputSceneSaveEvent) {
            scene_manager_next_scene(app->scene_manager, BasicScenesGreetingMessageScene);
            consumed = true;
        }
    }
    return consumed;
}

void basic_scenes_greeting_input_scene_on_exit(void* context) {
    UNUSED(context);
}