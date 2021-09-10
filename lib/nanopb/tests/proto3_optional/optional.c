#include <pb_encode.h>
#include <pb_decode.h>
#include <unittests.h>
#include "optional.pb.h"

int main()
{
    int status = 0;
    uint8_t buf[256];
    size_t msglen;
    
    COMMENT("Test encoding message with optional field")
    {
        pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
        TestMessage msg = TestMessage_init_zero;
        
        msg.has_opt_int = true;
        msg.opt_int = 99;
        msg.normal_int = 100;
        msg.opt_int2 = 101;
        
        TEST(pb_encode(&stream, TestMessage_fields, &msg));
        msglen = stream.bytes_written;
    }
    
    COMMENT("Test decoding message with optional field")
    {
        pb_istream_t stream = pb_istream_from_buffer(buf, msglen);
        TestMessage msg = TestMessage_init_zero;
        
        /* These fields should be missing from the message
         * so the values wouldn't be overwritten. */
        msg.opt_int2 = 5;
        msg.normal_int2 = 6;
        
        TEST(pb_decode_noinit(&stream, TestMessage_fields, &msg));
        TEST(msg.has_opt_int);
        TEST(msg.opt_int == 99);
        TEST(msg.normal_int == 100);
        TEST(!msg.has_opt_int2);
        TEST(msg.opt_int2 == 5);
        TEST(msg.normal_int2 == 6);
    }

    return status;
}

