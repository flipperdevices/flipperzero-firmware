#include "mf_classic_listener_data_formatter.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include "../iso14443_3a/iso14443_3a_listener_data_formatter_i.h"
#include <nfc/helpers/crypto1.h>
#include <nfc/protocols/mf_classic/mf_classic_listener_history_data.h>

#define TAG "MfcFormatter"

static const char* mfc_cmd[] = {
    [MfClassicListenerCommandProcessed] = "Processed",
    [MfClassicListenerCommandAck] = "Processed ACK",
    [MfClassicListenerCommandSilent] = "Processed Silent",
    [MfClassicListenerCommandNack] = "Not Processed NAK",
    [MfClassicListenerCommandSleep] = "Sleep",
};

static const char* mfc_state[] = {
    [MfClassicListenerStateIdle] = "Idle",
    [MfClassicListenerStateAuthComplete] = "Authenticated",
};

static const char* comm_states[] = {
    [MfClassicListenerCommStatePlain] = "Plain",
    [MfClassicListenerCommStateEncrypted] = "Encrypted",
};

void mf_classic_listener_data_format(
    const NfcPacket* packet,
    const NfcHistoryData* data,
    FuriString* output) {
    furi_assert(data);
    furi_assert(output);
    UNUSED(packet);
    const MfClassicListenerHistoryData* mf_classic_data = data;

    const char* event_text = iso14443_3a_listener_data_format_event_type(mf_classic_data->event);
    const char* state_text = mfc_state[mf_classic_data->state];
    const char* comm_text = comm_states[mf_classic_data->comms_state];
    const char* process_text = mfc_cmd[mf_classic_data->mfc_command];
    const char* result_text = nfc_hal_data_format_nfc_command(mf_classic_data->command);

    furi_string_printf(
        output,
        "E=%s, S=%s, Com=%s, MFC=%s, R=%s",
        event_text,
        state_text,
        comm_text,
        process_text,
        result_text);
}

static NfcHistoryCrcStatus mf_classic_listener_get_crc_status(const NfcHistoryData* data) {
    const MfClassicListenerHistoryData* mf_classic_data = data;
    const Iso14443_3aListenerEventType event = mf_classic_data->event;
    const MfClassicListenerCommState comm_state = mf_classic_data->comms_state;
    const MfClassicListenerState state = mf_classic_data->state;
    const bool crc_ok = mf_classic_data->crc_ok;

    NfcHistoryCrcStatus status = NfcHistoryCrcNotSet;

    if((event == Iso14443_3aListenerEventTypeReceivedData) ||
       (event == Iso14443_3aListenerEventTypeReceivedStandardFrame)) {
        if(comm_state == MfClassicListenerCommStateEncrypted) {
            if(state == MfClassicListenerStateAuthComplete) {
                status = crc_ok ? NfcHistoryCrcOk : NfcHistoryCrcBad;
            }
        } else {
            status = crc_ok ? NfcHistoryCrcOk : NfcHistoryCrcBad;
        }
    }
    return status;
}

static bool mf_classic_listener_decrypt_payload(
    const NfcPacket* packet,
    NfcHistoryData* data,
    NfcPacket* output) {
    furi_assert(packet);
    furi_assert(data);
    furi_assert(output);

    MfClassicListenerHistoryData* mf_classic_data = data;

    bool result = false;
    do {
        if(mf_classic_data->comms_state != MfClassicListenerCommStateEncrypted) {
            FURI_LOG_D(TAG, "Not encrypted");
            break;
        }

        if(mf_classic_data->state != MfClassicListenerStateAuthComplete) {
            FURI_LOG_D(TAG, "Encypted nonces no need to decrypt");
            break;
        }

        Crypto1* crypto = &mf_classic_data->crypto;
        for(size_t i = 0; i < packet->data_size; i++) {
            output->data[i] = crypto1_byte(crypto, 0, 0) ^ packet->data[i];
        }

        result = true;
    } while(false);

    return result;
}

const NfcProtocolFormatterBase mf_classic_listener_data_formatter = {
    .format_history = mf_classic_listener_data_format,
    .get_crc_status = mf_classic_listener_get_crc_status,
    .decrypt = mf_classic_listener_decrypt_payload,
};
