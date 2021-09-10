#include "unaligned_uint64.pb.h"
#include <pb_encode.h>

int main()
{
    uint8_t buf[128];
    pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
    MainMessage msg = MainMessage_init_zero;
    msg.bar[0] = 'A';
    pb_encode(&stream, MainMessage_fields, &msg);
    
    return 0;
}

