#pragma once

#include <nfc/nfc_dev.h>

typedef enum {
    NfcProtocolFormatFeatureNone = 0,
    NfcProtocolFormatFeatureMoreData = 1UL << 0,
} NfcProtocolFormatFeature;

NfcProtocolFormatFeature nfc_protocol_format_get_features(const NfcDev* device);

void nfc_protocol_format_render_info(const NfcDev* device, FuriString* str);
