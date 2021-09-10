/* This module implements a custom input stream that can be set to give IO error
 * at specific point. */

#ifndef FLAKYSTREAM_H
#define FLAKYSTREAM_H

#include <pb_decode.h>

typedef struct {
    pb_istream_t stream;
    const uint8_t *buffer;
    size_t position;
    size_t msglen;
    size_t fail_after;
} flakystream_t;

void flakystream_init(flakystream_t *stream, const uint8_t *buffer, size_t msglen, size_t fail_after);

#endif
