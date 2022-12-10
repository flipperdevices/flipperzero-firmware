#include "../nfc_i.h"
#include <dolphin/dolphin.h>

#define SYSTEM_EVENT

void nfc_scene_felica_info_select_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_felica_info_select_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;
    FelicaData* data = &nfc->dev->dev_data.felica_data;
    FelicaSelectState* state = &nfc->felica_select;

    FuriStringStack_init(nfc->felica_select.strings);
    FelicaAreaPath_init(nfc->felica_select.selected_areas);

    submenu_add_item(submenu, "[Actions]", 0, nfc_scene_felica_info_select_submenu_callback, nfc);
    uint8_t i = 1;
    if(state->selected_system == NULL) {
        FelicaSystemList_it_t it;
        for(FelicaSystemList_it(it, data->systems); !FelicaSystemList_end_p(it); FelicaSystemList_next(it)) {
            FelicaSystem* current_system = *FelicaSystemList_ref(it);
            FuriString* system_name = felica_get_system_name(current_system);
            submenu_add_item(
                submenu,
                furi_string_get_cstr(system_name),
                i++,
                nfc_scene_felica_info_select_submenu_callback,
                nfc);
            FuriStringStack_push_back(state->strings, system_name);
        }
    } else if(FelicaAreaPath_size(state->selected_areas) == 0) {
        FelicaArea* area_0 = &state->selected_system->root_area;
        FelicaNodeList_it_t it;
        for(FelicaNodeList_it(it, area_0->nodes); !FelicaNodeList_end_p(it); FelicaNodeList_next(it)) {
            FelicaNode* node = *FelicaNodeList_ref(it);
            if(node->type == FelicaNodeTypeArea) {
                FuriString* area_name = furi_string_alloc_printf("Area %d", node->ptr.area->number);
                submenu_add_item(
                    submenu, furi_string_get_cstr(area_name), i++, nfc_scene_felica_info_select_submenu_callback, nfc);
                FuriStringStack_push_back(state->strings, area_name);
            } else {
                uint16_t service_code = node->ptr.service->number << 6;
                FuriString* service_name = furi_string_alloc_printf("Service %04X", service_code);
                submenu_add_item(
                    submenu, furi_string_get_cstr(service_name), i++, nfc_scene_felica_info_select_submenu_callback, nfc);
                FuriStringStack_push_back(state->strings, service_name);
            }
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
            scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaInfoSelect);
            consumed = true;
        } else {
            FelicaNode* selected_node = NULL;
            if (FelicaAreaPath_size(state->selected_areas) == 0) {
                selected_node = *FelicaNodeList_get(state->selected_system->root_area.nodes, index);
            } else {
                FelicaArea* current_area = *FelicaAreaPath_back(state->selected_areas);
                selected_node = *FelicaNodeList_get(current_area->nodes, index);
            }

            if(selected_node->type == FelicaNodeTypeArea) {
                FelicaAreaPath_push_back(state->selected_areas, selected_node->ptr.area);
                scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaInfoSelect);
                consumed = true;
            } else if(selected_node->type == FelicaNodeTypeService) {
                state->selected_service = selected_node->ptr.service;
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
    FuriStringStack_it_t it;
    for(FuriStringStack_it(it, nfc->felica_select.strings); !FuriStringStack_end_p(it); FuriStringStack_next(it)) {
        furi_string_free(*FuriStringStack_ref(it));
    }
    submenu_reset(nfc->submenu);
}
