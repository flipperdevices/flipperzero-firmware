#include "ibutton_key.h"

#include <stdlib.h>
#include <string.h>

#include <core/check.h>

#include "ibutton_protocols.h"

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

iButtonProtocol ibutton_key_get_protocol_id(const iButtonKey* key) {
    return key->protocol_id;
}

void ibutton_key_set_protocol_id(iButtonKey* key, iButtonProtocol protocol_id) {
    key->protocol_id = protocol_id;
}

iButtonProtocolData* ibutton_key_get_protocol_data(const iButtonKey* key) {
    return key->protocol_data;
}

void ibutton_key_set_protocol_data(iButtonKey* key, iButtonProtocolData* protocol_data) {
    memset(key->protocol_data, 0, ibutton_protocols_get_max_data_size());
    memcpy(key->protocol_data, protocol_data, ibutton_protocols_get_data_size(key->protocol_id));
}
