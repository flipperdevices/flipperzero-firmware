#include "../nfc_i.h"
#include "core/string.h"
#include "gui/modules/submenu.h"
#include "gui/modules/text_box.h"
#include "gui/scene_manager.h"
#include "gui/view_dispatcher.h"
#include "protocols/felica.h"
#include <dolphin/dolphin.h>

// State bit format: 0000000000ddssssnnnnnnnnnnnnnnnn
// s: system index
// n: inode
// d: dump mode (00 = dump node content, 01 = dump area info, 10 = dump system info, 11 = reserved)
// 0: reserved. should be 0

#define STATE_SYS_INDEX_SHIFTS 16
#define STATE_SYS_INDEX_MASK 0xf
#define STATE_DUMP_MODE_SHIFTS 20
#define STATE_DUMP_MODE_MASK 0b11
#define STATE_INODE_MASK FELICA_INODE_MASK

#define TAG "FelicaSystemData"

enum SubmenuIndex {
    SubmenuIndexSystemInfo = 0,
    SubmenuIndexAreaInfo,
    SubmenuIndexDynamic,
};

enum DumpMode {
    DumpModeNodeContent = 0,
    DumpModeAreaInfo,
    DumpModeSystemInfo,
};

void nfc_scene_felica_system_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

static inline uint8_t nfc_scene_felica_system_get_sys_index(uint32_t state) {
    return (state >> STATE_SYS_INDEX_SHIFTS) & STATE_SYS_INDEX_MASK;
}

static inline uint8_t nfc_scene_felica_system_get_dump_mode(uint32_t state) {
    return (state >> STATE_DUMP_MODE_SHIFTS) & STATE_DUMP_MODE_MASK;
}

static inline FelicaINode nfc_scene_felica_system_get_inode(uint32_t state) {
    return state & STATE_INODE_MASK;
}

static inline uint32_t nfc_scene_felica_system_set_inode(uint32_t state, FelicaINode inode) {
    furi_assert(inode > FELICA_INODE_INVALID && inode <= FELICA_INODE_MAX);
    return (state & (~STATE_INODE_MASK)) | (inode & FELICA_INODE_MASK);
}

static inline uint32_t nfc_scene_felica_system_set_dump_mode(uint32_t state, uint8_t mode) {
    return (state & (~(STATE_DUMP_MODE_MASK << STATE_DUMP_MODE_SHIFTS))) |
           ((mode & STATE_DUMP_MODE_MASK) << STATE_DUMP_MODE_SHIFTS);
}

static void nfc_scene_felica_system_load_node(
    Nfc* nfc,
    FelicaSystem* system,
    FelicaNode* node,
    FelicaINode seek_to_inode,
    uint8_t back_from_dump_mode) {
    furi_assert(system != NULL);
    furi_assert(node != NULL);

    switch(node->type) {
    case FelicaNodeTypeArea: {
        furi_assert(node->area != NULL);

        Submenu* submenu = nfc->submenu;
        submenu_reset(submenu);

        if(node->parent == FELICA_INODE_INVALID) {
            submenu_add_item(
                submenu,
                "System Info",
                SubmenuIndexSystemInfo,
                nfc_scene_felica_system_submenu_callback,
                nfc);
        }

        submenu_add_item(
            submenu,
            "Area Info",
            SubmenuIndexAreaInfo,
            nfc_scene_felica_system_submenu_callback,
            nfc);

        FuriString* subnode_desc = furi_string_alloc();

        uint32_t i = SubmenuIndexDynamic, seek = 0;

        switch(back_from_dump_mode) {
        case DumpModeAreaInfo:
            seek = SubmenuIndexAreaInfo;
            break;
        case DumpModeSystemInfo:
            seek = SubmenuIndexSystemInfo;
            break;
        default:
            break;
        }

        for
            M_EACH(subnode_inode_p, node->area->nodes, FelicaINodeArray_t) {
                furi_assert(subnode_inode_p != NULL);
                FelicaINode subnode_inode = *subnode_inode_p;
                FelicaNode* subnode = felica_std_inode_lookup(system, subnode_inode);
                if(seek_to_inode != FELICA_INODE_INVALID && subnode_inode == seek_to_inode &&
                   back_from_dump_mode == DumpModeNodeContent) {
                    seek = i;
                }
                felica_std_describe_node(subnode, subnode_desc);
                submenu_add_item(
                    submenu,
                    furi_string_get_cstr(subnode_desc),
                    i,
                    nfc_scene_felica_system_submenu_callback,
                    nfc);
                i++;
                if(i > NfcCustomEventReserved) {
                    FURI_LOG_W(
                        TAG,
                        "Number of subnodes exceed maximum allowed number. List will be truncated");
                }
                furi_string_reset(subnode_desc);
            }
        furi_string_free(subnode_desc);
        if(seek_to_inode != FELICA_INODE_INVALID || back_from_dump_mode != DumpModeNodeContent) {
            submenu_set_selected_item(submenu, seek);
        }
        break;
    }
    case FelicaNodeTypeService: {
        furi_assert(node->service != NULL);
        TextBox* text_box = nfc->text_box;
        FuriString* text_box_store = nfc->text_box_store;
        furi_string_reset(text_box_store);

        felica_std_describe_node_detailed(node, text_box_store);
        text_box_set_text(text_box, furi_string_get_cstr(text_box_store));
        break;
    }
    }
}

void nfc_scene_felica_system_on_enter(void* context) {
    Nfc* nfc = context;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneFelicaSystem);

    FelicaINode curr_inode = nfc_scene_felica_system_get_inode(state);
    uint8_t curr_system_index = nfc_scene_felica_system_get_sys_index(state);
    FelicaSystem* curr_system =
        FelicaSystemArray_get(nfc->dev->dev_data.felica_data.systems, curr_system_index);
    furi_assert(curr_system != NULL);
    // TODO monolithic system handling here
    FelicaNode* curr_node = felica_std_inode_lookup(curr_system, curr_inode);
    furi_assert(curr_node != NULL);

    nfc_scene_felica_system_load_node(
        nfc, curr_system, curr_node, FELICA_INODE_INVALID, DumpModeNodeContent);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
    return;
}

bool nfc_scene_felica_system_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    SceneManager* scene_manager = nfc->scene_manager;
    ViewDispatcher* view_dispatcher = nfc->view_dispatcher;
    TextBox* text_box = nfc->text_box;
    FuriString* text_box_store = nfc->text_box_store;

    bool consumed = false;
    uint32_t state = scene_manager_get_scene_state(scene_manager, NfcSceneFelicaSystem);

    FelicaINode curr_inode = nfc_scene_felica_system_get_inode(state);
    uint8_t curr_system_index = nfc_scene_felica_system_get_sys_index(state);
    FelicaSystem* curr_system =
        FelicaSystemArray_get(nfc->dev->dev_data.felica_data.systems, curr_system_index);
    furi_assert(curr_system != NULL);
    FelicaNode* curr_node = felica_std_inode_lookup(curr_system, curr_inode);
    furi_assert(curr_node != NULL);

    switch(event.type) {
    case SceneManagerEventTypeCustom: {
        switch(event.event) {
        case SubmenuIndexSystemInfo: {
            // TODO show system info
            break;
        }
        case SubmenuIndexAreaInfo: {
            // TODO show area info
            furi_string_reset(text_box_store);

            felica_std_describe_node_detailed(curr_node, text_box_store);
            text_box_set_text(text_box, furi_string_get_cstr(text_box_store));

            uint32_t new_state = nfc_scene_felica_system_set_dump_mode(state, DumpModeAreaInfo);
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSystem, new_state);

            text_box_set_font(text_box, TextBoxFontHex);
            view_dispatcher_switch_to_view(view_dispatcher, NfcViewTextBox);
            break;
        }
        default: {
            size_t selected_index = event.event - SubmenuIndexDynamic;
            FelicaINode* selected_inode_p =
                FelicaINodeArray_get(curr_node->area->nodes, selected_index);
            furi_assert(selected_inode_p != NULL);
            FelicaINode selected_inode = *selected_inode_p;
            FelicaNode* next_node = felica_std_inode_lookup(curr_system, selected_inode);
            furi_assert(next_node != NULL);
            uint32_t new_state = nfc_scene_felica_system_set_inode(state, selected_inode);
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSystem, new_state);
            nfc_scene_felica_system_load_node(
                nfc, curr_system, next_node, FELICA_INODE_INVALID, DumpModeNodeContent);
            if(next_node->type == FelicaNodeTypeService) {
                text_box_set_font(text_box, TextBoxFontHex);
                view_dispatcher_switch_to_view(view_dispatcher, NfcViewTextBox);
            }
            consumed = true;
        }
        }
        break;
    }
    case SceneManagerEventTypeBack: {
        uint8_t prev_dump_mode = nfc_scene_felica_system_get_dump_mode(state);
        switch(prev_dump_mode) {
        case DumpModeAreaInfo:
        case DumpModeSystemInfo: {
            uint32_t new_state = nfc_scene_felica_system_set_dump_mode(state, DumpModeNodeContent);
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSystem, new_state);
            nfc_scene_felica_system_load_node(
                nfc, curr_system, curr_node, FELICA_INODE_INVALID, prev_dump_mode);
            view_dispatcher_switch_to_view(view_dispatcher, NfcViewMenu);
            consumed = true;
            break;
        }
        case DumpModeNodeContent:
        default: {
            FelicaINode parent_inode = curr_node->parent;
            if(parent_inode != FELICA_INODE_INVALID) {
                FelicaNode* next_node = felica_std_inode_lookup(curr_system, parent_inode);
                furi_assert(next_node != NULL);
                uint32_t new_state = nfc_scene_felica_system_set_inode(state, parent_inode);
                scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSystem, new_state);
                nfc_scene_felica_system_load_node(
                    nfc, curr_system, next_node, curr_inode, DumpModeNodeContent);
                // Returning from a service to area
                if(curr_node->type == FelicaNodeTypeService) {
                    view_dispatcher_switch_to_view(view_dispatcher, NfcViewMenu);
                }
                consumed = true;
            }
            break;
        }
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
    text_box_reset(nfc->text_box);
    return;
}
