#include "ds1990.h"

#include <core/string.h>
#include <core/core_defines.h>

#include "dallas_common.h"

#define DS1990_FAMILY_CODE 0x01U

static bool dallas_ds1990_read(OneWireHost*, void*);
static bool dallas_ds1990_load(FlipperFormat*, uint32_t, iButtonProtocolData*);
static bool dallas_ds1990_save(FlipperFormat*, const iButtonProtocolData*);
static void dallas_ds1990_render_brief_data(FuriString*, const iButtonProtocolData*);
static bool dallas_ds1990_is_valid(const iButtonProtocolData*);

const iButtonProtocolBase ibutton_protocol_ds1990 = {
    .family_code = DS1990_FAMILY_CODE,
    .features = iButtonProtocolFeatureWriteBlank,
    .data_size = sizeof(DallasCommonRomData),
    .manufacturer = "Dallas",
    .name = "DS1990",

    .read = dallas_ds1990_read,
    .save = dallas_ds1990_save,
    .load = dallas_ds1990_load,
    .render_data = NULL, /* No data to render */
    .render_brief_data = dallas_ds1990_render_brief_data,
    .is_valid = dallas_ds1990_is_valid,
};

bool dallas_ds1990_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    DallasCommonRomData *rom_data = protocol_data;
    return dallas_common_read_rom(host, rom_data);
}

bool dallas_ds1990_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data) {
    const DallasCommonRomData *rom_data = protocol_data;
    return dallas_common_save_rom_data(ff, rom_data);
}

bool dallas_ds1990_load(FlipperFormat* ff, uint32_t format_version, iButtonProtocolData* protocol_data) {
    DallasCommonRomData *rom_data = protocol_data;
    return dallas_common_load_rom_data(ff, format_version, rom_data);
}

void dallas_ds1990_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DallasCommonRomData* rom_data = protocol_data;

    for(size_t i = 0; i < sizeof(rom_data->bytes); ++i) {
        furi_string_cat_printf(result, "%02X ", rom_data->bytes[i]);
    }
}

bool dallas_ds1990_is_valid(const iButtonProtocolData* protocol_data) {
    const DallasCommonRomData* rom_data = protocol_data;
    return dallas_common_is_valid_crc(rom_data);
}
