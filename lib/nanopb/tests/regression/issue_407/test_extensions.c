#include <pb_encode.h>
#include <pb_decode.h>
#include "extensions.pb.h"
#include "unittests.h"

int main()
{
    uint8_t buffer[256];
    int status = 0;
    size_t msglen;
    
    {
        pb_ostream_t stream;
        SimpleMessage msg = SimpleMessage_init_zero;
        ExtMessage extmsg = ExtMessage_init_zero;
        pb_extension_t ext = pb_extension_init_zero;
        
        COMMENT("Encoding message");
        msg.has_number = true;
        msg.number = 1234;
        extmsg.has_second_number = true;
        extmsg.second_number = 5678;
        
        msg.extensions = &ext;
        ext.type = &ExtMessage_ext_message_ext;
        ext.dest = &extmsg;
        ext.next = NULL;
        
        stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        TEST(pb_encode(&stream, SimpleMessage_fields, &msg));
        msglen = stream.bytes_written;
        TEST(msglen == 9); /* 3 for number, 3 for submsg tag+len, 3 for second_number */
        TEST(buffer[0] == 0x08); /* varint 1 */
        TEST(buffer[1] == 0xD2); /* number = 1234 */
        TEST(buffer[2] == 0x09);
        TEST(buffer[3] == 0xE2); /* submsg 1100 */
        TEST(buffer[4] == 0x44);
        TEST(buffer[5] == 0x03); /* length 3 */
        TEST(buffer[6] == 0x08); /* varint 1 */
        TEST(buffer[7] == 0xae); /* second_number = 5678 */
        TEST(buffer[8] == 0x2c);
    }

    {
        pb_istream_t stream;
        SimpleMessage msg = SimpleMessage_init_zero;
        ExtMessage extmsg = ExtMessage_init_zero;
        pb_extension_t ext = pb_extension_init_zero;
        
        COMMENT("Decoding message");
        
        msg.extensions = &ext;
        ext.type = &ExtMessage_ext_message_ext;
        ext.dest = &extmsg;
        ext.next = NULL;
        ext.found = false;
        
        stream = pb_istream_from_buffer(buffer, msglen);
        TEST(pb_decode(&stream, SimpleMessage_fields, &msg));
        TEST(msg.has_number);
        TEST(msg.number == 1234);
        TEST(ext.found);
        TEST(extmsg.has_second_number);
        TEST(extmsg.second_number == 5678);
    }
    
    return status;
}
