#pragma once

#include "mf_desfire_poller.h"

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a_poller_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MF_DESFIRE_POLLER_STANDARD_FWT_FC (60000)

typedef enum {
    MfDesfirePollerStateIdle,
    MfDesfirePollerStateReadVersion,
    MfDesfirePollerStateReadFreeMemory,
    MfDesfirePollerStateReadMasterKey,
    MfDesfirePollerStateReadFailed,
    MfDesfirePollerStateReadSuccess,

    MfDesfirePollerStateNum,
} MfDesfirePollerState;

typedef enum {
    MfDesfirePollerSessionStateIdle,
    MfDesfirePollerSessionStateActive,
    MfDesfirePollerSessionStateStopRequest,
} MfDesfirePollerSessionState;

struct MfDesfirePoller {
    Iso14443_4aPoller* iso14443_4a_poller;
    MfDesfirePollerSessionState session_state;
    MfDesfirePollerState state;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    BitBuffer* input_buffer;
    BitBuffer* result_buffer;
    MfDesfireData* data;
    MfDesfirePollerCallback callback;
    MfDesfireError error;
    void* context;
};

MfDesfireError mf_desfire_process_error(Iso14443_4aError error);

MfDesfireError mf_desfire_send_chunks(
    MfDesfirePoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data);

MfDesfireError
    mf_desfire_poller_async_read_free_memory(MfDesfirePoller* instance, MfDesfireFreeMemory* data);

MfDesfireError mf_desfire_poller_async_read_key_settings(
    MfDesfirePoller* instance,
    MfDesfireKeySettings* data);

MfDesfireError mf_desfire_poller_async_read_key_version(
    MfDesfirePoller* instance,
    MfDesfireKeyVersion* data,
    size_t key_count);

MfDesfireError mf_desfire_poller_async_read_key_configuration(
    MfDesfirePoller* instance,
    MfDesfireKeyConfiguration* data);

#ifdef __cplusplus
}
#endif
