
#include "mf_plus_poller_data_formatter.h"
#include "../iso14443_4a/iso14443_4a_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/mf_plus/mf_plus_poller_history_data.h>

static const char* states[] = {
    [MfPlusPollerStateIdle] = "Idle",
    [MfPlusPollerStateReadVersion] = "Read Version",
    [MfPlusPollerStateParseIso4] = "Parse Iso4",
    [MfPlusPollerStateReadFailed] = "Read Failed",
    [MfPlusPollerStateReadSuccess] = "Read Success",
};

static const char* errors[] = {
    [MfPlusErrorNone] = "None",
    [MfPlusErrorUnknown] = "Unknown",
    [MfPlusErrorNotPresent] = "Not Present",
    [MfPlusErrorProtocol] = "Protocol",
    [MfPlusErrorAuth] = "Auth Error",
    [MfPlusErrorPartialRead] = "Partial Read",
    [MfPlusErrorTimeout] = "Timeout",
};

static void mf_plus_poller_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const MfPlusPollerHistoryData* mf_plus_data = data;
    UNUSED(request);

    const char* event_text = iso14443_4a_poller_data_format_event_type(mf_plus_data->event);
    const char* error_text = errors[mf_plus_data->error];
    const char* state_text = states[mf_plus_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(mf_plus_data->command);

    furi_string_printf(
        output, "E=%s,S=%s, Err=%s, C=%s", event_text, state_text, error_text, command_text);
}

const NfcProtocolFormatterBase mf_plus_poller_data_formatter = {
    .format_history = mf_plus_poller_data_format,
    .get_crc_status = NULL,
};
