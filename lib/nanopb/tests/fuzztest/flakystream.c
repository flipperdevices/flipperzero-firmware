#include "flakystream.h"
#include <string.h>

bool flakystream_callback(pb_istream_t *stream, pb_byte_t *buf, size_t count)
{
    flakystream_t *state = stream->state;

    if (state->position + count > state->msglen)
    {
        stream->bytes_left = 0;
        return false;
    }
    else if (state->position + count > state->fail_after)
    {
        PB_RETURN_ERROR(stream, "flaky error");
    }

    memcpy(buf, state->buffer + state->position, count);
    state->position += count;
    return true;
}

void flakystream_init(flakystream_t *stream, const uint8_t *buffer, size_t msglen, size_t fail_after)
{
    memset(stream, 0, sizeof(*stream));
    stream->stream.callback = flakystream_callback;
    stream->stream.bytes_left = SIZE_MAX;
    stream->stream.state = stream;
    stream->buffer = buffer;
    stream->position = 0;
    stream->msglen = msglen;
    stream->fail_after = fail_after;
}
