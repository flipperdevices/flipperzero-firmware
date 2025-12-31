#include "iso14443_4b_poller_data_formatter.h"
#include "../iso14443_3b/iso14443_3b_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/iso14443_4b/iso14443_4b_poller_history_data.h>

static const char* states[] = {
    [Iso14443_4bPollerStateIdle] = "Idle",
    [Iso14443_4bPollerStateError] = "Error",
    [Iso14443_4bPollerStateReady] = "Ready",

};

static const char* session_states[] = {
    [Iso14443_4bPollerSessionStateIdle] = "Idle",
    [Iso14443_4bPollerSessionStateActive] = "Active",
    [Iso14443_4bPollerSessionStateStopRequest] = "Stop Request",
};

static const char* errors[] = {
    [Iso14443_4bErrorNone] = "None",
    [Iso14443_4bErrorNotPresent] = "Not Present",
    [Iso14443_4bErrorProtocol] = "Protocol error",
    [Iso14443_4bErrorTimeout] = "Timeout",
};

static void iso14443_4b_poller_data_format(
    const NfcPacket* response,
    const NfcHistoryData* data,
    FuriString* output) {
    UNUSED(response);
    const Iso14443_4bPollerHistoryData* poller_data = data;

    const char* event_text = iso14443_3b_poller_data_format_event_type(poller_data->event);
    const char* state_text = states[poller_data->state];
    const char* session_state_text = session_states[poller_data->session_state];
    const char* error_text = errors[poller_data->error];
    const char* command_text = nfc_hal_data_format_nfc_command(poller_data->command);
    furi_string_printf(
        output,
        "E=%s, S=%s, Sst=%s, Err=%s, C=%s",
        event_text,
        state_text,
        session_state_text,
        error_text,
        command_text);
}

const NfcProtocolFormatterBase iso14443_4b_poller_data_formatter = {
    .format_history = iso14443_4b_poller_data_format,
    .get_crc_status = NULL,
};
