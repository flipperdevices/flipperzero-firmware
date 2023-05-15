#pragma once

#include "mf_classic.h"
#include <lib/nfc/protocols/nfca/nfca_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfClassicPoller MfClassicPoller;

typedef enum {
    MfClassicPollerEventTypeAuthRequest,
    MfClassicPollerEventTypeAuthSuccess,
    MfClassicPollerEventTypeAuthFailed,
    MfClassicPollerEventTypeReadSuccess,
    MfClassicPollerEventTypeReadFailed,
} MfClassicPollerEventType;

typedef struct {
    MfClassicError error;
} MfClassicPollerEventData;

typedef struct {
    MfClassicPollerEventType type;
    MfClassicPollerEventData data;
} MfClassicPollerEvent;

typedef enum {
    MfClassicPollerCommandContinue = NfcaPollerCommandContinue,
    MfClassicPollerCommandReset = NfcaPollerCommandReset,
    MfClassicPollerCommandStop = NfcaPollerCommandStop,
} MfClassicPollerCommand;

typedef MfClassicPollerCommand (
    *MfClassicPollerCallback)(MfClassicPollerEvent event, void* context);

MfClassicPoller* mf_classic_poller_alloc(NfcaPoller* nfca_poller);

void mf_classic_poller_free(MfClassicPoller* instance);

MfClassicError mf_classic_poller_start(
    MfClassicPoller* instance,
    NfcaPollerEventCallback callback,
    void* context);

MfClassicError mf_classic_poller_read(
    MfClassicPoller* instance,
    MfClassicPollerCallback callback,
    void* context);

MfClassicError mf_classic_poller_get_data(MfClassicPoller* instance, MfClassicData* data);

MfClassicError mf_classic_poller_reset(MfClassicPoller* instance);

MfClassicError mf_classic_poller_stop(MfClassicPoller* instance);

// Sync api

MfClassicError mf_classic_poller_auth(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data);

MfClassicError mf_classic_poller_read_block(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicBlock* data);

#ifdef __cplusplus
}
#endif
