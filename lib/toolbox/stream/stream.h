#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <mlib/m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Stream Stream;

typedef enum {
    StreamOffsetFromCurrent,
    StreamOffsetFromStart,
    StreamOffsetFromEnd,
} StreamOffset;

typedef size_t (*StreamWriteCB)(Stream* stream, const void* context);

/**
 * Free Stream
 * @param stream Stream instance
 */
void stream_free(Stream* stream);

/**
 * Clean (empty) Stream
 * @param stream Stream instance
 */
void stream_clean(Stream* stream);

/**
 * Indicates that the rw pointer is at the end of the stream
 * @param stream Stream instance
 * @return true if rw pointer is at the end of the stream
 * @return false if rw pointer is not at the end of the stream
 */
bool stream_eof(Stream* stream);

/**
 * Moves the rw pointer.
 * @param stream Stream instance
 * @param offset how much to move the pointer
 * @param offset_type starting from what
 * @return true 
 * @return false 
 */
bool stream_seek(Stream* stream, int32_t offset, StreamOffset offset_type);

/**
 * Gets the value of the rw pointer
 * @param stream Stream instance
 * @return size_t value of the rw pointer
 */
size_t stream_tell(Stream* stream);

/**
 * Gets the size of the stream
 * @param stream Stream instance
 * @return size_t size of the stream
 */
size_t stream_size(Stream* stream);

/**
 * Write N bytes to the stream
 * @param stream Stream instance
 * @param data data to write
 * @param size size of data to be written
 * @return size_t how many bytes was written
 */
size_t stream_write(Stream* stream, const uint8_t* data, size_t size);

/**
 * Read N bytes from stream
 * @param stream Stream instance
 * @param data data to be read
 * @param count size of data to be read
 * @return size_t how many bytes was read
 */
size_t stream_read(Stream* stream, uint8_t* data, size_t count);

/**
 * Delete N chars from the stream and write data by calling write_callback(context)
 * @param stream Stream instance
 * @param delete_size size of data to be deleted
 * @param write_callback write callback
 * @param context write callback context
 * @return int32_t size of written data minus size of deleted data
 */
int32_t stream_delete_and_insert(
    Stream* stream,
    size_t delete_size,
    StreamWriteCB write_callback,
    const void* context);

/****** Some random helpers starts here ******/

/**
 * Moves the rw pointer to the start
 * @param stream Stream instance
 */
void stream_rewind(Stream* stream);

/**
 * Write char to the stream
 * @param stream Stream instance
 * @param c char value
 * @return size_t how many bytes was written
 */
size_t stream_write_char(Stream* stream, char c);

/**
 * Write string to the stream
 * @param stream Stream instance
 * @param string string value
 * @return size_t how many bytes was written
 */
size_t stream_write_string(Stream* stream, string_t string);

/**
 * Write const char* to the stream
 * @param stream Stream instance
 * @param string c-string value
 * @return size_t how many bytes was written
 */
size_t stream_write_cstring(Stream* stream, const char* string);

/**
 * Write formatted string to the stream
 * @param stream Stream instance
 * @param format 
 * @param ... 
 * @return size_t how many bytes was written
 */
size_t stream_write_format(Stream* stream, const char* format, ...);

/**
 * Write formatted string to the stream, va_list version
 * @param stream Stream instance
 * @param format 
 * @param args 
 * @return size_t how many bytes was written
 */
size_t stream_write_vaformat(Stream* stream, const char* format, va_list args);

/**
 * Insert N chars to the stream, starting at the current pointer.
 * Data will be inserted, not overwritteт, so the stream will be increased in size.
 * @param stream Stream instance
 * @param data data to be inserted
 * @param size size of data to be inserted
 * @return size_t how many bytes was written
 */
size_t stream_insert(Stream* stream, const char* data, size_t size);

/**
 * Remove N chars from the stream, starting at the current pointer.
 * The size may be larger than stream size, the stream will be cleared from current rw pointer to the end.
 * @param stream Stream instance
 * @param size how many chars need to be deleted
 * @return size_t size of deletions in bytes
 */
size_t stream_delete(Stream* stream, size_t size);

#ifdef __cplusplus
}
#endif