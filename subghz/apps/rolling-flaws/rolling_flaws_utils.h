#pragma once

#include <furi.h>

#ifdef TAG
#undef TAG
#endif
#define TAG "RollingFlawsUtils"

size_t __furi_string_extract_string(
    FuriString* buffer,
    size_t start_index,
    char* text,
    char delim,
    FuriString* result);

size_t __furi_string_extract_string_until(
    FuriString* buffer,
    size_t start_index,
    char* text,
    char until_delim,
    FuriString* result);

uint32_t
    __furi_string_extract_int(FuriString* buffer, char* text, char delim, uint32_t default_value);

uint32_t __furi_string_hex_to_uint32(FuriString* str);

void __gui_redraw();