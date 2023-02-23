#include "ibutton_key.h"

#include "protocols/ibutton_protocol_base.h"

struct iButtonKey {
    iButtonProtocol protocol_id;
    iButtonProtocolData* protocol_data;
    size_t protocol_data_size;
};

iButtonKey* ibutton_key_alloc(size_t data_size) {
    iButtonKey* key = malloc(sizeof(iButtonKey));

    key->protocol_id = iButtonProtocolMax;
    key->protocol_data = malloc(data_size);
    key->protocol_data_size = data_size;

    return key;
}

void ibutton_key_free(iButtonKey* key) {
    free(key->protocol_data);
    free(key);
}

void ibutton_key_reset(iButtonKey* key) {
    key->protocol_id = iButtonProtocolMax;
    memset(key->protocol_data, 0, key->protocol_data_size);
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

size_t ibutton_key_get_protocol_data_size(const iButtonKey* key) {
    return key->protocol_data_size;
}
