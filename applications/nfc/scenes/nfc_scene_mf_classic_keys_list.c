#include "../nfc_i.h"

#include <m-array.h>

ARRAY_DEF(MfClassicUserKeys, char*, M_PTR_OPLIST);

static MfClassicUserKeys_t mfc_key_strs;

void nfc_scene_mf_classic_keys_list_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_mf_classic_keys_list_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;
    MfClassicDict* dict = mf_classic_dict_alloc(MfClassicDictTypeUser);
    uint32_t index = 0;
    string_t temp_key;
    MfClassicUserKeys_init(mfc_key_strs);
    string_init(temp_key);
    if(dict) {
        mf_classic_dict_rewind(dict);
        while(mf_classic_dict_get_next_key_str(dict, temp_key)) {
            char* current_key = (char*)malloc(sizeof(char) * 13);
            strncpy(current_key, string_get_cstr(temp_key), 12);
            MfClassicUserKeys_push_back(mfc_key_strs, current_key);
            FURI_LOG_D("ListKeys", "Key %d: %s", index, current_key);
            submenu_add_item(
                submenu,
                current_key,
                index++,
                nfc_scene_mf_classic_keys_list_submenu_callback,
                nfc);
        }
    }
    submenu_set_header(submenu, "Select key to remove");
    mf_classic_dict_free(dict);
    string_clear(temp_key);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_mf_classic_keys_list_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        MfClassicDict* dict = mf_classic_dict_alloc(MfClassicDictTypeUser);
        if(dict) {
            mf_classic_dict_rewind(dict);
            if(mf_classic_dict_remove_key(dict, event.event)) {
                if(mf_classic_dict_get_total_keys(dict) == 0) {
                    scene_manager_set_scene_state(
                        nfc->scene_manager, NfcSceneDeleteSuccess, NfcSceneMfClassicKeys);
                } else {
                    scene_manager_set_scene_state(
                        nfc->scene_manager, NfcSceneDeleteSuccess, NfcSceneMfClassicKeysList);
                }
                scene_manager_next_scene(nfc->scene_manager, NfcSceneDeleteSuccess);
            } else {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneDictNotFound);
            }
        }
        mf_classic_dict_free(dict);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_mf_classic_keys_list_on_exit(void* context) {
    Nfc* nfc = context;

    MfClassicUserKeys_it_t it;
    for(MfClassicUserKeys_it(it, mfc_key_strs); !MfClassicUserKeys_end_p(it);
        MfClassicUserKeys_next(it)) {
        free(MfClassicUserKeys_ref(it));
    }
    MfClassicUserKeys_clear(mfc_key_strs);
    submenu_reset(nfc->submenu);
}
