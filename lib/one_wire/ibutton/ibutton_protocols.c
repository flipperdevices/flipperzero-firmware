#include "ibutton_protocols.h"

#include <furi_hal_resources.h>
#include <furi_hal_ibutton.h>
#include <furi_hal_rfid.h>

#include <storage/storage.h>

#include <one_wire/one_wire_host.h>
#include <one_wire/one_wire_slave.h>

#include "ibutton_key_i.h"

#include "protocols/dallas/ibutton_protocols_dallas.h"
#include "protocols/misc/ibutton_protocols_misc.h"

#define IBUTTON_FILE_TYPE "Flipper iButton key"

#define IBUTTON_PROTOCOL_KEY_V1 "Key type"
#define IBUTTON_PROTOCOL_KEY_V2 "Protocol"

#define IBUTTON_CURRENT_FORMAT_VERSION 2U

typedef struct {
    iButtonProtocolsDallas* protocols_dallas;
    iButtonProtocolsMisc* protocols_misc;
} iButtonProtocolsContext;

static iButtonProtocolsContext* ibutton_ctx;

void ibutton_protocols_init() {
    furi_assert(!ibutton_ctx);
    ibutton_ctx = malloc(sizeof(iButtonProtocolsContext));
    ibutton_ctx->protocols_dallas = ibutton_protocols_dallas_alloc();
    ibutton_ctx->protocols_misc = ibutton_protocols_misc_alloc();
}

void ibutton_protocols_shutdown() {
    furi_assert(ibutton_ctx);
    ibutton_protocols_misc_free(ibutton_ctx->protocols_misc);
    ibutton_protocols_dallas_free(ibutton_ctx->protocols_dallas);
    free(ibutton_ctx);
}

uint32_t ibutton_protocols_get_protocol_count() {
    //TODO: Do not hardcode protocol count
    return 6;
}

static iButtonProtocolId ibutton_protocols_get_misc_id(iButtonProtocolId protocol_id) {
    //TODO: Do not hardcode offsets
    return protocol_id - 4;
}

static iButtonProtocolId ibutton_protocols_get_id_from_misc(iButtonProtocolId misc_protocol_id) {
    //TODO: Do not hardcode offsets
    return 4 + misc_protocol_id;
}

static iButtonProtocolId ibutton_protocols_get_id_by_name(const char* protocol_name) {
    iButtonProtocolId protocol_id = -1;

    if(ibutton_protocols_dallas_get_id_by_name(
           ibutton_ctx->protocols_dallas, &protocol_id, protocol_name)) {
        return protocol_id;
    } else if(ibutton_protocols_misc_get_id_by_name(
                  ibutton_ctx->protocols_misc, &protocol_id, protocol_name)) {
        return ibutton_protocols_get_id_from_misc(protocol_id);
    }

    return protocol_id;
}

uint32_t ibutton_protocols_get_features(iButtonProtocolId protocol_id) {
    return ibutton_protocols_dallas_get_features(ibutton_ctx->protocols_dallas, protocol_id);
}

size_t ibutton_protocols_get_max_data_size() {
    return MAX(
        ibutton_protocols_dallas_get_max_data_size(ibutton_ctx->protocols_dallas),
        ibutton_protocols_misc_get_max_data_size(ibutton_ctx->protocols_misc));
}

const char* ibutton_protocols_get_manufacturer(iButtonProtocolId protocol_id) {
    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        return ibutton_protocols_dallas_get_manufacturer(
            ibutton_ctx->protocols_dallas, protocol_id);
    } else {
        return ibutton_protocols_misc_get_manufacturer(
            ibutton_ctx->protocols_misc, ibutton_protocols_get_misc_id(protocol_id));
    }
}

const char* ibutton_protocols_get_name(iButtonProtocolId protocol_id) {
    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        return ibutton_protocols_dallas_get_name(ibutton_ctx->protocols_dallas, protocol_id);
    } else {
        return ibutton_protocols_misc_get_name(
            ibutton_ctx->protocols_misc, ibutton_protocols_get_misc_id(protocol_id));
    }
}

bool ibutton_protocols_read(iButtonKey* key) {
    iButtonProtocolId protocol_id;
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    bool success = false;
    if(ibutton_protocols_dallas_read(ibutton_ctx->protocols_dallas, protocol_data, &protocol_id)) {
        success = true;
    } else if(ibutton_protocols_misc_read(
                  ibutton_ctx->protocols_misc, protocol_data, &protocol_id)) {
        protocol_id = ibutton_protocols_get_id_from_misc(protocol_id);
        success = true;
    } else {
        protocol_id = -1;
    }

    ibutton_key_set_protocol_id(key, protocol_id);
    return success;
}

bool ibutton_protocols_write_blank(iButtonKey* key) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    return ibutton_protocols_dallas_write_blank(
        ibutton_ctx->protocols_dallas, protocol_data, protocol_id);
}

bool ibutton_protocols_write_copy(iButtonKey* key) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);
    return ibutton_protocols_dallas_write_copy(
        ibutton_ctx->protocols_dallas, protocol_data, protocol_id);
}

void ibutton_protocols_emulate_start(iButtonKey* key) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        ibutton_protocols_dallas_emulate_start(
            ibutton_ctx->protocols_dallas, protocol_data, protocol_id);
    } else {
        ibutton_protocols_misc_emulate_start(
            ibutton_ctx->protocols_misc,
            protocol_data,
            ibutton_protocols_get_misc_id(protocol_id));
    }
}

void ibutton_protocols_emulate_stop(iButtonKey* key) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        ibutton_protocols_dallas_emulate_stop(ibutton_ctx->protocols_dallas, protocol_id);
    } else {
        ibutton_protocols_misc_emulate_stop(
            ibutton_ctx->protocols_misc, ibutton_protocols_get_misc_id(protocol_id));
    }
}

bool ibutton_protocols_save(const iButtonKey* key, const char* file_name) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    do {
        const char* protocol_name = ibutton_protocols_get_name(protocol_id);

        if(!flipper_format_buffered_file_open_always(ff, file_name)) break;

        if(!flipper_format_write_header_cstr(ff, IBUTTON_FILE_TYPE, IBUTTON_CURRENT_FORMAT_VERSION))
            break;
        if(!flipper_format_write_string_cstr(ff, IBUTTON_PROTOCOL_KEY_V2, protocol_name)) break;

        //TODO: Do not hardcode offsets
        if(protocol_id < 4) {
            if(!ibutton_protocols_dallas_save(
                   ibutton_ctx->protocols_dallas, protocol_data, protocol_id, ff))
                break;
        } else {
            if(!ibutton_protocols_misc_save(
                   ibutton_ctx->protocols_misc,
                   protocol_data,
                   ibutton_protocols_get_misc_id(protocol_id),
                   ff))
                break;
        }

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return success;
}

bool ibutton_protocols_load(iButtonKey* key, const char* file_name) {
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

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

        const iButtonProtocolId protocol_id =
            ibutton_protocols_get_id_by_name(furi_string_get_cstr(tmp));
        ibutton_key_set_protocol_id(key, protocol_id);

        //TODO: Do not hardcode offsets
        if(protocol_id == -1) {
            break;
        } else if(protocol_id < 4) {
            if(!ibutton_protocols_dallas_load(
                   ibutton_ctx->protocols_dallas, protocol_data, protocol_id, format_version, ff))
                break;
        } else {
            if(!ibutton_protocols_misc_load(
                   ibutton_ctx->protocols_misc,
                   protocol_data,
                   ibutton_protocols_get_misc_id(protocol_id),
                   format_version,
                   ff))
                break;
        }

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_string_free(tmp);
    furi_record_close(RECORD_STORAGE);

    return success;
}

void ibutton_protocols_render_data(const iButtonKey* key, FuriString* result) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        ibutton_protocols_dallas_render_data(
            ibutton_ctx->protocols_dallas, protocol_data, protocol_id, result);
    } else {
        ibutton_protocols_misc_render_data(
            ibutton_ctx->protocols_misc,
            protocol_data,
            ibutton_protocols_get_misc_id(protocol_id),
            result);
    }
}

void ibutton_protocols_render_brief_data(const iButtonKey* key, FuriString* result) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        ibutton_protocols_dallas_render_brief_data(
            ibutton_ctx->protocols_dallas, protocol_data, protocol_id, result);
    } else {
        ibutton_protocols_misc_render_brief_data(
            ibutton_ctx->protocols_misc,
            protocol_data,
            ibutton_protocols_get_misc_id(protocol_id),
            result);
    }
}

void ibutton_protocols_render_error(const iButtonKey* key, FuriString* result) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        ibutton_protocols_dallas_render_error(
            ibutton_ctx->protocols_dallas, protocol_data, protocol_id, result);
    }
}

bool ibutton_protocols_is_valid(const iButtonKey* key) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    const iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        return ibutton_protocols_dallas_is_valid(
            ibutton_ctx->protocols_dallas, protocol_data, protocol_id);
    } else {
        return true;
    }
}

void ibutton_protocols_get_editable_data(
    const iButtonKey* key,
    iButtonProtocolEditableData* editable_data) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        ibutton_protocols_dallas_get_editable_data(
            ibutton_ctx->protocols_dallas, protocol_data, protocol_id, editable_data);
    } else {
        ibutton_protocols_misc_get_editable_data(
            ibutton_ctx->protocols_misc,
            protocol_data,
            ibutton_protocols_get_misc_id(protocol_id),
            editable_data);
    }
}

void ibutton_protocols_apply_edits(const iButtonKey* key) {
    const iButtonProtocolId protocol_id = ibutton_key_get_protocol_id(key);
    iButtonProtocolData* protocol_data = ibutton_key_get_protocol_data(key);

    //TODO: Do not hardcode offsets
    if(protocol_id < 4) {
        ibutton_protocols_dallas_apply_edits(
            ibutton_ctx->protocols_dallas, protocol_data, protocol_id);
    } else {
        ibutton_protocols_misc_apply_edits(
            ibutton_ctx->protocols_misc,
            protocol_data,
            ibutton_protocols_get_misc_id(protocol_id));
    }
}
