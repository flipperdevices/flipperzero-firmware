#pragma once

#include <core/string.h>
#include <nfc/nfc_device.h>
#include <nfc/protocols/nfc_generic_event.h>

#include "nfc_protocol_support_common.h"

#include "../nfc_custom_event.h"

typedef void (*NfcProtocolSupportRenderInfo)(
    const NfcDeviceData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
typedef NfcCustomEvent (
    *NfcProtocolSupportReadHandler)(NfcGenericEventData* event_data, void* context);

typedef struct {
    const NfcProtocolFeature features;
    NfcProtocolSupportRenderInfo render_info;
    NfcProtocolSupportReadHandler handle_read;
} NfcProtocolSupportBase;
