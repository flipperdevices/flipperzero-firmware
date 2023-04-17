#include "../nfc_i.h"
#include "gui/scene_manager.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexCardInfo = 0,
    SubmenuIndexDynamic,
};

void nfc_scene_felica_data_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_felica_data_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;
    FelicaData* data = &nfc->dev->dev_data.felica_data;

    submenu_add_item(
        submenu, "Card Info", SubmenuIndexCardInfo, nfc_scene_felica_data_submenu_callback, nfc);
    uint8_t i = SubmenuIndexDynamic;
    for
        M_EACH(current_system, data->systems, FelicaSystemArray_t) {
            FuriString* system_name = felica_get_system_name(current_system);
            submenu_add_item(
                submenu,
                furi_string_get_cstr(system_name),
                i++,
                nfc_scene_felica_data_submenu_callback,
                nfc);
            furi_string_free(system_name);
        }

    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneFelicaData));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_felica_data_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        uint8_t index = event.event;
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneMfUltralightMenu, index);

        if(index == SubmenuIndexCardInfo) {
            // TODO card info
            return false;
        }

        uint8_t system_index = (index - SubmenuIndexDynamic) & 0xf;
        scene_manager_set_scene_state(
            nfc->scene_manager, NfcSceneFelicaSystem, system_index << 16);
        scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaSystem);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    }

    return consumed;
}

void nfc_scene_felica_data_on_exit(void* context) {
    Nfc* nfc = context;

    submenu_reset(nfc->submenu);
}
