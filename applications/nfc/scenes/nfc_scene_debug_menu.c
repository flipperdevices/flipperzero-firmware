#include "nfc_scene_debug_menu.h"
#include "../nfc_i.h"

#include <furi.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>

enum SubmenuIndex {
    SubmenuIndexDetect,
    SubmenuIndexEmulate,
    SubmenuIndexReadEmv,
    SubmenuIndexReadMifareUl,
};

void nfc_scene_debug_menu_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = (Nfc*)context;

    view_dispatcher_send_custom_event(nfc->nfc_common.view_dispatcher, index);
}

const void nfc_scene_debug_menu_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu, "Detect", SubmenuIndexDetect, nfc_scene_debug_menu_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Emulate", SubmenuIndexEmulate, nfc_scene_debug_menu_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Read EMV", SubmenuIndexReadEmv, nfc_scene_debug_menu_submenu_callback, nfc);
    submenu_add_item(
        submenu,
        "Read Mifare Ultralight",
        SubmenuIndexReadMifareUl,
        nfc_scene_debug_menu_submenu_callback,
        nfc);
    submenu_set_selected_item(nfc->submenu, nfc->scene_debug_menu->state);

    view_dispatcher_switch_to_view(nfc->nfc_common.view_dispatcher, NfcViewMenu);
}

const bool nfc_scene_debug_menu_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = (Nfc*)context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexDetect) {
            nfc->scene_debug_menu->state = SubmenuIndexDetect;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_debug_detect);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexEmulate) {
            nfc->scene_debug_menu->state = SubmenuIndexEmulate;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_debug_emulate);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexReadEmv) {
            nfc->scene_debug_menu->state = SubmenuIndexReadEmv;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_debug_read_emv);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexReadMifareUl) {
            nfc->scene_debug_menu->state = SubmenuIndexReadMifareUl;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_debug_read_mifare_ul);
            return scene_manager_next_scene(nfc->scene_manager);
        }
    }

    return false;
}

const void nfc_scene_debug_menu_on_exit(void* context) {
    Nfc* nfc = (Nfc*)context;

    submenu_clean(nfc->submenu);
}

AppScene* nfc_scene_debug_menu_alloc() {
    AppScene* scene = furi_alloc(sizeof(AppScene));
    scene->id = NfcSceneDebugMenu;
    scene->on_enter = nfc_scene_debug_menu_on_enter;
    scene->on_event = nfc_scene_debug_menu_on_event;
    scene->on_exit = nfc_scene_debug_menu_on_exit;

    return scene;
}

void nfc_scene_debug_menu_free(AppScene* scene) {
    free(scene);
}