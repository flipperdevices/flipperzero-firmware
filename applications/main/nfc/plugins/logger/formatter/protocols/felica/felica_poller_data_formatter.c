#include "../nfc_hal/nfc_hal_formatter.h"
#include "../nfc_protocol_formatter_base.h"
#include <nfc/protocols/felica/felica_poller_history_data.h>

static const char* felica_errors[] = {
    [FelicaErrorNone] = "None",
    [FelicaErrorNotPresent] = "NotPresent",
    [FelicaErrorColResFailed] = "Collision Failed",
    [FelicaErrorBufferOverflow] = "Buffer Overflow",
    [FelicaErrorCommunication] = "Communication Error",
    [FelicaErrorFieldOff] = "Field Off",
    [FelicaErrorWrongCrc] = "WrongCrc",
    [FelicaErrorProtocol] = "Protocol Error",
    [FelicaErrorTimeout] = "Timeout",
};

static const char* states[FelicaPollerStateNum] = {
    [FelicaPollerStateIdle] = "Idle",
    [FelicaPollerStateActivated] = "Activated",
    [FelicaPollerStateAuthenticateInternal] = "Auth Internal",
    [FelicaPollerStateAuthenticateExternal] = "Auth External",
    [FelicaPollerStateReadBlocks] = "Read Blocks",
    [FelicaPollerStateReadSuccess] = "Read Success",
    [FelicaPollerStateReadFailed] = "Read Failed",
};

static void felica_poller_format_data(
    const NfcPacket* packet,
    const NfcHistoryData* data,
    FuriString* output) {
    const FelicaPollerHistoryData* felica_data = data;
    UNUSED(packet);

    const char* event_text = nfc_hal_data_format_event_type(felica_data->event);
    const char* state_text = states[felica_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(felica_data->command);
    const char* error = felica_errors[felica_data->error];

    furi_string_printf(
        output, "E=%s, S=%s, Err: %s, C=%s", event_text, state_text, error, command_text);
}

static NfcHistoryCrcStatus felica_get_crc_status(const NfcHistoryData* data) {
    const FelicaPollerHistoryData* poller_data = data;
    return (poller_data->error == FelicaErrorWrongCrc) ? NfcHistoryCrcBad : NfcHistoryCrcOk;
}

const NfcProtocolFormatterBase felica_poller_data_formatter = {
    .format_history = felica_poller_format_data,
    .get_crc_status = felica_get_crc_status,
};
