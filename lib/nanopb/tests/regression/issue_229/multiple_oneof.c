#include "multiple_oneof.pb.h"
#include <unittests.h>
#include <pb_encode.h>
#include <pb_decode.h>

int main()
{
    int status = 0;
    uint8_t buf[128];
    size_t msglen;

    {
        pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
        MainMessage msg = MainMessage_init_zero;
        msg.which_oneof1 = MainMessage_oneof1_uint32_tag;
        msg.oneof1.oneof1_uint32 = 1234;
        msg.which_oneof2 = MainMessage_oneof2_uint32_tag;
        msg.oneof2.oneof2_uint32 = 5678;
        TEST(pb_encode(&stream, MainMessage_fields, &msg));
        msglen = stream.bytes_written;
    }
    
    {
        pb_istream_t stream = pb_istream_from_buffer(buf, msglen);
        MainMessage msg = MainMessage_init_zero;
        TEST(pb_decode(&stream, MainMessage_fields, &msg));
        TEST(msg.which_oneof1 == MainMessage_oneof1_uint32_tag);
        TEST(msg.oneof1.oneof1_uint32 == 1234);
        TEST(msg.which_oneof2 == MainMessage_oneof2_uint32_tag);
        TEST(msg.oneof2.oneof2_uint32 == 5678);
    }
    
    return status;
}

