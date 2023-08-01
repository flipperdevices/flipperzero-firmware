#include "../ublox_i.h"

#define TAG "ublox_scene_enter_file_name"

void ublox_text_input_callback(void* context) {
    Ublox* ublox = context;

    view_dispatcher_send_custom_event(ublox->view_dispatcher, UbloxCustomEventTextInputDone);
}

void ublox_scene_enter_file_name_on_enter(void* context) {
    Ublox* ublox = context;
    TextInput* text_input = ublox->text_input;

    text_input_set_header_text(text_input, "Enter log file name");
    text_input_set_result_callback(
        text_input, ublox_text_input_callback, context, ublox->text_store, 100, false);

    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewTextInput);
}

bool ublox_scene_enter_file_name_on_event(void* context, SceneManagerEvent event) {
    Ublox* ublox = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UbloxCustomEventTextInputDone) {
            FURI_LOG_I(TAG, "text: %s", ublox->text_store);
            ublox->log_state = UbloxLogStateStartLogging;
            //scene_manager_next_scene(ublox->scene_manager, UbloxSceneDataDisplay);
            // don't add data_display as the next scene, instead go back to the last scene
            scene_manager_previous_scene(ublox->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void ublox_scene_enter_file_name_on_exit(void* context) {
    UNUSED(context);
}
