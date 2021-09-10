#include <pb_decode.h>
#include <unittests.h>
#include "oneof.pb.h"

int main()
{
    const uint8_t input_data[] = {
        0x08, 0x01, 0x10, 0x0F, 0x18, 0xAC, 0x02, 0x20,
        0xF1, 0x82, 0xA0, 0x85, 0x0C, 0x62, 0x00, 0x58,
        0xF1, 0x82, 0xA0, 0x85, 0x0C
    };
    
    int status = 0;
    TestMessage msg = TestMessage_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(input_data, sizeof(input_data));
    
    TEST(pb_decode(&stream, TestMessage_fields, &msg));
    TEST(msg.which_payload == TestMessage_plb11_tag);
    TEST(msg.payload.plb11.name[0] == 0);
    
    return status;
}
