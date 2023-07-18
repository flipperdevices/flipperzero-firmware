#include "../nfc_app_i.h"

void nfc_scene_mf_classic_keys_add_byte_input_callback(void* context) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventByteInputDone);
}

void nfc_scene_mf_classic_keys_add_on_enter(void* context) {
    NfcApp* instance = context;

    // Setup view
    ByteInput* byte_input = instance->byte_input;
    byte_input_set_header_text(byte_input, "Enter the key in hex");
    byte_input_set_result_callback(
        byte_input,
        nfc_scene_mf_classic_keys_add_byte_input_callback,
        NULL,
        instance,
        instance->byte_input_store,
        sizeof(MfClassicKey));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewByteInput);
}

bool nfc_scene_mf_classic_keys_add_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventByteInputDone) {
            // Add key to dict
            MfDict* dict = mf_dict_alloc(MfDictTypeUser);
            furi_assert(dict);

            MfClassicKey key = {};
            for(size_t i = 0; i < sizeof(MfClassicKey); i++) {
                key.data[i] = instance->byte_input_store[sizeof(MfClassicKey) - 1 - i];
            }
            if(mf_dict_is_key_present(dict, &key)) {
                scene_manager_next_scene(
                    instance->scene_manager, NfcSceneMfClassicKeysWarnDuplicate);
            } else if(mf_dict_add_key(dict, &key)) {
                scene_manager_next_scene(instance->scene_manager, NfcSceneSaveSuccess);
                dolphin_deed(DolphinDeedNfcMfcAdd);
            } else {
                scene_manager_previous_scene(instance->scene_manager);
            }

            mf_dict_free(dict);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_mf_classic_keys_add_on_exit(void* context) {
    NfcApp* instance = context;

    // Clear view
    byte_input_set_result_callback(instance->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(instance->byte_input, "");
}
