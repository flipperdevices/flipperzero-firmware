#include "shapshup_files.h"
#include <furi.h>
#include <inttypes.h>
#include <toolbox/stream/stream.h>
#include <toolbox/path.h>
#include "flipper_format_stream_i.h"

#include <subghz/types.h>

#define TAG "ShapShupFiles"

#define MAX_LINE 500
#define RAND_MAX_VALUE 700
#define RAND_MIN_VALUE 100
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

bool read_int32_shapshup(Stream* stream, int32_t* _data, const uint16_t data_size) {
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

uint32_t rand_range(uint32_t min, uint32_t max) {
    // size of range, inclusive
    const uint32_t length_of_range = max - min + 1;

    // add n so that we don't return a number below our range
    return (uint32_t)(rand() % length_of_range + min);
}

bool load_file_shapshup(Storage* storage, FlipperFormat* fff_data, const char* file_path) {
    furi_assert(storage);
    furi_assert(fff_data);
    furi_assert(file_path);

    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);
    Stream* fff_data_stream = flipper_format_get_raw_stream(fff_data);

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t temp_data32;
    bool result = false;

    do {
        stream_clean(fff_data_stream);
        if(!flipper_format_file_open_existing(fff_data_file, file_path)) {
            FURI_LOG_E(TAG, "Error open file %s", file_path);
            break;
        }

        if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
            FURI_LOG_E(TAG, "Missing or incorrect header");
            break;
        }

        if(((!furi_string_cmp_str(temp_str, SUBGHZ_KEY_FILE_TYPE)) ||
            (!furi_string_cmp_str(temp_str, SUBGHZ_RAW_FILE_TYPE))) &&
           temp_data32 == SUBGHZ_KEY_FILE_VERSION) {
        } else {
            FURI_LOG_E(TAG, "Type or version mismatch");
            break;
        }

        if(!flipper_format_read_uint32(fff_data_file, "Frequency", &temp_data32, 1)) {
            FURI_LOG_E(TAG, "Missing Frequency");
            break;
        }

        if(!flipper_format_read_string(fff_data_file, "Preset", temp_str)) {
            FURI_LOG_E(TAG, "Missing Preset");
            break;
        }

        if(!flipper_format_read_string(fff_data_file, "Protocol", temp_str)) {
            FURI_LOG_E(TAG, "Missing Protocol");
            break;
        }
        stream_copy_full(
            flipper_format_get_raw_stream(fff_data_file), flipper_format_get_raw_stream(fff_data));

        result = true;
    } while(0);

    furi_string_free(temp_str);
    flipper_format_free(fff_data_file);
    //furi_record_close(RECORD_STORAGE);

    return result;
}

bool write_file_noise_shapshup(
    Stream* file,
    bool is_negative_start,
    size_t current_position,
    bool empty_line) {
    size_t was_write = 0;
    if(empty_line) {
        was_write = stream_write_format(file, "%s: ", RAW_KEY_NAME);

        if(was_write <= 0) {
            FURI_LOG_E(TAG, "Can't write key!");
            return false;
        }
    }

    int8_t first;
    int8_t second;
    if(is_negative_start) {
        first = -1;
        second = 1;
    } else {
        first = 1;
        second = -1;
    }
    while(current_position < MAX_LINE) {
        was_write = stream_write_format(
            file,
            "%ld %ld ",
            rand_range(RAND_MIN_VALUE, RAND_MAX_VALUE) * first,
            rand_range(RAND_MIN_VALUE, RAND_MAX_VALUE) * second);

        if(was_write <= 0) {
            FURI_LOG_E(TAG, "Can't write random values!");
            return false;
        }

        current_position += was_write;
    }

    // Step back to write \n instead of space
    size_t offset = stream_tell(file);
    if(stream_seek(file, offset - 1, StreamOffsetFromCurrent)) {
        FURI_LOG_E(TAG, "Step back failed!");
        return false;
    }

    return stream_write_char(file, flipper_format_eoln) > 0;
}

bool write_file_data_shapshup(
    Stream* src,
    Stream* file,
    bool* is_negative_start,
    size_t* current_position) {
    size_t offset_file = 0;
    //    uint8_t buffer[buffer_size];
    bool result = false;
    int32_t value = 0;

    do {
        if(!read_int32_shapshup(src, &value, 1)) {
            result = true;
            break;
        }
        offset_file = stream_tell(file);
        stream_write_format(file, "%ld ", value);
        *current_position += stream_tell(file) - offset_file;

        if(*current_position > MAX_LINE) {
            if((is_negative_start && value > 0) || (!is_negative_start && value < 0)) {
                // Align values
                continue;
            }

            if(stream_write_format(file, "\n%s: ", RAW_KEY_NAME) == 0) {
                FURI_LOG_E(TAG, "Can't write new line!");
                result = false;
                break;
            }
            *current_position = 0;
        }

        //        uint16_t bytes_were_read = stream_read(src, buffer, buffer_size);
        //        if(bytes_were_read == 0) {
        //            break;
        //        }

        //        bool error = false;
        //        int16_t first_positive_position = -1;
        //        int16_t prev_positive_position = -1;
        //        int16_t first_negative_position = -1;
        //        for(uint16_t i = 0; i < bytes_were_read; i++) {
        //            if(buffer[i] == flipper_format_eoln) {
        //                if(!stream_seek(src, i - bytes_were_read + 1, StreamOffsetFromCurrent)) {
        //                    // TODO: Add error
        //                    error = true;
        //                    break;
        //                }
        //                result = true;
        //                break;
        //            } else if(buffer[i] == flipper_format_eolr) {
        //                // Ignore
        //            } else if (first_negative_position == -1 || first_positive_position == -1 || (first_positive_position > 0 && i - first_positive_position == 1)) {
        //                if (buffer[i] == '-') {
        //                    current_negative = true;
        //                    first_negative_position = i;
        //
        //                    if(first_positive_position > 0 && i - first_positive_position == 1) {
        //                        // fix real positive position
        //                        first_positive_position = prev_positive_position;
        //                    }
        //                }
        //                else if(buffer[i] == ' ') {
        //                    current_negative = false;
        //                    prev_positive_position = first_positive_position;
        //                    first_positive_position = i;
        //                }
        //            }
        //        }
        //
        //        if (error) {
        //            break;
        //        }
        //
        //        if(!result && (first_negative_position == -1 || first_positive_position == -1)) {
        //            // TODO: Add error
        //            break;
        //        }
        //
        //        if (result) {
        //            // Need to save current position for later purpose
        //        } else if (current_position + bytes_were_read < MAX_LINE) {
        //            // Simply add bytes to stream
        //            stream_write(file, buffer, bytes_were_read);
        //            current_position += bytes_were_read;
        //        } else {
        //            // We need to check line
        //            size_t size = 0;
        //            bool value_is_correct = false;
        //            if (is_negative_start) {
        //                value_is_correct = first_positive_position < first_negative_position;
        //
        //                // Last is positive number
        //                size = value_is_correct ? first_negative_position : first_positive_position;
        //            } else {
        //                value_is_correct = first_negative_position < first_positive_position;
        //
        //                // Last is negative number
        //                size = value_is_correct ? first_positive_position : first_negative_position;
        //            }
        //
        //            // We are get correct values not in the middle of digit or something like that
        //            if (value_is_correct) {
        //                stream_write(file, buffer, size);
        //            } else {
        //
        //            }
        //            stream_write(file, buffer, size);
        //            // But now we need to understand is it next number is correct
        //            for(uint16_t i = 0; i < bytes_were_read; i++) {
        //
        //            }
        //        }
        //        for(current_position; current_position < MAX_LINE; current_position++) {
        //
        //        }
        //
        //        if(result || error) {
        //            break;
        //        }
    } while(true);

    *is_negative_start = value < 0;

    return result;
}

bool write_file_split_shapshup(
    Storage* storage,
    FlipperFormat* flipper_string,
    const char* dir_path) {
#ifdef FURI_DEBUG
    FURI_LOG_W(TAG, "Save temp file splitted: %s", dir_path);
#endif
    uint8_t buffer[buffer_size];
    Stream* src = flipper_format_get_raw_stream(flipper_string);
    stream_rewind(src);

    FlipperFormat* flipper_format_file = flipper_format_file_alloc(storage);
    bool result = false;
    FuriString* temp_str = furi_string_alloc();

    do {
        if(storage_file_exists(storage, dir_path) &&
           storage_common_remove(storage, dir_path) != FSE_OK) {
            FURI_LOG_E(TAG, "Can't delete old file!");
            break;
        }
        path_extract_dirname(dir_path, temp_str);
        FS_Error fs_result = storage_common_mkdir(storage, furi_string_get_cstr(temp_str));
        if(fs_result != FSE_OK && fs_result != FSE_EXIST) {
            FURI_LOG_E(TAG, "Can't create dir!");
            break;
        }
        result = flipper_format_file_open_always(flipper_format_file, dir_path);
        if(!result) {
            FURI_LOG_E(TAG, "Can't open file for write!");
            break;
        }
        Stream* file = flipper_format_get_raw_stream(flipper_format_file);

        if(!stream_seek_to_key_shapshup(src, RAW_KEY_NAME, false)) {
            FURI_LOG_E(TAG, "Can't find key!");
            break;
        }
        bool is_negative_start = false;
        //        bool is_negative_end = false;
        bool found = false;

        size_t offset_start;
        offset_start = stream_tell(src);

        // Check for negative value at the start and end to align file by correct values
        size_t was_read = stream_read(src, buffer, 1);
        if(was_read <= 0) {
            FURI_LOG_E(TAG, "Can't obtain first mark!");
            break;
        }

        is_negative_start = buffer[0] == '-';
        // Goto end
        //        stream_seek(src, 32, StreamOffsetFromEnd);
        //        was_read = stream_read(src, buffer, buffer[buffer_size]);
        //
        //        if(was_read <= 0) {
        //            FURI_LOG_E(TAG, "Can't obtain end mark!");
        //            break;
        //        }
        //        // Seek for last value
        //        for(size_t i = 0; i < was_read; i++) {
        //            if(buffer[i] == flipper_format_eoln || buffer[i] == flipper_format_eolr) {
        //                // End of line
        //                break;
        //            }
        //            if(buffer[i] == ' ') {
        //                is_negative_end = false;
        //            } else if(buffer[i] == '-') {
        //                is_negative_end = true
        //            } else {
        //                // Other values is digits
        //            }
        //        }

        // Ready to write stream to file
        size_t current_position;
        stream_rewind(src);
        current_position = stream_copy(src, file, offset_start);
        if(current_position != offset_start) {
            FURI_LOG_E(TAG, "Invalid copy header data from one stream to another!");
            break;
        }

        found = true;

        current_position = 0;
        if(!write_file_noise_shapshup(file, is_negative_start, current_position, false)) {
            FURI_LOG_E(TAG, "Add start noise failed!");
            break;
        }

        if(stream_write_format(file, "%s: ", RAW_KEY_NAME) == 0) {
            FURI_LOG_E(TAG, "Can't write new line!");
            result = false;
            break;
        }

        if(!write_file_data_shapshup(src, file, &is_negative_start, &current_position)) {
            FURI_LOG_E(TAG, "Split by lines failed!");
            break;
        }

        if(!write_file_noise_shapshup(file, is_negative_start, current_position, false)) {
            FURI_LOG_E(TAG, "Add end noise failed!");
            break;
        }

        if(!write_file_noise_shapshup(file, is_negative_start, 0, true)) {
            FURI_LOG_E(TAG, "Add end noise failed!");
            break;
        }

        result = found;
    } while(false);
    flipper_format_file_close(flipper_format_file);
    flipper_format_free(flipper_format_file);
    furi_string_free(temp_str);

    return result;
}
