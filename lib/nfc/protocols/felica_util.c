#include "./felica.h"
#include "core/string.h"
#include <furi.h>

static const uint32_t TIME_CONSTANT_US = 302;

// TODO move this to felica.c
uint_least32_t felica_estimate_timing_us(uint_least8_t timing, uint_least8_t units) {
    uint_least32_t base_cost_factor = 1 + (timing & 0x7);
    uint_least32_t unit_cost_factor = 1 + ((timing >> 3) & 0x7);
    uint_least32_t scale = 1 << ((timing >> 6) * 2);
    return TIME_CONSTANT_US * scale * (base_cost_factor + unit_cost_factor * units);
}

FuriString* felica_get_system_name(FelicaSystem* system) {
    uint16_t code = system->code;

    const char* prefix;
    if(code == SUICA_SYSTEM_CODE || code == 0x0003) {
        prefix = "SuiCa";
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
        return furi_string_alloc_printf("System %04X", code);
    }

    return furi_string_alloc_printf("%s (%04X)", prefix, code);
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

void felica_std_cat_service(FelicaService* service, FuriString* out) {
    furi_assert(service != NULL);
    furi_assert(out != NULL);

    furi_string_cat(out, "access (code):");
    for
        M_EACH(acl_p, service->access_control_list, FelicaServiceAttributeList_t) {
            furi_assert(acl_p != NULL);
            FelicaServiceAttribute acl = *acl_p;
            furi_string_cat_printf(
                out,
                "\n- %s (%04X)",
                felica_get_service_attrib_name(acl, service->type),
                (service->number << 6) | (service->type) | acl);
        };

    furi_string_push_back(out, '\n');

    size_t block_count = FelicaBlockArray_size(service->blocks);
    if(block_count > 0) {
        furi_string_cat_printf(
            out, "\nsize %zd (%zdbytes)", block_count, block_count * FELICA_BLOCK_SIZE);
    }

    furi_string_push_back(out, '\n');
    for
        M_EACH(block, service->blocks, FelicaBlockArray_t) {
            for(size_t block_offset = 0; block_offset < sizeof(block->data); block_offset += 2) {
                if(block_offset % 8 == 0) {
                    furi_string_push_back(out, '\n');
                }
                furi_string_cat_printf(
                    out, "%02X%02X ", block->data[block_offset], block->data[block_offset + 1]);
            }
        }
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