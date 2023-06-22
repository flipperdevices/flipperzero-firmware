#include "../nfc_app_i.h"

void nfc_scene_select_protocol_submenu_callback(void* context, uint32_t index) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_scene_select_protocol_on_enter(void* context) {
    NfcApp* instance = context;
    Submenu* submenu = instance->submenu;

    FuriString* temp_str = furi_string_alloc();
    const char* prefix;
    if(scene_manager_has_previous_scene(instance->scene_manager, NfcSceneExtraActions)) {
        prefix = "Read";
        instance->protocols_detected_num = NfcProtocolTypeMax;
        for(size_t i = 0; i < NfcProtocolTypeMax; i++) {
            instance->protocols_detected[i] = i;
        }
    } else {
        prefix = "Read as";
        submenu_set_header(submenu, "Multi-protocol card");
    }

    for(size_t i = 0; i < instance->protocols_detected_num; i++) {
        furi_string_printf(
            temp_str, "%s %s", prefix, nfc_dev_get_protocol_name(instance->protocols_detected[i]));
        submenu_add_item(
            submenu,
            furi_string_get_cstr(temp_str),
            i,
            nfc_scene_select_protocol_submenu_callback,
            instance);
    }
    furi_string_free(temp_str);

    uint32_t state =
        scene_manager_get_scene_state(instance->scene_manager, NfcSceneSelectProtocol);
    submenu_set_selected_item(submenu, state);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_select_protocol_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        instance->protocols_detected_idx = event.event;
        // TODO remove this and switch to generic read
        const uint32_t nfc_read_scenes[NfcProtocolTypeMax] = {
            NfcSceneNfcaRead,
            NfcSceneNfcaRead,
            NfcSceneMfUltralightRead,
            NfcSceneMfClassicDictAttack,
            NfcSceneMfDesfireRead,
        };
        scene_manager_next_scene(
            instance->scene_manager,
            nfc_read_scenes[instance->protocols_detected[instance->protocols_detected_idx]]);
        scene_manager_set_scene_state(
            instance->scene_manager, NfcSceneSelectProtocol, event.event);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_select_protocol_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
