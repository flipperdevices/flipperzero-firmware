#include "../nfc_app_i.h"

#include "../helpers/protocol_support/nfc_protocol_support.h"

void nfc_scene_saved_menu_on_enter(void* context) {
    NfcApp* nfc = context;

    nfc_protocol_support_build_scene_saved_menu(nfc);

    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneSavedMenu));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_saved_menu_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneSavedMenu, event.event);
        consumed = nfc_protocol_support_handle_scene_saved_menu(nfc, event.event);
    }

    return consumed;
}

void nfc_scene_saved_menu_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
