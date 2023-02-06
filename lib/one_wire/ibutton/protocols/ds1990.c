#include "ds1990.h"

#include <core/string.h>
#include <core/core_defines.h>

#include "dallas_common.h"

#define DS1990_FAMILY_CODE 0x01U

static bool dallas_ds1990_read(OneWireHost*, void*);
static void dallas_ds1990_render_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1990_render_brief_data(FuriString*, const iButtonProtocolData*);

const iButtonProtocolBase ibutton_protocol_ds1990 = {
    .family_code = DS1990_FAMILY_CODE,
    .data_size = sizeof(DallasCommonRomData),

    .read = dallas_ds1990_read,
    .render_data = dallas_ds1990_render_data,
    .render_brief_data = dallas_ds1990_render_brief_data,
};

bool dallas_ds1990_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    DallasCommonRomData *rom_data = protocol_data;
    return dallas_common_read_rom(host, rom_data);
}

void dallas_ds1990_render_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DallasCommonRomData* rom_data = protocol_data;

    for(size_t i = 0; i < sizeof(rom_data->bytes); ++i) {
        furi_string_cat_printf(result, "%02X ", rom_data->bytes[i]);
    }
}

void dallas_ds1990_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DallasCommonRomData* rom_data = protocol_data;

    for(size_t i = 0; i < sizeof(rom_data->bytes); ++i) {
        furi_string_cat_printf(result, "%02X ", rom_data->bytes[i]);
    }
}
