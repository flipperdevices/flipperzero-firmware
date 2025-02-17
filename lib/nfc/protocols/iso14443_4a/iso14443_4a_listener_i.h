#pragma once

#include <nfc/protocols/nfc_generic_event.h>

#include "iso14443_4a_listener_history_data.h"
#include "iso14443_4a_listener.h"
#include "iso14443_4a_i.h"
#include <helpers/logger/nfc_logger_i.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Iso14443_4aListener {
    Iso14443_3aListener* iso14443_3a_listener;
    Iso14443_4aData* data;
    Iso14443_4aListenerState state;

    BitBuffer* tx_buffer;

    NfcGenericEvent generic_event;
    Iso14443_4aListenerEvent iso14443_4a_event;
    Iso14443_4aListenerEventData iso14443_4a_event_data;
    NfcGenericCallback callback;
    NfcGenericLogHistoryCallback log_callback;
    NfcHistoryItem history;
    Iso14443_4aListenerHistoryData history_data;
    void* context;
};

Iso14443_4aError
    iso14443_4a_listener_send_ats(Iso14443_4aListener* instance, const Iso14443_4aAtsData* data);

#ifdef __cplusplus
}
#endif
