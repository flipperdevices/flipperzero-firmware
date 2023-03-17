#include "protocol_ds1982.h"

#include <core/core_defines.h>
#include <toolbox/pretty_format.h>

#include "dallas_common.h"

#define DS1982_FAMILY_CODE 0x09U
#define DS1982_FAMILY_NAME "DS1982"

#define DS1982_OTP_DATA_SIZE 128U

#define DS1982_DATA_BYTE_COUNT 4U

#define DS1982_OTP_DATA_KEY "Otp Data"
#define DS1982_MEMORY_TYPE "OTP"

typedef struct {
    OneWireSlave* bus;
    DallasCommonCommandState command_state;
} DS1982ProtocolState;

typedef struct {
    DallasCommonRomData rom_data;
    uint8_t otp_data[DS1982_OTP_DATA_SIZE];
    DS1982ProtocolState state;
} DS1982ProtocolData;

static bool dallas_ds1982_read(OneWireHost*, void*);
static void dallas_ds1982_emulate(OneWireSlave*, iButtonProtocolData*);
static bool dallas_ds1982_load(FlipperFormat*, uint32_t, iButtonProtocolData*);
static bool dallas_ds1982_save(FlipperFormat*, const iButtonProtocolData*);
static void dallas_ds1982_render_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1982_render_brief_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds1982_render_error(FuriString*, const iButtonProtocolData*);
static bool dallas_ds1982_is_data_valid(const iButtonProtocolData*);
static void dallas_ds1982_get_editable_data(iButtonEditableData*, iButtonProtocolData*);
static void dallas_ds1982_apply_edits(iButtonProtocolData*);

const iButtonProtocolDallasBase ibutton_protocol_ds1982 = {
    .family_code = DS1982_FAMILY_CODE,
    .features = iButtonProtocolFeatureExtData,
    .data_size = sizeof(DS1982ProtocolData),
    .manufacturer = DALLAS_COMMON_MANUFACTURER_NAME,
    .name = DS1982_FAMILY_NAME,

    .read = dallas_ds1982_read,
    // DS1982 is OTP and requires high voltage to program which Flipper doesn't support.
    .write_blank = NULL,
    .write_copy = NULL,
    .emulate = dallas_ds1982_emulate,
    .save = dallas_ds1982_save,
    .load = dallas_ds1982_load,
    .render_data = dallas_ds1982_render_data,
    .render_brief_data = dallas_ds1982_render_brief_data,
    .render_error = dallas_ds1982_render_error,
    .is_valid = dallas_ds1982_is_data_valid,
    .get_editable_data = dallas_ds1982_get_editable_data,
    .apply_edits = dallas_ds1982_apply_edits,
};

bool dallas_ds1982_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    DS1982ProtocolData* data = protocol_data;
    return onewire_host_reset(host) && dallas_common_read_rom(host, &data->rom_data) &&
           dallas_common_read_mem(host, 0, data->otp_data, DS1982_OTP_DATA_SIZE, true);
}

static void dallas_ds1982_reset_callback(void* context) {
    furi_assert(context);
    DS1982ProtocolData* data = context;
    data->state.command_state = DallasCommonCommandStateIdle;
}

static bool dallas_ds1982_command_callback(uint8_t command, void* context) {
    furi_assert(context);
    DS1982ProtocolData* data = context;
    OneWireSlave* bus = data->state.bus;

    switch(command) {
    case DALLAS_COMMON_CMD_SEARCH_ROM:
        if(data->state.command_state == DallasCommonCommandStateIdle) {
            data->state.command_state = DallasCommonCommandStateRomCmd;
            return dallas_common_emulate_search_rom(bus, &data->rom_data);

        } else if(data->state.command_state == DallasCommonCommandStateRomCmd) {
            data->state.command_state = DallasCommonCommandStateMemCmd;
            dallas_common_emulate_read_mem(bus, data->otp_data, DS1982_OTP_DATA_SIZE, true);
            return false;

        } else {
            return false;
        }

    case DALLAS_COMMON_CMD_READ_ROM:
        if(data->state.command_state == DallasCommonCommandStateIdle) {
            data->state.command_state = DallasCommonCommandStateRomCmd;
            return dallas_common_emulate_read_rom(bus, &data->rom_data);
        } else {
            return false;
        }

    case DALLAS_COMMON_CMD_SKIP_ROM:
        if(data->state.command_state == DallasCommonCommandStateIdle) {
            data->state.command_state = DallasCommonCommandStateRomCmd;
            return true;
        } else {
            return false;
        }

    default:
        return false;
    }
}

void dallas_ds1982_emulate(OneWireSlave* bus, iButtonProtocolData* protocol_data) {
    DS1982ProtocolData* data = protocol_data;
    data->state.bus = bus;

    onewire_slave_set_reset_callback(bus, dallas_ds1982_reset_callback, protocol_data);
    onewire_slave_set_command_callback(bus, dallas_ds1982_command_callback, protocol_data);
}

bool dallas_ds1982_load(
    FlipperFormat* ff,
    uint32_t format_version,
    iButtonProtocolData* protocol_data) {
    DS1982ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(format_version < 2) break;
        if(!dallas_common_load_rom_data(ff, format_version, &data->rom_data)) break;
        if(!flipper_format_read_hex(ff, DS1982_OTP_DATA_KEY, data->otp_data, DS1982_OTP_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

bool dallas_ds1982_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data) {
    const DS1982ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(!dallas_common_save_rom_data(ff, &data->rom_data)) break;
        if(!flipper_format_write_hex(ff, DS1982_OTP_DATA_KEY, data->otp_data, DS1982_OTP_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

void dallas_ds1982_render_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1982ProtocolData* data = protocol_data;
    pretty_format_bytes_hex_canonical(
        result,
        DS1982_DATA_BYTE_COUNT,
        PRETTY_FORMAT_FONT_MONOSPACE,
        data->otp_data,
        DS1982_OTP_DATA_SIZE);
}

void dallas_ds1982_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1982ProtocolData* data = protocol_data;
    dallas_common_render_brief_data(
        result, &data->rom_data, data->otp_data, DS1982_OTP_DATA_SIZE, DS1982_MEMORY_TYPE);
}

void dallas_ds1982_render_error(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS1982ProtocolData* data = protocol_data;

    if(!dallas_common_is_valid_crc(&data->rom_data)) {
        dallas_common_render_crc_error(result, &data->rom_data);
    }
}

bool dallas_ds1982_is_data_valid(const iButtonProtocolData* protocol_data) {
    const DS1982ProtocolData* data = protocol_data;
    return dallas_common_is_valid_crc(&data->rom_data);
}

void dallas_ds1982_get_editable_data(
    iButtonEditableData* editable_data,
    iButtonProtocolData* protocol_data) {
    DS1982ProtocolData* data = protocol_data;
    editable_data->ptr = data->rom_data.bytes;
    editable_data->size = sizeof(DallasCommonRomData);
}

void dallas_ds1982_apply_edits(iButtonProtocolData* protocol_data) {
    DS1982ProtocolData* data = protocol_data;
    dallas_common_apply_edits(&data->rom_data, DS1982_FAMILY_CODE);
}
