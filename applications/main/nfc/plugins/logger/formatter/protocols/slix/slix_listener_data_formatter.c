
#include "slix_listener_data_formatter.h"
#include "slix_error_formatter.h"
#include "../nfc_hal/nfc_hal_formatter.h"
#include "../iso15693_3/iso15693_3_listener_data_formatter_i.h"
#include <nfc/protocols/slix/slix_listener_history_data.h>

static void iso15693_3_listener_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const SlixListenerHistoryData* slix_data = data;
    UNUSED(request);

    const char* event_text = iso15693_3_listener_data_format_event_type(slix_data->event);
    const char* error_text = slix_format_error(slix_data->error);
    const char* command_text = nfc_hal_data_format_nfc_command(slix_data->command);

    furi_string_printf(output, "E=%s, Err=%s, C=%s", event_text, error_text, command_text);
}

const NfcProtocolFormatterBase slix_listener_data_formatter = {
    .format_history = iso15693_3_listener_data_format,
    .get_crc_status = NULL,
};
