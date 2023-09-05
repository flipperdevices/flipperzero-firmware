#include "rolling_flaws_utils.h"
#include <gui/gui.h>

size_t __furi_string_extract_string(
    FuriString* buffer,
    size_t start_index,
    char* text,
    char delim,
    FuriString* result) {
    size_t len = strlen(text);
    size_t valid_index = furi_string_size(buffer) - 1;
    size_t field = furi_string_search_str(buffer, text, start_index) + len;
    size_t term = -1;
    if(field < valid_index) {
        term = furi_string_search_char(buffer, delim, field);
        if(term < valid_index) {
            furi_string_reset(result);
            furi_string_set_n(result, buffer, field, term - field);
            FURI_LOG_I(TAG, "%s data is >>%s<<", text, furi_string_get_cstr(result));
        } else {
            FURI_LOG_E(TAG, "Failed to find terminator for >>%s<<", text);
        }
    } else {
        FURI_LOG_E(TAG, "Failed to find >>%s<<", text);
    }

    return term;
}

size_t __furi_string_extract_string_until(
    FuriString* buffer,
    size_t start_index,
    char* text,
    char delim,
    FuriString* result) {
    size_t len = strlen(text);
    size_t valid_index = furi_string_size(buffer) - 1;
    size_t field = furi_string_search_str(buffer, text, start_index) + len;
    size_t term = -1;
    if(field < valid_index) {
        term = furi_string_search_char(buffer, delim, field);
        if(term < valid_index) {
            furi_string_reset(result);
            furi_string_set_n(result, buffer, field, term - field);
            FURI_LOG_I(TAG, "%s data is >>%s<<", text, furi_string_get_cstr(result));
        } else {
            term = furi_string_size(buffer);
            furi_string_reset(result);
            furi_string_set_n(result, buffer, field, term - field);
            FURI_LOG_E(TAG, "Failed to find terminator for >>%s<<, using end of string", text);
            FURI_LOG_I(TAG, "%s data is >>%s<<", text, furi_string_get_cstr(result));
        }
    } else {
        FURI_LOG_E(TAG, "Failed to find >>%s<<", text);
    }

    return term;
}

uint32_t
    __furi_string_extract_int(FuriString* buffer, char* text, char delim, uint32_t default_value) {
    uint32_t value = default_value;
    size_t len = strlen(text);
    size_t valid_index = furi_string_size(buffer) - 1;
    size_t field = furi_string_search_str(buffer, text, 0) + len;
    size_t term = -1;
    FURI_LOG_I(TAG, "Extracting %s from field %d len is %d", text, field, len);
    if(field < valid_index && len <= field) {
        term = furi_string_search_char(buffer, delim, field);
        if(term < valid_index) {
            FuriString* result = furi_string_alloc();
            furi_string_set_n(result, buffer, field, term - field);
            value = __furi_string_hex_to_uint32(result);
            FURI_LOG_D(TAG, "%s data is >>%s<<", text, furi_string_get_cstr(result));
            furi_string_free(result);
        } else {
            FURI_LOG_E(TAG, "Failed to find terminator for >>%s<<", text);
        }
    } else {
        FURI_LOG_E(TAG, "Failed to find >>%s<<", text);
    }

    return value;
}

uint32_t __furi_string_hex_to_uint32(FuriString* str) {
    uint32_t result = 0;
    for(size_t i = 0; i < furi_string_size(str); i++) {
        char ch = furi_string_get_char(str, i);
        result *= 16;
        if(ch >= '0' && ch <= '9') {
            result += ch - '0';
        } else if(ch >= 'A' && ch <= 'F') {
            result += ch - 'A' + 10;
        } else if(ch >= 'a' && ch <= 'f') {
            result += ch - 'a' + 10;
        } else {
            FURI_LOG_E(TAG, "Invalid hex character %c", ch);
        }
    }

    return result;
}

void __gui_redraw() {
    // Redraw screen
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_direct_draw_acquire(gui);
    gui_direct_draw_release(gui);
}
