#include "../example_number_input_i.h"

void example_number_input_scene_input_number_callback(void* context) {
    ExampleNumberInput* app = context;
    view_dispatcher_send_custom_event(
        app->view_dispatcher, ExampleNumberInputCustomEventTextInput);
}

void example_number_input_scene_input_number_on_enter(void* context) {
    furi_assert(context);
    ExampleNumberInput* app = context;
    NumberInput* number_input = app->number_input;
    size_t enter_name_length = 5;
    char* str = "Enter a number (0 - 9999)";
    const char* constStr = str;
    UNUSED(constStr);
    UNUSED(enter_name_length);
    number_input_set_header_text(number_input, constStr);
    number_input_set_result_callback(
        number_input,
        example_number_input_scene_input_number_callback,
        context,
        app->text_buffer,
        //app->text_store[0],
        enter_name_length,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNumberInputViewIdNumberInput);
    //view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNumberInputViewIdShowNumber);
}

bool example_number_input_scene_input_number_on_event(void* context, SceneManagerEvent event) {
    ExampleNumberInput* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    }
    return consumed;
}

void example_number_input_scene_input_number_on_exit(void* context) {
    ExampleNumberInput* app = context;
    UNUSED(app);
}