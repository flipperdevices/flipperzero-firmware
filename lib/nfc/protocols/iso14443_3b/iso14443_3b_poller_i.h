#pragma once

#include "iso14443_3b_poller.h"
#include "iso14443_3b_poller_history_data.h"
#include "iso14443_3b_i.h"
#include <helpers/logger/nfc_logger_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO14443_3B_POLLER_MAX_BUFFER_SIZE (256U)

struct Iso14443_3bPoller {
    Nfc* nfc;
    Iso14443_3bPollerState state;
    Iso14443_3bData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    NfcGenericEvent general_event;
    Iso14443_3bPollerEvent iso14443_3b_event;
    Iso14443_3bPollerEventData iso14443_3b_event_data;
    NfcGenericCallback callback;
    NfcGenericLogHistoryCallback log_callback;
    NfcHistoryItem history;
    Iso14443_3bPollerHistoryData history_data;
    void* context;
};

const Iso14443_3bData* iso14443_3b_poller_get_data(Iso14443_3bPoller* instance);

#ifdef __cplusplus
}
#endif
