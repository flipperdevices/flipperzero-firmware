#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
//#include "../../../lib/toolbox/path.h"
//#include "../../../lib/toolbox/stream/stream.h"
//#include "../../../furi/core/string.h"
//#include "../../../furi/furi.h"
//#include "../../../lib/flipper_format/flipper_format_stream_i.h"
//#include "../../../applications/services/storage/storage.h"
//#include "../../../furi/core/check.h"
//#include "../../../furi/core/record.h"
//#include "../../../lib/flipper_format/flipper_format.h"
//#include "../../../furi/core/log.h"
//#include "../../../lib/flipper_format/flipper_format_i.h"
//#include "../../../lib/subghz/types.h"
#include "shapshup_files.h"

#define TAG "ShapShupFiles"
#define RAW_KEY_NAME "RAW_Data"

const size_t buffer_size = 32;

static bool stream_read_valid_key_shapshup(Stream* stream, FuriString* key) {
    furi_string_reset(key);
    uint8_t buffer[buffer_size];

    bool found = false;
    bool error = false;
    bool accumulate = true;
    bool new_line = true;

    while(true) {
        size_t was_read = stream_read(stream, buffer, buffer_size);
        if(was_read == 0) break;

        for(size_t i = 0; i < was_read; i++) {
            uint8_t data = buffer[i];
            if(data == flipper_format_eoln) {
                // EOL found, clean data, start accumulating data and set the new_line flag
                furi_string_reset(key);
                accumulate = true;
                new_line = true;
            } else if(data == flipper_format_eolr) {
                // ignore
            } else if(data == flipper_format_comment && new_line) {
                // if there is a comment character and we are at the beginning of a new line
                // do not accumulate comment data and reset the new_line flag
                accumulate = false;
                new_line = false;
            } else if(data == flipper_format_delimiter) {
                if(new_line) {
                    // we are on a "new line" and found the delimiter
                    // this can only be if we have previously found some kind of key, so
                    // clear the data, set the flag that we no longer want to accumulate data
                    // and reset the new_line flag
                    furi_string_reset(key);
                    accumulate = false;
                    new_line = false;
                } else {
                    // parse the delimiter only if we are accumulating data
                    if(accumulate) {
                        // we found the delimiter, move the rw pointer to the delimiter location
                        // and signal that we have found something
                        if(!stream_seek(stream, i - was_read, StreamOffsetFromCurrent)) {
                            error = true;
                            break;
                        }

                        found = true;
                        break;
                    }
                }
            } else {
                // just new symbol, reset the new_line flag
                new_line = false;
                if(accumulate) {
                    // and accumulate data if we want
                    furi_string_push_back(key, data);
                }
            }
        }

        if(found || error) break;
    }

    return found;
}

bool stream_seek_to_key_shapshup(Stream* stream, const char* key, bool strict_mode) {
    bool found = false;
    FuriString* read_key;

    read_key = furi_string_alloc();

    while(!stream_eof(stream)) {
        if(stream_read_valid_key_shapshup(stream, read_key)) {
            if(furi_string_cmp_str(read_key, key) == 0) {
                if(!stream_seek(stream, 2, StreamOffsetFromCurrent)) {
                    break;
                }
                found = true;
                break;
            } else if(strict_mode) {
                found = false;
                break;
            }
        }
    }
    furi_string_free(read_key);

    return found;
}

static inline bool is_space_shapshup(char c) {
    return c == ' ' || c == '\t' || c == flipper_format_eolr;
}

static bool stream_read_value_shapshup(Stream* stream, FuriString* value, bool* last) {
    enum { LeadingSpace, ReadValue, TrailingSpace } state = LeadingSpace;
    const size_t buffer_size = 32;
    uint8_t buffer[buffer_size];
    bool result = false;
    bool error = false;

    furi_string_reset(value);

    while(true) {
        size_t was_read = stream_read(stream, buffer, buffer_size);

        if(was_read == 0) {
            if(state != LeadingSpace && stream_eof(stream)) {
                result = true;
                *last = true;
            } else {
                error = true;
            }
        }

        for(uint16_t i = 0; i < was_read; i++) {
            const uint8_t data = buffer[i];

            if(state == LeadingSpace) {
                if(is_space_shapshup(data)) {
                    continue;
                } else if(data == flipper_format_eoln) {
                    stream_seek(stream, i - was_read, StreamOffsetFromCurrent);
                    error = true;
                    break;
                } else {
                    state = ReadValue;
                    furi_string_push_back(value, data);
                }
            } else if(state == ReadValue) {
                if(is_space_shapshup(data)) {
                    state = TrailingSpace;
                } else if(data == flipper_format_eoln) {
                    if(!stream_seek(stream, i - was_read, StreamOffsetFromCurrent)) {
                        error = true;
                    } else {
                        result = true;
                        *last = true;
                    }
                    break;
                } else {
                    furi_string_push_back(value, data);
                }
            } else if(state == TrailingSpace) {
                if(is_space_shapshup(data)) {
                    continue;
                } else if(!stream_seek(stream, i - was_read, StreamOffsetFromCurrent)) {
                    error = true;
                } else {
                    *last = (data == flipper_format_eoln);
                    result = true;
                }
                break;
            }
        }

        if(error || result) break;
    }

    return result;
}

bool read_int32_shapshup(Stream* stream, int32_t* _data, uint16_t data_size) {
    bool result = false;
    result = true;
    FuriString* value;
    value = furi_string_alloc();

    for(size_t i = 0; i < data_size; i++) {
        bool last = false;
        result = stream_read_value_shapshup(stream, value, &last);
        if(result) {
            int scan_values = 0;

            int32_t* data = _data;
            scan_values = sscanf(furi_string_get_cstr(value), "%" PRIi32, &data[i]);

            if(scan_values != 1) {
                result = false;
                break;
            }
        } else {
            break;
        }

        if(last && ((i + 1) != data_size)) {
            result = false;
            break;
        }
    }

    furi_string_free(value);
    return result;
}

ShapShupRawFile* load_file_shapshup(const char* file_path) {
    furi_assert(file_path);

    Storage* storage = furi_record_open(RECORD_STORAGE);

    ShapShupRawFile* instance = malloc(sizeof(ShapShupRawFile));
    instance->total_len = 0;
    instance->total_count = 0;
    instance->min_value = 0;
    instance->max_value = 0;
    instance->max_len = 0;
    instance->min_len = 0;
    array_raw_init(instance->values);

    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);
    Stream* fff_data_stream = NULL;

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t temp_data32;
    instance->result = ShapShupFileResultUnknown;

    do {
        if(!flipper_format_file_open_existing(fff_data_file, file_path)) {
            instance->result = ShapShupFileResultOpenError;
            FURI_LOG_E(TAG, shapshup_files_result_description(instance->result));
            break;
        }

        fff_data_stream = flipper_format_get_raw_stream(fff_data_file);

        if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
            instance->result = ShapShupFileResultIncorrectHeader;
            FURI_LOG_E(TAG, shapshup_files_result_description(instance->result));
            break;
        }

        if(temp_data32 != SUBGHZ_KEY_FILE_VERSION) {
            instance->result = ShapShupFileResultTypeOfVersionMismatch;
            FURI_LOG_E(TAG, shapshup_files_result_description(instance->result));
            break;
        }

        if(furi_string_cmp_str(temp_str, SUBGHZ_RAW_FILE_TYPE) != 0) {
            instance->result = ShapShupFileResultNotRawFile;
            FURI_LOG_E(
                TAG,
                "%s, Value: %s",
                shapshup_files_result_description(instance->result),
                furi_string_get_cstr(temp_str));
            break;
        }

        if(!flipper_format_read_uint32(fff_data_file, "Frequency", &temp_data32, 1)) {
            instance->result = ShapShupFileResultMissingFrequency;
            FURI_LOG_E(TAG, shapshup_files_result_description(instance->result));
            break;
        } else {
            instance->frequency = temp_data32;
        }

        if(!flipper_format_read_string(fff_data_file, "Preset", temp_str)) {
            instance->result = ShapShupFileResultMissingPreset;
            FURI_LOG_E(TAG, shapshup_files_result_description(instance->result));
            break;
        }

        if(!flipper_format_read_string(fff_data_file, "Protocol", temp_str)) {
            instance->result = ShapShupFileResultMissingProtocol;
            FURI_LOG_E(TAG, shapshup_files_result_description(instance->result));
            break;
        }

        if(!stream_seek_to_key_shapshup(fff_data_stream, RAW_KEY_NAME, false)) {
            instance->result = ShapShupFileResultKeyNotFound;
            FURI_LOG_E(TAG, shapshup_files_result_description(instance->result));
            break;
        }
        // file_offset_start = stream_tell(fff_data_stream);
        instance->result = ShapShupFileResultOk;
    } while(false);

    uint64_t min_len = 0 - 1;
    uint64_t max_len = 0;

    if(instance->result == ShapShupFileResultOk) {
        int32_t value = 0;
        do {
            if(!read_int32_shapshup(fff_data_stream, &value, 1)) {
                break;
            }

            uint64_t abs_value = value < 0 ? value * -1 : value;

            if(value < instance->min_value) {
                instance->min_value = value;
            } else if(value > instance->max_value) {
                instance->max_value = value;
            }
            if(abs_value > max_len) {
                max_len = abs_value;
            } else if(abs_value < min_len) {
                min_len = abs_value;
            }
            array_raw_push_back(instance->values, value);
            instance->total_count++;
            instance->total_len += abs_value;
        } while(true);
        instance->max_len = max_len;
        instance->min_len = min_len;
    } else {
        array_raw_clear(instance->values);
    }

    FURI_LOG_I(
        TAG, "total_count: %lld, total_len: %lld", instance->total_count, instance->total_len);

    furi_string_free(temp_str);
    flipper_format_file_close(fff_data_file);
    flipper_format_free(fff_data_file);

    furi_record_close(RECORD_STORAGE);

    return instance;
}

void clean_raw_values(ShapShupRawFile* raw_file) {
    if(raw_file != NULL) {
        array_raw_clear(raw_file->values);
        free(raw_file);
        raw_file = NULL;
    }
}

static const char* shapshup_file_result_descriptions[] = {
    [ShapShupFileResultOk] = "OK",
    [ShapShupFileResultOpenError] = "Error open file",
    [ShapShupFileResultIncorrectHeader] = "Missing or incorrect header",
    [ShapShupFileResultTypeOfVersionMismatch] = "Type or version mismatch",
    [ShapShupFileResultNotRawFile] = "Not RAW file",
    [ShapShupFileResultMissingFrequency] = "Missing Frequency",
    [ShapShupFileResultMissingPreset] = "Missing Preset",
    [ShapShupFileResultMissingProtocol] = "Missing Protocol",
    [ShapShupFileResultKeyNotFound] = "Key not found",
    [ShapShupFileResultUnknown] = "Unknown error",
};

const char* shapshup_files_result_description(ShapShupFileResults index) {
    return shapshup_file_result_descriptions[index];
}
