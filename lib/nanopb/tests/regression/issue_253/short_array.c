#include <unittests.h>
#include <pb_encode.h>
#include "short_array.pb.h"

int main()
{
    int status = 0;
    
    COMMENT("Test message length calculation for short arrays");
    {
        uint8_t buffer[TestMessage_size] = {0};
        pb_ostream_t ostream = pb_ostream_from_buffer(buffer, TestMessage_size);
        TestMessage msg = TestMessage_init_zero;
        
        msg.rep_uint32_count = 1;
        msg.rep_uint32[0] = ((uint32_t)1 << 31);
        
        TEST(pb_encode(&ostream, TestMessage_fields, &msg));
        TEST(ostream.bytes_written == TestMessage_size);
    }
    
    return status;
}

