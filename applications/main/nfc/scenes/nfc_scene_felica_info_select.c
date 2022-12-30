#include "../nfc_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_felica_info_select_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_felica_info_select_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;
    FelicaData* data = &nfc->dev->dev_data.felica_data;
    FelicaSelectState* state = &nfc->felica_select;

    FelicaAreaPath_init(nfc->felica_select.selected_areas);

    submenu_add_item(submenu, "[Actions]", 0, nfc_scene_felica_info_select_submenu_callback, nfc);
    uint8_t i = 1;
    if(state->selected_system == NULL || state->selected_system->code == LITE_SYSTEM_CODE) {
        submenu_set_header(submenu, "Systems");
        FelicaSystemList_it_t it;
        for(FelicaSystemList_it(it, data->systems); !FelicaSystemList_end_p(it);
            FelicaSystemList_next(it)) {
            FelicaSystem* current_system = *FelicaSystemList_ref(it);
            FuriString* system_name = felica_get_system_name(current_system);
            submenu_add_item(
                submenu,
                furi_string_get_cstr(system_name),
                i++,
                nfc_scene_felica_info_select_submenu_callback,
                nfc);
            furi_string_free(system_name);
        }
    } else {
        FelicaSystem* system = state->selected_system;
        FuriString* header = furi_string_alloc_printf("%04X/", system->code);

        FelicaArea* area = &system->root_area;
        if(FelicaAreaPath_size(state->selected_areas) > 0) {
            FelicaAreaPath_it_t it;
            for(FelicaAreaPath_it(it, state->selected_areas); !FelicaAreaPath_end_p(it);
                FelicaAreaPath_next(it)) {
                FelicaArea* ancestor = *FelicaAreaPath_ref(it);
                furi_string_cat_printf(header, "%d/", ancestor->number);
            }
            area = *FelicaAreaPath_back(state->selected_areas);
        }
        furi_string_cat(header, "Areas");

        submenu_set_header(submenu, furi_string_get_cstr(header));
        furi_string_free(header);

        FelicaNodeList_it_t it;
        for(FelicaNodeList_it(it, area->nodes); !FelicaNodeList_end_p(it);
            FelicaNodeList_next(it)) {
            FelicaNode* node = *FelicaNodeList_ref(it);
            FuriString* node_name = furi_string_alloc();
            if(node->type == FelicaNodeTypeArea) {
                furi_string_printf(node_name, "Area %d", node->area->number);
                submenu_add_item(
                    submenu,
                    furi_string_get_cstr(node_name),
                    i++,
                    nfc_scene_felica_info_select_submenu_callback,
                    nfc);
            } else {
                uint16_t service_code = node->service->number << 6;
                furi_string_printf(node_name, "Service %04X", service_code);
                submenu_add_item(
                    submenu,
                    furi_string_get_cstr(node_name),
                    i++,
                    nfc_scene_felica_info_select_submenu_callback,
                    nfc);
            }

            furi_string_free(node_name);
        }
    }

    state->selected_service = NULL;
    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneFelicaInfoSelect));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_felica_info_select_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    FelicaData* data = &nfc->dev->dev_data.felica_data;
    FelicaSelectState* state = &nfc->felica_select;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        uint8_t index = event.event;
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneMfUltralightMenu, index);

        if(index == 0) {
            return false;
        }

        index -= 1;
        if(state->selected_system == NULL) {
            state->selected_system = *FelicaSystemList_get(data->systems, index);
            if(state->selected_system->code == LITE_SYSTEM_CODE) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaServiceData);
            } else {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaInfoSelect);
            }
            consumed = true;
        } else {
            FelicaNode* selected_node = NULL;
            if(FelicaAreaPath_size(state->selected_areas) == 0) {
                selected_node =
                    *FelicaNodeList_get(state->selected_system->root_area.nodes, index);
            } else {
                FelicaArea* current_area = *FelicaAreaPath_back(state->selected_areas);
                selected_node = *FelicaNodeList_get(current_area->nodes, index);
            }

            if(selected_node->type == FelicaNodeTypeArea) {
                FelicaAreaPath_push_back(state->selected_areas, selected_node->area);
                scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaInfoSelect);
                consumed = true;
            } else if(selected_node->type == FelicaNodeTypeService) {
                state->selected_service = selected_node->service;
                scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaServiceData);
                consumed = true;
            }
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(FelicaAreaPath_size(state->selected_areas) <= 1) {
            FelicaAreaPath_clear(state->selected_areas);
            state->selected_system = NULL;
        } else {
            FelicaAreaPath_pop_back(NULL, state->selected_areas);
        }
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    }

    return consumed;
}

void nfc_scene_felica_info_select_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    FelicaAreaPath_clear(nfc->felica_select.selected_areas);
    submenu_reset(nfc->submenu);
}
