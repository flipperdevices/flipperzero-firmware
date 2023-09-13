#include "font_provider.h"

#include <inttypes.h>
#include <toolbox/dir_walk.h>
#include <toolbox/path.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

#define FONT_BASE_PATH EXT_PATH("apps_assets/totp/fonts")
#define FONT_FILE_EXTENSION ".font"

size_t totp_font_provider_get_fonts_count() {
    size_t result = 0;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FuriString* path_src = furi_string_alloc();

    DirWalk* dir_walk = dir_walk_alloc(storage);
    dir_walk_set_recursive(dir_walk, false);

    if(dir_walk_open(dir_walk, FONT_BASE_PATH)) {
        char extension[sizeof(FONT_FILE_EXTENSION)];
        while(dir_walk_read(dir_walk, path_src, NULL) == DirWalkOK) {
            path_extract_extension(path_src, &extension[0], sizeof(extension));
            if(strncmp(&extension[0], FONT_FILE_EXTENSION, sizeof(FONT_FILE_EXTENSION)) == 0) {
                result++;
            }
        }
    }

    furi_string_free(path_src);
    dir_walk_free(dir_walk);

    furi_record_close(RECORD_STORAGE);

    return result;
}

bool totp_font_provider_get_font(size_t font_index, FontInfo* font_info) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);
    bool loaded = false;
    FuriString* font_path = furi_string_alloc_printf(
        "%s/%02" PRIu16 "%s", FONT_BASE_PATH, font_index, FONT_FILE_EXTENSION);

    do {
        if(!file_stream_open(
               stream, furi_string_get_cstr(font_path), FSAM_READ, FSOM_OPEN_EXISTING) ||
           !stream_rewind(stream)) {
            break;
        }
        uint8_t font_name_length;
        if(!stream_read(stream, &font_name_length, 1)) {
            break;
        }

        if(font_info->name != NULL) {
            free(font_info->name);
        }
        font_info->name = malloc(font_name_length + 1);
        furi_check(font_info->name);
        if(!stream_read(stream, (uint8_t*)font_info->name, font_name_length)) {
            break;
        }

        font_info->name[font_name_length] = '\0';
        if(!stream_read(stream, &font_info->height, 1) ||
           !stream_read(stream, &font_info->start_char, 1) ||
           !stream_read(stream, &font_info->end_char, 1) ||
           !stream_read(stream, &font_info->space_width, 1)) {
            break;
        }

        uint16_t bitmap_data_length;
        if(!stream_read(stream, (uint8_t*)&bitmap_data_length, 2)) {
            break;
        }

        if(font_info->data != NULL) {
            free(font_info->data);
        }

        font_info->data = malloc(bitmap_data_length);

        furi_check(font_info->data);

        if(!stream_read(stream, font_info->data, bitmap_data_length)) {
            break;
        }
        uint8_t descriptors_length;
        if(!stream_read(stream, &descriptors_length, 1)) {
            break;
        }

        if(font_info->char_info != NULL) {
            free(font_info->char_info);
        }
        uint16_t char_info_array_size = descriptors_length * sizeof(FontCharInfo);
        font_info->char_info = malloc(char_info_array_size);
        furi_check(font_info->char_info);
        if(!stream_read(stream, (uint8_t*)font_info->char_info, char_info_array_size)) {
            break;
        }

        loaded = true;
    } while(false);

    furi_string_free(font_path);
    file_stream_close(stream);
    stream_free(stream);

    furi_record_close(RECORD_STORAGE);
    return loaded;
}