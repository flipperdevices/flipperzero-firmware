#include "../mag_i.h"

void mag_scene_input_value_on_enter(void* context) {
    Mag* mag = context;
    TextInput* text_input = mag->text_input;

    FuriString* tmp_str;
    tmp_str = furi_string_alloc();
    UNUSED(tmp_str);

    text_input_set_header_text(text_input, "Enter track data (WIP)");
    //text_input_set_result_callback(
    //    text_input, mag_text_input_callback, mag, mag->mag_dev->dev_data, )
}

bool mag_scene_input_value_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    UNUSED(mag);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void mag_scene_input_value_on_exit(void* context) {
    Mag* mag = context;
    UNUSED(mag);
}