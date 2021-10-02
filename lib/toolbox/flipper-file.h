#pragma once
#include <stdint.h>
#include <mlib/m-string.h>
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Flipper file structure
# Commentary
Field name: field value

File header
    Filetype: String
    Version: Uint32

Supported types
    String: text
    Float: 0.02
    Uint32: 1
    Int32: -1
    Hex: A4
    Hex array: A4 B3 C2 D1 12 FF

End of line is \n when writing, but supports \r when reading.
The comment must start with a "#" character on a new line. 
*/

typedef struct FlipperFile FlipperFile;

FlipperFile* flipper_file_alloc(Storage* storage);

void flipper_file_free(FlipperFile* flipper_file);

bool flipper_file_open_read(FlipperFile* flipper_file, const char* filename);

bool flipper_file_new_write(FlipperFile* flipper_file, const char* filename);

bool flipper_file_close(FlipperFile* flipper_file);

bool flipper_file_read_header(FlipperFile* flipper_file, string_t filetype, uint32_t* version);

bool flipper_file_write_header(
    FlipperFile* flipper_file,
    string_t filetype,
    const uint32_t version);

bool flipper_file_write_header_cstr(
    FlipperFile* flipper_file,
    const char* filetype,
    const uint32_t version);

bool flipper_file_read_string(FlipperFile* flipper_file, const char* key, string_t data);

bool flipper_file_write_string(FlipperFile* flipper_file, const char* key, string_t data);

bool flipper_file_write_string_cstr(FlipperFile* flipper_file, const char* key, const char* data);

bool flipper_file_read_uint32(FlipperFile* flipper_file, const char* key, uint32_t* data);

bool flipper_file_write_uint32(FlipperFile* flipper_file, const char* key, const uint32_t data);

bool flipper_file_write_comment(FlipperFile* flipper_file, string_t data);

bool flipper_file_write_comment_cstr(FlipperFile* flipper_file, const char* data);

bool flipper_file_write_hex_array(
    FlipperFile* flipper_file,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size);

bool flipper_file_read_hex_array(
    FlipperFile* flipper_file,
    const char* key,
    uint8_t* data,
    const uint16_t data_size);

#ifdef __cplusplus
}
#endif