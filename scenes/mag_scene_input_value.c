#include "../mag_i.h"

void mag_scene_input_value_on_enter(void* context) {
    Mag* mag = context;
    TextInput* text_input = mag->text_input;

    // TODO: retrieve stored/existing data if editing rather than adding anew?
    mag_text_store_set(mag, furi_string_get_cstr(mag->mag_dev->dev_data.track[1].str));

    text_input_set_header_text(text_input, "Enter track data (WIP)");
    text_input_set_result_callback(
        text_input, mag_text_input_callback, mag, mag->text_store, MAG_TEXT_STORE_SIZE, true);

    text_input_add_illegal_symbols(text_input);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewTextInput);
}

bool mag_scene_input_value_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagEventNext) {
            consumed = true;

            furi_string_set(mag->mag_dev->dev_data.track[1].str, mag->text_store);
            scene_manager_next_scene(scene_manager, MagSceneInputName);
        }
    }

    return consumed;
}

void mag_scene_input_value_on_exit(void* context) {
    Mag* mag = context;
    UNUSED(mag);
}