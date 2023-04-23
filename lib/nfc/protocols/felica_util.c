#include "./felica.h"
#include "./felica_util.h"
#include "core/string.h"
#include <furi.h>

void felica_describe_system(FelicaSystem* system, FuriString* out) {
    uint16_t code = system->code;

    const char* prefix;
    if(code == SUICA_SYSTEM_CODE) {
        // OG SuiCa only
        prefix = "SuiCa";
    } else if(code == CYBERNET_SYSTEM_CODE) {
        // https://web.archive.org/web/20180507210145/http://www.wdic.org/w/RAIL/%E3%82%B5%E3%82%A4%E3%83%90%E3%83%8D%E8%A6%8F%E6%A0%BC%20(IC%E3%82%AB%E3%83%BC%E3%83%89)
        // CJRC == Congress of Japan Railway Cybernetics, also nicknamed Cybernet. Now part of JREA
        // JREA == Japan Railway Engineers' Association
        // Relation to JREM: JREM integrates the hardware/software, JREA/CJRC makes the standard.
        prefix = "CJRC Ticketing";
    } else if(code == NDEF_SYSTEM_CODE) {
        prefix = "NDEF";
    } else if(code == HCE_F_SYSTEM_CODE) {
        prefix = "HCE-F";
    } else if(code == OCTOPUS_SYSTEM_CODE) {
        prefix = "Octopus";
    } else if(code == EDY_SYSTEM_CODE) {
        prefix = "Edy";
    } else if(code == PASPY_SYSTEM_CODE) {
        prefix = "PASPY";
    } else if(code == BLACKBOARD_SYSTEM_CODE) {
        prefix = "Blackboard";
    } else if(code == SAPICA_SYSTEM_CODE) {
        prefix = "SAPICA";
    } else if(code == LITE_SYSTEM_CODE) {
        prefix = "FeliCa Lite";
    } else if(code == RYUTO_SYSTEM_CODE) {
        prefix = "Ryuto";
    } else if(code == OKICA_SYSTEM_CODE) {
        prefix = "OKICA";
    } else if(code == SECURE_ID_SYSTEM_CODE) {
        prefix = "FeliCa Secure ID";
    } else if(code == IRUCA_SYSTEM_CODE) {
        prefix = "IruCa";
    } else if(code == COMMON_AREA_SYSTEM_CODE) {
        prefix = "Common Area";
    } else if(code == PLUG_SYSTEM_CODE) {
        prefix = "FeliCa Plug";
    } else {
        furi_string_cat_printf(out, "System %04X", code);
        return;
    }

    furi_string_cat_printf(out, "%s (%04X)", prefix, code);
}

const char* felica_get_service_type_name(FelicaServiceType type) {
    switch(type) {
    case FelicaServiceTypeRandom:
        return "Random";
    case FelicaServiceTypeCyclic:
        return "Cyclic";
    case FelicaServiceTypePurse:
        return "Purse";
    default:
        return "???";
    }
}

const char* felica_get_service_attrib_name(FelicaServiceAttribute attrib, FelicaServiceType type) {
    bool is_purse = (type == FelicaServiceTypePurse);
    switch(attrib) {
    case FelicaServiceAttributeAuthRW:
        return "auth rw";
    case FelicaServiceAttributeUnauthRW:
        return "rw";
    case FelicaServiceAttributeAuthRO: // also FelicaServiceAttributeAuthCashbackDecrement
        return is_purse ? "auth inc" : "auth ro";
    case FelicaServiceAttributeUnauthRO: // also FelicaServiceAttributeUnauthCashbackDecrement
        return is_purse ? "inc" : "ro";
    case FelicaServiceAttributeAuthDecrement:
        return "auth dec";
    case FelicaServiceAttributeUnauthDecrement:
        return "dec";
    case FelicaServiceAttributeAuthPurseRO:
        return "auth ro";
    case FelicaServiceAttributeUnauthPurseRO:
        return "ro";
    default:
        return "???";
    }
}

void felica_std_describe_node(FelicaNode* node, FuriString* result) {
    switch(node->type) {
    case FelicaNodeTypeArea: {
        furi_assert(node->area != NULL);
        if(node->area->can_create_subareas) {
            furi_string_cat(result, "Area ");
        } else {
            furi_string_cat(result, "Leaf ");
        }
        furi_string_cat_printf(result, "%d", node->area->number);
        break;
    }
    case FelicaNodeTypeService: {
        furi_assert(node->service != NULL);
        const char* type_name = felica_get_service_type_name(node->service->type);
        furi_string_cat_printf(result, "%s %d", type_name, node->service->number);
    }
    }
}

void felica_std_hexdump_blocks(FelicaBlockArray_t blocks, FuriString* out) {
    for
        M_EACH(block, blocks, FelicaBlockArray_t) {
            for(size_t block_offset = 0; block_offset < sizeof(block->data); block_offset += 2) {
                furi_string_cat_printf(
                    out, "%02X%02X ", block->data[block_offset], block->data[block_offset + 1]);
            }
        }
}

void felica_std_cat_service(FelicaService* service, FuriString* out) {
    furi_assert(service != NULL);
    furi_assert(out != NULL);

    // Print access conditions
    furi_string_cat(out, "access (code):");
    for
        M_EACH(acl_p, service->access_control_list, FelicaServiceAttributeSet_t) {
            furi_assert(acl_p != NULL);
            FelicaServiceAttribute acl = *acl_p;
            furi_string_cat_printf(
                out,
                "\n- %s (%04X)",
                felica_get_service_attrib_name(acl, service->type),
                (service->number << 6) | (service->type) | acl);
        };

    furi_string_push_back(out, '\n');

    // If no block to dump, skip the hex dump section entirely
    size_t block_count = FelicaBlockArray_size(service->blocks);
    if(block_count == 0) {
        return;
    }

    // Print size and do a hexdump
    furi_string_cat_printf(
        out, "\nsize %zd (%zdbytes)", block_count, block_count * FELICA_BLOCK_SIZE);

    furi_string_cat(out, "\n\n");
    felica_std_hexdump_blocks(service->blocks, out);
}

void felica_std_describe_node_detailed(FelicaNode* node, FuriString* out) {
    furi_assert(node != NULL);
    felica_std_describe_node(node, out);
    switch(node->type) {
    case FelicaNodeTypeArea: {
        furi_assert(node->area != NULL);
        furi_string_cat_printf(out, "\nsize %zd", FelicaINodeArray_size(node->area->nodes));
        furi_string_cat_printf(
            out,
            "\nrange %04X-%04X",
            (node->area->number << 6) |
                (!node->area->can_create_subareas), // TODO make this its own function
            node->area->end_service_code);
        break;
    }
    case FelicaNodeTypeService: {
        furi_assert(node->service != NULL);
        furi_string_push_back(out, '\n');
        felica_std_cat_service(node->service, out);
        break;
    }
    }
}

void felica_print_card_stat(FelicaData* data, FuriString* out) {
    furi_assert(data != NULL);
    furi_assert(data->systems != NULL);
    furi_assert(out != NULL);

    // Monolithic systems
    if(data->is_monolithic) {
        FelicaSystem* system = FelicaSystemArray_get(data->systems, 0);
        furi_assert(system != NULL);
        // Lite
        if(system->is_lite) {
            furi_assert(!system->is_monolithic_ndef);

            size_t read = 0;
            const size_t total_blocks =
                sizeof(system->lite_info.S_PAD) / sizeof(system->lite_info.S_PAD[0]);
            for(size_t i = 0; i < total_blocks; i++) {
                if(system->lite_info.S_PAD[i] != NULL) {
                    read++;
                }
            }

            furi_string_cat_printf(out, "%zd/%zd blocks read", read, total_blocks);
            return;
        }

        // NDEF aka 12fc
        if(system->is_monolithic_ndef) {
            furi_assert(!system->is_lite);
            furi_assert(system->ndef_node.service != NULL);

            size_t blocks = FelicaBlockArray_size(system->ndef_node.service->blocks);

            furi_string_cat_printf(out, "%zd blocks read", blocks);
            return;
        }

        // Should never reach here
        furi_assert(false);
    }

    // Standard/Hybrid
    size_t num_systems = FelicaSystemArray_size(data->systems);
    size_t num_areas = 0, num_services = 0;
    for
        M_EACH(system, data->systems, FelicaSystemArray_t) {
            furi_assert(system != NULL);
            // Don't count Lite or monolithic NDEF systems as they are completely different.
            if(system->is_lite || system->is_monolithic_ndef) {
                continue;
            }
        for
            M_EACH(node, system->nodes, FelicaNodeArray_t) {
                furi_assert(node != NULL);
                if(node->type == FelicaNodeTypeArea) {
                    num_areas++;
                } else if(node->type == FelicaNodeTypeService) {
                    furi_assert(node->service != NULL);
                    furi_assert(node->service->access_control_list != NULL);
                    num_services +=
                        FelicaServiceAttributeSet_size(node->service->access_control_list);
                }
            }
        }

    furi_string_cat_printf(out, "%zd system", num_systems);
    if(num_systems > 1) {
        furi_string_push_back(out, 's');
    }
    furi_string_cat_printf(out, ", %zd area", num_areas);
    if(num_areas > 1) {
        furi_string_push_back(out, 's');
    }
    furi_string_cat_printf(out, ", %zd service", num_services);
    if(num_services > 1) {
        furi_string_push_back(out, 's');
    }
}

#define SPEC_TRIPLET(sp) felica_spec_major(sp), felica_spec_minor(sp), felica_spec_patch(sp)

void felica_print_card_spec(FelicaSpec* spec, FuriString* out) {
    furi_assert(spec != NULL);
    furi_assert(out != NULL);
    bool has_options = false;

    if(!felica_spec_valid(spec->basic_version)) {
        furi_string_cat(out, "version unknown");
        return;
    }
    furi_string_cat_printf(out, "version %d.%d.%d", SPEC_TRIPLET(spec->basic_version));

    furi_string_cat(out, "\noptions:");

    if(felica_spec_valid(spec->des_version)) {
        furi_string_cat_printf(out, "\n- des %d.%d.%d", SPEC_TRIPLET(spec->des_version));
        has_options = true;
    }

    if(felica_spec_valid(spec->ext_overlap_version)) {
        furi_string_cat_printf(
            out, "\n- xoverlap %d.%d.%d", SPEC_TRIPLET(spec->ext_overlap_version));
        has_options = true;
    }

    if(felica_spec_valid(spec->value_limited_purse_version)) {
        furi_string_cat_printf(out, "\n- vlp %d.%d.%d", SPEC_TRIPLET(spec->des_version));
        has_options = true;
    }

    if(felica_spec_valid(spec->comm_with_mac_version)) {
        furi_string_cat_printf(out, "\n- cwmac %d.%d.%d", SPEC_TRIPLET(spec->des_version));
        has_options = true;
    }

    if(!has_options) {
        furi_string_cat(out, "\n- none");
    }
}