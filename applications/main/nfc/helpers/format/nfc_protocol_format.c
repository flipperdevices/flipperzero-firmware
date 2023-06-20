#include "nfc_protocol_format.h"

#include <nfc/protocols/nfca/nfca.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/mf_desfire/mf_desfire.h>

typedef void (*NfcProtocolFormatRenderInfo)(const NfcDev* device, FuriString* str);

typedef struct {
    NfcProtocolFormatRenderInfo render_info;
    const NfcProtocolFormatFeature flags;
} NfcProtocolFormatBase;

static void
    nfc_protocol_format_common_iso14443_3a_render_info(const NfcaData* nfc_data, FuriString* str) {
    const char iso_type = FURI_BIT(nfc_data->sak, 5) ? '4' : '3';

    furi_string_cat_printf(str, "ISO 14443-%c (NFC-A)\n", iso_type);
    furi_string_cat_printf(str, "UID:");

    for(size_t i = 0; i < nfc_data->uid_len; i++) {
        furi_string_cat_printf(str, " %02X", nfc_data->uid[i]);
    }

    furi_string_cat_printf(str, "\nATQA: %02X %02X ", nfc_data->atqa[1], nfc_data->atqa[0]);
    furi_string_cat_printf(str, " SAK: %02X", nfc_data->sak);
}

static void nfc_protocol_format_iso14443_3a_render_info(const NfcDev* device, FuriString* str) {
    furi_string_cat_printf(
        str, "\e#%s\n", nfc_dev_get_protocol_name(device, NfcProtocolNameTypeFull));

    const NfcaData* data = nfc_dev_get_protocol_data(device, NfcProtocolTypeIso14443_3a);
    nfc_protocol_format_common_iso14443_3a_render_info(data, str);
}

static void nfc_protocol_format_iso14443_4a_render_info(const NfcDev* device, FuriString* str) {
    furi_string_cat_printf(
        str, "\e#%s\n", nfc_dev_get_protocol_name(device, NfcProtocolNameTypeFull));

    const Iso14443_4aData* data = nfc_dev_get_protocol_data(device, NfcProtocolTypeIso14443_4a);
    nfc_protocol_format_common_iso14443_3a_render_info(data->iso14443_3a_data, str);
}

static void nfc_protocol_format_mf_ultralight_render_info(const NfcDev* device, FuriString* str) {
    furi_string_cat_printf(
        str, "\e#%s\n", nfc_dev_get_protocol_name(device, NfcProtocolNameTypeFull));

    const MfUltralightData* data = nfc_dev_get_protocol_data(device, NfcProtocolTypeMfUltralight);
    nfc_protocol_format_common_iso14443_3a_render_info(data->nfca_data, str);
}

static void nfc_protocol_format_mf_classic_render_info(const NfcDev* device, FuriString* str) {
    furi_string_cat_printf(
        str, "\e#%s\n", nfc_dev_get_protocol_name(device, NfcProtocolNameTypeFull));

    const MfClassicData* data = nfc_dev_get_protocol_data(device, NfcProtocolTypeMfClassic);
    nfc_protocol_format_common_iso14443_3a_render_info(data->nfca_data, str);
}

// TODO: use proper type getters
static void nfc_protocol_format_mf_desfire_render_info(const NfcDev* device, FuriString* str) {
    furi_string_cat_printf(
        str, "\e#%s\n", nfc_dev_get_protocol_name(device, NfcProtocolNameTypeFull));

    const MfDesfireData* data = nfc_dev_get_protocol_data(device, NfcProtocolTypeMfDesfire);
    nfc_protocol_format_common_iso14443_3a_render_info(
        data->iso14443_4a_data->iso14443_3a_data, str);

    const uint32_t bytes_total = 1UL << (data->version.sw_storage >> 1);
    const uint32_t bytes_free = data->free_memory.is_present ? data->free_memory.bytes_free : 0;

    furi_string_cat_printf(str, "\n%lu", bytes_total);

    if(data->version.sw_storage & 1) {
        furi_string_push_back(str, '+');
    }

    furi_string_cat_printf(str, " bytes, %lu bytes free\n", bytes_free);

    const uint32_t app_count = simple_array_get_count(data->applications);
    uint32_t file_count = 0;

    for(uint32_t i = 0; i < app_count; ++i) {
        const MfDesfireApplication* app = simple_array_cget(data->applications, i);
        file_count += simple_array_get_count(app->file_ids);
    }

    furi_string_cat_printf(str, "%lu Application%s", app_count, app_count != 1 ? "s" : "");
    furi_string_cat_printf(str, ", %lu File%s", file_count, file_count != 1 ? "s" : "");
}

static const NfcProtocolFormatBase nfc_protocol_format[NfcProtocolTypeMax] = {
    [NfcProtocolTypeIso14443_3a] =
        {
            .flags = NfcProtocolFormatFeatureNone,
            .render_info = nfc_protocol_format_iso14443_3a_render_info,
        },
    [NfcProtocolTypeIso14443_4a] =
        {
            .flags = NfcProtocolFormatFeatureNone,
            .render_info = nfc_protocol_format_iso14443_4a_render_info,
        },
    [NfcProtocolTypeMfUltralight] =
        {
            .flags = NfcProtocolFormatFeatureMoreData,
            .render_info = nfc_protocol_format_mf_ultralight_render_info,
        },
    [NfcProtocolTypeMfClassic] =
        {
            .flags = NfcProtocolFormatFeatureMoreData,
            .render_info = nfc_protocol_format_mf_classic_render_info,
        },
    [NfcProtocolTypeMfDesfire] =
        {
            .flags = NfcProtocolFormatFeatureMoreData,
            .render_info = nfc_protocol_format_mf_desfire_render_info,
        },
};

NfcProtocolFormatFeature nfc_protocol_format_get_features(const NfcDev* device) {
    furi_assert(device);
    return nfc_protocol_format[nfc_dev_get_protocol_type(device)].flags;
}

void nfc_protocol_format_render_info(const NfcDev* device, FuriString* str) {
    furi_assert(device);
    nfc_protocol_format[nfc_dev_get_protocol_type(device)].render_info(device, str);
}
