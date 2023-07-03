#pragma once

#include <nfc/protocols/iso14443_4a/iso14443_4a.h>

// TODO: Move NfcProtocolFormatType to another file and rename it
#include "../nfc_protocol_support_common.h"

void nfc_render_iso14443_4a_info(
    const Iso14443_4aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
