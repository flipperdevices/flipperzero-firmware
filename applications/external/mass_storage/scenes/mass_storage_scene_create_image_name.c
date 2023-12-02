#include "../mass_storage_app_i.h"

enum TextInputIndex {
    TextInputResultOk,
};

static void mass_storage_scene_create_image_name_text_input_callback(void* context) {
    MassStorageApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, TextInputResultOk);
}

void mass_storage_scene_create_image_name_on_enter(void* context) {
    MassStorageApp* app = context;
    TextInput* text_input = app->text_input;

    text_input_set_header_text(text_input, "Image name, empty = default");

    text_input_set_minimum_length(text_input, 0);

    text_input_set_result_callback(
        text_input,
        mass_storage_scene_create_image_name_text_input_callback,
        app,
        app->create_image_name,
        sizeof(app->create_image_name),
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewTextInput);
}

bool mass_storage_scene_create_image_name_on_event(void* context, SceneManagerEvent event) {
    MassStorageApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case TextInputResultOk:
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void mass_storage_scene_create_image_name_on_exit(void* context) {
    MassStorageApp* app = context;
    text_input_reset(app->text_input);
}
