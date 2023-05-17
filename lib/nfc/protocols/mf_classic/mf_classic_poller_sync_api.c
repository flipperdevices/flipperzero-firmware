#include "mf_classic_poller_i.h"

#include <furi.h>

MfClassicError mf_classic_poller_auth(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data) {
    furi_assert(instance);
    furi_assert(key);

    if(data) {
        data->block_num = block_num;
        data->key = *key;
        data->key_type = key_type;
        for(size_t i = 0; i < 4; i++) {
            data->nt.data[i] = 1;
            data->nr.data[i] = 2;
            data->at.data[i] = 3;
            data->ar.data[i] = 4;
        }
    }

    return MfClassicErrorNone;
}

MfClassicError mf_classic_poller_read_block(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicBlock* data) {
    furi_assert(instance);
    furi_assert(key);
    furi_assert(data);
    UNUSED(block_num);
    UNUSED(key_type);

    if(data) {
        for(size_t i = 0; i < 16; i++) {
            data->data[i] = i;
        }
    }

    return MfClassicErrorNone;
}