#include "../nfc_crocodile_i.h"
#include "nfc_crocodile_scene.h"

enum {
    NfcCrocodileSceneMainMenuDictionaryListItem,
    NfcCrocodileSceneMainMenuSettingsItem,
    NfcCrocodileSceneMainMenuAboutItem,
} NfcCrocodileSceneMainMenuItems;

void nfc_crocodile_scene_main_menu_choice_callback(void* context, uint32_t index) {
    NfcCrocodile* nfc_crocodile = context;
    scene_manager_handle_custom_event(nfc_crocodile->scene_manager, index);
}

void nfc_crocodile_scene_main_menu_on_enter(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    submenu_reset(nfc_crocodile->submenu);
    //submenu_set_header(nfc_crocodile->submenu, "Nfc Crocodile");
    submenu_add_item(
        nfc_crocodile->submenu,
        "Start game",
        NfcCrocodileSceneMainMenuDictionaryListItem,
        nfc_crocodile_scene_main_menu_choice_callback,
        context);
    submenu_add_item(
        nfc_crocodile->submenu,
        "Settings",
        NfcCrocodileSceneMainMenuSettingsItem,
        nfc_crocodile_scene_main_menu_choice_callback,
        context);
    submenu_add_item(
        nfc_crocodile->submenu,
        "About",
        NfcCrocodileSceneMainMenuAboutItem,
        nfc_crocodile_scene_main_menu_choice_callback,
        context);
    submenu_set_selected_item(
        nfc_crocodile->submenu,
        scene_manager_get_scene_state(nfc_crocodile->scene_manager, NfcCrocodileSceneMainMenu));
    view_dispatcher_switch_to_view(nfc_crocodile->view_dispatcher, NfcCrocodileViewMenu);
}

bool nfc_crocodile_scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    NfcCrocodile* nfc_crocodile = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            nfc_crocodile->scene_manager, NfcCrocodileSceneMainMenu, event.event);
        switch(event.event) {
        case NfcCrocodileSceneMainMenuDictionaryListItem:
            scene_manager_next_scene(nfc_crocodile->scene_manager, NfcCrocodileSceneDictionary);
            consumed = true;
            break;
        case NfcCrocodileSceneMainMenuSettingsItem:
            scene_manager_next_scene(nfc_crocodile->scene_manager, NfcCrocodileSceneSettings);
            consumed = true;
            break;
        case NfcCrocodileSceneMainMenuAboutItem:
            scene_manager_next_scene(nfc_crocodile->scene_manager, NfcCrocodileSceneAbout);
            consumed = true;
            break;
        }
    }

    return consumed;
}

void nfc_crocodile_scene_main_menu_on_exit(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    submenu_reset(nfc_crocodile->submenu);
}