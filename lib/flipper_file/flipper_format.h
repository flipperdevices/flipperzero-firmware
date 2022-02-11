#pragma once
#include <stdint.h>
#include <mlib/m-string.h>
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlipperFormat FlipperFormat;

/**
 * Allocate FlipperFormat as string.
 * @return FlipperFormat* pointer to a FlipperFormat instance
 */
FlipperFormat* flipper_format_string_alloc();

/**
 * Allocate FlipperFormat as file.
 * @return FlipperFormat* pointer to a FlipperFormat instance
 */
FlipperFormat* flipper_format_file_alloc(Storage* storage);

/**
 * Opens the file, use only if FlipperFormat allocated as a file.
 * @param flipper_format 
 * @param path 
 * @param access_mode 
 * @param open_mode 
 * @return true 
 * @return false 
 */
bool flipper_format_file_open(
    FlipperFormat* flipper_format,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode);

/**
 * Closes the file, use only if FlipperFormat allocated as a file.
 * @param flipper_format 
 * @return true 
 * @return false 
 */
bool flipper_format_file_close(FlipperFormat* flipper_format);

/**
 * Free FlipperFormat.
 * @param flipper_format Pointer to a FlipperFormat instance
 */
void flipper_format_free(FlipperFormat* flipper_format);

/**
 * Set FlipperFormat mode.
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param strict_mode True obligates not to skip valid fields. False by default.
 */
void flipper_format_set_strict_mode(FlipperFormat* flipper_format, bool strict_mode);

/**
 * Rewind the RW pointer.
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return True on success
 */
bool flipper_format_rewind(FlipperFormat* flipper_format);

/**
 * Read the header (file type and version).
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param filetype File type string
 * @param version Version Value
 * @return True on success
 */
bool flipper_format_read_header(
    FlipperFormat* flipper_format,
    string_t filetype,
    uint32_t* version);

/**
 * Write the header (file type and version).
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param filetype File type string
 * @param version Version Value
 * @return True on success
 */
bool flipper_format_write_header(
    FlipperFormat* flipper_format,
    string_t filetype,
    const uint32_t version);

/**
 * Write the header (file type and version). Plain C string version.
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param filetype File type string
 * @param version Version Value
 * @return True on success
 */
bool flipper_format_write_header_cstr(
    FlipperFormat* flipper_format,
    const char* filetype,
    const uint32_t version);

/**
 * Get the count of values by key
 * @param flipper_format Pointer to a FlipperFormat instance 
 * @param key 
 * @param count 
 * @return bool 
 */
bool flipper_format_get_value_count(
    FlipperFormat* flipper_format,
    const char* key,
    uint32_t* count);

/**
 * Read a string by key
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_read_string(FlipperFormat* flipper_format, const char* key, string_t data);

/**
 * Write key and string
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_write_string(FlipperFormat* flipper_format, const char* key, string_t data);

/**
 * Write key and string. Plain C string version.
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_write_string_cstr(
    FlipperFormat* flipper_format,
    const char* key,
    const char* data);

/**
 * Read array of uint32 by key
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_read_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    uint32_t* data,
    const uint16_t data_size);

/**
 * Write key and array of uint32
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_write_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    const uint32_t* data,
    const uint16_t data_size);

/**
 * Read array of int32 by key
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_read_int32(
    FlipperFormat* flipper_format,
    const char* key,
    int32_t* data,
    const uint16_t data_size);

/**
 * Write key and array of int32
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_write_int32(
    FlipperFormat* flipper_format,
    const char* key,
    const int32_t* data,
    const uint16_t data_size);

/**
 * Read array of float by key
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_read_float(
    FlipperFormat* flipper_format,
    const char* key,
    float* data,
    const uint16_t data_size);

/**
 * Write key and array of float
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_write_float(
    FlipperFormat* flipper_format,
    const char* key,
    const float* data,
    const uint16_t data_size);

/**
 * Read array of hex-formatted bytes by key
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_read_hex(
    FlipperFormat* flipper_format,
    const char* key,
    uint8_t* data,
    const uint16_t data_size);

/**
 * Write key and array of hex-formatted bytes
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @param data Value
 * @param data_size Values count
 * @return True on success
 */
bool flipper_format_write_hex(
    FlipperFormat* flipper_format,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size);

/**
 * Write comment
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param data Comment text
 * @return True on success
 */
bool flipper_format_write_comment(FlipperFormat* flipper_format, string_t data);

/**
 * Write comment. Plain C string version.
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param data Comment text
 * @return True on success
 */
bool flipper_format_write_comment_cstr(FlipperFormat* flipper_format, const char* data);

/**
 * Removes the first matching key and its value. Sets the RW pointer to a position of deleted data.
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param key Key
 * @return True on success
 */
bool flipper_format_delete_key(FlipperFormat* flipper_format, const char* key);

/**
 * Updates the value of the first matching key to a string value. Sets the RW pointer to a position at the end of inserted data.
 * @param flipper_format Pointer to a FlipperFormat instance 
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_update_string(FlipperFormat* flipper_format, const char* key, string_t data);

/**
 * Updates the value of the first matching key to a string value. Plain C version. Sets the RW pointer to a position at the end of inserted data.
 * @param flipper_format Pointer to a FlipperFormat instance 
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_update_string_cstr(
    FlipperFormat* flipper_format,
    const char* key,
    const char* data);

/**
 * Updates the value of the first matching key to a uint32 array value. Sets the RW pointer to a position at the end of inserted data.
 * @param flipper_format Pointer to a FlipperFormat instance 
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_update_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    const uint32_t* data,
    const uint16_t data_size);

/**
 * Updates the value of the first matching key to a int32 array value. Sets the RW pointer to a position at the end of inserted data.
 * @param flipper_format Pointer to a FlipperFormat instance 
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_update_int32(
    FlipperFormat* flipper_format,
    const char* key,
    const int32_t* data,
    const uint16_t data_size);

/**
 * Updates the value of the first matching key to a float array value. Sets the RW pointer to a position at the end of inserted data.
 * @param flipper_format Pointer to a FlipperFormat instance 
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_update_float(
    FlipperFormat* flipper_format,
    const char* key,
    const float* data,
    const uint16_t data_size);

/**
 * Updates the value of the first matching key to an array of hex-formatted bytes. Sets the RW pointer to a position at the end of inserted data.
 * @param flipper_format Pointer to a FlipperFormat instance 
 * @param key Key
 * @param data Value
 * @return True on success
 */
bool flipper_format_update_hex(
    FlipperFormat* flipper_format,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size);

#ifdef __cplusplus
}
#endif