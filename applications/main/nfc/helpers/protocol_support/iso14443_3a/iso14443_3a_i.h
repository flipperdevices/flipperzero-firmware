#pragma once

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>

#include "iso14443_3a.h"

void nfc_protocol_support_render_info_iso14443_3a_common(
    const Iso14443_3aData* iso14443_3a_data,
    NfcProtocolFormatType format_type,
    FuriString* str);
