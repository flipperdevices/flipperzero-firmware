#include "sort_by_tag.pb.h"
#include <pb_encode.h>
#include <pb_decode.h>
#include "unittests.h"

int main()
{
    int status = 0;
    size_t msglen;
    pb_byte_t buf[256];
    
    {
        pb_ostream_t ostream = pb_ostream_from_buffer(buf, sizeof(buf));
        Unsorted msg = Unsorted_init_zero;
        COMMENT("Test encoding with unsorted structure");

        TEST(&msg.first < &msg.oneof.second);
        TEST(&msg.oneof.second < &msg.last);

        msg.first = 101;
        msg.which_oneof = Unsorted_second_tag;
        msg.oneof.second = 102;
        msg.last = 103;

        if (!pb_encode(&ostream, Unsorted_fields, &msg))
        {
            fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&ostream));
            return 1;
        }

        msglen = ostream.bytes_written;
    }

    {
        pb_istream_t istream = pb_istream_from_buffer(buf, msglen);
        Sorted msg = Sorted_init_zero;
        COMMENT("Test decoding with sorted structure");

        if (!pb_decode(&istream, Sorted_fields, &msg))
        {
            fprintf(stderr, "Decoding failed: %s\n", PB_GET_ERROR(&istream));
            return 2;
        }

        TEST(msg.first == 101);
        TEST(msg.which_oneof == Sorted_second_tag);
        TEST(msg.oneof.second == 102);
        TEST(msg.last == 103);

        TEST(&msg.first > &msg.oneof.second);
        TEST(&msg.oneof.second > &msg.last);
    }

    return status;
}