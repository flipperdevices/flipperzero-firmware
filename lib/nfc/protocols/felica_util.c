#include "./felica.h"
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
    if(code == SUICA_SYSTEM_CODE) {
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