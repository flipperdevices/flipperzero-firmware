#pragma once

#include "mf_classic_poller.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_poller_i.h>
#include <lib/nfc/helpers/nfc_util.h>
#include "crypto1.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfClassicAuthStateIdle,
    MfClassicAuthStatePassed,
} MfClassicAuthState;

typedef enum {
    MfClassicCardStateLost,
    MfClassicCardStateDetected,
} MfClassicCardState;

typedef enum {
    MfClassicReadModeDictAttack,
    MfClassicReadModeKeyReuse,
} MfClassicReadMode;

typedef enum {
    MfClassicPollerStateStart,
    MfClassicPollerStateIdle,

    // Write states
    MfClassicPollerStateRequestSectorTrailer,
    MfClassicPollerStateCheckWriteConditions,
    MfClassicPollerStateReadBlock,
    MfClassicPollerStateWriteBlock,

    // Read states
    MfClassicPollerStateNewSector,
    MfClassicPollerStateRequestKey,
    MfClassicPollerStateRequestReadSector,
    MfClassicPollerStateAuthKeyA,
    MfClassicPollerStateAuthKeyB,
    MfClassicPollerStateReadSector,
    MfClassicPollerStateSuccess,
    MfClassicPollerStateFail,

    MfClassicPollerStateNum,
} MfClassicPollerState;

typedef struct {
    uint8_t current_sector;
    MfClassicSectorTrailer sec_tr;
    uint8_t current_block;
    MfClassicKeyType key_type_read;
    MfClassicKeyType key_type_write;
    bool need_halt_before_write;
    MfClassicBlock tag_block;
} MfClassicPollerWriteContext;

typedef union {
    MfClassicPollerWriteContext write_ctx;
} MfClassicPollerModeContext;

struct MfClassicPoller {
    Iso14443_3aPoller* iso14443_3a_poller;

    MfClassicPollerState state;
    MfClassicPollerState prev_state;
    MfClassicAuthState auth_state;
    MfClassicCardState card_state;

    MfClassicReadMode read_mode;
    MfClassicKey current_key;
    uint8_t sectors_read;
    uint8_t key_reuse_sector;
    uint8_t sectors_total;

    MfClassicPollerModeContext mode_ctx;

    Crypto1* crypto;
    BitBuffer* tx_plain_buffer;
    BitBuffer* tx_encrypted_buffer;
    BitBuffer* rx_plain_buffer;
    BitBuffer* rx_encrypted_buffer;
    MfClassicData* data;

    NfcGenericEvent general_event;
    MfClassicPollerEvent mfc_event;
    MfClassicPollerEventData mfc_event_data;
    NfcGenericCallback callback;
    void* context;
};

typedef struct {
    uint8_t block_num;
    MfClassicKey key;
    MfClassicKeyType key_type;
    MfClassicBlock block;
} MfClassicReadBlockContext;

typedef struct {
    uint8_t block_num;
    MfClassicKey key;
    MfClassicKeyType key_type;
    MfClassicBlock block;
} MfClassicWriteBlockContext;

typedef struct {
    uint8_t block_num;
    MfClassicKey key;
    MfClassicKeyType key_type;
    int32_t value;
} MfClassicReadValueContext;

typedef struct {
    uint8_t block_num;
    MfClassicKey key;
    MfClassicKeyType key_type;
    MfClassicValueCommand value_cmd;
    int32_t data;
    int32_t new_value;
} MfClassicChangeValueContext;

typedef union {
    MfClassicAuthContext auth_context;
    MfClassicReadBlockContext read_block_context;
    MfClassicWriteBlockContext write_block_context;
    MfClassicReadValueContext read_value_context;
    MfClassicChangeValueContext change_value_context;
} MfClassicPollerContextData;

MfClassicError mf_classic_process_error(Iso14443_3aError error);

MfClassicPoller* mf_classic_poller_alloc(Iso14443_3aPoller* iso14443_3a_poller);

void mf_classic_poller_free(MfClassicPoller* instance);

MfClassicError mf_classic_async_auth(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data);

MfClassicError mf_classic_aync_halt(MfClassicPoller* instance);

MfClassicError
    mf_classic_async_read_block(MfClassicPoller* instance, uint8_t block_num, MfClassicBlock* data);

MfClassicError mf_classic_async_write_block(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicBlock* data);

MfClassicError mf_classic_async_value_cmd(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicValueCommand cmd,
    int32_t data);

MfClassicError mf_classic_async_value_transfer(MfClassicPoller* instance, uint8_t block_num);

#ifdef __cplusplus
}
#endif
