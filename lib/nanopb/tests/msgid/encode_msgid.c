/* Encode a message using msgid field as prefix */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_encode.h>
#include "msgid_example.pb.h"
#include "test_helpers.h"

/* This function writes the message id as a prefix to the message, allowing
 * the receiving side to identify message type. Here we use uint8_t to store
 * it, but e.g. varint or some custom header struct would work just as well.
 */
bool write_prefix(pb_ostream_t *stream, int msgid)
{
    uint8_t prefix = msgid;
    return pb_write(stream, &prefix, 1);
}

/* The main logic will call one of these functions.
 * Normally which function you call would be selected based on what message
 * you want to send, here it is decided based on command line parameter.
 */

bool encode_MyMessage1(pb_ostream_t *stream)
{
    MyMessage1 msg = MyMessage1_init_default;
    msg.intvalue = 1234;
    return write_prefix(stream, MyMessage1_msgid)
           && pb_encode(stream, MyMessage1_fields, &msg);
}

bool encode_MyMessage2(pb_ostream_t *stream)
{
    MyMessage2 msg = MyMessage2_init_default;
    msg.intvalue = 9999;
    strcpy(msg.strvalue, "Msg2");
    return write_prefix(stream, MyMessage2_msgid)
           && pb_encode(stream, MyMessage2_fields, &msg);
}

bool encode_MyMessage3(pb_ostream_t *stream)
{
    MyMessage3 msg = MyMessage3_init_default;
    msg.boolvalue = true;
    return write_prefix(stream, MyMessage3_msgid)
           && pb_encode(stream, MyMessage3_fields, &msg);
}

int main(int argc, char **argv)
{
    uint8_t buffer[128];
    pb_ostream_t stream;
    bool status = false;
    int option;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: encode_msgid [number]\n");
        return 1;
    }
    option = atoi(argv[1]);

    stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    if (option == 1)
    {
        status = encode_MyMessage1(&stream);
    }
    else if (option == 2)
    {
        status = encode_MyMessage2(&stream);
    }
    else if (option == 3)
    {
        status = encode_MyMessage3(&stream);
    }

    if (status)
    {
        SET_BINARY_MODE(stdout);
        fwrite(buffer, 1, stream.bytes_written, stdout);
        return 0;
    }
    else
    {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }
}
