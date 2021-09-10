/* Encode a message using callbacks inside oneof fields.
 * For encoding, callbacks inside oneofs require nothing special
 * so this is just normal callback usage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pb_encode.h>
#include "oneof.pb.h"
#include "test_helpers.h"

/* This is a nanopb-0.4 style global callback, that is referred by function name
 * and does not have to be bound separately to the message. It also allows defining
 * a custom data type for the field in the structure.
 */
bool SubMsg3_callback(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_t *field)
{
    if (ostream && field->tag == SubMsg3_strvalue_tag)
    {
         /* Our custom data type is char* */
        const char *str = *(const char**)field->pData;

        if (!pb_encode_tag_for_field(ostream, field))
            return false;

        return pb_encode_string(ostream, (const uint8_t*)str, strlen(str));
    }

    return true;
}

/* The two callbacks below are traditional callbacks that use function pointers
 * defined in pb_callback_t.
 */
bool encode_int32_array(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    int i;
    for (i = 0; i < 15; i++)
    {
        if (!pb_encode_tag_for_field(stream, field))
            return false;

        if (!pb_encode_varint(stream, i))
            return false;
    }
    return true;
}

bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    const char *str = "mystring";

    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (const uint8_t*)str, strlen(str));
}

int main(int argc, char **argv)
{
    uint8_t buffer[256];
    OneOfMessage msg = OneOfMessage_init_zero;
    pb_ostream_t stream;
    int option;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: encode_oneof [number]\n");
        return 1;
    }
    option = atoi(argv[1]);

    /* Prefix and suffix are used to test that the union does not disturb
     * other fields in the same message. */
    msg.prefix = 123;

    /* We encode one of the 'values' fields based on command line argument */
    if (option == 1)
    {
        msg.which_values = OneOfMessage_intvalue_tag;
        msg.values.intvalue = 999;
    }
    else if (option == 2)
    {
        msg.which_values = OneOfMessage_strvalue_tag;
        strcpy(msg.values.strvalue, "abcd");
    }
    else if (option == 3)
    {
        msg.which_values = OneOfMessage_submsg1_tag;
        msg.values.submsg1.array.funcs.encode = encode_int32_array;
    }
    else if (option == 4)
    {
        msg.which_values = OneOfMessage_submsg2_tag;
        msg.values.submsg2.strvalue.funcs.encode = encode_string;
    }
    else if (option == 5)
    {
        msg.which_values = OneOfMessage_submsg3_tag;
        msg.values.submsg3.which_values = SubMsg3_intvalue_tag;
        msg.values.submsg3.values.intvalue = 1234;
    }
    else if (option == 6)
    {
        msg.which_values = OneOfMessage_submsg3_tag;
        msg.values.submsg3.which_values = SubMsg3_strvalue_tag;
        msg.values.submsg3.values.strvalue = "efgh";
    }

    msg.suffix = 321;

    stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    if (pb_encode(&stream, OneOfMessage_fields, &msg))
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
