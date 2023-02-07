#include "ds1992.h"

#include <core/core_defines.h>

#include "dallas_common.h"

#define DS1992_FAMILY_CODE 0x08U
#define DS1992_SRAM_DATA_SIZE 128U

#define DS1992_BRIEF_HEAD_COUNT 4U
#define DS1992_BRIEF_TAIL_COUNT 3U

typedef struct {
    DallasCommonRomData rom_data;
    uint8_t sram_data[DS1992_SRAM_DATA_SIZE];
} DS1992ProtocolData;

static bool dallas_ds1992_read(OneWireHost* host, void* protocol_data);
static void dallas_ds1992_render_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1992_render_brief_data(FuriString*, const iButtonProtocolData*);

const iButtonProtocolBase ibutton_protocol_ds1992 = {
    .family_code = DS1992_FAMILY_CODE,
    .data_size = sizeof(DS1992ProtocolData),
    .manufacturer = "Dallas",
    .name = "DS1992",

    .read = dallas_ds1992_read,
    .render_data = dallas_ds1992_render_data,
    .render_brief_data = dallas_ds1992_render_brief_data,
};

bool dallas_ds1992_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    bool success = false;
    DS1992ProtocolData *data = protocol_data;

    do {
        if(!dallas_common_read_rom(host, &data->rom_data)) {
            break;
        }

        if(!dallas_common_read_mem(host, 0, data->sram_data, DS1992_SRAM_DATA_SIZE)) {
            break;
        }

        success = true;
    } while(false);

    return success;
}

void dallas_ds1992_render_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1992ProtocolData *data = protocol_data;

    for(size_t i = 0; i < DS1992_SRAM_DATA_SIZE; ++i) {
        const char* fmt = (i < DS1992_SRAM_DATA_SIZE - 1) ? "%02X " : "%02X";
        furi_string_cat_printf(result, fmt, data->sram_data[i]);
    }
}

void dallas_ds1992_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1992ProtocolData *data = protocol_data;

    for(size_t i = 0; i < sizeof(data->rom_data.bytes); ++i) {
        furi_string_cat_printf(result, "%02X ", data->rom_data.bytes[i]);
    }

    furi_string_cat_printf(result, "\nInternal SRAM: %zu Kbit\n", DS1992_SRAM_DATA_SIZE * 8U / 1024U);

    for(size_t i = 0; i < DS1992_BRIEF_HEAD_COUNT; ++i) {
        furi_string_cat_printf(result, "%02X ", data->sram_data[i]);
    }

    furi_string_cat_printf(result, "[  . . .  ]");

    for(size_t i = DS1992_SRAM_DATA_SIZE - DS1992_BRIEF_TAIL_COUNT; i < DS1992_SRAM_DATA_SIZE; ++i) {
        furi_string_cat_printf(result, " %02X", data->sram_data[i]);
    }
}
