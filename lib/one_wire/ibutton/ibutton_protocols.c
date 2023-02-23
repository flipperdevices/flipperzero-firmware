#include "ibutton_protocols.h"

#include <furi_hal_resources.h>

#include <storage/storage.h>

#include <one_wire/one_wire_host.h>
#include <one_wire/one_wire_slave.h>

#include <protocols/protocol_dict.h>

#include "ibutton_key_i.h"
#include "protocols/ibutton_protocol_defs_i.h"

#include "protocols/misc/ibutton_protocols_misc.h"

#define IBUTTON_FILE_TYPE "Flipper iButton key"

#define IBUTTON_PROTOCOL_KEY_V1 "Key type"
#define IBUTTON_PROTOCOL_KEY_V2 "Protocol"
#define IBUTTON_PROTOCOL_DALLAS_V1 "Dallas"

#define IBUTTON_CURRENT_FORMAT_VERSION 2U
#define IBUTTON_ONEWIRE_ROM_SIZE 8U

typedef struct {
    OneWireHost* host;
    OneWireSlave* bus;
    ProtocolDict* dict;
} iButtonProtocolsContext;

static iButtonProtocolsContext* ibutton_ctx;

void ibutton_protocols_init() {
    furi_assert(!ibutton_ctx);
    ibutton_ctx = malloc(sizeof(iButtonProtocolsContext));
    ibutton_ctx->host = onewire_host_alloc(&ibutton_gpio);
    ibutton_ctx->bus = onewire_slave_alloc(&ibutton_gpio);
    ibutton_ctx->dict = protocol_dict_alloc(ibutton_protocols_misc, iButtonProtocolMiscMax);
}

void ibutton_protocols_shutdown() {
    furi_assert(ibutton_ctx);
    protocol_dict_free(ibutton_ctx->dict);
    onewire_slave_free(ibutton_ctx->bus);
    onewire_host_free(ibutton_ctx->host);
    free(ibutton_ctx);
}

// 1-Wire only
static iButtonProtocol ibutton_protocols_get_id_by_family_code(uint8_t family_code) {
    iButtonProtocol protocol_id;

    for(protocol_id = iButtonProtocolOneWireMin; protocol_id < iButtonProtocolOneWireMax;
        ++protocol_id) {
        if(ibutton_protocols[protocol_id]->family_code == family_code) break;
    }

    return protocol_id;
}

static ProtocolId ibutton_protocols_get_misc_id(iButtonProtocol protocol_id) {
    furi_assert(protocol_id >= iButtonProtocolMiscMin);
    return protocol_id - iButtonProtocolMiscMin;
}

static iButtonProtocol ibutton_protocols_get_id_by_name(const char* protocol_name) {
    iButtonProtocol protocol_id;

    for(protocol_id = iButtonProtocolMin; protocol_id < iButtonProtocolMiscMin; ++protocol_id) {
        if(!strcmp(ibutton_protocols[protocol_id]->name, protocol_name)) break;
    }

    if(protocol_id == iButtonProtocolMiscMin) {
        protocol_id += protocol_dict_get_protocol_by_name(ibutton_ctx->dict, protocol_name);
    }

    return protocol_id;
}

uint32_t ibutton_protocols_get_features(iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->features;
}

size_t ibutton_protocols_get_max_data_size() {
    size_t max_data_size = 0;

    for(iButtonProtocol i = iButtonProtocolMin; i < iButtonProtocolMiscMin; ++i) {
        const size_t current_rom_size = ibutton_protocols[i]->data_size;
        if(current_rom_size > max_data_size) {
            max_data_size = current_rom_size;
        }
    }

    return MAX(max_data_size, protocol_dict_get_max_data_size(ibutton_ctx->dict));
}

const char* ibutton_protocols_get_manufacturer(iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    if(protocol_id < iButtonProtocolMiscMin) {
        return ibutton_protocols[protocol_id]->manufacturer;
    } else {
        return protocol_dict_get_manufacturer(
            ibutton_ctx->dict, ibutton_protocols_get_misc_id(protocol_id));
    }
}

const char* ibutton_protocols_get_name(iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    if(protocol_id < iButtonProtocolMiscMin) {
        return ibutton_protocols[protocol_id]->name;
    } else {
        return protocol_dict_get_name(
            ibutton_ctx->dict, ibutton_protocols_get_misc_id(protocol_id));
    }
}

static inline bool
    ibutton_protocols_read_misc(iButtonProtocolData* protocol_data, iButtonProtocol* protocol_id) {
    UNUSED(protocol_data);
    UNUSED(protocol_id);
    return false;
}

static inline bool ibutton_protocols_read_onewire(
    iButtonProtocolData* protocol_data,
    iButtonProtocol* protocol_id) {
    bool success = false;
    uint8_t rom_data[IBUTTON_ONEWIRE_ROM_SIZE];
    OneWireHost* host = ibutton_ctx->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    if(onewire_host_search(host, rom_data, OneWireHostSearchModeNormal)) {
        /* Considering any found 1-Wire device a success.
         * It can be checked later with ibutton_key_is_valid(). */
        success = true;

        /* If a 1-Wire device was found, protocol_id is guaranteed to be
         * one of the known keys or DSGeneric. */
        *protocol_id = ibutton_protocols_get_id_by_family_code(rom_data[0]);
        ibutton_protocols[*protocol_id]->read(host, protocol_data);
    }

    onewire_host_reset_search(host);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();

    return success;
}

bool ibutton_protocols_read(iButtonKey* key) {
    iButtonProtocol protocol_id = iButtonProtocolMax;
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    const bool success = ibutton_protocols_read_onewire(protocol_data, &protocol_id) ||
                         ibutton_protocols_read_misc(protocol_data, &protocol_id);

    ibutton_key_set_protocol_id(key, protocol_id);
    return success;
}

bool ibutton_protocols_write_blank(iButtonKey* key) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    furi_assert(protocol_id < iButtonProtocolMax);

    bool success = false;
    OneWireHost* host = ibutton_ctx->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    do {
        if(!(ibutton_protocols_get_features(protocol_id) & iButtonProtocolFeatureWriteBlank))
            break;
        if(!ibutton_protocols[protocol_id]->write_blank(host, protocol_data)) break;

        success = true;
    } while(false);

    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();

    return success;
}

bool ibutton_protocols_write_copy(iButtonKey* key) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    furi_assert(protocol_id < iButtonProtocolMax);

    bool success = false;
    OneWireHost* host = ibutton_ctx->host;

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    do {
        if(!(ibutton_protocols_get_features(protocol_id) & iButtonProtocolFeatureWriteCopy)) break;
        if(!ibutton_protocols[protocol_id]->write_copy(host, protocol_data)) break;

        success = true;
    } while(false);

    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();

    return success;
}

static inline void ibutton_protocols_emulate_misc_start(
    iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id) {
    UNUSED(protocol_data);
    UNUSED(protocol_id);
}

static inline void ibutton_protocols_emulate_onewire_start(
    iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id) {
    OneWireSlave* bus = ibutton_ctx->bus;
    ibutton_protocols[protocol_id]->emulate(bus, protocol_data);
    // Important: starting the bus AFTER calling emulate()
    onewire_slave_start(bus);
}

void ibutton_protocols_emulate_start(iButtonKey* key) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    furi_assert(protocol_id < iButtonProtocolMax);

    if(protocol_id < iButtonProtocolMiscMin) {
        ibutton_protocols_emulate_onewire_start(protocol_data, protocol_id);
    } else {
        ibutton_protocols_emulate_misc_start(protocol_data, protocol_id);
    }
}

static inline void ibutton_protocols_emulate_misc_stop() {
}

static inline void ibutton_protocols_emulate_onewire_stop() {
    OneWireSlave* bus = ibutton_ctx->bus;
    onewire_slave_stop(bus);
}

void ibutton_protocols_emulate_stop(iButtonKey* key) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    furi_assert(protocol_id < iButtonProtocolMax);

    if(protocol_id < iButtonProtocolMiscMin) {
        ibutton_protocols_emulate_onewire_stop();
    } else {
        ibutton_protocols_emulate_misc_stop();
    }
}

bool ibutton_protocols_save(const iButtonKey* key, const char* file_name) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    furi_assert(protocol_id < iButtonProtocolMax);

    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    do {
        const char* protocol_name = ibutton_protocols_get_name(protocol_id);

        if(!flipper_format_buffered_file_open_always(ff, file_name)) break;

        if(!flipper_format_write_header_cstr(ff, IBUTTON_FILE_TYPE, IBUTTON_CURRENT_FORMAT_VERSION))
            break;
        if(!flipper_format_write_string_cstr(ff, IBUTTON_PROTOCOL_KEY_V2, protocol_name)) break;
        if(!ibutton_protocols[protocol_id]->save(ff, protocol_data)) break;

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return success;
}

bool ibutton_protocols_load(iButtonKey* key, const char* file_name) {
    iButtonProtocol protocol_id = iButtonProtocolMax;
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    furi_assert(protocol_id < iButtonProtocolMax);

    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);
    FuriString* tmp = furi_string_alloc();

    do {
        if(!flipper_format_buffered_file_open_existing(ff, file_name)) break;

        uint32_t format_version;

        if(!flipper_format_read_header(ff, tmp, &format_version)) break;
        if(!furi_string_equal(tmp, IBUTTON_FILE_TYPE)) break;

        if(format_version == 1) {
            if(!flipper_format_read_string(ff, IBUTTON_PROTOCOL_KEY_V1, tmp)) break;
        } else if(format_version == 2) {
            if(!flipper_format_read_string(ff, IBUTTON_PROTOCOL_KEY_V2, tmp)) break;
        } else {
            break;
        }

        if((format_version == 1) && furi_string_equal(tmp, IBUTTON_PROTOCOL_DALLAS_V1)) {
            // Handle older key files which refer to DS1990 as just "Dallas"
            protocol_id = iButtonProtocolDS1990;
        } else {
            protocol_id = ibutton_protocols_get_id_by_name(furi_string_get_cstr(tmp));
        }

        if(protocol_id == iButtonProtocolMax) break;
        if(!ibutton_protocols[protocol_id]->load(ff, format_version, protocol_data)) break;

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_string_free(tmp);
    furi_record_close(RECORD_STORAGE);

    return success;
}

void ibutton_protocols_render_data(const iButtonKey* key, FuriString* result) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    furi_assert(protocol_id < iButtonProtocolMax);
    ibutton_protocols[protocol_id]->render_data(result, protocol_data);
}

void ibutton_protocols_render_brief_data(const iButtonKey* key, FuriString* result) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    furi_assert(protocol_id < iButtonProtocolMax);
    ibutton_protocols[protocol_id]->render_brief_data(result, protocol_data);
}

void ibutton_protocols_render_error(const iButtonKey* key, FuriString* result) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    furi_assert(protocol_id < iButtonProtocolMax);
    ibutton_protocols[protocol_id]->render_error(result, protocol_data);
}

bool ibutton_protocols_is_valid(const iButtonKey* key) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->is_valid(protocol_data);
}

void ibutton_protocols_get_editable_data(
    const iButtonKey* key,
    iButtonProtocolEditableData* editable_data) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    return ibutton_protocols[protocol_id]->get_editable_data(editable_data, protocol_data);
}

void ibutton_protocols_apply_edits(const iButtonKey* key) {
    const iButtonProtocol protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    furi_assert(protocol_id < iButtonProtocolMax);
    ibutton_protocols[protocol_id]->apply_edits(protocol_data);
}
