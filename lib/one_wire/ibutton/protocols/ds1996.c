#include "ds1996.h"

#include <core/core_defines.h>
#include <toolbox/pretty_format.h>

#include "dallas_common.h"

#include "blanks/rw1990.h"
#include "blanks/tm2004.h"

#define DS1996_FAMILY_CODE 0x0CU
#define DS1996_FAMILY_NAME "DS1996"

#define DS1996_SRAM_DATA_SIZE 8192U
#define DS1996_SRAM_PAGE_SIZE 32U
#define DS1996_COPY_SCRATCH_TIMEOUT_US 100U

#define DS1996_BRIEF_HEAD_COUNT 4U
#define DS1996_BRIEF_TAIL_COUNT 3U

#define DS1996_DATA_BYTE_COUNT 4U

#define DS1996_SRAM_DATA_KEY "Sram Data"

#define BITS_IN_BYTE 8U
#define BITS_IN_KBIT 1024U

typedef enum {
    DS1996CommandStateIdle,
    DS1996CommandStateRomCmd,
    DS1996CommandStateMemCmd,
} DS1996CommandState;

typedef struct {
    OneWireSlave* bus;
    DS1996CommandState command_state;
} DS1996ProtocolState;

typedef struct {
    DallasCommonRomData rom_data;
    uint8_t sram_data[DS1996_SRAM_DATA_SIZE];
    DS1996ProtocolState state;
} DS1996ProtocolData;

static bool dallas_ds1996_read(OneWireHost*, void*);
static bool dallas_ds1996_write_copy(OneWireHost*, iButtonProtocolData*);
static void dallas_ds1996_emulate(OneWireSlave*, iButtonProtocolData*);
static bool dallas_ds1996_load(FlipperFormat*, uint32_t, iButtonProtocolData*);
static bool dallas_ds1996_save(FlipperFormat*, const iButtonProtocolData*);
static void dallas_ds1996_render_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1996_render_brief_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1996_render_error(FuriString*, const iButtonProtocolData*);
static bool dallas_ds1996_is_data_valid(const iButtonProtocolData*);

const iButtonProtocolBase ibutton_protocol_ds1996 = {
    .family_code = DS1996_FAMILY_CODE,
    .features = iButtonProtocolFeatureExtData | iButtonProtocolFeatureWriteCopy,
    .data_size = sizeof(DS1996ProtocolData),
    .manufacturer = DALLAS_COMMON_MANUFACTURER_NAME,
    .name = DS1996_FAMILY_NAME,

    .read = dallas_ds1996_read,
    .write_blank = NULL, /* Data too big for known blanks */
    .write_copy = dallas_ds1996_write_copy,
    .emulate = dallas_ds1996_emulate,
    .save = dallas_ds1996_save,
    .load = dallas_ds1996_load,
    .render_data = dallas_ds1996_render_data,
    .render_brief_data = dallas_ds1996_render_brief_data,
    .render_error = dallas_ds1996_render_error,
    .is_valid = dallas_ds1996_is_data_valid,
};

bool dallas_ds1996_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    DS1996ProtocolData* data = protocol_data;
    return onewire_host_reset(host) && dallas_common_read_rom(host, &data->rom_data) &&
           dallas_common_read_mem(host, 0, data->sram_data, DS1996_SRAM_DATA_SIZE);
}

bool dallas_ds1996_write_copy(OneWireHost* host, iButtonProtocolData* protocol_data) {
    DS1996ProtocolData* data = protocol_data;
    return dallas_common_write_mem(
        host,
        DS1996_COPY_SCRATCH_TIMEOUT_US,
        DS1996_SRAM_PAGE_SIZE,
        data->sram_data,
        DS1996_SRAM_DATA_SIZE);
}

static void dallas_ds1996_reset_callback(void* context) {
    furi_assert(context);
    DS1996ProtocolData* data = context;
    data->state.command_state = DS1996CommandStateIdle;
}

static bool dallas_ds1996_command_callback(uint8_t command, void* context) {
    furi_assert(context);
    DS1996ProtocolData* data = context;
    OneWireSlave* bus = data->state.bus;

    switch(command) {
    case DALLAS_COMMON_CMD_SEARCH_ROM:
        if(data->state.command_state == DS1996CommandStateIdle) {
            data->state.command_state = DS1996CommandStateRomCmd;
            return dallas_common_emulate_search_rom(bus, &data->rom_data);

        } else if(data->state.command_state == DS1996CommandStateRomCmd) {
            data->state.command_state = DS1996CommandStateMemCmd;
            dallas_common_emulate_read_mem(bus, data->sram_data, DS1996_SRAM_DATA_SIZE);
            return false;

        } else {
            return false;
        }

    case DALLAS_COMMON_CMD_READ_ROM:
        if(data->state.command_state == DS1996CommandStateIdle) {
            data->state.command_state = DS1996CommandStateRomCmd;
            return dallas_common_emulate_read_rom(bus, &data->rom_data);
        } else {
            return false;
        }

    default:
        return false;
    }
}

void dallas_ds1996_emulate(OneWireSlave* bus, iButtonProtocolData* protocol_data) {
    DS1996ProtocolData* data = protocol_data;
    data->state.bus = bus;

    onewire_slave_set_reset_callback(bus, dallas_ds1996_reset_callback, protocol_data);
    onewire_slave_set_command_callback(bus, dallas_ds1996_command_callback, protocol_data);
}

bool dallas_ds1996_load(
    FlipperFormat* ff,
    uint32_t format_version,
    iButtonProtocolData* protocol_data) {
    DS1996ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(format_version < 2) break;
        if(!dallas_common_load_rom_data(ff, format_version, &data->rom_data)) break;
        if(!flipper_format_read_hex(
               ff, DS1996_SRAM_DATA_KEY, data->sram_data, DS1996_SRAM_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

bool dallas_ds1996_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data) {
    const DS1996ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(!dallas_common_save_rom_data(ff, &data->rom_data)) break;
        if(!flipper_format_write_hex(
               ff, DS1996_SRAM_DATA_KEY, data->sram_data, DS1996_SRAM_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

void dallas_ds1996_render_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1996ProtocolData* data = protocol_data;

    pretty_format_bytes_hex_canonical(
        result,
        DS1996_DATA_BYTE_COUNT,
        PRETTY_FORMAT_FONT_MONOSPACE,
        data->sram_data,
        DS1996_SRAM_DATA_SIZE);
}

void dallas_ds1996_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1996ProtocolData* data = protocol_data;

    for(size_t i = 0; i < sizeof(data->rom_data.bytes); ++i) {
        furi_string_cat_printf(result, "%02X ", data->rom_data.bytes[i]);
    }

    furi_string_cat_printf(
        result,
        "\nInternal SRAM: %zu Kbit\n",
        (size_t)(DS1996_SRAM_DATA_SIZE * BITS_IN_BYTE / BITS_IN_KBIT));

    for(size_t i = 0; i < DS1996_BRIEF_HEAD_COUNT; ++i) {
        furi_string_cat_printf(result, "%02X ", data->sram_data[i]);
    }

    furi_string_cat_printf(result, "[  . . .  ]");

    for(size_t i = DS1996_SRAM_DATA_SIZE - DS1996_BRIEF_TAIL_COUNT; i < DS1996_SRAM_DATA_SIZE;
        ++i) {
        furi_string_cat_printf(result, " %02X", data->sram_data[i]);
    }
}

void dallas_ds1996_render_error(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1996ProtocolData* data = protocol_data;

    if(!dallas_common_is_valid_crc(&data->rom_data)) {
        dallas_common_render_crc_error(result, &data->rom_data);
    }
}

bool dallas_ds1996_is_data_valid(const iButtonProtocolData* protocol_data) {
    const DS1996ProtocolData* data = protocol_data;
    return dallas_common_is_valid_crc(&data->rom_data);
}
