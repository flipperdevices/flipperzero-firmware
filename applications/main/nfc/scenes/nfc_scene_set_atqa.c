#include "../nfc_app_i.h"

void nfc_scene_set_atqa_byte_input_callback(void* context) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventByteInputDone);
}

void nfc_scene_set_atqa_on_enter(void* context) {
    NfcApp* instance = context;

    // Setup view
    ByteInput* byte_input = instance->byte_input;
    byte_input_set_header_text(byte_input, "Enter ATQA in hex");
    byte_input_set_result_callback(
        byte_input,
        nfc_scene_set_atqa_byte_input_callback,
        NULL,
        instance,
        instance->iso14443_3a_edit_data->atqa,
        2);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewByteInput);
}

bool nfc_scene_set_atqa_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventByteInputDone) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSetUid);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_set_atqa_on_exit(void* context) {
    NfcApp* instance = context;

    // Clear view
    byte_input_set_result_callback(instance->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(instance->byte_input, "");
}
