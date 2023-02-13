#include "ds1992.h"

#include <core/core_defines.h>
#include <toolbox/pretty_format.h>

#include "dallas_common.h"

#define DS1992_FAMILY_CODE 0x08U
#define DS1992_FAMILY_NAME "DS1992"

#define DS1992_SRAM_DATA_SIZE 128U

#define DS1992_BRIEF_HEAD_COUNT 4U
#define DS1992_BRIEF_TAIL_COUNT 3U

#define DS1992_DATA_ROW_LENGTH 7U

#define DS1992_SRAM_DATA_KEY "Sram Data"

#define BITS_IN_BYTE 8U
#define BITS_IN_KBIT 1024U

typedef struct {
    OneWireSlave* slave;
} DS1992ProtocolState;

typedef struct {
    DallasCommonRomData rom_data;
    uint8_t sram_data[DS1992_SRAM_DATA_SIZE];
    DS1992ProtocolState state;
} DS1992ProtocolData;

static bool dallas_ds1992_read(OneWireHost*, void*);
static void dallas_ds1992_emulate(OneWireSlave*, iButtonProtocolData*);
static bool dallas_ds1992_load(FlipperFormat*, uint32_t, iButtonProtocolData*);
static bool dallas_ds1992_save(FlipperFormat*, const iButtonProtocolData*);
static void dallas_ds1992_render_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1992_render_brief_data(FuriString*, const iButtonProtocolData*);
static bool dallas_ds1992_is_data_valid(const iButtonProtocolData*);

const iButtonProtocolBase ibutton_protocol_ds1992 = {
    .family_code = DS1992_FAMILY_CODE,
    .features = iButtonProtocolFeatureExtData | iButtonProtocolFeatureWriteBlank |
                iButtonProtocolFeatureWriteCopy,
    .data_size = sizeof(DS1992ProtocolData),
    .manufacturer = DALLAS_COMMON_MANUFACTURER_NAME,
    .name = DS1992_FAMILY_NAME,

    .read = dallas_ds1992_read,
    .emulate = dallas_ds1992_emulate,
    .save = dallas_ds1992_save,
    .load = dallas_ds1992_load,
    .render_data = dallas_ds1992_render_data,
    .render_brief_data = dallas_ds1992_render_brief_data,
    .is_valid = dallas_ds1992_is_data_valid,
};

bool dallas_ds1992_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    bool success = false;
    DS1992ProtocolData* data = protocol_data;

    do {
        if(!dallas_common_read_rom(host, &data->rom_data)) break;
        if(!dallas_common_read_mem(host, 0, data->sram_data, DS1992_SRAM_DATA_SIZE)) break;
        success = true;
    } while(false);

    return success;
}

static bool dallas_ds1992_emulate_callback(uint8_t command, void* context) {
    furi_assert(context);
    DS1992ProtocolData* data = context;
    OneWireSlave* slave = data->state.slave;

    switch(command) {
    case DALLAS_COMMON_CMD_SEARCH_ROM:
        // TODO: respond with internal SRAM if a ROM command was performed
        return dallas_common_emulate_search_rom(slave, &data->rom_data);
    case DALLAS_COMMON_CMD_READ_ROM:
        // TODO: set the state to indicate that a ROM command has occured
        return dallas_common_emulate_read_rom(slave, &data->rom_data);
    default:
        return false;
    }
}

void dallas_ds1992_emulate(OneWireSlave* slave, iButtonProtocolData* protocol_data) {
    DS1992ProtocolData* data = protocol_data;
    data->state.slave = slave;
    // TODO: reset the command state

    onewire_slave_set_command_callback(slave, dallas_ds1992_emulate_callback, protocol_data);
    onewire_slave_start(slave);
}

bool dallas_ds1992_load(
    FlipperFormat* ff,
    uint32_t format_version,
    iButtonProtocolData* protocol_data) {
    DS1992ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(format_version < 2) break;
        if(!dallas_common_load_rom_data(ff, format_version, &data->rom_data)) break;
        if(!flipper_format_read_hex(
               ff, DS1992_SRAM_DATA_KEY, data->sram_data, DS1992_SRAM_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

bool dallas_ds1992_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data) {
    const DS1992ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(!dallas_common_save_rom_data(ff, &data->rom_data)) break;
        if(!flipper_format_write_hex(
               ff, DS1992_SRAM_DATA_KEY, data->sram_data, DS1992_SRAM_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

void dallas_ds1992_render_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1992ProtocolData* data = protocol_data;
    pretty_format_bytes_hex(
        result, DS1992_DATA_ROW_LENGTH, data->sram_data, DS1992_SRAM_DATA_SIZE);
}

void dallas_ds1992_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1992ProtocolData* data = protocol_data;

    for(size_t i = 0; i < sizeof(data->rom_data.bytes); ++i) {
        furi_string_cat_printf(result, "%02X ", data->rom_data.bytes[i]);
    }

    furi_string_cat_printf(
        result,
        "\nInternal SRAM: %zu Kbit\n",
        (size_t)(DS1992_SRAM_DATA_SIZE * BITS_IN_BYTE / BITS_IN_KBIT));

    for(size_t i = 0; i < DS1992_BRIEF_HEAD_COUNT; ++i) {
        furi_string_cat_printf(result, "%02X ", data->sram_data[i]);
    }

    furi_string_cat_printf(result, "[  . . .  ]");

    for(size_t i = DS1992_SRAM_DATA_SIZE - DS1992_BRIEF_TAIL_COUNT; i < DS1992_SRAM_DATA_SIZE;
        ++i) {
        furi_string_cat_printf(result, " %02X", data->sram_data[i]);
    }
}

void dallas_ds1992_render_error(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1992ProtocolData* data = protocol_data;

    if(!dallas_common_is_valid_crc(&data->rom_data)) {
        furi_string_printf(result, "CRC Error");
    }
}

bool dallas_ds1992_is_data_valid(const iButtonProtocolData* protocol_data) {
    const DS1992ProtocolData* data = protocol_data;
    return dallas_common_is_valid_crc(&data->rom_data);
}
