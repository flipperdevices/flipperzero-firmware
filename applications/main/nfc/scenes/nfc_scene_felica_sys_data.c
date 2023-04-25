#include <inttypes.h>
#include "../nfc_i.h"

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

void nfc_scene_felica_sys_data_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

static inline uint8_t nfc_scene_felica_sys_data_get_sys_index(uint32_t state) {
    return (state >> STATE_SYS_INDEX_SHIFTS) & STATE_SYS_INDEX_MASK;
}

static inline uint8_t nfc_scene_felica_sys_data_get_dump_mode(uint32_t state) {
    return (state >> STATE_DUMP_MODE_SHIFTS) & STATE_DUMP_MODE_MASK;
}

static inline FelicaINode nfc_scene_felica_sys_data_get_inode(uint32_t state) {
    return state & STATE_INODE_MASK;
}

static inline uint32_t nfc_scene_felica_sys_data_set_inode(uint32_t state, FelicaINode inode) {
    furi_assert(inode > FELICA_INODE_INVALID && inode <= FELICA_INODE_MAX);
    return (state & (~STATE_INODE_MASK)) | (inode & FELICA_INODE_MASK);
}

static inline uint32_t nfc_scene_felica_sys_data_set_dump_mode(uint32_t state, uint8_t mode) {
    return (state & (~(STATE_DUMP_MODE_MASK << STATE_DUMP_MODE_SHIFTS))) |
           ((mode & STATE_DUMP_MODE_MASK) << STATE_DUMP_MODE_SHIFTS);
}

static void nfc_scene_felica_sys_data_load_node(
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
                nfc_scene_felica_sys_data_submenu_callback,
                nfc);
        }

        submenu_add_item(
            submenu,
            "Area Info",
            SubmenuIndexAreaInfo,
            nfc_scene_felica_sys_data_submenu_callback,
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
                    nfc_scene_felica_sys_data_submenu_callback,
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

static void nfc_scene_felica_sys_data_load_lite_info(Nfc* nfc, FelicaLiteInfo* lite_info) {
    furi_assert(nfc != NULL);
    furi_assert(lite_info != NULL);

    TextBox* text_box = nfc->text_box;
    FuriString* text_box_store = nfc->text_box_store;
    furi_string_reset(text_box_store);

    uint8_t* data;
    furi_string_cat(text_box_store, "S_PAD:\n");
    for(int i = 0; i < REG_LITE_BLOCK; i++) {
        data = lite_info->S_PAD[i];
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(text_box_store, '\n');
            }
            if(data != NULL) {
                furi_string_cat_printf(text_box_store, "%02X%02X ", data[i], data[i + 1]);
            } else {
                furi_string_cat(text_box_store, "???? ");
            }
        }
    }

    furi_string_cat(text_box_store, "REG:\n");
    data = lite_info->REG;
    for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
        if(!(i % 8) && i) {
            furi_string_push_back(text_box_store, '\n');
        }
        if(data != NULL) {
            furi_string_cat_printf(text_box_store, "%02X%02X ", data[i], data[i + 1]);
        } else {
            furi_string_cat(text_box_store, "???? ");
        }
    }

    furi_string_cat(text_box_store, "MAC:\n");
    data = lite_info->MAC;
    for(uint16_t i = 0; i < 8; i += 2) {
        if(!(i % 8) && i) {
            furi_string_push_back(text_box_store, '\n');
        }
        furi_string_cat_printf(text_box_store, "%02X%02X ", data[i], data[i + 1]);
    }

    furi_string_cat_printf(text_box_store, "DFC: %04X\n", lite_info->data_format_code);

    furi_string_cat(text_box_store, "ID data:\n");
    data = lite_info->ID_value;
    for(uint16_t i = 0; i < 6; i += 2) {
        furi_string_cat_printf(text_box_store, "%02X%02X ", data[i], data[i + 1]);
    }
    furi_string_cat(text_box_store, "\n");

    furi_string_cat_printf(text_box_store, "CKV: %04X\n", lite_info->card_key_version);

    furi_string_cat(text_box_store, "MC:\n");
    data = lite_info->memory_config;
    for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
        if(!(i % 8) && i) {
            furi_string_push_back(text_box_store, '\n');
        }
        furi_string_cat_printf(text_box_store, "%02X%02X ", data[i], data[i + 1]);
    }

    furi_string_cat_printf(text_box_store, "WCNT: %06" PRIX32 "\n", lite_info->write_count);

    furi_string_cat(text_box_store, "MAC_A:\n");
    data = lite_info->MAC_A;
    for(uint16_t i = 0; i < 8; i += 2) {
        if(!(i % 8) && i) {
            furi_string_push_back(text_box_store, '\n');
        }
        furi_string_cat_printf(text_box_store, "%02X%02X ", data[i], data[i + 1]);
    }
    text_box_set_text(text_box, furi_string_get_cstr(text_box_store));
}

static void nfc_scene_felica_sys_data_load_service_hexdump(Nfc* nfc, FelicaService* service) {
    furi_assert(service != NULL);
    furi_assert(service->blocks != NULL);
    TextBox* text_box = nfc->text_box;
    FuriString* text_box_store = nfc->text_box_store;
    furi_string_reset(text_box_store);

    felica_std_hexdump_blocks(service->blocks, text_box_store);
    text_box_set_text(text_box, furi_string_get_cstr(text_box_store));
}

void nfc_scene_felica_sys_data_on_enter(void* context) {
    Nfc* nfc = context;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneFelicaSysData);

    FelicaINode curr_inode = nfc_scene_felica_sys_data_get_inode(state);
    uint8_t curr_system_index = nfc_scene_felica_sys_data_get_sys_index(state);
    FelicaSystem* curr_system =
        FelicaSystemArray_get(nfc->dev->dev_data.felica_data.systems, curr_system_index);
    furi_assert(curr_system != NULL);

    if(curr_system->is_lite) {
        furi_assert(curr_system->code == LITE_SYSTEM_CODE);
        nfc_scene_felica_sys_data_load_lite_info(nfc, &curr_system->lite_info);
        text_box_set_font(nfc->text_box, TextBoxFontHex);
        view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
        return;
    }

    if(curr_system->is_monolithic_ndef) {
        FelicaNode* ndef_node = &curr_system->ndef_node;
        furi_assert(ndef_node->type == FelicaNodeTypeService);
        furi_assert(ndef_node->service != NULL);
        nfc_scene_felica_sys_data_load_service_hexdump(nfc, ndef_node->service);
        text_box_set_font(nfc->text_box, TextBoxFontHex);
        view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
        return;
    }

    FelicaNode* curr_node = felica_std_inode_lookup(curr_system, curr_inode);
    furi_assert(curr_node != NULL);

    nfc_scene_felica_sys_data_load_node(
        nfc, curr_system, curr_node, FELICA_INODE_INVALID, DumpModeNodeContent);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
    return;
}

bool nfc_scene_felica_sys_data_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    FuriHalNfcDevData* nfc_data = &nfc->dev->dev_data.nfc_data;
    FelicaData* felica_data = &nfc->dev->dev_data.felica_data;

    SceneManager* scene_manager = nfc->scene_manager;
    ViewDispatcher* view_dispatcher = nfc->view_dispatcher;
    TextBox* text_box = nfc->text_box;
    FuriString* text_box_store = nfc->text_box_store;

    bool consumed = false;
    uint32_t state = scene_manager_get_scene_state(scene_manager, NfcSceneFelicaSysData);

    FelicaINode curr_inode = nfc_scene_felica_sys_data_get_inode(state);
    uint8_t curr_system_index = nfc_scene_felica_sys_data_get_sys_index(state);
    FelicaSystem* curr_system = FelicaSystemArray_get(felica_data->systems, curr_system_index);
    furi_assert(curr_system != NULL);

    // Lite and monolithic NDEF connects to the previous scene directly and does not need navigation.
    // Let parent handler handle the events.
    if(curr_system->is_lite || curr_system->is_monolithic_ndef) {
        return false;
    }

    // Load node info for Standard systems
    FelicaNode* curr_node = felica_std_inode_lookup(curr_system, curr_inode);
    furi_assert(curr_node != NULL);

    switch(event.type) {
    case SceneManagerEventTypeCustom: {
        switch(event.event) {
        case SubmenuIndexSystemInfo: {
            // show system info
            furi_string_reset(text_box_store);

            // system IDm
            uint8_t system_idm_first_byte = nfc_data->uid[0];
            system_idm_first_byte |= curr_system_index << 4;

            furi_string_cat_printf(text_box_store, "system IDm %02X", system_idm_first_byte);
            for(size_t i = 1; i < 8; i++) {
                furi_string_cat_printf(text_box_store, ":%02X", nfc_data->uid[i]);
            }

            text_box_set_text(text_box, furi_string_get_cstr(text_box_store));

            uint32_t new_state =
                nfc_scene_felica_sys_data_set_dump_mode(state, DumpModeSystemInfo);
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSysData, new_state);

            text_box_set_font(text_box, TextBoxFontHex);
            view_dispatcher_switch_to_view(view_dispatcher, NfcViewTextBox);
            break;
        }
        case SubmenuIndexAreaInfo: {
            // show area info
            furi_string_reset(text_box_store);

            felica_std_describe_node_detailed(curr_node, text_box_store);
            text_box_set_text(text_box, furi_string_get_cstr(text_box_store));

            uint32_t new_state = nfc_scene_felica_sys_data_set_dump_mode(state, DumpModeAreaInfo);
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSysData, new_state);

            text_box_set_font(text_box, TextBoxFontHex);
            view_dispatcher_switch_to_view(view_dispatcher, NfcViewTextBox);
            break;
        }
        default: {
            // list area or show service
            size_t selected_index = event.event - SubmenuIndexDynamic;
            FelicaINode* selected_inode_p =
                FelicaINodeArray_get(curr_node->area->nodes, selected_index);
            furi_assert(selected_inode_p != NULL);
            FelicaINode selected_inode = *selected_inode_p;
            FelicaNode* next_node = felica_std_inode_lookup(curr_system, selected_inode);
            furi_assert(next_node != NULL);
            uint32_t new_state = nfc_scene_felica_sys_data_set_inode(state, selected_inode);
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSysData, new_state);
            nfc_scene_felica_sys_data_load_node(
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
        uint8_t prev_dump_mode = nfc_scene_felica_sys_data_get_dump_mode(state);
        switch(prev_dump_mode) {
        case DumpModeAreaInfo:
        case DumpModeSystemInfo: {
            uint32_t new_state =
                nfc_scene_felica_sys_data_set_dump_mode(state, DumpModeNodeContent);
            scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSysData, new_state);
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
                uint32_t new_state = nfc_scene_felica_sys_data_set_inode(state, parent_inode);
                scene_manager_set_scene_state(scene_manager, NfcSceneFelicaSysData, new_state);
                nfc_scene_felica_sys_data_load_node(
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

void nfc_scene_felica_sys_data_on_exit(void* context) {
    Nfc* nfc = context;
    submenu_reset(nfc->submenu);
    text_box_reset(nfc->text_box);
    furi_string_reset(nfc->text_box_store);
    return;
}
