#pragma once

#include "slix_poller_history_data.h"
#include <nfc/protocols/iso15693_3/iso15693_3_poller_i.h>
#include <helpers/logger/nfc_logger_i.h>

#define SLIX_POLLER_SET_PASSWORD_FWT (100000)

#ifdef __cplusplus
extern "C" {
#endif

struct SlixPoller {
    Iso15693_3Poller* iso15693_3_poller;
    SlixType type;
    SlixData* data;
    SlixPollerState poller_state;
    SlixError error;
    SlixRandomNumber random_number;
    bool privacy_password_checked;

    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    SlixPollerEventData slix_event_data;
    SlixPollerEvent slix_event;
    NfcGenericEvent general_event;
    NfcGenericCallback callback;
    NfcGenericLogHistoryCallback log_callback;
    NfcHistoryItem history;
    SlixPollerHistoryData history_data;
    void* context;
};

#ifdef __cplusplus
}
#endif
