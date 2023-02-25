#include "ibutton_protocols.h"

#include <storage/storage.h>

#include "ibutton_key_i.h"
#include "protocols/ibutton_protocols_defs.h"

#define IBUTTON_FILE_TYPE "Flipper iButton key"

#define IBUTTON_PROTOCOL_KEY_V1 "Key type"
#define IBUTTON_PROTOCOL_KEY_V2 "Protocol"

#define IBUTTON_CURRENT_FORMAT_VERSION 2U

#define SELECT_PROTOCOL_GROUP(id) \
    iButtonProtocolSelect select; \
    ibutton_protocols_get_group_by_id((id), &select);

#define PROTOCOL_BASE (select.base)
#define PROTOCOL_GROUP (select.protocols)
#define PROTOCOL_ID (select.id)

static iButtonProtocols** protocols;

typedef struct {
    const iButtonProtocolsBase* base;
    iButtonProtocols* protocols;
    iButtonLocalProtocolId id;
} iButtonProtocolSelect;

static bool ibutton_protocols_get_group_by_id(iButtonProtocolId id, iButtonProtocolSelect* data) {
    iButtonLocalProtocolId local_id = id;

    for(iButtonGroupId i = 0; i < iButtonProtocolGroupMax; ++i) {
        if(local_id < (signed)ibutton_protocols[i]->protocol_count) {
            data->base = ibutton_protocols[i];
            data->protocols = protocols[i];
            data->id = local_id;
            return true;

        } else {
            local_id -= ibutton_protocols[i]->protocol_count;
        }
    }
    return false;
}

void ibutton_protocols_init() {
    furi_assert(!protocols);

    protocols = malloc(sizeof(iButtonProtocols*) * iButtonProtocolGroupMax);

    for(iButtonGroupId i = 0; i < iButtonProtocolGroupMax; ++i) {
        protocols[i] = ibutton_protocols[i]->alloc();
    }
}

void ibutton_protocols_shutdown() {
    furi_assert(protocols);

    for(iButtonGroupId i = 0; i < iButtonProtocolGroupMax; ++i) {
        ibutton_protocols[i]->free(protocols[i]);
    }

    free(protocols);
}

uint32_t ibutton_protocols_get_protocol_count() {
    uint32_t count = 0;

    for(iButtonGroupId i = 0; i < iButtonProtocolGroupMax; ++i) {
        count += ibutton_protocols[i]->protocol_count;
    }

    return count;
}

static iButtonProtocolId ibutton_protocols_get_id_by_name(const char* name) {
    iButtonLocalProtocolId offset = 0;

    for(iButtonGroupId i = 0; i < iButtonProtocolGroupMax; ++i) {
        iButtonLocalProtocolId local_id;
        if(ibutton_protocols[i]->get_id_by_name(protocols[i], &local_id, name)) {
            return local_id + offset;
        }
        offset += ibutton_protocols[i]->protocol_count;
    }
    return -1;
}

uint32_t ibutton_protocols_get_features(iButtonProtocolId id) {
    SELECT_PROTOCOL_GROUP(id);
    return PROTOCOL_BASE->get_features(PROTOCOL_GROUP, PROTOCOL_ID);
}

size_t ibutton_protocols_get_max_data_size() {
    size_t max_size = 0;

    for(iButtonGroupId i = 0; i < iButtonProtocolGroupMax; ++i) {
        const size_t current_max_size = ibutton_protocols[i]->get_max_data_size(protocols[i]);
        if(current_max_size > max_size) {
            max_size = current_max_size;
        }
    }

    return max_size;
}

const char* ibutton_protocols_get_manufacturer(iButtonProtocolId id) {
    SELECT_PROTOCOL_GROUP(id);
    return PROTOCOL_BASE->get_manufacturer(PROTOCOL_GROUP, PROTOCOL_ID);
}

const char* ibutton_protocols_get_name(iButtonProtocolId id) {
    SELECT_PROTOCOL_GROUP(id);
    return PROTOCOL_BASE->get_name(PROTOCOL_GROUP, PROTOCOL_ID);
}

bool ibutton_protocols_read(iButtonKey* key) {
    iButtonLocalProtocolId id = -1;
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    iButtonLocalProtocolId offset = 0;
    for(iButtonGroupId i = 0; i < iButtonProtocolGroupMax; ++i) {
        if(ibutton_protocols[i]->read(protocols[i], data, &id)) {
            id += offset;
            break;
        }
        offset += ibutton_protocols[i]->protocol_count;
    }

    ibutton_key_set_protocol_id(key, id);
    return id != -1;
}

bool ibutton_protocols_write_blank(iButtonKey* key) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    return PROTOCOL_BASE->write_blank(PROTOCOL_GROUP, data, PROTOCOL_ID);
}

bool ibutton_protocols_write_copy(iButtonKey* key) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    return PROTOCOL_BASE->write_copy(PROTOCOL_GROUP, data, PROTOCOL_ID);
}

void ibutton_protocols_emulate_start(iButtonKey* key) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    PROTOCOL_BASE->emulate_start(PROTOCOL_GROUP, data, PROTOCOL_ID);
}

void ibutton_protocols_emulate_stop(iButtonKey* key) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    PROTOCOL_BASE->emulate_stop(PROTOCOL_GROUP, data, PROTOCOL_ID);
}

bool ibutton_protocols_save(const iButtonKey* key, const char* file_name) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    do {
        const char* protocol_name = ibutton_protocols_get_name(id);

        if(!flipper_format_buffered_file_open_always(ff, file_name)) break;

        if(!flipper_format_write_header_cstr(ff, IBUTTON_FILE_TYPE, IBUTTON_CURRENT_FORMAT_VERSION))
            break;
        if(!flipper_format_write_string_cstr(ff, IBUTTON_PROTOCOL_KEY_V2, protocol_name)) break;

        SELECT_PROTOCOL_GROUP(id);
        if(!PROTOCOL_BASE->save(PROTOCOL_GROUP, data, PROTOCOL_ID, ff)) break;

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return success;
}

bool ibutton_protocols_load(iButtonKey* key, const char* file_name) {
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);
    FuriString* tmp = furi_string_alloc();

    do {
        if(!flipper_format_buffered_file_open_existing(ff, file_name)) break;

        uint32_t version;

        if(!flipper_format_read_header(ff, tmp, &version)) break;
        if(!furi_string_equal(tmp, IBUTTON_FILE_TYPE)) break;

        if(version == 1) {
            if(!flipper_format_read_string(ff, IBUTTON_PROTOCOL_KEY_V1, tmp)) break;
        } else if(version == 2) {
            if(!flipper_format_read_string(ff, IBUTTON_PROTOCOL_KEY_V2, tmp)) break;
        } else {
            break;
        }

        const iButtonProtocolId id = ibutton_protocols_get_id_by_name(furi_string_get_cstr(tmp));
        ibutton_key_set_protocol_id(key, id);

        SELECT_PROTOCOL_GROUP(id);
        if(!PROTOCOL_BASE->load(PROTOCOL_GROUP, data, PROTOCOL_ID, version, ff)) break;

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_string_free(tmp);
    furi_record_close(RECORD_STORAGE);

    return success;
}

void ibutton_protocols_render_data(const iButtonKey* key, FuriString* result) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    PROTOCOL_BASE->render_data(PROTOCOL_GROUP, data, PROTOCOL_ID, result);
}

void ibutton_protocols_render_brief_data(const iButtonKey* key, FuriString* result) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    PROTOCOL_BASE->render_brief_data(PROTOCOL_GROUP, data, PROTOCOL_ID, result);
}

void ibutton_protocols_render_error(const iButtonKey* key, FuriString* result) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    PROTOCOL_BASE->render_error(PROTOCOL_GROUP, data, PROTOCOL_ID, result);
}

bool ibutton_protocols_is_valid(const iButtonKey* key) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    return PROTOCOL_BASE->is_valid(PROTOCOL_GROUP, data, PROTOCOL_ID);
}

void ibutton_protocols_get_editable_data(const iButtonKey* key, iButtonEditableData* editable) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    PROTOCOL_BASE->get_editable_data(PROTOCOL_GROUP, data, PROTOCOL_ID, editable);
}

void ibutton_protocols_apply_edits(const iButtonKey* key) {
    const iButtonProtocolId id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* data = ibutton_key_get_protocol_data(key);

    SELECT_PROTOCOL_GROUP(id);
    PROTOCOL_BASE->apply_edits(PROTOCOL_GROUP, data, PROTOCOL_ID);
}
