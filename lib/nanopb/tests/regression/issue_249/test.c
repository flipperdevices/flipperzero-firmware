#include "test.pb.h"
#include <unittests.h>
#include <pb_encode.h>
#include <pb_decode.h>

static bool write_array(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_varint(stream, 1000 + i))
            return false;
    }
    
    return true;
}

static bool read_array(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint32_t i;
    int *sum = *arg;
    
    if (!pb_decode_varint32(stream, &i))
        return false;
    
    *sum += i;
    
    return true;
}

int main()
{
    int status = 0;
    pb_byte_t buf[128] = {0};
    pb_size_t msglen;
    
    {
        MainMessage msg = MainMessage_init_zero;
        pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
        msg.submsg.foo.funcs.encode = &write_array;
        TEST(pb_encode(&stream, MainMessage_fields, &msg));
        msglen = stream.bytes_written;
    }
    
    {
        MainMessage msg = MainMessage_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(buf, msglen);
        int sum = 0;
        msg.submsg.foo.funcs.decode = &read_array;
        msg.submsg.foo.arg = &sum;
        TEST(pb_decode(&stream, MainMessage_fields, &msg));
        TEST(sum == 1000 + 1001 + 1002 + 1003 + 1004);
    }
    
    return status;
}

