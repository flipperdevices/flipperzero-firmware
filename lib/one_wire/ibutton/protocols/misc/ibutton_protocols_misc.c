#include "ibutton_protocols_misc.h"

#include <furi_hal_rfid.h>
#include <furi_hal_ibutton.h>

#include <toolbox/protocols/protocol_dict.h>

#include "ibutton_protocol_misc_defs.h"

#define IBUTTON_MISC_READ_TIMEOUT 100

#define IBUTTON_MISC_ROM_DATA_KEY_V1 "Data"
#define IBUTTON_MISC_ROM_DATA_KEY_V2 "Rom Data"

typedef struct {
    ProtocolDict* dict;
    ProtocolId emulate_id;
} iButtonProtocolsMisc;

static iButtonProtocolsMisc* ibutton_protocols_misc_alloc() {
    iButtonProtocolsMisc* protocols = malloc(sizeof(iButtonProtocolsMisc));

    protocols->dict = protocol_dict_alloc(ibutton_protocols_misc, iButtonProtocolMiscMax);
    protocols->emulate_id = PROTOCOL_NO;

    return protocols;
}

static void ibutton_protocols_misc_free(iButtonProtocolsMisc* protocols) {
    protocol_dict_free(protocols->dict);
    free(protocols);
}

static size_t ibutton_protocols_misc_get_max_data_size(iButtonProtocolsMisc* protocols) {
    return protocol_dict_get_max_data_size(protocols->dict);
}

static bool ibutton_protocols_misc_get_id_by_name(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolLocalId* id,
    const char* name) {
    const ProtocolId found_id = protocol_dict_get_protocol_by_name(protocols->dict, name);

    if(found_id != PROTOCOL_NO) {
        *id = found_id;
        return true;
    }
    return false;
}

static uint32_t ibutton_protocols_misc_get_features(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    UNUSED(id);
    return 0;
}

static const char* ibutton_protocols_misc_get_manufacturer(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolLocalId id) {
    return protocol_dict_get_manufacturer(protocols->dict, id);
}

static const char*
    ibutton_protocols_misc_get_name(iButtonProtocolsMisc* protocols, iButtonProtocolLocalId id) {
    return protocol_dict_get_name(protocols->dict, id);
}

typedef struct {
    uint32_t last_dwt_value;
    FuriStreamBuffer* stream;
} iButtonReadContext;

static void ibutton_protocols_comparator_callback(bool level, void* context) {
    iButtonReadContext* read_context = context;

    uint32_t current_dwt_value = DWT->CYCCNT;

    LevelDuration data =
        level_duration_make(level, current_dwt_value - read_context->last_dwt_value);
    furi_stream_buffer_send(read_context->stream, &data, sizeof(LevelDuration), 0);

    read_context->last_dwt_value = current_dwt_value;
}

static bool ibutton_protocols_misc_read(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId* id) {
    bool result = false;

    protocol_dict_decoders_start(protocols->dict);

    furi_hal_rfid_pins_reset();
    // pulldown pull pin, we sense the signal through the analog part of the RFID schematic
    furi_hal_rfid_pin_pull_pulldown();

    iButtonReadContext read_context = {
        .last_dwt_value = DWT->CYCCNT,
        .stream = furi_stream_buffer_alloc(sizeof(LevelDuration) * 512, 1),
    };

    furi_hal_rfid_comp_set_callback(ibutton_protocols_comparator_callback, &read_context);
    furi_hal_rfid_comp_start();

    const uint32_t tick_start = furi_get_tick();

    for(;;) {
        LevelDuration level;
        size_t ret = furi_stream_buffer_receive(
            read_context.stream, &level, sizeof(LevelDuration), IBUTTON_MISC_READ_TIMEOUT);

        if((furi_get_tick() - tick_start) > IBUTTON_MISC_READ_TIMEOUT) {
            break;
        }

        if(ret > 0) {
            ProtocolId decoded_index = protocol_dict_decoders_feed(
                protocols->dict,
                level_duration_get_level(level),
                level_duration_get_duration(level));

            if(decoded_index == PROTOCOL_NO) continue;

            *id = decoded_index;

            protocol_dict_get_data(
                protocols->dict,
                decoded_index,
                data,
                protocol_dict_get_data_size(protocols->dict, decoded_index));

            result = true;
        }
    }

    furi_hal_rfid_comp_stop();
    furi_hal_rfid_comp_set_callback(NULL, NULL);
    furi_hal_rfid_pins_reset();

    furi_stream_buffer_free(read_context.stream);

    return result;
}

static void ibutton_protocols_misc_emulate_callback(void* context) {
    iButtonProtocolsMisc* protocols = context;

    const LevelDuration level_duration =
        protocol_dict_encoder_yield(protocols->dict, protocols->emulate_id);

    furi_hal_ibutton_emulate_set_next(level_duration_get_duration(level_duration));
    furi_hal_ibutton_pin_write(level_duration_get_level(level_duration));
}

static void ibutton_protocols_misc_emulate_start(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    protocols->emulate_id = id;
    protocol_dict_set_data(
        protocols->dict, id, data, protocol_dict_get_data_size(protocols->dict, id));
    protocol_dict_encoder_start(protocols->dict, protocols->emulate_id);

    furi_hal_ibutton_pin_configure();
    furi_hal_ibutton_emulate_start(0, ibutton_protocols_misc_emulate_callback, protocols);
}

static void ibutton_protocols_misc_emulate_stop(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    UNUSED(data);
    UNUSED(id);
    furi_hal_ibutton_emulate_stop();
    furi_hal_ibutton_pin_reset();
}

static bool ibutton_protocols_misc_save(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FlipperFormat* ff) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, id);
    return flipper_format_write_hex(ff, IBUTTON_MISC_ROM_DATA_KEY_V2, data, data_size);
}

static bool ibutton_protocols_misc_load(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    uint32_t version,
    FlipperFormat* ff) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, id);
    switch(version) {
    case 1:
        return flipper_format_read_hex(ff, IBUTTON_MISC_ROM_DATA_KEY_V1, data, data_size);
    case 2:
        return flipper_format_read_hex(ff, IBUTTON_MISC_ROM_DATA_KEY_V2, data, data_size);
    default:
        return false;
    }
}

static void ibutton_protocols_misc_render_data(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FuriString* result) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, id);
    protocol_dict_set_data(protocols->dict, id, data, data_size);
    protocol_dict_render_data(protocols->dict, result, id);
}

static void ibutton_protocols_misc_render_brief_data(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FuriString* result) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, id);
    protocol_dict_set_data(protocols->dict, id, data, data_size);
    protocol_dict_render_brief_data(protocols->dict, result, id);
}

static void ibutton_protocols_misc_render_error(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FuriString* result) {
    UNUSED(protocols);
    UNUSED(data);
    UNUSED(id);
    UNUSED(result);
}

static bool ibutton_protocols_misc_is_valid(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    UNUSED(data);
    UNUSED(id);
    return true;
}

static void ibutton_protocols_misc_get_editable_data(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    iButtonEditableData* editable) {
    editable->ptr = data;
    editable->size = protocol_dict_get_data_size(protocols->dict, id);
}

static void ibutton_protocols_misc_apply_edits(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, id);
    protocol_dict_set_data(protocols->dict, id, data, data_size);
}

const iButtonProtocolsBase ibutton_protocol_group_misc = {
    .protocol_count = iButtonProtocolMiscMax,

    .alloc = (iButtonProtocolsAllocFunc)ibutton_protocols_misc_alloc,
    .free = (iButtonProtocolsFreeFunc)ibutton_protocols_misc_free,

    .get_max_data_size = (iButtonProtocolsGetSizeFunc)ibutton_protocols_misc_get_max_data_size,
    .get_id_by_name = (iButtonProtocolsGetIdFunc)ibutton_protocols_misc_get_id_by_name,
    .get_features = (iButtonProtocolsGetFeaturesFunc)ibutton_protocols_misc_get_features,

    .get_manufacturer = (iButtonProtocolsGetStringFunc)ibutton_protocols_misc_get_manufacturer,
    .get_name = (iButtonProtocolsGetStringFunc)ibutton_protocols_misc_get_name,

    .read = (iButtonProtocolsReadFunc)ibutton_protocols_misc_read,
    .write_blank = NULL,
    .write_copy = NULL,

    .emulate_start = (iButtonProtocolsApplyFunc)ibutton_protocols_misc_emulate_start,
    .emulate_stop = (iButtonProtocolsApplyFunc)ibutton_protocols_misc_emulate_stop,

    .save = (iButtonProtocolsSaveFunc)ibutton_protocols_misc_save,
    .load = (iButtonProtocolsLoadFunc)ibutton_protocols_misc_load,

    .render_data = (iButtonProtocolsRenderFunc)ibutton_protocols_misc_render_data,
    .render_brief_data = (iButtonProtocolsRenderFunc)ibutton_protocols_misc_render_brief_data,
    .render_error = (iButtonProtocolsRenderFunc)ibutton_protocols_misc_render_error,

    .is_valid = (iButtonProtocolsIsValidFunc)ibutton_protocols_misc_is_valid,
    .get_editable_data = (iButtonProtocolsGetDataFunc)ibutton_protocols_misc_get_editable_data,
    .apply_edits = (iButtonProtocolsApplyFunc)ibutton_protocols_misc_apply_edits,
};
