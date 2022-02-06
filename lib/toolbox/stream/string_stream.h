#include <stdlib.h>
#include <mlib/m-string.h>

typedef struct StringStream StringStream;

typedef enum {
    StringStreamOffsetFromCurrent,
    StringStreamOffsetFromStart,
    StringStreamOffsetFromEnd,
} StringStreamOffset;

/**
 * Allocate stream
 * @return StringStream* 
 */
StringStream* string_stream_alloc();

/**
 * Free StringStream
 * @param stream StringStream instance
 */
void string_stream_free(StringStream* stream);

/**
 * Clean (empty) StringStream
 * @param stream StringStream instance
 */
void string_stream_clean(StringStream* stream);

/**
 * Moves the rw pointer to the start
 * @param stream StringStream instance
 */
void string_stream_rewind(StringStream* stream);

/**
 * Moves the rw pointer.
 * @param stream StringStream instance
 * @param offset how much to move the pointer
 * @param offset_type starting from what
 * @return true 
 * @return false 
 */
bool string_stream_seek(StringStream* stream, int32_t offset, StringStreamOffset offset_type);

/**
 * Gets the value of the rw pointer
 * @param stream StringStream instance
 * @return size_t value of the rw pointer
 */
size_t string_stream_tell(StringStream* stream);

/**
 * Indicates that the rw pointer is at the end of the stream
 * @param stream StringStream instance
 * @return true if rw pointer is at the end of the stream
 * @return false if rw pointer is not at the end of the stream
 */
bool string_stream_eof(StringStream* stream);

/**
 * Gets the size of the stream
 * @param stream StringStream instance
 * @return size_t size of the stream
 */
size_t string_stream_size(StringStream* stream);

/**
 * Write N chars to the stream
 * @param stream StringStream instance
 * @param data data to write
 * @param size size of data to be written
 * @return size_t how many bytes was written
 */
size_t string_stream_write(StringStream* stream, const char* data, size_t size);

/**
 * Write char to the stream
 * @param stream StringStream instance
 * @param c char value
 * @return size_t how many bytes was written
 */
size_t string_stream_write_char(StringStream* stream, char c);

/**
 * Write string to the stream
 * @param stream StringStream instance
 * @param string string value
 * @return size_t how many bytes was written
 */
size_t string_stream_write_string(StringStream* stream, string_t string);

/**
 * Write const char* to the stream
 * @param stream StringStream instance
 * @param string c-string value
 * @return size_t how many bytes was written
 */
size_t string_stream_write_cstring(StringStream* stream, const char* string);

/**
 * Write formatted string to the stream
 * @param stream StringStream instance
 * @param format 
 * @param ... 
 * @return size_t how many bytes was written
 */
size_t string_stream_write_format(StringStream* stream, const char* format, ...);

/**
 * Write formatted string to the stream, va_list version
 * @param stream StringStream instance
 * @param format 
 * @param args 
 * @return size_t how many bytes was written
 */
size_t string_stream_write_vaformat(StringStream* stream, const char* format, va_list args);

/**
 * Read N data from stream
 * @param stream StringStream instance
 * @param data data to be read
 * @param count size of data to be read
 * @return size_t how many bytes was read
 */
size_t string_stream_read(StringStream* stream, const char* data, size_t count);

/**
 * Insert N chars to the stream, starting at the current pointer.
 * Data will be inserted, not overwritteт, so the stream will be increased in size.
 * @param stream StringStream instance
 * @param data data to be inserted
 * @param size size of data to be inserted
 * @return size_t how many bytes was written
 */
size_t string_stream_insert(StringStream* stream, const char* data, size_t size);

/**
 * Remove N chars from the stream, starting at the current pointer.
 * The size may be larger than it was in the stream, the stream will be cleared to the end.
 * @param stream StringStream instance
 * @param size how many chars need to be deleted
 * @return size_t size of deletions in bytes
 */
size_t string_stream_delete(StringStream* stream, size_t size);