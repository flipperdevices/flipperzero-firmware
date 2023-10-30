#pragma once

#include "mf_classic.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfClassicPollerLl MfClassicPollerLl;

MfClassicPollerLl* mf_classic_poller_ll_alloc(Nfc* nfc);
void mf_classic_poller_ll_free(MfClassicPollerLl* poller);

MfClassicError mf_classic_poller_ll_auth(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    bool is_nested);

MfClassicError mf_classic_poller_ll_read_block(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    MfClassicBlock* data);

MfClassicError mf_classic_poller_ll_write_block(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    MfClassicBlock* data);

MfClassicError mf_classic_poller_ll_value_block_command(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    uint8_t operation,
    uint32_t data);

MfClassicError
    mf_classic_poller_ll_value_block_transfer(MfClassicPollerLl* poller, uint8_t block_num);

MfClassicError mf_classic_poller_ll_halt(MfClassicPollerLl* poller);

#ifdef __cplusplus
}
#endif
