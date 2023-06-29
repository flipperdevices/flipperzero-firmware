#pragma once

typedef enum {
    NfcProtocolFeatureNone = 0,
    NfcProtocolFeatureMoreData = 1UL << 0,
} NfcProtocolFeature;

typedef enum {
    NfcProtocolFormatTypeShort,
    NfcProtocolFormatTypeFull,
} NfcProtocolFormatType;
