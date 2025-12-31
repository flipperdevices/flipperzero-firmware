
#include "mf_ultralight_listener_data_formatter.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include "../iso14443_3a/iso14443_3a_listener_data_formatter_i.h"
#include <nfc/protocols/mf_ultralight/mf_ultralight_listener_history_data.h>

static const char* mfu_cmd[] = {
    [MfUltralightCommandNotFound] = "NotFound",
    [MfUltralightCommandProcessed] = "Processed",
    [MfUltralightCommandProcessedACK] = "Processed ACK",
    [MfUltralightCommandProcessedSilent] = "Processed Silent",
    [MfUltralightCommandNotProcessedNAK] = "Not Processed NAK",
    [MfUltralightCommandNotProcessedSilent] = "Not Processed Silent",
    [MfUltralightCommandNotProcessedAuthNAK] = "Not Processed Auth NAK",
};

static void mf_ultralight_listener_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    UNUSED(request);
    const MfUltralightListenerHistoryData* mf_ultralight_data = data;

    const char* event_text =
        iso14443_3a_listener_data_format_event_type(mf_ultralight_data->event);
    const char* process_text = mfu_cmd[mf_ultralight_data->mfu_command];
    const char* result_text = nfc_hal_data_format_nfc_command(mf_ultralight_data->command);
    furi_string_printf(output, "E=%s, MFU: %s, R=%s", event_text, process_text, result_text);
}

static void mf_ultralight_listener_data_format_simplified(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    UNUSED(request);
    UNUSED(data);
    furi_string_printf(output, "Simplified test");
}

static NfcHistoryCrcStatus mf_ultralight_listener_get_crc_status(const NfcHistoryData* data) {
    const MfUltralightListenerHistoryData* mf_ultralight_data = data;
    NfcHistoryCrcStatus status = NfcHistoryCrcNotSet;
    if(mf_ultralight_data->event == Iso14443_3aListenerEventTypeReceivedStandardFrame)
        status = NfcHistoryCrcOk;
    else if(mf_ultralight_data->event == Iso14443_3aListenerEventTypeReceivedData)
        status = NfcHistoryCrcBad;
    return status;
}

const NfcProtocolFormatterBase mf_ultralight_listener_data_formatter = {
    .format_history = mf_ultralight_listener_data_format,
    .format_history_simplified = mf_ultralight_listener_data_format_simplified,
    .get_crc_status = mf_ultralight_listener_get_crc_status,
};
