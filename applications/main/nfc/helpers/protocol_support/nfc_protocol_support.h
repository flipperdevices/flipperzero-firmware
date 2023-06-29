#pragma once

#include <nfc/nfc_device.h>
#include <nfc/protocols/nfc_generic_event.h>

#include "../nfc_custom_event.h"

#include "nfc_protocol_support_common.h"

NfcProtocolFeature nfc_protocol_support_get_features(const NfcDevice* device);

void nfc_protocol_support_render_info(
    const NfcDevice* device,
    NfcProtocolFormatType format_type,
    FuriString* str);

NfcCustomEvent nfc_protocol_support_handle_read(NfcGenericEvent event, void* context);
