#pragma once

#include <nfc/protocols/felica_seac/felica_seac.h>

#include "../nfc_protocol_support_render_common.h"

void nfc_render_felica_seac_info(
    const FelicaSeacData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
