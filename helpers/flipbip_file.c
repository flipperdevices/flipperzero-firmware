#include "flipbip_file.h"

#include "../flipbip.h"

#include <storage/storage.h>
#include <lib/flipper_format/flipper_format.h>

bool flipbip_load_file(const char* file_path) {
    furi_assert(file_path);

    bool result = false;
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    do {
        if(!flipper_format_file_open_existing(file, file_path)) break;

        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, "FlipBIP") ||
           (version != 1)) {
            break;
        }

        if(!flipper_format_read_string(file, "X", temp_str)) {
            break;
        }

        result = true;
    } while(false);

    furi_record_close(RECORD_STORAGE);
    flipper_format_free(file);
    furi_string_free(temp_str);

    return result;
}

bool flipbip_save_file(const char* file_path) {
    furi_assert(file_path);

    bool result = false;
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    do {
        if(!flipper_format_file_open_existing(file, file_path) ||
           !flipper_format_file_open_new(file, file_path)) break;

        //uint32_t version = 1;
        //temp_str = "FlipBIP";
        if(!flipper_format_write_header_cstr(file, "FlipBIP", 1)) break;

        //temp_str = "12345abcde";
        if(!flipper_format_write_string_cstr(file, "X", "12345abcde")) {
            break;
        }

        result = true;
    } while(false);

    furi_record_close(RECORD_STORAGE);
    flipper_format_free(file);
    furi_string_free(temp_str);

    return result;
}