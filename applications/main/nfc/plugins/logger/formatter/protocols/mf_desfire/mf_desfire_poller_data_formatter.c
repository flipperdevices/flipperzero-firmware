
#include "mf_desfire_poller_data_formatter.h"
#include "../iso14443_4a/iso14443_4a_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/mf_desfire/mf_desfire_poller_history_data.h>

static const char* states[] = {
    [MfDesfirePollerStateIdle] = "Idle",
    [MfDesfirePollerStateReadVersion] = "Read Version",
    [MfDesfirePollerStateReadFreeMemory] = "Read Free Memory",
    [MfDesfirePollerStateReadMasterKeySettings] = "Read Master Key Settings",
    [MfDesfirePollerStateReadMasterKeyVersion] = "Read Master Key Version",
    [MfDesfirePollerStateReadApplicationIds] = "Read Application Ids",
    [MfDesfirePollerStateReadApplications] = "Read Applications",
    [MfDesfirePollerStateReadFailed] = "Read Failed",
    [MfDesfirePollerStateReadSuccess] = "Read Success",
};

static const char* errors[] = {
    [MfDesfireErrorNone] = "None",
    [MfDesfireErrorNotPresent] = "Not Present",
    [MfDesfireErrorProtocol] = "Protocol",
    [MfDesfireErrorAuthentication] = "Auth Error",
    [MfDesfireErrorTimeout] = "Timeout",
};

static void mf_desfire_poller_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const MfDesfirePollerHistoryData* mf_desfire_data = data;
    UNUSED(request);

    const char* event_text = iso14443_4a_poller_data_format_event_type(mf_desfire_data->event);
    const char* error_text = errors[mf_desfire_data->error];
    const char* state_text = states[mf_desfire_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(mf_desfire_data->command);

    furi_string_printf(
        output, "E=%s,S=%s, Err=%s, C=%s", event_text, state_text, error_text, command_text);
}

const NfcProtocolFormatterBase mf_desfire_poller_data_formatter = {
    .format_history = mf_desfire_poller_data_format,
    .get_crc_status = NULL,
};
