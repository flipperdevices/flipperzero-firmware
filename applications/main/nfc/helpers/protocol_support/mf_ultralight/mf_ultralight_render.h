#pragma once

#include <nfc/protocols/mf_ultralight/mf_ultralight.h>

// TODO: Move NfcProtocolFormatType to another file and rename it
#include "../nfc_protocol_support_common.h"

void nfc_render_mf_ultralight_info(
    const MfUltralightData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
