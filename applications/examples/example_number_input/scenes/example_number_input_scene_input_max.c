#include "../example_number_input_i.h"

void example_number_input_scene_input_max_callback(void* context, int32_t number) {
    ExampleNumberInput* app = context;
    app->max_value = number;
    view_dispatcher_send_custom_event(
        app->view_dispatcher, ExampleNumberInputCustomEventTextInput);
}

void example_number_input_scene_input_max_on_enter(void* context) {
    furi_assert(context);
    ExampleNumberInput* app = context;
    NumberInput* number_input = app->number_input;
    
    int32_t max = 2147483646; //has no effect if larger
    static char str[50];
    snprintf(str, sizeof(str), "Enter the maximum value");
    const char* constStr = str;
    
    number_input_set_header_text(number_input, constStr);
    number_input_set_result_callback(
        number_input,
        example_number_input_scene_input_max_callback,
        context,
        app->max_value,
        app->min_value,
        max);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNumberInputViewIdNumberInput);
}

bool example_number_input_scene_input_max_on_event(void* context, SceneManagerEvent event) {
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

void example_number_input_scene_input_max_on_exit(void* context) {
    ExampleNumberInput* app = context;
    UNUSED(app);
}