#include "ibutton_protocols_misc.h"

#include <furi_hal_rfid.h>
#include <furi_hal_ibutton.h>

#include <toolbox/protocols/protocol_dict.h>

#include "ibutton_protocol_misc_defs.h"

#define IBUTTON_MISC_READ_TIMEOUT 100

typedef struct {
    ProtocolDict* dict;
    ProtocolId protocol_id;
} iButtonProtocolsMisc;

static iButtonProtocolsMisc* ibutton_protocols_misc_alloc() {
    iButtonProtocolsMisc* protocols = malloc(sizeof(iButtonProtocolsMisc));

    protocols->dict = protocol_dict_alloc(ibutton_protocols_misc, iButtonProtocolMiscMax);
    protocols->protocol_id = PROTOCOL_NO;

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
    iButtonProtocolId* protocol_id,
    const char* protocol_name) {
    const ProtocolId found_id = protocol_dict_get_protocol_by_name(protocols->dict, protocol_name);

    if(found_id != PROTOCOL_NO) {
        *protocol_id = found_id;
        return true;
    }
    return false;
}

static uint32_t ibutton_protocols_misc_get_features(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    UNUSED(protocol_id);
    return 0;
}

static const char* ibutton_protocols_misc_get_manufacturer(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolId protocol_id) {
    return protocol_dict_get_manufacturer(protocols->dict, protocol_id);
}

static const char* ibutton_protocols_misc_get_name(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolId protocol_id) {
    return protocol_dict_get_name(protocols->dict, protocol_id);
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
    iButtonProtocolData* protocol_data,
    iButtonProtocolId* protocol_id) {
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

            *protocol_id = decoded_index;

            protocol_dict_get_data(
                protocols->dict,
                decoded_index,
                protocol_data,
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

static void ibutton_protocols_misc_emulate_start(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    UNUSED(protocol_data);
    UNUSED(protocol_id);
}

static void ibutton_protocols_misc_emulate_stop(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    UNUSED(protocol_data);
    UNUSED(protocol_id);
}

static bool ibutton_protocols_misc_save(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FlipperFormat* ff) {
    UNUSED(protocols);
    UNUSED(protocol_data);
    UNUSED(protocol_id);
    UNUSED(ff);
    return false;
}

static bool ibutton_protocols_misc_load(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    uint32_t format_version,
    FlipperFormat* ff) {
    UNUSED(protocols);
    UNUSED(protocol_data);
    UNUSED(protocol_id);
    UNUSED(format_version);
    UNUSED(ff);
    return false;
}

static void ibutton_protocols_misc_render_data(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, protocol_id);
    protocol_dict_set_data(protocols->dict, protocol_id, protocol_data, data_size);
    protocol_dict_render_data(protocols->dict, result, protocol_id);
}

static void ibutton_protocols_misc_render_brief_data(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, protocol_id);
    protocol_dict_set_data(protocols->dict, protocol_id, protocol_data, data_size);
    protocol_dict_render_brief_data(protocols->dict, result, protocol_id);
}

static void ibutton_protocols_misc_render_error(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result) {
    UNUSED(protocols);
    UNUSED(protocol_data);
    UNUSED(protocol_id);
    UNUSED(result);
}

static bool ibutton_protocols_misc_is_valid(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    UNUSED(protocol_data);
    UNUSED(protocol_id);
    return true;
}

static void ibutton_protocols_misc_get_editable_data(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    iButtonEditableData* data) {
    data->ptr = protocol_data;
    data->size = protocol_dict_get_data_size(protocols->dict, protocol_id);
}

static void ibutton_protocols_misc_apply_edits(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    const size_t data_size = protocol_dict_get_data_size(protocols->dict, protocol_id);
    protocol_dict_set_data(protocols->dict, protocol_id, protocol_data, data_size);
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
