#pragma once

#include "mf_classic_poller.h"
#include <lib/nfc/protocols/nfca/nfca_poller_i.h>
#include <nfc/helpers/nfc_poller_buffer.h>
#include <lib/nfc/protocols/nfc_util.h>
#include "crypto1.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfClassicPollerSessionStateIdle,
    MfClassicPollerSessionStateActive,
    MfClassicPollerSessionStateStopRequest,
} MfClassicPollerSessionState;

typedef enum {
    MfClassicAuthStateIdle,
    MfClassicAuthStatePassed,
} MfClassicAuthState;

typedef enum {
    MfClassicPollerStateIdle,
} MfClassicPollerState;

struct MfClassicPoller {
    NfcaPoller* nfca_poller;
    MfClassicPollerSessionState session_state;
    MfClassicAuthState auth_state;
    Crypto1* crypto;
    NfcPollerBuffer* plain_buff;
    NfcPollerBuffer* encrypted_buff;
    MfClassicData* data;
    MfClassicPollerCallback callback;
    void* context;
};

typedef struct {
    uint8_t block_num;
    MfClassicKey key;
    MfClassicKeyType key_type;
    MfClassicBlock block;
} MfClassicReadBlockContext;

typedef union {
    MfClassicAuthContext auth_context;
    MfClassicReadBlockContext read_block_context;
} MfClassicPollerContextData;


MfClassicError mf_classic_process_error(NfcaError error);

MfClassicError mf_classic_async_auth(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data);

MfClassicError
    mf_classic_async_read_block(MfClassicPoller* instance, uint8_t block_num, MfClassicBlock* data);

#ifdef __cplusplus
}
#endif
