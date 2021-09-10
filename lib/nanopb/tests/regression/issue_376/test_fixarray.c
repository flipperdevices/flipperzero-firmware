#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "fixed_array.pb.h"
#include "unittests.h"

int main(int argc, char **argv)
{
    int status = 0;
    uint8_t buffer[64];
    pb_size_t msglen = 0;
    
    {
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        MainMessage msg = MainMessage_init_zero;
        
        msg.submsg.data[0] = 0;
        msg.submsg.data[4] = 5;
        
        TEST(pb_encode(&stream, MainMessage_fields, &msg));

        msglen = stream.bytes_written;
        TEST(msglen > 5);
    }
    
    {
        pb_istream_t stream = pb_istream_from_buffer(buffer, msglen);
        MainMessage msg = MainMessage_init_zero;
        
        TEST(pb_decode(&stream, MainMessage_fields, &msg));
        
        TEST(msg.submsg.data[0] == 0);
        TEST(msg.submsg.data[4] == 5);
    }
    
    return status;
}

