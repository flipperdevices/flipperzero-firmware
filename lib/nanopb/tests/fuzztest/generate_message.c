/* Generates a random, valid protobuf message. Useful to seed
 * external fuzzers such as afl-fuzz.
 */

#include <pb_encode.h>
#include <pb_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "alltypes_static.pb.h"
#include "random_data.h"

#ifndef FUZZTEST_BUFSIZE
#define FUZZTEST_BUFSIZE 4096
#endif

/* Check that size/count fields do not exceed their max size.
 * Otherwise we would have to loop pretty long in generate_message().
 * Note that there may still be a few encoding errors from submessages.
 */
static void limit_sizes(alltypes_static_AllTypes *msg)
{
    pb_field_iter_t iter;
    pb_field_iter_begin(&iter, alltypes_static_AllTypes_fields, msg);
    while (pb_field_iter_next(&iter))
    {
        if (PB_LTYPE(iter.type) == PB_LTYPE_BYTES)
        {
            ((pb_bytes_array_t*)iter.pData)->size %= iter.data_size - PB_BYTES_ARRAY_T_ALLOCSIZE(0);
        }
        
        if (PB_HTYPE(iter.type) == PB_HTYPE_REPEATED)
        {
            *((pb_size_t*)iter.pSize) %= iter.array_size;
        }
        
        if (PB_HTYPE(iter.type) == PB_HTYPE_ONEOF)
        {
            /* Set the oneof to this message type with 50% chance. */
            if (rand_word() & 1)
            {
                *((pb_size_t*)iter.pSize) = iter.tag;
            }
        }
    }
}

static void generate_message()
{
    alltypes_static_AllTypes msg;
    alltypes_static_TestExtension extmsg = alltypes_static_TestExtension_init_zero;
    pb_extension_t ext = pb_extension_init_zero;

    static uint8_t buf[FUZZTEST_BUFSIZE];
    pb_ostream_t stream = {0};
    
    do {
        rand_fill((void*)&msg, sizeof(msg));
        limit_sizes(&msg);

        rand_fill((void*)&extmsg, sizeof(extmsg));
        ext.type = &alltypes_static_TestExtension_testextension;
        ext.dest = &extmsg;
        ext.next = NULL;
        msg.extensions = &ext;

        stream = pb_ostream_from_buffer(buf, sizeof(buf));
    } while (!pb_encode(&stream, alltypes_static_AllTypes_fields, &msg));
    
    fwrite(buf, 1, stream.bytes_written, stdout);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: generate_message <seed>\n");
        return 1;
    }

    random_set_seed(atol(argv[1]));

    generate_message();
    
    return 0;
}

