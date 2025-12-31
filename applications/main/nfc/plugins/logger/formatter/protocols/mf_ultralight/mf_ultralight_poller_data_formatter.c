#include "mf_ultralight_poller_data_formatter.h"
#include "../iso14443_3a/iso14443_3a_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"

#include <nfc/protocols/mf_ultralight/mf_ultralight_poller_history_data.h>

static const char* poller_states[MfUltralightPollerStateNum] = {
    [MfUltralightPollerStateIdle] = "Idle",
    [MfUltralightPollerStateRequestMode] = "Request Mode",
    [MfUltralightPollerStateReadVersion] = "Read Version",
    [MfUltralightPollerStateDetectMfulC] = "Detect MfulC",
    [MfUltralightPollerStateDetectNtag203] = "Detect Ntag203",
    [MfUltralightPollerStateGetFeatureSet] = "Get Features",
    [MfUltralightPollerStateReadSignature] = "Read Signature",
    [MfUltralightPollerStateReadCounters] = "Read Counters",
    [MfUltralightPollerStateReadTearingFlags] = "Read Tearing",
    [MfUltralightPollerStateAuth] = "Authenticate",
    [MfUltralightPollerStateAuthMfulC] = "Authenticate MfulC",
    [MfUltralightPollerStateReadPages] = "Read Pages",
    [MfUltralightPollerStateTryDefaultPass] = "Try Default Pass",
    [MfUltralightPollerStateCheckMfulCAuthStatus] = "Check MfulC Auth",
    [MfUltralightPollerStateReadFailed] = "Read Failed",
    [MfUltralightPollerStateReadSuccess] = "Read Success",
    [MfUltralightPollerStateRequestWriteData] = "Request Tx Data",
    [MfUltralightPollerStateWritePages] = "Write Pages",
    [MfUltralightPollerStateWriteFail] = "Write Fail",
    [MfUltralightPollerStateWriteSuccess] = "Write Success",
};

static void mf_ultralight_poller_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const MfUltralightPollerHistoryData* mf_ultralight_data = data;
    UNUSED(request);

    const char* event_text = iso14443_3a_poller_data_format_event_type(mf_ultralight_data->event);
    const char* state_text = poller_states[mf_ultralight_data->state];
    const char* result_text = nfc_hal_data_format_nfc_command(mf_ultralight_data->command);
    furi_string_printf(output, "E=%s, MFU: %s, R=%s", event_text, state_text, result_text);
}

const NfcProtocolFormatterBase mf_ultralight_poller_data_formatter = {
    .format_history = mf_ultralight_poller_data_format,
};
