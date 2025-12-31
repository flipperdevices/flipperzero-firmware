
#include "slix_poller_data_formatter.h"
#include "slix_error_formatter.h"
#include "../iso15693_3/iso15693_3_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/slix/slix_poller_history_data.h>

static const char* states[SlixPollerStateNum] = {
    [SlixPollerStateIdle] = "Idle",
    [SlixPollerStateGetNxpSysInfo] = "GetNxpSysInfo",
    [SlixPollerStateReadSignature] = "Read Signature",
    [SlixPollerStateCheckPrivacyPassword] = "Check Privacy Pwd",
    [SlixPollerStatePrivacyUnlock] = "Privacy Unlock",
    [SlixPollerStateReady] = "Ready",
    [SlixPollerStateError] = "Error",
};

static void iso15693_3_poller_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const SlixPollerHistoryData* slix_data = data;
    UNUSED(request);

    const char* event_text = iso15693_3_poller_data_format_event_type(slix_data->event);
    const char* error_text = slix_format_error(slix_data->error);
    const char* state_text = states[slix_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(slix_data->command);

    furi_string_printf(
        output, "E=%s, S=%s, Err=%s, C=%s", event_text, state_text, error_text, command_text);
}

const NfcProtocolFormatterBase slix_poller_data_formatter = {
    .format_history = iso15693_3_poller_data_format,
    .get_crc_status = NULL,
};
