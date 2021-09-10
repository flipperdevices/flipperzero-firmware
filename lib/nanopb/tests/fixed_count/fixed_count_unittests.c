#include <stdio.h>
#include <string.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include "unittests.h"
#include "fixed_count.pb.h"

int main()
{
    int status = 0;

    {
      pb_byte_t buffer[Message1_size];
      Message1 msg_a = Message1_init_zero;
      Message1 msg_b = Message1_init_zero;

      pb_ostream_t ostream;
      pb_istream_t istream;
      size_t message_length;

      msg_a.data[0] = 1;
      msg_a.data[0] = 2;
      msg_a.data[0] = 3;

      COMMENT("Test encode and decode with three entries");

      ostream = pb_ostream_from_buffer(buffer, Message1_size);
      TEST(pb_encode(&ostream, Message1_fields, &msg_a));
      message_length = ostream.bytes_written;

      istream = pb_istream_from_buffer(buffer, message_length);
      TEST(pb_decode(&istream, Message1_fields, &msg_b));

      TEST(istream.bytes_left == 0);
      TEST(memcmp(&msg_b, &msg_a, sizeof(msg_a)) == 0);
    }

    {
      pb_byte_t input[] = {0x08, 0x00, 0x08, 0x01, 0x08, 0x02, 0x08, 0x03};
      pb_istream_t stream;
      Message1 msg = Message1_init_zero;

      COMMENT("Test wrong number of entries");

      stream = pb_istream_from_buffer(input, 6);
      TEST(pb_decode(&stream, Message1_fields, &msg));
      TEST(msg.data[0] == 0 && msg.data[1] == 1 && msg.data[2] == 2);

      stream = pb_istream_from_buffer(input, 8);
      TEST(!pb_decode(&stream, Message1_fields, &msg));
      TEST(strcmp(stream.errmsg, "array overflow") == 0);

      stream = pb_istream_from_buffer(input, 4);
      TEST(!pb_decode(&stream, Message1_fields, &msg));
      TEST(strcmp(stream.errmsg, "wrong size for fixed count field") == 0);
    }

    {
      pb_byte_t buffer[Message2_size];
      Message2 msg_a = Message2_init_zero;
      Message2 msg_b = Message2_init_zero;

      pb_ostream_t ostream;
      pb_istream_t istream;
      size_t message_length;

      COMMENT("Test encode and decode with nested messages");

      msg_a.data[0].data[0] = 1;
      msg_a.data[0].data[1] = 2;
      msg_a.data[0].data[2] = 3;
      msg_a.data[1].data[0] = 4;
      msg_a.data[1].data[1] = 5;
      msg_a.data[1].data[2] = 6;

      ostream = pb_ostream_from_buffer(buffer, Message2_size);
      TEST(pb_encode(&ostream, Message2_fields, &msg_a));
      message_length = ostream.bytes_written;

      istream = pb_istream_from_buffer(buffer, message_length);
      TEST(pb_decode(&istream, Message2_fields, &msg_b));

      TEST(istream.bytes_left == 0);
      TEST(memcmp(&msg_b, &msg_a, sizeof(msg_a)) == 0);
    }

    {
      pb_byte_t buffer[Message3_size];
      Message3 msg_a = Message3_init_zero;
      Message3 msg_b = Message3_init_zero;

      pb_ostream_t ostream;
      pb_istream_t istream;
      size_t message_length;

      COMMENT("Test encode and decode with two nested messages");

      msg_a.data1[0].data[0].data[0] = 1;
      msg_a.data1[0].data[0].data[1] = 2;
      msg_a.data1[0].data[0].data[2] = 3;
      msg_a.data1[0].data[1].data[0] = 4;
      msg_a.data1[0].data[1].data[1] = 5;
      msg_a.data1[0].data[1].data[2] = 6;

      msg_a.data1[1].data[0].data[0] = 7;
      msg_a.data1[1].data[0].data[1] = 8;
      msg_a.data1[1].data[0].data[2] = 9;
      msg_a.data1[1].data[1].data[0] = 10;
      msg_a.data1[1].data[1].data[1] = 11;
      msg_a.data1[1].data[1].data[2] = 12;

      msg_a.data2[0].data[0].data[0] = 11;
      msg_a.data2[0].data[0].data[1] = 12;
      msg_a.data2[0].data[0].data[2] = 13;
      msg_a.data2[0].data[1].data[0] = 14;
      msg_a.data2[0].data[1].data[1] = 15;
      msg_a.data2[0].data[1].data[2] = 16;

      msg_a.data2[1].data[0].data[0] = 17;
      msg_a.data2[1].data[0].data[1] = 18;
      msg_a.data2[1].data[0].data[2] = 19;
      msg_a.data2[1].data[1].data[0] = 110;
      msg_a.data2[1].data[1].data[1] = 111;
      msg_a.data2[1].data[1].data[2] = 112;

      ostream = pb_ostream_from_buffer(buffer, Message3_size);
      TEST(pb_encode(&ostream, Message3_fields, &msg_a));
      message_length = ostream.bytes_written;

      istream = pb_istream_from_buffer(buffer, message_length);
      TEST(pb_decode(&istream, Message3_fields, &msg_b));

      TEST(istream.bytes_left == 0);
      TEST(memcmp(&msg_b, &msg_a, sizeof(msg_a)) == 0);
    }

    if (status != 0)
        fprintf(stdout, "\n\nSome tests FAILED!\n");

    return status;
}
