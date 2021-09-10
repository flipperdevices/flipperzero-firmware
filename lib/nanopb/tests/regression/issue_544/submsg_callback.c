#include "submsg_callback.pb.h"
#include <pb_encode.h>
#include <pb_decode.h>
#include "unittests.h"

bool msg_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    /* This tests decoding the submessage already in the message level callback. */

    SubMessage *submsg = (SubMessage*)field->pData;

    if (!pb_decode(stream, SubMessage_fields, submsg))
        PB_RETURN_ERROR(stream, "submsg decode failed");
    
    if (submsg->foo != 1234)
        PB_RETURN_ERROR(stream, "submsg.foo wrong value");
    
    return true;
}

int main()
{
    int status = 0;
    pb_byte_t buf[64];
    size_t msglen;

    {
        pb_ostream_t ostream = pb_ostream_from_buffer(buf, sizeof(buf));
        MyMessage msg = MyMessage_init_zero;

        msg.which_oneof = MyMessage_submsg_tag;
        msg.oneof.submsg.foo = 1234;

        if (!pb_encode(&ostream, MyMessage_fields, &msg))
        {
            fprintf(stderr, "pb_encode() failed: %s\n", PB_GET_ERROR(&ostream));
            return 1;
        }

        msglen = ostream.bytes_written;
        TEST(msglen > 0);
    }

    {
        pb_istream_t istream = pb_istream_from_buffer(buf, msglen);
        MyMessage msg = MyMessage_init_zero;
        msg.cb_oneof.funcs.decode = msg_callback;

        if (!pb_decode(&istream, MyMessage_fields, &msg))
        {
            fprintf(stderr, "pb_decode() failed: %s\n", PB_GET_ERROR(&istream));
            return 1;
        }
    }

    return status;
}