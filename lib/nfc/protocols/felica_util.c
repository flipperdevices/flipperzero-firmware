#include "./felica.h"
#include <furi.h>

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