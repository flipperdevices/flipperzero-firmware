#include "iso15693_3_poller_data_formatter_i.h"
#include "iso15693_3_error_formatter.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/iso15693_3/iso15693_3_poller_history_data.h>

static const char* states[] = {
    [Iso15693_3PollerStateIdle] = "Idle",
    [Iso15693_3PollerStateColResInProgress] = "Collision Resolve",
    [Iso15693_3PollerStateColResFailed] = "Collision Resolve Failed",
    [Iso15693_3PollerStateActivated] = "Activated",
};

static const char* events[] = {
    [Iso15693_3PollerEventTypeError] = "Error",
    [Iso15693_3PollerEventTypeReady] = "Ready",
};

const char* iso15693_3_poller_data_format_event_type(const Iso15693_3PollerEventType event) {
    return events[event];
}

static void iso15693_3_poller_data_format(
    const NfcPacket* response,
    const NfcHistoryData* data,
    FuriString* output) {
    UNUSED(response);
    const Iso15693_3PollerHistoryData* poller_data = data;

    const char* event_text = nfc_hal_data_format_event_type(poller_data->event);
    const char* state_text = states[poller_data->state];
    const char* error_text = iso15693_3_format_error(poller_data->error);
    const char* command_text = nfc_hal_data_format_nfc_command(poller_data->command);

    furi_string_printf(
        output, "E=%s, S=%s, Err=%s, C=%s", event_text, state_text, error_text, command_text);
}

static NfcHistoryCrcStatus iso15693_3_poller_get_crc_status(const NfcHistoryData* data) {
    const Iso15693_3PollerHistoryData* poller_data = data;
    return (poller_data->error == Iso15693_3ErrorWrongCrc) ? NfcHistoryCrcBad : NfcHistoryCrcOk;
}

const NfcProtocolFormatterBase iso15693_3_poller_data_formatter = {
    .format_history = iso15693_3_poller_data_format,
    .get_crc_status = iso15693_3_poller_get_crc_status,
};
