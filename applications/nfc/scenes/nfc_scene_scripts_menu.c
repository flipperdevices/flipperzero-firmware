#include "nfc_scene_scripts_menu.h"
#include "../nfc_i.h"

#include <furi.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>

enum SubmenuIndex {
    SubmenuIndexBankCard,
    SubmenuIndexMifareUltralight,
};

void nfc_scene_scripts_menu_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = (Nfc*)context;

    view_dispatcher_send_custom_event(nfc->nfc_common.view_dispatcher, index);
}

const void nfc_scene_scripts_menu_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu,
        "Read bank card",
        SubmenuIndexBankCard,
        nfc_scene_scripts_menu_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read Mifare Ultralight",
        SubmenuIndexMifareUltralight,
        nfc_scene_scripts_menu_submenu_callback,
        nfc);
    submenu_set_selected_item(nfc->submenu, nfc->scene_scripts_menu->state);

    view_dispatcher_switch_to_view(nfc->nfc_common.view_dispatcher, NfcViewMenu);
}

const bool nfc_scene_scripts_menu_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = (Nfc*)context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexBankCard) {
            nfc->scene_scripts_menu->state = SubmenuIndexBankCard;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_not_implemented);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexMifareUltralight) {
            nfc->scene_scripts_menu->state = SubmenuIndexMifareUltralight;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_read_mifare_ul);
            return scene_manager_next_scene(nfc->scene_manager);
        }
    }

    return false;
}

const void nfc_scene_scripts_menu_on_exit(void* context) {
    Nfc* nfc = (Nfc*)context;

    submenu_clean(nfc->submenu);
}

AppScene* nfc_scene_scripts_menu_alloc() {
    AppScene* scene = furi_alloc(sizeof(AppScene));
    scene->id = NfcSceneScriptsMenu;
    scene->on_enter = nfc_scene_scripts_menu_on_enter;
    scene->on_event = nfc_scene_scripts_menu_on_event;
    scene->on_exit = nfc_scene_scripts_menu_on_exit;

    return scene;
}

void nfc_scene_scripts_menu_free(AppScene* scene) {
    free(scene);
}