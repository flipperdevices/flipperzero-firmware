#pragma once

#include <nfc/protocols/mf_ultralight/mf_ultralight.h>

#include "../nfc_protocol_support_render_common.h"

void nfc_render_mf_ultralight_info(
    const MfUltralightData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
