/* A small tool that decodes a raw binary message, while providing useful
 * info on corrupted messages also. */

/* Define _ISOC99_SOURCE to get snprintf() even though otherwise in ansi-C mode */
#define _ISOC99_SOURCE 1

#include <stdio.h>
#include <string.h>
#include <pb_decode.h>
#include "test_helpers.h"

#define HISTORY_LEN 32
static pb_byte_t g_history[HISTORY_LEN];
static int g_position;

/* This binds the pb_istream_t to stdin and logs the most recent bytes read. */
bool callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    FILE *file = (FILE*)stream->state;
    size_t len = fread(buf, 1, count, file);
    
    if (len < HISTORY_LEN)
    {
        memmove(g_history, g_history + len, HISTORY_LEN - len);
        memcpy(g_history + HISTORY_LEN - len, buf, len);
    }
    else
    {
        memcpy(g_history, buf + len - HISTORY_LEN, HISTORY_LEN);
    }
    
    g_position += len;
    
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

void print_history(int position)
{
    int i;
    
    if (position < g_position - HISTORY_LEN)
        position = g_position - HISTORY_LEN;
    
    printf("LATEST BYTES READ (%d to %d): ", position, g_position);
    
    for (i = HISTORY_LEN - (g_position - position); i < HISTORY_LEN; i++)
    {
        printf("%02x ", g_history[i]);
    }
    
    printf("\n");
}

bool raw_decode(pb_istream_t *stream, const char *indent)
{
    const char *wiretypes[8] = {"VARINT", "64BIT", "STRING", "SGRP", "EGRP", "32BIT", "????", "????"};

    while (stream->bytes_left)
    {
        uint32_t tag;
        pb_wire_type_t wire_type;
        bool eof;
        int position = g_position;

        if (!pb_decode_tag(stream, &wire_type, &tag, &eof))
        {
            if (eof)
            {
                break;
            }
            else
            {
                printf("ERROR: Failed to parse tag: %s\n", PB_GET_ERROR(stream));
                print_history(position);
                return false;
            }
        }
        
        if (tag == 0)
        {
            printf("%sterminating on zero tag\n", indent);
            return true;
        }

        printf("%sAt %d: field tag %d, wire type %d (%s)",
               indent, position, (int)tag, wire_type, wiretypes[wire_type]);
        
        if (wire_type == PB_WT_VARINT)
        {
            uint64_t value;
            position = g_position;
            if (!pb_decode_varint(stream, &value))
            {
                printf("\n%sERROR: Failed to parse varint: %s\n", indent, PB_GET_ERROR(stream));
                print_history(position);
                return false;
            }
            
            printf(", varint value (%d bytes): %llu\n",
                   g_position - position, (unsigned long long)value);
        }
        else if (wire_type == PB_WT_64BIT)
        {
            uint64_t value;
            position = g_position;
            if (!pb_decode_fixed64(stream, &value))
            {
                printf("\n%sERROR: Failed to parse fixed64: %s\n", indent, PB_GET_ERROR(stream));
                print_history(position);
                return false;
            }
            
            printf(", fixed64 value (%d bytes): 0x%016llx\n",
                   g_position - position, (unsigned long long)value);
        }
        else if (wire_type == PB_WT_32BIT)
        {
            uint32_t value;
            position = g_position;
            if (!pb_decode_fixed32(stream, &value))
            {
                printf("\n%sERROR: Failed to parse fixed32: %s\n", indent, PB_GET_ERROR(stream));
                print_history(position);
                return false;
            }
            
            printf(", fixed32 value (%d bytes): 0x%08lx\n",
                   g_position - position, (unsigned long)value);
        }
        else if (wire_type == PB_WT_STRING)
        {
            pb_istream_t substream;
            position = g_position;
            if (!pb_make_string_substream(stream, &substream))
            {
                printf("ERROR: Failed to parse string length: %s\n", PB_GET_ERROR(stream));
                print_history(position);
                return false;
            }
            else
            {
            
                if (substream.bytes_left == 0)
                {
                    printf(", empty string\n");
                }
                else
                {
                    char prefix[8];
                    snprintf(prefix, sizeof(prefix), "f%d> ", (int)tag);
                    
                    printf(", string len %d bytes, attempting recursive decode\n",
                       (int)substream.bytes_left);
                
                    if (!raw_decode(&substream, prefix))
                    {
                        printf("%sfield %d: recursive decode failed, continuing with upper level\n\n",
                               indent, (int)tag);
                    }
                    
                    pb_close_string_substream(stream, &substream);
                }
            }
        }
        else
        {
            printf("\n");
        }
    }
    
    return true;
}

int main()
{
    pb_istream_t stream = {&callback, NULL, SIZE_MAX};
    stream.state = stdin;
    SET_BINARY_MODE(stdin);

    if (!raw_decode(&stream, ""))
    {
        return 1;
    } else {
        return 0;
    }
}
