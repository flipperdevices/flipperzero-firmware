
#include "iso14443_3a_listener_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_listener_history_data.h>

static const char* events[] = {
    [Iso14443_3aListenerEventTypeFieldOff] = "Field Off",
    [Iso14443_3aListenerEventTypeHalted] = "Halt",
    [Iso14443_3aListenerEventTypeReceivedStandardFrame] = "RxFrame",
    [Iso14443_3aListenerEventTypeReceivedData] = "RxData",
};

static const char* states[] = {
    [Iso14443_3aListenerStateActive] = "Active",
    [Iso14443_3aListenerStateIdle] = "Idle",
};

static void iso14443_3a_listener_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const Iso14443_3aListenerHistoryData* iso14443_3a_data = data;
    UNUSED(request);

    const char* event_text = nfc_hal_data_format_event_type(iso14443_3a_data->event);
    const char* state_text = states[iso14443_3a_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(iso14443_3a_data->command);
    furi_string_printf(output, "E=%s, S=%s, C=%s", event_text, state_text, command_text);
}

const char* iso14443_3a_listener_data_format_event_type(Iso14443_3aListenerEventType event) {
    return events[event];
}

const NfcProtocolFormatterBase iso14443_3a_listener_data_formatter = {
    .format_history = iso14443_3a_listener_data_format,
    .get_crc_status = NULL,
};
