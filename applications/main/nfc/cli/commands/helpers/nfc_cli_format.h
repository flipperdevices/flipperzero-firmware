#pragma once

#include <furi.h>
#include <nfc/protocols/nfc_protocol.h>

const char* nfc_cli_get_protocol_name(NfcProtocol protocol);

void nfc_cli_format_array(
    const uint8_t* data,
    const size_t data_size,
    const char* header,
    FuriString* output);

void nfc_cli_printf_array(const uint8_t* data, const size_t data_size, const char* header);
