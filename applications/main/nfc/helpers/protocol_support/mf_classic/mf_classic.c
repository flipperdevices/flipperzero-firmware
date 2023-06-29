#include <nfc/protocols/mf_classic/mf_classic_poller.h>

#include "../iso14443_3a/iso14443_3a_i.h"

static void nfc_protocol_support_render_info_mf_classic(
    const MfClassicData* data,
    NfcProtocolFormatType type,
    FuriString* str) {
    nfc_protocol_support_render_info_iso14443_3a_common(data->iso14443_3a_data, type, str);

    uint8_t sectors_total = mf_classic_get_total_sectors_num(data->type);
    uint8_t keys_total = sectors_total * 2;
    uint8_t keys_found = 0;
    uint8_t sectors_read = 0;
    mf_classic_get_read_sectors_and_keys(data, &sectors_read, &keys_found);

    furi_string_cat_printf(str, "\nKeys Found: %u/%u", keys_found, keys_total);
    furi_string_cat_printf(str, "\nSectors Read: %u/%u", sectors_read, sectors_total);

    // TODO: Something else?
}

static NfcCustomEvent
    nfc_protocol_support_handle_read_mf_classic(MfClassicPollerEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);

    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;
    // TODO: Implement read mf_classic using key cache
    return custom_event;
}

const NfcProtocolSupportBase nfc_protocol_support_mf_classic = {
    .features = NfcProtocolFeatureMoreData,
    .render_info = (NfcProtocolSupportRenderInfo)nfc_protocol_support_render_info_mf_classic,
    .handle_read = (NfcProtocolSupportReadHandler)nfc_protocol_support_handle_read_mf_classic,
};
