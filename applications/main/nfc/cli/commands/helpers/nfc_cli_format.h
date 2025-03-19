#pragma once

#include <furi.h>

void nfc_cli_format_array(
    const uint8_t* data,
    const size_t data_size,
    const char* header,
    FuriString* output);

void nfc_cli_printf_array(const uint8_t* data, const size_t data_size, const char* header);
