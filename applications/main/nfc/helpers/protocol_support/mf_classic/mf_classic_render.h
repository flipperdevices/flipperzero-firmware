#pragma once

#include <nfc/protocols/mf_classic/mf_classic.h>

// TODO: Move NfcProtocolFormatType to another file and rename it
#include "../nfc_protocol_support_common.h"

void nfc_render_mf_classic_info(
    const MfClassicData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
