#pragma once

#include "../nfc_protocol_support_render_common.h"
#include <nfc/protocols/innovatron/innovatron.h>

void nfc_render_innovatron_info(
    const InnovatronData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
