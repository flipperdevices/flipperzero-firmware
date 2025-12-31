#include "iso14443_4a_listener_data_formatter.h"
#include "../iso14443_3a/iso14443_3a_listener_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include <nfc/protocols/iso14443_4a/iso14443_4a_listener_history_data.h>

static const char* states[] = {
    [Iso14443_4aListenerStateActive] = "Active",
    [Iso14443_4aListenerStateIdle] = "Idle",
};

static void iso14443_4a_listener_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const Iso14443_4aListenerHistoryData* iso14443_4a_data = data;
    UNUSED(request);

    const char* event_text = iso14443_3a_listener_data_format_event_type(iso14443_4a_data->event);
    const char* state_text = states[iso14443_4a_data->state];
    const char* command_text = nfc_hal_data_format_nfc_command(iso14443_4a_data->command);
    furi_string_printf(output, "E=%s, S=%s, C=%s", event_text, state_text, command_text);
}

const NfcProtocolFormatterBase iso14443_4a_listener_data_formatter = {
    .format_history = iso14443_4a_listener_data_format,
    .get_crc_status = NULL,
};
