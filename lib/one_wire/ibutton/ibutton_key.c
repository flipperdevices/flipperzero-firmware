#include "ibutton_key.h"

#include <stdlib.h>
#include <string.h>

#include <storage/storage.h>

#include "ibutton_protocols.h"

#define IBUTTON_KEY_FILE_TYPE "Flipper iButton key"
#define IBUTTON_KEY_CURRENT_FORMAT_VERSION 2U
#define IBUTTON_KEY_ONEWIRE_ROM_SIZE 8U

struct iButtonKey {
    iButtonProtocol protocol_id;
    iButtonProtocolData* protocol_data;
};

iButtonKey* ibutton_key_alloc() {
    iButtonKey* key = malloc(sizeof(iButtonKey));

    key->protocol_id = iButtonProtocolMax;
    key->protocol_data = malloc(ibutton_protocols_get_max_data_size());

    return key;
}

void ibutton_key_free(iButtonKey* key) {
    free(key->protocol_data);
    free(key);
}

bool ibutton_key_is_valid(iButtonKey* key) {
    if(key->protocol_id < iButtonProtocolMax) {
        return ibutton_protocols_is_valid(key->protocol_data, key->protocol_id);
    } else {
        return false;
    }
}

const char* ibutton_key_get_manufacturer_name(iButtonKey* key) {
    return ibutton_protocols_get_manufacturer(key->protocol_id);
}

const char* ibutton_key_get_protocol_name(iButtonKey* key) {
    return ibutton_protocols_get_name(key->protocol_id);
}

uint32_t ibutton_key_get_features(iButtonKey* key) {
    return ibutton_protocols_get_features(key->protocol_id);
}

static bool ibutton_key_read_onewire(iButtonKey* key, OneWireHost* host) {
    bool success = false;
    uint8_t rom_data[IBUTTON_KEY_ONEWIRE_ROM_SIZE];

    onewire_host_start(host);
    furi_delay_ms(100);

    FURI_CRITICAL_ENTER();

    do {
        if(!onewire_host_search(host, rom_data, OneWireHostSearchModeNormal)) break;

        /* Considering any found 1-Wire device a success.
         * It can be checked later with ibutton_key_is_valid(). */
        success = true;

        key->protocol_id = ibutton_protocols_get_id_by_family_code(rom_data[0]);
        if(key->protocol_id == iButtonProtocolMax) break;

        if(!onewire_host_reset(host)) break;
        if(!ibutton_protocols_read(host, key->protocol_data, key->protocol_id)) break;
    } while(false);

    onewire_host_reset_search(host);
    onewire_host_stop(host);

    FURI_CRITICAL_EXIT();

    return success;
}

bool ibutton_key_read(iButtonKey* key, OneWireHost* host) {
    return ibutton_key_read_onewire(key, host);
}

bool ibutton_key_emulate(iButtonKey* key, OneWireSlave* slave) {
    bool success = false;

    if(key->protocol_id < iButtonProtocolMax) {
        ibutton_protocols_emulate(slave, key->protocol_data, key->protocol_id);
        success = true;
    }

    return success;
}

bool ibutton_key_save(iButtonKey* key, const char* file_name) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperFormat* ff = flipper_format_file_alloc(storage);

    do {
        if(key->protocol_id >= iButtonProtocolMax) break;
        const char* protocol_name = ibutton_protocols_get_name(key->protocol_id);

        if(!flipper_format_file_open_always(ff, file_name)) break;

        if(!flipper_format_write_header_cstr(
               ff, IBUTTON_KEY_FILE_TYPE, IBUTTON_KEY_CURRENT_FORMAT_VERSION))
            break;
        if(!flipper_format_write_string_cstr(ff, "Protocol", protocol_name)) break;
        if(!ibutton_protocols_save(ff, key->protocol_data, key->protocol_id)) break;

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return success;
}

bool ibutton_key_load(iButtonKey* key, const char* file_name) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperFormat* ff = flipper_format_file_alloc(storage);
    FuriString* tmp = furi_string_alloc();

    do {
        if(!flipper_format_file_open_existing(ff, file_name)) break;

        uint32_t version;

        if(!flipper_format_read_header(ff, tmp, &version)) break;
        if(!furi_string_equal(tmp, IBUTTON_KEY_FILE_TYPE)) break;

        if(version == 1) {
            if(!flipper_format_read_string(ff, "Key type", tmp)) break;
        } else if(version == 2) {
            if(!flipper_format_read_string(ff, "Protocol", tmp)) break;
        } else {
            break;
        }

        if((version == 1) && furi_string_equal(tmp, "Dallas")) {
            // Handle older key files which refer to DS1990 as just "Dallas"
            key->protocol_id = iButtonProtocolDS1990;
        } else {
            key->protocol_id = ibutton_protocols_get_id_by_name(furi_string_get_cstr(tmp));
        }

        if(key->protocol_id == iButtonProtocolMax) break;
        if(!ibutton_protocols_load(ff, version, key->protocol_data, key->protocol_id)) break;

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_string_free(tmp);
    furi_record_close(RECORD_STORAGE);

    return success;
}

void ibutton_key_get_rendered_data(iButtonKey* key, FuriString* result) {
    ibutton_protocols_render_data(result, key->protocol_data, key->protocol_id);
}

void ibutton_key_get_rendered_brief_data(iButtonKey* key, FuriString* result) {
    ibutton_protocols_render_brief_data(result, key->protocol_data, key->protocol_id);
}

void ibutton_key_get_rendered_error(iButtonKey* key, FuriString* result) {
    if(key->protocol_id < iButtonProtocolMax) {
        ibutton_protocols_render_error(result, key->protocol_data, key->protocol_id);
    } else {
        furi_string_printf(result, "Unsupported Protocol");
    }
}
