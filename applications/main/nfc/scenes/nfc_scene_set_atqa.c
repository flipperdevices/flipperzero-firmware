#include "../nfc_app_i.h"

void nfc_scene_set_atqa_byte_input_changed_callback(void* context) {
    NfcApp* instance = context;
    memcpy(instance->iso14443_3a_edit_data->atqa, instance->byte_input_store, 2);
}

void nfc_scene_set_atqa_byte_input_done_callback(void* context) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventByteInputDone);
}

void nfc_scene_set_atqa_on_enter(void* context) {
    NfcApp* instance = context;

    memcpy(instance->byte_input_store, instance->iso14443_3a_edit_data->atqa, 2);

    // Setup view
    ByteInput* byte_input = instance->byte_input;
    byte_input_set_header_text(byte_input, "Enter ATQA in hex");
    byte_input_set_result_callback(
        byte_input,
        nfc_scene_set_atqa_byte_input_done_callback,
        nfc_scene_set_atqa_byte_input_changed_callback,
        instance,
        instance->byte_input_store,
        2);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewByteInput);
}

bool nfc_scene_set_atqa_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventByteInputDone) {
            nfc_device_set_data(
                instance->nfc_device, NfcProtocolIso14443_3a, instance->iso14443_3a_edit_data);
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
