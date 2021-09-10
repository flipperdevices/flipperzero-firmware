/* Decode a message using callbacks inside oneof fields */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_decode.h>
#include <assert.h>
#include "oneof.pb.h"
#include "test_helpers.h"
#include "unittests.h"

/* This is a nanopb-0.4 style global callback, that is referred by function name
 * and does not have to be bound separately to the message. It also allows defining
 * a custom data type for the field in the structure.
 */
bool SubMsg3_callback(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_t *field)
{
    if (istream && field->tag == SubMsg3_strvalue_tag)
    {
        /* We could e.g. malloc some memory and assign it to our custom datatype
         * in the message structure here, accessible by field->pData. But in
         * this example we just print the string directly.
         */

        uint8_t buffer[64];
        int strlen = istream->bytes_left;

        if (strlen > sizeof(buffer) - 1)
            return false;

        buffer[strlen] = '\0';

        if (!pb_read(istream, buffer, strlen))
            return false;

        printf("  strvalue: \"%s\"\n", buffer);
    }

    return true;
}

/* The two callbacks below are traditional callbacks that use function pointers
 * defined in pb_callback_t.
 */
bool print_int32(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;

    printf((char*)*arg, (int)value);
    return true;
}

bool print_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint8_t buffer[64];
    int strlen = stream->bytes_left;

    if (strlen > sizeof(buffer) - 1)
        return false;

    buffer[strlen] = '\0';

    if (!pb_read(stream, buffer, strlen))
        return false;

    /* Print the string, in format comparable with protoc --decode.
     * Format comes from the arg defined in main().
     */
    printf((char*)*arg, buffer);
    return true;
}

/* The callback below is a message-level callback which is called before each
 * submessage is encoded. It is used to set the pb_callback_t callbacks inside
 * the submessage. The reason we need this is that different submessages share
 * storage inside oneof union, and before we know the message type we can't set
 * the callbacks without overwriting each other.
 */
bool msg_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    /* Print the prefix field before the submessages.
     * This also demonstrates how to access the top level message fields
     * from callbacks.
     */
    OneOfMessage *topmsg = field->message;
    printf("prefix: %d\n", (int)topmsg->prefix);

    if (field->tag == OneOfMessage_submsg1_tag)
    {
        SubMsg1 *msg = field->pData;
        printf("submsg1 {\n");
        msg->array.funcs.decode = print_int32;
        msg->array.arg = "  array: %d\n";
    }
    else if (field->tag == OneOfMessage_submsg2_tag)
    {
        SubMsg2 *msg = field->pData;
        printf("submsg2 {\n");
        msg->strvalue.funcs.decode = print_string;
        msg->strvalue.arg = "  strvalue: \"%s\"\n";
    }
    else if (field->tag == OneOfMessage_submsg3_tag)
    {
        /* Because SubMsg3 callback is bound by function name, we do not
         * need to initialize anything here. But we just print a string
         * to get protoc-equivalent formatted output from the testcase.
         */
        printf("submsg3 {\n");
    }

    /* Once we return true, pb_dec_submessage() will go on to decode the
     * submessage contents. But if we want, we can also decode it ourselves
     * above and leave stream->bytes_left at 0 value, inhibiting automatic
     * decoding.
     */
    return true;
}

int main(int argc, char **argv)
{
    uint8_t buffer[256];
    OneOfMessage msg = OneOfMessage_init_zero;
    pb_istream_t stream;
    size_t count;

    SET_BINARY_MODE(stdin);
    count = fread(buffer, 1, sizeof(buffer), stdin);

    if (!feof(stdin))
    {
        fprintf(stderr, "Message does not fit in buffer\n");
        return 1;
    }

    /* Set up the cb_values callback, which will in turn set up the callbacks
     * for each oneof field once the field tag is known. */
    msg.cb_values.funcs.decode = msg_callback;
    stream = pb_istream_from_buffer(buffer, count);
    if (!pb_decode(&stream, OneOfMessage_fields, &msg))
    {
        fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    /* This is just printing for the test case logic */
    if (msg.which_values == OneOfMessage_intvalue_tag)
    {
        printf("prefix: %d\n", (int)msg.prefix);
        printf("intvalue: %d\n", (int)msg.values.intvalue);
    }
    else if (msg.which_values == OneOfMessage_strvalue_tag)
    {
        printf("prefix: %d\n", (int)msg.prefix);
        printf("strvalue: \"%s\"\n", msg.values.strvalue);
    }
    else if (msg.which_values == OneOfMessage_submsg3_tag &&
             msg.values.submsg3.which_values == SubMsg3_intvalue_tag)
    {
        printf("  intvalue: %d\n", (int)msg.values.submsg3.values.intvalue);
        printf("}\n");
    }
    else
    {
        printf("}\n");
    }
    printf("suffix: %d\n", (int)msg.suffix);

    assert(msg.prefix == 123);
    assert(msg.suffix == 321);

    return 0;
}
