#include "ibutton_protocols_dallas.h"

#include <furi_hal_resources.h>

#include <one_wire/one_wire_host.h>
#include <one_wire/one_wire_slave.h>

#include "ibutton_protocol_dallas_defs.h"

#define IBUTTON_ONEWIRE_ROM_SIZE 8U

struct iButtonProtocolsDallas {
    OneWireHost* host;
    OneWireSlave* bus;
};

iButtonProtocolsDallas* ibutton_protocols_dallas_alloc() {
    iButtonProtocolsDallas* protocols = malloc(sizeof(iButtonProtocolsDallas));

    protocols->host = onewire_host_alloc(&ibutton_gpio);
    protocols->bus = onewire_slave_alloc(&ibutton_gpio);

    return protocols;
}

void ibutton_protocols_dallas_free(iButtonProtocolsDallas* protocols) {
    onewire_slave_free(protocols->bus);
    onewire_host_free(protocols->host);
    free(protocols);
}

size_t ibutton_protocols_dallas_get_max_data_size(iButtonProtocolsDallas* protocols) {
    UNUSED(protocols);
    size_t max_data_size = 0;

    for(iButtonProtocolId i = 0; i < iButtonProtocolDSMax; ++i) {
        const size_t current_rom_size = ibutton_protocols_dallas[i]->data_size;
        if(current_rom_size > max_data_size) {
            max_data_size = current_rom_size;
        }
    }

    return max_data_size;
}

bool ibutton_protocols_dallas_get_id_by_name(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId* protocol_id,
    const char* protocol_name) {
    UNUSED(protocols);
    // Handle older key files which refer to DS1990 as just "Dallas"
    if(strcmp(protocol_name, "Dallas") == 0) {
        *protocol_id = iButtonProtocolDS1990;
        return true;
    }

    for(iButtonProtocolId i = 0; i < iButtonProtocolDSMax; ++i) {
        if(strcmp(ibutton_protocols_dallas[i]->name, protocol_name) == 0) {
            *protocol_id = i;
            return true;
        }
    }
    return false;
}

uint32_t ibutton_protocols_dallas_get_features(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[protocol_id]->features;
}

const char* ibutton_protocols_dallas_get_manufacturer(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[protocol_id]->manufacturer;
}

const char* ibutton_protocols_dallas_get_name(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[protocol_id]->name;
}

static iButtonProtocolId ibutton_protocols_dallas_get_id_by_family_code(uint8_t family_code) {
    iButtonProtocolId protocol_id;

    for(protocol_id = 0; protocol_id < iButtonProtocolDSGeneric; ++protocol_id) {
        if(ibutton_protocols_dallas[protocol_id]->family_code == family_code) break;
    }

    return protocol_id;
}

bool ibutton_protocols_dallas_read(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId* protocol_id) {
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

        /* If a 1-Wire device was found, protocol_id is guaranteed to be
         * one of the known keys or DSGeneric. */
        *protocol_id = ibutton_protocols_dallas_get_id_by_family_code(rom_data[0]);
        ibutton_protocols_dallas[*protocol_id]->read(host, protocol_data);
    }

    onewire_host_reset_search(host);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();

    return success;
}

bool ibutton_protocols_dallas_write_blank(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    furi_assert(protocol_id < iButtonProtocolDSMax);
    const iButtonProtocolDallasBase* protocol = ibutton_protocols_dallas[protocol_id];
    furi_assert(protocol->features & iButtonProtocolFeatureWriteBlank);

    OneWireHost* host = protocols->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    const bool success = protocol->write_blank(host, protocol_data);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();
    return success;
}

bool ibutton_protocols_dallas_write_copy(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    furi_assert(protocol_id < iButtonProtocolDSMax);

    const iButtonProtocolDallasBase* protocol = ibutton_protocols_dallas[protocol_id];
    furi_assert(protocol->features & iButtonProtocolFeatureWriteCopy);

    OneWireHost* host = protocols->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    const bool success = protocol->write_copy(host, protocol_data);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();
    return success;
}

void ibutton_protocols_dallas_emulate_start(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    furi_assert(protocol_id < iButtonProtocolDSMax);
    OneWireSlave* bus = protocols->bus;
    ibutton_protocols_dallas[protocol_id]->emulate(bus, protocol_data);
    onewire_slave_start(bus);
}

void ibutton_protocols_dallas_emulate_stop(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id) {
    furi_assert(protocol_id < iButtonProtocolDSMax);
    onewire_slave_stop(protocols->bus);
}

bool ibutton_protocols_dallas_save(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FlipperFormat* ff) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[protocol_id]->save(ff, protocol_data);
}

bool ibutton_protocols_dallas_load(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    uint32_t version,
    FlipperFormat* ff) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[protocol_id]->load(ff, version, protocol_data);
}

void ibutton_protocols_dallas_render_data(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    const iButtonProtocolDallasBase* protocol = ibutton_protocols_dallas[protocol_id];
    furi_assert(protocol->render_data);
    protocol->render_data(result, protocol_data);
}

void ibutton_protocols_dallas_render_brief_data(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[protocol_id]->render_brief_data(result, protocol_data);
}

void ibutton_protocols_dallas_render_error(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[protocol_id]->render_error(result, protocol_data);
}

bool ibutton_protocols_dallas_is_valid(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    return ibutton_protocols_dallas[protocol_id]->is_valid(protocol_data);
}

void ibutton_protocols_dallas_get_editable_data(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    iButtonProtocolEditableData* editable_data) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[protocol_id]->get_editable_data(editable_data, protocol_data);
}

void ibutton_protocols_dallas_apply_edits(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id) {
    UNUSED(protocols);
    furi_assert(protocol_id < iButtonProtocolDSMax);
    ibutton_protocols_dallas[protocol_id]->apply_edits(protocol_data);
}
