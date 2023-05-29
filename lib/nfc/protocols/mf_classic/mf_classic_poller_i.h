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
    MfClassicCardStateNotDetected,
    MfClassicCardStateDetected,
} MfClassicCardState;

typedef enum {
    MfClassicReadModeDictAttack,
    MfClassicReadModeKeyReuse,
} MfClassicReadMode;

typedef enum {
    MfClassicPollerStateStart,
    MfClassicPollerStateIdle,
    MfClassicPollerStateNewSector,
    MfClassicPollerStateRequestKey,
    MfClassicPollerStateAuthKeyA,
    MfClassicPollerStateAuthKeyB,
    MfClassicPollerStateReadSector,
    MfClassicPollerStateReadComplete,

    MfClassicPollerStateNum,
} MfClassicPollerState;

struct MfClassicPoller {
    NfcaPoller* nfca_poller;
    MfClassicPollerState state;
    MfClassicPollerState prev_state;
    MfClassicPollerSessionState session_state;
    MfClassicAuthState auth_state;
    MfClassicCardState card_state;
    MfClassicReadMode read_mode;
    MfClassicKey current_key;
    MfClassicPollerEventData event_data;
    uint8_t sectors_read;
    uint8_t key_reuse_sector;
    uint8_t sectors_total;
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

MfClassicError mf_classic_aync_halt(MfClassicPoller* instance);

MfClassicError
    mf_classic_async_read_block(MfClassicPoller* instance, uint8_t block_num, MfClassicBlock* data);

#ifdef __cplusplus
}
#endif
