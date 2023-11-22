#include "kb_layout_provider.h"

#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

#define KB_LAYOUTS_FILE EXT_PATH("apps_assets/totp/kb_layouts.klx")

bool totp_kb_layout_provider_get_layout_data(AutomationKeyboardLayout kb_layout, uint16_t* buffer) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);

    bool result = false;
    do {
        if(!file_stream_open(stream, KB_LAYOUTS_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
            break;
        }

        if(!stream_seek(
               stream,
               sizeof(uint8_t) + kb_layout * (TOTP_KB_LAYOUT_NAME_MAX_LENGTH + sizeof(uint16_t)) +
                   TOTP_KB_LAYOUT_NAME_MAX_LENGTH,
               StreamOffsetFromStart)) {
            break;
        }

        uint16_t offset;
        if(stream_read(stream, (uint8_t*)&offset, sizeof(uint16_t)) != sizeof(uint16_t)) {
            break;
        }

        if(!stream_seek(stream, offset, StreamOffsetFromStart)) {
            break;
        }

        size_t bytes_to_read = TOTP_KB_LAYOUT_DATA_LENGTH * sizeof(uint16_t);
        if(stream_read(stream, (uint8_t*)buffer, bytes_to_read) != bytes_to_read) {
            break;
        }

        result = true;
    } while(false);

    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return result;
}

uint8_t totp_kb_layout_provider_get_layouts_count() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);

    uint8_t result = 0;
    do {
        if(!file_stream_open(stream, KB_LAYOUTS_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
            break;
        }

        if(!stream_rewind(stream)) {
            break;
        }

        if(stream_read(stream, &result, 1) != 1) {
            break;
        }
    } while(false);

    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return result;
}

bool totp_kb_layout_provider_get_layout_name(
    AutomationKeyboardLayout kb_layout,
    char* buffer,
    size_t buffer_length) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);

    bool result = false;
    do {
        if(!file_stream_open(stream, KB_LAYOUTS_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
            break;
        }

        if(!stream_seek(
               stream,
               sizeof(uint8_t) + kb_layout * (TOTP_KB_LAYOUT_NAME_MAX_LENGTH + sizeof(uint16_t)),
               StreamOffsetFromStart)) {
            break;
        }

        size_t bytes_to_read = M_MIN(TOTP_KB_LAYOUT_NAME_MAX_LENGTH, buffer_length);
        if(stream_read(stream, (uint8_t*)buffer, bytes_to_read) != bytes_to_read) {
            break;
        }

        buffer[buffer_length - 1] = '\0';

        result = true;
    } while(false);

    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return result;
}

bool totp_kb_layout_provider_get_layout_by_name(
    const char* name,
    AutomationKeyboardLayout* kb_layout) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);

    uint8_t count = 0;
    bool found = false;
    do {
        if(!file_stream_open(stream, KB_LAYOUTS_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
            break;
        }

        if(!stream_rewind(stream)) {
            break;
        }

        if(stream_read(stream, &count, 1) != 1) {
            break;
        }

        char current_name[TOTP_KB_LAYOUT_NAME_MAX_LENGTH + 1];
        for(AutomationKeyboardLayout i = 0; i < count; i++) {
            if(stream_read(stream, (uint8_t*)&current_name[0], TOTP_KB_LAYOUT_NAME_MAX_LENGTH) !=
               TOTP_KB_LAYOUT_NAME_MAX_LENGTH) {
                break;
            }

            current_name[TOTP_KB_LAYOUT_NAME_MAX_LENGTH] = '\0';
            if(strncasecmp(name, &current_name[0], TOTP_KB_LAYOUT_NAME_MAX_LENGTH) == 0) {
                found = true;
                *kb_layout = i;
                break;
            }

            if(!stream_seek(stream, sizeof(uint16_t), StreamOffsetFromCurrent)) {
                break;
            }
        }
    } while(false);

    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return found;
}
