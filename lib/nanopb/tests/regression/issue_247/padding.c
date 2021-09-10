#include <pb_encode.h>
#include <unittests.h>
#include <string.h>
#include "padding.pb.h"

int main()
{
    int status = 0;
    
    TestMessage msg;
    
    /* Set padding bytes to garbage */
    memset(&msg, 0xAA, sizeof(msg));
    
    /* Set all meaningful fields to 0 */
    msg.submsg.boolfield = false;
    msg.submsg.intfield = 0;
    
    /* Test encoding */
    {
        pb_byte_t buf[128] = {0};
        pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
        TEST(pb_encode(&stream, TestMessage_fields, &msg));
        
        /* Because all fields have zero values, proto3 encoder
         * shouldn't write out anything. */
        TEST(stream.bytes_written == 0);
    }
    
    return status;
}

