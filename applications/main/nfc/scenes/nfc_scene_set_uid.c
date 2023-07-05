#include "../nfc_app_i.h"

void nfc_scene_set_uid_byte_input_callback(void* context) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventByteInputDone);
}

void nfc_scene_set_uid_on_enter(void* context) {
    NfcApp* instance = context;

    if(scene_manager_has_previous_scene(instance->scene_manager, NfcSceneSavedMenu)) {
        nfc_device_copy_data(
            instance->nfc_device, NfcProtocolIso14443_3a, instance->iso14443_3a_edit_data);
    }

    // Setup view
    ByteInput* byte_input = instance->byte_input;
    byte_input_set_header_text(byte_input, "Enter UID in hex");
    byte_input_set_result_callback(
        byte_input,
        nfc_scene_set_uid_byte_input_callback,
        NULL,
        instance,
        instance->iso14443_3a_edit_data->uid,
        instance->iso14443_3a_edit_data->uid_len);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewByteInput);
}

bool nfc_scene_set_uid_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventByteInputDone) {
            nfc_device_set_data(
                instance->nfc_device, NfcProtocolIso14443_3a, instance->iso14443_3a_edit_data);
            if(scene_manager_has_previous_scene(instance->scene_manager, NfcSceneSavedMenu)) {
                if(nfc_save(instance)) {
                    scene_manager_next_scene(instance->scene_manager, NfcSceneSaveSuccess);
                    consumed = true;
                }
            } else {
                scene_manager_next_scene(instance->scene_manager, NfcSceneSaveName);
                consumed = true;
            }
        }
    }

    return consumed;
}

void nfc_scene_set_uid_on_exit(void* context) {
    NfcApp* instance = context;

    // Clear view
    byte_input_set_result_callback(instance->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(instance->byte_input, "");
}
