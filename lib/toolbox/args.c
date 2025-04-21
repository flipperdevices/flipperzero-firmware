#include "args.h"
#include "hex.h"
#include "strint.h"
#include "m-core.h"

size_t args_get_first_word_length(FuriString* args) {
    size_t ws = furi_string_search_char(args, ' ');
    if(ws == FURI_STRING_FAILURE) {
        ws = furi_string_size(args);
    }

    return ws;
}

size_t args_length(FuriString* args) {
    return furi_string_size(args);
}

bool args_read_int_and_trim(FuriString* args, int* value) {
    size_t cmd_length = args_get_first_word_length(args);

    if(cmd_length == 0) {
        return false;
    }

    int32_t temp;
    if(strint_to_int32(furi_string_get_cstr(args), NULL, &temp, 10) == StrintParseNoError) {
        *value = temp;
        furi_string_right(args, cmd_length);
        furi_string_trim(args);
        return true;
    }

    return false;
}

bool args_read_float_and_trim(FuriString* args, float* value) {
    size_t cmd_length = args_get_first_word_length(args);
    if(cmd_length == 0) {
        return false;
    }

    char* end_ptr;
    float temp = strtof(furi_string_get_cstr(args), &end_ptr);
    if(end_ptr == furi_string_get_cstr(args)) {
        return false;
    }

    *value = temp;
    furi_string_right(args, cmd_length);
    furi_string_trim(args);
    return true;
}

bool args_read_string_and_trim(FuriString* args, FuriString* word) {
    size_t cmd_length = args_get_first_word_length(args);

    if(cmd_length == 0) {
        return false;
    }

    furi_string_set_n(word, args, 0, cmd_length);
    furi_string_right(args, cmd_length);
    furi_string_trim(args);

    return true;
}

bool args_read_probably_quoted_string_and_trim(FuriString* args, FuriString* word) {
    if(furi_string_size(args) > 1 && furi_string_get_char(args, 0) == '\"') {
        size_t second_quote_pos = furi_string_search_char(args, '\"', 1);

        if(second_quote_pos == 0) {
            return false;
        }

        furi_string_set_n(word, args, 1, second_quote_pos - 1);
        furi_string_right(args, second_quote_pos + 1);
        furi_string_trim(args);
        return true;
    } else {
        return args_read_string_and_trim(args, word);
    }
}

bool args_char_to_hex(char hi_nibble, char low_nibble, uint8_t* byte) {
    uint8_t hi_nibble_value = 0;
    uint8_t low_nibble_value = 0;
    bool result = false;

    if(hex_char_to_hex_nibble(hi_nibble, &hi_nibble_value)) {
        if(hex_char_to_hex_nibble(low_nibble, &low_nibble_value)) {
            result = true;
            *byte = (hi_nibble_value << 4) | low_nibble_value;
        }
    }

    return result;
}

bool args_read_hex_bytes(FuriString* args, uint8_t* bytes, size_t bytes_count) {
    bool result = true;
    const char* str_pointer = furi_string_get_cstr(args);

    if(args_get_first_word_length(args) == (bytes_count * 2)) {
        for(size_t i = 0; i < bytes_count; i++) {
            if(!args_char_to_hex(str_pointer[i * 2], str_pointer[i * 2 + 1], &(bytes[i]))) {
                result = false;
                break;
            }
        }
    } else {
        result = false;
    }

    return result;
}

bool args_read_duration(FuriString* args, uint32_t* value, const char* default_unit) {
    FuriString* unit_string = args;

    // Determine the last character in the string to identify the unit
    size_t last_symbol_index = furi_string_size(unit_string) - 1;
    char last_symbol = furi_string_get_char(unit_string, last_symbol_index);
    if(last_symbol != 's' && last_symbol != 'm' && last_symbol != 'h') {
        if(last_symbol < '0' || '9' < last_symbol) {
            return false;
        }

        if(default_unit == NULL) {
            // Use default value (milliseconds)
            last_symbol = '\0';
        } else {
            // Extract from default unit
            unit_string = furi_string_alloc_set_str(default_unit);
            last_symbol_index = furi_string_size(unit_string) - 1;
            last_symbol = furi_string_get_char(unit_string, last_symbol_index);
        }
    }

    uint32_t multiplier = 1;
    size_t suffix_length = 1;

    // Determine the multiplier based on the unit
    switch(last_symbol) {
    case '\0':
        suffix_length = 0;
        break;
    case 's':
        if(last_symbol_index > 0 &&
           furi_string_get_char(unit_string, last_symbol_index - 1) == 'm') {
            // If it's milliseconds ("ms") rather than seconds
            multiplier = 1;
            suffix_length = 2;
        } else {
            multiplier = 1000; // Seconds to milliseconds
        }
        break;
    case 'm':
        multiplier = 60000; // Minutes to milliseconds
        break;
    case 'h':
        multiplier = 3600000; // Hours to milliseconds
        break;
    }

    size_t value_end_index = furi_string_size(args);

    if(unit_string == args) {
        value_end_index -= suffix_length;
    } else {
        furi_string_free(unit_string);
    }

    // Extract value
    const char* start_ptr = furi_string_get_cstr(args);
    char* end_ptr = NULL;
    double temp_value = strtod(start_ptr, &end_ptr);

    const uint32_t max_value = UINT32_MAX / multiplier;
    bool is_success = start_ptr + value_end_index == end_ptr && start_ptr != end_ptr &&
                      0 <= temp_value && temp_value <= max_value;

    *value = is_success ? round(temp_value * multiplier) : 0;

    return is_success;
}
