#include "iso15693_3_listener_data_formatter_i.h"
#include "iso15693_3_error_formatter.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/iso15693_3/iso15693_3_listener_history_data.h>

static const char* states[] = {
    [Iso15693_3ListenerStateReady] = "Ready",
    [Iso15693_3ListenerStateSelected] = "Selected",
    [Iso15693_3ListenerStateQuiet] = "Quiet",
};

static const char* events[] = {
    [Iso15693_3ListenerEventTypeFieldOff] = "Field Off",
    [Iso15693_3ListenerEventTypeCustomCommand] = "Custom Command",
    [Iso15693_3ListenerEventTypeSingleEof] = "Single Eof",
};

const char* iso15693_3_listener_data_format_event_type(const Iso15693_3ListenerEventType event) {
    return events[event];
}

static void iso15693_3_listener_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const Iso15693_3ListenerHistoryData* iso15693_3_data = data;
    UNUSED(request);

    const char* event_text = nfc_hal_data_format_event_type(iso15693_3_data->event);
    const char* state_text = states[iso15693_3_data->state];
    const char* error_text = iso15693_3_format_error(iso15693_3_data->error);
    const char* command_text = nfc_hal_data_format_nfc_command(iso15693_3_data->command);

    furi_string_printf(
        output, "E=%s, S=%s, Err=%s, C=%s", event_text, state_text, error_text, command_text);
}

static NfcHistoryCrcStatus iso15693_3_listener_get_crc_status(const NfcHistoryData* data) {
    const Iso15693_3ListenerHistoryData* poller_data = data;
    return (poller_data->error == Iso15693_3ErrorWrongCrc) ? NfcHistoryCrcBad : NfcHistoryCrcOk;
}

const NfcProtocolFormatterBase iso15693_3_listener_data_formatter = {
    .format_history = iso15693_3_listener_data_format,
    .get_crc_status = iso15693_3_listener_get_crc_status,
};
