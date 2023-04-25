#include "../nfc_i.h"
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
    FuriString* system_name = furi_string_alloc();

    submenu_add_item(
        submenu, "Card Info", SubmenuIndexCardInfo, nfc_scene_felica_data_submenu_callback, nfc);
    uint8_t i = SubmenuIndexDynamic;
    for
        M_EACH(current_system, data->systems, FelicaSystemArray_t) {
            felica_describe_system(current_system, system_name);
            submenu_add_item(
                submenu,
                furi_string_get_cstr(system_name),
                i++,
                nfc_scene_felica_data_submenu_callback,
                nfc);
            furi_string_reset(system_name);
        }

    furi_string_free(system_name);

    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneFelicaData) >> 1);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_felica_data_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    SceneManager* scene_manager = nfc->scene_manager;
    ViewDispatcher* view_dispatcher = nfc->view_dispatcher;
    FelicaData* data = &nfc->dev->dev_data.felica_data;
    TextBox* text_box = nfc->text_box;
    FuriString* text_box_store = nfc->text_box_store;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        uint8_t index = event.event;
        scene_manager_set_scene_state(scene_manager, NfcSceneFelicaData, index << 1);

        if(index == SubmenuIndexCardInfo) {
            furi_string_reset(text_box_store);

            felica_print_card_spec(&data->spec, text_box_store);
            text_box_set_text(text_box, furi_string_get_cstr(text_box_store));

            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaData, 1);

            text_box_set_font(text_box, TextBoxFontHex);
            view_dispatcher_switch_to_view(view_dispatcher, NfcViewTextBox);
            consumed = true;
        } else {
            uint8_t system_index = (index - SubmenuIndexDynamic) & 0xf;
            scene_manager_set_scene_state(
                scene_manager, NfcSceneFelicaSysData, system_index << 16);
            scene_manager_next_scene(scene_manager, NfcSceneFelicaSysData);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        uint8_t state = scene_manager_get_scene_state(scene_manager, NfcSceneFelicaData);
        if(state & 1) {
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaData, 0);
            view_dispatcher_switch_to_view(view_dispatcher, NfcViewMenu);
            consumed = true;
        } else {
            consumed = scene_manager_previous_scene(scene_manager);
        }
    }

    return consumed;
}

void nfc_scene_felica_data_on_exit(void* context) {
    Nfc* nfc = context;

    submenu_reset(nfc->submenu);
    text_box_reset(nfc->text_box);
    furi_string_reset(nfc->text_box_store);
}
