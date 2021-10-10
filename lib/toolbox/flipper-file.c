#include <furi.h>
#include "flipper-file.h"
#include <toolbox/hex.h>
#include <inttypes.h>

struct FlipperFile {
    File* file;
    Storage* storage;
};

const char* flipper_file_filetype_key = "Filetype";
const char* flipper_file_version_key = "Version";
const char flipper_file_eoln = '\n';
const char flipper_file_eolr = '\r';
const char flipper_file_delimiter = ':';
const char flipper_file_comment = '#';
const char* flipper_file_scratchpad = ".scratch.pad";

/**
 * Negative seek helper
 * @param file 
 * @param offset 
 * @return bool 
 */
bool flipper_file_seek(File* file, int32_t offset) {
    uint64_t position = storage_file_tell(file);
    return storage_file_seek(file, position + offset, true);
}

/**
 * Writes data to a file as a hexadecimal array.
 * @param file 
 * @param data 
 * @param data_size 
 * @return true on success write 
 */
bool flipper_file_write_hex_internal(File* file, const uint8_t* data, const uint16_t data_size) {
    const uint8_t byte_text_size = 3;
    char byte_text[byte_text_size];

    bool result = true;
    uint16_t bytes_written;
    for(uint8_t i = 0; i < data_size; i++) {
        snprintf(byte_text, byte_text_size, "%02X", data[i]);

        if(i != 0) {
            // space
            const char space = ' ';
            bytes_written = storage_file_write(file, &space, sizeof(space));
            if(bytes_written != sizeof(space)) {
                result = false;
                break;
            }
        }

        bytes_written = storage_file_write(file, &byte_text, strlen(byte_text));
        if(bytes_written != strlen(byte_text)) {
            result = false;
            break;
        }
    }

    return result;
}

/**
 * Reads a valid key from a file as a string.
 * After reading, the rw pointer will be on the flipper_file_delimiter symbol.
 * Optimized not to read comments and values into RAM.
 * @param file 
 * @param key 
 * @return true on success read 
 */
bool flipper_file_read_valid_key(File* file, string_t key) {
    string_clean(key);
    bool found = false;
    bool error = false;
    const uint8_t buffer_size = 32;
    uint8_t buffer[buffer_size];
    bool accumulate = true;
    bool new_line = true;

    while(true) {
        uint16_t bytes_were_read = storage_file_read(file, buffer, buffer_size);
        if(bytes_were_read == 0) break;

        for(uint16_t i = 0; i < bytes_were_read; i++) {
            if(buffer[i] == flipper_file_eoln) {
                // EOL found, clean data, start accumulating data and set the new_line flag
                string_clean(key);
                accumulate = true;
                new_line = true;
            } else if(buffer[i] == flipper_file_eolr) {
                // Ignore
            } else if(buffer[i] == flipper_file_comment && new_line) {
                // if there is a comment character and we are at the beginning of a new line
                // do not accumulate comment data and reset the new_line flag
                accumulate = false;
                new_line = false;
            } else if(buffer[i] == flipper_file_delimiter) {
                if(new_line) {
                    // we are on a "new line" and found the delimiter
                    // this can only be if we have previously found some kind of key, so
                    // clear the data, set the flag that we no longer want to accumulate data
                    // and reset the new_line flag
                    string_clean(key);
                    accumulate = false;
                    new_line = false;
                } else {
                    // parse the delimiter only if we are accumulating data
                    if(accumulate) {
                        // we found the delimiter, move the rw pointer to the correct location
                        // and signal that we have found something
                        if(!flipper_file_seek(file, i - bytes_were_read)) {
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
                    string_push_back(key, buffer[i]);
                }
            }
        }

        if(found || error) break;
    }

    return found;
}

/**
 * Sets rw pointer to the data after the key
 * @param file 
 * @param key 
 * @return true if key was found 
 */
bool flipper_file_seek_to_key(File* file, const char* key) {
    bool found = false;
    string_t readed_key;

    string_init(readed_key);

    while(!storage_file_eof(file)) {
        if(flipper_file_read_valid_key(file, readed_key)) {
            if(string_cmp_str(readed_key, key) == 0) {
                if(!flipper_file_seek(file, 2)) break;

                found = true;
                break;
            }
        }
    }
    string_clear(readed_key);

    return found;
}

/**
 * Reads data as a string from the stored rw pointer to the \r or \\n symbol position
 * @param file 
 * @param str_result 
 * @return true on success read
 */
bool flipper_file_read_until(File* file, string_t str_result) {
    string_clean(str_result);
    const uint8_t buffer_size = 32;
    uint8_t buffer[buffer_size];

    do {
        uint16_t bytes_were_read = storage_file_read(file, buffer, buffer_size);
        // TODO process EOF
        if(bytes_were_read == 0) break;

        bool result = false;
        bool error = false;
        for(uint16_t i = 0; i < bytes_were_read; i++) {
            if(buffer[i] == flipper_file_eoln) {
                if(!flipper_file_seek(file, i - bytes_were_read)) {
                    error = true;
                    break;
                }

                result = true;
                break;
            } else if(buffer[i] == flipper_file_eolr) {
                // Ignore
            } else {
                string_push_back(str_result, buffer[i]);
            }
        }

        if(result || error) {
            break;
        }
    } while(true);

    return string_size(str_result) != 0;
}

bool flipper_file_seek_to_next_line(File* file) {
    const uint8_t buffer_size = 32;
    uint8_t buffer[buffer_size];
    bool result = false;
    bool error = false;

    do {
        uint16_t bytes_were_read = storage_file_read(file, buffer, buffer_size);
        if(bytes_were_read == 0) {
            if(storage_file_eof(file)) {
                result = true;
                break;
            }
        }

        for(uint16_t i = 0; i < bytes_were_read; i++) {
            if(buffer[i] == flipper_file_eoln) {
                if(!flipper_file_seek(file, i - bytes_were_read)) {
                    error = true;
                    break;
                }

                result = true;
                break;
            }
        }

        if(result || error) {
            break;
        }
    } while(true);

    return result;
}

/**
 * Read one value from array-like string (separated by ' ')
 * @param file 
 * @param value 
 * @return bool 
 */
bool flipper_file_read_one_value(File* file, string_t value, bool* last) {
    string_clean(value);
    const uint8_t buffer_size = 32;
    uint8_t buffer[buffer_size];
    bool result = false;
    bool error = false;

    while(true) {
        uint16_t bytes_were_read = storage_file_read(file, buffer, buffer_size);

        if(bytes_were_read == 0) {
            // check EOF
            if(storage_file_eof(file) && string_size(value) > 0) {
                result = true;
                *last = true;
                break;
            }
        }

        for(uint16_t i = 0; i < bytes_were_read; i++) {
            if(buffer[i] == flipper_file_eoln || buffer[i] == flipper_file_eolr) {
                if(string_size(value) > 0) {
                    if(!flipper_file_seek(file, i - bytes_were_read)) {
                        error = true;
                        break;
                    }

                    result = true;
                    *last = true;
                    break;
                } else {
                    error = true;
                }
            } else if(buffer[i] == ' ') {
                if(string_size(value) > 0) {
                    if(!flipper_file_seek(file, i - bytes_were_read)) {
                        error = true;
                        break;
                    }

                    result = true;
                    *last = false;
                    break;
                }
            } else {
                string_push_back(value, buffer[i]);
            }
        }

        if(error || result) break;
    }

    return result;
}

/**
 * Write helper
 * @param file 
 * @param data 
 * @param data_size 
 * @return bool 
 */
bool flipper_file_write(File* file, const void* data, uint16_t data_size) {
    uint16_t bytes_written = storage_file_write(file, data, data_size);
    return bytes_written == data_size;
}

/**
 * Write key and key delimiter
 * @param file 
 * @param key 
 * @return bool 
 */
bool flipper_file_write_key(File* file, const char* key) {
    bool result = false;

    do {
        result = flipper_file_write(file, key, strlen(key));
        if(!result) break;

        const char delimiter_buffer[2] = {flipper_file_delimiter, ' '};
        result = flipper_file_write(file, delimiter_buffer, sizeof(delimiter_buffer));
    } while(false);

    return result;
}

/**
 * Write EOL
 * @param file 
 * @return bool 
 */
bool flipper_file_write_eol(File* file) {
    return flipper_file_write(file, &flipper_file_eoln, sizeof(flipper_file_eoln));
}

bool flipper_file_get_scratchpad_name(const char** name) {
    // TODO do not rewrite existing file
    *name = flipper_file_scratchpad;
    return true;
}

bool flipper_file_copy(File* file_from, File* file_to, uint64_t start_offset, uint64_t stop_offset) {
    bool result = false;

    const uint8_t buffer_size = 32;
    uint8_t buffer[buffer_size];
    uint64_t current_offset = start_offset;

    if(storage_file_seek(file_from, start_offset, true)) {
        do {
            int32_t bytes_count = MIN(buffer_size, stop_offset - current_offset);
            if(bytes_count <= 0) {
                result = true;
                break;
            }

            uint16_t bytes_were_read = storage_file_read(file_from, buffer, bytes_count);
            if(bytes_were_read != bytes_count) break;

            uint16_t bytes_were_written = storage_file_write(file_to, buffer, bytes_count);
            if(bytes_were_written != bytes_count) break;

            current_offset += bytes_count;
        } while(true);
    }

    return result;
}

/***********************************************************************************************/

FlipperFile* flipper_file_alloc(Storage* storage) {
    //furi_assert(storage);

    FlipperFile* flipper_file = malloc(sizeof(FlipperFile));
    flipper_file->storage = storage;
    flipper_file->file = storage_file_alloc(flipper_file->storage);

    return flipper_file;
}

void flipper_file_free(FlipperFile* flipper_file) {
    furi_assert(flipper_file);
    if(storage_file_is_open(flipper_file->file)) {
        storage_file_close(flipper_file->file);
    }
    storage_file_free(flipper_file->file);
    free(flipper_file);
}

bool flipper_file_open_read(FlipperFile* flipper_file, const char* filename) {
    furi_assert(flipper_file);
    bool result = storage_file_open(flipper_file->file, filename, FSAM_READ, FSOM_OPEN_EXISTING);
    return result;
}

bool flipper_file_open_append(FlipperFile* flipper_file, const char* filename) {
    furi_assert(flipper_file);
    bool result = storage_file_open(flipper_file->file, filename, FSAM_WRITE, FSOM_OPEN_APPEND);

    // TODO:
    // check for '/n' at the end of file and add if not exist
    return result;
}

bool flipper_file_new_write(FlipperFile* flipper_file, const char* filename) {
    furi_assert(flipper_file);
    bool result = storage_file_open(flipper_file->file, filename, FSAM_WRITE, FSOM_CREATE_ALWAYS);
    return result;
}

bool flipper_file_close(FlipperFile* flipper_file) {
    furi_assert(flipper_file);
    if(storage_file_is_open(flipper_file->file)) {
        return storage_file_close(flipper_file->file);
    }
    return true;
}

bool flipper_file_rewind(FlipperFile* flipper_file) {
    furi_assert(flipper_file);
    return storage_file_seek(flipper_file->file, 0, true);
}

bool flipper_file_read_header(FlipperFile* flipper_file, string_t filetype, uint32_t* version) {
    bool result = false;
    do {
        result = flipper_file_read_string(flipper_file, flipper_file_filetype_key, filetype);
        if(!result) break;
        result = flipper_file_read_uint32(flipper_file, flipper_file_version_key, version, 1);
        if(!result) break;
    } while(false);

    return result;
}

bool flipper_file_write_header(
    FlipperFile* flipper_file,
    string_t filetype,
    const uint32_t version) {
    bool result = false;
    do {
        result = flipper_file_write_string(flipper_file, flipper_file_filetype_key, filetype);
        if(!result) break;
        result = flipper_file_write_uint32(flipper_file, flipper_file_version_key, &version, 1);
        if(!result) break;
    } while(false);

    return result;
}

bool flipper_file_write_header_cstr(
    FlipperFile* flipper_file,
    const char* filetype,
    const uint32_t version) {
    bool result = false;
    string_t value;
    string_init_set(value, filetype);
    result = flipper_file_write_header(flipper_file, value, version);
    string_clear(value);
    return result;
}

bool flipper_file_read_string(FlipperFile* flipper_file, const char* key, string_t data) {
    furi_assert(flipper_file);

    bool result = false;
    if(flipper_file_seek_to_key(flipper_file->file, key)) {
        if(flipper_file_read_until(flipper_file->file, data)) {
            result = true;
        }
    }
    return result;
}

bool flipper_file_write_string(FlipperFile* flipper_file, const char* key, string_t data) {
    furi_assert(flipper_file);

    bool result = false;
    do {
        result = flipper_file_write_key(flipper_file->file, key);
        if(!result) break;

        result = flipper_file_write(flipper_file->file, string_get_cstr(data), string_size(data));
        if(!result) break;

        result = flipper_file_write_eol(flipper_file->file);
    } while(false);

    return result;
}

bool flipper_file_write_string_cstr(FlipperFile* flipper_file, const char* key, const char* data) {
    bool result = false;
    string_t value;
    string_init_set(value, data);
    result = flipper_file_write_string(flipper_file, key, value);
    string_clear(value);
    return result;
}

bool flipper_file_read_uint32(
    FlipperFile* flipper_file,
    const char* key,
    uint32_t* data,
    const uint16_t data_size) {
    bool result = false;
    string_t value;
    string_init(value);

    if(flipper_file_seek_to_key(flipper_file->file, key)) {
        result = true;
        for(uint16_t i = 0; i < data_size; i++) {
            bool last = false;
            result = flipper_file_read_one_value(flipper_file->file, value, &last);
            if(result) {
                if(sscanf(string_get_cstr(value), "%" PRIu32, &data[i]) != 1) {
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
    }

    string_clear(value);
    return result;
}

bool flipper_file_get_value_count(FlipperFile* flipper_file, const char* key, uint32_t* count) {
    furi_assert(flipper_file);
    bool result = false;
    bool last = false;

    string_t value;
    string_init(value);

    uint32_t position = storage_file_tell(flipper_file->file);
    do {
        if(!flipper_file_seek_to_key(flipper_file->file, key)) break;

        // Balance between speed and memory consumption
        // I prefer lower speed but less memory consumption
        *count = 0;

        result = true;
        while(true) {
            if(!flipper_file_read_one_value(flipper_file->file, value, &last)) {
                result = false;
                break;
            }

            *count = *count + 1;
            if(last) break;
        }

    } while(true);

    if(!storage_file_seek(flipper_file->file, position, true)) {
        result = false;
    }

    string_clear(value);
    return result;
}

bool flipper_file_write_uint32(
    FlipperFile* flipper_file,
    const char* key,
    const uint32_t* data,
    const uint16_t data_size) {
    bool result = false;
    string_t value;
    string_init(value);

    do {
        result = flipper_file_write_key(flipper_file->file, key);
        if(!result) break;

        for(uint16_t i = 0; i < data_size; i++) {
            string_printf(value, "%" PRIu32, data[i]);
            if((i + 1) < data_size) {
                string_cat(value, " ");
            }

            result =
                flipper_file_write(flipper_file->file, string_get_cstr(value), string_size(value));
            if(!result) break;
        }

        result = flipper_file_write_eol(flipper_file->file);
    } while(false);

    string_clear(value);
    return result;
}

bool flipper_file_read_float(
    FlipperFile* flipper_file,
    const char* key,
    float* data,
    const uint16_t data_size) {
    bool result = false;
    string_t value;
    string_init(value);

    if(flipper_file_seek_to_key(flipper_file->file, key)) {
        result = true;
        for(uint16_t i = 0; i < data_size; i++) {
            bool last = false;
            result = flipper_file_read_one_value(flipper_file->file, value, &last);
            if(result) {
                if(sscanf(string_get_cstr(value), "%f", &data[i]) != 1) {
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
    }

    string_clear(value);
    return result;
}

bool flipper_file_write_float(
    FlipperFile* flipper_file,
    const char* key,
    const float* data,
    const uint16_t data_size) {
    bool result = false;
    string_t value;
    string_init(value);

    do {
        result = flipper_file_write_key(flipper_file->file, key);
        if(!result) break;

        for(uint16_t i = 0; i < data_size; i++) {
            string_printf(value, "%f", data[i]);
            if((i + 1) < data_size) {
                string_cat(value, " ");
            }

            result =
                flipper_file_write(flipper_file->file, string_get_cstr(value), string_size(value));
            if(!result) break;
        }

        result = flipper_file_write_eol(flipper_file->file);
    } while(false);

    string_clear(value);
    return result;
}

bool flipper_file_write_comment(FlipperFile* flipper_file, string_t data) {
    furi_assert(flipper_file);

    bool result = false;
    do {
        const char comment_buffer[2] = {flipper_file_comment, ' '};
        result = flipper_file_write(flipper_file->file, comment_buffer, sizeof(comment_buffer));
        if(!result) break;

        result = flipper_file_write(flipper_file->file, string_get_cstr(data), string_size(data));
        if(!result) break;

        result = flipper_file_write_eol(flipper_file->file);
    } while(false);

    return result;
}

bool flipper_file_write_comment_cstr(FlipperFile* flipper_file, const char* data) {
    bool result = false;
    string_t value;
    string_init_set(value, data);
    result = flipper_file_write_comment(flipper_file, value);
    string_clear(value);
    return result;
}

bool flipper_file_write_hex_array(
    FlipperFile* flipper_file,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size) {
    furi_assert(flipper_file);

    bool result = false;
    do {
        result = flipper_file_write_key(flipper_file->file, key);
        if(!result) break;

        result = flipper_file_write_hex_internal(flipper_file->file, data, data_size);
        if(!result) break;

        result = flipper_file_write_eol(flipper_file->file);
    } while(false);

    return result;
}

bool flipper_file_read_hex_array(
    FlipperFile* flipper_file,
    const char* key,
    uint8_t* data,
    const uint16_t data_size) {
    furi_assert(flipper_file);
    bool result = false;
    string_t value;
    string_init(value);

    if(flipper_file_seek_to_key(flipper_file->file, key)) {
        for(uint16_t i = 0; i < data_size; i++) {
            bool last = false;
            result = flipper_file_read_one_value(flipper_file->file, value, &last);
            if(result) {
                if(hex_chars_to_uint8(
                       string_get_char(value, 0), string_get_char(value, 1), &data[i])) {
                    result = true;
                } else {
                    result = false;
                    break;
                }
            } else {
                break;
            }

            // if it's last value but we not fill whole array
            if(last && ((i + 1) != data_size)) {
                result = false;
                break;
            }
        }
    }

    string_clear(value);
    return result;
}

bool flipper_file_delete_key(FlipperFile* flipper_file, const char* key) {
    bool result = false;
    File* scratch_file = storage_file_alloc(flipper_file->storage);

    uint32_t position = storage_file_tell(flipper_file->file);

    do {
        uint64_t file_size = storage_file_size(flipper_file->file);
        if(file_size == 0) break;

        if(!storage_file_seek(flipper_file->file, 0, true)) break;

        if(!flipper_file_seek_to_key(flipper_file->file, key)) break;
        uint64_t start_position = storage_file_tell(flipper_file->file) - strlen(key);
        if(start_position >= 2) {
            start_position -= 2;
        } else {
            // something wrong
            break;
        }

        if(!flipper_file_seek_to_next_line(flipper_file->file)) break;
        uint64_t end_position = storage_file_tell(flipper_file->file);
        if(end_position < file_size) {
            end_position += 1;
        }

        const char* scratch_name = "";
        if(!flipper_file_get_scratchpad_name(&scratch_name)) break;

        if(!storage_file_open(scratch_file, scratch_name, FSAM_WRITE, FSOM_CREATE_ALWAYS)) break;

        if(!flipper_file_copy(flipper_file->file, scratch_file, 0, start_position)) break;
        if(!flipper_file_copy(flipper_file->file, scratch_file, end_position, file_size)) break;

        file_size = storage_file_tell(scratch_file);
        if(file_size == 0) break;

        if(!storage_file_seek(flipper_file->file, 0, true)) break;
        if(!flipper_file_copy(scratch_file, flipper_file->file, 0, file_size)) break;

        if(!storage_file_truncate(flipper_file->file)) break;

        if(!storage_file_close(scratch_file)) break;
        if(storage_common_remove(flipper_file->storage, scratch_name) != FSE_OK) break;
        result = true;
    } while(false);

    storage_file_free(scratch_file);

    if(!storage_file_seek(flipper_file->file, position, true)) {
        result = false;
    }

    return result;
}