#include "../nfc_i.h"
#include "core/string.h"
#include "gui/modules/submenu.h"
#include "gui/scene_manager.h"
#include "protocols/felica.h"
#include <dolphin/dolphin.h>

// State bit format: 000000000000ssssnnnnnnnnnnnnnnnn
// s: system index
// n: inode

enum SubmenuIndex {
    SubmenuIndexSystemInfo = 0,
    SubmenuIndexAreaInfo,
    SubmenuIndexDynamic,
};

void nfc_scene_felica_system_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

static void nfc_scene_felica_system_load_node(Nfc* nfc, uint32_t state, FelicaINode prev_inode) {
    uint16_t inode = state & FELICA_INODE_MASK;
    uint8_t system_index = (state >> 16) & 0xf;
    FelicaData* data = &nfc->dev->dev_data.felica_data;

    FelicaSystem* system = FelicaSystemArray_get(data->systems, system_index);
    furi_assert(system != NULL);
    FelicaNode* node = FelicaNodeArray_get(system->nodes, inode);
    furi_assert(node != NULL);

    switch(node->type) {
    case FelicaNodeTypeArea: {
        furi_assert(node->area != NULL);

        Submenu* submenu = nfc->submenu;
        submenu_reset(submenu);

        for
            M_EACH(subnode_inode_p, node->area->nodes, FelicaINodeArray_t) {
                furi_assert(subnode_inode_p != NULL);
                FelicaINode subnode_inode = *subnode_inode_p;
                FelicaNode* subnode = felica_std_inode_lookup(system, subnode_inode);
                FuriString* subnode_desc = felica_describe_node(subnode);
                submenu_add_item(
                    submenu,
                    furi_string_get_cstr(subnode_desc),
                    SubmenuIndexDynamic + subnode_inode,
                    nfc_scene_felica_system_submenu_callback,
                    nfc);
            }
        if(prev_inode != FELICA_INODE_INVALID) {
            submenu_set_selected_item(submenu, SubmenuIndexDynamic + prev_inode);
        }
        break;
    }
    // TODO service display with textbox
    default:
        break;
    }
}

void nfc_scene_felica_system_on_enter(void* context) {
    Nfc* nfc = context;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneFelicaSystem);
    nfc_scene_felica_system_load_node(nfc, state, FELICA_INODE_INVALID);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
    return;
}

bool nfc_scene_felica_system_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneFelicaSystem);

    switch(event.type) {
    case SceneManagerEventTypeCustom: {
        switch(event.event) {
        case SubmenuIndexSystemInfo: {
            // TODO show system info
            break;
        }
        case SubmenuIndexAreaInfo: {
            // TODO show area info
            break;
        }
        default: {
            FelicaINode selected_inode = (event.event - SubmenuIndexDynamic) & FELICA_INODE_MASK;
            uint32_t new_state = (state & (~FELICA_INODE_INVALID)) | selected_inode;
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneFelicaSystem, new_state);
            nfc_scene_felica_system_load_node(nfc, new_state, FELICA_INODE_INVALID);
            consumed = true;
        }
        }
        break;
    }
    case SceneManagerEventTypeBack: {
        FelicaINode curr_inode = state & FELICA_INODE_MASK;
        uint8_t curr_system_code = (state >> 16) & 0xf;
        FelicaSystem* curr_system =
            FelicaSystemArray_get(nfc->dev->dev_data.felica_data.systems, curr_system_code);
        furi_assert(curr_system != NULL);
        FelicaNode* curr_node = felica_std_inode_lookup(curr_system, curr_inode);
        furi_assert(curr_node != NULL);

        FelicaINode new_inode = curr_node->parent;
        if(new_inode != FELICA_INODE_INVALID) {
            uint32_t new_state = (state & (~FELICA_INODE_INVALID)) | new_inode;
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneFelicaSystem, new_state);
            nfc_scene_felica_system_load_node(nfc, new_state, curr_inode);
            consumed = true;
        }

        break;
    }
    default: {
        break;
    }
    }
    return consumed;
}

void nfc_scene_felica_system_on_exit(void* context) {
    Nfc* nfc = context;
    submenu_reset(nfc->submenu);
    return;
}