#include "iso14443_3b_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/iso14443_3b/iso14443_3b_poller_history_data.h>

static const char* poller_events[] = {
    [Iso14443_3bPollerEventTypeError] = "Error",
    [Iso14443_3bPollerEventTypeReady] = "Ready",
};

static const char* states[] = {
    [Iso14443_3bPollerStateIdle] = "Idle",
    [Iso14443_3bPollerStateColResInProgress] = "Collision Resolve",
    [Iso14443_3bPollerStateColResFailed] = "Collision Resolve Failed",
    [Iso14443_3bPollerStateActivationInProgress] = "Activation In Progress",
    [Iso14443_3bPollerStateActivationFailed] = "Activation Failed",
    [Iso14443_3bPollerStateActivated] = "Activated",
};

const char* iso14443_3b_poller_data_format_event_type(const Iso14443_3bPollerEventType event) {
    return poller_events[event];
}

static void iso14443_3b_poller_data_format(
    const NfcPacket* response,
    const NfcHistoryData* data,
    FuriString* output) {
    UNUSED(response);
    const Iso14443_3bPollerHistoryData* poller_data = data;

    const char* event_text = nfc_hal_data_format_event_type(poller_data->event);
    const char* state_text = states[poller_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(poller_data->command);
    furi_string_printf(output, "E=%s, S=%s, C=%s", event_text, state_text, command_text);
}

static NfcHistoryCrcStatus iso14443_3b_poller_get_crc_status(const NfcHistoryData* data) {
    const Iso14443_3bPollerHistoryData* poller_data = data;
    return (poller_data->error == Iso14443_3bErrorWrongCrc) ? NfcHistoryCrcBad : NfcHistoryCrcOk;
}

const NfcProtocolFormatterBase iso14443_3b_poller_data_formatter = {
    .format_history = iso14443_3b_poller_data_format,
    .get_crc_status = iso14443_3b_poller_get_crc_status,
};
