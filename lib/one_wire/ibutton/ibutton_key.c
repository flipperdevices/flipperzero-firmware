#include "ibutton_key.h"

#include <stdlib.h>
#include <string.h>

#include "ibutton_protocols_i.h"

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

void ibutton_key_reset(iButtonKey* key) {
    key->protocol_id = iButtonProtocolMax;
    memset(key->protocol_data, 0, ibutton_protocols_get_max_data_size());
}

bool ibutton_key_is_valid(iButtonKey* key) {
    if(key->protocol_id < iButtonProtocolMax) {
        return ibutton_protocols_is_valid(key->protocol_data, key->protocol_id);
    } else {
        return false;
    }
}

uint32_t ibutton_key_get_features(iButtonKey* key) {
    return ibutton_protocols_get_features(key->protocol_id);
}

const char* ibutton_key_get_manufacturer_name(iButtonKey* key) {
    return ibutton_protocols_get_manufacturer(key->protocol_id);
}

const char* ibutton_key_get_protocol_name(iButtonKey* key) {
    return ibutton_protocols_get_name(key->protocol_id);
}

void ibutton_key_get_editable_data(iButtonKey* key, iButtonKeyEditableData* data) {
    furi_assert(data);
    if(key->protocol_id < iButtonProtocolMax) {
        ibutton_protocols_get_editable_data(
            &data->ptr, &data->size, key->protocol_data, key->protocol_id);
    } else {
        data->ptr = NULL;
        data->size = 0;
    }
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
    }
}

void ibutton_key_set_protocol_id(iButtonKey* key, uint32_t protocol_id) {
    ibutton_key_reset(key);
    key->protocol_id = protocol_id;
    ibutton_key_apply_edits(key);
}

bool ibutton_key_read(iButtonKey* key) {
    return ibutton_protocols_read(key->protocol_data, &key->protocol_id);
}

bool ibutton_key_write_blank(iButtonKey* key) {
    if(key->protocol_id < iButtonProtocolMax) {
        return ibutton_protocols_write_blank(key->protocol_data, key->protocol_id);
    } else {
        return false;
    }
}

bool ibutton_key_write_copy(iButtonKey* key) {
    if(key->protocol_id < iButtonProtocolMax) {
        return ibutton_protocols_write_copy(key->protocol_data, key->protocol_id);
    } else {
        return false;
    }
}

void ibutton_key_emulate_start(iButtonKey* key) {
    if(key->protocol_id < iButtonProtocolMax) {
        ibutton_protocols_emulate_start(key->protocol_data, key->protocol_id);
    }
}

void ibutton_key_emulate_stop(iButtonKey* key) {
    if(key->protocol_id < iButtonProtocolMax) {
        ibutton_protocols_emulate_stop(key->protocol_id);
    }
}

bool ibutton_key_save(iButtonKey* key, const char* file_name) {
    if(key->protocol_id < iButtonProtocolMax) {
        return ibutton_protocols_save(file_name, key->protocol_data, key->protocol_id);
    } else {
        return false;
    }
}

bool ibutton_key_load(iButtonKey* key, const char* file_name) {
    return ibutton_protocols_load(file_name, key->protocol_data, &key->protocol_id);
}

void ibutton_key_apply_edits(iButtonKey* key) {
    if((key->protocol_id < iButtonProtocolMax) &&
       (ibutton_protocols_get_features(key->protocol_id) & iButtonProtocolFeatureApplyEdits)) {
        ibutton_protocols_apply_edits(key->protocol_data, key->protocol_id);
    }
}
