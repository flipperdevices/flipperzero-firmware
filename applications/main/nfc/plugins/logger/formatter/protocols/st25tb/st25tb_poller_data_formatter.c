
#include "st25tb_poller_data_formatter.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/st25tb/st25tb_poller_history_data.h>

static const char* errors[] = {
    [St25tbErrorNone] = "None",
    [St25tbErrorNotPresent] = "Not Present",
    [St25tbErrorColResFailed] = "Collision Failed",
    [St25tbErrorBufferOverflow] = "Buffer Overflow",
    [St25tbErrorCommunication] = "Communication Error",
    [St25tbErrorFieldOff] = "Field Off",
    [St25tbErrorWrongCrc] = "Wrong Crc",
    [St25tbErrorTimeout] = "Timeout",
    [St25tbErrorWriteFailed] = "Write Failed",
};

static const char* states[] = {
    [St25tbPollerStateSelect] = "Select",
    [St25tbPollerStateRequestMode] = "Request Mode",
    [St25tbPollerStateRead] = "Read",
    [St25tbPollerStateWrite] = "Write",
    [St25tbPollerStateSuccess] = "Success",
    [St25tbPollerStateFailure] = "Failure",
};

static void st25_tb_poller_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const St25tbPollerHistoryData* st25tb_data = data;
    UNUSED(request);

    const char* event_text = nfc_hal_data_format_event_type(st25tb_data->event);
    const char* error_text = errors[st25tb_data->error];
    const char* state_text = states[st25tb_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(st25tb_data->command);

    furi_string_printf(
        output, "E=%s,S=%s, Err=%s, C=%s", event_text, state_text, error_text, command_text);
}

static NfcHistoryCrcStatus st25_tb_poller_get_crc_status(const NfcHistoryData* data) {
    const St25tbPollerHistoryData* poller_data = data;
    return (poller_data->error == St25tbErrorWrongCrc) ? NfcHistoryCrcBad : NfcHistoryCrcOk;
}

const NfcProtocolFormatterBase st25tb_poller_data_formatter = {
    .format_history = st25_tb_poller_data_format,
    .get_crc_status = st25_tb_poller_get_crc_status,
};
