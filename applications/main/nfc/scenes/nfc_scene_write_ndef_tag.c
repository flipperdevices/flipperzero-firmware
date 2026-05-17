#include "../nfc_app_i.h"

enum SubmenuIndex {
    SubmenuIndexNtag213 = NdefNtagTypeNTAG213,
    SubmenuIndexNtag215 = NdefNtagTypeNTAG215,
    SubmenuIndexNtag216 = NdefNtagTypeNTAG216,
};

static void nfc_scene_write_ndef_tag_submenu_callback(void* context, uint32_t index) {
    NfcApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_scene_write_ndef_tag_on_enter(void* context) {
    NfcApp* instance = context;
    Submenu* submenu = instance->submenu;

    submenu_set_header(submenu, "Target tag type");
    submenu_add_item(
        submenu, "NTAG213 (144 B)", SubmenuIndexNtag213,
        nfc_scene_write_ndef_tag_submenu_callback, instance);
    submenu_add_item(
        submenu, "NTAG215 (504 B)", SubmenuIndexNtag215,
        nfc_scene_write_ndef_tag_submenu_callback, instance);
    submenu_add_item(
        submenu, "NTAG216 (888 B)", SubmenuIndexNtag216,
        nfc_scene_write_ndef_tag_submenu_callback, instance);

    // Default selection: NTAG215
    uint32_t state = scene_manager_get_scene_state(instance->scene_manager, NfcSceneWriteNdefTag);
    if(state == 0) state = SubmenuIndexNtag215;
    submenu_set_selected_item(submenu, state);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_write_ndef_tag_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        instance->ndef_write.ntag_type = (NdefNtagType)event.event;
        scene_manager_set_scene_state(
            instance->scene_manager, NfcSceneWriteNdefTag, event.event);
        scene_manager_next_scene(instance->scene_manager, NfcSceneWriteNdef);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_write_ndef_tag_on_exit(void* context) {
    NfcApp* instance = context;
    submenu_reset(instance->submenu);
}
