#include <pb_decode.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "test_helpers.h"
#include "test.pb.h"

bool stream_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    FILE *file = (FILE*)stream->state;
    size_t len = fread(buf, 1, count, file);
    
    if (len == count)
    {
        return true;
    }
    else
    {
        stream->bytes_left = 0;
        return false;
    }
}

int main()
{
    pb_istream_t stream = {&stream_callback, NULL, SIZE_MAX};
    MyMessage msg = MyMessage_init_default;
    bool status;
    stream.state = stdin;
    SET_BINARY_MODE(stdin);

    set_max_alloc_bytes(512);

    status = pb_decode(&stream, MyMessage_fields, &msg);
    assert(!status);
    assert(strcmp(stream.errmsg, "realloc failed") == 0);
    return 0;
}

