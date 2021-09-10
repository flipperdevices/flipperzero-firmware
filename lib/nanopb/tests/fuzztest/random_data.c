#include "random_data.h"
#include <string.h>
#include <malloc_wrappers.h>
#include <pb_encode.h>

#ifndef LLVMFUZZER

static uint32_t g_random_seed = 1234;

void random_set_seed(uint32_t seed)
{
    g_random_seed = seed;
}

uint32_t random_get_seed()
{
    return g_random_seed;
}

/* Uses xorshift64 here instead of rand() for both speed and
 * reproducibility across platforms. */
uint32_t rand_word()
{
    g_random_seed ^= g_random_seed << 13;
    g_random_seed ^= g_random_seed >> 17;
    g_random_seed ^= g_random_seed << 5;
    return g_random_seed;
}

/* Get a random integer in range, with approximately flat distribution. */
int rand_int(int min, int max)
{
    return rand_word() % (max + 1 - min) + min;
}

bool rand_bool()
{
    return rand_word() & 1;
}

/* Get a random byte, with skewed distribution.
 * Important corner cases like 0xFF, 0x00 and 0xFE occur more
 * often than other values. */
uint8_t rand_byte()
{
    uint32_t w = rand_word();
    uint8_t b = w & 0xFF;
    if (w & 0x100000)
        b >>= (w >> 8) & 7;
    if (w & 0x200000)
        b <<= (w >> 12) & 7;
    if (w & 0x400000)
        b ^= 0xFF;
    return b;
}

/* Get a random length, with skewed distribution.
 * Favors the shorter lengths, but always atleast 1. */
size_t rand_len(size_t max)
{
    uint32_t w = rand_word();
    size_t s;
    if (w & 0x800000)
        w &= 3;
    else if (w & 0x400000)
        w &= 15;
    else if (w & 0x200000)
        w &= 255;

    s = (w % max);
    if (s == 0)
        s = 1;

    return s;
}

/* Fills a buffer with random data with skewed distribution. */
void rand_fill(uint8_t *buf, size_t count)
{
    for (; count > 0; count--)
    {
        *buf++ = rand_byte();
    }
}

/* Fill with random protobuf-like data */
size_t rand_fill_protobuf(uint8_t *buf, size_t min_bytes, size_t max_bytes, int min_tag)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buf, max_bytes);

    while(stream.bytes_written < min_bytes)
    {
        pb_wire_type_t wt = rand_int(0, 3);
        if (wt == 3) wt = 5; /* Gap in values */

        if (!pb_encode_tag(&stream, wt, rand_int(min_tag, min_tag + 512)))
            break;

        if (wt == PB_WT_VARINT)
        {
            uint64_t value;
            rand_fill((uint8_t*)&value, sizeof(value));
            pb_encode_varint(&stream, value);
        }
        else if (wt == PB_WT_64BIT)
        {
            uint64_t value;
            rand_fill((uint8_t*)&value, sizeof(value));
            pb_encode_fixed64(&stream, &value);
        }
        else if (wt == PB_WT_32BIT)
        {
            uint32_t value;
            rand_fill((uint8_t*)&value, sizeof(value));
            pb_encode_fixed32(&stream, &value);
        }
        else if (wt == PB_WT_STRING)
        {
            size_t len;
            uint8_t *buf;

            if (min_bytes > stream.bytes_written)
                len = rand_len(min_bytes - stream.bytes_written);
            else
                len = 0;

            buf = malloc(len);
            pb_encode_varint(&stream, len);
            rand_fill(buf, len);
            pb_write(&stream, buf, len);
            free(buf);
        }
    }

    return stream.bytes_written;
}

/* Given a buffer of data, mess it up a bit */
void rand_mess(uint8_t *buf, size_t count)
{
    int m = rand_int(0, 3);

    if (m == 0)
    {
        /* Replace random substring */
        int s = rand_int(0, count - 1);
        int l = rand_len(count - s);
        rand_fill(buf + s, l);
    }
    else if (m == 1)
    {
        /* Swap random bytes */
        int a = rand_int(0, count - 1);
        int b = rand_int(0, count - 1);
        int x = buf[a];
        buf[a] = buf[b];
        buf[b] = x;
    }
    else if (m == 2)
    {
        /* Duplicate substring */
        int s = rand_int(0, count - 2);
        int l = rand_len((count - s) / 2);
        memcpy(buf + s + l, buf + s, l);
    }
    else if (m == 3)
    {
        /* Add random protobuf noise */
        int s = rand_int(0, count - 1);
        int l = rand_len(count - s);
        rand_fill_protobuf(buf + s, l, count - s, 1);
    }
}

/* Append or prepend protobuf noise */
void rand_protobuf_noise(uint8_t *buffer, size_t bufsize, size_t *msglen)
{
    int m = rand_int(0, 2);
    size_t max_size = bufsize - 32 - *msglen;
    if (m == 1)
    {
        /* Prepend */
        uint8_t *tmp = malloc_with_check(bufsize);
        size_t s = rand_fill_protobuf(tmp, rand_len(max_size), bufsize - *msglen, 1000);
        memmove(buffer + s, buffer, *msglen);
        memcpy(buffer, tmp, s);
        free_with_check(tmp);
        *msglen += s;
    }
    else if (m == 2)
    {
        /* Append */
        size_t s = rand_fill_protobuf(buffer + *msglen, rand_len(max_size), bufsize - *msglen, 1000);
        *msglen += s;
    }
}

#endif
