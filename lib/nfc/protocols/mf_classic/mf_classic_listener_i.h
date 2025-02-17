#pragma once

#include "mf_classic_listener.h"
#include "mf_classic_listener_history_data.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener_i.h>
#include <nfc/protocols/nfc_generic_event.h>
#include <nfc/helpers/crypto1.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MfClassicListener {
    Iso14443_3aListener* iso14443_3a_listener;
    MfClassicListenerState state;
    MfClassicListenerCommState comm_state;

    MfClassicData* data;
    BitBuffer* tx_plain_buffer;
    BitBuffer* tx_encrypted_buffer;
    BitBuffer* rx_plain_buffer;

    Crypto1* crypto;
    MfClassicAuthContext auth_context;

    // Write block context
    uint8_t write_block;

    // Value operation data
    int32_t transfer_value;
    bool transfer_valid;
    MfClassicValueCommand value_cmd;

    NfcGenericEvent generic_event;
    MfClassicListenerEvent mfc_event;
    MfClassicListenerEventData mfc_event_data;
    NfcGenericCallback callback;
    NfcGenericLogHistoryCallback log_callback;
    void* context;

    bool cmd_in_progress;
    size_t current_cmd_idx;
    size_t current_cmd_handler_idx;

    size_t total_block_num;
    NfcHistoryItem history;
    MfClassicListenerHistoryData history_data;
};

#ifdef __cplusplus
}
#endif
