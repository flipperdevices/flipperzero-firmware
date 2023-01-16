#include "../brainfuck_i.h"

void set_input_text_input_callback(void* context) {
    BFApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, brainfuckCustomEventTextInputDone);
}

char tmpBuffer[64] = {};
void brainfuck_scene_set_input_on_enter(void* context) {
    BFApp* app = context;
    TextInput* text_input = app->text_input;

    text_input_set_header_text(text_input, "Edit input buffer");
    text_input_set_result_callback(
        text_input,
        set_input_text_input_callback,
        app,
        tmpBuffer,
        64,
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, brainfuckViewTextInput);
}

bool brainfuck_scene_set_input_on_event(void* context, SceneManagerEvent event) {
    BFApp* app = context;
    UNUSED(app);
    
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == brainfuckCustomEventTextInputDone) {
            memcpy(app->inputBuffer, tmpBuffer, 64);
            scene_manager_next_scene(app->scene_manager, brainfuckSceneDevEnv);
        }
    }
    return consumed;
}

void brainfuck_scene_set_input_on_exit(void* context) {
    UNUSED(context);
}
