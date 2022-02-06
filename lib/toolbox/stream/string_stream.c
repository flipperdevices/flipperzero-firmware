#include "string_stream.h"
#include <furi/common_defines.h>

struct StringStream {
    string_t string;
    size_t index;
};

StringStream* string_stream_alloc() {
    StringStream* stream = malloc(sizeof(StringStream));
    string_init(stream->string);
    stream->index = 0;
    return stream;
}

void string_stream_free(StringStream* stream) {
    string_clear(stream->string);
    free(stream);
}

void string_stream_clean(StringStream* stream) {
    string_stream_rewind(stream);
    string_reset(stream->string);
}

void string_stream_rewind(StringStream* stream) {
    stream->index = 0;
}

bool string_stream_eof(StringStream* stream) {
    return (string_stream_tell(stream) >= string_stream_size(stream));
}

size_t string_stream_size(StringStream* stream) {
    return string_size(stream->string);
}

size_t string_stream_tell(StringStream* stream) {
    return stream->index;
}

size_t string_stream_write(StringStream* stream, const char* data, size_t size) {
    // TODO: can be optimized for edge cases
    size_t i;
    for(i = 0; i < size; i++) {
        string_stream_write_char(stream, data[i]);
    }

    return i;
}

size_t string_stream_write_char(StringStream* stream, char c) {
    if(string_stream_eof(stream)) {
        string_push_back(stream->string, c);
    } else {
        string_set_char(stream->string, stream->index, c);
    }
    stream->index++;

    return 1;
}

size_t string_stream_write_string(StringStream* stream, string_t string) {
    return string_stream_write(stream, string_get_cstr(string), string_size(string));
}

size_t string_stream_write_cstring(StringStream* stream, const char* string) {
    return string_stream_write(stream, string, strlen(string));
}

size_t string_stream_write_format(StringStream* stream, const char* format, ...) {
    size_t size;

    va_list args;
    va_start(args, format);
    size = string_stream_write_vaformat(stream, format, args);
    va_end(args);

    return size;
}

size_t string_stream_write_vaformat(StringStream* stream, const char* format, va_list args) {
    string_t data;
    string_init_vprintf(data, format, args);
    size_t size = string_stream_write_string(stream, data);
    string_clear(data);

    return size;
}

bool string_stream_seek(StringStream* stream, int32_t offset, StringStreamOffset offset_type) {
    bool result = true;
    switch(offset_type) {
    case StringStreamOffsetFromStart:
        if(offset >= 0) {
            stream->index = offset;
        } else {
            result = false;
            stream->index = 0;
        }
        break;
    case StringStreamOffsetFromCurrent:
        if(((int32_t)stream->index + offset) > 0) {
            stream->index += offset;
        } else {
            result = false;
            stream->index = 0;
        }
        break;
    case StringStreamOffsetFromEnd:
        if(((int32_t)string_size(stream->string) + offset) > 0) {
            stream->index = string_size(stream->string) + offset;
        } else {
            result = false;
            stream->index = 0;
        }
        break;
    }

    int32_t diff = (stream->index - string_size(stream->string));
    if(diff > 0) {
        stream->index -= diff;
        result = false;
    }

    return result;
}

size_t string_stream_read(StringStream* stream, uint8_t* data, size_t count) {
    size_t write_index = 0;
    const char* cstr = string_get_cstr(stream->string);

    if(!string_stream_eof(stream)) {
        while(true) {
            if(write_index >= count) break;

            data[write_index] = cstr[stream->index];
            write_index++;
            string_stream_seek(stream, 1, StringStreamOffsetFromCurrent);
            if(string_stream_eof(stream)) break;
        }
    }

    return write_index;
}

size_t string_stream_insert(StringStream* stream, const char* data, size_t size) {
    size_t stream_size_before = stream_size(stream);

    do {
        string_t right;
        string_init_set(right, &string_get_cstr(stream->string)[stream->index]);

        if(!string_stream_delete(stream, string_stream_size(stream))) break;
        if(string_stream_write(stream, data, size) != size) break;

        string_cat(stream->string, right);
        string_clear(right);

        result = true;
    } while(false);

    return stream_size(stream) - stream_size_before;
}

size_t string_stream_delete(StringStream* stream, size_t size) {
    size_t stream_size_before = stream_size(stream);

    do {
        if(string_stream_eof(stream)) break;
        size_t remain_size = string_stream_size(stream) - string_stream_tell(stream);
        remain_size = MIN(size, remain_size);

        string_replace_at(stream->string, stream->index, remain_size, "");
    } while(false);

    return stream_size_before - stream_size(stream);
}