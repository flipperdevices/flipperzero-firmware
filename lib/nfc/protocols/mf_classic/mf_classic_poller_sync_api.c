#include "mf_classic_poller_i.h"

#include <furi.h>

MfClassicError
    mf_classic_poller_auth(MfClassicPoller* instance, uint8_t block_num, MfClassicKey* key) {
    furi_assert(instance);
    furi_assert(key);
    UNUSED(block_num);

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

    return MfClassicErrorNone;
}