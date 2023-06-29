#include "iso14443_3a_i.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

void nfc_protocol_support_render_info_iso14443_3a_common(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        const char iso_type = FURI_BIT(data->sak, 5) ? '4' : '3';
        furi_string_cat_printf(str, "ISO 14443-%c (NFC-A)\n", iso_type);
    }

    furi_string_cat_printf(str, "UID:");

    for(size_t i = 0; i < data->uid_len; i++) {
        furi_string_cat_printf(str, " %02X", data->uid[i]);
    }

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "\nATQA: %02X %02X ", data->atqa[1], data->atqa[0]);
        furi_string_cat_printf(str, " SAK: %02X", data->sak);
    }
}

static void nfc_protocol_support_render_info_iso14443_3a(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    UNUSED(format_type);
    nfc_protocol_support_render_info_iso14443_3a_common(data, NfcProtocolFormatTypeFull, str);
}

static NfcCustomEvent
    nfc_protocol_support_handle_read_iso14443_3a(Iso14443_3aPollerEvent* event, void* context) {
    UNUSED(context);
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == Iso14443_3aPollerEventTypeReady) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    }

    return custom_event;
}

const NfcProtocolSupportBase nfc_protocol_support_iso14443_3a = {
    .features = NfcProtocolFeatureNone,
    .render_info = (NfcProtocolSupportRenderInfo)nfc_protocol_support_render_info_iso14443_3a,
    .handle_read = (NfcProtocolSupportReadHandler)nfc_protocol_support_handle_read_iso14443_3a,
};
