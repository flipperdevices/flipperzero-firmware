#include "../nfc_magic_i.h"
enum SubmenuIndex {
    SubmenuIndexRead,
    SubmenuIndexWipe,
    SubmenuIndexWrite,
};

void nfc_magic_scene_write_menu_submenu_callback(void* context, uint32_t index) {
    NfcMagic* nfc_magic = context;
    view_dispatcher_send_custom_event(nfc_magic->view_dispatcher, index);
}

void nfc_magic_scene_write_menu_on_enter(void* context) {
    NfcMagic* nfc_magic = context;

    Submenu* submenu = nfc_magic->submenu;
    submenu_add_item(
        submenu,
        "Read Card",
        SubmenuIndexRead,
        nfc_magic_scene_write_menu_submenu_callback,
        nfc_magic);
    submenu_add_item(
        submenu, "Wipe", SubmenuIndexWipe, nfc_magic_scene_write_menu_submenu_callback, nfc_magic);
    submenu_add_item(
        submenu,
        "Write",
        SubmenuIndexWrite,
        nfc_magic_scene_write_menu_submenu_callback,
        nfc_magic);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(nfc_magic->scene_manager, NfcMagicSceneWriteMenu));
    view_dispatcher_switch_to_view(nfc_magic->view_dispatcher, NfcMagicViewMenu);
}

bool nfc_magic_scene_write_menu_on_event(void* context, SceneManagerEvent event) {
    NfcMagic* nfc_magic = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneStart);
            consumed = true;
        } else if(event.event == SubmenuIndexWipe) {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneStart);
            consumed = true;
        } else if(event.event == SubmenuIndexWrite) {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneWriteMenu);
            consumed = true;
        }
        scene_manager_set_scene_state(nfc_magic->scene_manager, NfcMagicSceneStart, event.event);
    }

    return consumed;
}

void nfc_magic_scene_write_menu_on_exit(void* context) {
    NfcMagic* nfc_magic = context;
    submenu_reset(nfc_magic->submenu);
}
