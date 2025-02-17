#pragma once

#include "iso15693_3_poller.h"
//#include "iso15693_3_poller_history_data.h"

#include "iso15693_3_i.h"
#include <helpers/logger/nfc_logger_i.h>

#include <toolbox/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO15693_3_POLLER_MAX_BUFFER_SIZE (64U)

struct Iso15693_3Poller {
    Nfc* nfc;
    Iso15693_3PollerState state;
    Iso15693_3Data* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    NfcGenericEvent general_event;
    Iso15693_3PollerEvent iso15693_3_event;
    Iso15693_3PollerEventData iso15693_3_event_data;
    NfcGenericCallback callback;
    NfcGenericLogHistoryCallback log_callback;
    NfcHistoryItem history;
    Iso15693_3PollerHistoryData history_data;
    void* context;
};

const Iso15693_3Data* iso15693_3_poller_get_data(Iso15693_3Poller* instance);

#ifdef __cplusplus
}
#endif
