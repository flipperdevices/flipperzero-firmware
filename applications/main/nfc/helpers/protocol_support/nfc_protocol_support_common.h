#pragma once

#include <nfc/nfc_device.h>
#include <nfc/protocols/nfc_generic_event.h>

#include "../nfc_custom_event.h"
#include "../../scenes/nfc_scene.h"
#include "../../nfc_app.h"

typedef enum {
    NfcProtocolFeatureNone = 0,
    NfcProtocolFeatureMoreData = 1UL << 0,
} NfcProtocolFeature;

typedef enum {
    NfcProtocolFormatTypeShort,
    NfcProtocolFormatTypeFull,
} NfcProtocolFormatType;

enum {
    SubmenuIndexCommonInfo,
    SubmenuIndexCommonRename,
    SubmenuIndexCommonDelete,
    SubmenuIndexCommonMax,
};

void nfc_protocol_support_common_submenu_callback(void* context, uint32_t index);
