#pragma once

#include "mf_desfire_poller.h"
#include "mf_desfire_poller_history_data.h"

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a_poller_i.h>
#include <helpers/logger/nfc_logger_i.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfDesfirePollerSessionStateIdle,
    MfDesfirePollerSessionStateActive,
    MfDesfirePollerSessionStateStopRequest,
} MfDesfirePollerSessionState;

struct MfDesfirePoller {
    Iso14443_4aPoller* iso14443_4a_poller;
    MfDesfirePollerSessionState session_state;
    MfDesfirePollerState state;
    MfDesfireError error;
    MfDesfireData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    BitBuffer* input_buffer;
    BitBuffer* result_buffer;
    MfDesfirePollerEventData mf_desfire_event_data;
    MfDesfirePollerEvent mf_desfire_event;
    NfcGenericEvent general_event;
    NfcGenericCallback callback;
    NfcGenericLogHistoryCallback log_callback;
    NfcHistoryItem history;
    MfDesfirePollerHistoryData history_data;
    void* context;
};

MfDesfireError mf_desfire_process_error(Iso14443_4aError error);

MfDesfireError mf_desfire_process_status_code(uint8_t status_code);

const MfDesfireData* mf_desfire_poller_get_data(MfDesfirePoller* instance);

#ifdef __cplusplus
}
#endif
