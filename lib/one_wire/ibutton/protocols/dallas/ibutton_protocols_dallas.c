#include "ibutton_protocols_dallas.h"

#include <furi_hal_resources.h>

#include "ibutton_protocol_dallas_defs.h"

#define IBUTTON_ONEWIRE_ROM_SIZE 8U

typedef struct {
    OneWireHost* host;
    OneWireSlave* bus;
} iButtonProtocolsDallas;

static iButtonProtocolsDallas* ibutton_protocols_dallas_alloc() {
    iButtonProtocolsDallas* protocols = malloc(sizeof(iButtonProtocolsDallas));

    protocols->host = onewire_host_alloc(&ibutton_gpio);
    protocols->bus = onewire_slave_alloc(&ibutton_gpio);

    return protocols;
}

static void ibutton_protocols_dallas_free(iButtonProtocolsDallas* protocols) {
    onewire_slave_free(protocols->bus);
    onewire_host_free(protocols->host);
    free(protocols);
}

static size_t ibutton_protocols_dallas_get_max_data_size(iButtonProtocolsDallas* protocols) {
    UNUSED(protocols);
    size_t max_data_size = 0;

    for(iButtonProtocolLocalId i = 0; i < iButtonProtocolDSMax; ++i) {
        const size_t current_rom_size = ibutton_protocols_dallas[i]->data_size;
        if(current_rom_size > max_data_size) {
            max_data_size = current_rom_size;
        }
    }

    return max_data_size;
}

static bool ibutton_protocols_dallas_get_id_by_name(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolLocalId* id,
    const char* name) {
    UNUSED(protocols);
    // Handle older key files which refer to DS1990 as just "Dallas"
    if(strcmp(name, "Dallas") == 0) {
        *id = iButtonProtocolDS1990;
        return true;
    }

    for(iButtonProtocolLocalId i = 0; i < iButtonProtocolDSMax; ++i) {
        if(strcmp(ibutton_protocols_dallas[i]->name, name) == 0) {
            *id = i;
            return true;
        }
    }
    return false;
}

static uint32_t ibutton_protocols_dallas_get_features(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[id]->features;
}

static const char* ibutton_protocols_dallas_get_manufacturer(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[id]->manufacturer;
}

static const char* ibutton_protocols_dallas_get_name(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[id]->name;
}

static iButtonProtocolLocalId ibutton_protocols_dallas_get_id_by_family_code(uint8_t family_code) {
    iButtonProtocolLocalId id;

    for(id = 0; id < iButtonProtocolDSGeneric; ++id) {
        if(ibutton_protocols_dallas[id]->family_code == family_code) break;
    }

    return id;
}

static bool ibutton_protocols_dallas_read(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId* id) {
    bool success = false;
    uint8_t rom_data[IBUTTON_ONEWIRE_ROM_SIZE];
    OneWireHost* host = protocols->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    if(onewire_host_search(host, rom_data, OneWireHostSearchModeNormal)) {
        /* Considering any found 1-Wire device a success.
         * It can be checked later with ibutton_key_is_valid(). */
        success = true;

        /* If a 1-Wire device was found, id is guaranteed to be
         * one of the known keys or DSGeneric. */
        *id = ibutton_protocols_dallas_get_id_by_family_code(rom_data[0]);
        ibutton_protocols_dallas[*id]->read(host, data);
    }

    onewire_host_reset_search(host);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();

    return success;
}

static bool ibutton_protocols_dallas_write_blank(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    furi_assert(id < iButtonProtocolDSMax);
    const iButtonProtocolDallasBase* protocol = ibutton_protocols_dallas[id];
    furi_assert(protocol->features & iButtonProtocolFeatureWriteBlank);

    OneWireHost* host = protocols->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    const bool success = protocol->write_blank(host, data);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();
    return success;
}

static bool ibutton_protocols_dallas_write_copy(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    furi_assert(id < iButtonProtocolDSMax);

    const iButtonProtocolDallasBase* protocol = ibutton_protocols_dallas[id];
    furi_assert(protocol->features & iButtonProtocolFeatureWriteCopy);

    OneWireHost* host = protocols->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    const bool success = protocol->write_copy(host, data);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();
    return success;
}

static void ibutton_protocols_dallas_emulate_start(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    furi_assert(id < iButtonProtocolDSMax);
    OneWireSlave* bus = protocols->bus;
    ibutton_protocols_dallas[id]->emulate(bus, data);
    onewire_slave_start(bus);
}

static void ibutton_protocols_dallas_emulate_stop(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    furi_assert(id < iButtonProtocolDSMax);
    UNUSED(data);
    onewire_slave_stop(protocols->bus);
}

static bool ibutton_protocols_dallas_save(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FlipperFormat* ff) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[id]->save(ff, data);
}

static bool ibutton_protocols_dallas_load(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    uint32_t version,
    FlipperFormat* ff) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[id]->load(ff, version, data);
}

static void ibutton_protocols_dallas_render_data(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FuriString* result) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    const iButtonProtocolDallasBase* protocol = ibutton_protocols_dallas[id];
    furi_assert(protocol->render_data);
    protocol->render_data(result, data);
}

static void ibutton_protocols_dallas_render_brief_data(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FuriString* result) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[id]->render_brief_data(result, data);
}

static void ibutton_protocols_dallas_render_error(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    FuriString* result) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[id]->render_error(result, data);
}

static bool ibutton_protocols_dallas_is_valid(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[id]->is_valid(data);
}

static void ibutton_protocols_dallas_get_editable_data(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id,
    iButtonEditableData* editable) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[id]->get_editable_data(editable, data);
}

static void ibutton_protocols_dallas_apply_edits(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* data,
    iButtonProtocolLocalId id) {
    UNUSED(protocols);
    furi_assert(id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[id]->apply_edits(data);
}

const iButtonProtocolsBase ibutton_protocol_group_dallas = {
    .protocol_count = iButtonProtocolDSMax,

    .alloc = (iButtonProtocolsAllocFunc)ibutton_protocols_dallas_alloc,
    .free = (iButtonProtocolsFreeFunc)ibutton_protocols_dallas_free,

    .get_max_data_size = (iButtonProtocolsGetSizeFunc)ibutton_protocols_dallas_get_max_data_size,
    .get_id_by_name = (iButtonProtocolsGetIdFunc)ibutton_protocols_dallas_get_id_by_name,
    .get_features = (iButtonProtocolsGetFeaturesFunc)ibutton_protocols_dallas_get_features,

    .get_manufacturer = (iButtonProtocolsGetStringFunc)ibutton_protocols_dallas_get_manufacturer,
    .get_name = (iButtonProtocolsGetStringFunc)ibutton_protocols_dallas_get_name,

    .read = (iButtonProtocolsReadFunc)ibutton_protocols_dallas_read,
    .write_blank = (iButtonProtocolsWriteFunc)ibutton_protocols_dallas_write_blank,
    .write_copy = (iButtonProtocolsWriteFunc)ibutton_protocols_dallas_write_copy,

    .emulate_start = (iButtonProtocolsApplyFunc)ibutton_protocols_dallas_emulate_start,
    .emulate_stop = (iButtonProtocolsApplyFunc)ibutton_protocols_dallas_emulate_stop,

    .save = (iButtonProtocolsSaveFunc)ibutton_protocols_dallas_save,
    .load = (iButtonProtocolsLoadFunc)ibutton_protocols_dallas_load,

    .render_data = (iButtonProtocolsRenderFunc)ibutton_protocols_dallas_render_data,
    .render_brief_data = (iButtonProtocolsRenderFunc)ibutton_protocols_dallas_render_brief_data,
    .render_error = (iButtonProtocolsRenderFunc)ibutton_protocols_dallas_render_error,

    .is_valid = (iButtonProtocolsIsValidFunc)ibutton_protocols_dallas_is_valid,
    .get_editable_data = (iButtonProtocolsGetDataFunc)ibutton_protocols_dallas_get_editable_data,
    .apply_edits = (iButtonProtocolsApplyFunc)ibutton_protocols_dallas_apply_edits,
};
