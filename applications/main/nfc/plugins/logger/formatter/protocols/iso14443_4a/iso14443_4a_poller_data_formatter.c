#include "iso14443_4a_poller_data_formatter_i.h"
#include "../iso14443_3a/iso14443_3a_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller_history_data.h>

static const char* states[] = {
    [Iso14443_4aPollerStateIdle] = "Idle",
    [Iso14443_4aPollerStateReadAts] = "Read Ats",
    [Iso14443_4aPollerStateError] = "Error",
    [Iso14443_4aPollerStateReady] = "Ready",
};

static const char* session_states[] = {
    [Iso14443_4aPollerSessionStateIdle] = "Idle",
    [Iso14443_4aPollerSessionStateActive] = "Active",
    [Iso14443_4aPollerSessionStateStopRequest] = "Stop Request",
};

static const char* errors[] = {
    [Iso14443_4aErrorNone] = "None",
    [Iso14443_4aErrorNotPresent] = "Not Present",
    [Iso14443_4aErrorProtocol] = "Protocol error",
    [Iso14443_4aErrorTimeout] = "Timeout",
};

static const char* events[] = {
    [Iso14443_4aPollerEventTypeError] = "Error",
    [Iso14443_4aPollerEventTypeReady] = "Ready",
};

const char* iso14443_4a_poller_data_format_event_type(const Iso14443_4aPollerEventType event) {
    return events[event];
}

static void iso14443_4a_poller_data_format(
    const NfcPacket* response,
    const NfcHistoryData* data,
    FuriString* output) {
    UNUSED(response);
    const Iso14443_4aPollerHistoryData* poller_data = data;

    const char* event_text = iso14443_3a_poller_data_format_event_type(poller_data->event);
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

const NfcProtocolFormatterBase iso14443_4a_poller_data_formatter = {
    .format_history = iso14443_4a_poller_data_format,
    .get_crc_status = NULL,
};
