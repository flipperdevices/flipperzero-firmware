#include "../nfc_relay.h"
enum SubmenuIndex {
    SubmenuIndexAsCard,
    SubmenuIndexAsReader,
    SubmenuIndexConfig,
};

void nfc_relay_scene_start_submenu_callback(void* context, uint32_t index) {
    NfcRelay* nfc_relay = context;
    view_dispatcher_send_custom_event(nfc_relay->view_dispatcher, index);
}

void nfc_relay_scene_start_on_enter(void* context) {
    NfcRelay* nfc_relay = context;

    Submenu* submenu = nfc_relay->submenu;
    submenu_add_item(
        submenu, "As Card", SubmenuIndexAsCard, nfc_relay_scene_start_submenu_callback, nfc_relay);

    submenu_add_item(
        submenu,
        "As Reader",
        SubmenuIndexAsReader,
        nfc_relay_scene_start_submenu_callback,
        nfc_relay);

    submenu_add_item(
        submenu, "Config", SubmenuIndexConfig, nfc_relay_scene_start_submenu_callback, nfc_relay);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(nfc_relay->scene_manager, NfcRelaySceneStart));
    view_dispatcher_switch_to_view(nfc_relay->view_dispatcher, NfcRelayViewMenu);
}

bool nfc_relay_scene_start_on_event(void* context, SceneManagerEvent event) {
    NfcRelay* nfc_relay = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexAsCard) {
            scene_manager_set_scene_state(
                nfc_relay->scene_manager, NfcRelaySceneStart, SubmenuIndexAsCard);
            scene_manager_next_scene(nfc_relay->scene_manager, NfcRelaySceneAsCard);
            consumed = true;
        } else if(event.event == SubmenuIndexAsReader) {
            scene_manager_set_scene_state(
                nfc_relay->scene_manager, NfcRelaySceneStart, SubmenuIndexAsReader);
            scene_manager_next_scene(nfc_relay->scene_manager, NfcRelaySceneAsReader);
            consumed = true;
        } else if(event.event == SubmenuIndexConfig) {
            scene_manager_set_scene_state(
                nfc_relay->scene_manager, NfcRelaySceneStart, SubmenuIndexConfig);
            scene_manager_next_scene(nfc_relay->scene_manager, NfcRelaySceneConfig);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_relay_scene_start_on_exit(void* context) {
    NfcRelay* nfc_relay = context;
    submenu_reset(nfc_relay->submenu);
}
