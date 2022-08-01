#include "../nfc_i.h"

void nfc_scene_mf_classic_keys_list_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_mf_classic_keys_list_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;
    MfClassicDict* dict = mf_classic_dict_alloc(MfClassicDictTypeUser);
    uint32_t index = 0;
    string_t current_key;
    string_init(current_key);
    if(dict) {
        mf_classic_dict_rewind(dict);
        while(mf_classic_dict_get_next_key_str(dict, &current_key)) {
            submenu_add_item(
                submenu,
                string_get_cstr(current_key),
                index++,
                nfc_scene_mf_classic_keys_list_submenu_callback,
                nfc);
        }
        mf_classic_dict_free(dict);
    }
    submenu_set_header(submenu, "Select key to remove");
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_mf_classic_keys_list_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    // Nfc* nfc = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        FURI_LOG_D("ListKeys", "Selected item %d", event.event);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_mf_classic_keys_list_on_exit(void* context) {
    Nfc* nfc = context;

    submenu_reset(nfc->submenu);
}
