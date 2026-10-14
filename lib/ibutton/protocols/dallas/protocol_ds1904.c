#include "protocol_ds1904.h"

#include <core/string.h>
#include <core/core_defines.h>
#include <furi_hal_rtc.h>

#include "dallas_common.h"

#define DS1904_FAMILY_CODE 0x24U
#define DS1904_FAMILY_NAME "DS1904"

#define DS1904_READ_CLOCK 0x66U
#define DS1904_WRITE_CLOCK 0x99U

#define DS1904_CLOCK_DATA_SIZE 4U
#define DS1904_CLOCK_DATA_KEY "Clock Data"
#define DS1904_DEVICE_CONTROL_BYTE_KEY "Device Control Byte"

typedef struct {
    OneWireSlave* bus;
} DS1904ProtocolState;

typedef struct {
    DallasCommonRomData rom_data;
    uint8_t device_control_byte;
    uint8_t clock_data[DS1904_CLOCK_DATA_SIZE];
    DS1904ProtocolState state;
} DS1904ProtocolData;

static bool dallas_ds1904_read(OneWireHost*, iButtonProtocolData*);
static bool
    dallas_ds1904_read_clock(OneWireHost* host, uint8_t* device_control_byte, uint8_t* clock_data);
static void dallas_ds1904_emulate(OneWireSlave*, iButtonProtocolData*);
static bool dallas_ds1904_load(FlipperFormat*, uint32_t, iButtonProtocolData*);
static bool dallas_ds1904_save(FlipperFormat*, const iButtonProtocolData*);
static void dallas_ds1904_render_brief_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1904_render_error(FuriString*, const iButtonProtocolData*);
static bool dallas_ds1904_is_data_valid(const iButtonProtocolData*);
static void dallas_ds1904_get_editable_data(iButtonEditableData*, iButtonProtocolData*);
static void dallas_ds1904_apply_edits(iButtonProtocolData*);

const iButtonProtocolDallasBase ibutton_protocol_ds1904 = {
    .family_code = DS1904_FAMILY_CODE,
    .features = iButtonProtocolFeatureNone,
    .data_size = sizeof(DS1904ProtocolData),
    .manufacturer = DALLAS_COMMON_MANUFACTURER_NAME,
    .name = DS1904_FAMILY_NAME,

    .read = dallas_ds1904_read,
    .write_blank = NULL,
    .write_copy = NULL, /* No data to write a copy */
    .emulate = dallas_ds1904_emulate,
    .save = dallas_ds1904_save,
    .load = dallas_ds1904_load,
    .render_data = NULL, /* No data to render */
    .render_brief_data = dallas_ds1904_render_brief_data,
    .render_error = dallas_ds1904_render_error,
    .is_valid = dallas_ds1904_is_data_valid,
    .get_editable_data = dallas_ds1904_get_editable_data,
    .apply_edits = dallas_ds1904_apply_edits,
};

bool dallas_ds1904_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    DS1904ProtocolData* data = protocol_data;
    return onewire_host_reset(host) && dallas_common_read_rom(host, &data->rom_data) &&
           dallas_ds1904_read_clock(host, &data->device_control_byte, data->clock_data);
}

bool dallas_ds1904_read_clock(OneWireHost* host, uint8_t* device_control_byte, uint8_t* clock_data) {
    uint8_t data[1 + DS1904_CLOCK_DATA_SIZE];

    onewire_host_write(host, DS1904_READ_CLOCK);
    onewire_host_read_bytes(host, data, sizeof(data));
    *device_control_byte = data[0];
    memcpy(clock_data, data + 1, DS1904_CLOCK_DATA_SIZE);

    return true;
}

static bool dallas_ds1904_reset_callback(bool is_short, void* context) {
    DS1904ProtocolData* data = context;
    if(!is_short) {
        onewire_slave_set_overdrive(data->state.bus, is_short);
    }
    return !is_short;
}

static bool
    dallas_ds1904_emulate_read_clock(OneWireSlave* bus, const uint8_t device_control_byte) {
    uint8_t response[5];
    response[0] = device_control_byte;
    uint32_t ts = furi_hal_rtc_get_timestamp();
    memcpy(response + 1, &ts, sizeof(ts));

    return onewire_slave_send(bus, response, sizeof(response));
}

static bool
    dallas_ds1904_emulate_write_clock(OneWireSlave* bus, const uint8_t* device_control_byte) {
    // What does it mean to write the clock of a virtualized RTC?
    UNUSED(bus);
    UNUSED(device_control_byte);
    return true;
}

static bool dallas_ds1904_command_callback(uint8_t command, void* context) {
    furi_assert(context);
    DS1904ProtocolData* data = context;
    OneWireSlave* bus = data->state.bus;

    switch(command) {
    case DALLAS_COMMON_CMD_SEARCH_ROM:
        dallas_common_emulate_search_rom(bus, &data->rom_data);
        break;
    case DALLAS_COMMON_CMD_READ_ROM:
        dallas_common_emulate_read_rom(bus, &data->rom_data);
        break;
    case DS1904_READ_CLOCK:
        dallas_ds1904_emulate_read_clock(bus, data->device_control_byte);
        break;
    case DS1904_WRITE_CLOCK:
        dallas_ds1904_emulate_write_clock(bus, &data->device_control_byte);
        break;
    default:
        break;
    }

    // No support for multiple consecutive commands
    return false;
}

void dallas_ds1904_emulate(OneWireSlave* bus, iButtonProtocolData* protocol_data) {
    DS1904ProtocolData* data = protocol_data;
    data->state.bus = bus;

    onewire_slave_set_reset_callback(bus, dallas_ds1904_reset_callback, protocol_data);
    onewire_slave_set_command_callback(bus, dallas_ds1904_command_callback, protocol_data);
}

bool dallas_ds1904_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data) {
    const DS1904ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(!dallas_common_save_rom_data(ff, &data->rom_data)) break;
        if(!flipper_format_write_hex(
               ff, DS1904_DEVICE_CONTROL_BYTE_KEY, &data->device_control_byte, 1))
            break;
        if(!flipper_format_write_hex(
               ff, DS1904_CLOCK_DATA_KEY, data->clock_data, DS1904_CLOCK_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

bool dallas_ds1904_load(
    FlipperFormat* ff,
    uint32_t format_version,
    iButtonProtocolData* protocol_data) {
    DS1904ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(format_version < 2) break;
        if(!dallas_common_load_rom_data(ff, format_version, &data->rom_data)) break;
        if(!flipper_format_read_hex(
               ff, DS1904_DEVICE_CONTROL_BYTE_KEY, &data->device_control_byte, 1))
            break;
        if(!flipper_format_read_hex(
               ff, DS1904_CLOCK_DATA_KEY, data->clock_data, DS1904_CLOCK_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

void dallas_ds1904_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1904ProtocolData* data = protocol_data;

    for(size_t i = 0; i < sizeof(DallasCommonRomData); ++i) {
        furi_string_cat_printf(result, "%02X ", data->rom_data.bytes[i]);
    }

    furi_string_cat_printf(result, "\nDeviceControl %02x", data->device_control_byte);
    furi_string_cat_printf(
        result,
        "\nClock %02x%02x%02x%02x",
        data->clock_data[0],
        data->clock_data[1],
        data->clock_data[2],
        data->clock_data[3]);
}

void dallas_ds1904_render_error(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1904ProtocolData* data = protocol_data;

    if(!dallas_common_is_valid_crc(&data->rom_data)) {
        dallas_common_render_crc_error(result, &data->rom_data);
    }
}

bool dallas_ds1904_is_data_valid(const iButtonProtocolData* protocol_data) {
    const DS1904ProtocolData* data = protocol_data;
    return dallas_common_is_valid_crc(&data->rom_data);
}

void dallas_ds1904_get_editable_data(
    iButtonEditableData* editable_data,
    iButtonProtocolData* protocol_data) {
    DS1904ProtocolData* data = protocol_data;
    editable_data->ptr = data->rom_data.bytes;
    editable_data->size = sizeof(DallasCommonRomData);
}

void dallas_ds1904_apply_edits(iButtonProtocolData* protocol_data) {
    DS1904ProtocolData* data = protocol_data;
    dallas_common_apply_edits(&data->rom_data, DS1904_FAMILY_CODE);
}
