/* Decode a message using msgid prefix to identify message type */

#include <stdio.h>
#include <stdlib.h>
#include <pb_decode.h>
#include "msgid_example.pb.h"
#include "test_helpers.h"

/* This function reads the prefix written by sending side. */
bool read_prefix(pb_istream_t *stream, int *msgid)
{
    uint8_t prefix = 0;

    if (!pb_read(stream, &prefix, 1))
        return false;

    *msgid = prefix;
    return true;
}

/* Main function will call one of these functions based on the prefix */

bool handle_MyMessage1(pb_istream_t *stream)
{
    MyMessage1 msg = MyMessage1_init_default;

    if (!pb_decode(stream, MyMessage1_fields, &msg))
        return false;

    printf("Got MyMessage1: intvalue = %d\n", (int)msg.intvalue);
    return true;
}

bool handle_MyMessage2(pb_istream_t *stream)
{
    MyMessage2 msg = MyMessage2_init_default;

    if (!pb_decode(stream, MyMessage2_fields, &msg))
        return false;

    printf("Got MyMessage2: intvalue = %d, strvalue = %s\n",
           (int)msg.intvalue, msg.strvalue);
    return true;
}

bool handle_MyMessage3(pb_istream_t *stream)
{
    MyMessage3 msg = MyMessage3_init_default;

    if (!pb_decode(stream, MyMessage3_fields, &msg))
        return false;

    printf("Got MyMessage3: boolvalue = %d\n", (int)msg.boolvalue);
    return true;
}

int main(int argc, char **argv)
{
    uint8_t buffer[128];
    pb_istream_t stream;
    size_t count;
    bool status = false;
    int prefix;

    /* Read the data into buffer */
    SET_BINARY_MODE(stdin);
    count = fread(buffer, 1, sizeof(buffer), stdin);

    if (!feof(stdin))
    {
        printf("Message does not fit in buffer\n");
        return 1;
    }

    stream = pb_istream_from_buffer(buffer, count);

    if (!read_prefix(&stream, &prefix))
    {
        printf("Failed to read prefix: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    /* Call message handler based on prefix.
     * We could write the switch cases manually, comparing against
     * the MyMessageX_msgid defines. However, this uses the automatically
     * generated X-macro construct to make the switch case.
     */

    switch (prefix)
    {
        #define PB_MSG(id,len,name) case id: status = handle_ ## name(&stream); break;
        MSGID_EXAMPLE_MESSAGES
        #undef PB_MSG

        default: printf("Unknown prefix: %d\n", prefix); return 1;
    }

    if (!status)
    {
        printf("Parsing failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    } else {
        return 0;
    }
}
