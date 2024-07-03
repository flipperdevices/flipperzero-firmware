#include "../example_number_input_i.h"

void example_number_input_scene_show_number_callback(
    ExampleNumberInputCustomEvent event,
    void* context) {
    furi_assert(context);
    ExampleNumberInput* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void example_number_input_scene_show_number_on_enter(void* context) {
    furi_assert(context);
    ExampleNumberInput* app = context;
    example_number_input_show_number_model_set_number(app->show_number, app->current_number);
    example_number_input_show_number_set_callback(
        app->show_number, example_number_input_scene_show_number_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ExampleNumberInputViewIdShowNumber);
}

bool example_number_input_scene_show_number_on_event(void* context, SceneManagerEvent event) {
    ExampleNumberInput* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case ExampleNumberInputCustomEventShowNumberOk:
            scene_manager_next_scene(app->scene_manager, ExampleNumberInputSceneInputNumber);
            consumed = true;
            break;
        case ExampleNumberInputCustomEventShowNumberBack:
            scene_manager_stop(app->scene_manager);
            view_dispatcher_stop(app->view_dispatcher);
            consumed = true;
            break;
        case ExampleNumberInputCustomEventShowNumberLeft:
            scene_manager_next_scene(app->scene_manager, ExampleNumberInputSceneInputMin);
            consumed = true;
            break;
        case ExampleNumberInputCustomEventShowNumberRight:
            scene_manager_next_scene(app->scene_manager, ExampleNumberInputSceneInputMax);
            consumed = true;
            break;
        default:
            break;
        }
    }

    return consumed;
}

void example_number_input_scene_show_number_on_exit(void* context) {
    UNUSED(context);
}