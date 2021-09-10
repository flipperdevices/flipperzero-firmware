#include <unittests.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "submsg_array.pb.h"

int main()
{
    int status = 0;
    
    COMMENT("Test encoding for submessage with array");
    {
        uint8_t buffer[TestMessage_size] = {0};
        pb_ostream_t ostream = pb_ostream_from_buffer(buffer, TestMessage_size);
        TestMessage msg = TestMessage_init_zero;
        
        msg.submsg.rep_uint32_count = 3;
        msg.submsg.rep_uint32[0] = 0;
        msg.submsg.rep_uint32[1] = 1;
        msg.submsg.rep_uint32[2] = 2;
        
        TEST(pb_encode(&ostream, TestMessage_fields, &msg));
        TEST(ostream.bytes_written > 0);
        
        {
            pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
            TestMessage msg2 = TestMessage_init_zero;

            TEST(pb_decode(&istream, TestMessage_fields, &msg2));
            TEST(msg2.submsg.rep_uint32_count == 3);
            TEST(msg2.submsg.rep_uint32[0] == 0);
            TEST(msg2.submsg.rep_uint32[1] == 1);
            TEST(msg2.submsg.rep_uint32[2] == 2);
        }
    }
    
    return status;
}

