#include "defaults.pb.h"
#include <unittests.h>
#include <pb_decode.h>

int check_defaults(const DefaultsMsg *msg)
{
    int status = 0;
    
    TEST(msg->b1[0] == 0xDE && msg->b1[1] == 0xAD && msg->b1[2] == 0x00 &&
         msg->b1[3] == 0xBE && msg->b1[4] == 0xEF);
    TEST(msg->b2.bytes[0] == 0xDE && msg->b2.bytes[1] == 0xAD &&
         msg->b2.bytes[2] == 0x00 && msg->b2.bytes[3] == 0xBE &&
         msg->b2.bytes[4] == 0xEF && msg->b2.size == 5);
    TEST(msg->b3.bytes[0] == 0xDE && msg->b3.bytes[1] == 0xAD &&
         msg->b3.bytes[2] == 0x00 && msg->b3.bytes[3] == 0xBE &&
         msg->b3.bytes[4] == 0xEF && msg->b2.size == 5);
    TEST(msg->s1[0] == (char)0xC3 && msg->s1[1] == (char)0xA4 &&
         msg->s1[2] == (char)0xC3 && msg->s1[3] == (char)0xB6 &&
         msg->s1[4] == '\0');
    
    return status;
}

int main()
{
    int status = 0;
    
    {
        DefaultsMsg msg = DefaultsMsg_init_default;
        COMMENT("Checking defaults from static initializer");
        status += check_defaults(&msg);
    }

    {
        DefaultsMsg msg = DefaultsMsg_init_zero;
        pb_istream_t empty = {0,0,0};
        pb_decode(&empty, DefaultsMsg_fields, &msg);
        COMMENT("Checking defaults set at runtime");
        status += check_defaults(&msg);
    }

    return status;
}

