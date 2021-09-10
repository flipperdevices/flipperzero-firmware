#include <pb_encode.h>
#include <pb_decode.h>

#include "mixed.pb.h"
#include "unittests.h"

int main()
{
    int status = 0;
    pb_byte_t buf[64];
    size_t msglen;

    {
        pb_ostream_t ostream = pb_ostream_from_buffer(buf, sizeof(buf));
        MixedMessage msg = MixedMessage_init_default;

        msg.has_proto2_value = true;
        msg.proto2_value = 0;
        msg.proto3_value = 0;
        
        if (!pb_encode(&ostream, MixedMessage_fields, &msg))
        {
            fprintf(stderr, "pb_encode() failed: %s\n", PB_GET_ERROR(&ostream));
            return 1;
        }

        msglen = ostream.bytes_written;
        TEST(msglen > 0);
    }

    {
        pb_istream_t istream = pb_istream_from_buffer(buf, msglen);
        MixedMessage msg = MixedMessage_init_default;
        
        TEST(msg.proto2_value == 100);
        TEST(msg.proto3_value == 200);

        if (!pb_decode(&istream, MixedMessage_fields, &msg))
        {
            fprintf(stderr, "pb_decode() failed: %s\n", PB_GET_ERROR(&istream));
            return 1;
        }

        TEST(msg.proto2_value == 0);
        TEST(msg.proto3_value == 0);
    }

    return status;
}