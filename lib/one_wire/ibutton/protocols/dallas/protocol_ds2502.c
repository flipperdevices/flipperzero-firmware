#include "protocol_ds2502.h"

#include <core/core_defines.h>
#include <toolbox/pretty_format.h>

#include "dallas_common.h"

#define DS2502_FAMILY_CODE 0x09U
#define DS2502_FAMILY_NAME "DS2502"

// Some Dell chargers reportedly use FC 0x28 but they seem to just be DS2502
// https://github.com/orgua/OneWireHub#implemented-slaves
#define DS2502_DELL_FAMILY_CODE 0x28U
#define DS2502_DELL_FAMILY_NAME "Dell Charger"

#define DS2502_OTP_DATA_SIZE 128U

#define DS2502_DATA_BYTE_COUNT 4U

#define DS2502_OTP_DATA_KEY "Otp Data"
#define DS2502_MEMORY_TYPE "OTP"

typedef struct {
    OneWireSlave* bus;
    DallasCommonCommandState command_state;
} DS2502ProtocolState;

typedef struct {
    DallasCommonRomData rom_data;
    uint8_t otp_data[DS2502_OTP_DATA_SIZE];
    DS2502ProtocolState state;
} DS2502ProtocolData;

static bool dallas_ds2502_read(OneWireHost*, void*);
static void dallas_ds2502_emulate(OneWireSlave*, iButtonProtocolData*);
static bool dallas_ds2502_load(FlipperFormat*, uint32_t, iButtonProtocolData*);
static bool dallas_ds2502_save(FlipperFormat*, const iButtonProtocolData*);
static void dallas_ds2502_render_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds2502_render_brief_data(FuriString*, const iButtonProtocolData*);
static void dallas_ds2502_render_error(FuriString*, const iButtonProtocolData*);
static bool dallas_ds2502_is_data_valid(const iButtonProtocolData*);
static void dallas_ds2502_get_editable_data(iButtonEditableData*, iButtonProtocolData*);
static void dallas_ds2502_apply_edits(iButtonProtocolData*);
static void dallas_ds2502_dell_apply_edits(iButtonProtocolData*);

const iButtonProtocolDallasBase ibutton_protocol_ds2502 = {
    .family_code = DS2502_FAMILY_CODE,
    .features = iButtonProtocolFeatureExtData,
    .data_size = sizeof(DS2502ProtocolData),
    .manufacturer = DALLAS_COMMON_MANUFACTURER_NAME,
    .name = DS2502_FAMILY_NAME,

    .read = dallas_ds2502_read,
    // DS2502 is OTP and requires high voltage to program which Flipper doesn't support.
    .write_blank = NULL,
    .write_copy = NULL,
    .emulate = dallas_ds2502_emulate,
    .save = dallas_ds2502_save,
    .load = dallas_ds2502_load,
    .render_data = dallas_ds2502_render_data,
    .render_brief_data = dallas_ds2502_render_brief_data,
    .render_error = dallas_ds2502_render_error,
    .is_valid = dallas_ds2502_is_data_valid,
    .get_editable_data = dallas_ds2502_get_editable_data,
    .apply_edits = dallas_ds2502_apply_edits,
};

const iButtonProtocolDallasBase ibutton_protocol_ds2502_dell = {
    .family_code = DS2502_DELL_FAMILY_CODE,
    .features = iButtonProtocolFeatureExtData,
    .data_size = sizeof(DS2502ProtocolData),
    .manufacturer = DALLAS_COMMON_MANUFACTURER_NAME,
    .name = DS2502_DELL_FAMILY_NAME,

    .read = dallas_ds2502_read,
    // DS2502 is OTP and requires high voltage to program which Flipper doesn't support.
    .write_blank = NULL,
    .write_copy = NULL,
    .emulate = dallas_ds2502_emulate,
    .save = dallas_ds2502_save,
    .load = dallas_ds2502_load,
    .render_data = dallas_ds2502_render_data,
    .render_brief_data = dallas_ds2502_render_brief_data,
    .render_error = dallas_ds2502_render_error,
    .is_valid = dallas_ds2502_is_data_valid,
    .get_editable_data = dallas_ds2502_get_editable_data,
    .apply_edits = dallas_ds2502_dell_apply_edits,
};

bool dallas_ds2502_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    DS2502ProtocolData* data = protocol_data;
    return onewire_host_reset(host) && dallas_common_read_rom(host, &data->rom_data) &&
           dallas_common_read_mem(host, 0, data->otp_data, DS2502_OTP_DATA_SIZE, true);
}

static void dallas_ds2502_reset_callback(void* context) {
    furi_assert(context);
    DS2502ProtocolData* data = context;
    data->state.command_state = DallasCommonCommandStateIdle;
}

static bool dallas_ds2502_command_callback(uint8_t command, void* context) {
    furi_assert(context);
    DS2502ProtocolData* data = context;
    OneWireSlave* bus = data->state.bus;

    switch(command) {
    case DALLAS_COMMON_CMD_SEARCH_ROM:
        if(data->state.command_state == DallasCommonCommandStateIdle) {
            data->state.command_state = DallasCommonCommandStateRomCmd;
            return dallas_common_emulate_search_rom(bus, &data->rom_data);

        } else if(data->state.command_state == DallasCommonCommandStateRomCmd) {
            data->state.command_state = DallasCommonCommandStateMemCmd;
            dallas_common_emulate_read_mem(bus, data->otp_data, DS2502_OTP_DATA_SIZE, true);
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

void dallas_ds2502_emulate(OneWireSlave* bus, iButtonProtocolData* protocol_data) {
    DS2502ProtocolData* data = protocol_data;
    data->state.bus = bus;

    onewire_slave_set_reset_callback(bus, dallas_ds2502_reset_callback, protocol_data);
    onewire_slave_set_command_callback(bus, dallas_ds2502_command_callback, protocol_data);
}

bool dallas_ds2502_load(
    FlipperFormat* ff,
    uint32_t format_version,
    iButtonProtocolData* protocol_data) {
    DS2502ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(format_version < 2) break;
        if(!dallas_common_load_rom_data(ff, format_version, &data->rom_data)) break;
        if(!flipper_format_read_hex(ff, DS2502_OTP_DATA_KEY, data->otp_data, DS2502_OTP_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

bool dallas_ds2502_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data) {
    const DS2502ProtocolData* data = protocol_data;
    bool success = false;

    do {
        if(!dallas_common_save_rom_data(ff, &data->rom_data)) break;
        if(!flipper_format_write_hex(ff, DS2502_OTP_DATA_KEY, data->otp_data, DS2502_OTP_DATA_SIZE))
            break;
        success = true;
    } while(false);

    return success;
}

void dallas_ds2502_render_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS2502ProtocolData* data = protocol_data;
    pretty_format_bytes_hex_canonical(
        result,
        DS2502_DATA_BYTE_COUNT,
        PRETTY_FORMAT_FONT_MONOSPACE,
        data->otp_data,
        DS2502_OTP_DATA_SIZE);
}

void dallas_ds2502_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS2502ProtocolData* data = protocol_data;
    dallas_common_render_brief_data(
        result, &data->rom_data, data->otp_data, DS2502_OTP_DATA_SIZE, DS2502_MEMORY_TYPE);
}

void dallas_ds2502_render_error(FuriString* result, const iButtonProtocolData* protocol_data) {
    const DS2502ProtocolData* data = protocol_data;

    if(!dallas_common_is_valid_crc(&data->rom_data)) {
        dallas_common_render_crc_error(result, &data->rom_data);
    }
}

bool dallas_ds2502_is_data_valid(const iButtonProtocolData* protocol_data) {
    const DS2502ProtocolData* data = protocol_data;
    return dallas_common_is_valid_crc(&data->rom_data);
}

void dallas_ds2502_get_editable_data(
    iButtonEditableData* editable_data,
    iButtonProtocolData* protocol_data) {
    DS2502ProtocolData* data = protocol_data;
    editable_data->ptr = data->rom_data.bytes;
    editable_data->size = sizeof(DallasCommonRomData);
}

void dallas_ds2502_apply_edits(iButtonProtocolData* protocol_data) {
    DS2502ProtocolData* data = protocol_data;
    dallas_common_apply_edits(&data->rom_data, DS2502_FAMILY_CODE);
}

void dallas_ds2502_dell_apply_edits(iButtonProtocolData* protocol_data) {
    DS2502ProtocolData* data = protocol_data;
    dallas_common_apply_edits(&data->rom_data, DS2502_DELL_FAMILY_CODE);
}
